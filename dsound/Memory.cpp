#include "Memory.h"
#include <TlHelp32.h>
#include "Utils.h"
#include "skCrypt.h"
#include "syscall.h"
#include <vector>
#include <Psapi.h>
typedef struct _MEMORY_REGION {
	DWORD_PTR dwBaseAddr;
	DWORD_PTR dwMemorySize;
}MEMORY_REGION;
static DWORD  processID = 0L;
static HANDLE processHandle = nullptr;
auto NtWriteVirtualMemory = makesyscall<bool>("NtWriteVirtualMemory");
auto NtReadVirtualMemory = makesyscall<bool>("NtReadVirtualMemory");
DWORD Memory::getProcessIDByName(const char* processName)
{

	UINT32               procThreadCount = 0L;
	DWORD                processID = 0L;
	HANDLE               processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32       processEntry;

	if (processSnap == INVALID_HANDLE_VALUE)
		goto Label;

	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(processSnap, &processEntry)) {
		do {
			if (strncmp(processEntry.szExeFile, processName, sizeof(processEntry.szExeFile)) == 0 && processEntry.cntThreads > procThreadCount) {
				processID = processEntry.th32ProcessID;
				procThreadCount = processEntry.cntThreads;
			}

		} while (Process32Next(processSnap, &processEntry));
	}

	CloseHandle(processSnap);

Label:

	return processID;
}

inline DWORD getAowProcessID()
{
	auto processName = skCrypt("aow_exe.exe");

	UINT32               procThreadCount = 0L;
	DWORD                processID = 0L;
	HANDLE               processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32       processEntry;

	if (processSnap == INVALID_HANDLE_VALUE)
		goto Label;

	processEntry.dwSize = sizeof(PROCESSENTRY32);
	processName.decrypt();
	if (Process32First(processSnap, &processEntry)) {
		do {
			if (strncmp(processEntry.szExeFile, processName, sizeof(processName)) == 0 && processEntry.cntThreads > procThreadCount) {
				processID = processEntry.th32ProcessID;
				procThreadCount = processEntry.cntThreads;
			}

		} while (Process32Next(processSnap, &processEntry));
	}

	CloseHandle(processSnap);

Label:
	processName.clear();

	return processID;
}

HANDLE Memory::getProcessHandle()
{
	processID = getAowProcessID();
	if (processID)
		processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	else
		processHandle = nullptr;

	return processHandle;
}

BOOL Memory::read(UINT32 address, PVOID buffer, UINT32 bufferSize)
{
	unsigned long OldProtect;
	unsigned long OldProtect2;
	VirtualProtectEx(processHandle, reinterpret_cast<PVOID>(address), bufferSize, PAGE_EXECUTE_READWRITE, &OldProtect);
	NtReadVirtualMemory(processHandle, reinterpret_cast<PVOID>(address), buffer, bufferSize, nullptr);
	VirtualProtectEx(processHandle, reinterpret_cast<PVOID>(address), bufferSize, OldProtect, NULL);
	return true;
}

