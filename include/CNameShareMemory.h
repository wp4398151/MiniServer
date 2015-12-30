#ifndef _CNAMESHAREMEMORY_H_
#define _CNAMESHAREMEMORY_H_

#include <Windows.h>

// [2015/12/11 wupeng] 
// warp share memory operation
class CNameShareMemory
{
public:
	CNameShareMemory() : m_hFileMapping(NULL), m_pSharedBuf(NULL){
	}
	BOOL Init(LPSTR lpSharedMemName, INT iMemSize, BOOL bIsCreate = FALSE);
	BOOL Close();
	CHAR* GetBuffer();
private:
	HANDLE m_hFileMapping;
	CHAR* m_pSharedBuf;
};

#endif //CNAMESHAREMEMORY_H_