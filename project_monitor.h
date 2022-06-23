#pragma once
#include "std.h"
#include <boost/noncopyable.hpp>
#include "json/json.h"
#include "rabbitmq_reader.h"
#include "rabbitmq_writer.h"



/*
time:2022-3-22

*/

class RabbitMQReader;
class RabbitMQWriter;
class Project;



class ProjectMonitor : public boost::noncopyable
{
public:
	static ProjectMonitor& instance();
	void init();
	void registerProject(uint32_t project_id, std::weak_ptr<Project> project);
	void unregisterProject(uint32_t project_id);
	void sendData(const string& data);
private:
	ProjectMonitor();
	~ProjectMonitor() {};
	void cmdReaderCbk(const string& queue, string& message);
	void cmdParse(Json::Value& json);
	std::shared_ptr<Project> getProjectPtr(uint32_t project_id);
private:
	std::map<uint32_t, std::weak_ptr<Project>> map_;
	std::mutex mutex_;
	unique_ptr<RabbitMQReader> cmd_reader_;
	unique_ptr<RabbitMQWriter> cmd_writer_;
	unique_ptr<RabbitMQWriter> data_writer_;
};