#include <ws2tcpip.h> 
#include <windows.h>
#include <tchar.h>
#include <string>
#include "winsock2.h" 
#include "resource.h"
#pragma comment(lib, "ws2_32.lib") 
using namespace std;

HWND hDialog, hSI, hBSEND, hLabel, hGI, hBSTART, hBSTOP;
WSADATA WSAdata;
SOCKET Socket, AcceptSocket;
sockaddr_in SockAddr;
string MyIP = "127.0.0.1";
const int PORT = 8888;
const int SIZE = 256;
char SendMess[SIZE] = {};
char ReceiveMess[SIZE] = {};

BOOL CALLBACK DlgProc(HWND hWnd, UINT mess, WPARAM wp, LPARAM lp)
{
	switch (mess)
	{
		case WM_INITDIALOG:
		{
			hDialog = GetDlgItem(hWnd, DIALOG);
			hSI = GetDlgItem(hWnd, SEND_INFO);
			hBSEND = GetDlgItem(hWnd, bSEND);
			hLabel = GetDlgItem(hWnd, LABEL);
			hGI = GetDlgItem(hWnd, GET_INFO);
			hBSTART = GetDlgItem(hWnd, bSTART);
			hBSTOP = GetDlgItem(hWnd, bSTOP);
			return(TRUE);
		}
		case WM_COMMAND:
		{
			{
				EnableWindow(hBSEND, FALSE);
				while (true)
				{
					ReceiveData(ReceiveMess, SIZE);
					SetWindowText(hGI, LPWSTR(ReceiveMess));
					Sleep(15000);
					SetWindowText(hGI, NULL);
					Sleep(15000);
				}
			}
			if (LOWORD(wp) == bSTART)
			{
				EnableWindow(hBSTART, FALSE);
				EnableWindow(hBSTOP, TRUE);
				InitSocket();
				StartHosting(PORT);
			}
			if (LOWORD(wp) == bSTOP)
			{
				EnableWindow(hBSTOP, FALSE);
				EnableWindow(hBSTART, TRUE);
				EnableWindow(hBSEND, FALSE);
				SetWindowText(hSI, NULL);
				SetWindowText(hGI, NULL);
				closesocket(AcceptSocket);
				closesocket(Socket);
				WSACleanup();
			}
			if (LOWORD(wp) == SEND_INFO)
			{
				GetWindowText(hSI, (LPWSTR)SendMess, SIZE);
				if (strlen(SendMess) > NULL && IsWindowEnabled(hBSTART) == FALSE)
				{
					EnableWindow(hBSEND, TRUE);
				}
				else EnableWindow(hBSEND, FALSE);
			}
			if (LOWORD(wp) == bSEND)
			{
				SendDataSMS();
				SetWindowText(hSI, NULL);
			}
			return(TRUE);
		}
		case WM_CLOSE:
		{
			closesocket(AcceptSocket);
			closesocket(Socket);
			WSACleanup();
			EndDialog(hWnd, NULL);
			return(TRUE);
		}
	}
	return(FALSE);
}


void Bind(int PORT)
{
	SockAddr.sin_family = AF_INET; //для сетевого протокола
	SockAddr.sin_port = htons(PORT);
	inet_pton (AF_INET, "0.0.0.0", &SockAddr.sin_addr); 
	if (bind(Socket, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR)
	{
		MessageBox(0, TEXT("Error in Bind function"), TEXT("..."), MB_OK | MB_ICONERROR);
		WSACleanup();
		EndDialog(0, 0);
	}
}
void InitSocket(void)
{
	if (WSAStartup(MAKEWORD(2, 2), &WSAdata) != NO_ERROR) //инициализирует WinSock
	{
		MessageBox(0, TEXT("Error InitSocket function"), TEXT("..."), MB_OK | MB_ICONERROR);
		WSACleanup();         //завершает функцию Windows Sockets для всех потоков
		EndDialog(0, 0);
	}
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		MessageBox(0, TEXT("Error 2 InitSocket function"), TEXT("..."), MB_OK | MB_ICONERROR);
		WSACleanup();
		EndDialog(0, 0);
	}
}
void Listen(void)
{
	if (listen(Socket, 1) == SOCKET_ERROR)
	{
		MessageBox(0, TEXT("Error in Listen function"), TEXT("..."), MB_OK | MB_ICONERROR);
		WSACleanup();
		EndDialog(0, 0);
	}
	AcceptSocket = accept(Socket, 0, 0);
	while (AcceptSocket == SOCKET_ERROR)
	{
		AcceptSocket = accept(Socket, 0,0);
	}
	Socket = AcceptSocket;
}
void StartHosting(int PORT)
{
	Bind(PORT);
	Listen();
}
bool SendData(char* BUFF)
{
	send(Socket, BUFF, strlen(BUFF), NULL);
	return(TRUE);
}
void SendDataSMS(void)
{
	SendData(SendMess);
}
bool ReceiveData(char* BUFF, int LocalSize)
{
	LocalSize = SIZE;
	int i = recv(Socket, BUFF, LocalSize, NULL);
	BUFF[i] = '\0';
	return(true);
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int CmdShow)
{
	return DialogBox(hInst, MAKEINTRESOURCE(DIALOG), NULL, (DLGPROC)DlgProc);
}