#include "dev_data_mgr.h"
#include "async_http_client.h"
#include "json/json.h"
#include "utils.h"
#include "logging.h"
#include <boost/filesystem.hpp>
#include "jt_send.h"


DevDataMgr::DevDataMgr(boost::asio::io_service& io_service, const ServerConfig::ProjectConfig& config,std::weak_ptr<Project> project):
	project_(project)
    ,io_service_(io_service)
	, strand_(io_service_)
	, timer_(io_service_)
	, config_info_(config)
{
	config_ = Config::instance().getServerConfig().http_plat;
	file_name_ = getFileName();
	load();
}


DevDataMgr::~DevDataMgr()
{
	save();
}

void DevDataMgr::run()
{
	onHeartBeat();
}

void DevDataMgr::stop()
{
	exit_ = true;
	timer_.cancel();
}

void DevDataMgr::onHeartBeat()
{
	if (exit_)
	{
		return;
	}

	save();
	sendQueryTotal();
	if (!reg_list_.empty())
	{
		auto project = project_.lock();
		if (project->isLogin())
		{
			for (auto &reg : reg_list_)
			{
				project->sendMsg(&reg);
			}
			reg_list_.clear();
		}
	}

	auto self = shared_from_this();
	timer_.expires_from_now(std::chrono::seconds(60));	//60秒获取一次
	timer_.async_wait(
		strand_.wrap([this, self](boost::system::error_code ec) {
		if (!ec)
		{
			onHeartBeat();
		}
	}));
}

void DevDataMgr::sendQueryTotal()
{
	auto http = std::make_shared<AsyncHTTPClient>(io_service_);
	auto callback = std::bind(&DevDataMgr::httpCallback, shared_from_this(), std::placeholders::_1);

	Json::Value json;
	json["startTime"] = Utils::timeToJsonTime(timestamp_);
	json["endTime"] = Utils::timeToJsonTime(time(0));
	json["projectId"] = config_info_.project_id;

	Json::FastWriter writer;
	http->run(AsyncHTTPClient::MT_POST, config_.base_url, config_.port, config_.page_url, writer.write(json), callback);
}

void DevDataMgr::sendQueryList(uint32_t count)
{
	auto callback = std::bind(&DevDataMgr::httpCallback, shared_from_this(), std::placeholders::_1);
	Json::Value json;
	Json::FastWriter writer;
	json["startTime"] = Utils::timeToJsonTime(timestamp_);
	json["endTime"] = Utils::timeToJsonTime(time(0));
	json["projectId"] = config_info_.project_id;

	for (uint32_t i = 0; i * 100 < count; ++i)
	{
		auto http = std::make_shared<AsyncHTTPClient>(io_service_);
		json["startIndex"] = i * 100;
		http->run(AsyncHTTPClient::MT_POST, config_.base_url, config_.port, config_.alldata_url, writer.write(json), callback);
	}
}

void DevDataMgr::httpCallback(const string& response)
{
	auto gbk_str = Utils::Utf8ToGbk(response);

	Json::Reader reader;
	Json::Value js_info;
	if (!reader.parse(gbk_str, js_info))
	{
		LOG_ERROR << "httpCallback parse json failed: " << gbk_str;
		return;
	}

	if (js_info["code"].isNumeric() && js_info["code"].asInt() == 200)
	{
		auto& result = js_info["result"];
		if (result.isNumeric())
		{
			sendQueryList(result.asInt());
		}
		else if(result.isArray())
		{
			for (auto& dev : result)
			{
				if (!dev["dataTerminalId"].isNumeric()
					|| !dev["modifyTime"].isNumeric()
					|| !dev["dockVehicleColor"].isNumeric())
				{
					continue;
				}
				DevParam param;
				param.dataTerminalId = dev["dataTerminalId"].asInt();
				param.vehicle_no = dev["dockVehicleNo"].asString();
				param.vehicle_type = dev["dockVehicleType"].asInt();
				param.vehicle_color = dev["dockVehicleColor"].asInt();
				param.wid = dev["dockCommids"].asString();
				if (param.wid.empty())
				{
					param.wid = dev["commids"].asString();
				}
				//运输行业编码
				param.trans_type = dev["transType"].asString();
				//车籍地
				param.vehicle_nationality = dev["vehicleNationality"].asString();
				//业户ID
				param.owers_id = dev["corpId"].asInt();
				//业户名称
				param.owers_name = dev["dockCorpName"].asString();
				//业户联系电话
				param.owers_tel = dev["owersTel"].asString();
				//车载终端厂商唯一编码
				param.producer_id = dev["producerId"].asString();
				//车载终端型号
				param.terminal_model_type = dev["terminalModelType"].asString();
				int32_t is_valid = dev["isValid"].asInt();
				time_t modify_time = dev["modifyTime"].asDouble() / 1000 + 1;
				if (timestamp_ < modify_time)
				{
					timestamp_ = modify_time;
				}
				if (is_valid)
				{
					auto reg = fomartRegister(param);
					reg_list_.push_back(reg);
				}
				updateDevInfo(param, is_valid);
			}
		}
	}
	else
	{
		LOG_ERROR << "httpCallback request failed: " << gbk_str;
		return;
	}
}

