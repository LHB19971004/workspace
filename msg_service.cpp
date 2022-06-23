#include "msg_service.h"
#include "rabbitmq_reader.h"
#include "project.h"
#include "msg_def.h"
#include "logging.h"
#include "jt_send.h"
#include "dev_data_mgr.h"
#include "project_monitor.h"
#include "utils.h"
#include "kafka_reader.h"
#include "odbc_conn.h"
MsgService::MsgService(boost::asio::io_service& io_service, const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project) :
	project_(project)
	, config_(config)
	, io_service_(io_service)
	, strand_(io_service_)
	, timer_(io_service_)
{
	
}

MsgService::~MsgService()
{

}

void MsgService::start()
{
	if (config_.mq_consumer.enable)
	{
		auto mq_callback = std::bind(&MsgService::mqReaderCbk, this,
			std::placeholders::_1, std::placeholders::_2);

		RabbitMQReader::Config mq_config{ config_.mq_consumer.address,
				config_.mq_consumer.port,
				config_.mq_consumer.user,
				config_.mq_consumer.password,
				config_.mq_consumer.vhost,
				config_.mq_consumer.exchange,
				config_.mq_consumer.queue,
				config_.mq_consumer.routing_key,
				config_.mq_consumer.zip,
				false};
		mq_reader_ = boost::make_unique<RabbitMQReader>(mq_config, mq_callback);
	}
	//kafka init
	if (config_.kafka_consumer.enable)
	{
		auto kafka_callback = std::bind(&MsgService::kafkaReaderCbk, this, std::placeholders::_1, std::placeholders::_2);
		KafkaReader::Config kafka_config
		{
			config_.kafka_consumer.tag,
			config_.kafka_consumer.brokers,
			config_.kafka_consumer.topic,
			config_.kafka_consumer.group_id,
		};
		kafka_reader_ = boost::make_unique<KafkaReader>(kafka_config, kafka_callback);
	}

	onHeartBeat();
}

void MsgService::stop()
{
	boost::system::error_code ec;
	timer_.cancel(ec);
	if (mq_reader_)
	{
		mq_reader_.reset();
	}
	if (kafka_reader_)
	{
		kafka_reader_.reset();
	}
}

void MsgService::mqReaderCbk(const string& queue, string& message)
{
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(message, root))
	{
		LOG_ERROR << __FUNCTION__ << " parse error: " << message.c_str();
	}
	
	if (root.isArray())
	{
		for (auto& obj : root)
		{
			parseJson(obj);
		}
	}
	else
	{
		parseJson(root);
	}
}


void MsgService::kafkaReaderCbk(const string& queue, string& message)
{
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(Utils::Utf8ToGbk(message), root))
	{
		LOG_ERROR << __FUNCTION__ << "parse error" << message.c_str();
	}
	if (root.isArray())
	{
		for (auto& obj : root)
		{
			parseJson(obj);
		}
	}
	else
	{
		parseJson(root);
	}
}


void  MsgService::parseICData(const Json::Value& json)
{
	//发送ic卡数据
	if (!json.isMember("AiSTICCard") || !json.isMember("AiTrack"))
	{
		return;
	}
	Json::Value ai_sticcard = json["AiSTICCard"];
	if (!ai_sticcard.isMember("DriverName")
		|| !ai_sticcard.isMember("DQCN")
		|| !ai_sticcard.isMember("CIAN")
		|| !ai_sticcard.isMember("StatusType")
		|| !ai_sticcard.isMember("ReadResult")
		|| !ai_sticcard.isMember("ExpireTime"))
	{
		return;
	}
	Json::Value ai_track = json["AiTrack"];
	if(!ai_track.isMember("DataTerminalID"))
	{
		return;
	}
	
	auto dt_id = ai_track["DataTerminalID"].asInt();
	if (ai_sticcard["StatusType"].asInt() != 1 || ai_sticcard["ReadResult"].asInt() != 0)
	{
		return;
	}

	Json::FastWriter writer;
	auto ret = writer.write(json);
	if (!ret.empty() && ret.back() == '\n')
	{
		ret.pop_back();
	}
	LOG_INFO << __FUNCTION__ << ": " << ret;
	auto project = project_.lock();
	if (!project)
	{
		return;
	}
	ReportICInfo driver_info;
	driver_info.vehicle_no = ai_track["VehicleNo"].asString();
	driver_info.vehicle_color = 2;
	project->getDevInfo(dt_id, driver_info.vehicle_no, driver_info.vehicle_color);
	driver_info.driver_name = ai_sticcard["DriverName"].asString();
	driver_info.id_number = ai_sticcard["DQCN"].asString();
	driver_info.cert = driver_info.id_number;
	driver_info.department = ai_sticcard["CIAN"].asString();
	if (ai_sticcard.isMember("ExpireTime"))
	{
		string str_date = ai_sticcard["ExpireTime"].asString();
		int32_t n_pos1 = str_date.find('(', 0);
		int32_t n_pos2 = str_date.find('+', 0);
		if (string::npos != n_pos1 && string::npos != n_pos2)
		{
			str_date = str_date.substr(n_pos1 + 1, n_pos2 - n_pos1 - 1);
			driver_info.expire_time = static_cast<time_t>(strtoull(str_date.c_str(), 0, 10) / 1000);
		}
	}
	else
	{
		double gps_time_d = ai_sticcard["ExpireTime"].asDouble();
		driver_info.expire_time = static_cast<time_t>(gps_time_d / 1000);
	}

	project->sendMsg(&driver_info);
}

