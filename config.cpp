#include "config.h"
#include "config_parser.h"
#include "utils.h"
#include <boost/algorithm/string.hpp>
Config & Config::instance()
{
	static Config _;
	return _;
}

bool Config::parse(const std::string & con)
{
	ConfigParser parser(con);
	auto config = parser.parse();
	if (config)
	{
		return initialize(rootConfig_ = config);
	}
	return false;
}

const ServerConfig & Config::getServerConfig() const
{
	return serverConfig_;
}

const std::string & Config::getLogLevel() const
{
	return logLevel_;
}

bool Config::load(const std::string & fileName)
{
	std::string content;
	if (Utils::readFile(fileName, content))
	{
		return parse(content);
	}
	return false;
}

Config::Config()
{
}


Config::~Config()
{
}

bool Config::initialize(const std::shared_ptr<ConfigItem>& config)
{
	assert(config != nullptr);

	int tcp_thread_count = 0;
	if (config->item("tcp_thread_count", tcp_thread_count))
	{
		serverConfig_.tcp_thread_count = tcp_thread_count;
	}


	std::string logLevel;
	if (config->item("log_level", logLevel))
	{
		logLevel_ = logLevel;
	}

	bool deamon = false;
	if (config->item("deamon", deamon))
	{
		serverConfig_.deamon = deamon;
	}

	std::shared_ptr<ConfigItem> http_plat;
	if (config->item("http_plat", http_plat))
	{
		auto& config = http_plat;
		bool enable = false;
		if (config->item("enable", enable))
		{
			serverConfig_.http_plat.enable = enable;
		}
		string base_url;
		if (config->item("base_url", base_url))
		{
			serverConfig_.http_plat.base_url = base_url;
		}
		int port;
		if (config->item("port", port))
		{
			serverConfig_.http_plat.port = port;
		}
		string page_url;
		if (config->item("page_addr", page_url))
		{
			serverConfig_.http_plat.page_url = page_url;
		}
		string alldata_url;
		if (config->item("alldata_addr", alldata_url))
		{
			serverConfig_.http_plat.alldata_url = alldata_url;
		}
	}

	std::vector<std::shared_ptr<ConfigItem>> dbConfigs;
	config->item("db_config", dbConfigs);

	for (auto config : dbConfigs)
	{
		ServerConfig::DbConfig db_config;
		string tag;
		if (config->item("tag", tag))
		{
			db_config.tag = tag;
		}

		string provider;
		if (config->item("provider", provider))
		{
			db_config.provider = provider;
		}

		string server;
		if (config->item("server", server))
		{
			db_config.server = server;
		}

		string database;
		if (config->item("database", database))
		{
			db_config.database = database;
		}

		string user;
		if (config->item("user", user))
		{
			db_config.user = user;
		}

		string password;
		if (config->item("password", password))
		{
			db_config.password = password;
		}

		serverConfig_.db_configs.insert({db_config.tag, db_config});
	}

	std::shared_ptr<ConfigItem> monitor;
	if (config->item("monitor", monitor))
	{
		auto& config = monitor;

		std::shared_ptr<ConfigItem> cmd_consumer;
		if (config->item("cmd_consumer", cmd_consumer))
		{
			auto& config = cmd_consumer;

			bool enable = false;
			if (config->item("enable", enable))
			{
				serverConfig_.monitor.cmd_consumer.enable = enable;
			}
			string address;
			if (config->item("address", address))
			{
				serverConfig_.monitor.cmd_consumer.address = address;
			}
			int port;
			if (config->item("port", port))
			{
				serverConfig_.monitor.cmd_consumer.port = port;
			}
			string user;
			if (config->item("user", user))
			{
				serverConfig_.monitor.cmd_consumer.user = user;
			}
			string password;
			if (config->item("password", password))
			{
				serverConfig_.monitor.cmd_consumer.password = password;
			}
			string vhost;
			if (config->item("vhost", vhost))
			{
				serverConfig_.monitor.cmd_consumer.vhost = vhost;
			}
			string exchange;
			if (config->item("exchange", exchange))
			{
				serverConfig_.monitor.cmd_consumer.exchange = exchange;
			}
			string queue;
			if (config->item("queue", queue))
			{
				serverConfig_.monitor.cmd_consumer.queue = queue;
			}
			string routing_key;
			if (config->item("routing_key", routing_key))
			{
				serverConfig_.monitor.cmd_consumer.routing_key = routing_key;
			}
			bool zip = true;
			if (config->item("zip", zip))
			{
				serverConfig_.monitor.cmd_consumer.zip = zip;
			}
		}

		std::shared_ptr<ConfigItem> cmd_producer;
		if (config->item("cmd_producer", cmd_producer))
		{
			auto& config = cmd_producer;

			bool enable = false;
			if (config->item("enable", enable))
			{
				serverConfig_.monitor.cmd_producer.enable = enable;
			}
			string address;
			if (config->item("address", address))
			{
				serverConfig_.monitor.cmd_producer.address = address;
			}
			int port;
			if (config->item("port", port))
			{
				serverConfig_.monitor.cmd_producer.port = port;
			}
			string user;
			if (config->item("user", user))
			{
				serverConfig_.monitor.cmd_producer.user = user;
			}
			string password;
			if (config->item("password", password))
			{
				serverConfig_.monitor.cmd_producer.password = password;
			}
			string vhost;
			if (config->item("vhost", vhost))
			{
				serverConfig_.monitor.cmd_producer.vhost = vhost;
			}
			string exchange;
			if (config->item("exchange", exchange))
			{
				serverConfig_.monitor.cmd_producer.exchange = exchange;
			}
			string routing_key;
			if (config->item("routing_key", routing_key))
			{
				serverConfig_.monitor.cmd_producer.routing_key = routing_key;
			}
			bool zip = true;
			if (config->item("zip", zip))
			{
				serverConfig_.monitor.cmd_producer.zip = zip;
			}
		}

		std::shared_ptr<ConfigItem> data_producer;
		if (config->item("data_producer", data_producer))
		{
			auto& config = data_producer;

			bool enable = false;
			if (config->item("enable", enable))
			{
				serverConfig_.monitor.data_producer.enable = enable;
			}
			string address;
			if (config->item("address", address))
			{
				serverConfig_.monitor.data_producer.address = address;
			}
			int port;
			if (config->item("port", port))
			{
				serverConfig_.monitor.data_producer.port = port;
			}
			string user;
			if (config->item("user", user))
			{
				serverConfig_.monitor.data_producer.user = user;
			}
			string password;
			if (config->item("password", password))
			{
				serverConfig_.monitor.data_producer.password = password;
			}
			string vhost;
			if (config->item("vhost", vhost))
			{
				serverConfig_.monitor.data_producer.vhost = vhost;
			}
			string exchange;
			if (config->item("exchange", exchange))
			{
				serverConfig_.monitor.data_producer.exchange = exchange;
			}
			string routing_key;
			if (config->item("routing_key", routing_key))
			{
				serverConfig_.monitor.data_producer.routing_key = routing_key;
			}
			bool zip = true;
			if (config->item("zip", zip))
			{
				serverConfig_.monitor.cmd_producer.zip = zip;
			}
		}
	}

	std::vector<std::shared_ptr<ConfigItem>> projectConfigs;
	config->item("project", projectConfigs);
	for (auto config : projectConfigs)
	{
		std::string name;
		auto nameCfg = config->item();
		if (nameCfg)
		{
			name = nameCfg->text();
		}

		ServerConfig::ProjectConfig project;
		int project_id;
		if (config->item("project_id", project_id))
		{
			project.project_id = project_id;
		}

		string project_name;
		if (config->item("project_name", project_name))
		{
			project.project_name = project_name;
		}

		int user_name;
		if (config->item("user_name", user_name))
		{
			project.user_name = user_name;
		}
		string user_password;
		if (config->item("user_password", user_password))
		{
			project.user_password = user_password;
		}
		int access_code;
		if (config->item("access_code", access_code))
		{
			project.access_code = access_code;
		}
		int project_version;
		if (config->item("project_version", project_version))
		{
			project.project_version = (ProjectVersion)project_version;
		}
		string plat_id;
		if (config->item("plat_id", plat_id))
		{
			project.plat_id = plat_id;
		}

		string produce_id;
		if (config->item("produce_id", produce_id))
		{
			project.produce_id = produce_id;
		}

		string terminal_type; 
		if (config->item("terminal_type", terminal_type))
		{
			project.terminal_type = terminal_type;
		}

		bool http_enable;
		if (config->item("http_enable", http_enable))
		{
			project.http_enable = http_enable;
		}
		std::shared_ptr<ConfigItem> encrypt;
		if(config->item("encrypt", encrypt))
		{
			auto& config = encrypt;
			bool encrypt_flag;
			if (config->item("encrypt_flag", encrypt_flag))
			{
				project.encrypt.encrypt_flag = encrypt_flag;
			}
			int encrypt_key;
			if (config->item("encrypt_key", encrypt_key))
			{
				project.encrypt.encrypt_key = encrypt_key;
			}
			int m1;
			if (config->item("m1", m1))
			{
				project.encrypt.m1 = m1;
			}
			int IA1;
			if (config->item("IA1", IA1))
			{
				project.encrypt.IA1 = IA1;
			}
			int IC1;
			if (config->item("IC1", IC1))
			{
				project.encrypt.IC1 = IC1;
			}
		}

		std::shared_ptr<ConfigItem> main_link;
		if (config->item("main_link", main_link))
		{
			auto& config = main_link;

			string host;
			if (config->item("host", host))
			{
				project.main_link.host = host;
			}
			int port;
			if (config->item("port", port))
			{
				project.main_link.port = port;
			}
		}

		std::shared_ptr<ConfigItem> slave_link;
		if (config->item("slave_link", slave_link))
		{
			auto& config = slave_link;
			string ip;
			if (config->item("ip", ip))
			{
				project.slave_link.ip = ip;
			}
			string host;
			if (config->item("host", host))
			{
				project.slave_link.host = host;
			}
			int port;
			if (config->item("port", port))
			{
				project.slave_link.port = port;
			}
		}

		std::shared_ptr<ConfigItem> mq_consumer;
		if (config->item("mq_consumer", mq_consumer))
		{
			auto& config = mq_consumer;

			bool enable = false;
			if (config->item("enable", enable))
			{
				project.mq_consumer.enable = enable;
			}
			string address;
			if (config->item("address", address))
			{
				project.mq_consumer.address = address;
			}
			int port;
			if (config->item("port", port))
			{
				project.mq_consumer.port = port;
			}
			string user;
			if (config->item("user", user))
			{
				project.mq_consumer.user = user;
			}
			string password;
			if (config->item("password", password))
			{
				project.mq_consumer.password = password;
			}
			string vhost;
			if (config->item("vhost", vhost))
			{
				project.mq_consumer.vhost = vhost;
			}
			string exchange;
			if (config->item("exchange", exchange))
			{
				project.mq_consumer.exchange = exchange;
			}
			string queue;
			if (config->item("queue", queue))
			{
				project.mq_consumer.queue = queue;
			}
			string routing_key;
			if (config->item("routing_key", routing_key))
			{
				project.mq_consumer.routing_key = routing_key;
			}
			bool zip = true;
			if (config->item("zip", zip))
			{
				project.mq_consumer.zip = zip;
			}
		}

		std::shared_ptr<ConfigItem> kafka_consumer;
		if (config->item("kafka_consumer", kafka_consumer))
		{
			auto& config = kafka_consumer;
			bool enable = false;
			if (config->item("enable", enable))
			{
				project.kafka_consumer.enable = enable;
			}
			string tag;
			if (config->item("tag", tag))
			{
				project.kafka_consumer.tag = tag;
			}
			string brokers;
			if (config->item("brokers", brokers))
			{
				project.kafka_consumer.brokers = brokers;
			}
			string topic;
			if (config->item("topic", topic))
			{
				project.kafka_consumer.topic = topic;
			}
			string groupid;
			if (config->item("group_id", groupid))
			{
				project.kafka_consumer.group_id = groupid;
			}
		}
		serverConfig_.projects.insert(make_pair(project.project_id, project));
	}
	return true;
}