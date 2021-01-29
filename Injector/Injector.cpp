#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>

#define DLL_PATH		"R:/Projects/CoD4/Shadowjump/DLL/Debug/Shadowjump.dll"
#define PROCESS_NAME	"iw3mp.exe"

using namespace std;

/// <summary>
/// Load the DLL specified by dllPath into the process specified by processId
/// </summary>
/// <param name="pid">ID of the process to inject the DLL into</param>
/// <param name="DLL_Path">Path of the DLL to inject</param>
/// <param name="pThreadId">Buffer for resulting threadId of injected dll</param>
/// <returns>true if successful</returns>
bool injectDLL(DWORD processId, string dllPath, DWORD *pThreadId)
{
    cout << "Attempting to inject DLL with path '" << dllPath << "' into process with id " << processId << std::endl;

    // Open the process
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
    if (hProc == NULL)
    {
        cerr << "Can't open process" << std::endl;
        return false;
    }

    // Allocate memory in the process to store the DLL path in
    size_t dllPathStrSize = dllPath.length() + 1;
    LPVOID allocatedMemory = VirtualAllocEx(hProc, NULL, dllPathStrSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (allocatedMemory == NULL)
    {
        cerr << "Can't allocate memory" << std::endl;
        return false;
    }

    // Write the DLL path to the newly allocated memory
    if (WriteProcessMemory(hProc, allocatedMemory, dllPath.c_str(), dllPathStrSize, 0) == 0)
    {
        cerr << "Couldn't write DLL path into newly allocated memory in process" << std::endl;
        return false;
    }

    // Load the kernel32 dll because nearly every process uses it, including iw3mp.exe
    HMODULE kernel32Handle = LoadLibraryA("kernel32");
    if (!kernel32Handle)
    {
        cerr << "Failed to load kernel32 library" << std::endl;
        return false;
    }

    // Find the LoadLibraryA address within the kernel32.dll so that we can use it to load our DLL from within the process
    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32Handle, "LoadLibraryA");

    // Call the LoadLibrary function on the memory in the process where the path of our DLL was written
    if (CreateRemoteThread(hProc, NULL, 0, loadLibraryAddr, allocatedMemory, 0, pThreadId) == NULL)
    {
        cerr << "Failed to create remote thread" << std::endl;
        return false;
    }

    return true;
}

/// <summary>
/// Find the id of a process by its name. Returns MAXDWORD if not found
/// </summary>
/// <param name="processName">The name of the process for which to find pid</param>
/// <returns>The pid if found, otherwise MAXDWORD</returns>
DWORD getProcessIdByName(string processName)
{
    DWORD processId = MAXDWORD;

    // Create a buffer for the snapshot entries to be stored in
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // Create a snapshot of all current processes
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    // Loop through the processes until we find what we want
    while (Process32Next(snapshot, &entry) == TRUE)
    {
        if (_wcsicmp(entry.szExeFile, L"iw3mp.exe") == 0)
        {
            // Found the id of the desired process
            processId = entry.th32ProcessID;
            break;
        }
    }

    CloseHandle(snapshot);
    return processId;
}

int main()
{
    DWORD processId;
    if ((processId = getProcessIdByName(PROCESS_NAME)) == MAXDWORD)
    {
        cerr << "Failed to find id of process with name '" << PROCESS_NAME << "'" << std::endl;
        return 1;
    }

    cout << "Found id of process with name '" << PROCESS_NAME << "' to be " << processId << std::endl;

    DWORD threadId;
    if (!injectDLL(processId, DLL_PATH, &threadId))
    {
        cerr << "Failed to load DLL from path '" << DLL_PATH << "' into process with id " << processId << std::endl;
        return 1;
    }

    cout << "Successfully loaded DLL from path '" << DLL_PATH << "' into process with id " << processId << std::endl;
    
    (void)cin.get();
    return 0;
}