#include "include.h"

DWORD GetPID(const wchar_t* procName);
DWORD GetModuleBaseAddressv2(DWORD pID, const wchar_t* moduleName);
DWORD GetModuleSize(DWORD pID, const wchar_t* moduleName);
BOOL ComparePattern(HANDLE pHandle, DWORD address, char* pattern, char* mask);
DWORD ExternalAoBScan(HANDLE pHandle, DWORD pID, const wchar_t* mod, char* pattern, char* mask);