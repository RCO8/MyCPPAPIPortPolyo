#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PopupProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("In Game");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	WndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
	WndClass.lpfnWndProc = PopupProc;
	WndClass.lpszClassName = TEXT("PopupClass");
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd;

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

#include "resource.h"
HWND hChild, hPopup, h1, h2, h11, h12, h21, h22;
HDC hMemDC[8];
HBITMAP hbrick[8];
HBITMAP brick[8];
int popWinX = 1000, popWinY = 10;
int bgIndex = 0;
DWORD WINAPI ThreadFunc(LPVOID temp)
{
	HDC hdc;
	BYTE Blue = 0;
	HBRUSH hBrush, hOldBrush;
	hdc = GetDC(hWndMain);
	for (;;) {
		bgIndex++;
		if (bgIndex > 7)
			bgIndex = 0;
		Sleep(100);
		BitBlt(hdc, popWinX, popWinY, 512, 266, hMemDC[bgIndex], 0, 0, SRCCOPY);
	}
	ReleaseDC(hWndMain, hdc);
	return 0;
}

const int spriteGrid = 32;
struct PlayerControll
{
	HDC hMEMGirl[3];	//walk, run, bike
	HBITMAP hGirl[3];
	HBITMAP girl[3];
	int direction = 1;
	int frame[4] = { 0,1,0,2 };
	int nowFrame = 0;
	int frameState = 0;	//현재 동작상태
	int moving = 0;	//walking - running - biking
	int x, y;
	int speed = 2;
}play1, play2;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//In Game

	HDC hdc;
	PAINTSTRUCT ps;
	HWND hParent, hOwner;
	TCHAR str[256];

	switch (iMessage) {
	case WM_CREATE:
		hdc = GetDC(hWnd);
		hPopup = CreateWindow(TEXT("PopupClass"), TEXT("애니메이션"), WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE,
			popWinX, popWinY, 528, 306, hWnd, (HMENU)0, g_hInst, NULL);	//배경화면 팝업으로

		//이미지로
		int i;
		for (i = 0; i < 3; i++)
		{
			play1.hMEMGirl[i] = CreateCompatibleDC(hdc);
			play1.hGirl[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9 + i));
			play2.hMEMGirl[i] = CreateCompatibleDC(hdc);
			play2.hGirl[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP12 + i));
		}
		for (i = 0; i < 3; i++)
		{
			play1.girl[i] = (HBITMAP)SelectObject(play1.hMEMGirl[i], play1.hGirl[i]);
			play2.girl[i] = (HBITMAP)SelectObject(play2.hMEMGirl[i], play2.hGirl[i]);
		}

		play1.x = 0;
		play1.y = 0;
		play2.x = 640 - spriteGrid*2;
		play2.y = 480 - spriteGrid*2;
		SetTimer(hWnd, 1, 200, NULL);
		SetTimer(hWnd, 2, 200, NULL);

		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
			//1P
		case 'W':
			play1.direction = 1;
			play1.moving = 1;
			break;
		case 'S':
			play1.direction = 2;
			play1.moving = 1;
			break;
		case 'A':
			play1.direction = 3;
			play1.moving = 1;
			break;
		case 'D':
			play1.direction = 4;
			play1.moving = 1;
			break;
		case VK_SHIFT:
			if (play1.moving > 0)
			{
				play1.moving = 2;
				play1.frameState = 1;
			}
			break;
		case VK_LCONTROL:
			break;
			//2P
		case VK_UP:
			play2.direction = 1;
			play2.moving = 1;
			break;
		case VK_DOWN:
			play2.direction = 2;
			play2.moving = 1;
			break;
		case VK_LEFT:
			play2.direction = 3;
			play2.moving = 1;
			break;
		case VK_RIGHT:
			play2.direction = 4;
			play2.moving = 1;
			break;
		case VK_RSHIFT:
			if (play2.moving > 0)
			{
				play2.moving = 2;
				play2.frameState = 1;
			}
			break;
		case VK_RCONTROL:
			break;
		}
		InvalidateRect(hWnd, NULL, 1);
		return 0;
	case WM_KEYUP:
		switch (wParam)
		{
		case 'W':
		case 'S':
		case 'A':
		case 'D':
			play1.moving = 0;
			play1.nowFrame = 0;
			play1.frameState = 0;
			break;
		case VK_LSHIFT:
			play1.moving = play1.direction > 0 ? 1 : 0;
			play1.nowFrame = 0;
			play1.frameState = 0;
			break;
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			play2.moving = 0;
			play2.nowFrame = 0;
			play2.frameState = 0;
			break;
		case VK_RSHIFT:
			play2.moving = play2.direction > 0 ? 1 : 0;
			play2.nowFrame = 0;
			play2.frameState = 0;
			break;
		}
		InvalidateRect(hWnd, NULL, 1);
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (play1.moving > 0)
			{
				if (play1.moving == 1)
					play1.speed = 2;
				else if (play1.moving == 2)
					play1.speed = 3;
				switch(play1.direction)
				{
				case 1: play1.y -= play1.speed; break;
				case 2:	play1.y += play1.speed; break;
				case 3: play1.x -= play1.speed; break;
				case 4: play1.x += play1.speed; break;
				}
				play1.nowFrame++;
				play1.nowFrame %= 4;
			}
			break;
		case 2:
			if (play2.moving > 0)
			{
				if (play2.moving == 1)
					play2.speed = 2;
				else if (play2.moving == 2)
					play2.speed = 3;
				switch (play2.direction)
				{
				case 1: play2.y -= play2.speed; break;
				case 2:	play2.y += play2.speed; break;
				case 3: play2.x -= play2.speed; break;
				case 4: play2.x += play2.speed; break;
				}
				play2.nowFrame++;
				play2.nowFrame %= 4;
			}
			break;
		}
		InvalidateRect(hWnd, NULL, 1);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, play1.x, play1.y, spriteGrid, spriteGrid, play1.hMEMGirl[play1.frameState],
			play1.frame[play1.nowFrame] * spriteGrid, (play1.direction-1) * spriteGrid, SRCCOPY);
		BitBlt(hdc, play2.x, play2.y, spriteGrid, spriteGrid, play2.hMEMGirl[play2.frameState],
			play2.frame[play2.nowFrame] * spriteGrid, (play2.direction-1) * spriteGrid, SRCCOPY);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

LRESULT CALLBACK PopupProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HWND hParent, hOwner;
	TCHAR str[256];

	DWORD ThreadID;
	HANDLE hThread;

	switch (iMessage) {
	case WM_CREATE:
		hdc = GetDC(hWnd);

		int i;
		for (i = 0; i < 8; i++)
		{
			hMemDC[i] = CreateCompatibleDC(hdc);
			hbrick[i]=LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1+i));
		}
		for (i = 0; i < 8; i++) brick[i] = (HBITMAP)SelectObject(hMemDC[i], hbrick[i]);

		hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
		CloseHandle(hThread);
		return 0;
	case WM_MOVE:
		popWinX = LOWORD(lParam);
		popWinY = HIWORD(lParam);
		InvalidateRect(hWndMain, NULL, 1);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
