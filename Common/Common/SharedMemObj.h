#ifndef _SHAREDMEMOBJ_H_
#define _SHAREDMEMOBJ_H_
// 用来得到共享内存
#include <windows.h>
#ifdef WIN32
typedef HANDLE  SHMEMHandle;
typedef int		SHMEMKey;
#else // linux
typedef INT		SHMEMHandle;
typedef key_t	SHMEMKey;
#endif


namespace wupenghelper
{
	// 兼容共享内存 API --------------------------
	SHMEMHandle CreateShareMem(SHMEMKey keyShmem, INT iSize);
	SHMEMHandle AttachShareMem(SHMEMKey keyShmem, INT iSize);
	LPVOID		MapShareMem(SHMEMHandle hShmem);
	BOOL		UnmapShareMem(LPVOID pSharedMemBuf);
	VOID		CloseSharedMem(SHMEMHandle hShmem);

	// 用于管理共享内存区域的对象	
	template<typename T>
	class CSharedMemObj
	{
	public:
		CSharedMemObj(BOOL bIsAttach, UINT count, SHMEMKey keyShareMem);
		~CSharedMemObj();

		T* GetObj(UINT idx);
	private:
		UINT			m_iSize;
		char *			m_pSharedMemBuf;
		SHMEMHandle		m_hSharedhandle;
	};

	template<typename T>
	CSharedMemObj<T>::CSharedMemObj(BOOL bIsAttach, UINT count, SHMEMKey keyShareMem)
	{
		int totalMemsize = sizeof(T)*count;
		m_iSize = count;
		m_pSharedMemBuf = NULL;
		m_hSharedhandle = NULL;

		if (bIsAttach == TRUE)
		{
			m_hSharedhandle = AttachShareMem(keyShareMem, totalMemsize);
		}
		else
		{
			m_hSharedhandle = CreateShareMem(keyShareMem, totalMemsize);
		}

		if (m_hSharedhandle == NULL)
		{
			return;
		}

		m_pSharedMemBuf = (char*)MapShareMem(m_hSharedhandle);
		if (m_pSharedMemBuf == NULL)
		{
			CloseHandle(m_hSharedhandle);
			m_hSharedhandle = NULL;
			return;
		}
		if (bIsAttach == TRUE)
		{
			memset(m_pSharedMemBuf, 0, totalMemsize);
		}
	}

	template<typename T>
	CSharedMemObj<T>::~CSharedMemObj()
	{
		if (NULL != m_pSharedMemBuf && NULL != m_hSharedhandle)
		{
			UnmapShareMem(m_pSharedMemBuf);
			CloseHandle(m_hSharedhandle);
			m_pSharedMemBuf = NULL;
			m_hSharedhandle = NULL;
		}
	}

	template<typename T>
	T* CSharedMemObj<T>::GetObj(UINT idx)
	{
		if (NULL == m_pSharedMemBuf || NULL == m_hSharedhandle || idx >= m_iSize)
		{
			return NULL;
		}

		return (T*)(m_pSharedMemBuf + (idx * sizeof(T)));
	}
}

#endif // _SHAREDMEMOBJ_H_




















