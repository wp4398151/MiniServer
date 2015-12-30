#include "stdafx.h"
#include "Helper.h"
#include <windows.h>
#include <Psapi.h>

#undef UNICODE
#include <Tlhelp32.h>
#undef UNICODE

// [2015/12/17 wupeng] 
// test
//char tempbuf[max_path] = { 0 };
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", 2);
//dolog(tempbuf);
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", 50);
//dolog(tempbuf);
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", strlen("my.dll"));
//dolog(tempbuf);
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", 0);
//dolog(tempbuf);
BOOL InsertStrInStrSpecifyPosA(LPSTR lpTarBuf, UINT cBufLen, LPSTR lpSrcStr, LPSTR lpInsertStr, UINT iPos)
{
	if (ISNULL(lpTarBuf)){
		DOLOG("lpTarBuf NULL !");
		return FALSE;
	}

	if (ISNULL(lpSrcStr)){
		DOLOG("lpSrcStr NULL !");
		return FALSE;
	}

	if (ISNULL(lpInsertStr)){
		DOLOG("lpInsertStr NULL !");
		return FALSE;
	}
	
	UINT cInsertStrLen = (UINT)strlen(lpInsertStr);
	UINT cSrcStrLen = (UINT)strlen(lpSrcStr);

	if (iPos > cSrcStrLen)
	{
		DOLOG("iPos stack overflow !");
		return FALSE;
	}
	ZeroMemory(lpTarBuf, cBufLen);
	UINT cTotalNeed = cSrcStrLen + cInsertStrLen  + 1;

	if (cTotalNeed > cBufLen)
	{
		DOLOG("not enough for fill the result string!");
		return FALSE;
	}
	
	UINT uCurPos = 0;
	memcpy(lpTarBuf, lpSrcStr, iPos);
	uCurPos += iPos;
	memcpy(lpTarBuf + uCurPos, lpInsertStr, cInsertStrLen);
	uCurPos += cInsertStrLen;
	memcpy(lpTarBuf + uCurPos, lpSrcStr + iPos, cSrcStrLen - iPos);
		
	return TRUE;
}

// [2015/12/15 wupeng] 
// if failed return 0, success return the Process ID
DWORD GetSpecificProcIDByName(LPSTR lpName)
{
	DWORD resProcessID = 0;
	if (ISNULL(lpName))
	{ 
		DOLOG("Process Name can't be null");
		return 0;
	}

	if (ISFALSE(EnableDebugPrivilege(TRUE)))
	{
		DOLOG("EnableDebugPrivilege Failed !");
		return 0;
	}

	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32, sizeof(pe32));

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		DOLOG(" CreateToolhelp32Snapshot Failed ！ \n" + GetLastError());
		return 0;
	}

	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		DOLOG(" >>" + pe32.szExeFile + " : " + pe32.th32ProcessID);
		if (strcmp(pe32.szExeFile, lpName) == 0)
		{
			resProcessID = pe32.th32ProcessID;
		}
		bMore = Process32Next(hProcessSnap, &pe32);
	}

	// 不要忘记清除掉snapshot对象  
	CloseHandle(hProcessSnap);
	return resProcessID;
}

// [2015/12/15 wupeng] 
// if failed return 0, success return the Process ID
DWORD GetSpecificProcIDByNameEx(LPSTR lpName)
{
	if (ISNULL(lpName))
	{ 
		DOLOG("Process Name can't be null");
		return 0;
	}
	
	if (ISFALSE(EnableDebugPrivilege(TRUE)))
	{
		DOLOG("EnableDebugPrivilege Failed !");
		return 0;
	}

	DWORD allProcessIDs[2048] = { 0 };
	DWORD cbNeed = 0;

	if (ISZERO(EnumProcesses(allProcessIDs, sizeof(allProcessIDs), &cbNeed)))
	{
		DOLOG("EnumProcess Failed ! " + GetLastError());
		return 0;
	}

	HANDLE hProcess = NULL;
	CHAR szProcessName[MAX_PATH] = { 0 };
	for (DWORD i = 0; i < cbNeed; i++)
	{ 
		if (ISZERO(allProcessIDs[i]))
		{
			continue;
		}

		ZeroMemory(szProcessName, sizeof(szProcessName));
		hProcess = OpenProcess(
			PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE,
				allProcessIDs[i]);
		if (ISNOTNULL(hProcess))
		{
			if (ISNOZERO(GetModuleBaseNameA(hProcess, NULL, szProcessName,
				sizeof(szProcessName))))
			{
				DOLOG(" >>" + szProcessName + " : " + allProcessIDs[i]);
				if (ISZERO(strcmp(szProcessName, lpName)))
				{
					DOLOG("Found the Process " + lpName);
					CloseHandle(hProcess);
					return allProcessIDs[i];
				}
			}
			else
			{
				DOLOG("GetmoudleBaseName Failed ! " + GetLastError());
			}
			CloseHandle(hProcess);
		}
		else
		{
			DOLOG("OpenProcess Failed ! " + allProcessIDs[i] + " LastError:" + GetLastError());
		}
	}
	
	DOLOG("can't found the specify name process Failed ! ");
	return 0;
}

// [2015/12/15 wupeng] 
BOOL EnableDebugPrivilege(BOOL bEnableDebugPrivilege)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		DOLOG("OpenProcessToken Failed "+GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		DOLOG("LookupPrivilegeValue Failed "+ GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnableDebugPrivilege)
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		tp.Privileges[0].Attributes = 0;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		DOLOG("AdjustTokenPrivileges Failed "+ GetLastError());

		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);

	return TRUE;
}
