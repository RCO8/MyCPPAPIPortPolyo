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
bool gameStart = false;
bool autoMove = true;
int x = 1, y = 2;
int xx = 2, yy = 2, zz = 0;	//zz�� �������� ��ġ
int ex = 1, ey = 1;
int eMove = 1;
int see = 1;
int fish = 0;
int life = 5;
int maxStep = 15;
int timeSpeed = 250;
int nowStage = 0;
static int xMove = 0, yMove = 0;
static int step = 0;
char map[3][7][12] = {
	{
		"###########",
		"#  #   #  #",
		"#     ##   ",
		"#         #",
		"#   #  #   ",
		"###########"
	},
	{
		"###########",
		"#    #     ",
		"     #  #  ",
		"#      ## #",
		"          #",
		"###########"
	},
	{
		"###########",
		"       #  #",
		"     ###  #",
		"####     ##",
		"#     #   #",
		"###########"
	}
};

void MoveCheck(HWND hWnd);

//��Ʈ�� ���ҽ� �ڵ�(��� �� ������)
HBITMAP background[10];

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
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	//�޴� ��� �κ�
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//������ ����
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		0, 0, 1000, 600,
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
	TCHAR score[10];
	
	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;

		//��Ʈ�� ���ҽ����� Backgroud[]�迭�� ���
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//��Ÿ ���ҽ�
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//��Ÿ ���ҽ�
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//��� ���ҽ�
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//��� ���ҽ�
		background[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));//��� ���ҽ�
		background[6] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));//��� ���ҽ�
		
		//����� ����.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);

		SetTimer(hWnd, 1, 300, NULL);
		return 0;
	case WM_TIMER:
		if (gameStart) {
			if (autoMove) {
				//�÷��̾� �̵�
				MoveCheck(hWnd);

			}


			//�� �̵�
			ex += eMove;
			if (x == ex && y == ey)	//�÷��̾�� �΋H����
				life--;
			if (ex > 9 || ex < 0 || (map[nowStage][ey][ex + eMove] == '#'))
				eMove *= -1;
			if (life == 0)
			{
				KillTimer(hWnd, 1);
				MessageBox(hWnd, TEXT("Game Over"), TEXT("Game Over"), MB_OK);
				gameStart = false;
				PostQuitMessage(0);
			}

			InvalidateRect(hWnd, NULL, FALSE);
		}
		return 0;
	case WM_SETFOCUS:
		SetFocus(hWnd);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			if (gameStart)
			{
				yMove = -1;
				xMove = 0;
			}
			break;
		case VK_DOWN:
			if (gameStart)
			{
				yMove = 1;
				xMove = 0;
			}
			break;
		case VK_LEFT:
			if (gameStart)
			{
				see = 2;
				xMove = -1;
				yMove = 0;
			}
			break;
		case VK_RIGHT:
			if (gameStart)
			{
				see = 1;
				xMove = 1;
				yMove = 0;
			}
			break;
		case VK_RETURN:
			gameStart = true;
			break;
		}
		if (!autoMove)
		{
			MoveCheck(hWnd);
		}

		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_COMMAND:
		if (!gameStart) {
			switch (wParam) {
			case ID_LIFES_3:
				life = 3;
				break;
			case ID_LIFES_5:
				life = 5;
				break;
			case ID_LIFES_10:
				life = 10;
				break;
			case ID_SPEED_150:
				timeSpeed = 150;
				break;
			case ID_SPEED_250:
				timeSpeed = 250;
				break;
			case ID_SPEED_350:
				timeSpeed = 350;
				break;
			case ID_STEP_10:
				maxStep = 10;
				break;
			case ID_STEP_15:
				maxStep = 15;
				break;
			case ID_STEP_20:
				maxStep = 20;
				break;
			case ID_AUTOMOVE_OFF:
				autoMove = false;
				break;
			case ID_AUTOMOVE_ON:
				autoMove = true;
				break;
			}
		}
		SetTimer(hWnd, 1, timeSpeed, NULL);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBitmap(hdc);
		wsprintf(score, TEXT("life : %d"), life);
		TextOut(hdc, 0, 0, score, lstrlen(score));
		wsprintf(score, TEXT("fish : %d"), fish);
		TextOut(hdc, 0, 15, score, lstrlen(score));
		wsprintf(score, TEXT("step : %d"), step);
		TextOut(hdc, 0, 30, score, lstrlen(score));

		if (!gameStart)
		{
			SetTextAlign(hdc, TA_CENTER);
			TextOut(hdc, 500, 300, TEXT("Enter�� ������ ������ ���۵˴ϴ�."), 21);
		}

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void MoveCheck(HWND hWnd)
{
	if (xMove < 0 && map[nowStage][y][x - 1] != '#') x--;
	else if (xMove > 0 && map[nowStage][y][x + 1] != '#') x++;

	if (x < 0)
	{
		if (nowStage > 0)
		{
			nowStage--;
			step = 0;
			x = 10;
		}
		else
			x = 0;
	}
	else if (x > 10)
	{
		if (nowStage < 3)
		{
			nowStage++;
			step = 0;
			x = 0;
		}
		else
			x = 10;
	}

	if (yMove < 0 && map[nowStage][y - 1][x] != '#') y--;
	else if (yMove > 0 && map[nowStage][y + 1][x] != '#') y++;

	if (y < 0) y = 0;
	else if (y > 5) y = 5;


	step++;
	if (step > maxStep)
	{
		xx = rand() % 10;
		yy = rand() % 5;
		zz = rand() % 3;
		step = 0;
		life--;
	}
	if (x == xx && y == yy && nowStage == zz)
	{
		xx = rand() % 10;
		yy = rand() % 5;
		zz = rand() % 3;
		fish++;
		step = 0;

		if (fish > 99)
		{
			KillTimer(hWnd, 1);
			MessageBox(hWnd, TEXT("Game Clear"), TEXT("Victory"), MB_OK);
			PostQuitMessage(0);
		}
	}
	if (map[nowStage][yy][xx] == '#')
	{
		xx = rand() % 10;
		yy = rand() % 5;
		zz = rand() % 3;
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
	hBackBit = CreateCompatibleBitmap(hdc, 1000, 600);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//�ӽ� ��Ʈ�� �̹����� ���� ���� DC�ڵ� ����
	hMemDC2 = CreateCompatibleDC(hdc);
	//����� ����
	SelectObject(hMemDC2, background[3]);
	//������ hMemDC2�� ���� hMemDC�� ����
	BitBlt(hMemDC, 0, 0, 1000, 600, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	int i, j;
	for(i=0;i<=6;i++)
		for (j = 0; j <= 11; j++) {
			if(map[nowStage][i][j] == '#')
				TransBlt(hMemDC, j * 88, i * 88, background[5], RGB(255, 0, 0));
		}

	//��������� ���������� ���� �� hMemDC�� ����
	if(zz == nowStage)
		TransBlt(hMemDC, xx * 88, yy * 88, background[4], RGB(255, 0, 0));

	TransBlt(hMemDC, x*88, y*88, background[see], RGB(255, 0, 0));

	TransBlt(hMemDC, ex*88, ey*88, background[6], RGB(255, 0, 0));
	//������ �̹����� �׸���
	
	BitBlt(hdc, 0, 0, 1024, 768, hMemDC, 0, 0, SRCCOPY);

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