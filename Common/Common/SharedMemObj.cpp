#include "stdafx.h"

#include "SharedMemObj.h"
#include "stdio.h"

#ifndef WIN32
#include <sys/shm.h>
#include <sys/ipc.h>
#endif

namespace wupenghelper
{
	SHMEMHandle CreateShareMem(SHMEMKey keyShmem, INT iSize)
	{
#ifdef WIN32
		char keybuf[32] = { 0 };
		sprintf_s(keybuf, "%d", keyShmem);
		return CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, iSize, keybuf);
#else
		return shmget(keyShemem, iSize, IPC_CREAT | IPC_EXCL | 0666);
#endif
	}

	SHMEMHandle AttachShareMem(SHMEMKey keyShmem, INT iSize)
	{
#ifdef WIN32
		char keybuf[32] = { 0 };
		sprintf_s(keybuf, "%d", keyShmem);
		return OpenFileMappingA(FILE_MAP_ALL_ACCESS, TRUE, keybuf);
#else
		return shmget(keyShmem, iSize, 0);
#endif
	}

	LPVOID MapShareMem(SHMEMHandle hShmem)
	{
#ifdef WIN32
		return MapViewOfFile(hShmem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#else
		return shmat(hShmem, 0, 0);
#endif
	}

	BOOL UnmapShareMem(LPVOID pSharedMemBuf)
	{
#ifdef WIN32
		if (pSharedMemBuf != NULL)
		{
			return UnmapViewOfFile(pSharedMemBuf);
		}
		return FALSE;
#else
		shmdt(pSharedMemBuf);
#endif

	}

	VOID CloseSharedMem(SHMEMHandle hShmem)
	{
#ifdef WIN32
		if (hShmem != NULL)
		{
			CloseHandle(hShmem);
		}
#else
		shmctl(hShmem, IPC_RMID, 0);
#endif
	}
}