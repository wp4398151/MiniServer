#ifndef _WPOBJPOOL_H_
#define _WPOBJPOOL_H_
#include <queue>
#include <assert.h>
#include "UtilHelper.h"

using namespace std;

namespace wupenghelper
{
	class ObjWithTicket
	{
	public:
		ObjWithTicket(){ m_TicketId = 0; }

		int GetTicketId(){ return m_TicketId ;  }
		void SetTicketId(int ticketId){ m_TicketId = ticketId;  }
	protected:
		int m_TicketId;
	};

//
// 带票根索引的对象，用于被管理	
// 
	template<typename T, int TICKETINDEXSIZE = 250>
	class ObjPoolWithTicket
	{
	public:
		ObjPoolWithTicket(){
			m_TicketCount = 0;
		}

		~ObjPoolWithTicket(){
			m_TicketCount = 0;
		}

		T* GetByTicket(int idx);
		T *NewObj();
		void DelObj(int vectorIndex);
		void DelObj(T* pTObj);
		void AssignmentObj();

	public:
		vector<T*> m_IndexVector;
		queue<T*> m_ObjFreeQueue;
		int m_TicketCount;		// 用于生成对象的索引, 如果超出了int的上限分配则会出现复用问题
	};
	
	template<typename T, int TICKETINDEXSIZE>
	T* ObjPoolWithTicket<T, TICKETINDEXSIZE>::GetByTicket(int idx)
	{
		if (ISINRANGE(idx, 0, m_IndexVector.size() - 1))
		{
			assert(ISINRANGE(idx, 0, m_IndexVector.size() - 1));
			return NULL;
		}
		assert(m_IndexVector[idx] != NULL);
		return m_IndexVector[idx];
	}

	template<typename T, int TICKETINDEXSIZE>
	T *ObjPoolWithTicket<T, TICKETINDEXSIZE>::NewObj()
	{
		if (m_Objdeque.empty())
		{
			AssignmentObj();
		}
		T *pT = NULL;
		pT = m_Objdeque.front();
		assert(pT != NULL);
		if (NULL == pT)
		{
			return NULL;
		}
		pT->Init();
		m_Objdeque.pop();
		return pT;
	}

	template<typename T, int TICKETINDEXSIZE>
	void ObjPoolWithTicket<T, TICKETINDEXSIZE>::DelObj(T* pTObj)
	{
		if (pTObj)
		{
			pTObj->ClearUp();
			m_Objdeque.push(pTObj);
		}
		return;
	}

	template<typename T, int TICKETINDEXSIZE>
	void ObjPoolWithTicket<T, TICKETINDEXSIZE>::DelObj(int idx)
	{
		if (ISINRANGE(idx, 0, m_IndexVector.size() - 1))
		{
			assert(ISINRANGE(idx, 0, m_IndexVector.size() - 1));
			return;
		}
		T * pT = m_IndexVector[idx]
		assert(pT != NULL);
		if (pT != NULL)
		{
			pT->ClearUp();
			m_Objdeque.push(pT);
		}
	}

	template<typename T, int TICKETINDEXSIZE>
	void ObjPoolWithTicket<T, TICKETINDEXSIZE>::AssignmentObj()
	{
		T *pT = NULL;
		for (int i = 0; i < TICKETINDEXSIZE; i++)
		{
			pT = new T();
			assert( pT != NULL );
			if (pT)
			{
				pT->SetTicketId(m_IndexVector++);
				m_IndexVector[m_IndexVector - 1] = pT;
				m_Objdeque.push(pT);
			}
			else
			{
				return;
			}
		}
	}

	//一次分配的最大大小
#define MAXPREINITOBJCOUNT 250
	template <typename T>
	class ObjPool
	{
	public:
		T *NewObj();
		VOID DelObj(T* pTObj);
		VOID AssignmentObj();
	private:
		queue<T*> m_Objdeque;
	};

	template<typename T>
	VOID ObjPool<T>::AssignmentObj()
	{
		T *pT = NULL;
		for (int i = 0; i < MAXPREINITOBJCOUNT; i++)
		{
			pT = new T();
			assert( pT != NULL);
			if (pT)
			{
				m_Objdeque.push(pT);
			}
			else
			{
				return;
			}
		}
	}

	template<typename T>
	T * ObjPool<T>::NewObj()
	{
		if (0 <= m_Objdeque.size())
		{
			AssignmentObj();
		}
		T *pT = NULL;
		pT = m_Objdeque.front();
		assert(pT != NULL);
		if (NULL == pT)
		{
			return NULL;
		}
		pT->Init();
		m_Objdeque.pop();
		return pT;
	}

	template<typename T>
	VOID ObjPool<T>::DelObj(T* pTObj)
	{
		if (pTObj)
		{
			pTObj->ClearUp();
			m_Objdeque.push(pTObj);
		}
		return;
	}
}
#endif //_WPOBJPOOL_H_