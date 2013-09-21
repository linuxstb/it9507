
#ifndef WINIO_H
#define WINIO_H

#ifdef WINIO_DLL
#define WINIO_API _declspec(dllexport)
#else
#define WINIO_API _declspec(dllimport)
#endif

#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
  WINIO_API bool _stdcall InitializeWinIo();
  WINIO_API void _stdcall ShutdownWinIo();
  WINIO_API PBYTE _stdcall MapPhysToLin(PBYTE pbPhysAddr, DWORD dwPhysSize, HANDLE *pPhysicalMemoryHandle);
  WINIO_API bool _stdcall UnmapPhysicalMemory(HANDLE PhysicalMemoryHandle, PBYTE pbLinAddr);
  WINIO_API bool _stdcall GetPhysLong(PBYTE pbPhysAddr, PDWORD pdwPhysVal);
  WINIO_API bool _stdcall SetPhysLong(PBYTE pbPhysAddr, DWORD dwPhysVal);
  WINIO_API bool _stdcall GetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
  WINIO_API bool _stdcall SetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);
  WINIO_API bool _stdcall InstallWinIoDriver(PSTR pszWinIoDriverPath, bool IsDemandLoaded);
  WINIO_API bool _stdcall RemoveWinIoDriver();
#ifdef __cplusplus
}
#endif
extern bool IsNT;
extern HANDLE hDriver;
extern bool IsWinIoInitialized;

bool _stdcall StartWinIoDriver();
bool _stdcall StopWinIoDriver();

#endif
