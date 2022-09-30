#include "tinky.h"

int		usage(void)
{
	puts("Usage: .\\svc.exe [ install | start | stop | delete ]");
	return 1;
}

char* getMessageError(int errorMessageID)
{
	LPSTR messageBuffer = 0;

	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	return messageBuffer;
}

int		findProcessByName(const char* name)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe;
	BOOL hResult;
	int pid = 0;

	if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
		return 0;
	pe.dwSize = sizeof(PROCESSENTRY32);
	hResult = Process32First(hSnapshot, &pe);

	for (; hResult; hResult = Process32Next(hSnapshot, &pe)) {
		if (!strcmp(name, pe.szExeFile)) {
			pid = pe.th32ProcessID;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return pid;
}

HANDLE	getTokenProcess(const char *name)
{
	HANDLE token = 0;
	int pid = findProcessByName(name);
	HANDLE p = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

	OpenProcessToken(p, TOKEN_ALL_ACCESS, &token);
	return token;
}