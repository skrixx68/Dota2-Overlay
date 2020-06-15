#include "include.h"

DWORD GetPID(const wchar_t* procName) {															// Itterates through every process and looks for a process who's executable name matches the char array passed to this function,
																						// then returns the process ID of that process
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	DWORD pID = NULL;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);				// Creates a snapshot of the currently running processes to itterate over

	if (Process32First(snapshot, &entry)) {												// Grabs the first process' information
		do {
			if (_wcsicmp(entry.szExeFile, procName) == 0) { 								// Compare the process file name to the only argument passed into the function																	
				pID = entry.th32ProcessID;												// If they are the same set the pID value to that process pID
				break;																	// and break out of the do while loop
			}
		} while (Process32Next(snapshot, &entry));										// Continue scanning the next process in the snapshot
	}

	CloseHandle(snapshot);																// Close the handle since we're done with it

	return pID;																			// Returns the pID

}

DWORD GetModuleBaseAddressv2(DWORD pID, const wchar_t* moduleName) {								// Itterates through the process with the provided process ID and returns the base address of the module provided

	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);
	DWORD baseAddress = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);					// Creates a snapshot of the process with the provided process ID

	if (Module32First(snapshot, &entry)) {												// Grabs the first modules information
		do {
			if (_wcsicmp(entry.szModule, moduleName) == 0) {							// Compares the module name to the argument passed into the function
				baseAddress = (DWORD)entry.modBaseAddr;									// if they are the same set the baseAddress variable to the base address of the module
				break;																	// and break out of the do while loop
			}
		} while (Module32Next(snapshot, &entry));										// continue scanning the next module in the snapshot
	}

	CloseHandle(snapshot);																// Close the handle since we're done with it
	return baseAddress;																	// Return the base Address
}


DWORD GetModuleSize(DWORD pID, const wchar_t* moduleName) {								// Itterates through the process with the provided process ID and returns the base address of the module provided

	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);
	DWORD moduleSize = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);					// Creates a snapshot of the process with the provided process ID

	if (Module32First(snapshot, &entry)) {												// Grabs the first modules information
		do {
			if (_wcsicmp(entry.szModule, moduleName) == 0) {							// Compares the module name to the argument passed into the function
				moduleSize = (DWORD)entry.modBaseSize;									// if they are the same set the moduleSize variable to the size of the module
				break;																	// and break out of the do while loop
			}
		} while (Module32Next(snapshot, &entry));										// continue scanning the next module in the snapshot
	}

	CloseHandle(snapshot);																// Close the handle since we're done with it
	return moduleSize;																	// Return the module size
}

BOOL ComparePattern(HANDLE pHandle, DWORD address, char* pattern, char* mask) {			// Given an address pattern and mask, it will check if the current address matches that pattern

	DWORD patternSize = strlen(mask);													// Set the length of the pattern so we don't scan more than we need to

	auto memBuf = new char[patternSize + 1];											// Creaet a new char array with the length of the pattern size
	memset(memBuf, 0, patternSize + 1);													// Set the above array to all 0s
	ReadProcessMemory(pHandle, (LPVOID)address, memBuf, patternSize, 0);				// Read the memory from the address provied (With a langth of the pattern size to the above array


	for (DWORD i = 1; i < patternSize; i++) {											// For each byte in the above array

		if (memBuf[i] != pattern[i] && mask[i] != *"?") {								// If the pattern at that index doesn't match the array at that index, and the mask doesn't have a wild card
			delete memBuf;																// Delete the buffer we created
			return false;																// Return false since the pattern didn't match
		}
	}
	delete memBuf;																		// Delete the buffer we created
	return true;																		// Return true since every byte matched
}

DWORD ExternalAoBScan(HANDLE pHandle, DWORD pID, const wchar_t* mod, char* pattern, char* mask) {			// This function will store the entire memory of a specific external module into memory, and itterate over it searching for an AoB pattern
																									// This was done somewhat lazily and coppies the entire memory space, instead of itterating over a page at a time
																									// It will have a higher memory usage than itterating over a page at a time, but unless you're running on a low memory system you  should be fine

	std::vector<DWORD> matches;																		// Create a vector to store all our results in
	DWORD patternSize = strlen(mask);																// Store the length of the pattern

	DWORD moduleBase = GetModuleBaseAddressv2(pID, mod);												// Get the base address of the module
	DWORD moduleSize = GetModuleSize(pID, mod);														// Get the size of the module

	if (!moduleBase || !moduleSize) {																// If either GetModuleBaseAddress or GetModuleSize returned NULL
		std::cout << "Could not get " << mod << " base address or size" << std::endl;				// Let the user know 
		return NULL;																				// Return NULL
	}

	auto moduleBytes = new char[moduleSize + 1];													// Create a new array of bytes the size of the module
	memset(moduleBytes, 0, moduleSize + 1);															// Set all the bytes in that array to 0
	ReadProcessMemory(pHandle, (LPVOID)moduleBase, moduleBytes, moduleSize, 0);						// Read the entire module into a local buffer that we can read from

	for (int i = 0; i + patternSize < moduleSize; i++) {											// For each byte in that module, if the index + the pattern size wont go past the end of the buffer
		if (pattern[0] == moduleBytes[i]) {															// If the first byte in the pattern is equal to the current byte in the module memory
			if (ComparePattern(pHandle, moduleBase + i, pattern, mask)) {							// Check if the entire pattern matches
				matches.push_back(moduleBase + i);													// If it does, push that address into the matches Vector
			}
		}
	}

	delete moduleBytes;																				// Delete the buffer we created

	if (matches.size() == 0) {																		// If there we no matches
		return NULL;																				// Return NULL
	}
	return matches[0];																				// If there were, return the first match (Change this plus the function type to return the entire vector
}


//int main() {
//	char process[] = "TestEXE.exe";																													// The name of the process (and module) we need to access
//	char pattern[] = "\x68\x00\x00\x00\x00\xa1\x00\x00\x00\x00\x50\xe8\x00\x00\x00\x00\x83\xc4\x00\x8b\xc8\xff\x15\x00\x00\x00\x00\x3b\xdc";		// The pattern we are looking for
//	char mask[] = "x????x????xx????xx?xxxx????xx";																									// The mask we are using for that pattern
//
//	DWORD pID = GetPID(process);																													// Obtain the process ID of the above process
//	if (!pID) {																																		// If we can't get a valid process ID
//		std::cout << "Could not find process" << std::endl;																							// Let the user know
//		std::cin.get();																																// Wait for them to press enter
//		return 1;																																	// Return 1 and exit the program
//	}
//
//	HANDLE pHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, pID);											// Obtain a handle to the process
//	if (!pHandle) {																																	// If we can't get a handle to the process
//		std::cout << "Could not obtain handle" << std::endl;																						// Let the user know
//		std::cin.get();																																// Wait for them to press enter
//		return 1;																																	// Return 1 and exit the program
//	}
//
//	DWORD address = ExternalAoBScan(pHandle, pID, process, pattern, mask);																			// Run the AoB scan and store the returned address in the address varaible
//	if (address) {																																	// If it's not NULL
//		std::cout << "Address: " << std::hex << address << std::endl;																				// Print it to the screen
//	}
//	else {																																			// If it is null
//		std::cout << "No address" << std::endl;																										// Let the user know
//	}
//
//	std::cin.get();																																	// Wait for the user to press enter
//	return 0;																																		// Return 0 and exit the program
//
//}