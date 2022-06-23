#pragma once
#include "std.h"

class Project;
class App
{
public:
	App(std::string& root_folder);
	~App();
	void stop();
	int32_t exec();
	boost::asio::io_service& GetIoService() { return io_service_; };
private:
	int32_t init();
	int32_t uninit();
private:
	std::string root_folder_;
	boost::asio::io_service io_service_;
	std::unique_ptr<boost::asio::io_service::work> io_work_;
	boost::asio::detail::thread_group thread_group_;
	boost::asio::signal_set signal_;
	std::mutex mutex_;
	std::condition_variable cv_;
	bool exit_ = false;
	map<uint32_t, std::shared_ptr<Project>> projects_;
};

