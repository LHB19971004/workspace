#pragma once
#include "config.h"
#include <boost/noncopyable.hpp>
#include "rdkafkacpp.h"
#include "logging.h"


class ExampleRebalanceCb : public RdKafka::RebalanceCb {
public:
	ExampleRebalanceCb(const string& topic) :topic_(topic) {};
private:
	static string part_list_print(const std::vector<RdKafka::TopicPartition*>&partitions) {
		string str;
		char tmp[128];
		for (unsigned int i = 0; i < partitions.size(); i++)
		{
			sprintf(tmp, "%s[%d],", partitions[i]->topic().c_str(), partitions[i]->partition());
			str.append(tmp);
		}
		if (!str.empty() && str.back() == ',')
		{
			str.pop_back();
		}
		return str;
	}
public:
	void rebalance_cb(RdKafka::KafkaConsumer *consumer,
		RdKafka::ErrorCode err,
		std::vector<RdKafka::TopicPartition*> &partitions) {
		string part_list = part_list_print(partitions);
		LOG_ERROR << "RdKafka RebalanceCb:" << topic_ << "," << RdKafka::err2str(err) << "Partitions:" << part_list;
		if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
			consumer->assign(partitions);
		}
		else {
			consumer->unassign();
		}
	}
private:
	string topic_;
};



class ReaderEventCb : public RdKafka::EventCb {
public:
	ReaderEventCb(const string& topic) :topic_(topic) {};
	void event_cb(RdKafka::Event &event) {
		switch (event.type())
		{
		case RdKafka::Event::EVENT_ERROR:
			LOG_ERROR << "[" << topic_.c_str() << "]" << (event.fatal() ? "FATAL" : "NORMAL") << "_ERROR-" << event.severity() << "-" << event.fac().c_str() << ": " << event.str().c_str();
			break;
		case RdKafka::Event::EVENT_STATS:
			LOG_ERROR << "[" << topic_.c_str() << "]STATS-" << event.severity() << "-" << event.fac().c_str() << ": " << event.str().c_str();
			break;
		case RdKafka::Event::EVENT_LOG:
			LOG_ERROR << "[" << topic_.c_str() << "]LOG-" << event.severity() << "-" << event.fac().c_str() << ": " << event.str().c_str();
			break;
		default:
			LOG_ERROR << "[" << topic_.c_str() << "]EVENT-" << event.severity() << "-" << event.fac().c_str() << ": " << event.str().c_str();
			break;
		}
	}
	string topic_;
};



class KafkaReader : private boost::noncopyable
{
public:
	typedef shared_ptr<KafkaReader> Ptr;
	using CallBack = std::function<void(const string& queue, string& message)>;
	struct Config
	{
		string  tag;
		string brokers;
		string topic;
		string group_id;
	};
	KafkaReader(Config const& config, CallBack callback);
	~KafkaReader();
	void lock();
	void unlock();

private:
	int32_t RecreateConsumerIfNeeded();
	int32_t Consume();
	int32_t DoWork();
private:
	Config config_;
	unique_ptr<RdKafka::KafkaConsumer> consumer_;
	std::string consumer_tag_;
	std::thread thread_;
	bool volatile exit_ = false;
	CallBack  callback_;
	std::condition_variable cv_;
	std::mutex mutex_;
	bool lock_ = true;
	ReaderEventCb event_cb_;
	ExampleRebalanceCb rebalance_cb_;
};




