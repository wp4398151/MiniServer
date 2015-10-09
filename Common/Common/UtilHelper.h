#ifndef _UTILHELPER_H_
#define _UTILHELPER_H_

#include <Windows.h>
#include <string>
#include <process.h>
#include <time.h>
using namespace std;

#define ISINRANGE(var, minor, greator) (( (var) >= (minor) && (var) <= (greator) )? true : false)

#define CHECKNULLALARAM1_RET(val, msg, retstm) if( NULL == (val)){ DOERR(msg); retstm; }
#define CHECKNULL1_RET(val, retstm) if(NULL == (val)){ DOERR(msg); retstm; }


const int kConvertCodepageCacheLen = 2048;
// Windows����
BOOL ConvertWidechar2UTF8(const wstring &rWStr, string &rStr);
BOOL ConvertWidechar2ANSI(const wstring &rWStr, string &rStr);
BOOL ConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL ConvertANSI2Widechar(const string& rStr, wstring& rWStr);
// CRT ��ƽ̨�������linux���ж�����.
// �����windows��������ĺ�����winCRTû��ʵ��UTF8
BOOL CRTConvertANSI2Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertWidechar2ANSI(const wstring& rWStr, string& rStr);
BOOL CRTConvertWidechar2UTF8(const wstring& rWStr, string& rStr);

////////////////////////////////////////////////////
// �ж��Ƿ���С����
// Wupeng 
////////////////////////////////////////////////////
bool IsByteLittleEndian(bool &bIsLittleEndian);

////////////////////////////////////////////////////
// �õ���ǰִ�г����·��
// Wupeng 
////////////////////////////////////////////////////
bool GetAppPath(string& strPath);

////////////////////////////////////////////////////
// �滻����ƥ����ַ���
// Wupeng 
////////////////////////////////////////////////////
string &ReplaceAll(string& rSrcString, string& rSearchString, string& rReplaceString);

////////////////////////////////////////////////////
// ʹ��ԭ�Ӳ�����ʵ����
// Wupeng 
////////////////////////////////////////////////////
class LiteUnpendLock
{
public:
	LiteUnpendLock() : m_lock(0) {}
	~LiteUnpendLock(){}

	bool Lock()
	{
		return InterlockedExchange(&m_lock, 1) == 0;
	}

	void Unlock()
	{
		InterlockedExchange(&m_lock, 0);
	}

private:
	volatile unsigned int m_lock;
};

class LiteUnpendLockEx
{
public:

	LiteUnpendLockEx() : m_lock(0) {}
	~LiteUnpendLockEx(){}

	bool GetLock()
	{
		return InterlockedCompareExchange(&m_lock, 1, 0) == 0;
	}

	void Unlock()
	{
		InterlockedExchange(&m_lock, 0);
	}

private:
	volatile unsigned int m_lock;
};


////////////////////////////////////////////////////
// �򵥵��߳���
// Wupeng 
////////////////////////////////////////////////////
template<typename T>
class LiteThread
{
public:
	LiteThread(T *pInst);
	~LiteThread();

	BOOL close();
	BOOL start();
		
protected:
	T *m_pInst;
	HANDLE m_hThread;
	unsigned int m_threadID;
};

template<typename T>
LiteThread<T>::LiteThread(T *pInst)
{
	m_pInst = pInst;
	m_hThread = 0;
	m_threadID = 0;
}

template<typename T>
LiteThread<T>::~LiteThread()
{
	m_pInst = NULL;
	m_hThread = 0;
	m_threadID = 0;
}

template<typename T>
BOOL LiteThread<T>::close()
{
	if (0 == WaitForSingleObject(m_hThread , 0))
	{
		if (CloseHandle(m_hThread))
		{
			m_pInst = NULL;
			m_hThread = 0;
			m_threadID = 0;
			return TRUE;
		}
	}
	return FALSE;
}

// T��Ҫ��һ����̬����: RunFunc
// unsigned __stdcall RunFunc(void* inst)
// {
//		T* pInst = inst;
//		...
// }
template<typename T>
BOOL LiteThread<T>::start()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, T::RunFunc, m_pInst, 0, &m_threadID);
	if (0 == m_hThread )
	{
		return FALSE;
	}
	return TRUE;
}

/*
LiteThreadʹ��
class LogicModel
{
public:
	static unsigned int WINAPI RunFunc(void * pVoid)
	{
		LogicModel* pOneInst = (LogicModel*)pVoid;
		pOneInst->alert();

		return 0;	// _endthreadex(0);
	}

	void alert(){ DOERR("OK!"); }
};

int _tmain(int argc, _TCHAR* argv[])
{
	//WholeServerSelect();
	LogicModel model;
	LiteThread<LogicModel> logicModel(&model);
	logicModel.start();

	while (!logicModel.close())
	{
		Sleep(1);
	}
	return 0;
}
*/

template<typename T>
class CLiteSingleton{
public:
	static T& Instance(){
		static T s_Inst;
		return s_Inst;
	}
};

class TimeManager
{
public:
	// �õ�δ��ʱ��
	static time_t GetOverDayTime(time_t curTime);
	static time_t GetOverDayTime();
	static void PrintTime(time_t atime);
};

#endif //_UTILHELPER_H_