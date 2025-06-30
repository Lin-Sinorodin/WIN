#include <iostream>
#include "windows.h"
#include <tchar.h>
#include <psapi.h>

// modified from https://learn.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes
void GetProcessName(DWORD processID, LPWSTR processName) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (NULL != hProcess) {
        HMODULE hModule;
        DWORD cbNeeded;
        if (EnumProcessModulesEx(hProcess, &hModule, sizeof(hModule), &cbNeeded, LIST_MODULES_64BIT)) {
            GetModuleBaseNameW(hProcess, hModule, processName, MAX_PATH);
        }
        CloseHandle(hProcess);
    }
}


// modified from https://learn.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes
DWORD FindProcessByName(LPCWSTR targetName) {
    // Get the list of process identifiers.
    DWORD aProcesses[1024], cbNeeded;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 1;
    }

    // Calculate how many process identifiers were returned.
    DWORD cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.
    for (int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            WCHAR processName[MAX_PATH] = TEXT("<unknown>");
            GetProcessName(aProcesses[i], processName);
            if (wcscmp(processName, targetName) == 0) {
                _tprintf(TEXT("Found process \"%s\" at PID: %05u\n"), processName, aProcesses[i]);
                return aProcesses[i];
            }
        }
    }
    return -1;
}


void InjectDLL(LPCWSTR targetProcessName, LPCSTR dllPath) {
    SIZE_T dllPathLen = strlen(dllPath) + 1;  // include null terminator

    // try to find the PID of the target process
    DWORD processID = FindProcessByName(targetProcessName);
    if (processID < 0) {
        std::cout << "Failed locate target process\n";
        return;
    }

    // open a handle to target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processID);
    if (hProcess == NULL) {
        std::cout << "Failed open process\n";
        return;
    }

    // allocate memory in remote thread to store dll path
    LPVOID buffer = VirtualAllocEx(hProcess, NULL, dllPathLen, MEM_COMMIT, PAGE_READWRITE);
    if (buffer != NULL) {
        // copy dll path to remote process virtual memory address
        if (WriteProcessMemory(hProcess, buffer, dllPath, dllPathLen, NULL)) {
            // try to create a new thread that loads the injected DLL (and runs its entry function)
            HANDLE newThread = CreateRemoteThreadEx(
                    hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, buffer, 0, NULL, NULL
            );
            if (newThread != NULL) {
                std::cout << "injected dll: " << dllPath << " to target process: " << processID << std::endl;
            }
            else {
                std::cout << "failed running dll injection\n";
            }
        }
        else {
            std::cout << "failed write dll path on remote process, error: " << GetLastError() << std::endl;
        }

        // release all allocated memory on the remote process
        VirtualFreeEx(hProcess, buffer, dllPathLen, MEM_DECOMMIT | MEM_RELEASE);
    }
    else {
        std::cout << "failed allocate memory on remote process, error: " << GetLastError() << std::endl;
    }

    CloseHandle(hProcess);
}


int main() {
    LPCWSTR targetProcessName = L"notepad.exe";
    LPCSTR dllPath = "C:\\Users\\hp\\Downloads\\my_awesome_dll.dll";
    InjectDLL(targetProcessName, dllPath);
    return 0;
}