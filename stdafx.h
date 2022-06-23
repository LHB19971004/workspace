// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <map>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <boost\scoped_array.hpp>

using namespace std;

//#include "log_local2/log_local.h"
//#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))

#pragma warning(disable:4244)