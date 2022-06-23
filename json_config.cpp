#include "stdafx.h"
#include "json_config.h"
#include "log_local\log_local.h"
#include "tiny_xml/xml.h"
#include "tinyxml.h"
#include <ThreadWithLog/ThreadWithLog.h>
#include <map>
#include <algorithm>
#include "http_method/http_method.h"


const static string FILE_NAME = "config.json";

JsonConfigMgr::JsonConfigMgr() 
	:running_(false),
	event_(NULL),
	lock_("dev"),
	type_lock_("alarm_type")
{

}
JsonConfigMgr& JsonConfigMgr::inst()
{
	static JsonConfigMgr _;
	return _;
}

int32_t JsonConfigMgr::init()
{
	if (!http_url_.empty())
	{
		return 0;
	}
	if (!readJsonData() || http_url_.empty())
	{
		return 0;
	}
	running_ = true;
	event_ = CreateEvent(0, 1, 0, 0);
	thread_ = boost::thread(&JsonConfigMgr::readProc, this);
	return 1;
}


int32_t JsonConfigMgr::updateData(Json::Value& js_ret)
{
	if (js_ret.isMember("dev_class"))
	{
		auto& dev = js_ret["dev_class"];
		if (!dev.isMember("adas_rm_class"))
		{
			return 0;
		}
		if (!dev["adas_rm_class"].isArray() || !dev["adas_bsj_class"].isArray() || !dev["adas_youwei_class"].isArray())
		{
			return 0;
		}
		auto& rm_result = dev["adas_rm_class"];
		auto& bsj_result = dev["adas_bsj_class"];
		auto& youwei_result = dev["adas_youwei_class"];
		decltype(map_data_) map_data;
		for (auto i = 0; i < rm_result.size(); i++)
		{
			auto class_id = rm_result[i].asInt();
			map_data.insert(make_pair(class_id, DEV_CLASS_RM));
		}
		for (auto i = 0; i < bsj_result.size(); i++)
		{
			auto class_id = bsj_result[i].asInt();
			map_data.insert(make_pair(class_id, DEV_CLASS_BSJ));
		}
		for (auto i = 0; i < youwei_result.size(); i++)
		{
			auto class_id = youwei_result[i].asInt();
			map_data.insert(make_pair(class_id, DEV_CLASS_YOUWEI));
		}
		{

			CsLockScoped _(&lock_);
			map_data.swap(map_data_);
		}
	}
	if (js_ret.isMember("alarm_type") && js_ret["alarm_type"].isArray())
	{
		decltype(map_alarm_type_) map_alarm;
		auto& result_type = js_ret["alarm_type"];
		for (auto it = result_type.begin(); it != result_type.end(); it++)
		{
			StAlarmType alarm_type_;
			auto& result = *it;
			alarm_type_._protocal = result["protocal"].asInt();
			alarm_type_._modal = result["modal"].asInt();
			alarm_type_._alarm_type = result["alarm_type"].asInt();
			alarm_type_._class = result["class"].asInt();
			auto event = result["event"].asInt();
			//´æµ½mapÖÐ
			map_alarm.insert(make_pair(alarm_type_, event));
		}
		{
			CsLockScoped _(&type_lock_);
			map_alarm.swap(map_alarm_type_);
		}
	}
	return 1;
}

int32_t JsonConfigMgr::readJsonData()
{
	ifstream ifs;
	ifs.open(FILE_NAME, ios::in);
	bool is_ok = ifs.is_open();
	Json::Reader read;
	Json::Value js_info;
	if (!read.parse(ifs, js_info))
	{
		assert(0);
		return 0;
	}
	if (!updateData(js_info))
	{
		return 0;
	}
	ifs.close();
	return 1;
}


void JsonConfigMgr::readProc()
{
	while (running_)
	{
		readThirdInfo();
		WaitForSingleObject(event_, 1000 * 30);
	}
}

int32_t JsonConfigMgr::writeFile(Json::Value& js_data)
{
	Json::FastWriter write;
	ofstream ofs;
	ofs.open(FILE_NAME, ios::out);
	bool isOk = ofs.is_open();
	if (isOk)
	{
		string strJson = write.write(js_data);
		ofs << strJson;
	}
	ofs.close();
	return 1;
}


int32_t JsonConfigMgr::readThirdInfo()
{
	boost::scoped_array<char> buffer;
	int32_t buffer_size = 409600;
	try
	{
		buffer.reset(new char[buffer_size]);
	}
	catch (std::bad_alloc &)
	{
		assert(0);
		return 0;
	}
	list<string> header;
	uint32_t ret_len = buffer_size;
	header.push_back("Content-Type: application/json");
	HttpMethod7(http_url_.c_str(), "", MT_POST, 3, buffer.get(), ret_len, header);
	string str_result = string(buffer.get(), ret_len);
	Json::Reader reader;
	Json::Value js_info;
	map<int32_t, int32_t> class_map;
	if (!reader.parse(str_result, js_info))
	{
		LogLocalText(LOG_LOCAL_ERROR_DEAD, "Json parse fail:%s", str_result.c_str());
		return 0;
	}
	writeFile(js_info);
	if (!updateData(js_info))
	{
		assert(0);
		return 0;

	}
	return 1;
}

DevClass JsonConfigMgr::getThirdClassId(int32_t id)
{
	CsLockScoped cs_lock(&lock_);
	auto iter = map_data_.find(id);
	if (iter != map_data_.end())
	{
		return (DevClass)iter->second;
	}
	return DEV_CLASS_UNKNOWN;
}

int32_t JsonConfigMgr::getEventType(const StAlarmType& alarm)
{
	CsLockScoped lock(&type_lock_);
	auto iter = map_alarm_type_.find(alarm);
	if (iter != map_alarm_type_.end())
	{
		return (iter->second);
	}
	return 0;
}



int32_t JsonConfigMgr::uninit()
{
	if (running_)
	{
		running_ = false;
		if (event_)
		{
			SetEvent(event_);
		}
		if (thread_.joinable())
		{
			thread_.join();
		}
		if (event_)
		{
			CloseHandle(event_);
			event_ = NULL;
		}
	}
	return 1;
}