#include "Utils.h"

#include <format>
#include <random>
#include <Windows.h>

#include "skCrypt.h"
#include "Memory.h"

std::string GetLastErrorAsString(DWORD errorCode)
{
    LPSTR       msgBuffer = nullptr;
    std::string msg("");

    if (errorCode != 0)
    {
        SIZE_T size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL,
                SUBLANG_DEFAULT),
            (LPSTR)&msgBuffer,
            0,
            NULL);
        msg = std::string(msgBuffer, size);

        LocalFree(msgBuffer);
    }

    return msg;
}

void fatal(const char* error, const char* _where, UINT32 errorCode)
{
    std::string errorMsg = GetLastErrorAsString(static_cast<DWORD>(errorCode));
    std::string message;

    auto __error  = skCrypt("Error: {}\n");
    auto __where  = skCrypt("Where: {}\n");
    auto __reason = skCrypt("Reason: {}\n");
    auto __title  = skCrypt("Fatal Error");

    message.reserve(50UL);
    message  = std::format("Error: {}\n", error);
    message += std::format("Where: {}\n", _where);
    message += std::format("Reason: {}\n", errorMsg.c_str());

    MessageBox(nullptr, message.c_str(), __title.decrypt(), MB_OK | MB_ICONERROR);

    __error.clear();
    __where.clear();
    __reason.clear();
    __title.clear();

    exit(EXIT_FAILURE);
}

void notifyUser(const char* message, const char* title)
{
    MessageBox(nullptr, message, title, MB_OK | MB_TOPMOST);
}

void launch8BPGame()
{
    char    appMarketPath[512] = "";
    HKEY    mainKey;
    DWORD   keyType = REG_SZ, maxPathSize = sizeof(appMarketPath);
    LSTATUS result;

    auto __parameters = skCrypt("-startpkg com.miniclip.eightballpool -from DesktopLink");
    auto __subKey = skCrypt("SOFTWARE\\WOW6432Node\\Tencent\\MobileGamePC\\AppMarket\\");

    if (Memory::getProcessIDByName(skCrypt("aow_exe.exe")) != NULL)
        return;

    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, __subKey.decrypt(), 0UL, KEY_READ, &mainKey);
    if (result == ERROR_SUCCESS) {
        result = RegQueryValueEx(mainKey, skCrypt("InstallPath"), 0, &keyType, reinterpret_cast<BYTE*>(appMarketPath), &maxPathSize);
        if (result == ERROR_SUCCESS) {
            strcat(appMarketPath, skCrypt("\\AppMarket.exe"));
            ShellExecute(nullptr, nullptr, appMarketPath, __parameters.decrypt(), nullptr, SW_HIDE);
        }

        RegCloseKey(mainKey);
    }

    __subKey.clear();
    __parameters.clear();
}

int getRandomInt(const int& min, const int& max)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(min, max);

    return uni(rng);
}

float getRandomFloat(const float& min, const float& max)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> uni(min, max);

    return uni(rng);
}