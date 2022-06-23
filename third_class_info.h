#pragma once
#include "def.h"

int32_t ThirdClassInfoInit();

int32_t ThirdClassInfoUninit();

DevClass GetThirdClassInfo(int32_t id);

int32_t GetEventType(int32_t protocal,int32_t modal,int32_t alarm_type,int32_t class_value);
