#ifndef _CDISPLAYADAPTERHELPER_H_
#define _CDISPLAYADAPTERHELPER_H_

#include <Windows.h>
#include "CNameShareMemory.h"


#define HOOKDLLNAME "GraphicsCaptureHook.dll"
#define BIT64 "64"

#define SHAREMEMNAME	"MySharedMem"
#define EVENTNAME		"MyEventName"

// [wupeng 2015/12/11] 判断指定进程的使用的显卡信息
class CDisplayAdapterHelper
{
public:
	CDisplayAdapterHelper(){ 
		ZeroMemory(m_cbVideoAdapterName, sizeof(m_cbVideoAdapterName));  
	}

	// 会使用阻塞当前线程
	CHAR* GetVideoAdapterName(){ return m_cbVideoAdapterName; }
	BOOL GetProcessAdapterInfoByName(LPSTR lpAppName, BOOL bInjectSafeMode);
	BOOL GetProcessAdapterInfoById(DWORD ProcessID, BOOL bInjectSafeMode);
private:
	BOOL AdjustDllforProcess(HANDLE hProcess, LPSTR lpDllPath);
	BOOL AdjustDllfor64bit(CHAR* cbTargetBuf, UINT uSize, LPSTR lpDllPath);
	BOOL InjectSpecificDllAndProcessByNameInner(LPSTR lpAppName, LPSTR lpDllName, BOOL bInjectSafeMode);
	BOOL InjectSpecificDllAndProcessByIdInner(DWORD ProcessID, LPSTR lpDllName, BOOL bInjectSafeMode);

private:
	CHAR m_cbVideoAdapterName[MAX_PATH];
};

#endif _CDISPLAYADAPTERHELPER_H_


