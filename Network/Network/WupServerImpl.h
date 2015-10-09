//
// 使用Libevent来作为网络服务器端
//
#ifndef _WUPSERVERIMPL_H_
#define _WUPSERVERIMPL_H_
#include <assert.h>
#include <event2/event-config.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <event2/buffer.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include <deque>

using namespace std;

// 所有的协议体,都是前是4个字节是长度，后面是内容

class WupServerImpl;
#define MAX_EVBUFFERLEN 1024*8 
#define READFDLEN_ONECE 1024*8 

// 链接实体
class ConnectionInterface
{
public:
	ConnectionInterface(WupServerImpl* pServerInst,
							struct sockaddr_in& rSocketAddr,
								int fdClient)
	{
		m_pServerInst = pServerInst;
		m_Addr = rSocketAddr;
		m_fdClient = fdClient;
		m_evBuffer = evbuffer_new();
		assert(m_evBuffer);
		evbuffer_expand(m_evBuffer, MAX_EVBUFFERLEN);
	}
	
	void setEvent(struct event* pEvent)
	{
		m_pEvent = pEvent;
	}
	virtual ~ConnectionInterface()
	{
		if (m_evBuffer)
		{
			evbuffer_free(m_evBuffer);
		}
	}

	virtual void setNonBlock(int fd)
	{
#ifdef WIN32
		int iResult = 0;
		unsigned long iMode = 1;
		iResult = ioctlsocket(fd, FIONBIO, &iMode);
		assert(iResult == NO_ERROR);
#endif // WIN32
#ifdef LINUX
		ioctl()
#endif // LINUX
	}
	virtual void close(){ ; }
	virtual void onRead(int fd, short evt)
	{ 
		int iResult = 0;
		int cbRead = evbuffer_read(m_evBuffer, fd, READFDLEN_ONECE);
		if (cbRead == 0)
		{
			close();
			return;
		}
		else if(cbRead < 0)
		{
			if (WSAEWOULDBLOCK != WSAGetLastError())
			{
				close();
				return;
			}
		}
	
		// 进行协议拼接
		int totoalLen = 0;
		while((totoalLen = evbuffer_get_length(m_evBuffer)) > 4)
		{
			int iProtoLen = 0;
			iResult = evbuffer_copyout(m_evBuffer, &iProtoLen, 4);
			if (iResult == -1)
			{
				close();
				return;
			}
			if (totoalLen - 4 >= iProtoLen)
			{
				assert(0 == evbuffer_drain(m_evBuffer, 4));
				//有完整的协议，可以拼接协议
				// 测试的时候使用固定长度
				char* pBuf = new char[iProtoLen];
				assert(iProtoLen == evbuffer_remove(m_evBuffer, pBuf, iProtoLen));
				printf(" 受到了正常的数据");
			}
			else
			{
				//没有完整的协议，需要等到下次有读事件的时候，在进行处理
				break;
			}
		}
	}

private:
	WupServerImpl* m_pServerInst;
	struct sockaddr_in m_Addr;
	struct event* m_pEvent;
	int m_fdClient;
	struct evbuffer* m_evBuffer;
};

class ConnMngrInterface
{
public:
	static ConnectionInterface *newConn(WupServerImpl* pServerInst,
											struct sockaddr_in& rSocketAddr,
												int fdClient)
	{
		ConnectionInterface *pConn = new ConnectionInterface(pServerInst, rSocketAddr, fdClient);
		assert(pConn);
		m_ConnsQueue.push_back(pConn);
		return pConn;
	}

public:
	static deque<ConnectionInterface*> m_ConnsQueue;
};


// 初始化服务器Libevent相关的对象，
// 以及有新链接的时候创建新链接
class WupServerImpl
{
public:
	WupServerImpl()
	{
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		assert(iResult == NO_ERROR);
		if (iResult != NO_ERROR) {
			return ;
		}
	}

	virtual	~WupServerImpl()
	{
			WSACleanup();
	}

	bool Start(unsigned short usPort)
	{
		m_sPort = usPort;
		init_server(m_sPort);
		return true;
	}
	
	static void on_read(int fd, short ev, void *param)
	{
		assert(param != NULL);
		((ConnectionInterface*)param)->onRead(fd, ev);
	}

	static void on_accept(int fd, short ev, void *param)
	{
		int iResult = 0;

		assert(param!= NULL);
		WupServerImpl *pThis = (WupServerImpl *)param;
		
		struct sockaddr_in clientAddr;
		memset(&clientAddr, 0, sizeof(clientAddr));
		int clientLen = sizeof(struct sockaddr);
		int client_fd = accept(fd, (struct sockaddr*)&clientAddr, &clientLen);
		if (client_fd == INVALID_SOCKET)
		{
			int code = WSAGetLastError();
			assert(client_fd != INVALID_SOCKET);
		}
		printf("--------- 成功接收");
		//struct event* pEvConnection = (struct event*)malloc(sizeof(struct event));
		//struct event* pEvConnection = event_new( );

		// ConnMngrInterface::newConn(WupServerImpl, struct sockaddr &, struct event*, int);
		ConnectionInterface* pConn = ConnMngrInterface::newConn(pThis, clientAddr, client_fd);
		assert(pConn);

		struct event* pEvConnection = event_new(pThis->m_base, client_fd , EV_READ | EV_PERSIST, WupServerImpl::on_read, pConn);
		pConn->setEvent(pEvConnection);
		
		/*iResult = event_assign(pEvConnection, pThis->m_base, 
								client_fd, EV_READ | EV_PERSIST, 
									on_accept, pConn);
									*/
		assert(pEvConnection);
		event_add(pEvConnection , NULL);
	}

	void init_server(short sPort)
	{
		int iResult = 0;
		// 绑定监听端口
		memset(&m_sSvrAddr, 0, sizeof(m_sSvrAddr));
		m_sSvrAddr.sin_family = AF_INET;
		m_sSvrAddr.sin_addr.s_addr = INADDR_ANY;
		m_sSvrAddr.sin_port = htons(sPort);
		
		m_iSvrFd = socket(AF_INET, SOCK_STREAM, 0);
		assert(m_iSvrFd != INVALID_SOCKET);
		int flag = 1;
		iResult = setsockopt(m_iSvrFd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));
		assert(iResult != SOCKET_ERROR);

		iResult = bind(m_iSvrFd, (struct sockaddr*)&m_sSvrAddr, sizeof(struct sockaddr));

		if (iResult == SOCKET_ERROR)
		{
			int code = WSAGetLastError();
			assert(iResult != SOCKET_ERROR);
		}
		
		iResult = listen(m_iSvrFd, 5);
		assert(iResult != SOCKET_ERROR);
		
		m_base = event_base_new();
		assert(m_base);

//		struct event* m_evAccept = (struct event*)malloc(sizeof(event));
		//assert(m_evAccept);
		//memset(m_evAccept, 0, sizeof(event));
		//iResult = event_assign(m_evAccept, m_base, m_iSvrFd, EV_READ | EV_PERSIST, on_accept, this);
		//assert(iResult == 0);
		//event_base_set(m_base, m_evAccept);
		//assert(iResult == 0);

		struct event* m_evAccept = event_new(m_base, m_iSvrFd, EV_READ | EV_PERSIST, WupServerImpl::on_accept, this);
		assert(m_evAccept);
		iResult = event_add(m_evAccept, NULL);
		assert(iResult == 0);
		event_base_dispatch(m_base);
	}
public:
	int m_iSvrFd;
	struct sockaddr_in m_sSvrAddr;

	unsigned short m_sPort;
	event_base *m_base;
};

#endif // _WUPSERVERIMPL_H_