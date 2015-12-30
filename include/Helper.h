#ifndef _HELPER_H_
#define _HELPER_H_

#include <Windows.h>
#include <sstream>
#include <string>
#include <process.h>
#include <time.h>

using namespace std;

//#define POPSHOWBOX	// 是否弹框
#define USEDOLOG	// 调试信息

// [2015/12/11 wupeng] 
// Convinient to cat string
class SstringEx
{
public:
	SstringEx& operator+(const int &v){
		m_ss << v;
		return *this;
	}

	SstringEx& operator+(char* v){
		m_ss << v;
		return *this;
	}
	SstringEx& operator+(const char* v){
		m_ss << v;
		return *this;
	}

	SstringEx& operator+(double &v){
		m_ss << v;
		return *this;
	}
public:
	std::stringstream m_ss;
};

// [2015/12/11 wupeng] 
// Debug log help macro
#ifdef USEDOLOG
#ifdef POPSHOWBOX
	#define DOLOG(msg) {		\
		SstringEx tempMsg;		\
		MessageBoxA(NULL, (tempMsg +"wupeng::" + msg + " (" + __FILE__ + ":" + __LINE__ + ")\n").m_ss.str().c_str(), "Infomation", MB_OK); \
	}
#else
	#define DOLOG(msg) {	\
		SstringEx tempMsg;	\
		OutputDebugStringA((tempMsg + "wupeng::" + msg + " (" + __FILE__ + ":" + __LINE__ + ")\n").m_ss.str().c_str()); \
	}
#endif
#else // USEDOLOG
	#define DOLOG(msg)
#endif // USEDOLOG

#define SAFE_DELETE(ptr) if(ptr != NULL){delete ptr; ptr = NULL; }

#define NULL_RET(exp, msg, ret) if(NULL == (exp)){ DOLOG(msg); ret; }
#define FALSE_RET(exp, msg, ret) if(FALSE == (exp)){ DOLOG(msg); ret; }
#define TRUE_RET(exp, msg, ret) if(TRUE == (exp)){ DOLOG(msg); ret; }

#define ISFALSE(x) ((x) == FALSE)
#define ISNOTTRUE(x) ((x) != TRUE)
#define ISTRUE(x) ((x) == TRUE)
#define ISZERO(x) ((x) == 0)
#define ISNOZERO(x) ((x) != 0)
#define ISNULL(x) ((x) == NULL)
#define ISNOTNULL(x) ((x) != NULL)

DWORD GetSpecificProcIDByName(LPSTR lpName);
DWORD GetSpecificProcIDByNameEx(LPSTR lpName);
BOOL EnableDebugPrivilege(BOOL bEnableDebugPrivilege);

#include <string>
#include <process.h>
#include <time.h>
using namespace std;

#define ISINRANGE(var, minor, greator) (( (var) >= (minor) && (var) <= (greator) )? true : false)

#define CHECKNULLALARAM1_RET(val, msg, retstm) if( NULL == (val)){ DOERR(msg); retstm; }
#define CHECKNULL1_RET(val, retstm) if(NULL == (val)){ DOERR(msg); retstm; }

////////////////////////////////////////////////////
// 字符串帮助函数
// Wupeng 
////////////////////////////////////////////////////
const int kConvertCodepageCacheLen = 2048;
// Windows可用
BOOL ConvertWidechar2UTF8(const wstring &rWStr, string &rStr);
BOOL ConvertWidechar2ANSI(const wstring &rWStr, string &rStr);
BOOL ConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL ConvertANSI2Widechar(const string& rStr, wstring& rWStr);
// CRT 跨平台，如果是linux下列都可用.
// 如果是windows，用上面的函数。winCRT没有实现UTF8
BOOL CRTConvertANSI2Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertWidechar2ANSI(const wstring& rWStr, string& rStr);
BOOL CRTConvertWidechar2UTF8(const wstring& rWStr, string& rStr);


////////////////////////////////////////////////////
// 判断是否是小端序
// Wupeng 
////////////////////////////////////////////////////
bool IsByteLittleEndian(bool &bIsLittleEndian);

////////////////////////////////////////////////////
// 得到当前执行程序的路径
// Wupeng 
////////////////////////////////////////////////////
bool GetAppPath(string& strPath);

////////////////////////////////////////////////////
// 替换所有匹配的字符串
// Wupeng 
////////////////////////////////////////////////////
string &ReplaceAll(string& rSrcString, string& rSearchString, string& rReplaceString);

////////////////////////////////////////////////////
// 使用原子操作来实现锁
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
// 线程类
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
	if (0 == WaitForSingleObject(m_hThread, 0))
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

template<typename T>
BOOL LiteThread<T>::start()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, T::RunFunc, m_pInst, 0, &m_threadID);
	if (0 == m_hThread)
	{
		return FALSE;
	}
	return TRUE;
}

template<typename T>
class CLiteSingleton{
public:
	static T& Instance(){
		static T s_Inst;
		return s_Inst;
	}
};

////////////////////////////////////////////////////
// 时间管理
// Wupeng 
////////////////////////////////////////////////////
class TimeManager
{
public:
	// 得到未来时间
	static time_t GetOverDayTime(time_t curTime);
	static time_t GetOverDayTime();
	static void PrintTime(time_t atime);
};

// [2015/12/17 wupeng] 
BOOL InsertStrInStrSpecifyPosA(LPSTR lpTarBuf, UINT cBufLen, LPSTR lpSrcStr, LPSTR lpInsertStr, UINT iPos);

#endif //HELPER_H_
