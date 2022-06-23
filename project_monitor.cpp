#include "project_monitor.h"
#include "project.h"
#include "config.h"
#include <json/json.h>
#include "logging.h"
#include "utils.h"

ProjectMonitor& ProjectMonitor::instance()
{
	static ProjectMonitor _;
	return _;
}

void ProjectMonitor::init()
{
	auto config = Config::instance().getServerConfig().monitor;
	if (config.cmd_consumer.enable)
	{
		auto mq_call_back = std::bind(&ProjectMonitor::cmdReaderCbk, this, placeholders::_1, placeholders::_2);
		RabbitMQReader::Config mq_config{ config.cmd_consumer.address,
							config.cmd_consumer.port,
							config.cmd_consumer.user,
							config.cmd_consumer.password,
							config.cmd_consumer.vhost,
							config.cmd_consumer.exchange,
							config.cmd_consumer.queue,
							config.cmd_consumer.routing_key,
							config.cmd_consumer.zip,
							true };
		cmd_reader_ = boost::make_unique<RabbitMQReader>(mq_config, mq_call_back);
		cmd_reader_->unlock();
	}
	if (config.cmd_producer.enable)
	{
		cmd_writer_ = boost::make_unique<RabbitMQWriter>(RabbitMQWriter::Config{
			config.cmd_producer.address,
			config.cmd_producer.port,
			config.cmd_producer.user,
			config.cmd_producer.password,
			config.cmd_producer.vhost,
			config.cmd_producer.exchange,
			config.cmd_producer.routing_key,
			config.cmd_producer.zip });
	}
	if (config.data_producer.enable)
	{
		data_writer_ = boost::make_unique<RabbitMQWriter>(RabbitMQWriter::Config{
			config.data_producer.address,
			config.data_producer.port,
			config.data_producer.user,
			config.data_producer.password,
			config.data_producer.vhost,
			config.data_producer.exchange,
			config.data_producer.routing_key,
			config.data_producer.zip
			});
	}
}

void ProjectMonitor::registerProject(uint32_t project_id, std::weak_ptr<Project> project)
{
	std::lock_guard<std::mutex> _(mutex_);
	map_[project_id] = project;
}

void ProjectMonitor::unregisterProject(uint32_t project_id)
{
	std::lock_guard<mutex> _(mutex_);
	map_.erase(project_id);

}

void ProjectMonitor::cmdReaderCbk(const string& queue, string& message)
{
	if (!cmd_writer_)
	{
		return;
	}
	Json::Value js_info;
	Json::Reader read;
	if (!read.parse(message, js_info))
	{
		LOG_ERROR << __FUNCTION__ << "parse error: " << message.c_str();
	}
	if (js_info.isArray())
	{
		for (auto& obj : js_info)
		{
			cmdParse(obj);
		}
	}
	else
	{
		cmdParse(js_info);
	}
}

void ProjectMonitor::cmdParse(Json::Value& json)
{
	if (!json.isObject() || json.isMember("Directive"))
	{
		return;
	}
	auto& cmd = json["Directive"];
	if (cmd["ProjectID"].isIntegral() && cmd["ProjectID"].asInt())
	{
		auto project_id = cmd["ProjectID"].asInt();
		auto project = getProjectPtr(project_id);
		if (project)
		{
			uint32_t up_link, down_link, pakcet_rate;
			project->getStatus(up_link, down_link, pakcet_rate);
			Json::Value result;
			result["UPLinkStatus"] = up_link;
			result["DownLinkStatus"] = down_link;
			result["MidLinkStatus"] = 1;//没有中间层
			result["PacketRate"] = pakcet_rate;
			json["Result"] = result;
			json["Staus"] = "sucess";
			json["TimeStamp"] = Utils::timeToJsonTime(time(0));
			Json::Value response;
			Json::Value js_array;
			response["DirectiveH"] = json;
			js_array.append(response);
			Json::FastWriter writer;
			auto strJson = writer.write(js_array);
			if (!strJson.empty() && strJson.back() == '\n')
			{
				strJson.pop_back();
			}
			LOG_INFO << "send project data:" << strJson.c_str();
			cmd_writer_->produce(strJson);
		}
	}
}

std::shared_ptr<Project> ProjectMonitor::getProjectPtr(uint32_t project_id)
{
	std::lock_guard<mutex> _(mutex_);
	auto iter = map_.find(project_id);
	if (iter != map_.end())
	{
		auto ptr = iter->second.lock();
		if (!ptr)
		{
			map_.erase(iter);
		}
		return ptr;
	}
	return nullptr;
}

void ProjectMonitor::sendData(const string& data)
{
	if (data_writer_)
	{
		data_writer_->produce(data);
	}
}

