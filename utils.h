#pragma once

#include "std.h"
#include <boost/uuid/uuid_generators.hpp>

class Utils
{
public:
	Utils();
	~Utils();

	static bool readFile(const std::string & fileName, std::string &content);
	static std::string now(const char *format);
	static std::string formatTime(time_t ts, const char *format);
	static bool exists(const std::string &path);
	static bool mkdir(const std::string &path);
	static std::string pathCombine(const std::string &path1, const std::string &path2);
	static std::string absolute(const std::string &path);
    static bool deamonize();
	static std::string devIDToWID(int64_t devid);
	static std::string getKeyIDByFileName(const std::string& file_name);
	static std::string timeToJsonTime(time_t value);
	static uint16_t getSequence();
	static std::string GbkToUtf8(const string& gbk);
	static std::string Utf8ToGbk(const string& utf);
	static std::string getGuid();
	static uint16_t crc16(const char *buff, int len);
	static std::wstring stringToWstring(const string& str);
	static std::list<std::string> findPath(const string& str);
private:
	static boost::uuids::random_generator ramdom_gen_;
	static std::mutex mutex_;
};

