#pragma once
#include "std.h"
#include "easylogging++.h"

#define LOG_TRACE LOG(TRACE)
#define LOG_DEBUG LOG(DEBUG)
#define LOG_INFO LOG(INFO)
#define LOG_WARNING LOG(WARNING)
#define LOG_ERROR LOG(ERROR)

bool logInitialize(const std::string &logFolder);
bool logSetLogLevel(const std::string &logLevel);

