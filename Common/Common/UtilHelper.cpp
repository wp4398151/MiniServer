#include "stdafx.h"

#include "UtilHelper.h"
#include "WpLogHelper.h"
#include <Windows.h>
#include <iostream>

using namespace std;
using namespace WpHelper;

// �жϴ�С����
// ���ж� С���ǵ͵͸߸ߣ����ĵ�λ���ڵ��ڴ棬��λ���ڸ��ڴ�
bool IsByteLittleEndian(bool &bIsLittleEndian)
{
	union
	{
		short value;
		char union_bytes[sizeof(short)];
	} test;
	test.value = 0x0102;
	if ((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2))
	{
		bIsLittleEndian = false;
		return true;
	}
	else if ((test.union_bytes[0] == 2) && (test.union_bytes[1] == 1))
	{
		bIsLittleEndian = true;
		return true;
	}
	else
	{
		return false;
	}
}

// �õ���ǰ�����·��
bool GetAppPath(string& strPath)
{
	char modulePath[MAX_PATH];
	if (0 == GetModuleFileNameA(NULL, modulePath, MAX_PATH))
	{
		DOERR("�õ���ǰ");
		return false;
	}
	strPath = modulePath;
	strPath  = strPath.substr(0, strPath.find_last_of('\\'));
	return true;
}

string &ReplaceAll(string& rSrcString, string& rSearchString, string& rReplaceString)
{
	string::size_type pos = 0;
	while ((pos = rSrcString.find(rSearchString, pos)) != rSrcString.npos) {
		rSrcString.replace(pos, rSearchString.size(), rReplaceString);
		pos += rReplaceString.length();
	}
	return rSrcString;
}

