#ifndef _WUPCLIENTIMPL_H_
#define _WUPCLIENTIMPL_H_

#include <WinSock2.h>
#include <string>
#include <assert.h>

using namespace std;

class WupClientImpl
{
public:

	void Init(char* szIp, unsigned short usPort){
		m_szIp = szIp;
		m_sPort = usPort;
		//----------------------
		// Initialize Winsock
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		assert(iResult == NO_ERROR);
		if (iResult != NO_ERROR) {
			return ;
		}

	}

	void Disconnect()
	{
		int iResult = SOCKET_ERROR;
		iResult = shutdown(m_ConnectSocket, SD_BOTH);
		if (iResult == SOCKET_ERROR)
		{
			int code = WSAGetLastError();
			assert(iResult != SOCKET_ERROR);
		}
		iResult = closesocket(m_ConnectSocket);
		assert(iResult != SOCKET_ERROR);
		if (iResult == SOCKET_ERROR) {
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return ;
		}
	}

	void Connect()
	{
		int iResult = 0;
		//----------------------
		// Create a SOCKET for connecting to server
		m_ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		assert(m_ConnectSocket != INVALID_SOCKET);
		if (m_ConnectSocket == INVALID_SOCKET) {
			wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return ;
		}
		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(m_szIp.c_str());
		clientService.sin_port = htons(m_sPort);
		// ----------------------
		// Connect to server.
		iResult = connect(m_ConnectSocket, 
							(SOCKADDR *)& clientService, 
								sizeof (clientService));

		if (iResult == SOCKET_ERROR) {
			int code = WSAGetLastError();
			assert(iResult != SOCKET_ERROR);
			wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
			iResult = closesocket(m_ConnectSocket);
			if (iResult == SOCKET_ERROR)
				wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return ;
		}
	}

	void Release()
	{
		WSACleanup();
	}
	
	bool SendBuf(char* szBuf, int cbBufLen)
	{
		int iResult = 0;
		iResult = send(m_ConnectSocket, szBuf, cbBufLen, 0);
		if (iResult == SOCKET_ERROR) {
			wprintf(L"send failed with error: %d\n", WSAGetLastError());
			closesocket(m_ConnectSocket);
			WSACleanup();
			return false;
		}
		return true;
	}

	bool ReceiveBuf()
	{

	}


public:
	string m_szIp;
	unsigned short m_sPort;
	SOCKET m_ConnectSocket;
};

#endif //_WUPCLIENTIMPL_H_