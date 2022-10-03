#ifndef TINKY_H

#pragma warning(disable : 4820)
#pragma warning(disable : 5045)

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <tlhelp32.h>

#define SVCNAME "tinky"
#define KEYLOG "winkey.exe"
#define OPT_MAX 33

typedef int (*t_cmd)(SC_HANDLE);

int		confService(void);

int		usage(void);
char*	getMessageError(int errorMessageID);
int		findProcessByName(const char* name);
HANDLE	getTokenProcess(const char* name);

#endif
