#include "stdafx.h"
#include "third_class_info.h"
#include "json_config.h"

DevClass GetThirdClassInfo(int32_t id)
{
	 return JsonConfigMgr::inst().getThirdClassId(id);
}

int32_t ThirdClassInfoInit()
{
	return JsonConfigMgr::inst().init();
}

int32_t GetEventType(int32_t protocal, int32_t modal, int32_t alarm_type, int32_t class_value)
{
	StAlarmType alarm;
	alarm._protocal = protocal;
	alarm._modal = modal;
	alarm._alarm_type = alarm_type;
	alarm._class = class_value;
	return  JsonConfigMgr::inst().getEventType(alarm);
}

int32_t ThirdClassInfoUninit()
{
	return JsonConfigMgr::inst().uninit(); 
}
