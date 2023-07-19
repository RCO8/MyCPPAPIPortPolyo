#include <windows.h>
//#include <cmath>//���� ���Ҷ� �ʿ��� ���
#include "resource.h"
//#include <mmsystem.h>//���带 ������ �ʿ��� ���
//#include <commctrl.h>//���� ��Ʈ���� ����(���α׷��� ��) ���
//#pragma comment(lib."winmm.lib")//��Ƽ�̵�� �Լ��� ����ϱ� ���� lib����

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//�ν��Ͻ� �ڵ�
LPCTSTR lpszClass = TEXT("Chess");
HWND hWndMain;
char towers[8][9] = {
	"RNBQKBNR",
	"PPPPPPPP",
	"        ",
	"        ",
	"        ",
	"        ",
	"pppppppp",
	"rnbqkbnr"
};
char targetPoint[8][9];
int pointX, pointY;
int selectX, selectY;
int turn = 1;
char selectTower;

void RemovePoints();
void MovePawn();
void MoveKnight();
void MoveRook();
void MoveBishop();
void MoveKing();
char DrawPoint(int X, int Y);

//��Ʈ�� ���ҽ� �ڵ�(��� �� ������)
HBITMAP background[15];	//��, ŷ, ���, ����Ʈ, ��, �� ������

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
	TCHAR str[256];

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 600, 600);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//��� ���ҽ�
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		//��� ��
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
		background[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
		background[6] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		background[7] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7));
		//��� ��
		background[8]  = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP8));
		background[9]  = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9));
		background[10] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP10));
		background[11] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP11));
		background[12] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP12));
		background[13] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP13));
		//�̵��� ��
		background[14] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP14));

		return 0;
	case WM_LBUTTONDOWN:
		pointX = (LOWORD(lParam) / 75);
		pointY = (HIWORD(lParam) / 75);
		if (targetPoint[pointY][pointX] == 'x')
		{
			towers[pointY][pointX] = selectTower;
			towers[selectY][selectX] = ' ';

			//���� ������ ���� �� �ٲٱ�
			if (selectTower == 'p' && pointY == 0)
				MessageBox(hWnd, TEXT("�ٲܸ��� �����ϼ���"), TEXT("��� �� ����!"), MB_OK);
			if (selectTower == 'P' && pointY == 7)
				MessageBox(hWnd, TEXT("�ٲܸ��� �����ϼ���"), TEXT("��� �� ����!"), MB_OK);

			turn = turn == 1 ? 2 : 1;
		}
		RemovePoints();
		if (turn == 1)
		{
			selectX = pointX;
			selectY = pointY;
			switch (towers[pointY][pointX])
			{
			case 'k':
				selectTower = towers[pointY][pointX];
				MoveKing();
				break;
			case 'q':
				selectTower = towers[pointY][pointX];
				MoveRook();
				MoveBishop();
				break;
			case 'b':
				selectTower = towers[pointY][pointX];
				MoveBishop();
				break;
			case 'n':
				selectTower = towers[pointY][pointX];
				MoveKnight();
				break;
			case 'r':
				selectTower = towers[pointY][pointX];
				MoveRook();
				break;
			case 'p':
				selectTower = towers[pointY][pointX];
				MovePawn();
				break;
			}
		}
		else if (turn == 2)
		{
			selectX = pointX;
			selectY = pointY;
			switch(towers[pointY][pointX])
			{
				case 'K':
					selectTower = towers[pointY][pointX];
					MoveKing();
					break;
				case 'Q':
					selectTower = towers[pointY][pointX];
					MoveRook();
					MoveBishop();
					break;
				case 'B':
					selectTower = towers[pointY][pointX];
					MoveBishop();
					break;
				case 'N':
					selectTower = towers[pointY][pointX];
					MoveKnight();
					break;
				case 'R':
					selectTower = towers[pointY][pointX];
					MoveRook();
					break;
				case 'P':
					selectTower = towers[pointY][pointX];
					MovePawn();
					break;
			}
		}
		InvalidateRect(hWnd, 0, NULL);
		return 0;
	case WM_RBUTTONDOWN:
		RemovePoints();
		InvalidateRect(hWnd, 0, NULL);
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

