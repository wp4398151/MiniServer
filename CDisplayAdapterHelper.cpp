#include "stdafx.h"
#include <assert.h>
#include "injectHelper.h"
#include "CDisplayAdapterHelper.h"
#include "Helper.h"
#include "CNameEvent.h"

BOOL CDisplayAdapterHelper::GetProcessAdapterInfoByName(LPSTR lpAppName, BOOL bInjectSafeMode)
{
	return InjectSpecificDllAndProcessByNameInner(lpAppName, HOOKDLLNAME, bInjectSafeMode);
}

BOOL CDisplayAdapterHelper::GetProcessAdapterInfoById(DWORD ProcessID, BOOL bInjectSafeMode)
{
	return InjectSpecificDllAndProcessByIdInner(ProcessID, HOOKDLLNAME, bInjectSafeMode);
}

BOOL CDisplayAdapterHelper::InjectSpecificDllAndProcessByNameInner(LPSTR lpAppName, LPSTR lpDllName, BOOL bInjectSafeMode)
{
	assert(ISNOTNULL(lpAppName));
	if (ISNULL(lpAppName))
	{
		DOLOG("AppName should not to be null !");
		return FALSE;
	}

	DWORD procid = GetSpecificProcIDByNameEx(lpAppName);

	if (ISZERO(procid))
	{
		DOLOG("GetSpecificProcIDByNameEx Failed");
		return FALSE;
	}

	return InjectSpecificDllAndProcessByIdInner(procid,  lpDllName,  bInjectSafeMode);
}

BOOL CDisplayAdapterHelper::AdjustDllforProcess(HANDLE hProcess, LPSTR lpDllPath)
{
	BOOL bIs32Bit = FALSE;

//#ifdef _WIN64
//	BOOL myselfBitWide = TRUE;
//#else
//	BOOL mySelfBitWide = FALSE; 
//#endif
//	BOOL bSelfIsWow64 = FALSE;
//	IsWow64Process(GetCurrentProcess(), &bSelfIsWow64);
	IsWow64Process(hProcess, &bIs32Bit);

	if (!bIs32Bit)
	{
		CHAR tempStrBuf[MAX_PATH] = { 0 };

		if (ISTRUE(AdjustDllfor64bit(tempStrBuf, MAX_PATH, lpDllPath)))
		{
			memcpy(lpDllPath, tempStrBuf, strlen(tempStrBuf) + 1);
		}
	}
	return TRUE;
}

BOOL CDisplayAdapterHelper::AdjustDllfor64bit(CHAR* cbTargetBuf, UINT uSize, LPSTR lpDllPath)
{
	if (ISNULL(cbTargetBuf))
	{
		DOLOG("cbTargetBuf Can't be null");
		return FALSE;
	}
	
	if (ISNULL(lpDllPath))
	{
		DOLOG("lpDllpath Can't be null !");
		return FALSE;
	}
	
	LPSTR lpDotPos = strrchr(lpDllPath, '.');
	UINT uPos = lpDotPos - lpDllPath;

	return InsertStrInStrSpecifyPosA(cbTargetBuf, uSize, lpDllPath, BIT64, uPos);
}

BOOL CDisplayAdapterHelper::InjectSpecificDllAndProcessByIdInner(DWORD dwProcessID, LPSTR lpDllName, BOOL bInjectSafeMode)
{
#if defined _M_X64 && _MSC_VER == 1800
	//workaround AVX2 bug in VS2013, http://connect.microsoft.com/VisualStudio/feedback/details/811093
	_set_FMA3_enable(0);
#endif
	
	assert(dwProcessID != 0);
	if (ISZERO(dwProcessID))
	{
		DOLOG("id of target process can't be zero !");
		return FALSE;
	}
	assert(ISNOTNULL(lpDllName));
	if(ISNULL(lpDllName))
	{
		DOLOG("name of dll can't not be null !");
		return FALSE;
	}

	CHAR cbDllPath[MAX_PATH] = { 0 };
	//if (ISZERO(GetCurrentDirectoryA(dirLen, cbDllPath)))
	if(ISZERO(GetModuleFileNameA(GetModuleHandle(NULL), cbDllPath, MAX_PATH)))
	{
		DOLOG("GetCurrentDirectory Failed ! " + GetLastError());
		return FALSE;
	}
	CHAR* lpFullPath = strrchr(cbDllPath, '\\');
	lpFullPath[1] = '\0';
	UINT dirLen = strlen(cbDllPath);
	const size_t fileNameLen = strlen(lpDllName);
	size_t len = dirLen + fileNameLen + 1;
	cbDllPath[dirLen - 1] = '\\';
	strncpy_s(cbDllPath + dirLen, len - dirLen, lpDllName, fileNameLen);

	BOOL result = TRUE;
	LoadSeDebugPrivilege();
	
	CNameEvent nameEvent;
	nameEvent.Init(EVENTNAME, TRUE);
	CNameShareMemory shareMemInst;

	shareMemInst.Init(SHAREMEMNAME, MAX_PATH, TRUE);
	char* lpInfo = shareMemInst.GetBuffer();
	ZeroMemory(lpInfo, MAX_PATH);

	if (!bInjectSafeMode)
	{
		OPPROC pOpenProcess;
		HANDLE hProcess;
		char pOPStr[12];
		int i;

		memcpy(pOPStr, "NpflUvhel{x", 12); //OpenProcess obfuscated
		for (i = 0; i<11; i++) pOPStr[i] ^= i ^ 1;

		pOpenProcess = (OPPROC)GetProcAddress(GetModuleHandle(TEXT("KERNEL32")), pOPStr);

		if (ISNULL(pOpenProcess))
		{
			DOLOG("GetProcAddress failed ! " + GetLastError());
			return FALSE;
		}

		hProcess = (*pOpenProcess)(PROCESS_ALL_ACCESS, FALSE, dwProcessID);

		if (ISNULL(hProcess))
		{
			DOLOG("OpenProcess Failed ! " + GetLastError());
			return FALSE;
		}
		if (ISFALSE(AdjustDllforProcess(hProcess, cbDllPath)))
		{
			DOLOG("AdjustDllforProcess Failed ! " + GetLastError());
			return FALSE;
		}

		if (ISNOTTRUE(InjectLibraryA(hProcess, cbDllPath, (DWORD)(len - 1))))
		{
			result = FALSE;
			DOLOG("Inject Library failed ! " + GetLastError());
		}

		CloseHandle(hProcess);
	}
	else
	{

#ifdef _WIN64
		CHAR cbTempDllPath[MAX_PATH] = { 0 };
		if (ISTRUE(AdjustDllfor64bit(cbTempDllPath, MAX_PATH, cbDllPath)))
		{
			memcpy(cbDllPath, cbTempDllPath, strlen(cbTempDllPath) + 1);
		}
#endif
		if (!InjectLibrarySafeA(dwProcessID, cbDllPath, (DWORD)(len - 1)))
		{
			DOLOG("InjectLibrarySafeA failed ! "+ GetLastError());
			result = FALSE;
		}
	}

	if(ISTRUE(nameEvent.Wait()))
	{
		lpInfo = shareMemInst.GetBuffer();
		if (ISNOTNULL(lpInfo))
		{
			DOLOG(" 得到的信息是>>>>>>" + lpInfo);
			strcpy(m_cbVideoAdapterName, lpInfo);
		}
	}
	nameEvent.Close();
	shareMemInst.Close();

	return result ;
}
