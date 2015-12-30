#ifndef _CNAMEEVENT_H_
#define _CNAMEEVENT_H_

#include <Windows.h>
#include "Helper.h"

// 等待Event最大时长
#define BLOCKOUTTIMEMSEC 10*1000

// [2015/12/11 wupeng] 
// 用来同步的Event对象
class CNameEvent
{
public:
	CNameEvent() : m_hEvent(NULL)
	{ 
		ZeroMemory(m_cbEventName, sizeof(m_cbEventName));
	}

	~CNameEvent() 
	{ 
		if (m_hEvent != NULL) 
			CloseHandle(m_hEvent); 
	}

	CNameEvent(const CNameEvent &rNameEvent){ m_hEvent = rNameEvent.m_hEvent; }
	CNameEvent& operator=(const CNameEvent &rNameEvent)
	{ 
		m_hEvent = rNameEvent.m_hEvent;
		return *this;
	}

public:
	BOOL Init(LPSTR lpEventName, BOOL bIsCreate = FALSE);
	BOOL Close();
	BOOL Wait();
	BOOL Fire();

private:
	BOOL OpenEvent();
	BOOL CreateEvent();

	HANDLE m_hEvent;
	CHAR m_cbEventName[MAX_PATH];
};

#endif //CNAMEEVENT_H_