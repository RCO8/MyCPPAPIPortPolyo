#include <windows.h>
//#include <cmath>//각을 구할때 필요한 헤더
#include "resource.h"
//#include <mmsystem.h>//사운드를 넣을때 필요한 헤더
//#include <commctrl.h>//공통 컨트롤을 위한(프로그래스 바) 헤더
//#pragma comment(lib."winmm.lib")//멀티미디어 함수를 사용하기 위한 lib파일

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//인스턴스 핸들
LPCTSTR lpszClass = TEXT("내 취향에 맞는 꽃");
HWND hWndMain;

//비트맵 리소스 핸들(배경 및 아이콘)
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

//비트맵 이미지의 필요 없는 배경을 없애 주는 함수
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);

//비트맵을 윈도우상에 그려주는 함수
void DrawBitmap(HDC hdc);

//꽃세기
void FlowerCount()
{
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < 20; j++)
			if (map[nowStage][i][j] == '1') flower[0]++;
			else if (map[nowStage][i][j] == '2') flower[1]++;
			else if (map[nowStage][i][j] == '3') flower[2]++;
}

//윈도우메인 부분
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	//윈도우 클래스 등록
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	//메뉴 등록 부분
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//UpdateWindow(hWnd);

	/*인삿말
	MessageBox(hWnd, TEXT("안녕하세요.\n혜광 컴퓨터 학원입니다."), TEXT("WELCOME TO
	FORTRESS WORLD!"), MB_OK);
	*/

	//메시지 루프
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

// 윈도우 메시지 처리
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

		//비트맵 리소스들을 Backgroud[]배열에 등록
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
			//꽃을 먹으면
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
		//게임 클리어
		if (flower[0] + flower[1] + flower[2] == 0) clear = true;

		//게임오버
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
			TextOut(hdc, 512, 384, TEXT("클리어! Enter키를 눌러 다음으로"), 19);	//스테이지 클리어
		if(gameOver)
			TextOut(hdc, 512, 384, TEXT("게임오버!"), 5);

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

COLORREF visibleColor = RGB(255, 255, 255);

/*비트맵 파일을 화면에 보이게할 함수.
더블 버퍼링을 이용하여 화면의 깜박이는 현상 없앰.
프로그래스와 에디트 부분은 아직 깜박이는 현상이 있음.
*/
void DrawBitmap(HDC hdc)
{
	//실제 비트멥 이미지를 가질 DC핸들과 가상의 DC핸들 변수선언
	HDC hMemDC, hMemDC2;
	HBITMAP hOldBitmap, hBackBit;

	//실제 비트멥 이미지를 가질 DC핸들 지정
	hMemDC = CreateCompatibleDC(hdc);
	hBackBit = CreateCompatibleBitmap(hdc, 1024, 768);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//임시 비트맵 이미지를 가질 가상 DC핸들 지정
	hMemDC2 = CreateCompatibleDC(hdc);
	//배경을 선택
	SelectObject(hMemDC2, background[1]);

	//가상의 hMemDC2를 실제 hMemDC에 복사
	BitBlt(hMemDC, 0, 0, 1024, 768, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//투명색으로 빨간색으로 지정 후 hMemDC에 복사

	const int spriteSize = 50;
	int enemyTmp = 0;
	for(int i=0;i<15;i++)
	{
		for (int j = 0; j < 20; j++)
		{
			//벽 그리기
			if (map[nowStage][i][j] == '#')
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[2], visibleColor);
			//꽃 그리기
			if (map[nowStage][i][j] == '1')	
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[4], visibleColor);
			else if (map[nowStage][i][j] == '2')	
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[5], visibleColor);
			else if (map[nowStage][i][j] == '3')	
				TransBlt(hMemDC, j * spriteSize, i * spriteSize, background[6], visibleColor);
			//오리 그리기
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


	//실제의 이미지를 그린다
	BitBlt(hdc, 0, 0, 1024, 768, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBackBit);
	DeleteDC(hMemDC);
}

/*
비트맵의 필요없는 배경부분을 없애줄수 있는 함수.
TransparentBle함수에 비하여 속도나 성능이 뛰어남.
*/
//* TransBlt 함수
// 투명 비트맵을 출력하는 함수. (x,y)에 출력 위치. clrMask에 투명색을 준다.
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