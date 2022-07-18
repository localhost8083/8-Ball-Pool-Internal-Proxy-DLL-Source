#pragma once

#include <Windows.h>
int getProcId();
struct Memory
{
	Memory() {}

	virtual UINT32 getGameModuleBase();
	UINT32 getGameModuleBase2();
	virtual BOOL   read(UINT32 address, PVOID buffer, UINT32 bufferSize);
	virtual BOOL   write(UINT32 address, PVOID buffer, UINT32 bufferSize);
	static void offsetsearch2(int offset, BYTE write[], SIZE_T size, int header);


	template <typename T>
	inline T read(UINT32 address)
	{
		T buffer{};

		read(address, &buffer, sizeof(buffer));

		return buffer;
	}

	template <typename T>
	inline T read32Pointer(UINT32 address)
	{
		UINT32 buffer{};

		read(address, &buffer, sizeof(buffer));

		return reinterpret_cast<T>(buffer);
	}

	template <typename T>
	inline BOOL write(UINT32 address, T value)
	{
		return write(address, &value, sizeof(value));
	}

	static DWORD  getProcessIDByName(const char* processName);
	static HANDLE getProcessHandle();
};