int32_t DevDataMgr::updateDevInfo(DevParam& param, int32_t is_valid)
{
	std::lock_guard<std::mutex> _(mutex_);
	if (is_valid)
	{
		dev_map_[param.dataTerminalId] = param;
	}
	else
	{
		dev_map_.erase(param.dataTerminalId);
	}
	return 1;
}

int32_t DevDataMgr::getDevInfo(int32_t vehicle_id, DevParam& param)
{
	std::lock_guard<std::mutex> _(mutex_);
	auto it = dev_map_.find(vehicle_id);
	if (it != dev_map_.end())
	{
		param = it->second;
		return 1;
	}
	return 0;
}

std::list<JtRegister> DevDataMgr::GetDevRegiterInfo()
{
	std::list<JtRegister> reg_list;
	std::lock_guard<std::mutex> _(mutex_);
	for (auto param : dev_map_)
	{
		reg_list.emplace_back(fomartRegister(param.second));
	}
	
	return reg_list;
}



int32_t DevDataMgr::getDevInfo(int32_t vehicle_id, string& vehicle_no, uint8_t& vehicle_color)
{
	std::lock_guard<std::mutex> _(mutex_);
	auto it = dev_map_.find(vehicle_id);
	if (it != dev_map_.end())
	{
		if (!it->second.vehicle_no.empty())
		{
			vehicle_no = it->second.vehicle_no;
		}
		vehicle_color = it->second.vehicle_color;
		return 1;
	}
	return 0;
}

void DevDataMgr::save()
{
	ofstream file_objid(file_name_, std::ios::binary);

	if (!file_objid.fail())
	{
		decltype(dev_map_) tmp_map;
		{
			std::lock_guard<std::mutex> _(mutex_);
			tmp_map = dev_map_;
		}

		try
		{
			boost::archive::binary_oarchive bin_oar(file_objid);
			bin_oar << tmp_map;
		}
		catch (boost::archive::archive_exception &)
		{
			file_objid.close();
			return;
		}
		file_objid.close();
	}
}

void DevDataMgr::load()
{
	ifstream file_objid(file_name_, std::ios::binary);
	if (!file_objid.fail())
	{
		try
		{
			boost::archive::binary_iarchive bin_iar(file_objid);
			bin_iar >> dev_map_;
		}
		catch (boost::archive::archive_exception &)
		{
			//文件读取出错时处理 1.删除文件
			//删除文件前 先关闭文件
			file_objid.close();
			boost::filesystem::remove(file_name_);
			return;
		}
		catch (...)
		{
			file_objid.close();
			//LogLocalText(LOG_LOCAL_ERROR_ERROR, "DevParamMgr::load() fail");
			//printf_s("DevParamMgr::load() fail  ");
			assert(0);
			return;
		}

		file_objid.close();
	}
	else
	{
		boost::filesystem::remove(file_name_);
	}
}

string DevDataMgr::getFileName()
{
	char tmp[128];
	sprintf(tmp, "app_data\\dev_param_%d.dat", config_info_.project_id);
	return tmp;
}

JtRegister DevDataMgr::fomartRegister(const DevParam& param)
{
	JtRegister ack;
	ack.vehicle_no = param.vehicle_no;
	ack.vehicle_color = param.vehicle_color;
	ack.platform_id = config_info_.plat_id;
	if (param.producer_id.empty())
	{
		ack.producer_id = config_info_.produce_id;
	}
	else
	{
		ack.producer_id = param.producer_id;
	}
	if (param.terminal_model_type.empty())
	{
		ack.terminal_model_type = config_info_.terminal_type;
	}
	else
	{
		ack.terminal_model_type = param.terminal_model_type;
	}
	//车载终端通讯模块IMEI码 //待定
	ack.terminal_id = param.wid;
	ack.terminal_sim_code = param.owers_tel;
	return ack;
}