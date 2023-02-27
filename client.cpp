#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

void CALLBACK CompletionRoutine(DWORD errorCode, DWORD bytestransfered, LPOVERLAPPED lpOverlapped) { }

int main() {
	HANDLE pipe;
	string name;
	char* data = new char[512];
	OVERLAPPED ovr;
	BOOL work = true, get;
	ZeroMemory(&ovr, sizeof(ovr));
	name = "\\\\.\\pipe\\newpipe";
	pipe = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("Server data: \n");
	while (work) {
		get = ReadFileEx(pipe, data, 512, &ovr, CompletionRoutine);
		if (get && pipe != INVALID_HANDLE_VALUE) {
			SleepEx(INFINITE, true);
      printf("%s\n", data);
		}
		else work = false;
	}

	return 0;
}
