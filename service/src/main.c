#include "../include/tinky.h"

#pragma comment(lib, "advapi32.lib")

int		usage(void)
{
	puts("Usage: .\\svc.exe [ install | start | stop | delete ]");
	return 1;
}

char*	getMessageError(int errorMessageID)
{
	LPSTR messageBuffer = 0;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	return messageBuffer;
}

int		install(SC_HANDLE hSCManager)
{
	char fullpath[MAX_PATH] = { 0 };

	GetFullPathName(".\\winkey.exe", MAX_PATH, fullpath, 0);
	
	SC_HANDLE hService = CreateService(
		hSCManager,
		"tinky",
		"tinky",
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		fullpath,
		0, 0, 0, 0, 0
		);
	if (!hService) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Install: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	CloseServiceHandle(hService);
	return puts("Service {tinky} installed successfully.");
}

int		start(SC_HANDLE hSCManager)
{
	SC_HANDLE hService = OpenService(hSCManager, "tinky", SERVICE_START);
	
	if (!hService) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Start OpenService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	if (!StartService(hService, 0, 0)) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Start StartService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	CloseServiceHandle(hService);
	return puts("Service {tinky} started successfully.");
}

int		stop(SC_HANDLE hSCManager)
{
	SC_HANDLE hService = OpenService(hSCManager, "tinky", SERVICE_STOP);

	if (!hService) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Stop OpenService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	if (!ControlService(hService, SERVICE_CONTROL_STOP, 0)) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Stop ControlService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	CloseServiceHandle(hService);
	return puts("Service {tinky} stopped successfully.");
}

int		delete(SC_HANDLE hSCManager)
{
	SC_HANDLE hService = OpenService(hSCManager, "tinky", DELETE);

	if (!hService) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Delete OpenService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	if (!DeleteService(hService)) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Delete DeleteService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	CloseServiceHandle(hService);
	return puts("Service {tinky} deleted successfully.");
}

int		main(int ac, char **av)
{
	SC_HANDLE hSCManager;
	const char* opt = " install start stop delete";
	t_cmd func_opt[26] = { [1] = &install, [9] = &start, [15] = &stop, [20] = &delete };
	int func_len_opt[26] = { [1] = 7, [9] = 5, [15] = 4, [20] = 6 };
	char* p = 0;

	if (ac != 2)
		return usage();

	if (!(hSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS))) {
		if (GetLastError() == ERROR_ACCESS_DENIED) {
			char* messageBuffer = getMessageError(GetLastError());
			printf("Error: %s\n", messageBuffer);
			LocalFree(messageBuffer);
			return 1;
		}
	}
	if ((p = strstr(opt, av[1])) && (p - opt) < 26
			&& (strlen(av[1]) == func_len_opt[p - opt])) {
		func_opt[p - opt](hSCManager);
	}
	CloseServiceHandle(hSCManager);
}