#pragma once

#include "std.h"
#include "msg_def.h"
struct ServerConfig
{
	struct ProjectConfig
	{
		uint32_t project_id;
		string project_name;
		uint32_t user_name;
		string user_password;
		uint32_t access_code;
		ProjectVersion project_version;
		uint8_t version[3] = { 0,0,1 };
		bool http_enable = true;
		string plat_id;
		string produce_id;
		string terminal_type;
		struct Encrypt
		{
			bool encrypt_flag = false;
			uint32_t encrypt_key;
			uint32_t m1;
			uint32_t IA1;
			uint32_t IC1;
		}encrypt;

		struct MainLink
		{
			string host;
			uint16_t port;
		}main_link;

		struct SlaveLink
		{
			string ip;
			string host;
			uint16_t port;
		}slave_link;

		struct MqConsumer {
			bool enable = false;
			string address;
			uint16_t port;
			string user;
			string password;
			string vhost;
			string exchange;
			string queue;
			string routing_key;
			bool zip = true;
		}mq_consumer;

		struct KafkaConsumer {
			bool enable = false;
			std::string tag;
			std::string brokers;
			std::string topic;
			std::string group_id;
		}kafka_consumer;
	};
	map<uint32_t, ProjectConfig> projects;

	int32_t tcp_thread_count = 1;
	bool deamon;
	struct HttpPlat {
		bool enable = false;
		string base_url;
		uint16_t port = 80;
		string page_url;
		string alldata_url;
	}http_plat;

	struct DbConfig {
		string tag;
		string provider;
		string server;
		string database;
		string user;
		string password;
	};
	map<string, DbConfig> db_configs;

	struct Monitor {
		struct MqConsumer {
			bool enable = false;
			string address;
			uint16_t port;
			string user;
			string password;
			string vhost;
			string exchange;
			string queue;
			string routing_key;
			bool zip = true;
		}cmd_consumer;

		struct MqProducer {
			bool enable = false;
			string address;
			uint16_t port;
			string user;
			string password;
			string vhost;
			string exchange;
			string routing_key;
			bool zip = true;
		};
		MqProducer cmd_producer;
		MqProducer data_producer;
	}monitor;
};

class ConfigItem;
class Config
{
public:
	static Config &instance();
	bool load(const std::string &fileName);
	bool parse(const std::string &content);
	const ServerConfig &getServerConfig() const;
	const std::string &getLogLevel() const;
private:
	Config();
	~Config();
	bool initialize(const std::shared_ptr<ConfigItem> &config);

	ServerConfig serverConfig_;
	std::string logLevel_ = "info";
	std::shared_ptr<ConfigItem> rootConfig_;
};

