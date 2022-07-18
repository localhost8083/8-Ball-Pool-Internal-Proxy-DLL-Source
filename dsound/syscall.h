#pragma once
#pragma once
#include <Windows.h>
#include <iostream>
#include <bcrypt.h>

using namespace std;

template<typename T = NTSTATUS>
class makesyscall
{
public:
    makesyscall(LPCSTR lpFuncName);
    ~makesyscall();
public:
    template<typename... Args>
    T operator()(Args... arg);
private:
    uint32_t	GetSyscallIndex(const BYTE* pFuncAddr);
    void		CreateShellSysCall(const uint32_t sysindex);
private:
    BYTE* m_pShellCode;
};

template<typename T> template<typename... Args>
T makesyscall<T>::operator()(Args... arg)
{
    if (!m_pShellCode)
        return T(0);

    typedef T(__cdecl* mFunc)(Args...);
    mFunc pFunc = mFunc(m_pShellCode);

    return pFunc(arg...);
}

template<typename T>
makesyscall<T>::makesyscall(LPCSTR lpFuncName)
{
    PBYTE pFuncAddr = (PBYTE)GetProcAddress(GetModuleHandleA("ntdll.dll"), lpFuncName);
    CreateShellSysCall(GetSyscallIndex(pFuncAddr));
}

template<typename T>
makesyscall<T>::~makesyscall()
{
    if (m_pShellCode)
        VirtualFree(m_pShellCode, NULL, MEM_RELEASE);
}

template<typename T>
uint32_t makesyscall<T>::GetSyscallIndex(const BYTE* pFuncAddr)
{
    if (!pFuncAddr)
        return 0;

    for (int8_t i = 0; i < 0xF; ++i)
    {
        if (*(pFuncAddr + i) == 0xB8)
        {
            return *PUINT(pFuncAddr + i + 1);
        }
    }

    return 0;
}

template<typename T>
void makesyscall<T>::CreateShellSysCall(const uint32_t sysindex)
{
    if (!sysindex)
        return;

#ifdef _WIN64
    BYTE ShellCode[]
    {
        0x4C, 0x8B, 0xD1,					//mov r10, rcx 
        0xB8, 0x00, 0x00, 0x00, 0x00,		        //mov eax, SysCallIndex
        0x0F, 0x05,					        //syscall
        0xC3								//ret				
    };
#elif _WIN32
    BYTE ShellCode[]
    {
        0x90, 0x90, 0x90,					//nop, nop, nop
        0xB8, 0x00, 0x00, 0x00, 0x00,		        //mov eax, SysCallIndex
        0xBA, 0x00, 0x00, 0x00, 0x00,		        //mov edx, [Wow64Transition]
        0xFF, 0xD2,						//call edx
        0xC3								//ret
    };

    *PDWORD(ShellCode + 9) = *(PDWORD)GetProcAddress(GetModuleHandleA("ntdll.dll"), "Wow64Transition");
#endif

    * PDWORD(ShellCode + 4) = sysindex;

    m_pShellCode = (PBYTE)VirtualAlloc(nullptr, sizeof(ShellCode), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if (!m_pShellCode)
        return;

    memcpy(m_pShellCode, ShellCode, sizeof(ShellCode));
}