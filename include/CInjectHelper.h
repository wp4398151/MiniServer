#ifndef _CINJECTHELPER_H_
#define _CINJECTHELPER_H_

#include <Windows.h>

// [2015/12/11 wupeng] 
// Inject the specific API 
class CInjectHelper
{
public:
	CInjectHelper()
	{
		ZeroMemory(m_cbOriginASMData, sizeof(m_cbOriginASMData));
	}
		
private:
	BYTE m_cbOriginASMData[32];
	FARPROC func;
	FARPROC hookFunc;
	bool bHooked;
	bool b64bitJump;
	bool bAttemptedBounce;
	LPVOID bounceAddress;
};

#endif //CINJECTHELPER_H_