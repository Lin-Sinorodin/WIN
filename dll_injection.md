# DLL Injection

* __DLL Injection:__ Load a `dll` into another process, so it can run there

* Idea:
  * Select a remote process, to run our injected dll in
  * Obtain a handle to that remote process
  * On that remote process, call the `CreateRemoteThreadEx` to create a new thread on the process virtual memory space. From MSDN:
    ```c++
    HANDLE CreateRemoteThreadEx(
      [in]            HANDLE                       hProcess,
      [in, optional]  LPSECURITY_ATTRIBUTES        lpThreadAttributes,
      [in]            SIZE_T                       dwStackSize,
      [in]            LPTHREAD_START_ROUTINE       lpStartAddress,
      [in, optional]  LPVOID                       lpParameter,
      [in]            DWORD                        dwCreationFlags,
      [in, optional]  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
      [out, optional] LPDWORD                      lpThreadId
    );
    ```
  * The new thread will run the `LoadLibraryA` function
    * This function will allow us to load our `dll` to the new process
    * This function loaded into the same address in all functions, so we already know its address in the target process virtual memory space!
  * The call will look now something like this
    ```c++
    HANDLE hProcess;    // get handle to the process
    LPVOID lpDllPath;   // set path to my dll
    
    HANDLE CreateRemoteThreadEx(
      hProcess, NULL, 0, LoadLibraryA, lpDllPath, 0, NULL, NULL
    );
    ```
  * Using this, I can use the entry of my `dll` and run there anything I want, it will run on that injected process
