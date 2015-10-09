#ifndef _WPLOGHELPER_H_
#define _WPLOGHELPER_H_

#include <sstream>

using namespace std;

namespace WpHelper
{
	// �𵽼�ƴ���ַ���,�Լ������,�Լ���ʾ
	class LogClass
	{
	public:
		LogClass& operator+(int num);
		LogClass& operator+(float num);
		LogClass& operator+(const char *str);
		LogClass& operator+(const string *str);
		void ShowMsg();	 
		void DisplayEnd();
		void Display();

	public:
		stringstream m_Str;
	};
	
	// ������־
	class LogMngr
	{
	public:
		
	public:
		string m_LogPath;
	};



#define DOERR(x)						\
{										\
	LogClass log;						\
	(log + x + "(" + __FILE__ + ":" + __LINE__ + ")").ShowMsg();				\
}

#define DOLOG(x)						\
	{									\
		LogClass log;					\
		(log + x).Display();			\
	}

#define DOLOGL(x)						\
	{									\
		LogClass log;					\
		(log + x + "(" + __FILE__ + ":" + __LINE__ + ")").DisplayEnd();		\
	}
}
#endif //_WPLOGHELPER_H_