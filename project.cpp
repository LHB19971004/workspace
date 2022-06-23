#include "project.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "rabbitmq_reader.h"
#include "logging.h"
#include "packet_parser.h"
#include "format_helper.h"
#include "utils.h"
#include "funs.h"
#include "jt_frame.h"
#include "msg_service.h"
#include "msg_def.h"
#include "jt_send.h"
#include "tcp_connection.h"
#include "dev_data_mgr.h"
#include "jt_body.h"
#include "project_monitor.h"
#include "msg_proc.h"

Project::Project(boost::asio::io_service& io_service, ServerConfig::ProjectConfig const &config)
	: io_service_(io_service)
	, config_(config)
	, formater_(config_)
{

}

Project::~Project()
{
	if (msg_service_)
		msg_service_->stop();
	if (tcp_server_)
		tcp_server_->stop();
	if (slave_link_)
		slave_link_->stop();
	if (dev_data_mgr_)
		dev_data_mgr_->stop();

	ProjectMonitor::instance().unregistProject(config_.project_id);
}

bool Project::run()
{
	weak_ptr<Project> weak = shared_from_this();
	if (config_.http_enable)
	{
		dev_data_mgr_ = std::make_shared<DevDataMgr>(io_service_, config_, weak);
		dev_data_mgr_->run();
	}

	msg_service_ = std::make_shared<MsgService>(io_service_, config_, weak);
	msg_service_->start();

	msg_proc_ = std::make_shared<MsgProc>(config_, weak);
	main_link_ = std::make_shared<TcpClient>(io_service_,
		config_.main_link.host, config_.main_link.port, config_, weak);
	main_link_->run();

	tcp_server_ = boost::make_unique<TcpServer>(io_service_,
		config_.slave_link.host, config_.slave_link.port, config_, weak);
	tcp_server_->run();

	ProjectMonitor::instance().registProject(config_.project_id, weak);
	return 1;
}

int32_t Project::sendMsg(const Jt809BodySend* packet_body, bool main_link/* = true*/)
{
	if (!login_ && packet_body->msg_type_ != UP_CONNECT_REQ)
	{
		return 0;
	}

	boost::scoped_array<char> buffer;
	int32_t buffer_len = 0;
	if (!formater_.format(packet_body, buffer, buffer_len))
	{
		return 0;
	}
	if (main_link)
		return main_link_ && main_link_->sendData(buffer.get(), buffer_len);
	else
		return slave_link_ && slave_link_->sendData(buffer.get(), buffer_len);
}

int32_t Project::sendMsg(uint16_t msg_type, const char* body, uint32_t body_len, bool main_link/* = true*/)
{
	if (!login_ && msg_type != UP_CONNECT_REQ)
	{
		return 0;
	}

	boost::scoped_array<char> buffer;
	int32_t buffer_len = 0;
	if (!formater_.format(msg_type, body, body_len, buffer, buffer_len))
	{
		return 0;
	}
	if (main_link)
		return main_link_ && main_link_->sendData(buffer.get(), buffer_len);
	else
		return slave_link_ && slave_link_->sendData(buffer.get(), buffer_len);
}


int32_t Project::sendMsg(const JtPos& pos, string& raw_data)
{
	if (!login_ || !main_link_)
	{
		return 0;
	}

	boost::scoped_array<char> buffer;
	int32_t buffer_len = 0;
	auto res = pos.toBinary(config_.project_version);
	if (!formater_.format(pos.msg_type_, res.data(), res.size(), buffer, buffer_len))
	{
		return 0;
	}

	main_link_->sendData(buffer.get(), buffer_len);
	raw_data = MemoryToText(buffer.get(), buffer_len);
	return 1;
}

void Project::mainNotify(int32_t tcp_state)
{
	switch (tcp_state)
	{
	case E_TCP_CONNECT:
	{
		LOG_INFO << "Project " << config_.project_name << " 登录";
		JtLoginReq login{ config_.user_name , config_.user_password, config_.slave_link.ip, config_.slave_link.port,config_.access_code };
		sendMsg(&login);
	}
	break;
	case E_TCP_DISCONNECT:
	{
		LOG_INFO << "Project " << config_.project_name << " 主链路已断开";
		msg_service_->stopConsume();
		login_ = false;
	}
	break;
	default:
		break;
	}
}

void Project::onFrame(Jt809Frame& frame, const char* buff, int32_t buff_len)
{
	if (frame.msg_id == UP_CONNECT_RSP)
	{
		const Jt809RegisterResponse *res = dynamic_cast<const Jt809RegisterResponse *>(frame.body.get());
		if (res && res->result == 0)
		{
			LOG_INFO << "Project " << config_.project_name << " main link login success";
			login_ = true;
			msg_service_->startConsume();
			//2021.11.29   当从未登录变成登录状态时，发送注册信息
			auto reg_list = dev_data_mgr_->GetDevRegiterInfo();
			for (auto reg : reg_list)
			{
				sendMsg(&reg);
			}
		}
	}
	msg_proc_->proc(frame);
}

void Project::slaveNotify(int32_t tcp_state, shared_ptr<TcpConnection> connection)
{
	switch (tcp_state)
	{
	case E_TCP_CONNECT:
	{
		LOG_INFO << "Project " << config_.project_name << " 从链路已连接";
		slave_link_.swap(connection);
	}
	break;
	case E_TCP_DISCONNECT:
	{
		if (connection == slave_link_)
		{
			slave_link_.reset();
			LOG_INFO << "Project " << config_.project_name << " 从链路已断开";
		}
	}
	}
}

void Project::onSlaveFrame(Jt809Frame& frame, const char* buff, int32_t buff_len)
{
	msg_proc_->proc(frame);
}

int32_t Project::getDevInfo(int32_t vehicle_id, string& vehicle_no, uint8_t& vehicle_color)
{
	if (dev_data_mgr_)
		return dev_data_mgr_->getDevInfo(vehicle_id, vehicle_no, vehicle_color);
	return 0;
}

void Project::getStatus(uint32_t& up_link, uint32_t& down_link, uint32_t& packet_rate)
{
	up_link = login_ ? 1 : 2;
	down_link = slave_link_ ? 1 : 2;
	if (msg_service_)
	{
		packet_rate = msg_service_->getPacketRate();
	}
	else
	{
		packet_rate = 0;
	}
}

void Project::mainData(const char* buff, int32_t buff_len)
{
	Jt809Frame frame;
	if (!formater_.parse(buff, buff_len, frame))
	{
		return;
	}
	onFrame(frame, buff, buff_len);
}

void Project::slaveData(const char* buff, int32_t buff_len)
{
	Jt809Frame frame;
	if (!formater_.parse(buff, buff_len, frame))
	{
		return;
	}
	onFrame(frame, buff, buff_len);
}