////////////////////////////////////////////////////
// widechar Unicode ת����UTF8, Ŀǰֻ֧��
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertWidechar2UTF8(const wstring &rWStr, string &rStr)
{
	int iReceiveSize = 0;
	iReceiveSize = WideCharToMultiByte(CP_UTF8,		// ���ֽڵĴ���ҳ
		0,				// ����ת��ϸ�ڣ�0������
		rWStr.c_str(),	// ��Ҫת���Ŀ��ֽ��ַ���
		-1,				// ���ֽ��ַ����ĳ���
		NULL,			// ���ת��֮����ַ����Ļ���
		0,				// ����Ĵ�С
		NULL,			// ����ת����Ĭ���ַ�
		NULL);			// �Ƿ���ȫת���ɹ�
	if (0 == iReceiveSize)
	{
		int icode = GetLastError();
		DOERR("�����ַ���ת���ֽ���ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	char bufChar[kConvertCodepageCacheLen] = { 0 };

	if (0 == WideCharToMultiByte(
		CP_UTF8,	// ���ֽڵĴ���ҳ
		0,			// ����ת��ϸ�ڣ�0������
		rWStr.c_str(),			// ��Ҫת���Ŀ��ֽ��ַ���
		-1,// ���ֽ��ַ����ĳ���
		bufChar,// ���ת��֮����ַ����Ļ���
		iReceiveSize,// ����Ĵ�С
		NULL,// ����ת����Ĭ���ַ�
		NULL// �Ƿ���ȫת���ɹ�
		))
	{
		DOERR("���ַ�ת��UTF8����");
		return FALSE;
	}
	rStr = bufChar;
	return TRUE;
}

////////////////////////////////////////////////////
// widechar Unicode ת����ANSI
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertWidechar2ANSI(const wstring &rWStr, string &rStr)
{
	int iReceiveSize = 0;
	iReceiveSize = WideCharToMultiByte(CP_ACP,		// ���ֽڵĴ���ҳ
							0,				// ����ת��ϸ�ڣ�0������
							rWStr.c_str(),	// ��Ҫת���Ŀ��ֽ��ַ���
							-1,				// ���ֽ��ַ����ĳ���
							NULL,			// ���ת��֮����ַ����Ļ���
							0,				// ����Ĵ�С
							NULL,			// ����ת����Ĭ���ַ�
							NULL);			// �Ƿ���ȫת���ɹ�
	if (0 == iReceiveSize)
	{
		DOERR("�����ַ���ת���ֽ���ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	CHAR bufChar[kConvertCodepageCacheLen] = { 0 };

	if (0 == WideCharToMultiByte(
		CP_UTF8,				// ���ֽڵĴ���ҳ
		0,						// ����ת��ϸ�ڣ�0������
		rWStr.c_str(),			// ��Ҫת���Ŀ��ֽ��ַ���
		-1,						// ���ֽ��ַ����ĳ���
		bufChar,				// ���ת��֮����ַ����Ļ���
		iReceiveSize,			// ����Ĵ�С
		NULL,					// ����ת����Ĭ���ַ�
		NULL					// �Ƿ���ȫת���ɹ�
		))
	{
		DOERR("���ַ�ת��UTF8����");
		return FALSE;
	}
	rStr = bufChar;
	return TRUE;
}

////////////////////////////////////////////////////
// UTF8 ת����WideChar Unicode
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertUTF82Widechar(const string& rStr, wstring& rWStr)
{
	int iReceiveSize = 0;
	iReceiveSize = MultiByteToWideChar(CP_UTF8,		// ���ֽڵĴ���ҳ
										0,				// ����ת��ϸ�ڣ�0������
										rStr.c_str(),	// ��Ҫת���Ŀ��ֽ��ַ���
										-1,				// ���ֽ��ַ����ĳ���
										NULL,			// ���ת��֮����ַ����Ļ���
										0);				// ����Ĵ�С
	if (0 == iReceiveSize)
	{
		DOERR("�����ַ���ת���ֽ���ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	WCHAR bufChar[kConvertCodepageCacheLen*sizeof(WCHAR)] = { 0 };

	if (0 == MultiByteToWideChar(
		CP_UTF8,	// ���ֽڵĴ���ҳ
		0,			// ����ת��ϸ�ڣ�0������
		rStr.c_str(),			// ��Ҫת���Ŀ��ֽ��ַ���
		-1,// ���ֽ��ַ����ĳ���
		bufChar,// ���ת��֮����ַ����Ļ���
		kConvertCodepageCacheLen*sizeof(WCHAR)))
	{
		DOERR("ANSIת�����ַ�����");
		return FALSE;
	}
	rWStr = bufChar;

	return TRUE;
}

////////////////////////////////////////////////////
// ANSIת����WideChar Unicode
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertANSI2Widechar(const string& rStr, wstring& rWStr)
{
	int iReceiveSize = 0;
	iReceiveSize = MultiByteToWideChar(CP_ACP,		// ���ֽڵĴ���ҳ
		0,				// ����ת��ϸ�ڣ�0������
		rStr.c_str(),	// ��Ҫת���Ŀ��ֽ��ַ���
		-1,				// ���ֽ��ַ����ĳ���
		NULL,			// ���ת��֮����ַ����Ļ���
		0);				// ����Ĵ�С
	if (0 == iReceiveSize)
	{
		DOERR("�����ַ���ת���ֽ���ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	WCHAR bufChar[kConvertCodepageCacheLen*sizeof(WCHAR)] = { 0 };

	if (0 == MultiByteToWideChar(
		CP_UTF8,	// ���ֽڵĴ���ҳ
		0,			// ����ת��ϸ�ڣ�0������
		rStr.c_str(),			// ��Ҫת���Ŀ��ֽ��ַ���
		-1,// ���ֽ��ַ����ĳ���
		bufChar,// ���ת��֮����ַ����Ļ���
		kConvertCodepageCacheLen*sizeof(WCHAR)))
	{
		DOERR("ANSIת�����ַ�����");
		return FALSE;
	}
	rWStr = bufChar;

	return TRUE;
}

////////////////////////////////////////////////////
// ANSIת����WideChar Unicode,ʹ��CRT,δʵ��
// Wupeng 
////////////////////////////////////////////////////
BOOL CRTConvertANSI2Widechar(const string& rStr, wstring& rWStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.ACP");
	char *szLocale = setlocale(LC_ALL, ".ACP");

	if ( NULL == szLocale )
	{
		DOERR("����ת��֮����ַ�������ʧ�� "+ szLocale );
		return FALSE;
	}

	int iReceiveSize = mbstowcs(NULL, rStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOERR("����ת��֮����ַ�������ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	wchar_t bufChar[kConvertCodepageCacheLen+1] = { 0 };
		
	mbstowcs(bufChar, rStr.c_str(), iReceiveSize + 1);
	rWStr = bufChar;
	return TRUE;
}

BOOL CRTConvertUTF82Widechar(const string& rStr, wstring& rWStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.UTF-8");
	char *szLocale = setlocale(LC_ALL, ".UTF-8");
	
	if ( NULL == szLocale )
	{
		DOERR("����ת��֮����ַ�������ʧ�� "+ szLocale );
		return FALSE;
	}

	int iReceiveSize = mbstowcs(NULL, rStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOERR("����ת��֮����ַ�������ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	wchar_t bufChar[kConvertCodepageCacheLen+1] = { 0 };
		
	mbstowcs(bufChar, rStr.c_str(), iReceiveSize + 1);
	rWStr = bufChar;
	return TRUE;
}

BOOL CRTConvertWidechar2UTF8(const wstring& rWStr, string& rStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.UTF-8");
	char *szLocale = setlocale(LC_ALL, ".UTF-8");
	if ( NULL == szLocale )
	{
		DOERR("����ת��֮����ַ�������ʧ�� "+ szLocale );
		return FALSE;
	}

	int iReceiveSize = wcstombs(NULL, rWStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOERR("����ת��֮����ַ�������ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	char bufChar[kConvertCodepageCacheLen+1] = { 0 };
	wcstombs(bufChar, rWStr.c_str(), iReceiveSize + 1);
	rStr = bufChar;
	return TRUE;
}

BOOL CRTConvertWidechar2ANSI(const wstring& rWStr, string& rStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.ACP");
	char *szLocale = setlocale(LC_ALL, ".ACP");
	
	if ( NULL == szLocale )
	{
		DOERR("����ת��֮����ַ�������ʧ�� "+ szLocale );
		return FALSE;
	}

	int iReceiveSize = wcstombs(NULL, rWStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOERR("����ת��֮����ַ�������ʧ��");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOERR("�ַ����ĳ��ȳ����˻���Ĵ�С");
		return FALSE;
	}
	char bufChar[kConvertCodepageCacheLen+1] = { 0 };
	wcstombs(bufChar, rWStr.c_str(), iReceiveSize + 1);
	rStr = bufChar;
	return TRUE;
}

/*
���Դ���
//	wstring wstr = L"12345";
//	string str = "12345";
//	string strTar;
//	wstring wstrTar;
//	ConvertWidechar2UTF8(wstr, strTar);
//	ConvertWidechar2ANSI(wstr, strTar);
//	ConvertUTF82Widechar(str, wstrTar);
//	ConvertANSI2Widechar(str, wstrTar);

//	CRTConvertWidechar2UTF8(wstr, strTar);
//	CRTConvertWidechar2ANSI(wstr, strTar);
//	CRTConvertANSI2Widechar(str, wstrTar);
//	CRTConvertANSI2Widechar(str, wstrTar);
//	CRTConvertUTF82Widechar(str, wstrTar);
*/

void TimeManager::PrintTime(time_t atime)
{
	tm timeStruct;
	char buff[32];
	localtime_s(&timeStruct, &atime);
	asctime_s(buff, &timeStruct);
	cout << buff << endl;
}

time_t TimeManager::GetOverDayTime()
{
	time_t curTime;
	time(&curTime);
	tm timeStruct;
	localtime_s(&timeStruct, &curTime);
	timeStruct.tm_hour = 0;
	timeStruct.tm_min = 0;
	timeStruct.tm_sec = 0;
	timeStruct.tm_mday += 1;
	return mktime(&timeStruct);
}

time_t TimeManager::GetOverDayTime(time_t curTime)
{
	tm timeStruct;
	localtime_s(&timeStruct, &curTime);
	timeStruct.tm_hour = 0;
	timeStruct.tm_min = 0;
	timeStruct.tm_sec = 0;
	timeStruct.tm_mday += 1;
	return mktime(&timeStruct);
}
