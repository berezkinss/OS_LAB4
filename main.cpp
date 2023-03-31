#include <windows.h>
#include <iostream>
#include <string>

#define PAGES 28

using std::string;
using std::to_string;
using std::cout;

HANDLE *semaphores_for_writers = new HANDLE[PAGES]; 
HANDLE *semaphores_for_readers = new HANDLE[PAGES]; 
HANDLE *running_processes = new HANDLE[PAGES << 1];

void creating_semaphores(bool is_writer)
{
	string semaphore_name;
	for (int i = 0; i < PAGES; ++i)
	{
		if (is_writer) // создаём семафоры писателей для страниц памяти
		{
			semaphore_name = "semaphore " + to_string(i) + " (writer)";
			semaphores_for_writers[i] = CreateSemaphoreA(NULL, 1, 1, semaphore_name.c_str()); 
		}
		else // создаём семафоры читателей для страниц памяти
		{
			semaphore_name = "semaphore " + to_string(i) + " (reader)";
			semaphores_for_readers[i] = CreateSemaphoreA(NULL, 0, 1, semaphore_name.c_str()); 
		}
	}
}

void creating_processes(bool is_writer)
{
	LPSTARTUPINFOA startup_info = new STARTUPINFOA[PAGES]; 
	LPPROCESS_INFORMATION process_information = new PROCESS_INFORMATION[PAGES]; 
	string process_name;
	if (is_writer) // имя exe-файла писателя
	{
		process_name = "D:\\OS4\\exe\\os_lab_04_1_writer.exe";
	}
	else // имя exe-файла читателя
	{
		process_name = "D:\\OS4\\exe\\os_lab_04_1_reader.exe";
	}
	for (int i = 0; i < PAGES; ++i)
	{
		ZeroMemory(&startup_info[i], sizeof(startup_info[i])); // зануляем блок памяти для STARTUPINFOA
		CreateProcessA(process_name.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startup_info[i], &process_information[i]); 
		if (is_writer)
		{
			running_processes[i] = process_information[i].hProcess; 
		}
		else
		{
			running_processes[PAGES + i] = process_information[i].hProcess; 
		}
	}
}

int main()
{
	
	SYSTEM_INFO system_info;
	int buffer_size;
	GetSystemInfo(&system_info);
	buffer_size = PAGES * system_info.dwPageSize;

	string buffer_name, mapped_name;
	HANDLE buffer_handle, mapped_handle;
	LPVOID address;

	buffer_name = "D:\\OS4\\buffer.txt";
	mapped_name = "buffer";

	
	buffer_handle = CreateFileA(buffer_name.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	mapped_handle = CreateFileMappingA(buffer_handle, NULL, PAGE_READWRITE, 0, buffer_size, mapped_name.c_str());
	address = MapViewOfFile(mapped_handle, FILE_MAP_WRITE, 0, 0, buffer_size);
	VirtualLock(address, buffer_size); 

	
	creating_semaphores(true);
	creating_semaphores(false);

	
	creating_processes(true);
	creating_processes(false);

	WaitForMultipleObjects(PAGES << 1, running_processes, TRUE, INFINITE); 

	return 0;
}
