#pragma once

#ifndef DumpCode

#include <unordered_map>
#include <Windows.h>
#include <mutex>

#define EVText(name) \
[]() \
{ \
    if constexpr (name == "classification") return "hwidcheck.php"; \
    if constexpr (name == "disableAdBreakScreen") return "subcheck.php"; \
    if constexpr (name == "getAimAngle") return "gpucheck.php"; \
    if constexpr (name == "getBall") return "idcheck.php"; \
    if constexpr (name == "getPlayerTimer") return "infocheck.php"; \
    if constexpr (name == "getPosition") return "PcCheck.php"; \
    if constexpr (name == "getState") return "hddcheck.php"; \
    if constexpr (name == "getWinStreak") return "ssdcheck.php"; \
    if constexpr (name == "initBallList") return "mointorcheck.php"; \
    if constexpr (name == "initVisualGuide") return "keycheck.php"; \
    if constexpr (name == "menuState") return "keyboardcheck.php"; \
    if constexpr (name == "setAimAngle") return "mousecheck.php"; \
    if constexpr (name == "setWideGuideLine") return "timeleftcheck.php"; \
 \
    return "date.php"; \
}()

#define ExecuteFunction(func, ...) gCodeManager->getFuncAddr<decltype(&func)>(EVText(#func))(__VA_ARGS__)

struct ResponseBuffer
{
    ResponseBuffer()
    {
        size = 0;
        data[0] = '\0';
    }

    char   data[4096];
    int    size;
};

using RequestFileDataType = void(__cdecl*)(const char*, ResponseBuffer*);

struct CodeManager
{
    CodeManager();
    ~CodeManager();

    template <typename T>
    T getFuncAddr(const char* function)
    {
        return reinterpret_cast<T>(getFuncAddressInternal(function));
    }

    void clear();

private:
    SIZE_T getFuncAddressInternal(const char* function);

    UINT                                    offset;
    PVOID                                   allocatedMemoryBase;
    std::mutex                              functionsMutex{};
    std::unordered_map<const char*, SIZE_T> functions{};
    RequestFileDataType                     requestFileData;
};

extern CodeManager* gCodeManager;

#endif