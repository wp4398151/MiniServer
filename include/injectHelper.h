#ifndef _INJECTHELPER_H_
#define _INJECTHELPER_H_
#include <Windows.h>

typedef HHOOK(WINAPI *SWHEXPROC)(int, HOOKPROC, HINSTANCE, DWORD);
typedef HANDLE(WINAPI *CRTPROC)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI *WPMPROC)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
typedef LPVOID(WINAPI *VAEPROC)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
typedef BOOL(WINAPI *VFEPROC)(HANDLE, LPVOID, SIZE_T, DWORD);
typedef HANDLE(WINAPI *OPPROC) (DWORD, BOOL, DWORD);

BOOL LoadSeDebugPrivilege();
BOOL WINAPI InjectLibrarySafeA(DWORD threadID, const CHAR*pDLL, DWORD dwLen);
BOOL WINAPI InjectLibraryA(HANDLE hProcess, const CHAR *pDLL, DWORD dwLen);

#endif //INJECTHELPER_H_