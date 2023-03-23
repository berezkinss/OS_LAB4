#include <windows.h>
#include <iostream>

#pragma comment(lib, "winmm.lib")

using namespace std;

int main()
{
	system("chcp 1251");
	system("cls");

	CHAR buffer[512];										
	BOOL isConnected = FALSE;								
	OVERLAPPED overlapped = OVERLAPPED();					
	OVERLAPPED syncPipe = OVERLAPPED();						

	HANDLE Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE Pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\berezkin"),							
								  PIPE_ACCESS_DUPLEX,										
								  PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,	
																							 
								  PIPE_UNLIMITED_INSTANCES,									
								  512,														
								  512,														
								  0,														
								  NULL														
								  );

	int choose;


	if (Event != INVALID_HANDLE_VALUE && Pipe != INVALID_HANDLE_VALUE)
	{
		do
		{
			system("cls");

			cout << "+---+---------------------------------------+" << endl;
			cout << "| 1 | Присоединиться к именованному каналу  |" << endl;
			cout << "+---+---------------------------------------+" << endl;
			cout << "| 2 | Отправить сообщение                   |" << endl;
			cout << "+---+---------------------------------------+" << endl;
			cout << "| 3 | Отсоединиться от именованного канала  |" << endl;
			cout << "+---+---------------------------------------+" << endl;
			cout << "| 0 | Выйти                                 |" << endl;
			cout << "+---+---------------------------------------+" << endl;

			cin >> choose;

			switch (choose) {

			case 1:

					syncPipe.hEvent = Event;

					isConnected = ConnectNamedPipe(Pipe, &syncPipe);
					WaitForSingleObject(Event, INFINITE);

					if (isConnected)
						cout << "Подключение прошло успешно!" << endl;
					else
						cout << "Не удалось подключиться к именованному каналу!" << endl;

				system("pause");

				break;

			case 2:

				if (isConnected == FALSE) cout << "Нет соединения!" << endl;
				else {

					cout << "Введите сообщение: ";
					cin >> buffer;

					overlapped.hEvent = Event;
					isConnected = WriteFile(Pipe, (LPCVOID)buffer, 512, NULL, &overlapped);

					if (WaitForSingleObject(Event, 20000) == WAIT_OBJECT_0 && isConnected)
						cout << "Запись удалась!" << endl;
					else
						cout << "Запись не удалась!" << endl;

				}

				cout << endl;

				system("pause");

				break;

			case 3:

				isConnected = DisconnectNamedPipe(Pipe);

				if (isConnected)
					cout << "Вы были отсоединены от именованного канала!" << endl;
				else
					cout << "Не удалось отсоединиться!" << endl;

				isConnected = FALSE;

				system("pause");

				break;

			case 0:

				break;

			default:

				break;

			}
		} while (choose);
	}
	else
		cout << "Не удалось создать именованный канал, перезапустите программу!" << endl;

	if (Pipe != INVALID_HANDLE_VALUE)
		CloseHandle(Pipe);
	if (Event != INVALID_HANDLE_VALUE)
		CloseHandle(Event);


	return 0;
}
