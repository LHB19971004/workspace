#pragma once

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "config.h"

using namespace AmqpClient;

class RabbitMQReader : private boost::noncopyable
{
public:
    using CallBack = std::function<void(const string& queue, string& message)>;
    typedef shared_ptr<RabbitMQReader> Ptr;

	struct Config
	{
		string address;
		uint16_t port;
		string user;
		string password;
		string vhost;
		string exchange;
		string queue;
		string routing_key;
		bool zip;
		bool auto_delete;
	};

    RabbitMQReader(Config const &config, CallBack callback);
    ~RabbitMQReader(void);

	void lock();
	void unlock();
private:
    int32_t Consume();
    void DoWork();
    int32_t Initialize(const Channel::ptr_t &channel);
    Channel::ptr_t RecreateChannelIfNeeded(Channel::ptr_t &channel);
private:
	Config config_;
    std::string consumer_tag_;
    std::thread thread_;
    bool exit_ = false;
    Channel::ptr_t channel_;
    CallBack callback_;

	std::condition_variable cv_;
	std::mutex mutex_;
	bool lock_ = true;
};