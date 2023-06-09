#include <windows.h>
//#include <cmath>//���� ���Ҷ� �ʿ��� ���
#include "resource.h"
#include "maps.h"
//#include <mmsystem.h>//���带 ������ �ʿ��� ���
//#include <commctrl.h>//���� ��Ʈ���� ����(���α׷��� ��) ���
//#pragma comment(lib."winmm.lib")//��Ƽ�̵�� �Լ��� ����ϱ� ���� lib����

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//�ν��Ͻ� �ڵ�
LPCTSTR lpszClass = TEXT("���ڹ�");
HWND hWndMain;

const int spriteSize = 50;
int nx, ny, nMove;
int moveCount = 0;
int hole = 0;
bool clear = false;

//��Ʈ�� ���ҽ� �ڵ�(��� �� ������)
HBITMAP background[5];

//��Ʈ�� �̹����� �ʿ� ���� ����� ���� �ִ� �Լ�
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);
//��Ʈ���� ������� �׷��ִ� �Լ�
void DrawBitmap(HDC hdc);
//�������� ������
void InitStage();
//�̵�
void Move(int xDir, int yDir);

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
	hWndMain = hWnd;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT crt;
	TCHAR moveStr[50];

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1024, 912);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//��Ʈ�� ���ҽ����� Backgroud[]�迭�� ���
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//��� ���ҽ�
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//��Ÿ ���ҽ�
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//��Ÿ ���ҽ�
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//��Ÿ ���ҽ�
		SetTimer(hWnd, 1, 50, NULL);
		//����� ����.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);
		InitStage();
		return 0;
	case WM_TIMER:

		InvalidateRect(hWnd, NULL, 0);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hWnd);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBitmap(hdc);
		wsprintf(moveStr, TEXT("�̵��� Ƚ�� : %d"), moveCount);
		TextOut(hdc, 50, 50 * 16+20, moveStr, lstrlen(moveStr));
		wsprintf(moveStr, TEXT("���� ĭ : %d"), hole);
		TextOut(hdc, 50, 50 * 16 + 40, moveStr, lstrlen(moveStr));
		if (hole == 0) clear = true;
		if (clear)
			TextOut(hdc, 512, 512, TEXT("�������� Ŭ����!"), 9);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
			if (!clear)
			{
				case VK_UP:
					Move(0, -1);
					break;
				case VK_DOWN:
					Move(0, 1);
					break;
				case VK_LEFT:
					Move(-1, 0);
					break;
				case VK_RIGHT:
					Move(1, 0);
					break;
				case 'R':
					//����
					InitStage();
					break;
				case VK_RETURN:
					if (clear) {
						if(nowStage<maxStage)
							nowStage++;
						InitStage();
						clear = false;
					}
					break;
			}
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void InitStage()
{
	moveCount = 0;
	memcpy(ns, map[nowStage], sizeof(ns));
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 21; j++)
		{
			if (map[nowStage][i][j] == '@')
			{
				nx = j;
				ny = i;
				ns[ny][nx] = ' ';
			}
		}
	InvalidateRect(hWndMain, NULL, true);
}
void Move(int dx, int dy)
{
	if (ns[ny + dy][nx + dx] != '#') {
		if (ns[ny + dy][nx + dx] == 'o') {
			if (ns[ny + dy * 2][nx + dx * 2] == ' ' || ns[ny + dy * 2][nx + dx * 2] == '.') {
				if (map[nowStage][ny + dy][nx + dx] == '.') {
					ns[ny + dy][nx + dx] = '.';
				}
				else {
					ns[ny + dy][nx + dx] = ' ';
				}
				ns[ny + dy * 2][nx + dx * 2] = 'o';
			}
			else {
				return;
			}
		}
		nx += dx;
		ny += dy;
		if(!clear)
			moveCount++;
		InvalidateRect(hWndMain, NULL, false);
	}
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
	hBackBit = CreateCompatibleBitmap(hdc, 1024, 1024);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//�ӽ� ��Ʈ�� �̹����� ���� ���� DC�ڵ� ����
	hMemDC2 = CreateCompatibleDC(hdc);
	//����� ����
	SelectObject(hMemDC2, background[1]);

	//������ hMemDC2�� ���� hMemDC�� ����
	//BitBlt(hMemDC, 0, 0, 1024, 768, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//��������� ���������� ���� �� hMemDC�� ����

	hole = 0;
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 21; j++) {
			if(ns[i][j] == '#')
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[1], RGB(255, 0, 0));
			else if(ns[i][j] == 'o')
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[2], RGB(255, 0, 0));
			else if (ns[i][j] == '.')
			{
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[3], RGB(255, 0, 0));
				hole++;
			}
		}
	}
	TransBlt(hMemDC, nx* spriteSize, ny* spriteSize, background[4], RGB(255, 0, 0));

	//������ �̹����� �׸���
	BitBlt(hdc, 0, 0, 1024, 1024, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBackBit);
	DeleteDC(hMemDC);
}

/*
��Ʈ���� �ʿ���� ���κ��� �����ټ� �ִ� �Լ�.
TransparentBle�Լ��� ���Ͽ� �ӵ��� ������ �پ.
*/
//* TransBlt �Լ�
// ���� ��Ʈ���� ����ϴ� �Լ�. (x,y)�� ��� ��ġ. clrMask�� ������� �ش�.
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask)
{
	BITMAP bm;
	COLORREF cColor;
	HBITMAP bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC		hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT	ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hbitmap);
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;
	DPtoLP(hdcTemp, &ptSize, 1);

	hdcBack = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem = CreateCompatibleDC(hdc);
	hdcSave = CreateCompatibleDC(hdc);

	bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndMem = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	bmBackOld = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld = (HBITMAP)SelectObject(hdcSave, bmSave);

	SetMapMode(hdcTemp, GetMapMode(hdc));

	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	cColor = SetBkColor(hdcTemp, clrMask);

	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	SetBkColor(hdcTemp, cColor);

	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, x, y, SRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);
	BitBlt(hdc, x, y, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}