BOOL Memory::write(UINT32 address, PVOID buffer, UINT32 bufferSize)
{
	unsigned long OldProtect;
	unsigned long OldProtect2;
	VirtualProtectEx(processHandle, reinterpret_cast<PVOID>(address), bufferSize, PAGE_EXECUTE_READWRITE, &OldProtect);
	NtWriteVirtualMemory(processHandle, reinterpret_cast<PVOID>(address), buffer, bufferSize, nullptr);
	VirtualProtectEx(processHandle, reinterpret_cast<PVOID>(address), bufferSize, OldProtect, NULL);
	return true;
}
int getAowProcId2()
{
	int pid = 0;
	PROCESS_MEMORY_COUNTERS ProcMC;
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE ProcHandle;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &ProcEntry) == TRUE)
	{
		while (Process32Next(snapshot, &ProcEntry) == TRUE)
		{
			if (strcmp(ProcEntry.szExeFile, "aow_exe.exe") == 0)
			{
				ProcHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcEntry.th32ProcessID);

				if (NULL == ProcHandle)
					continue;

				if (GetProcessMemoryInfo(ProcHandle, &ProcMC, sizeof(ProcMC)))
				{
					if (ProcMC.WorkingSetSize > 300000000)
					{
						pid = ProcEntry.th32ProcessID;
						return pid;
						break;
					}

				}

				CloseHandle(ProcHandle);
			}
		}
	}

	CloseHandle(snapshot);
}
int getGagaProcId()
{
	int pid = 0;
	PROCESS_MEMORY_COUNTERS ProcMC;
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE ProcHandle;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &ProcEntry) == TRUE)
	{
		while (Process32Next(snapshot, &ProcEntry) == TRUE)
		{
			if (strcmp(ProcEntry.szExeFile, "AndroidProcess.exe") == 0)
			{
				ProcHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcEntry.th32ProcessID);

				if (NULL == ProcHandle)
					continue;

				if (GetProcessMemoryInfo(ProcHandle, &ProcMC, sizeof(ProcMC)))
				{
					if (ProcMC.WorkingSetSize > 300000000)
					{
						pid = ProcEntry.th32ProcessID;
						return pid;
						break;
					}

				}

				CloseHandle(ProcHandle);
			}
		}
	}

	CloseHandle(snapshot);
}
int getLDProcId()
{
	int pid = 0;
	PROCESS_MEMORY_COUNTERS ProcMC;
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE ProcHandle;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &ProcEntry) == TRUE)
	{
		while (Process32Next(snapshot, &ProcEntry) == TRUE)
		{
			if (strcmp(ProcEntry.szExeFile, "LdBoxHeadless.exe") == 0)
			{
				ProcHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcEntry.th32ProcessID);

				if (NULL == ProcHandle)
					continue;

				if (GetProcessMemoryInfo(ProcHandle, &ProcMC, sizeof(ProcMC)))
				{
					if (ProcMC.WorkingSetSize > 300000000)
					{
						pid = ProcEntry.th32ProcessID;
						return pid;
						break;
					}

				}

				CloseHandle(ProcHandle);
			}
		}
	}

	CloseHandle(snapshot);
}
int getLDDProcId()
{
	int pid = 0;
	PROCESS_MEMORY_COUNTERS ProcMC;
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE ProcHandle;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &ProcEntry) == TRUE)
	{
		while (Process32Next(snapshot, &ProcEntry) == TRUE)
		{
			if (strcmp(ProcEntry.szExeFile, "LdVBoxHeadless.exe") == 0)
			{
				ProcHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcEntry.th32ProcessID);

				if (NULL == ProcHandle)
					continue;

				if (GetProcessMemoryInfo(ProcHandle, &ProcMC, sizeof(ProcMC)))
				{
					if (ProcMC.WorkingSetSize > 300000000)
					{
						pid = ProcEntry.th32ProcessID;
						return pid;
						break;
					}

				}

				CloseHandle(ProcHandle);
			}
		}
	}

	CloseHandle(snapshot);
}
int getProcId()
{
	int aow = 0;
	int gaga = 0;
	aow = getAowProcId2();
	gaga = getGagaProcId();
	if (gaga == 0 || gaga == 1)
	{
		if (aow == 0 || aow == 1)
		{
			return 0;
		}
		else
		{
			return aow;
		}
	}
	else
	{
		return gaga;
	}
}
int SundaySearch(BYTE* bStartAddr, int dwSize, BYTE* bSearchData, DWORD dwSearchSize)
{
	if (dwSize < 0)
	{
		return -1;
	}
	int iIndex[256] = { 0 };
	int i, j;
	DWORD k;

	for (i = 0; i < 256; i++)
	{
		iIndex[i] = -1;
	}

	j = 0;
	for (i = dwSearchSize - 1; i >= 0; i--)
	{
		if (iIndex[bSearchData[i]] == -1)
		{
			iIndex[bSearchData[i]] = dwSearchSize - i;
			if (++j == 256)
				break;
		}
	}
	i = 0;
	BOOL bFind = FALSE;
	//j=dwSize-dwSearchSize+1;
	j = dwSize - dwSearchSize + 1;
	while (i < j)
	{
		for (k = 0; k < dwSearchSize; k++)
		{
			if (bStartAddr[i + k] != bSearchData[k])
				break;
		}
		if (k == dwSearchSize)
		{
			//ret=bStartAddr+i;
			bFind = TRUE;
			break;
		}
		if (i + dwSearchSize >= dwSize)
		{

			return -1;
		}
		k = iIndex[bStartAddr[i + dwSearchSize]];
		if (k == -1)
			i = i + dwSearchSize + 1;
		else
			i = i + k;
	}
	if (bFind)
	{
		return i;
	}
	else
		return -1;

}
int MemFind(BYTE* buffer, int dwBufferSize, BYTE* bstr, DWORD dwStrLen)
{
	if (dwBufferSize < 0)
	{
		return -1;
	}
	DWORD  i, j;
	for (i = 0; i < dwBufferSize; i++)
	{
		for (j = 0; j < dwStrLen; j++)
		{
			if (buffer[i + j] != bstr[j] && bstr[j] != '?')
				break;
		}
		if (j == dwStrLen)
			return i;
	}
	return -1;
}
BOOL MemSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet)
{
	DWORD pid = getProcId();
	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	BYTE* pCurrMemoryData = NULL;
	MEMORY_BASIC_INFORMATION	mbi;
	std::vector<MEMORY_REGION> m_vMemoryRegion;
	mbi.RegionSize = 0x1000;
	DWORD dwAddress = dwStartAddr;



	while (VirtualQueryEx(phandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < dwEndAddr) && ((dwAddress + mbi.RegionSize) > dwAddress))
	{

		if ((mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE))
		{

			MEMORY_REGION mData = { 0 };
			mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
			mData.dwMemorySize = mbi.RegionSize;
			m_vMemoryRegion.push_back(mData);

		}
		dwAddress = (DWORD)mbi.BaseAddress + mbi.RegionSize;

	}


	std::vector<MEMORY_REGION>::iterator it;
	for (it = m_vMemoryRegion.begin(); it != m_vMemoryRegion.end(); it++)
	{
		MEMORY_REGION mData = *it;


		DWORD_PTR dwNumberOfBytesRead = 0;

		if (bIsCurrProcess)
		{
			pCurrMemoryData = (BYTE*)mData.dwBaseAddr;
			dwNumberOfBytesRead = mData.dwMemorySize;
		}
		else
		{

			pCurrMemoryData = new BYTE[mData.dwMemorySize];
			ZeroMemory(pCurrMemoryData, mData.dwMemorySize);
			ReadProcessMemory(phandle, (LPCVOID)mData.dwBaseAddr, pCurrMemoryData, mData.dwMemorySize, &dwNumberOfBytesRead);

			if ((int)dwNumberOfBytesRead <= 0)
			{
				delete[] pCurrMemoryData;
				continue;
			}
		}
		if (iSearchMode == 0)
		{
			DWORD_PTR dwOffset = 0;
			int iOffset = MemFind(pCurrMemoryData, dwNumberOfBytesRead, bSearchData, nSearchSize);
			while (iOffset != -1)
			{
				dwOffset += iOffset;
				vRet.push_back(dwOffset + mData.dwBaseAddr);
				dwOffset += nSearchSize;
				iOffset = MemFind(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, bSearchData, nSearchSize);
			}
		}
		else if (iSearchMode == 1)
		{

			DWORD_PTR dwOffset = 0;
			int iOffset = SundaySearch(pCurrMemoryData, dwNumberOfBytesRead, bSearchData, nSearchSize);

			while (iOffset != -1)
			{
				dwOffset += iOffset;
				vRet.push_back(dwOffset + mData.dwBaseAddr);
				dwOffset += nSearchSize;
				iOffset = MemFind(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, bSearchData, nSearchSize);
			}

		}

		if (!bIsCurrProcess && (pCurrMemoryData != NULL))
		{
			delete[] pCurrMemoryData;
			pCurrMemoryData = NULL;
		}

	}
	return TRUE;
}

