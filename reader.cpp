#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <windows.h>

#define N 16

using namespace std;

int main() {
	HANDLE free, used, mutex, out, map;
	DWORD point = 0;
	LONG page = -1;
	string output;
	LPVOID buf;
	char* res;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	free = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, "free");
	used = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, "used");
	mutex = OpenMutexA(MUTEX_ALL_ACCESS, false, "mutex");
	out = GetStdHandle(STD_OUTPUT_HANDLE);
	map = OpenFileMappingA(GENERIC_READ, false, "map");
	buf = MapViewOfFile(map, FILE_MAP_READ, 0, 0, N * sysInfo.dwPageSize);
	srand(time(0));

	if (map) {
		for (int i = 0; i < 3; ++i) {
			WaitForSingleObject(mutex, INFINITE);
			output = "Time: " + to_string(GetTickCount()) + " | Lock mutex\n";
			WriteFile(out, output.data(), output.length(), &point, NULL);

			WaitForSingleObject(used, INFINITE);
			output = "Time: " + to_string(GetTickCount()) + " | Take used semaphore\n";
			WriteFile(out, output.data(), output.length(), &point, NULL);

			if (ReleaseSemaphore(free, 1, &page)) {
				//page = N - page - 1;
				res = (char*)((long long)buf + page * sysInfo.dwPageSize);
				output = "Time: " + to_string(GetTickCount()) + " | Read: " + res + " | Page: " + to_string(page) + "\n";
				WriteFile(out, output.data(), output.length(), &point, NULL);
				output = "Time: " + to_string(GetTickCount()) + " | Add free semaphore\n";
				WriteFile(out, output.data(), output.length(), &point, NULL);
				Sleep(rand() % 1000 + 500);
			}
			else {
				output = "Time: " + to_string(GetTickCount()) + " | Semaphore error" + " | Error code: " + to_string(GetLastError()) + "\n";
				WriteFile(out, output.data(), output.length(), &point, NULL);
			}

			if (ReleaseMutex(mutex)) {
				output = "Time: " + to_string(GetTickCount()) + " | Unlock mutex\n";
				WriteFile(out, output.data(), output.length(), &point, NULL);
			}
			else {
				output = "Time: " + to_string(GetTickCount()) + " | Mutex error" + " | Error code: " + to_string(GetLastError()) + "\n";
				WriteFile(out, output.data(), output.length(), &point, NULL);
			}
		}
	}
	else WriteFile(out, "Mapping failed\n", strlen("Mapping failed\n"), &point, NULL);

	CloseHandle(out);

	return 0;
}
