#include <windows.h>
//#include <cmath>//���� ���Ҷ� �ʿ��� ���
#include "resource.h"
//#include <mmsystem.h>//���带 ������ �ʿ��� ���
//#include <commctrl.h>//���� ��Ʈ���� ����(���α׷��� ��) ���
//#pragma comment(lib."winmm.lib")//��Ƽ�̵�� �Լ��� ����ϱ� ���� lib����

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//�ν��Ͻ� �ڵ�
LPCTSTR lpszClass = TEXT("Checker");
HWND hWndMain;

//��Ʈ�� ���ҽ� �ڵ�(��� �� ������)
HBITMAP background[10];

char Board[8][9] = {
	" 1 1 1 1",
	"1 1 1 1 ",
	" 1 1 1 1",
	"        ",
	"        ",
	"2 2 2 2 ",
	" 2 2 2 2",
	"2 2 2 2 ",
};
bool turn = true;	//flase : Red, true : Blue
int mPointX, mPointY;
int selectMouseX, selectMouseY;
void ClearPoint()	//��ǥ�� ����
{
	int i, j;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
			if (Board[j][i] == '5')
				Board[j][i] = ' ';
}
void CheckWinner(HWND getWnd, bool t)	//���� �˻�
{
	TCHAR msg[20];
	char turnNode[2];

	if (t)
	{
		wsprintf(msg, TEXT("�Ķ� ���� �̰���ϴ�."));
		turnNode[0] = '2';
		turnNode[1] = '4';
	}
	else
	{
		wsprintf(msg, TEXT("���� ���� �̰���ϴ�."));
		turnNode[0] = '1';
		turnNode[1] = '3';
	}

	for(int i=0;i<8;i++)
		for (int j = 0; j < 8; j++)
		{
			if (Board[i][j] == turnNode[0] || Board[i][j] == turnNode[1])
				return;
		}
	MessageBox(getWnd, msg, TEXT("���� ��"), MB_OK);
}

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
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
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
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
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
	TCHAR str[20];
	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1024, 768);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//��Ʈ�� ���ҽ����� Backgroud[]�迭�� ���
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//��� ���ҽ�
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//��Ÿ ���ҽ�
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//��Ÿ ���ҽ�
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//��Ÿ ���ҽ�
		background[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));//��Ÿ ���ҽ�
		background[6] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));//��Ÿ ���ҽ�
		background[7] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7));//��Ÿ ���ҽ�
		SetTimer(hWnd, 1, 50, NULL);
		//����� ����.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);

		return 0;
	case WM_LBUTTONDOWN:
		mPointX = LOWORD(lParam) / 50;
		mPointY = HIWORD(lParam) / 50;
		if (turn)	//Blue
		{
			if (Board[mPointY][mPointX] == '2' || Board[mPointY][mPointX] == '4')	//�Ķ� ���� Ŭ��
			{
				selectMouseX = mPointX;
				selectMouseY = mPointY;

				ClearPoint();

				//���� �밢��
				if (Board[mPointY - 1][mPointX - 1] == ' ')
					Board[mPointY - 1][mPointX - 1] = '5';
				else if (Board[mPointY - 1][mPointX - 1] == '1' || Board[mPointY - 1][mPointX - 1] == '3')	//�տ� ��밡 ������
					if(Board[mPointY - 2][mPointX - 2] == ' ')
					Board[mPointY - 2][mPointX - 2] = '5';

				//������ �밢��
				if (Board[mPointY - 1][mPointX + 1] == ' ')
					Board[mPointY - 1][mPointX + 1] = '5';
				else if (Board[mPointY - 1][mPointX + 1] == '1' || Board[mPointY - 1][mPointX + 1] == '3')
					if (Board[mPointY - 2][mPointX + 2] == ' ')
						Board[mPointY - 2][mPointX + 2] = '5';

				if (Board[mPointY][mPointX] == '4')	//�ڷ� �̵�
				{
					if (Board[mPointY + 1][mPointX - 1] == ' ')
						Board[mPointY + 1][mPointX - 1] = '5';
					else if (Board[mPointY + 1][mPointX - 1] == '1' || Board[mPointY + 1][mPointX - 1] == '3')
						if (Board[mPointY + 2][mPointX - 2] == ' ')
							Board[mPointY + 2][mPointX - 2] = '5';

					if (Board[mPointY + 1][mPointX + 1] == ' ')
						Board[mPointY + 1][mPointX + 1] = '5';
					else if (Board[mPointY + 1][mPointX + 1] == '1' || Board[mPointY + 1][mPointX + 1] == '3')
						if (Board[mPointY + 2][mPointX + 2] == ' ')
							Board[mPointY + 2][mPointX + 2] = '5';
				}
			}
		}
		else	//Red
		{
			if (Board[mPointY][mPointX] == '1' || Board[mPointY][mPointX] == '3')
			{
				selectMouseX = mPointX;
				selectMouseY = mPointY;

				ClearPoint();

				//���� �밢��
				if (Board[mPointY + 1][mPointX - 1] == ' ')
					Board[mPointY + 1][mPointX - 1] = '5';
				else if (Board[mPointY + 1][mPointX - 1] == '2' || Board[mPointY + 1][mPointX - 1] == '4')	//�տ� ��밡 ������
					if (Board[mPointY + 2][mPointX - 2] == ' ')
						Board[mPointY + 2][mPointX - 2] = '5';

				//������ �밢��
				if (Board[mPointY + 1][mPointX + 1] == ' ')
					Board[mPointY + 1][mPointX + 1] = '5';
				else if (Board[mPointY + 1][mPointX + 1] == '2' || Board[mPointY + 1][mPointX + 1] == '4')
					if (Board[mPointY + 2][mPointX + 2] == ' ')
						Board[mPointY + 2][mPointX + 2] = '5';

				if (Board[mPointY][mPointX] == '3')	//�ڷ� �̵�
				{
					if (Board[mPointY - 1][mPointX - 1] == ' ')
						Board[mPointY - 1][mPointX - 1] = '5';
					else if (Board[mPointY - 1][mPointX - 1] == '2' || Board[mPointY - 1][mPointX - 1] == '4')
						if (Board[mPointY - 2][mPointX - 2] == ' ')
							Board[mPointY - 2][mPointX - 2] = '5';

					if (Board[mPointY - 1][mPointX + 1] == ' ')
						Board[mPointY - 1][mPointX + 1] = '5';
					else if (Board[mPointY - 1][mPointX + 1] == '2' || Board[mPointY - 1][mPointX + 1] == '4')
						if (Board[mPointY - 2][mPointX + 2] == ' ')
							Board[mPointY - 2][mPointX + 2] = '5';
				}
			}
		}
		if (Board[mPointY][mPointX] == '5')
		{
			Board[mPointY][mPointX] = Board[selectMouseY][selectMouseX];
			Board[selectMouseY][selectMouseX] = ' ';

			ClearPoint();

			if (turn) //Blue
			{
				//���� ���� ��
				if (mPointY < selectMouseY)	//���� �ö� ��
				{
					if (mPointX < selectMouseX)
						if (Board[mPointY + 1][mPointX + 1] == '1' || Board[mPointY + 1][mPointX + 1] == '3')	//�������� �̵�
							Board[mPointY + 1][mPointX + 1] = ' ';
					if(mPointX > selectMouseX)
						if (Board[mPointY + 1][mPointX - 1] == '1' || Board[mPointY + 1][mPointX - 1] == '3')	//���������� �̵�
							Board[mPointY + 1][mPointX - 1] = ' ';
				}
				else if (mPointY > selectMouseY)
				{
					if(mPointX < selectMouseX)
						if (Board[mPointY - 1][mPointX + 1] == '1' || Board[mPointY - 1][mPointX + 1] == '3')	//�������� �̵�
							Board[mPointY - 1][mPointX + 1] = ' ';
					if(mPointX > selectMouseX)
						if (Board[mPointY - 1][mPointX - 1] == '1' || Board[mPointY - 1][mPointX - 1] == '3')	//���������� �̵�
							Board[mPointY - 1][mPointX - 1] = ' ';
				}

				//���� �ǳ����� �����ϸ� ����
				if (Board[0][mPointX] == '2')
					Board[mPointY][mPointX] = '4';
				turn = false;
			}
			else
			{
				if (mPointY > selectMouseY)
				{
					if(mPointX < selectMouseX)
						if (Board[mPointY - 1][mPointX + 1] == '2' || Board[mPointY - 1][mPointX + 1] == '4')	//�������� �̵�
							Board[mPointY - 1][mPointX + 1] = ' ';
					if (mPointX > selectMouseX)
						if (Board[mPointY - 1][mPointX - 1] == '2' || Board[mPointY - 1][mPointX - 1] == '4')	//���������� �̵�
							Board[mPointY - 1][mPointX - 1] = ' ';
				}
				else if (mPointY < selectMouseY)
				{
					if(mPointX < selectMouseX)
						if (Board[mPointY + 1][mPointX + 1] == '2' || Board[mPointY + 1][mPointX + 1] == '4')	//�������� �̵�
							Board[mPointY + 1][mPointX + 1] = ' ';
					if (mPointX > selectMouseX)
						if (Board[mPointY + 1][mPointX - 1] == '2' || Board[mPointY + 1][mPointX - 1] == '4')	//���������� �̵�
							Board[mPointY + 1][mPointX - 1] = ' ';
				}

				//���� �ǳ����� �����ϸ� ����
				if (Board[7][mPointX] == '1')
					Board[mPointY][mPointX] = '3';
				turn = true;
			}

			//�� ������ �˻�
			CheckWinner(hWnd, turn);
		}
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hWnd);
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
	hBackBit = CreateCompatibleBitmap(hdc, 400, 400);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//�ӽ� ��Ʈ�� �̹����� ���� ���� DC�ڵ� ����
	hMemDC2 = CreateCompatibleDC(hdc);
	//����� ����

	//������ hMemDC2�� ���� hMemDC�� ����
	BitBlt(hMemDC, 0, 0, 400, 400, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//�� �׸���
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			TransBlt(hMemDC, j * 50, i * 50, background[((i % 2) + (j % 2)) % 2 + 1], RGB(255, 0, 255));
			if (Board[i][j] == '1')
				TransBlt(hMemDC, j * 50, i * 50, background[3], RGB(255, 255, 255));
			else if(Board[i][j] == '2')
				TransBlt(hMemDC, j * 50, i * 50, background[5], RGB(255, 255, 255));
			else if (Board[i][j] == '3')
				TransBlt(hMemDC, j * 50, i * 50, background[4], RGB(255, 255, 255));
			else if (Board[i][j] == '4')
				TransBlt(hMemDC, j * 50, i * 50, background[6], RGB(255, 255, 255));
			else if (Board[i][j] == '5')
				TransBlt(hMemDC, j * 50, i * 50, background[7], RGB(255, 255, 255));
		}

	//�� �׸���
	

	//������ �̹����� �׸���
	BitBlt(hdc, 0, 0, 400, 400, hMemDC, 0, 0, SRCCOPY);

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