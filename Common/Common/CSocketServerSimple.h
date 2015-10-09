#ifndef _CSOCKETSERVERSIMPLE_H_
#define _CSOCKETSERVERSIMPLE_H_

//http ://msdn.microsoft.com/zh-cn/ms740506

// ��ֹwindows.h��WinSock2.h�еĶ����ظ�
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
	Enum_TypeTotalCount,	// ΪPacket����������
};

class Packet{
public:
	virtual VOID Init() = 0;			// ���еĶ���Ӧ��ʵ��Init(),����صĳ�ʼ��
	virtual VOID CleanUp() = 0;			// ���еĶ���Ӧ������CleanUp(),��������ص��ͷ�
	virtual VOID Release() = 0;			// �ͷŶ���

	virtual EnumPacketType GetPacketType() = 0;		// �õ���Ϣ��������
	virtual unsigned char* GetPacketBuf() = 0;		// �õ�����Ϣ�Ļ���, ����ĳ���
	virtual UINT GetPacketLenth() = 0;				// ��Ϣbuf�ڴ泤��
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
	// ��Ҫ���ɵ�ÿ��Э�鲻ͬ
	// ʱ���
	virtual VOID GetStampTime(UINT& rStampTime );		
	virtual VOID SetStampTime(const UINT &rTimeStamp);	

	// ����������
	virtual VOID GetHipCount(UINT& rHipCount);
	virtual VOID SetHipCount(const UINT &rHipCount);	
private:
	unsigned char m_PacketBuf[k_dwPacketHeartBeat];
};

typedef Packet *(*FUNCCREATE)();

////////////////////////////////////////////////////
// ����Э���������
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
		DOERR("������Ч��Э�������");
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
// ��������ʵ�� 
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
	SOCKET m_SockConncet;	// �����ӵ�socket
};

bool WholeProcessServer();

bool WholeProcessClient();

bool WholeServerSelect();

#endif //_CSOCKETSERVERSIMPLE_H_