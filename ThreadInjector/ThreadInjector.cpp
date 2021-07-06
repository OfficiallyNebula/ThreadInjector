#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <libloaderapi.h>


//Get Process ID from the Window title
void getProcId(const char* window_title, DWORD& process_id)
{
    GetWindowThreadProcessId(FindWindow(NULL, window_title), &process_id);
}

void error(const char* error_title, const char* error_message)
{
    MessageBox(NULL, error_message, error_title, NULL);
    exit(-1);
}

//Check if file exists
bool fileExists(std::string file_name)
{
    struct stat buffer;
    return (stat(file_name.c_str(), &buffer) == 0);
}

int main()
{
    DWORD procId = NULL;
    char dll_path[MAX_PATH];
    const char* dll_name = "Test.dll";
    const char* window_title = "AssaultCube";


    //Begin error checking
    if (!fileExists(dll_name))
    {
        error("fileExists", "File does not exist");
    }

    if (!GetFullPathName(dll_name, MAX_PATH, dll_path, nullptr))
    {
        error("GetFullPathName", "Failed to get full path");
    }

    getProcId(window_title, procId);
    if (procId == NULL)
    {
        error("getProcId", "Failed to get Process ID");
    }

    HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
    if (!h_process)
    {
        error("OpenProcess", "Failed to open a handle to process");
    }

    void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocated_memory)
    {
        error("Allocated_memory", "Failed to allocate Memory");
    }

    if (!WriteProcessMemory(h_process, nullptr, dll_path, MAX_PATH, nullptr))
    {
        error("WriteProcessMemory", "Failed to write process memory");
    }

    HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
    if (!h_thread)
    {
        error("h_thread", "Failed to create remote thread");
    }

    //End error checking

    //Close Handle to process
    CloseHandle(h_process);
    //Free memory of process
    VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);
    MessageBox(0, "Successfully Injected!", "Success", 0);


}