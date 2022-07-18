#ifndef DumpCode

#include "CodeManager.h"
#include "Utils.h"

#include <fstream>
#include "const.h"
#include "skCrypt.h"
#include "GlobalVars.h"

static CodeManager  codeManager;
CodeManager*        gCodeManager = &codeManager;

CodeManager::CodeManager()
{
    HMODULE instance;
    DWORD   errorCode;

    this->offset = 0UL;
    this->allocatedMemoryBase = VirtualAlloc(nullptr, 1024ULL * 1024ULL, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE); // allocate 1 MB
    if (this->allocatedMemoryBase == nullptr) {
        errorCode = GetLastError();
        fatal(skCrypt("Couldn't allocate memory"), skCrypt("VirtualAlloc"), errorCode);
    }

    instance        = GetModuleHandle(nullptr);
    requestFileData = reinterpret_cast<RequestFileDataType>(GetProcAddress(instance, skCrypt("requestFileData")));
    //if (requestFileData == nullptr)
        //exit(003); // probably he is trying to crack. 

    gGlobalVars->makeRequestFunc = reinterpret_cast<MakeRequestType>(GetProcAddress(instance, skCrypt("makeRequest")));
    //if (gGlobalVars->makeRequestFunc == nullptr)
        //exit(004);

    gGlobalVars->selfDeleteFunc = reinterpret_cast<SelfDeleteType>(GetProcAddress(instance, skCrypt("selfDelete")));
    //if (gGlobalVars->selfDeleteFunc == nullptr)
       // exit(005);
}

CodeManager::~CodeManager()
{
    VirtualFree(this->allocatedMemoryBase, 0UL, MEM_RELEASE);
}

SIZE_T CodeManager::getFuncAddressInternal(const char* filename)
{
    int            i;
    ResponseBuffer buffer;
    SIZE_T         funcPtr;

    this->functionsMutex.lock();
    const auto& func = functions.find(filename);
    if (func == this->functions.end()) {
        requestFileData(filename, &buffer);
        for (i = 0; i < buffer.size; i++) {
            buffer.data[i] ^= rawDataEncryptionKey[i % rawDataEncryptionKeySize];
        }

        funcPtr = reinterpret_cast<SIZE_T>(this->allocatedMemoryBase) + this->offset;
        memcpy(reinterpret_cast<PVOID>(funcPtr), buffer.data, buffer.size);
        this->functions[filename] = funcPtr;
        this->offset += buffer.size;
    }
    else
    {
        funcPtr = func->second;
    }

    this->functionsMutex.unlock();

    return funcPtr;
}

void CodeManager::clear()
{
    this->functionsMutex.lock();
    this->functions.clear();
    this->offset = 0;
    this->functionsMutex.unlock();
}

#endif