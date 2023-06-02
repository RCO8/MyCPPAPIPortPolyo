#include <windows.h>
//#include <cmath>//각을 구할때 필요한 헤더
#include "resource.h"
//#include <mmsystem.h>//사운드를 넣을때 필요한 헤더
//#include <commctrl.h>//공통 컨트롤을 위한(프로그래스 바) 헤더
//#pragma comment(lib."winmm.lib")//멀티미디어 함수를 사용하기 위한 lib파일

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//인스턴스 핸들
LPCTSTR lpszClass = TEXT("Fortress");
HWND hWndMain;
bool gameStart = false;
bool autoMove = true;
int x = 1, y = 2;
int xx = 2, yy = 2, zz = 0;	//zz는 스테이지 위치
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

//비트맵 리소스 핸들(배경 및 아이콘)
HBITMAP background[10];

//비트맵 이미지의 필요 없는 배경을 없애 주는 함수
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);

//비트맵을 윈도우상에 그려주는 함수
void DrawBitmap(HDC hdc);

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
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	//메뉴 등록 부분
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		0, 0, 1000, 600,
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
	TCHAR score[10];
	
	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;

		//비트맵 리소스들을 Backgroud[]배열에 등록
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//기타 리소스
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//기타 리소스
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//배경 리소스
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//배경 리소스
		background[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));//블록 리소스
		background[6] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));//블록 리소스
		
		//배경음 지정.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);

		SetTimer(hWnd, 1, 300, NULL);
		return 0;
	case WM_TIMER:
		if (gameStart) {
			if (autoMove) {
				//플레이어 이동
				MoveCheck(hWnd);

			}


			//적 이동
			ex += eMove;
			if (x == ex && y == ey)	//플레이어와 부딫히면
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
			TextOut(hdc, 500, 300, TEXT("Enter를 누르면 게임이 시작됩니다."), 21);
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
	hBackBit = CreateCompatibleBitmap(hdc, 1000, 600);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//임시 비트맵 이미지를 가질 가상 DC핸들 지정
	hMemDC2 = CreateCompatibleDC(hdc);
	//배경을 선택
	SelectObject(hMemDC2, background[3]);
	//가상의 hMemDC2를 실제 hMemDC에 복사
	BitBlt(hMemDC, 0, 0, 1000, 600, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	int i, j;
	for(i=0;i<=6;i++)
		for (j = 0; j <= 11; j++) {
			if(map[nowStage][i][j] == '#')
				TransBlt(hMemDC, j * 88, i * 88, background[5], RGB(255, 0, 0));
		}

	//투명색으로 빨간색으로 지정 후 hMemDC에 복사
	if(zz == nowStage)
		TransBlt(hMemDC, xx * 88, yy * 88, background[4], RGB(255, 0, 0));

	TransBlt(hMemDC, x*88, y*88, background[see], RGB(255, 0, 0));

	TransBlt(hMemDC, ex*88, ey*88, background[6], RGB(255, 0, 0));
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