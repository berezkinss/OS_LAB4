#define _CRT_SECURE_NO_WARNINGS 
#pragma comment(lib, "winmm.lib") 
#include <windows.h>
#include <iostream>
#include <string>
#include <stdlib.h>

#define PAGES 28 

using std::string;
using std::to_string;
using std::strlen;

HANDLE* semaphores_for_writers = new HANDLE[PAGES]; // семафоры писателей для страниц памяти
HANDLE* semaphores_for_readers = new HANDLE[PAGES]; // семафоры читателей для страниц памяти

void opening_semaphores()
{
    string semaphore_name;
    for (int i = 0; i < PAGES; ++i)
    {
        semaphore_name = "semaphore " + to_string(i) + " (writer)";
        semaphores_for_writers[i] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, semaphore_name.c_str()); 
        semaphore_name = "semaphore " + to_string(i) + " (reader)";
        semaphores_for_readers[i] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, semaphore_name.c_str()); 
    }
}

void log_page(DWORD page)
{
    string logname = "D:\\OS4\\pages for excel\\page " + to_string(page) + ".txt";
    FILE *log = fopen(logname.c_str(), "w");

}

int main()
{
    srand(time(NULL)); 

    
    SYSTEM_INFO system_info;
    int buffer_size;
    GetSystemInfo(&system_info);
    buffer_size = PAGES * system_info.dwPageSize;

    string buffer_name, mapped_name;
    HANDLE mapped_handle;
    LPVOID address;

    buffer_name = "D:\\OS4\\buffer.txt";
    mapped_name = "buffer";

    
    mapped_handle = OpenFileMappingA(GENERIC_READ, FALSE, mapped_name.c_str());
    address = MapViewOfFile(mapped_handle, FILE_MAP_READ, 0, 0, buffer_size);

    
    FILE *logfile, *logfile2;
    string logfile_name, logfile_name2;
    logfile_name = "D:\\OS4\\reading logs\\reader log " + to_string(GetCurrentProcessId()) + ".txt";
    logfile = fopen(logfile_name.c_str(), "w");

    logfile_name2 = "D:\\OS4\\reader for excel\\reader log " + to_string(GetCurrentProcessId()) + ".txt";
    logfile2 = fopen(logfile_name2.c_str(), "w");

    opening_semaphores(); 

    char *data; 

    DWORD start_time = timeGetTime();
    while (timeGetTime() < start_time + 15000)
    {
        fprintf(logfile, "|Process: %d|\t\t\t|State: WAITING|\t\t\t\t\t|time: %d|\n", GetCurrentProcessId(), timeGetTime());
        fprintf(logfile2, "%d %d\n", timeGetTime(), 0);

        DWORD page = WaitForMultipleObjects(PAGES, semaphores_for_readers, FALSE, INFINITE); 

        fprintf(logfile, "|Process: %d|\t\t\t|State: READING; Page: %d|\t\t\t\t|time: %d|\n", GetCurrentProcessId(), page, timeGetTime());
        fprintf(logfile2, "%d %d\n", timeGetTime(), 1);

        int offset = page * system_info.dwPageSize; 
        data = (char*)((long long)address + offset); 
        Sleep(500 + rand() % 1001); 

        string logname = "D:\\OS4\\pages for excel\\page " + to_string(page) + ".txt";
        FILE *log = fopen(logname.c_str(), "a");
        fprintf(log, "%d %d\n", timeGetTime(), 2);
        ReleaseSemaphore(semaphores_for_writers[page], 1, NULL);
        fprintf(log, "%d %d\n", timeGetTime(), 0);
        fclose(log);

        fprintf(logfile, "|Process: %d|\t\t\t|State: RELEASED; Read data: %s|\t\t\t|time: %d|\n\n", GetCurrentProcessId(), data, timeGetTime());
        fprintf(logfile2, "%d %d\n", timeGetTime(), 2);
    }

    return 0;
}
