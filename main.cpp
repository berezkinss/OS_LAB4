#include <iostream>
#include <string>
#include <windows.h>

#define N 16 // 030808

using namespace std;

int main() {
	HANDLE free, used, mutex, file, map, writers[N], readers[N];
	LPVOID mapview;
	DWORD filesize;
	DWORD err;
	free = CreateSemaphoreA(NULL, N, N, "free");
	used = CreateSemaphoreA(NULL, 0, N, "used");
	mutex = CreateMutexA(NULL, false, "mutex");
	file = CreateFileA("C:\\Users\\sasha\\Documents\\os4test\\buffer.txt", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	filesize = N * sysInfo.dwPageSize;
	map = CreateFileMappingA(file, NULL, PAGE_READWRITE, 0, filesize, "map");
	err = GetLastError();
	mapview = MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0, 0, filesize);
	VirtualLock(mapview, filesize);

	//writers
	for (int i = 0; i < N; ++i) {
		wstring log = L"C:\\Users\\sasha\\Documents\\os4test\\wlog\\writer_" + to_wstring(i) + L".txt";
		STARTUPINFO startInfo;
		PROCESS_INFORMATION procInfo;
		SECURITY_ATTRIBUTES secureAttr = { sizeof(secureAttr), NULL, TRUE };
		ZeroMemory(&startInfo, sizeof(startInfo));
		HANDLE out = CreateFile(log.data(), GENERIC_WRITE, FILE_SHARE_WRITE, &secureAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		startInfo.cb = sizeof(startInfo);
		startInfo.hStdOutput = out;
		startInfo.hStdError = NULL;
		startInfo.hStdInput = NULL;
		startInfo.dwFlags |= STARTF_USESTDHANDLES;
		ZeroMemory(&procInfo, sizeof(procInfo));
		if (CreateProcess(L"C:\\Users\\sasha\\Documents\\os4test\\exe\\writer.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startInfo, &procInfo))
			writers[i] = procInfo.hProcess;
	}
	//readers 
	for (int i = 0; i < N; ++i) {
		wstring log = L"C:\\Users\\sasha\\Documents\\os4test\\rlog\\reader_" + to_wstring(i) + L".txt";
		STARTUPINFO startInfo;
		PROCESS_INFORMATION procInfo;
		SECURITY_ATTRIBUTES secureAttr = { sizeof(secureAttr), NULL, TRUE };
		ZeroMemory(&startInfo, sizeof(startInfo));
		HANDLE out = CreateFile(log.data(), GENERIC_WRITE, FILE_SHARE_WRITE, &secureAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		startInfo.cb = sizeof(startInfo);
		startInfo.hStdOutput = out;
		startInfo.dwFlags |= STARTF_USESTDHANDLES;
		ZeroMemory(&procInfo, sizeof(procInfo));
		if (CreateProcess(L"C:\\Users\\sasha\\Documents\\os4test\\exe\\reader.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startInfo, &procInfo))
			readers[i] = procInfo.hProcess;
	}

	WaitForMultipleObjects(N, writers, true, INFINITE);
	WaitForMultipleObjects(N, readers, true, INFINITE);

	for (int i = 0; i < N; ++i) {
		CloseHandle(writers[i]);
		CloseHandle(readers[i]);
	}
	CloseHandle(free);
	CloseHandle(used);
	CloseHandle(mutex);
	CloseHandle(mapview);
	CloseHandle(map);
	CloseHandle(file);

	return 0;
}
