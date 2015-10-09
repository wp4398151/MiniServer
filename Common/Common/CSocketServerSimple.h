#ifndef _CSOCKETSERVERSIMPLE_H_
#define _CSOCKETSERVERSIMPLE_H_

//http ://msdn.microsoft.com/zh-cn/ms740506

// 防止windows.h和WinSock2.h中的定义重复
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock.h>
//#include <WS2tcpip.h>
#include <stdio.h>
#include "WpLogHelper.h"
#include "UtilHelper.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace WpHelper;

class Packet;

enum EnumPacketType{
	Enum_Invalid = 0,
	Enum_HeartBeat = 1,
	Enum_TypeTotalCount,	// 为Packet的类型总数
};

class Packet{
public:
	virtual VOID Init() = 0;			// 所有的对象应该实现Init(),对象池的初始化
	virtual VOID CleanUp() = 0;			// 所有的对象应该事先CleanUp(),用来对象池的释放
	virtual VOID Release() = 0;			// 释放对象

	virtual EnumPacketType GetPacketType() = 0;		// 得到消息包的类型
	virtual unsigned char* GetPacketBuf() = 0;		// 得到该消息的缓存, 缓存的长度
	virtual UINT GetPacketLenth() = 0;				// 消息buf内存长度
};

static const DWORD k_dwPacketHeartBeat = sizeof(UINT) + sizeof(UINT);

class PacketHeartBeat : public Packet
{
public:
	static Packet *CreatePacket(){ return new PacketHeartBeat(); }

	PacketHeartBeat(){ CleanUp(); }
	virtual ~PacketHeartBeat(){ CleanUp(); }

	VOID Init(){ CleanUp(); }
	VOID CleanUp(){ 
		memset(m_PacketBuf, 0, k_dwPacketHeartBeat ); 
		memcpy(m_PacketBuf, (const unsigned char*)&k_dwPacketHeartBeat, sizeof(k_dwPacketHeartBeat));
	}
	virtual VOID Release(){ delete this; }

	virtual EnumPacketType GetPacketType(){ return Enum_HeartBeat; }
	virtual unsigned char *GetPacketBuf(){ return m_PacketBuf; }
	virtual UINT GetPacketLenth(){ return k_dwPacketHeartBeat; }

	//////////////////////////////////////////////
	// 需要生成的每个协议不同
	// 时间戳
	virtual VOID GetStampTime(UINT& rStampTime );		
	virtual VOID SetStampTime(const UINT &rTimeStamp);	

	// 心跳总跳数
	virtual VOID GetHipCount(UINT& rHipCount);
	virtual VOID SetHipCount(const UINT &rHipCount);	
private:
	unsigned char m_PacketBuf[k_dwPacketHeartBeat];
};

typedef Packet *(*FUNCCREATE)();

////////////////////////////////////////////////////
// 数据协议包管理器
// Wupeng 
////////////////////////////////////////////////////
class PacketMngr
{
public:
	static PacketMngr& GetPacketMngr(){
		static PacketMngr *s_packetMngr = new PacketMngr();
		return *s_packetMngr;
	}
	Packet *CreatePacket(EnumPacketType typePacket){
		if (typePacket > Enum_Invalid &&  typePacket < Enum_TypeTotalCount)
		{
			return (funcCreateArray[typePacket])();
		}
		DOERR("不是有效的协议包类型");
		return FALSE;
	}
protected:
	PacketMngr(){
		memset(funcCreateArray, 0, sizeof(funcCreateArray));
		funcCreateArray[Enum_HeartBeat] = PacketHeartBeat::CreatePacket;
	}
	FUNCCREATE funcCreateArray[Enum_TypeTotalCount];
};

////////////////////////////////////////////////////
// 网络链接实体 
// Wupeng 
////////////////////////////////////////////////////
class NetEntity{
public:
	NetEntity();
	virtual ~NetEntity();

	BOOL Init(SOCKET sockConncet);
	BOOL CleanUp();
	
	BOOL Close();
	
	BOOL SendPacket(Packet *pPacket);
	Packet *GetPacket();
				
protected:
	SOCKET m_SockConncet;	// 此链接的socket
};

bool WholeProcessServer();

bool WholeProcessClient();

bool WholeServerSelect();

#endif //_CSOCKETSERVERSIMPLE_H_