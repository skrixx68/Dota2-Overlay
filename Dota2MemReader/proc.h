#include "include.h"


DWORD GetProcId(const wchar_t* procname);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modname);
uintptr_t GetDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offset);