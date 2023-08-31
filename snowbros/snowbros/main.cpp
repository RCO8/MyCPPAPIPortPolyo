#include <windows.h>
//#include <cmath>//���� ���Ҷ� �ʿ��� ���
#include "resource.h"
#include "Datas.h"
//#include <mmsystem.h>//���带 ������ �ʿ��� ���
//#include <commctrl.h>//���� ��Ʈ���� ����(���α׷��� ��) ���
//#pragma comment(lib."winmm.lib")//��Ƽ�̵�� �Լ��� ����ϱ� ���� lib����

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//�ν��Ͻ� �ڵ�
LPCTSTR lpszClass = TEXT("Fortress");
HWND hWndMain;

//��Ʈ�� ���ҽ� �ڵ�(��� �� ������)
HBITMAP background[50];

struct Player {
	int x, y;
	int width = 32, height = 32;
	/*
	* �̵�, ����, �޸���, �б�, ������, �Ⱦ�����, ����, �и���, ����, ����
	*/
	HBITMAP sprites[54];
	int frame;
	//��ư�� ������ ����
	int pressKey = 0;	
	int pressJump = 0;
	int startPosY;	//�����Ҷ� ������ ��ġ

	void MoveHorizontal()
	{
		if (pressKey > 0)
		{
			//���� ��� ���ϰ�
			x += 1;
		}
		else if (pressKey < 0)
		{
			x -= 1;
		}

		//���� �Ѿ�� ���ϱ�
		if (x <= 0) x = 0;
		if (x >= 224) x = 224;
	}

	void Gravity()
	{

	}
}nick, tom;

//��Ʈ���� ������� �׷��ִ� �Լ�
void DrawBitmap(HDC hdc);

//��������� �κ�
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	//������ Ŭ���� ���
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	//�޴� ��� �κ�
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//������ ����
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//UpdateWindow(hWnd);

	/*�λ�
	MessageBox(hWnd, TEXT("�ȳ��ϼ���.\n���� ��ǻ�� �п��Դϴ�."), TEXT("WELCOME TO
	FORTRESS WORLD!"), MB_OK);
	*/

	//�޽��� ����
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

// ������ �޽��� ó��
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT crt;
	TCHAR tmp[10];
	TCHAR dataPath[20] = TEXT("stageFiles\\");
	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1024, 768);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		for (int i = 0; i < maxStage; i++)
			background[i + 1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1 + i));//��� ���ҽ�
		//����� ����.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);

		for (int i = 0; i < 7; i++)
			nick.sprites[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PLAY1_SPR1 + i));
		nick.x = 80;
		nick.y = 180;
		SetTimer(hWnd, PLAY1_CONTROLL, 10, NULL);


		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			//���� ����������
			if (moveFloor <= 208)
				moveFloor += 2;
			else
			{
				stage++;
				moveFloor = 0;

				nick.x = 80;
				nick.y = 180;

				KillTimer(hWnd, 1);
			}
			break;
		case PLAY1_CONTROLL:
			//������ ����

			//�¿�� �̵�
			nick.MoveHorizontal();

			//���� �� ��������
			if (nick.pressJump > 0)
			{
				nick.y--;
				if (nick.y == nick.startPosY - 32)
				{
					nick.pressJump = 0;
				}
			}
			else if (nick.pressJump < 0)
			{
				nick.y++;
				if (nick.y == nick.startPosY + 32)
					nick.pressJump = 0;
				//���� �ٴڿ� ���������� ��������
				if(stageTiles[stage-1][(nick.y / tileGrid) - 1][nick.x / tileGrid] == 'x')
				{
					nick.y = nick.y;
				}
			}
			break;
		case PLAY2_CONTROLL:
			break;
		}
		InvalidateRect(hWnd, NULL, 0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			if (moveFloor == 0)	//to next stage
			{
				moveFloor = 0;
				SetTimer(hWnd, 1, 1, NULL);
			}
			break;
		case VK_UP:
			if (nick.pressJump == 0)
			{
				nick.startPosY = nick.y;
				nick.pressJump = 1;
			}
			break;
		case VK_DOWN:
			if (nick.pressJump == 0)
			{
				nick.startPosY = nick.y;
				nick.pressJump = -1;
			}
			break;
		case VK_LEFT:
			//�¿���� ����
			nick.pressKey = -1;
			break;
		case VK_RIGHT:
			//�¿����
			nick.pressKey = 1;
			break;
		}
		InvalidateRect(hWnd, NULL, 0);
		return 0;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
			nick.pressKey = 0;
			break;
		default:
			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBitmap(hdc);
		wsprintf(tmp, TEXT("%d %d"), nick.x, nick.y);
		TextOut(hdc,20,250,tmp,lstrlen(tmp));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

/*��Ʈ�� ������ ȭ�鿡 ���̰��� �Լ�.
���� ���۸��� �̿��Ͽ� ȭ���� �����̴� ���� ����.
���α׷����� ����Ʈ �κ��� ���� �����̴� ������ ����.
*/
void DrawBitmap(HDC hdc)
{
	//���� ��Ʈ�� �̹����� ���� DC�ڵ�� ������ DC�ڵ� ��������
	HDC hMemDC, hMemDC2;
	HBITMAP hOldBitmap, hBackBit;

	//���� ��Ʈ�� �̹����� ���� DC�ڵ� ����
	hMemDC = CreateCompatibleDC(hdc);
	hBackBit = CreateCompatibleBitmap(hdc, 1024, 768);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//�ӽ� ��Ʈ�� �̹����� ���� ���� DC�ڵ� ����
	hMemDC2 = CreateCompatibleDC(hdc);
	//��� �׸���
	SelectObject(hMemDC2, background[stage]);
	BitBlt(hMemDC, 0, 16 + moveFloor, 256, 224 + moveFloor, hMemDC2, 0, 0, SRCCOPY);
	//������� ����Ͽ� ��ȯ�� ����
	SelectObject(hMemDC2, background[stage+1]);
	BitBlt(hMemDC, 0, (16 - 208) + moveFloor, 256, 224 + moveFloor, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//��������� ���������� ���� �� hMemDC�� ����

	TransBlt(hMemDC, nick.x, nick.y, nick.sprites[nick.frame], RGB(255, 255, 255));

	//������ �̹����� �׸���
	BitBlt(hdc, 0, 0, 256, 224, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBackBit);
	DeleteDC(hMemDC);
}