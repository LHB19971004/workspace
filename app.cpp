#include "app.h"
#include "logging.h"
#include "utils.h"
#include "config.h"
#include <boost/range/algorithm_ext/erase.hpp>
#include "boost/algorithm/string.hpp"
#include "project.h"
#include "thread_pool.h"
#include "odbc_conn.h"
#include "project_monitor.h"

App::App(std::string& root_folder)
	:
	root_folder_(root_folder),
#ifdef _WIN32
	signal_(io_service_, SIGINT, SIGTERM)
#else
	signal_(io_service_, SIGINT, SIGTERM, SIGPIPE)
#endif
{

}

App::~App()
{
}

int32_t App::init()
{
	setlocale(LC_ALL,"chs");
	Utils::mkdir("app_data");
	auto logFolder = Utils::pathCombine(root_folder_, "logs");
	if (!logInitialize(logFolder))
	{
		std::cerr << "LogInitialize failed." << std::endl;
		return false;
	}
	LOG_INFO << "Server root: " << root_folder_;

	auto confFile = Utils::pathCombine(root_folder_, "app.conf");
	if (!Config::instance().load(confFile))
	{
		LOG_ERROR << "App::init() failed. Config::instance().load fail.";
		return false;
	}
 
	// set log level
	auto logLevel = Config::instance().getLogLevel();
	if (!logSetLogLevel(logLevel))
	{
		std::cerr << "logSetLogLevel failed." << std::endl;
		return false;
	}

	auto &config = Config::instance().getServerConfig();
	LOG_INFO << "Server deamon: " << config.deamon;

	if (config.deamon && !Utils::deamonize())
	{
		std::cerr << "Deamonize failed." << std::endl;
		return false;
	}

#ifndef _WIN32
	chdir(root_folder_.c_str());
#endif
	io_work_ = boost::make_unique<boost::asio::io_service::work>(io_service_);
	thread_group_.create_threads([this] {
			while (!exit_) {
				io_service_.run(); 
			}
	}, config.tcp_thread_count);

	ProjectMonitor::instance().init();
#ifdef _WIN32
	OdbcProc::instance().init();
#endif
	for (auto project_config: config.projects)
	{
		auto project = std::make_shared<Project>(io_service_, project_config.second);
		if (project->run())
		{
			projects_.insert({ project_config.second.project_id, project});
		}
		else
		{
			LOG_INFO << "project run error: " << project_config.first;
		}
	}
	return 1;
}

int32_t App::uninit()
{
	io_work_.reset();
	io_service_.stop();
	thread_group_.join();
	projects_.clear();
#ifdef _WIN32
	OdbcProc::instance().uninit();
#endif
	return 1;
}

int32_t App::exec()
{
	if (!init())
	{
		return 0;
	}

	std::function<void(const boost::system::error_code &, int)> handler;
	handler = [this, &handler](const boost::system::error_code &, int sig)
	{
#ifdef _WIN32
		stop();
#else
		if (sig == SIGPIPE)
		{
			LOG_WARNING << "App got SIGPIPE, ignore";
			// continue wait
			signal_.async_wait(handler);
		}
		else
		{
			stop();
		}
#endif
	};
	signal_.async_wait(handler);

	std::unique_lock <std::mutex> lck(mutex_);
	cv_.wait(lck,[this](){
		return exit_;
	});
	
	uninit();
	LOG_INFO << "App Close";
	return 1;
}

void App::stop()
{
	std::unique_lock <std::mutex> lck(mutex_);
	exit_ = true;
	cv_.notify_one();
}