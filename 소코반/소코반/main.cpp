#include <windows.h>
//#include <cmath>//각을 구할때 필요한 헤더
#include "resource.h"
#include "maps.h"
//#include <mmsystem.h>//사운드를 넣을때 필요한 헤더
//#include <commctrl.h>//공통 컨트롤을 위한(프로그래스 바) 헤더
//#pragma comment(lib."winmm.lib")//멀티미디어 함수를 사용하기 위한 lib파일

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//인스턴스 핸들
LPCTSTR lpszClass = TEXT("소코반");
HWND hWndMain;

const int spriteSize = 50;
int nx, ny, nMove;
int moveCount = 0;
int hole = 0;
bool clear = false;

//비트맵 리소스 핸들(배경 및 아이콘)
HBITMAP background[5];

//비트맵 이미지의 필요 없는 배경을 없애 주는 함수
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);
//비트맵을 윈도우상에 그려주는 함수
void DrawBitmap(HDC hdc);
//스테이지 재정의
void InitStage();
//이동
void Move(int xDir, int yDir);

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

		//비트맵 리소스들을 Backgroud[]배열에 등록
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//배경 리소스
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//기타 리소스
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//기타 리소스
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//기타 리소스
		SetTimer(hWnd, 1, 50, NULL);
		//배경음 지정.
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
		wsprintf(moveStr, TEXT("이동한 횟수 : %d"), moveCount);
		TextOut(hdc, 50, 50 * 16+20, moveStr, lstrlen(moveStr));
		wsprintf(moveStr, TEXT("남은 칸 : %d"), hole);
		TextOut(hdc, 50, 50 * 16 + 40, moveStr, lstrlen(moveStr));
		if (hole == 0) clear = true;
		if (clear)
			TextOut(hdc, 512, 512, TEXT("스테이지 클리어!"), 9);
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
					//리셋
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
	hBackBit = CreateCompatibleBitmap(hdc, 1024, 1024);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//임시 비트맵 이미지를 가질 가상 DC핸들 지정
	hMemDC2 = CreateCompatibleDC(hdc);
	//배경을 선택
	SelectObject(hMemDC2, background[1]);

	//가상의 hMemDC2를 실제 hMemDC에 복사
	//BitBlt(hMemDC, 0, 0, 1024, 768, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//투명색으로 빨간색으로 지정 후 hMemDC에 복사

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

	//실제의 이미지를 그린다
	BitBlt(hdc, 0, 0, 1024, 1024, hMemDC, 0, 0, SRCCOPY);

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