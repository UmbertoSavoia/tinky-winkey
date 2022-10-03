#ifndef WINKEY_H

#pragma warning(disable : 5045)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>
#include <wingdi.h>
#include <stdio.h>
#include <signal.h>

BOOL	DoScreenshot(void);
void	GetClipboard(FILE *flog);

#endif
