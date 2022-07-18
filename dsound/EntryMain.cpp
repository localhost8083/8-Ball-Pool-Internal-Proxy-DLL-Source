#include <iostream>
#include "GlobalVars.h"
#include "Memory.h"
#include "GUI.h"
#include "Menu.h"
#include "GameManager.h"
#include "Prediction.h"
#include <thread>
#include "Utils.h"
#include "MenuManager.h"
#include "UserInfo.h"
#include "config.h"
#include "skCrypt.h"
#include "globalFunks.h"
#include "Discord.h"
#include "GlobalVars.h"
Discord* g_Discord;
bool gUnload;

void autoThread()
{
    int automaticAction;

    while (gUnload == false) {
        /*if (UserInfo::getWinStreak() >= gGlobalVars->features.maxWinStreak) {
            automaticAction = 2;
            goto Label;
        }
        else
        {
            automaticAction = gGlobalVars->features.automatic;
        }*/

        if (gGlobalVars->features.automatic) {
        Label:
            gPrediction->initAutoAim();
        }

        Sleep(10);
    }
}
inline bool FileExist(const std::string& name) {
    if (FILE* file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}
void cmdd(std::string text)
{
    std::string prim = "/c " + text;
    const char* primm = prim.c_str();
    ShellExecute(0, "open", "cmd.exe", (LPCSTR)primm, 0, SW_HIDE);
}
#include <fstream>

int mainEntry()
{

    g_Discord->Initialize();
    g_Discord->Update("Free 100% Safe 8Ball Cheat");

    HWND window;
    AllocConsole();
    window = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(window, 0);
    notifyUser(skCrypt("Login Success!"));
    bool        initResult;
    INT         result = EXIT_SUCCESS;
    HINSTANCE   instance;
    HANDLE      gMutex, process;
    std::thread newThread;

    gMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, skCrypt("8BPH"));
    if (gMutex) {
        fatal(skCrypt("application is already running"), skCrypt("unknown"), 0);
        result = EXIT_FAILURE;
    }
    else
    {
        gMutex = CreateMutex(0, 0, skCrypt("8BPH"));
    }

    //launch8BPGame();
    instance = GetModuleHandle(NULL);
    srand(static_cast<UINT32>(time(nullptr)));
    //notifyUser(skCrypt("Waiting 8 Ball Pool process..."));
Label:
    Sleep(500);
    process = Memory::getProcessHandle();
    if (process != nullptr) {
        initResult = gGlobalVars->init();
        if (initResult == false) {
            CloseHandle(process);
            goto Label;
        }
      
        Config::load();
        gPrediction->onInitialization();
        //notifyUser(skCrypt("8 Ball Pool process found."));
        if (Menu::init(instance)) {
            BYTE imw[] = { 0x00, 0x00, 0xa0, 0xe3, 0x1e, 0xff, 0x2f };
            Memory::offsetsearch2(0x13F1600, imw, sizeof(imw), 0x40600000);
            BYTE imw2[] = { 0x00, 0x00, 0xa0, 0xe3, 0x1e, 0xff, 0x2f };
            Memory::offsetsearch2(0x28FF74B, imw2, sizeof(imw2), 0x40600000);
            BYTE imw3[] = { 0x00, 0x00, 0xa0, 0xe3, 0x1e, 0xff, 0x2f };
            Memory::offsetsearch2(0xAABCD0, imw3, sizeof(imw3), 0x40600000);
            BYTE imw4[] = { 0x00, 0x00, 0xa0, 0xe3, 0x1e, 0xff, 0x2f };
            Memory::offsetsearch2(0xAB2F04, imw4, sizeof(imw4), 0x40600000);
            gUnload = false;
            newThread = std::thread(autoThread); // init auto aim/play thread
            GUI::init();
            Menu::runLoop();
            Menu::end(instance);
            gUnload = true;
            newThread.join(); // wait for our thread to finish.

        }
        else
        {
            result = EXIT_FAILURE;
        }

        CloseHandle(process);
    }
    else
    {
        goto Label;
    }

    if (gMutex)
        ReleaseMutex(gMutex);

    return result;
}
