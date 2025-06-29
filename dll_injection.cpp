#include <iostream>
#include "windows.h"
#include <tchar.h>
#include <psapi.h>


int main()
{
    LPCSTR dll_path = "C:\\Users\\hp\\source\\repos\\mydll\\x64\\Release\\mydll.dll";
    SIZE_T dll_path_len = strlen(dll_path) + 1;  // include null terminator
    DWORD processID = 15524;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processID);
    if (hProcess == NULL) {
        std::cout << "Failed open process\n";
        return -1;
    }

    // allocate memory in remote thread to store dll path
    LPVOID buffer = VirtualAllocEx(hProcess, NULL, dll_path_len, MEM_COMMIT, PAGE_READWRITE);
    if (buffer != NULL) {
        std::cout << "allocated buffer at: " << buffer << std::endl;

        // copy dll path to remote process virtual memory address
        SIZE_T wrote;
        if (WriteProcessMemory(hProcess, buffer, dll_path, dll_path_len, &wrote)) {
            std::cout << "wrote dll path to process memory, bytes: " << wrote << std::endl;

            // try to create a new thread that loads the injected DLL (and runs its entry function)
            HANDLE newThread = CreateRemoteThreadEx(
                hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, buffer, 0, NULL, NULL
            );
            if (newThread == NULL) {
                std::cout << "failed running dll injection\n";
            }
        }

        // release all allocated memory on the remote process
        VirtualFreeEx(hProcess, buffer, dll_path_len, MEM_DECOMMIT | MEM_RELEASE);
    }
    else {
        std::cout << GetLastError() << std::endl;
    }
    
    CloseHandle(hProcess);
}
