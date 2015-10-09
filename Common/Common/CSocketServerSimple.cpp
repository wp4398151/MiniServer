#include "stdafx.h"
#include "CSocketServerSimple.h"

////////////////////////////////////////////////////
// 心跳消息包
// Wupeng 
////////////////////////////////////////////////////
VOID PacketHeartBeat::GetStampTime(UINT& rStampTime)
{
	memcpy(&rStampTime, m_PacketBuf, sizeof(UINT));
}

VOID PacketHeartBeat::SetStampTime(const UINT &rTimeStamp)
{
	memcpy(m_PacketBuf, &rTimeStamp, sizeof(UINT));
}

VOID PacketHeartBeat::GetHipCount(UINT& rHipCount)
{
	memcpy(&rHipCount, m_PacketBuf + sizeof(UINT), sizeof(UINT));
}

VOID PacketHeartBeat::SetHipCount(const UINT &rHipCount)
{
	memcpy(m_PacketBuf + sizeof(UINT), &rHipCount, sizeof(UINT));
}

////////////////////////////////////////////////////
// 网络实体
// Wupeng 
////////////////////////////////////////////////////
NetEntity::NetEntity()
{
	CleanUp();
}

NetEntity::~NetEntity()
{
	CleanUp();
}

BOOL NetEntity::Init(SOCKET sockConncet)
{
	if (sockConncet == INVALID_SOCKET)
	{
		DOERR("传入的套接字无效");
		return FALSE;
	}
	CleanUp();
	m_SockConncet = sockConncet;
	return TRUE;
}

BOOL NetEntity::CleanUp()
{
	if (m_SockConncet != INVALID_SOCKET)
	{
		if (FALSE == Close())
		{
			DOERR("关闭套接字错误");
			m_SockConncet = INVALID_SOCKET;
			return FALSE;
		}
	}
	m_SockConncet = INVALID_SOCKET;
	return TRUE;
}

BOOL NetEntity::Close()
{
	if ( m_SockConncet == INVALID_SOCKET)
	{
		DOERR("套接字无效");
		return FALSE;
	}
	//if (SOCKET_ERROR == shutdown(m_SockConncet, SD_BOTH))
	if (SOCKET_ERROR == shutdown(m_SockConncet, 2))
	{
		DOERR("关闭连接错误");
		return FALSE;
	}
	if (SOCKET_ERROR == closesocket(m_SockConncet))
	{
		DOERR("关闭套接字错误");
		return FALSE;
	}
	m_SockConncet = INVALID_SOCKET;
	return TRUE;
}

BOOL NetEntity::SendPacket(Packet *pPacket)
{
	CHECKNULLALARAM1_RET(pPacket, "需要发送的数据包不能为空", return FALSE);
	int iResult = 0;
	UINT uType = (UINT)pPacket->GetPacketType();

	iResult = send(m_SockConncet, (char*)&uType, sizeof(uType), 0);
	if ( iResult == SOCKET_ERROR)
	{
		DOERR("发送协议失败");
		return FALSE;
	}

	iResult = send(m_SockConncet, (char*)pPacket->GetPacketBuf(), pPacket->GetPacketLenth(), 0);

	if ( iResult == SOCKET_ERROR)
	{
		DOERR("发送协议失败");
		return FALSE;
	}
		
	return TRUE;
}

Packet *NetEntity::GetPacket()
{
	int iResult = 0;
	EnumPacketType uType = Enum_Invalid;
	
	iResult = recv(m_SockConncet, (char*)&uType, sizeof(uType), 0);

	if (iResult <= 0)
	{
		DOERR("接受数据失败");
		return NULL;
	}
	PacketMngr::GetPacketMngr().CreatePacket(uType);
	return NULL;
}

