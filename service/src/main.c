#include "tinky.h"

#pragma comment(lib, "advapi32.lib")

int		install(SC_HANDLE hSCManager)
{
	char fullpath[MAX_PATH] = { 0 };

	GetModuleFileName(0, fullpath, MAX_PATH);
	
	SC_HANDLE hService = CreateService(
		hSCManager,
		SVCNAME,
		SVCNAME,
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
	return printf("Service {%s} installed successfully.\n", SVCNAME);
}

int		start(SC_HANDLE hSCManager)
{
	char fullpath[MAX_PATH] = { 0 };

	SC_HANDLE hService = OpenService(hSCManager, SVCNAME, SERVICE_START);

	if (!hService) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Start OpenService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	GetFullPathName(".\\winkey.exe", MAX_PATH, fullpath, 0);
	if (!StartService(hService, 1, (char *[]) { fullpath })) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Start StartService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	CloseServiceHandle(hService);
	return printf("Service {%s} started successfully.\n", SVCNAME);
}

int		stop(SC_HANDLE hSCManager)
{
	SC_HANDLE hService = OpenService(hSCManager, SVCNAME, SERVICE_STOP);
	SERVICE_STATUS serviceStatus = {0};

	if (!hService) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Stop OpenService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Stop ControlService: Error: %s\n", messageBuffer);
		LocalFree(messageBuffer);
		return 1;
	}
	CloseServiceHandle(hService);
	return printf("Service {%s} stopped successfully.\n", SVCNAME);
}

int		delete(SC_HANDLE hSCManager)
{
	SC_HANDLE hService = OpenService(hSCManager, SVCNAME, DELETE);

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
	return printf("Service {%s} deleted successfully.\n", SVCNAME);
}

int		main(int ac, char **av)
{
	SC_HANDLE hSCManager;
	const char* opt = " install start stop delete";
	t_cmd func_opt[26] = { [1] = &install, [9] = &start, [15] = &stop, [20] = &delete };
	size_t func_len_opt[26] = { [1] = 7, [9] = 5, [15] = 4, [20] = 6 };
	char* p = 0;

	/*if (ac != 2)
		return usage();*/

	if (ac == 1 && confService() < 0) {
		char* messageBuffer = getMessageError(GetLastError());
		printf("Error confService: %s\n", messageBuffer);
		LocalFree(messageBuffer);
	}
	hSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager) {
		if (GetLastError() == ERROR_ACCESS_DENIED) {
			char* messageBuffer = getMessageError(GetLastError());
			printf("Error OpenSCManager: %s\n", messageBuffer);
			LocalFree(messageBuffer);
			return 1;
		}
	}
	p = strstr(opt, av[1]);
	if (p && (p - opt) < 26 && (strlen(av[1]) == func_len_opt[p - opt])) {
		func_opt[p - opt](hSCManager);
	}
	CloseServiceHandle(hSCManager);
}