WarnInfoGDBY  MsgService::fomartWarnInfo(const JtPos& pos,string& wid,string& status)
{
	WarnInfoGDBY warn_info;
	warn_info.vehicle_no = pos.vehicle_no;
	warn_info.vehicle_color = pos.vehicle_color;
	if(status.find("超速报警") != string::npos)
	{
		warn_info.warn_type = 1;
	}
	if (status.find("疲劳驾驶报警") != string::npos)
	{
		warn_info.warn_type = 2;
	}
	warn_info.warn_time = pos.gps_time;//报警时间
	warn_info.event_start_time = pos.gps_time;
	warn_info.event_end_time = pos.gps_time;
	warn_info.platformId1 = config_.plat_id;

	//报警内容
	if (wid.size() > 20)
	{
		wid = wid.substr(0, 20);
	}
	else if (wid.size() < 20)
	{
		int size = 20 - wid.size();
		string ss1(size, '0');
		wid = ss1 + wid;
	}
	tm tm_time;
	_localtime64_s(&tm_time, &pos.gps_time);
	char tmp[256] = { 0 };
	sprintf_s(tmp, sizeof(tmp), "%s%04X20%02d%02d%02d%02d%02d%02d00", wid.c_str(), warn_info.warn_type,
		tm_time.tm_year - 100, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	//报警信息id(终端手机号+报警类型标志+时间+序号)
	string warn_no = tmp;
	int lat = pos.lat*1000000;
	int lon = pos.lon* 1000000;
	string driver_name = "";// 驾驶员姓名
	string diver_no = "";//驾驶员驾照号码
	string driver_id = "";//驾驶员从业资格证号
	char bas_inf[1024] = { 0 };
	sprintf(bas_inf,"WARN_NO:=%s;WARN_TYPE:=%04X;SPEED:=%d;LONGITUDE:=%d;LATITUDE:=%d;ALTITUDE:=%d;DRIVER_NAME:=%s;DRIVER_ID:=%s;LICENCE:=%s;WARN_TIME:=%llu",
		warn_no.c_str(), warn_info.warn_type, pos.speed,
		lon, lat, pos.height,driver_name.c_str(),diver_no.c_str(),driver_id.c_str(), warn_info.warn_time);
	warn_info.info_content = bas_inf;
	LOG_INFO << "send alarm content: " << bas_inf;
	return warn_info;

}


void MsgService::parseJson(Json::Value& json)
{
	parseICData(json);
	if (!json.isMember("AiTrack"))
	{
		return;
	}

	Json::Value ai_track = json["AiTrack"];
	if (ai_track["NotTrack"].asBool())
	{
		return;
	}
	if (!ai_track.isMember("Lon") || !ai_track.isMember("Lat") || !ai_track.isMember("Status") ||
		!ai_track.isMember("Speed") || !ai_track.isMember("VehicleID") ||
		!ai_track.isMember("Direction") || !ai_track.isMember("Odometer") ||
		!ai_track.isMember("ACCStatus") || !ai_track.isMember("LoStatus") ||
		/*!ai_track.isMember("RegName") ||*/ !ai_track.isMember("LineStatus") ||
		!ai_track.isMember("Height") || !ai_track.isMember("DataTerminalID"))
	{
		return;
	}

	string wid = ai_track["Wid"].asString();
	auto vehicle_id = ai_track["VehicleID"].asInt();
	JtPos pos;
	pos.vehicle_no = ai_track["VehicleNo"].asString();
	pos.vehicle_color = 2;
	time_t time_sec = 0;
	if (ai_track["GpsTime"].isString())
	{
		string str_date = ai_track["GpsTime"].asString();
		int32_t n_pos1 = str_date.find('(', 0);
		int32_t n_pos2 = str_date.find('+', 0);
		if (string::npos != n_pos1 && string::npos != n_pos2)
		{
			str_date = str_date.substr(n_pos1 + 1, n_pos2 - n_pos1 - 1);
			time_sec = static_cast<time_t>(strtoull(str_date.c_str(), 0, 10) / 1000);
		}
	}
	else
	{
		double gps_time_d = ai_track["GpsTime"].asDouble();
		time_sec = static_cast<time_t>(gps_time_d / 1000);
	}
	pos.gps_time = time_sec;
	pos.lat = ai_track["Lat"].asDouble();
	pos.lon = ai_track["Lon"].asDouble();
	pos.speed = ai_track["Speed"].asUInt();
	pos.direction = ai_track["Direction"].asUInt();
	pos.height = ai_track["Height"].asUInt();
	pos.odometer = ai_track["Odometer"].asDouble();
	auto status_str = ai_track["Status"].asString();
	pos.status = devStatusAnalysis(status_str);
	pos.alarm = devAlarmAnalysis(status_str, pos.speed);
	string custom_name = ai_track["CustomName"].asString();
	pos.vehicle_no = custom_name;
	auto  data_terminal_id = ai_track["DataTerminalID"].asInt();

	if (ai_track["ACCStatus"].asInt() == 2)
		pos.status |= 0x00000001;
	if (ai_track["LoStatus"].asInt() == 3)
		pos.status |= 0x00000002;

	auto project = project_.lock();
	if (!project)
	{
		return;
	}
	project->getDevInfo(data_terminal_id, pos.vehicle_no, pos.vehicle_color);
	string raw_data;
	if (project->sendMsg(pos, raw_data))
	{
		//推送报警数据
		if (status_str.find("超速报警") != string::npos || status_str.find("疲劳驾驶报警") != string::npos)
		{
			auto ack = fomartWarnInfo(pos, wid, status_str);
			project->sendMsg(&ack);
		}
		if (pos.alarm > 0)
		{
			//LOG_INFO << "send alarm pos: " << raw_data;
		}
	  
		packet_rate_curr_++;

		Json::Value json_bak;
		json_bak.swap(ai_track);
		json_bak["ProjectID"] = config_.project_id;
		json_bak["CarName"] = pos.vehicle_no;
		json_bak["CarColor"] = pos.vehicle_color;
		json_bak["PushTime"] = Utils::timeToJsonTime(time(0));
		json_bak["RawData"] = raw_data;
		json_bak["ProjectName"] = config_.project_name;
		Json::Value js_array;
		js_array.append(move(json_bak));
		Json::FastWriter writer;
		auto res = writer.write(js_array);
		if (!res.empty() && res.back() == '\n')
		{
			res.pop_back();
		}
		ProjectMonitor::instance().sendData(res);
		//LOG_INFO << "Project " <<  config_.project_id <<" SendPos: " << res;
	}
}

uint32_t MsgService::devStatusAnalysis(const string& str)
{
	uint32_t status = 0;
	if (str.empty())
	{
		return status;
	}
	if (str.find("ACC开") != string::npos)
	{
		status |= 0x1;
	}

	if (str.find("3D") != string::npos)
	{
		status |= 0x2;
	}

	if (str.find("停运") != string::npos)
	{
		status |= 0x10;
	}

	if (str.find("加密") != string::npos)
	{
		status |= 0x20;
	}

	if (str.find("油路断开") != string::npos)
	{
		status |= 0x200;
	}

	if (str.find("电路断开") != string::npos)
	{
		status |= 0x400;
	}

	if (str.find("车门已锁") != string::npos)
	{
		status |= 0x800;
	}

	return status;
}

uint32_t MsgService::devAlarmAnalysis(const string& alarm_str, uint16_t speed)
{
	uint32_t alarm_status = 0;
	//if(status_str.find ("劫警") != string::npos )
	//{
	//	alarm_str |= 0x1;
	//}
	if (1)
	{
		if (alarm_str.find("超速报警") != string::npos || speed > 99)
		{
			alarm_status |= 0x2;
		}
	}

	if (alarm_str.find("疲劳驾驶报警") != string::npos)
	{
		alarm_status |= 0x4;
	}
	return alarm_status;
}

void MsgService::stopConsume()
{
	if (mq_reader_)
	{
		mq_reader_->lock();
	}
	if (kafka_reader_)
	{
		kafka_reader_->lock();
	}
}

void MsgService::startConsume()
{
	if (mq_reader_)
	{
		mq_reader_->unlock();
	}
	if (kafka_reader_)
	{
		kafka_reader_->unlock();
	}
}

void MsgService::updatePacketRate()
{
	packet_rate_ = (uint32_t)packet_rate_curr_;
	packet_rate_curr_ = 0;

	LOG_INFO << "Project " << config_.project_id << " packet rate: " << packet_rate_;
}

void MsgService::queryDB()
{
	auto self = shared_from_this();
#ifdef _WIN32
	OdbcProc::instance().getSentryAsync(config_.project_id, [this, self](uint8_t object_type, const string& object_id, uint32_t info_id, const string& result) {
		JtMsgPostQueryAck ack;
		ack.object_type = object_type;
		ack.object_id = object_id;
		ack.info_id = info_id;
		ack.info_content = result;
		auto project = project_.lock();
		if (project)
		{
			project->sendMsg(&ack);
		}
	});
#endif
}
void MsgService::onHeartBeat()
{
	auto project = project_.lock();
	if (project)
	{
		project->sendMsg(UP_LINKTEST_REQ, NULL, NULL);
	}
	queryDB();
	updatePacketRate();

	auto self = shared_from_this();
	timer_.expires_from_now(std::chrono::seconds(60));	//60?????????
	timer_.async_wait(
		strand_.wrap([this, self](boost::system::error_code ec) {
		if (!ec)
		{
			onHeartBeat();
		}
	}));
}