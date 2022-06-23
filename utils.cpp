#include "utils.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#ifndef _WIN32
#include <unistd.h>
#endif

boost::uuids::random_generator Utils::ramdom_gen_;
std::mutex Utils::mutex_;
	
Utils::Utils()
{
}


Utils::~Utils()
{
}

bool Utils::readFile(const std::string & fileName, std::string &content)
{
	std::ifstream in(fileName, std::ios::binary);
	if (in.good())
	{	
		in.seekg(0, std::ios::end);
		content.reserve(in.tellg());
		in.seekg(0, std::ios::beg);

		content.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
		return true;
	}
	return false;
}

std::string Utils::now(const char * format)
{
	auto ts = time(nullptr);
	return formatTime(ts, format);
}

std::string Utils::formatTime(time_t ts, const char * format)
{
	auto tm = localtime(&ts);

	char strTime[20];
	strftime(strTime, sizeof(strTime), format, tm);
	return strTime;
}

bool Utils::exists(const std::string & path)
{
	namespace fs = boost::filesystem;
	boost::system::error_code ec;
	bool ok = fs::exists(path, ec);
	return ok && !ec;
}

bool Utils::mkdir(const std::string & path)
{
	namespace fs = boost::filesystem;
	boost::system::error_code ec;
	bool ok = fs::create_directories(path, ec);
	return ok && !ec;
}

std::string Utils::pathCombine(const std::string & path1, const std::string & path2)
{
	namespace fs = boost::filesystem;
	auto fsPath1 = fs::path(path1);
	auto fsPath2 = fs::path(path2);
	auto res = (fsPath1 / fsPath2).string();
	return res;
}

std::string Utils::absolute(const std::string & path)
{
	namespace fs = boost::filesystem;
	auto fsPath = fs::path(path);
	auto absPath = fs::absolute(fsPath).string();
	return absPath;
}

bool Utils::deamonize()
{
#ifdef _WIN32
    return true;
#else
    pid_t pid = fork();
    if (pid < 0)
    {
        return false;
    }
    // parent exit
    if (pid > 0)
    {
        exit(0);
    }
    
    if (setsid() < 0)
    {
        return false;
    }

    pid = fork();
    if (pid < 0)
    {
        return false;
    }
    if (pid > 0)
    {
        exit(0);
    }

    umask(0);
    //chdir("/");
    close(STDERR_FILENO);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    return true;
#endif
}

std::string Utils::devIDToWID(int64_t devid)
{
	char tmp[20] = { 0 };
	sprintf(tmp, "%011llu", devid);
	std::string wid = tmp;
	return wid;
}

std::string Utils::getKeyIDByFileName(const std::string& file_name)
{
	std::string key_id;
	size_t pos = file_name.rfind("_");
	if (pos != std::string::npos)
	{
		pos++;
		size_t pos2 = file_name.find(".", pos);
		if (pos2 != std::string::npos)
		{
			key_id = file_name.substr(pos, pos2 - pos);
		}
	}
	return key_id;
}

std::string Utils::timeToJsonTime(time_t value)
{
	return "/Date(" + to_string(value) + "000+0800)/";
}

uint16_t Utils::getSequence()
{
	static std::atomic<uint16_t> seq;
	return seq++;
}

std::string Utils::GbkToUtf8(const string& gbk)
{
	return boost::locale::conv::between(gbk.c_str(), "UTF-8", "GBK");
}

std::string Utils::Utf8ToGbk(const string& utf)
{
	return boost::locale::conv::between(utf.c_str(), "GBK", "UTF-8");
}

std::string Utils::getGuid()
{
    boost::uuids::uuid u;
	{
		std::lock_guard<std::mutex> _(mutex_);
		u = ramdom_gen_();
	}
	auto uuid = boost::uuids::to_string(u);
	return boost::algorithm::erase_all_copy(uuid, "-");
}

uint16_t Utils::crc16(const char *buff, int len)
{
	uint16_t crc = 0xFFFF;          // initial value
	uint16_t polynomial = 0x1021;   // 0001 0000 0010 0001  (0, 5, 12)

	for (int j = 0; j < len; ++j)
	{
		for (int i = 0; i < 8; i++)
		{
			char bit = ((buff[j] >> (7 - i) & 1) == 1);
			char c15 = ((crc >> 15 & 1) == 1);
			crc <<= 1;
			if (c15 ^ bit) crc ^= polynomial;
		}
	}

	crc &= 0xffff;

	return crc;
}

std::wstring Utils::stringToWstring(const string& str)
{
#ifdef _WIN32
	return boost::locale::conv::to_utf<wchar_t>(str.c_str(), "UTF-8");
#else
	return boost::locale::conv::to_utf<wchar_t>(str.c_str(), "UTF-8");
#endif
}

std::list<std::string> Utils::findPath(const string& str)
{
	namespace fs = boost::filesystem;
	std::list<std::string> tmp_list;
	if (fs::exists(str))
	{
		fs::directory_iterator item_begin(str);
		fs::directory_iterator item_end;
		for (; item_begin != item_end; item_begin++)
		{
			if (fs::is_directory(*item_begin))
			{
				//cout << item_begin->path() << "\t[dir]" << endl;
				findPath(item_begin->path().string());
			}
			else
			{
				//cout << item_begin->path() << endl;
				tmp_list.emplace_back(item_begin->path().string());
			}
		}
	}
	return tmp_list;
}