void Memory::offsetsearch2(int offset, BYTE write[], SIZE_T size, int header)
{
	DWORD pid = getProcId();
	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	int addr = header + offset;
	unsigned long OldProtect;
	unsigned long OldProtect2;
	VirtualProtectEx(phandle, (BYTE*)addr, size, PAGE_EXECUTE_READWRITE, &OldProtect);
	///WriteProcessMemory(phandle, (BYTE*)addr, write, size, NULL);
	NtWriteVirtualMemory(phandle, (void*)(addr), (LPCVOID)write, size, NULL);
	VirtualProtectEx(phandle, (BYTE*)addr, size, OldProtect, NULL);
}


int SINGLEAOBSCAN(BYTE BypaRep[], SIZE_T size)
{
	DWORD pid = getProcId();
	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	std::vector<DWORD_PTR> Bypassdo;
	MemSearch(BypaRep, size, 0x26000000, 0xB0000000, false, 0, Bypassdo);

	if (Bypassdo.size() != 0) {
		return Bypassdo[0];
	}
}
UINT32 Memory::getGameModuleBase()
{
	constexpr BYTE  gameModuleBaseAOB[] = {
		0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
		0x80, 0x1A, 0x49, 0x03, 0x00, 0x02, 0x00, 0x05, 0x34, 0x00, 0x20, 0x00, 0x08, 0x00, 0x28, 0x00,
		0x1A, 0x00, 0x19, 0x00, 0x06, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
		0x34, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	SIZE_T                   i, j;
	PBYTE                    buffer;
	MEMORY_BASIC_INFORMATION memBasicInfo;
	UINT64                   baseAddress = 0LL;

	while (VirtualQueryEx(processHandle, reinterpret_cast<LPCVOID>(baseAddress), &memBasicInfo, sizeof(memBasicInfo))) {
		if (baseAddress >= 0xFFFFFFFF)
			break;

		if (memBasicInfo.State & MEM_COMMIT) {
			if (memBasicInfo.Protect == PAGE_READONLY) {
				if (memBasicInfo.RegionSize > sizeof(gameModuleBaseAOB)) {
					if ((memBasicInfo.AllocationProtect & PAGE_NOCACHE) == 0L) {
						if (buffer = reinterpret_cast<PBYTE>(malloc(memBasicInfo.RegionSize)); buffer != nullptr) {
							if (Memory::read(reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress), buffer, memBasicInfo.RegionSize)) {
								for (i = 0; i < (memBasicInfo.RegionSize - sizeof(gameModuleBaseAOB)); i++) {
									/*for (j = 0; j < sizeof(gameModuleBaseAOB); j++) {
										if (buffer[i + j] != gameModuleBaseAOB[j] && gameModuleBaseAOB[j] != '?')
											break;
									}

									if (j == sizeof(gameModuleBaseAOB)) {
										free(buffer);
										return reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress) + i;
									}*/

									if (memcmp(PVOID(SIZE_T(buffer) + i), gameModuleBaseAOB, sizeof(gameModuleBaseAOB)) == 0) {
										free(buffer);
										return reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress) + i;
									}
								}
							}

							free(buffer);
						}
					}
				}
			}
		}

		baseAddress = reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress) + memBasicInfo.RegionSize;
	}

	//fatal("Game module base found", "getGameModuleBase", 0);

	return 0ULL;
}
UINT32 Memory::getGameModuleBase2()
{
	int libue4header = 0;
	BYTE ue4head[] = {

		0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
		0x80, 0x1A, 0x49, 0x03, 0x00, 0x02, 0x00, 0x05, 0x34, 0x00, 0x20, 0x00, 0x08, 0x00, 0x28, 0x00,
		0x1A, 0x00, 0x19, 0x00, 0x06, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
		0x34, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	libue4header = SINGLEAOBSCAN(ue4head, sizeof(ue4head));
	return libue4header;
}