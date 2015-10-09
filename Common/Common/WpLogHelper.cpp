#include "stdafx.h"

#include <Windows.h>
#include "WpLogHelper.h"

namespace WpHelper
{
	LogClass& LogClass::operator+(int num)
	{
		m_Str << num;
		return *this;
	}

	LogClass& LogClass::operator+(float num)
	{
		m_Str << num;
		return *this;
	}

	LogClass& LogClass::operator+(const char *str)
	{
		m_Str << str;
		return *this;
	}

	LogClass& LogClass::operator+(const string *str)
	{
		m_Str << str;
		return *this;
	}

	void LogClass::ShowMsg()
	{
		MessageBoxA(NULL, m_Str.str().c_str(), "Error", MB_OK );
	}

	void LogClass::DisplayEnd()
	{
		m_Str << "\r\n";
		OutputDebugStringA(m_Str.str().c_str());
	}

	void LogClass::Display()
	{
		OutputDebugStringA(m_Str.str().c_str());
	}
}