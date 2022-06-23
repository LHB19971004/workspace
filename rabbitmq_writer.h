#pragma once

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "config.h"

using namespace AmqpClient;

class RabbitMQWriter : private boost::noncopyable
{
public:
    typedef shared_ptr<RabbitMQWriter> Ptr;

	struct Config
	{
		string address;
		uint16_t port;
		string user;
		string password;
		string vhost;
		string exchange;
		string routing_key;
		bool zip;
	};

    RabbitMQWriter(const Config& config);
    ~RabbitMQWriter(void);

	void produce(const string& str);
private:
	void push(const string& str);
    void DoWork();
    Channel::ptr_t RecreateChannelIfNeeded(Channel::ptr_t &channel);
	void save();
	void load();
	string getFileName();
private:
	Config config_;
    std::thread thread_;
    bool exit_ = false;
    Channel::ptr_t channel_;

	std::condition_variable cv_;
	std::mutex mutex_;
	list<string> list_;
};