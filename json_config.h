#pragma once
#include <boost/thread.hpp>
#include <cs_lock/CsLockScoped.h>
#include <cs_lock/CsLock.h>
#include <json/json.h>
#include <json/writer.h>
#include "def.h"

struct StAlarmType
{
	int32_t _protocal;
	int32_t _modal;
	int32_t _alarm_type;
	int32_t _class;
	bool operator<(const StAlarmType& other) const
	{
		if (_protocal < other._protocal)
		{
			return true;
		}
		else if (_protocal == other._protocal)
		{
			if (_modal < other._modal)
			{
				return true;
			}
			else if (_modal == other._modal)
			{
				if (_alarm_type < other._alarm_type)
				{
					return true;
				}
				else if (_alarm_type == other._alarm_type)
				{
					if (_class != other._class)
					{
						return true;
					}
				}

			}
		}
		return false;
	}

};



class JsonConfigMgr
{
public:
	static JsonConfigMgr& inst();
	int32_t init();
	int32_t uninit();
	int32_t getEventType(const StAlarmType& alarm);
	DevClass getThirdClassId(int32_t id);
private:
	JsonConfigMgr();
	~JsonConfigMgr() {};
	int32_t readJsonData();
	int32_t writeFile(Json::Value& js_data);
	int32_t readThirdInfo();
	int32_t updateData(Json::Value& js_ret);
	void readProc();
private:
	map<StAlarmType, int32_t> map_alarm_type_;
	map<int32_t, int32_t> map_data_;
	string http_url_;
	CsLock lock_;
	CsLock type_lock_;
	boost::thread thread_;
	volatile int32_t running_;
	HANDLE  event_;
};
