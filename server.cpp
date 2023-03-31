#include <windows.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::cin;
using std::string;

int main()
{
	HANDLE pipe;
	string pipename, eventname;
	char *data = new char[256];
	OVERLAPPED overlapped;
	DWORD written;

	ZeroMemory(&overlapped, sizeof(overlapped)); 

	eventname = "someevent";
	overlapped.hEvent = CreateEventA(NULL, TRUE, FALSE, eventname.c_str()); 

	pipename = "\\\\.\\pipe\\pepe"; 
	pipe = CreateNamedPipeA(pipename.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 256, 256, 0, NULL); 
	ConnectNamedPipe(pipe, &overlapped);

	cout << "Enter the data until you enter the word \"stop\"\n\n";
	while (true)
	{
		cin >> data;
		if (strcmp(data, "stop") != 0) 
		{
			cout << endl;
			WriteFile(pipe, data, strlen(data) + 1, &written, &overlapped); 
			WaitForSingleObject(overlapped.hEvent, INFINITE); 
		}
		else // если ввели слово "stop"
		{
			cout << "\nServer finished\n";
			break; 
		}
	}

	DisconnectNamedPipe(pipe);

	return 0;
}
