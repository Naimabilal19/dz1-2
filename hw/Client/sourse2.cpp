#include <ws2tcpip.h> 
#include <windows.h>
#include <tchar.h>
#include <string>
#include "winsock2.h" 
#include "resource.h"
#pragma comment(lib, "ws2_32.lib") 
using namespace std;


HWND hDialog, hLabel1, hIP, hBCONNECT, hBUNCONNECT, hSI, hBSEND, hLabel2, hGI;
WSADATA WSAdata;
SOCKET Socket, AcceptSocket;
sockaddr_in SockAddr;
const int PORT = 8888;
const int SIZE = 256;
char EnterIP[SIZE] = {};
char SendMess[SIZE] = {};
char ReceiveMess[SIZE] = {};

BOOL CALLBACK DlgProc(HWND hWnd, UINT mess, WPARAM wp, LPARAM lp)
{
	switch (mess)
	{
		case WM_INITDIALOG:
		{
			hDialog = GetDlgItem(hWnd, DIALOG);
			hLabel1 = GetDlgItem(hWnd, LABEL1);
			hIP = GetDlgItem(hWnd, IP);
			hBCONNECT = GetDlgItem(hWnd, bCONNECT);
			hBUNCONNECT = GetDlgItem(hWnd, bUNCONNECT);
			hSI = GetDlgItem(hWnd, SEND_INFO);
			hBSEND = GetDlgItem(hWnd, bSEND);
			hLabel2 = GetDlgItem(hWnd, LABEL2);
			hGI = GetDlgItem(hWnd, GET_INFO);
			return(TRUE);
		}
		case WM_COMMAND:
		{
			{
				EnableWindow(hBSEND, FALSE);
				EnableWindow(hBCONNECT, FALSE);
				while (true)
				{
					ReceiveData(SendMess, SIZE);
					SetWindowText(hGI, LPWSTR(ReceiveMess));
					Sleep(15000);
					SetWindowText(hGI, NULL);
					Sleep(15000);
				}
			}
			if (LOWORD(wp) == IP)
			{
				GetWindowText(hIP, (LPWSTR)EnterIP, SIZE);
				if (strlen(EnterIP) > NULL)
				{
					EnableWindow(hBCONNECT, TRUE);
				}
				else EnableWindow(hBCONNECT, FALSE);
			}
			if (LOWORD(wp) == bCONNECT)
			{
				EnableWindow(hBCONNECT, FALSE);
				EnableWindow(hBUNCONNECT, TRUE);
				InitSocket();
				ConnectToServer(EnterIP, PORT);
				EnableWindow(hIP, FALSE);
			}
			if (LOWORD(wp) == bUNCONNECT)
			{
				EnableWindow(hBUNCONNECT, FALSE);
				EnableWindow(hBCONNECT, TRUE);
				EnableWindow(hBSEND, FALSE);
				EnableWindow(hIP, TRUE);
				SetWindowText(hIP, NULL);
				SetWindowText(hSI, NULL);
				SetWindowText(hGI, NULL);
				closesocket(AcceptSocket);
				closesocket(Socket);
				WSACleanup();
			}
			if (LOWORD(wp) == SEND_INFO)
			{
				GetWindowText(hSI, (LPWSTR)SendMess, SIZE);
				if (strlen(SendMess) > NULL && IsWindowEnabled(hBCONNECT) == FALSE)
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

void InitSocket(void)
{
	if (WSAStartup(MAKEWORD(2, 2), &WSAdata) != NO_ERROR)
	{
		MessageBox(NULL, TEXT("Error in 1/2 InitSocket func"), TEXT("Information"), MB_OK | MB_ICONERROR);
		WSACleanup();
		EndDialog(NULL, NULL);
	}
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		MessageBox(NULL, TEXT("Error in 2/2 InitSocket func"), TEXT("Information"), MB_OK | MB_ICONERROR);
		WSACleanup();
		EndDialog(NULL, NULL);
	}
}
void ConnectToServer(const char* ConnectToIP, int PORT)
{
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, ConnectToIP, &SockAddr.sin_addr);
	if (connect(Socket, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR)
	{
		MessageBox(NULL, TEXT("Error in ConncectToServer func"), TEXT("Information"), MB_OK | MB_ICONERROR);
		WSACleanup();
		EndDialog(NULL, NULL);
	}
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