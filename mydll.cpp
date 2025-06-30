#include "pch.h"
#define DLL_EXPORT
#include "mydll.h"

#define FUNCTION "my_epic_function"
typedef void(*PFUNC)(void);


extern "C" DECLDIR void DisplayMessage() {
    // use GetModuleHandleA with null to get current process handle
    PFUNC pFunc = (PFUNC)GetProcAddress(GetModuleHandleA(NULL), FUNCTION);
    if (pFunc != NULL) {
        (*pFunc)();
    }
    else {
        printf("Failed to find address of my_epic_function\n");
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            DisplayMessage();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

