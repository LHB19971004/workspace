#include "stdafx.h"

#pragma comment(lib,"log_local.lib")
#pragma comment(lib,"cs_lock.lib")
#pragma comment(lib,"ThreadWithLog.lib")
#pragma comment(lib,"ws2_32.lib")
#ifdef _DEBUG
#pragma comment(lib, "json_vc71_libmtd.lib")
#else
#pragma comment(lib, "json_vc71_libmt.lib")
#endif
#pragma comment(lib, "http_method.lib")
#pragma comment(lib, "tiny_xml.lib")