#include "kafka_reader.h"
#include <stdint.h>
#ifdef _WIN32
#include "GZipCodec/GZipCodec.h"
#else
#include "gzip_codec/gzip_codec.h"
#endif
#include <common_fun/fun_coding.h>
#include "boost/locale.hpp"
#include "test_code.h"

static const int32_t KAFKA_CONSUME_INTERVAL = 1;

KafkaReader::KafkaReader(Config const& config, CallBack callback) : config_(config), 
            callback_(callback),rebalance_cb_(config.topic), event_cb_(config.topic)
{
	thread_ = std::thread(&KafkaReader::DoWork, this);
}

KafkaReader::~KafkaReader()
{
	exit_ = true;
	cv_.notify_one();
	thread_.join();
}


int32_t KafkaReader::RecreateConsumerIfNeeded()
{
	if (!consumer_)
	{
		try
		{
			string error = "";
			unique_ptr<RdKafka::Conf> conf(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
			unique_ptr<RdKafka::Conf> tconf(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
			
			//ÉèÖÃbroker
			if (conf->set("metadata.broker.list", config_.brokers, error) != RdKafka::Conf::CONF_OK)
			{
				LOG_ERROR << "RdKafka conf set broker failed"<< ",tag:"<<config_.tag << "error:" << error;
			}

			//ÉèÖÃgroup id
			if (conf->set("group.id", config_.group_id, error) != RdKafka::Conf::CONF_OK)
			{
				LOG_ERROR << "RdKafka conf set group.id failed" <<",tag"<< config_.tag << "error:" << error;
			}

			if(conf->set("enable.partition.eof", "false", error) != RdKafka::Conf::CONF_OK)
			{
				LOG_ERROR << "Failed to set enable.partition.eof: " << error;
			}
			conf->set("event_cb", &event_cb_, error);
			conf->set("rebalance_cb", &rebalance_cb_, error);
			conf->set("default_topic_conf", tconf.get(), error);
			consumer_.reset(RdKafka::KafkaConsumer::create(conf.get(), error));
			if (!consumer_)
			{
				LOG_ERROR << "KafkaConsumer create failed" <<",tag"<< config_.tag << "error:" << error;
				return 0;
			}
			vector<string> topics = { config_.topic };
			RdKafka::ErrorCode err = consumer_->subscribe(topics);
			if (err)
			{
				LOG_ERROR << "KafkaConsumer subscribe failed" << ",tag" << config_.tag << "error:" << error;
				consumer_.reset();
				return 0;
			}
		}
		catch (std::runtime_error const &e)
		{
			LOG_ERROR << __FUNCTION__ << "exception:" << e.what();
			consumer_.reset();
		}
	}
	return 1;
}


int KafkaReader::DoWork()
{
	while (!exit_)
	{
		std::unique_lock<std::mutex> _(mutex_);
		cv_.wait(_, [this]()
		{
			return !lock_ || exit_;
		});
		if (exit_)
		{
			return 1;
		}
		if (Consume())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		}
	}
	return 1;
}



int32_t KafkaReader::Consume()
{
	while (!exit_)
	{
		if (RecreateConsumerIfNeeded())
		{
			unique_ptr<RdKafka::Message> message(consumer_->consume(1000));
			switch (message->err()) {
			case RdKafka::ERR__TIMED_OUT:
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				break;
			case RdKafka::ERR_NO_ERROR:
			{
				auto str = string(static_cast<const char *>(message->payload()), message->len());
				callback_(config_.topic, str); 
			}
			break;
			case RdKafka::ERR__UNKNOWN_TOPIC:
			case RdKafka::ERR__UNKNOWN_PARTITION:
			default:
				LOG_ERROR << "KafkaConsumer Consume failed" << ",tag" << config_.tag << "error:" << message->errstr();
				consumer_->close();
				consumer_.reset();
			}
		}
	}
	return 1;
}


void KafkaReader::lock()
{
	lock_ = true;
}

void KafkaReader::unlock()
{
	lock_ = false;
	cv_.notify_one();
}