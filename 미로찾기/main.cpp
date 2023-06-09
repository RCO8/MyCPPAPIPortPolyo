#include <windows.h>
//#include <cmath>//���� ���Ҷ� �ʿ��� ���
#include "resource.h"
//#include <mmsystem.h>//���带 ������ �ʿ��� ���
//#include <commctrl.h>//���� ��Ʈ���� ����(���α׷��� ��) ���
//#pragma comment(lib."winmm.lib")//��Ƽ�̵�� �Լ��� ����ϱ� ���� lib����

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//�ν��Ͻ� �ڵ�
LPCTSTR lpszClass = TEXT("�� ���⿡ �´� ��");
HWND hWndMain;

//��Ʈ�� ���ҽ� �ڵ�(��� �� ������)
HBITMAP background[10];

/*
#a################ #
#         # #    #b#
######### #   ## # #
  #   #2# #####  # #
# # # # #     #  # #
#   # # ##### #  # #
##### #       ## # #
#     # # ###  # # #
# ##### # #1## # # #
# #     # #  # # # #
# # ##### ## # # # #
#   #        #   # #
################## #
                   #
####################
*/
char map[3][16][21] = 
{
		{
			{"####################"},
			{"#                  #"},
			{"################## #"},
			{"#   #1          #  #"},
			{"#h# ##### ##### # ##"},
			{"# #3#     #1       #"},
			{"# ################ #"},
			{"#            #   # #"},
			{"# ##########   # #h#"},
			{"#         2##### # #"},
			{"###### #####   #   #"},
			{"#    # #     #3#####"},
			{"# ## # # # #####   #"},
			{"# 1#   v #       #2#"},
			{"####################"}
		},
		{
			{"####################"},
			{"#         #        #"},
			{"#3##### # # ###### #"},
			{"###   # #   #   3# #"},
			{"#   # # ##### # ## #"},
			{"# ### #       #1#  #"},
			{"# #   ########### ##"},
			{"# # #           # 2#"},
			{"#   ####v# #### ####"},
			{"### #    # # 1#    #"},
			{"#   # #### # #######"},
			{"#2# #    # #       #"},
			{"### ## # # ##### # #"},
			{"#1  #3 #2#       #3#"},
			{"####################"}
		},
		{
			{"####################"},
			{"#   #        #     #"},
			{"# ### ###### # ### #"},
			{"# #   #   2#     # #"},
			{"#   # # ########## #"},
			{"##### # #      # # #"},
			{"#     # # # ####   #"},
			{"# ##### # #    # ###"},
			{"#       # ## # #   #"},
			{"#########  # # # # #"},
			{"#       ## ### # # #"},
			{"# #####    #   #1# #"},
			{"# #   ###### # ### #"},
			{"# #3#        # #   #"},
			{"####################"}
		}
};

enum moveState { up, down, left, right };
struct butterfly
{
	int x = 1, y = 1;
	moveState ms;
} player;
struct duck {
	int x;
	int y;
	int moveConvert = 1;
	// h : horizontal, v : vertical
	char HorizontalVertical;
} enemy[3];
int nowStage = 0;
int flower[3] = { 0,0,0 };
bool clear = false;
bool gameOver = false;

#define CHECKTOWALL(px, py, w) (map[nowStage][py][px] != w)

//��Ʈ�� �̹����� �ʿ� ���� ����� ���� �ִ� �Լ�
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);

//��Ʈ���� ������� �׷��ִ� �Լ�
void DrawBitmap(HDC hdc);

