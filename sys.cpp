#include "std.h"

#ifdef _WIN32
#include <dbghelp.h>
void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件  
	//  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


	// Dump信息  
	//  
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;
	// 写入Dump文件内容  
	//  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);


	CloseHandle(hDumpFile);
}

long __stdcall MyUnhandledExceptionFilter(__in struct _EXCEPTION_POINTERS* e)
{
	static int seq = 0;
	TCHAR szFileName[128];
	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);
	wsprintf(szFileName, L"CrashDump-%04d%02d%02d-%02d%02d%02d_%d.dmp",
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, seq++);
	CreateDumpFile(szFileName, e);
	return 1;
}

void sysInit()
{
	TCHAR   title[100];
	GetConsoleTitle(title, ARRAYSIZE(title));
	HWND hwnd = ::FindWindow(NULL, title);
	if (hwnd)
	{
		HMENU hmenu = ::GetSystemMenu(hwnd, FALSE);
		assert(hmenu);
		if (hmenu)
		{
			::RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
			::RemoveMenu(hmenu, SC_MAXIMIZE, MF_BYCOMMAND);
		}
	}
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
}

#else
void sysInit()
{

}
#endif
