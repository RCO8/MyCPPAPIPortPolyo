#include <windows.h>
//#include <cmath>//���� ���Ҷ� �ʿ��� ���
#include "resource.h"
//#include <mmsystem.h>//���带 ������ �ʿ��� ���
//#include <commctrl.h>//���� ��Ʈ���� ����(���α׷��� ��) ���
//#pragma comment(lib."winmm.lib")//��Ƽ�̵�� �Լ��� ����ϱ� ���� lib����

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//�ν��Ͻ� �ڵ�
LPCTSTR lpszClass = TEXT("Fortress");
HWND hWndMain;

//��Ʈ�� ���ҽ� �ڵ�(��� �� ������)
HBITMAP background[10];

const int imgSize = 50;
char stage[14][23] = {
	//x : ��, # : ���, * : ��ź
	{"                      "},
	{" x x#x x x xx x x x x "},
	{"    #      xx         "},
	{" x x x#x x##  x x#x x "},
	{"           xx# #      "},
	{" x x x x x xx x x x x "},
	{"        ##            "},
	{"           #          "},
	{" x x x x x xx x x x#x "},
	{"           xx#        "},
	{" x x x x x  ##x x x x "},
	{"     ###   xx         "},
	{" x x x x x xx x x#x x "},
	{"                      "}
};
struct Player {
	int x=0, y=0;
	int bombCount = 1;
} me;
struct Bomb {
	int x, y;
	int length;
}bmb[10];
int blockCount = 0;

//��Ʈ�� �̹����� �ʿ� ���� ����� ���� �ִ� �Լ�
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);
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
	LPMINMAXINFO pmmi;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1200, 800);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//��Ʈ�� ���ҽ����� Backgroud[]�迭�� ���
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//�� ���ҽ�
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//��� ���ҽ�
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//�÷��̾� ���ҽ�
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//��ź ���ҽ�
		background[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));//���� ���ҽ�

		//����� ����.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);

		SetTimer(hWnd, 1, 1000, NULL);
		KillTimer(hWnd, 1);

		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			if(stage[me.y - 1][me.x] == ' ' || stage[me.y - 1][me.x] == '+') me.y -= 1;
			break;
		case VK_DOWN:
			if (stage[me.y + 1][me.x] == ' ' || stage[me.y + 1][me.x] == '+') me.y += 1;
			break;
		case VK_LEFT:
			if (stage[me.y][me.x - 1] == ' ' || stage[me.y][me.x - 1] == '+') me.x -= 1;
			break;
		case VK_RIGHT:
			if (stage[me.y][me.x + 1] == ' ' || stage[me.y][me.x + 1] == '+') me.x += 1;
			break;
		case 'Z':
			//��ź ����
			if (me.bombCount > 0)
			{
				me.bombCount--;
				stage[me.y][me.x] = '*';
				bmb[0].x = me.x;
				bmb[0].y = me.y;
				SetTimer(hWnd, 1, 1000, NULL);
			}
			break;
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			//��ź ��Ʈ����
			stage[bmb[0].y][bmb[0].x] = '+';
			if (stage[bmb[0].y - 1][bmb[0].x] != 'x' && bmb[0].y-1 >= 0)stage[bmb[0].y-1][bmb[0].x] = '+';
			if (stage[bmb[0].y + 1][bmb[0].x] != 'x' && bmb[0].y+1 <= 14)stage[bmb[0].y+1][bmb[0].x] = '+';
			if (stage[bmb[0].y][bmb[0].x - 1] != 'x' && bmb[0].x-1 >= 0)stage[bmb[0].y][bmb[0].x-1] = '+';
			if (stage[bmb[0].y][bmb[0].x + 1] != 'x' && bmb[0].x+1 <= 22)stage[bmb[0].y][bmb[0].x+1] = '+';

			KillTimer(hWnd, 1);

			me.bombCount++;
			SetTimer(hWnd, 2, 500, NULL);
			break;
		case 2:
			KillTimer(hWnd, 2);

			if (stage[me.y][me.x] == '+') {
				if (MessageBox(hWnd, TEXT("���� ����!!!"), TEXT("GAME"), MB_OK) == IDOK)
					PostQuitMessage(0);
			}

			stage[bmb[0].y][bmb[0].x] = ' ';
			if(stage[bmb[0].y - 1][bmb[0].x] == '+') stage[bmb[0].y - 1][bmb[0].x] = ' ';
			if (stage[bmb[0].y + 1][bmb[0].x] == '+')stage[bmb[0].y + 1][bmb[0].x] = ' ';
			if (stage[bmb[0].y][bmb[0].x - 1] == '+')stage[bmb[0].y][bmb[0].x - 1] = ' ';
			if (stage[bmb[0].y][bmb[0].x + 1] == '+')stage[bmb[0].y][bmb[0].x + 1] = ' ';

			if (blockCount == 0)
			{
				if (MessageBox(hWnd, TEXT("���� Ŭ����!!!"), TEXT("GAME"), MB_OK) == IDOK)
					PostQuitMessage(0);
			}
			break;
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_GETMINMAXINFO:	//������ â�� ������� �ʱ� ���� �̺�Ʈ
		pmmi = (LPMINMAXINFO)lParam;
		pmmi->ptMinTrackSize.x = 125;
		pmmi->ptMinTrackSize.y = 840;
		pmmi->ptMaxTrackSize.x = 1250;
		pmmi->ptMaxTrackSize.y = 840;

		pmmi->ptMaxSize.x = 1250;
		pmmi->ptMaxSize.y = 840;
		pmmi->ptMaxPosition.x = 100;
		pmmi->ptMaxPosition.y = 100;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBitmap(hdc);
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
	hBackBit = CreateCompatibleBitmap(hdc, 1200, 800);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//�ӽ� ��Ʈ�� �̹����� ���� ���� DC�ڵ� ����
	hMemDC2 = CreateCompatibleDC(hdc);
	//����� ����
	SelectObject(hMemDC2, background[1]);

	//������ hMemDC2�� ���� hMemDC�� ����
	int i=0, j=1;
	while (i < 24 || j < 15)	//�׵θ���
	{
		if (i < 24)
		{
			BitBlt(hMemDC, i * imgSize, 0, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			BitBlt(hMemDC, i * imgSize, 750, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			i++;
		}
		if (j < 15)
		{
			BitBlt(hMemDC, 0, j * 50, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			BitBlt(hMemDC, 1150, j * 50, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			j++;
		}
	}
	DeleteDC(hMemDC2);
	
	blockCount = 0;
	//��������� ���������� ���� �� hMemDC�� ����
	for (int y = 0; y < 14; y++)
		for (int x = 0; x < 22; x++)
		{
			//��, ��� �׸���
			if(stage[y][x] == 'x')
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[1], RGB(255, 255, 255));
			else if (stage[y][x] == '#')
			{
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[2], RGB(255, 255, 255));
				blockCount++;
			}
			else if(stage[y][x] == '*')
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[4], RGB(255, 255, 255));
			else if(stage[y][x] == '+')
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[5], RGB(255, 255, 255));
		}
	TransBlt(hMemDC, (me.x* imgSize) + 50, (me.y* imgSize) + 50, background[3], RGB(255, 255, 255));
	//������ �̹����� �׸���
	BitBlt(hdc, 0, 0, 1200, 800, hMemDC, 0, 0, SRCCOPY);

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