//�ɼ���
void FlowerCount()
{
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < 20; j++)
			if (map[nowStage][i][j] == '1') flower[0]++;
			else if (map[nowStage][i][j] == '2') flower[1]++;
			else if (map[nowStage][i][j] == '3') flower[2]++;
}

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

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1000, 750);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//��Ʈ�� ���ҽ����� Backgroud[]�迭�� ���
		for (int i = 1; i <= 7; i++) background[i]=LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1 +i-1));

		SetTimer(hWnd, 1, 200, NULL);
		SetTimer(hWnd, 2, 100, NULL);

		FlowerCount();
		nowStage = 0;

		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (!clear && !gameOver) {
				if (player.ms == up && CHECKTOWALL(player.x, player.y - 1, '#'))
					player.y -= 1;
				else if (player.ms == down && CHECKTOWALL(player.x, player.y + 1, '#'))
					player.y += 1;
				else if (player.ms == left && CHECKTOWALL(player.x - 1, player.y, '#'))
					player.x -= 1;
				else if (player.ms == right && CHECKTOWALL(player.x + 1, player.y, '#'))
					player.x += 1;
			}
			//���� ������
			if (map[nowStage][player.y][player.x] == '1')
			{
				map[nowStage][player.y][player.x] = ' ';
				flower[0]--;
			}
			else if (map[nowStage][player.y][player.x] == '2')
			{
				map[nowStage][player.y][player.x] = ' ';
				flower[1]--;
			}
			else if (map[nowStage][player.y][player.x] == '3')
			{
				map[nowStage][player.y][player.x] = ' ';
				flower[2]--;
			}
			break;
		case 2:
			if (!clear && !gameOver)
			{
				for(int i=0;i<3;i++)
					if (enemy[i].HorizontalVertical == 'h')
					{
						if (enemy[i].x == 0 || enemy[i].x == 19)
							enemy[i].moveConvert *= -1;

						enemy[i].x += enemy[i].moveConvert;
					}
					else if (enemy[i].HorizontalVertical == 'v')
					{
						if (enemy[i].y == 0 || enemy[i].y == 14)
							enemy[i].moveConvert *= -1;

						enemy[i].y += enemy[i].moveConvert;
					}
			}
			break;
		}
		//���� Ŭ����
		if (flower[0] + flower[1] + flower[2] == 0) clear = true;

		//���ӿ���
		for(int i=0;i<3;i++)
			if (player.x == enemy[i].x && player.y == enemy[i].y) gameOver = true;

		InvalidateRect(hWnd, NULL, 0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
			case VK_UP:
				player.ms = up;
				break;
			case VK_DOWN:
				player.ms = down;
				break;
			case VK_LEFT:
				player.ms = left;
				break;
			case VK_RIGHT:
				player.ms = right;
				break;
			case VK_RETURN:
				if (clear)
				{
					clear = false;
					nowStage++;
					player.x = 1;
					player.y = 1;
					FlowerCount();
				}
				else if (gameOver)
				{
					gameOver = false;
					player.x = 1;
					player.y = 1;
					FlowerCount();
				}
				break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hWnd);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBitmap(hdc);
		SetTextAlign(hdc, TA_CENTER);
		if (clear)
			TextOut(hdc, 512, 384, TEXT("Ŭ����! EnterŰ�� ���� ��������"), 19);	//�������� Ŭ����
		if(gameOver)
			TextOut(hdc, 512, 384, TEXT("���ӿ���!"), 5);

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

COLORREF visibleColor = RGB(255, 255, 255);

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
	//����� ����
	SelectObject(hMemDC2, background[1]);

	//������ hMemDC2�� ���� hMemDC�� ����
	BitBlt(hMemDC, 0, 0, 1024, 768, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//��������� ���������� ���� �� hMemDC�� ����

	const int spriteSize = 50;
	int enemyTmp = 0;
	for(int i=0;i<15;i++)
	{
		for (int j = 0; j < 20; j++)
		{
			//�� �׸���
			if (map[nowStage][i][j] == '#')
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[2], visibleColor);
			//�� �׸���
			if (map[nowStage][i][j] == '1')	
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[4], visibleColor);
			else if (map[nowStage][i][j] == '2')	
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[5], visibleColor);
			else if (map[nowStage][i][j] == '3')	
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[6], visibleColor);
			//���� �׸���
			if (map[nowStage][i][j] == 'h' || map[nowStage][i][j] == 'v')
			{
				enemy[enemyTmp].HorizontalVertical = map[nowStage][i][j];
				enemy[enemyTmp].x = j;
				enemy[enemyTmp].y = i;
				enemyTmp++;
				map[nowStage][i][j] = ' ';
			}
		}
	}
	TransBlt(hMemDC, player.x* spriteSize, player.y* spriteSize, background[3], RGB(255, 0, 0));
	for (int i = 0; i < 3; i++)
		TransBlt(hMemDC, enemy[i].x * spriteSize, enemy[i].y * spriteSize, background[7], visibleColor);


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