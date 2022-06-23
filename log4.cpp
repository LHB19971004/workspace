#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/DailyRollingFileAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4.h"
#include "utils.h"
#include "log4_filter.h"
#include <boost/algorithm/string.hpp>
log4cpp::PatternLayout* generateLayout()
{
	log4cpp::PatternLayout* pLayout = new log4cpp::PatternLayout();
	pLayout->setConversionPattern("%d{%H:%M:%S,%l} %p [%c]%x: %m%n");
	return pLayout;
}

log4cpp::Filter* generateFilter(log4cpp::Priority::PriorityLevel level)
{
	return new log4cpp::PriorityFilter(level);
}

log4cpp::DailyRollingFileAppender* generateAppender(const std::string& name, log4cpp::Priority::PriorityLevel level)
{
	auto file_name = "log\\" + name + "_" + boost::to_lower_copy(log4cpp::Priority::getPriorityName(level)) + ".log";
	log4cpp::DailyRollingFileAppender* appender = new log4cpp::DailyRollingFileAppender(name, file_name, 30);
	appender->setLayout(generateLayout());
	appender->setFilter(generateFilter(level));
	return appender;
}

void init_log()
{
	Utils::mkdir("log");
	log4cpp::Category& test = log4cpp::Category::getInstance(std::string("test"));
	test.setAppender(generateAppender("test", log4cpp::Priority::WARN));
	test.setAppender(generateAppender("test", log4cpp::Priority::ERROR));
	test.setAppender(generateAppender("test", log4cpp::Priority::NOTSET));
	test.setPriority(log4cpp::Priority::NOTSET);

	//test.getStream(log4cpp::Priority::DEBUG) << "debug test";
	//test.getStream(log4cpp::Priority::WARN) << "warn test";
	//test.getStream(log4cpp::Priority::ERROR) << "error test";
	test.debug("debug test");
	test.warn("warn test");
	test.error("error test");

	log4cpp::Category& debug = log4cpp::Category::getInstance(std::string("debug"));
	debug.setAppender(generateAppender("debug", log4cpp::Priority::WARN));
	debug.setAppender(generateAppender("debug", log4cpp::Priority::ERROR));
	debug.setAppender(generateAppender("debug", log4cpp::Priority::NOTSET));
	debug.setPriority(log4cpp::Priority::NOTSET);

	//test.getStream(log4cpp::Priority::DEBUG) << "debug test";
	//test.getStream(log4cpp::Priority::WARN) << "warn test";
	//test.getStream(log4cpp::Priority::ERROR) << "error test";
	debug.debug("debug test");
	debug.warn("warn test");
	debug.error("error test");
}