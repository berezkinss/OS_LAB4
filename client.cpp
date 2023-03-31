#include <windows.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::cin;
using std::string;

void CALLBACK CompletionRoutine(DWORD errorCode, DWORD bytestransfered, LPOVERLAPPED lpOverlapped) 
{

}

int main()
{
	HANDLE pipe;
	string pipename;
	char *data = new char[256];
	OVERLAPPED overlapped;

	ZeroMemory(&overlapped, sizeof(overlapped)); 

	pipename = "\\\\.\\pipe\\pepe"; 
	pipe = CreateFileA(pipename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 

	cout << "You will see results below\n\n";
	BOOL result;
	while (true)
	{
		result = ReadFileEx(pipe, data, 256, &overlapped, CompletionRoutine); 
		if (result && pipe != INVALID_HANDLE_VALUE)
		{
			cout << data << endl << endl; 
			SleepEx(INFINITE, TRUE); 
		}
		else 
		{
			cout << "Client finished\n";
			break; 
		}
	}

	return 0;
}
