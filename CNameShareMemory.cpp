#include "stdafx.h"
#include "CNameShareMemory.h"
#include "Helper.h"
#include <assert.h>

BOOL CNameShareMemory::Init(LPSTR lpSharedMemName, INT iMemSize, BOOL bIsCreate)
{
	assert(ISNOTNULL(lpSharedMemName));
	if (ISNULL(lpSharedMemName))
	{
		DOLOG("memory name can't be null!");
		return FALSE;
	}

	if (bIsCreate)
	{
		m_hFileMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, iMemSize, lpSharedMemName);
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			DOLOG(lpSharedMemName + " already exist !");
			Close();
			return FALSE;
		}
	}
	else
	{
		m_hFileMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, lpSharedMemName);
	}

	if (ISNULL(m_hFileMapping))
	{
		DOLOG("get FileMapping Failed ! " + GetLastError())
		return FALSE;
	}
	
	m_pSharedBuf = (CHAR*)MapViewOfFile(m_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (ISNULL(m_pSharedBuf))
	{
		DOLOG("MapViewOfFile Failed ! " + GetLastError());
		Close();
		return FALSE;
	}
	
	return TRUE;
}

BOOL CNameShareMemory::Close()
{
	if (ISNOTNULL(m_pSharedBuf))
	{
		if(ISNOTTRUE(UnmapViewOfFile(m_pSharedBuf)))
		{
			DOLOG("Unmapped Memory failed !" + GetLastError());
		}
		m_pSharedBuf = NULL;
	}
	if (ISNOTNULL(m_hFileMapping))
	{
		if (ISNOTTRUE(CloseHandle(m_hFileMapping)))
		{
			DOLOG("Close Handle Error" + GetLastError());
		}
		m_hFileMapping = NULL;
	}
	return TRUE;
}

CHAR* CNameShareMemory::GetBuffer()
{
	assert(ISNOTNULL(m_pSharedBuf));
	if (ISNULL(m_pSharedBuf))
	{
		DOLOG("shared membuf is empty" + GetLastError())
		return FALSE;
	}
	return m_pSharedBuf;
}