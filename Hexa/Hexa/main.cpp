#include <windows.h>
#include <time.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Hexa");

#define RANDOM (rand() % 10) + 2

const int imgSize = 20;
struct Block {
	int x, y;
	int index[3];
}nowBlock, nextBlock;
int board[20][10];
int score;
bool gameOver = false;

void DrawScreen();
void DrawNextBlock();
void Turning(int direction);
void Falling();
void DestroyBlock();
void Reset();

HDC hdc;
HDC hMemDC[12];
HBITMAP hbrick[12];
HBITMAP brick[12];
int i;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;		//윈도우 핸들
	MSG Message;		//메시지 구조체
	WNDCLASS WndClass;	//윈도우 클래스 구조체
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;			//윈도우클레스 데이터 영역
	WndClass.cbWndExtra = 0;			//윈도우 데이터 영역
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//바탕색 브러쉬 핸들
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);			//커서 핸들
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);			//아이콘 핸들
	WndClass.hInstance = hInstance;		//인스턴스 핸들
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	//윈도우 프로시저 지정
	WndClass.lpszClassName = lpszClass;	//윈도우 클래스 이름
	WndClass.lpszMenuName = NULL;		//메뉴이름
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);		//윈도우 클래스 등록

	hWnd = CreateWindow(lpszClass,		//윈도우 클래스 이름
		lpszClass,		//타이틀바에 출력될문자열
		WS_OVERLAPPEDWINDOW,	//윈도우 스타일
		CW_USEDEFAULT,		//윈도우 좌상x좌표
		CW_USEDEFAULT,		//윈도우 좌상y좌표
		1024,		//윈도우 폭
		768,		//윈도우 높이
		NULL,			//부모 윈도우 핸들
		(HMENU)NULL,		//메뉴 혹은 자식 윈도우의 식별자
		hInstance,		//윈도우를 생성한 인스턴스 핸들 
		NULL 			//CREATESTRUCT 구조체를 통해 전달되는 값
	);


	if (!hWnd) return(FALSE);		//윈도우 생성실패시 종료

	ShowWindow(hWnd, nCmdShow);		//윈도우 화면에 나타내기
	UpdateWindow(hWnd);			//윈도우 클라이언트 영역을 칠한다.
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);	//키보드 메시지번역
		DispatchMessage(&Message);	//메시지를 해당프로시저로 보낸다
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	srand(time(NULL));

	PAINTSTRUCT ps;
	switch (iMessage) {
	case WM_CREATE:
		hdc = GetDC(hWnd);

		for (i = 1; i <= 12; i++)
		{
			hMemDC[i] = CreateCompatibleDC(hdc);
			hbrick[i]=LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1 +i-1)); //리소스 ID값이 연속적으로 정렬된경우
		}

		for (i = 1; i <= 12; i++)
			brick[i] = (HBITMAP)SelectObject(hMemDC[i], hbrick[i]);

		Reset();
		SetTimer(hWnd, 1, 500, NULL);

		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_KEYDOWN:
		GetDC(hWnd);
			switch (wParam)
			{
			case VK_LEFT:
				if (nowBlock.x > 0 && !gameOver)
					if (board[nowBlock.y][nowBlock.x - 1] < 1 || board[nowBlock.y + 1][nowBlock.x - 1] < 1 || board[nowBlock.y + 2][nowBlock.x - 1] < 1)
						nowBlock.x--;
				break;
			case VK_RIGHT:
				if (nowBlock.x < 9 && !gameOver)
					if (board[nowBlock.y][nowBlock.x + 1] < 1 || board[nowBlock.y + 1][nowBlock.x + 1] < 1 || board[nowBlock.y + 2][nowBlock.x + 1] < 1)
						nowBlock.x++;
				break;
			case VK_UP:
				Turning(-1);
				break;
			case VK_DOWN:
				Turning(1);
				break;
			case VK_SPACE:
				//블럭이 바로 내려가도록
				if(!gameOver)
					while (board[nowBlock.y + 3][nowBlock.x] < 2 && nowBlock.y + 3 < 20)
						nowBlock.y++;
				break;
			case 'R':
				if (gameOver)
					Reset();
			}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_TIMER:
		GetDC(hWnd);
		if (!gameOver)
		{
			switch (wParam)
			{
			case 1:
				if (nowBlock.y + 3 < 20 && board[nowBlock.y + 3][nowBlock.x] < 2)
					nowBlock.y++;
				else
				{
					for (int i = 0; i < 3; i++)
						board[nowBlock.y + i][nowBlock.x] = nowBlock.index[i];

					//블록 부수기
					DestroyBlock();

					//다음블럭 생성
					DrawNextBlock();
				}
				break;
			}
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawScreen();
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		for (i = 1; i <= 5; i++)
		{
			SelectObject(hMemDC[i], brick[i]);
			DeleteObject(hMemDC[i]);
		}
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawScreen()
{
	TCHAR ch[10];

	//바탕배경
	int x, y;
	for (x = 0; x < 10; x++)
		for (y = 0; y < 20; y++)
		{
			wsprintf(ch, TEXT("%d"), board[y][x]);
			TextOut(hdc, 500 + (x * 15), y * 15, ch, lstrlen(ch));
			BitBlt(hdc, (x+1) * imgSize, (y+1) * imgSize, imgSize, imgSize, hMemDC[board[y][x] > 1 ? board[y][x] : 12], 0, 0, SRCCOPY);
		}

	//테투리블럭
	x = 0;	y = 1;
	while (x < 12 || y < 21)
	{
		if (x < 12)
		{
			BitBlt(hdc, x * imgSize, 0 * imgSize, imgSize, imgSize, hMemDC[1], 0, 0, SRCCOPY);
			BitBlt(hdc, x * imgSize, 21 * imgSize, imgSize, imgSize, hMemDC[1], 0, 0, SRCCOPY);
			x++;
		}
		if (y < 21)
		{
			BitBlt(hdc, 0 * imgSize, y * imgSize, imgSize, imgSize, hMemDC[1], 0, 0, SRCCOPY);
			BitBlt(hdc, 11 * imgSize, y * imgSize, imgSize, imgSize, hMemDC[1], 0, 0, SRCCOPY);
			y++;
		}
	}
	//nowBlock
	BitBlt(hdc, (nowBlock.x+1) * imgSize, (nowBlock.y) * imgSize, imgSize, imgSize, hMemDC[nowBlock.index[0]], 0, 0, SRCCOPY);
	BitBlt(hdc, (nowBlock.x + 1) * imgSize, (nowBlock.y + 1) * imgSize, imgSize, imgSize, hMemDC[nowBlock.index[1]], 0, 0, SRCCOPY);
	BitBlt(hdc, (nowBlock.x + 1) * imgSize, (nowBlock.y + 2) * imgSize, imgSize, imgSize, hMemDC[nowBlock.index[2]], 0, 0, SRCCOPY);

	TextOut(hdc, 240, 20, TEXT("Next Block"), 10);
	BitBlt(hdc, (nextBlock.x + 13) * imgSize, (nextBlock.y + 2) * imgSize, imgSize, imgSize, hMemDC[nextBlock.index[0]], 0, 0, SRCCOPY);
	BitBlt(hdc, (nextBlock.x + 13) * imgSize, (nextBlock.y + 3) * imgSize, imgSize, imgSize, hMemDC[nextBlock.index[1]], 0, 0, SRCCOPY);
	BitBlt(hdc, (nextBlock.x + 13) * imgSize, (nextBlock.y + 4) * imgSize, imgSize, imgSize, hMemDC[nextBlock.index[2]], 0, 0, SRCCOPY);

	if (gameOver)
	{
		TextOut(hdc, 80, 200, TEXT("GAME OVER!"), 10);
		TextOut(hdc, 90, 220, TEXT("R to Reset"), 10);
	}
}

void Reset()
{
	gameOver = false;
	//Reset
	nowBlock.x = 5;
	nowBlock.y = 1;

	for (int i = 0; i < 3; i++)
	{
		nowBlock.index[i] = RANDOM;
		nextBlock.index[i] = RANDOM;
	}

	for (int y = 0; y < 20; y++)
		for (int x = 0; x < 10; x++)
			board[y][x] = 0;
}

void DrawNextBlock()
{
	for (int i = 0; i < 3; i++)
		nowBlock.index[i] = nextBlock.index[i];

	nowBlock.x = 5;
	nowBlock.y = 1;

	for (int i = 0; i < 3; i++)
		nextBlock.index[i] = RANDOM;

	//생성할 블록이 범위값을 넘기면 게임오버로
	if (board[nowBlock.y + 2][nowBlock.x] > 0 || board[nowBlock.y + 1][nowBlock.x] > 0)
		gameOver = true;
}

void Turning(int direction)
{
	int tmp;
	if (!gameOver)
		if (direction < 0)	//아래로 올리기
		{
			tmp = nowBlock.index[0];
			nowBlock.index[0] = nowBlock.index[1];
			nowBlock.index[1] = nowBlock.index[2];
			nowBlock.index[2] = tmp;
		}
		else if (direction > 0)	//위로 올리기
		{
			tmp = nowBlock.index[0];
			nowBlock.index[0] = nowBlock.index[2];
			nowBlock.index[2] = nowBlock.index[1];
			nowBlock.index[1] = tmp;
		}
}

void DestroyBlock()
{
	int x, y;
	int tmp = 1;
	int count = 0;
	int t;
	bool canRemove = false;
	bool blockCheck[20][10];	//블록을 부술수 있는지 체크
	for (y = 0; y < 20; y++)
		for (x = 0; x < 10; x++)
			blockCheck[y][x] = false;

	for (y = 0; y < 20; y++)
		for (x = 0; x < 10; x++)
		{
			t = board[y][x];

			if (t > 1)
			{
				//가로
				if (board[y][x - 1] == t && board[y][x + 1] == t) {
					for (int i = -1; i <= 1; i++)
						blockCheck[y][x + i] = true;
					canRemove = true;
				}
				//세로
				if (board[y - 1][x] == t && board[y + 1][x] == t) {
					for (int i = -1; i <= 1; i++)
						blockCheck[y + i][x] = true;
					canRemove = true;
				}
				//대각
				if (board[y - 1][x - 1] == t && board[y + 1][x + 1] == t) {
					for (int i = -1; i <= 1; i++)
						blockCheck[y + i][x + i] = true;
					canRemove = true;
				}
				if (board[y + 1][x - 1] == t && board[y - 1][x + 1] == t) {
					for (int i = -1; i <= 1; i++)
						blockCheck[y - i][x + i] = true;
					canRemove = true;
				}

				if (blockCheck[y][x])
				{
					board[y][x] = 0;
				}
			}
		}

	Falling();
}

void Falling()
{
	int empty;
	for (int x = 0; x < 10; x++)
		for (int y = 19; y > -1; y--)
		{
			if (board[y][x] > 1)
			{
				empty = 1;
				while (board[y + empty][x] == 0 && y + empty < 20)
				{
					board[y + empty][x] = board[y + (empty - 1)][x];
					board[y + (empty - 1)][x] = 0;
					empty++;
				}
			}
		}
}