// 完整的基础socket stream通信
bool WholeProcessServer()
{
	// 初始化
	WSADATA wsaData = { 0 };
	int iResult = 0;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);	
	
	if (iResult != 0)
	{
		return false;
	}

	SOCKET sock = INVALID_SOCKET;
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}
	sockaddr_in bindaddr;
	bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_port = htons(19861);

	iResult = bind(sock, (SOCKADDR*)&bindaddr, sizeof(bindaddr));

	if (iResult == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}
		
	if ( listen(sock, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}
	
	SOCKET clientSocket = INVALID_SOCKET;
	// sockaddr clientaddr = {0};
	// int addrlen = 0;	
	// clientSocket = accept(sock, &clientaddr, &addrlen);
	clientSocket = accept(sock, NULL, 0);

	if (clientSocket == INVALID_SOCKET)
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}
	
	char recvBuf[MAX_PATH] = { 0 };
	iResult = recv(clientSocket, recvBuf, MAX_PATH - 1, 0);

	if (iResult <= 0)
	{
		closesocket(clientSocket);
		closesocket(sock);
		WSACleanup();
		// 链接关闭0，或者遇到错误<0
		return false;
	}
	
	printf("success %s", recvBuf);
	
	//if (SOCKET_ERROR == shutdown(clientSocket, SD_SEND))
	if (SOCKET_ERROR == shutdown(clientSocket, 2))
	{
		closesocket(clientSocket);
		closesocket(sock);
		WSACleanup();
		// 链接关闭0，或者遇到错误<0
		return false;
	}

	closesocket(clientSocket);
	closesocket(sock);
	WSACleanup();
	return true;
}

// 完成的基础stream 通信
bool WholeProcessClient()
{
	WSADATA wsaData = {0};
	int iResult = 0;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		return false;
	}
	//sockaddr_in myaddr = { 0 };
	//iResult = inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr);
	//myaddr.sin_port = htons(19861);

	sockaddr_in myaddr = { 0 };
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(19861);
	
	if (iResult == -1)
	{
		WSACleanup();
		return false;
	}

	SOCKET sock = INVALID_SOCKET;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}
	
	iResult = connect(sock, (const sockaddr*)&myaddr, sizeof(myaddr));
	if (iResult == SOCKET_ERROR)
	{
		DOERR("connect : "+ WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return false;
	}

	char sendBuf[MAX_PATH] = { 0 };
	strcpy_s(sendBuf, "wupeng data");
	iResult = send(sock, sendBuf, strlen(sendBuf), 0);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}
	
	//iResult = shutdown(sock, SD_SEND);
	//iResult = shutdown(sock);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	closesocket(sock);
	WSACleanup();
	return true;
}

bool WholeServerSelect()
{
	WSAData wsaData = { 0 };
	WSAStartup(MAKEWORD(2,2), &wsaData);
	
	SOCKET sock = INVALID_SOCKET;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}
	
	/*
	sockaddr_in myaddr = { 0 };
	if (-1 == inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr))
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}
	myaddr.sin_port = htons(19861);
	*/

	sockaddr_in myaddr;
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(19861);

	if (SOCKET_ERROR == bind(sock, (sockaddr*)&myaddr, sizeof(myaddr)))
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}
	
	if (SOCKET_ERROR == listen(sock, SOMAXCONN))
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	SOCKET conn = INVALID_SOCKET;

	sockaddr_in clientaddr = { 0 };
	int clientaddrlen = sizeof(clientaddr);

	conn = accept(sock, (sockaddr*)&clientaddr, &clientaddrlen);

	if (INVALID_SOCKET == conn)
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	SOCKET socketArr[FD_SETSIZE] = {0};
	INT socketArrCount = 0;

	socketArr[socketArrCount++] = conn;

	char* pIP = inet_ntoa(clientaddr.sin_addr);
	unsigned int nport = clientaddr.sin_port;
	
	fd_set read_fds;
	//fd_set write_fds;
	fd_set exception_fds;
	
	//FD_ZERO(&read_fds);
	//FD_ZERO(&write_fds);
	//FD_SET(conn, &read_fds);
	//FD_SET(conn, &read_fds);
	//FD_ZERO(&read_fds);
	//FD_ZERO(&exception_fds);
	while (TRUE)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&exception_fds);
		FD_SET(conn, &read_fds);
		FD_SET(conn, &exception_fds);

		if (0 > select(conn + 1, &read_fds, NULL, &exception_fds, NULL))
		{
			DOERR("选择错误");
			break;
		}
		
		if (FD_ISSET(conn, &read_fds))
		{
			char buf[MAX_PATH] = { 0 };
			int iRet = recv(conn, buf, MAX_PATH - 1, 0);
			if (0 == iRet)
			{
				DOERR("连接断开了 ");
				closesocket(conn);
				closesocket(sock);
				WSACleanup();
				return true;
			}
			else if (SOCKET_ERROR == iRet  )
			{
				closesocket(conn);
				closesocket(sock);
				WSACleanup();
				return false;
			}
			
			DOERR(buf);
		}
		else if (FD_ISSET(conn, &exception_fds))
		{
			DOERR("出现了错误");
			break;
		}
		else
		{
			DOERR("不应该选择成功");
			break;
		}
	}
	
	closesocket(conn);
	closesocket(sock);
	WSACleanup();
	
	return true;
}