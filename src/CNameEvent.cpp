#include "stdafx.h"
#include "CNameEvent.h"
#include <assert.h>

BOOL CNameEvent::Init(LPSTR lpEventName, BOOL bIsCreate)
{
	assert(ISNOTNULL(lpEventName));
	if (ISNULL(lpEventName))
	{ 
		DOLOG("length of Event Name Can't be 0 ");
		return FALSE;
	}
	
	if (ISZERO(strlen(lpEventName)))
	{
		DOLOG("length of Event Name Can't be 0 ");
		return FALSE;
	}

	if (ISNOZERO(strcpy_s(m_cbEventName, _countof(m_cbEventName), lpEventName)))
	{
		DOLOG("copy event name failed !");
		return FALSE;
	}

	if (bIsCreate)
	{
		return this->CreateEvent();
	}
	else
	{
		return this->OpenEvent();
	}
}

BOOL CNameEvent::OpenEvent()
{
	assert(ISNULL(m_hEvent));
	if (ISNOTNULL(m_hEvent))
	{
		DOLOG("the Event already opened!");
		return FALSE;
	}
	
	m_hEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, m_cbEventName);
	
	assert(ISNOTNULL(m_hEvent));
	if (ISNULL(m_hEvent))
	{
		DOLOG("open Event failed! " + GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL CNameEvent::CreateEvent()
{
	assert(ISNULL(m_hEvent));
	if (ISNOTNULL(m_hEvent))
	{
		DOLOG("the Event already created!");
		return FALSE;
	}

	m_hEvent = CreateEventA(NULL,
							TRUE,
							FALSE,
							m_cbEventName);
	assert(ISNOTNULL(m_hEvent));
	if (ISNULL(m_hEvent))
	{
		DOLOG("Create Event Failed " + GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL CNameEvent::Close()
{
	if (ISNOTNULL(m_hEvent))
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	ZeroMemory(m_cbEventName, sizeof(m_cbEventName));
	return TRUE;
}

BOOL CNameEvent::Wait()
{
	assert(ISNOTNULL(m_hEvent));
	if (ISNULL(m_hEvent))
	{
		DOLOG("can't wait null event handle!");
		return FALSE;
	}
	DWORD retCode = 0;
	retCode = WaitForSingleObject(m_hEvent, BLOCKOUTTIMEMSEC);

	BOOL isOK = FALSE;
	switch (retCode)
	{
	case WAIT_FAILED:
		DOLOG("wait event failed! " + GetLastError());
		isOK = FALSE;
		break;
	case WAIT_OBJECT_0:
		isOK = TRUE;
		break;
	case WAIT_TIMEOUT:
		DOLOG(" waitforSingleObject Timeout !");
		isOK = FALSE;
	default:
		DOLOG("waitforSingleObject unknown conidtion ! " + GetLastError());
		isOK = FALSE;
	};
	return isOK;
}

BOOL CNameEvent::Fire()
{
	assert(ISNOTNULL(m_hEvent));
	if (ISNULL(m_hEvent))
	{
		DOLOG("can't Fire null event handle!");
		return FALSE;
	}
	BOOL bResult = TRUE;
	
	bResult = SetEvent(m_hEvent);
	if (ISNOTTRUE(bResult))
	{
		DOLOG("SetEvent Failed!" + GetLastError());
		bResult = FALSE;
	}
	return bResult;
}
