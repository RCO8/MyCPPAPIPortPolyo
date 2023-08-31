#include <windows.h>
//#include <cmath>//각을 구할때 필요한 헤더
#include "resource.h"
#include "Datas.h"
//#include <mmsystem.h>//사운드를 넣을때 필요한 헤더
//#include <commctrl.h>//공통 컨트롤을 위한(프로그래스 바) 헤더
//#pragma comment(lib."winmm.lib")//멀티미디어 함수를 사용하기 위한 lib파일

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//인스턴스 핸들
LPCTSTR lpszClass = TEXT("Fortress");
HWND hWndMain;

//비트맵 리소스 핸들(배경 및 아이콘)
HBITMAP background[50];

struct Player {
	int x, y;
	int width = 32, height = 32;
	/*
	* 미동, 걸음, 달리기, 밀기, 던지기, 걷어차기, 점프, 밀리기, 죽음, 워프
	*/
	HBITMAP sprites[54];
	int frame;
	//버튼을 누르는 판정
	int pressKey = 0;	
	int pressJump = 0;
	int startPosY;	//점프할때 시작점 위치

	void MoveHorizontal()
	{
		if (pressKey > 0)
		{
			//벽을 통과 못하게
			x += 1;
		}
		else if (pressKey < 0)
		{
			x -= 1;
		}

		//끝에 넘어가지 못하기
		if (x <= 0) x = 0;
		if (x >= 224) x = 224;
	}

	void Gravity()
	{

	}
}nick, tom;

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
	TCHAR tmp[10];
	TCHAR dataPath[20] = TEXT("stageFiles\\");
	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1024, 768);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		for (int i = 0; i < maxStage; i++)
			background[i + 1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1 + i));//배경 리소스
		//배경음 지정.
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
			//다음 스테이지로
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
			//프레임 제어

			//좌우로 이동
			nick.MoveHorizontal();

			//점프 및 내려가기
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
				//또한 바닥에 닿을때까지 떨어지기
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
			//좌우반전 해제
			nick.pressKey = -1;
			break;
		case VK_RIGHT:
			//좌우반전
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
	//배경 그리기
	SelectObject(hMemDC2, background[stage]);
	BitBlt(hMemDC, 0, 16 + moveFloor, 256, 224 + moveFloor, hMemDC2, 0, 0, SRCCOPY);
	//다음배경 대비하여 전환을 위해
	SelectObject(hMemDC2, background[stage+1]);
	BitBlt(hMemDC, 0, (16 - 208) + moveFloor, 256, 224 + moveFloor, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//투명색으로 빨간색으로 지정 후 hMemDC에 복사

	TransBlt(hMemDC, nick.x, nick.y, nick.sprites[nick.frame], RGB(255, 255, 255));

	//실제의 이미지를 그린다
	BitBlt(hdc, 0, 0, 256, 224, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBackBit);
	DeleteDC(hMemDC);
}