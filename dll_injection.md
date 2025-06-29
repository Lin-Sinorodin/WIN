# DLL Injection

* __DLL Injection:__ Load a `dll` into another process, so it can run there

* Idea:
  * Use `Process Explorer` with the `find` option to find process that uses a specific `dll`
  * For example, we can look for `user32.dll`
  * We will search for processes that uses this dll
  * We will create our `dll`, load it, and get its address
  * Then, we will replace the code ad the target dll with the code of our `dll`
  * Because `dll`s are loaded once and saved in the same location in memory:
    * This action will inject the code of our `dll` to all the using processes
  * Limitations:
    * We should use a `dll` that is not heavily used, to make minimal damage to the injected process
    * We should use a `dll` that has enough size in memory, so it can contain the code of our `dll`
