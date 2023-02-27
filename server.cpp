#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

int main() {
	HANDLE pipe;
	OVERLAPPED ovr;
	DWORD written;
	string name, input;
	bool work = true;
	ZeroMemory(&ovr, sizeof(ovr));
	ovr.hEvent = CreateEventA(NULL, true, false, "event");
	name = "\\\\.\\pipe\\newpipe";
	pipe = CreateNamedPipeA(name.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 256, 256, 0, NULL);
	ConnectNamedPipe(pipe, &ovr);
	while (work) {
		system("cls");
		printf("Enter something (enter 0 to exit): ");
		cin >> input;
		if (input == "0") {
			work = false;
			continue;
		}
		input += '\0';
		WriteFile(pipe, input.data(), input.length() * sizeof(char), &written, &ovr);
		WaitForSingleObject(ovr.hEvent, INFINITE);
	}
	DisconnectNamedPipe(pipe);

	return 0;
}
