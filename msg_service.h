#pragma once
#include "std.h"
#include "json/json.h"
#include "config.h"
#include "jt_send.h"
class RabbitMQReader;
class KafkaReader;
class Project;
class DevDataMgr;

class MsgService:public std::enable_shared_from_this<MsgService>
{
public:
	MsgService(boost::asio::io_service& io_service, const ServerConfig::ProjectConfig& config, std::weak_ptr<Project> project);
	~MsgService();

	void start();
	void stop();

	void stopConsume();
	void startConsume();

	uint32_t getPacketRate() { return packet_rate_; };
private:
	void mqReaderCbk(const string& queue, string& message);
	void kafkaReaderCbk(const string& queue, string& message);
	void parseJson(Json::Value& json);
	WarnInfoGDBY fomartWarnInfo( const JtPos& pos, string& wid, string& status);
	void parseICData(const Json::Value& json);
	uint32_t devStatusAnalysis(const string& status_str);
	uint32_t devAlarmAnalysis(const string& alarm_str, uint16_t speed);

	void updatePacketRate();
	void onHeartBeat();
	void queryDB();
private:
	std::weak_ptr<Project> project_;
	ServerConfig::ProjectConfig config_;
	boost::asio::io_service& io_service_;
	boost::asio::io_service::strand strand_;
	boost::asio::steady_timer timer_;
	unique_ptr<RabbitMQReader> mq_reader_;

	unique_ptr<KafkaReader> kafka_reader_;
	std::atomic<uint32_t> packet_rate_curr_;
	std::atomic<uint32_t> packet_rate_;
};

