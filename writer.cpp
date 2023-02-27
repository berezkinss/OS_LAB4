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
	string output, saved;
	LPVOID buf;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	free = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, "free");
	used = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, "used");
	mutex = OpenMutexA(MUTEX_ALL_ACCESS, false, "mutex");
	out = GetStdHandle(STD_OUTPUT_HANDLE);
	map = OpenFileMappingA(GENERIC_WRITE, false, "map");
	buf = MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, sysInfo.dwPageSize * N);
	srand(time(0));

	if (map) {
		for (int i = 0; i < 3; ++i) {
			WaitForSingleObject(mutex, INFINITE);
			output = "Time: " + to_string(GetTickCount()) + " | Lock mutex\n";
			WriteFile(out, output.data(), output.length(), &point, NULL);

			WaitForSingleObject(free, INFINITE);
			output = "Time: " + to_string(GetTickCount()) + " | Take free semaphore\n";
			WriteFile(out, output.data(), output.length(), &point, NULL);

			if (ReleaseSemaphore(used, 1, &page)) {
				saved = to_string(GetCurrentProcessId() + (rand() % 100)) + "    ";
				memcpy((LPVOID)((long long)buf + (page * sysInfo.dwPageSize)), saved.data(), saved.length() * sizeof(char));
				output = "Time: " + to_string(GetTickCount()) + " | Saved: " + saved + " | Page: " + to_string(page) + "\n";
				WriteFile(out, output.data(), output.length(), &point, NULL);
				output = "Time: " + to_string(GetTickCount()) + " | Add used semaphore\n";
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
	} else WriteFile(out, "Mapping failed\n", strlen("Mapping failed\n"), &point, NULL);

	CloseHandle(out);

	return 0;
}
