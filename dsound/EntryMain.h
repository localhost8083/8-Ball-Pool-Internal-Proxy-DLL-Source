#pragma once
#include <winnt.h>
#include <minwinbase.h>
#include <processthreadsapi.h>
#include <handleapi.h>
int mainEntry();
void mainEntryStart()
{
	HANDLE Hdl;
	Hdl = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(mainEntry), nullptr, NULL, nullptr);
	CloseHandle(Hdl);
}