char DrawPoint(int X, int Y)
{
	if (turn == 1)	//�����
	{
		return towers[Y][X] >= 'a' ? NULL : 'x';
	}
	else if(turn == 2) //�����
	{
		return towers[Y][X] >= 'A' && towers[Y][X] <= 'Z' ? NULL : 'x';
	}
}
void RemovePoints()
{
	for (int y = 0; y < 8; y++)
		for (int x = 0; x < 8; x++)
			if (targetPoint[y][x] != ' ')
				targetPoint[y][x] = ' ';
}
void MovePawn()
{
	if (selectTower == 'P')
	{
		if (towers[pointY + 1][pointX] == ' ')
		{
			targetPoint[pointY + 1][pointX] = DrawPoint(pointX, pointY + 1);
			if (pointY == 1 && towers[pointY + 2][pointX] == ' ')
				targetPoint[pointY + 2][pointX] = DrawPoint(pointX, pointY + 2);
		}

		//�밢���� ���� ������ ǥ���ϱ�
		if (towers[pointY + 1][pointX - 1] >= 'a') 
			targetPoint[pointY + 1][pointX - 1] = DrawPoint(pointX - 1, pointY + 1);
		if (towers[pointY + 1][pointX + 1] >= 'a') 
			targetPoint[pointY + 1][pointX + 1] = DrawPoint(pointX + 1, pointY + 1);
	}
	if (selectTower == 'p')
	{
		if (towers[pointY - 1][pointX] == ' ')
		{
			targetPoint[pointY - 1][pointX] = DrawPoint(pointX, pointY - 1);
			if (pointY == 6 && towers[pointY - 2][pointX] == ' ')
				targetPoint[pointY - 2][pointX] = DrawPoint(pointX, pointY - 2);
		}

		//�밢���� ���� ������ ǥ���ϱ�
		if (towers[pointY - 1][pointX - 1] <= 'Z' && towers[pointY - 1][pointX - 1] >= 'A')
			targetPoint[pointY - 1][pointX - 1] = DrawPoint(pointX - 1, pointY - 1);
		if (towers[pointY - 1][pointX + 1] <= 'Z' && towers[pointY - 1][pointX + 1] >= 'A')
			targetPoint[pointY - 1][pointX + 1] = DrawPoint(pointX + 1, pointY - 1);
	}
}
void MoveKnight()
{
	if (pointY > 1)
	{
		if(pointX > 0) targetPoint[pointY - 2][pointX - 1] = DrawPoint(pointX - 1, pointY - 2);
		if(pointX < 7) targetPoint[pointY - 2][pointX + 1] = DrawPoint(pointX + 1, pointY - 2);
	}
	if (pointY < 6)
	{
		if (pointX > 0) targetPoint[pointY + 2][pointX - 1] = DrawPoint(pointX - 1, pointY + 2);
		if (pointX < 7) targetPoint[pointY + 2][pointX + 1] = DrawPoint(pointX + 1, pointY + 2);
	}
	if (pointX > 1)
	{
		if (pointY > 0) targetPoint[pointY - 1][pointX - 2] = DrawPoint(pointX - 2, pointY - 1);
		if (pointY < 7) targetPoint[pointY + 1][pointX - 2] = DrawPoint(pointX - 2, pointY + 1);
	}
	if (pointX < 6)
	{
		if (pointY > 0) targetPoint[pointY - 1][pointX + 2] = DrawPoint(pointX + 2, pointY - 1);
		if (pointY < 7) targetPoint[pointY + 1][pointX + 2] = DrawPoint(pointX + 2, pointY + 1);
	}
}
void MoveRook()
{
	bool canMove[4] = { true,true,true,true };
	for (int i = 1; i < 8; i++)
	{
		if (pointY - i >= 0 && canMove[0])
		{
			targetPoint[pointY - i][pointX] = DrawPoint(pointX, pointY - i);
			if (towers[pointY - i][pointX] != ' ')
				canMove[0] = false;
		}
		if (pointY + i <= 7 && canMove[1])
		{
			targetPoint[pointY + i][pointX] = DrawPoint(pointX, pointY + i);
			if (towers[pointY + i][pointX] != ' ')
				canMove[1] = false;
		}
		if (pointX - i >= 0 && canMove[2])
		{
			targetPoint[pointY][pointX - i] = DrawPoint(pointX - i, pointY);
			if (towers[pointY][pointX - i] != ' ')
				canMove[2] = false;
		}
		if (pointX + i <= 7 && canMove[3])
		{
			targetPoint[pointY][pointX + i] = DrawPoint(pointX + i, pointY);
			if (towers[pointY][pointX + i] != ' ')
				canMove[3] = false;
		}
	}
}
void MoveBishop()
{
	bool canMove[4] = { true,true,true,true };
	for (int i = 1; i < 8; i++)
	{
		if (pointY - i >= 0 && pointX - i >= 0 && canMove[0])
		{
			targetPoint[pointY - i][pointX - i] = DrawPoint(pointX - i, pointY - i);
			if (towers[pointY - i][pointX - i] != ' ')
				canMove[0] = false;
		}
		if (pointY + i <= 7 && pointX - i >= 0 && canMove[1])
		{
			targetPoint[pointY + i][pointX - i] = DrawPoint(pointX - i, pointY + i);
			if (towers[pointY + i][pointX - i] != ' ')
				canMove[1] = false;
		}
		if (pointY - i >= 0 && pointX + i <= 7 && canMove[2])
		{
			targetPoint[pointY - i][pointX + i] = DrawPoint(pointX + i, pointY - i);
			if (towers[pointY - i][pointX + i] != ' ')
				canMove[2] = false;
		}
		if (pointY + i <= 7 && pointX + i <= 7 && canMove[3])
		{
			targetPoint[pointY + i][pointX + i] = DrawPoint(pointX + i, pointY + i);
			if (towers[pointY + i][pointX + i] != ' ')
				canMove[3] = false;
		}
	}
}
void MoveKing()
{
	if (pointY > 0)
	{
		targetPoint[pointY - 1][pointX - 1] = DrawPoint(pointX - 1, pointY - 1);
		targetPoint[pointY - 1][pointX] = DrawPoint(pointX, pointY - 1);
		targetPoint[pointY - 1][pointX + 1] = DrawPoint(pointX + 1, pointY - 1);
	}
	if(pointX > 0) targetPoint[pointY][pointX - 1] = DrawPoint(pointX - 1, pointY);
	if(pointX < 7) targetPoint[pointY][pointX + 1] = DrawPoint(pointX + 1, pointY);
	if (pointY < 7)
	{
		targetPoint[pointY + 1][pointX - 1] = DrawPoint(pointX - 1, pointY + 1);
		targetPoint[pointY + 1][pointX] = DrawPoint(pointX, pointY + 1);
		targetPoint[pointY + 1][pointX + 1] = DrawPoint(pointX + 1, pointY + 1);
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
	hBackBit = CreateCompatibleBitmap(hdc, 600, 600);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//�ӽ� ��Ʈ�� �̹����� ���� ���� DC�ڵ� ����
	hMemDC2 = CreateCompatibleDC(hdc);
	//����� ����
	SelectObject(hMemDC2, background[1]);

	//������ hMemDC2�� ���� hMemDC�� ����
	BitBlt(hMemDC, 0, 0, 600, 600, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//��������� ���������� ���� �� hMemDC�� ����
	for (int y = 0; y < 8; y++)
		for (int x = 0; x < 8; x++)
		{
			if (targetPoint[y][x] == 'x')
			{
				TransBlt(hMemDC, x * 75, y * 75, background[14], RGB(255, 0, 0));
			}
			switch (towers[y][x])
			{
				case 'K':
					TransBlt(hMemDC, x * 75, y * 75, background[2], RGB(255, 0, 0));
					break;
				case 'Q':
					TransBlt(hMemDC, x * 75, y * 75, background[3], RGB(255, 0, 0));
					break;
				case 'B':
					TransBlt(hMemDC, x * 75, y * 75, background[4], RGB(255, 0, 0));
					break;
				case 'N':
					TransBlt(hMemDC, x * 75, y * 75, background[5], RGB(255, 0, 0));
					break;
				case 'R':
					TransBlt(hMemDC, x * 75, y * 75, background[6], RGB(255, 0, 0));
					break;
				case 'P':
					TransBlt(hMemDC, x * 75, y * 75, background[7], RGB(255, 0, 0));
					break;
				case 'k':
					TransBlt(hMemDC, x * 75, y * 75, background[8], RGB(255, 0, 0));
					break;
				case 'q':
					TransBlt(hMemDC, x * 75, y * 75, background[9], RGB(255, 0, 0));
					break;
				case 'b':
					TransBlt(hMemDC, x * 75, y * 75, background[10], RGB(255, 0, 0));
					break;
				case 'n':
					TransBlt(hMemDC, x * 75, y * 75, background[11], RGB(255, 0, 0));
					break;
				case 'r':
					TransBlt(hMemDC, x * 75, y * 75, background[12], RGB(255, 0, 0));
					break;
				case 'p':
					TransBlt(hMemDC, x * 75, y * 75, background[13], RGB(255, 0, 0));
					break;
			}
		}
	//������ �̹����� �׸���
	BitBlt(hdc, 0, 0, 600, 600, hMemDC, 0, 0, SRCCOPY);

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