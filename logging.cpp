#include "logging.h"
#include "utils.h"

INITIALIZE_EASYLOGGINGPP

bool logInitialize(const std::string &logFolder)
{
	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
	el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);

	el::Configurations conf;
	conf.setToDefault();
	conf.setGlobally(el::ConfigurationType::ToFile, "true");
	conf.setGlobally(el::ConfigurationType::Filename, Utils::pathCombine(logFolder, "e6_jt.log"));
	// default logger uses default configurations
	el::Loggers::reconfigureLogger("default", conf);
	el::Loggers::setLoggingLevel(el::Level::Debug);
	return true;
}

bool logSetLogLevel(const std::string & logLevel)
{
	auto _ = std::unordered_map<std::string, el::Level>
	{
		{ "trace", el::Level::Trace },
		{ "debug", el::Level::Debug },
		{ "fatal", el::Level::Fatal },
		{ "error", el::Level::Error },
		{ "warning", el::Level::Warning },
		{ "verbose", el::Level::Verbose },
		{ "info", el::Level::Info },
	};

	auto itr = _.find(logLevel);
	if (itr == _.end())
	{
		LOG_ERROR << "logSetLogLevel failed. unknown level : " << logLevel;
		return false;
	}

	LOG_INFO << "Current log level: " << logLevel;
	el::Loggers::setLoggingLevel(itr->second);
	return true;
}