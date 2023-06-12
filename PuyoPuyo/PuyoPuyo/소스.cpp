#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Puyo Puyo");

HWND hWnd;		//윈도우 핸들
HDC hdc;
HDC hMemDC[6];
HBITMAP hbrick[6];
HBITMAP brick[6];
int i;

#define Random(x) (rand() % x)+1;

struct Block {
	int x, y;
	int color;	//1~5까지의 색상
} nowBlock, nowBlock2, nextBlock, nextBlock2;
int downSpeed = 500;
int isBlock[24][11];
int score;
bool playable = true;	//닿았을 때 블록이 자동적으로 내려갈 때 못움직이도록

void DrawMap();
void Turn();
void Moving(int direction);
void MakeNewBlock();
void PopBlocks(Block lined);
void FallBlocks();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
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
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_CREATE:
		hdc = GetDC(hWnd);

		for (i = 1; i <= 5; i++)
		{
			hMemDC[i] = CreateCompatibleDC(hdc);
			// hbrick[i]=LoadBitmap(g_hInst, MAKEINTRESOURCE(first_BITMAP+i-1)); 리소스 ID값이 연속적으로 정렬된경우
		}
		// or
		hbrick[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		hbrick[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		hbrick[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
		hbrick[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
		hbrick[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));

		for (i = 1; i <= 5; i++)
			brick[i] = (HBITMAP)SelectObject(hMemDC[i], hbrick[i]);

		SetTimer(hWnd, 1, downSpeed, NULL);	//컨트롤타이머

		for (int y = 0; y < 24; y++)
			for (int x = 0; x < 11; x++)
				isBlock[y][x] = 0;

		score = 0;

		srand(time(NULL));
		nowBlock.x = 5;
		nowBlock.y = 2;
		nowBlock.color = Random(5);
		nowBlock2.x = nowBlock.x;
		nowBlock2.y = nowBlock.y - 1;
		nowBlock2.color = Random(5);

		nextBlock.x = 14;
		nextBlock.y = 3;
		nextBlock.color = Random(5);
		nextBlock2.x = nextBlock.x;
		nextBlock2.y = nextBlock.y - 1;
		nextBlock2.color = Random(5);
		
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_KEYDOWN:
		if (playable)
		{
			switch (wParam)
			{
				case VK_UP:
					Turn();
					break;
				case VK_DOWN:
					downSpeed = 100;
					break;
				case VK_LEFT:
					Moving(-1);
					break;
				case VK_RIGHT:
					Moving(1);
					break;
			}
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_DOWN:
			downSpeed = 500;
			break;
		}
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			SetTimer(hWnd, 1, downSpeed, NULL);
			playable = true;
			if ((nowBlock.y == 23 || nowBlock2.y == 23) ||
				(isBlock[nowBlock.y + 1][nowBlock.x] > 0 || isBlock[nowBlock2.y + 1][nowBlock2.x] > 0))	//바닥이나 떨어진 블록에 닿았으면
			{
				isBlock[nowBlock.y][nowBlock.x] = nowBlock.color;
				isBlock[nowBlock2.y][nowBlock2.x] = nowBlock2.color;

				//아래 빈공간이 있다면
				bool blockEmpty = isBlock[nowBlock.y + 1][nowBlock.x] < 1 && nowBlock.y < 23;
				bool blockEmpty2 = isBlock[nowBlock2.y + 1][nowBlock2.x] < 1 && nowBlock2.y < 23;
				if (blockEmpty || blockEmpty2)
				{
					KillTimer(hWnd, 1);
					SetTimer(hWnd, 2, downSpeed, NULL);
					break;
				}

				//똑같은색의 블록이 4개 이상 연결되면
				PopBlocks(nowBlock);
				PopBlocks(nowBlock2);

				KillTimer(hWnd, 1);
				SetTimer(hWnd, 3, downSpeed, NULL);
			}
			nowBlock.y++;
			nowBlock2.y++;
			break;
		case 2:	//블록이 닿고 떨어질 때
			SetTimer(hWnd, 2, downSpeed, NULL);
			DrawMap();
			playable = false;
			if (isBlock[nowBlock.y + 1][nowBlock.x] < 1 && nowBlock.y < 23)
			{
				isBlock[nowBlock.y][nowBlock.x] = 0;
				isBlock[nowBlock.y + 1][nowBlock.x] = nowBlock.color;
				nowBlock.y++;
			}
			else if (isBlock[nowBlock2.y + 1][nowBlock2.x] < 1 && nowBlock2.y < 23)
			{
				isBlock[nowBlock2.y][nowBlock2.x] = 0;
				isBlock[nowBlock2.y + 1][nowBlock2.x] = nowBlock2.color;
				nowBlock2.y++;
			}
			else
			{
				//똑같은색의 블록이 4개 이상 연결되면
				PopBlocks(nowBlock);
				PopBlocks(nowBlock2);

				KillTimer(hWnd, 2);
				SetTimer(hWnd, 3, downSpeed, NULL);
			}
			break;
		case 3:	//제거하고 또 떨어지기
			SetTimer(hWnd, 3, downSpeed, NULL);
			playable = false;
			for (int x = 0; x < 11; x++)
				for (int y = 22; y >= 0; y--)
				{
					if (isBlock[y + 1][x] < 1 && y+1 < 23)
					{
						isBlock[y + 1][x] = isBlock[y][x];
						isBlock[y][x] = 0;
					}
				}

			//이후 새블럭 만들기
			MakeNewBlock();

			KillTimer(hWnd, 3);
			SetTimer(hWnd, 1, downSpeed, NULL);
			break;
		}
	
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawMap();

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		for (i = 1; i <= 5; i++)
			SelectObject(hMemDC[i], brick[i]);
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
void DrawMap()
{
	hdc = GetDC(hWnd);
	// 11 x 24 블록화면
	Rectangle(hdc, 19, 19, 12 * 20 + 1, 25 * 20 + 1);
	//다음블럭 표시
	Rectangle(hdc, 260, 19, 340, 100);
	SetTextAlign(hdc, TA_CENTER);
	TextOut(hdc, 300, 18, TEXT("NEXT"), 4);

	TCHAR n[2];
	//쌓은 블럭 표시
	for (int y = 0; y < 24; y++)
		for (int x = 0; x < 11; x++)
		{
			if(isBlock[y][x]> 0)
			BitBlt(hdc, (x + 1) * 20, (y + 1) * 20, 20, 20, hMemDC[isBlock[y][x]], 0, 0, SRCCOPY);
			wsprintf(n, TEXT("%d"), isBlock[y][x]);
			TextOut(hdc, (15 * x) + 500, (15 * y) + 20, n, lstrlen(n));
		}

	if (playable)
	{
		BitBlt(hdc, (nowBlock.x + 1) * 20, nowBlock.y * 20, 300, 375, hMemDC[nowBlock.color], 0, 0, SRCCOPY);
		BitBlt(hdc, (nowBlock2.x + 1) * 20, nowBlock2.y * 20, 300, 375, hMemDC[nowBlock2.color], 0, 0, SRCCOPY);
	}
	BitBlt(hdc, nextBlock.x * 20, nextBlock.y * 20, 300, 375, hMemDC[nextBlock.color], 0, 0, SRCCOPY);
	BitBlt(hdc, nextBlock2.x * 20, nextBlock2.y * 20, 300, 375, hMemDC[nextBlock2.color], 0, 0, SRCCOPY);

	//점수 표시

}

void MakeNewBlock()
{
	nowBlock.x = 5;
	nowBlock.y = 2;
	nowBlock.color = nextBlock.color;
	nowBlock2.x = nowBlock.x;
	nowBlock2.y = nowBlock.y - 1;
	nowBlock2.color = nextBlock2.color;

	nextBlock.color = Random(5);
	nextBlock2.color = Random(5);
}

void Turn()
{
	if (nowBlock.y - nowBlock2.y > 0)	//중심블록이 위에 있으면
	{
		//오른쪽 벽에 붙어 있으면 중심블럭이 왼쪽으로 밀려남
		if (nowBlock.x == 10 || isBlock[nowBlock.y][nowBlock.x + 1] > 0)
		{
			nowBlock.x -= 1;
			nowBlock2.x -= 1;
		}
		nowBlock2.x = nowBlock.x + 1;
		nowBlock2.y = nowBlock.y;
	}
	else if (nowBlock.y - nowBlock2.y < 0)	//중심블록 아래에 있으면
	{
		//왼쪽 벽에 붙어 있으면 중심블럭이 오른쪽으로 밀려남
		if (nowBlock.x == 0 || isBlock[nowBlock.y][nowBlock.x - 1] > 0)
		{
			nowBlock.x += 1;
			nowBlock2.x += 1;
		}
		nowBlock2.x = nowBlock.x - 1;
		nowBlock2.y = nowBlock.y;
	}
	else if (nowBlock.x - nowBlock2.x > 0)	//중심블록이 왼쪽에 있으면
	{
		nowBlock2.x = nowBlock.x;
		nowBlock2.y = nowBlock.y - 1;
	}
	else if (nowBlock.x - nowBlock2.x < 0)	//중심블록이 오른쪽에 있으면
	{
		nowBlock2.x = nowBlock.x;
		nowBlock2.y = nowBlock.y + 1;
	}
}

void Moving(int direction)
{
	bool blockLimit, blockLimit2;
	bool sameHorizontal;

	bool sideBlock = isBlock[nowBlock.y][nowBlock.x + direction] > 0;
	bool sideBlock2 = isBlock[nowBlock2.y][nowBlock2.x + direction] > 0;

	if (direction < 0)	//왼쪽방향
	{
		blockLimit = nowBlock.x < nowBlock2.x && nowBlock.x > 0;
		blockLimit2 = nowBlock.x > nowBlock2.x && nowBlock2.x > 0;
		sameHorizontal = nowBlock.x > 0 && nowBlock2.x > 0;

		if (blockLimit || blockLimit2 || sameHorizontal)	//벽에 붙었으면
		{
			if (!sideBlock && !sideBlock2)	//옆에 블럭이 가로막지 않으면
			{
				nowBlock.x--;
				nowBlock2.x--;
			}
		}
	}
	else if (direction > 0)	//오른쪽방향
	{
		blockLimit = nowBlock.x > nowBlock2.x && nowBlock.x < 10;
		blockLimit2 = nowBlock.x < nowBlock2.x && nowBlock2.x < 10;
		sameHorizontal = nowBlock.x < 10 && nowBlock2.x < 10;

		if (blockLimit || blockLimit2 || sameHorizontal)	//벽에 붙었으면
		{
			if (!sideBlock && !sideBlock2)	//옆에 블럭이 가로막지 않으면
			{
				nowBlock.x++;
				nowBlock2.x++;
			}
		}
	}
}

void PopBlocks(Block lined)	//같은 블록이 4개 이상으로 연결되면 제거
{
	int count = 1;
	bool isPop[24][11];
	const int xFix = lined.x, yFix = lined.y;

	for (int y = 0; y < 24; y++)
		for (int x = 0; x < 11; x++)
			isPop[y][x] = false;

	if (isBlock[lined.y][lined.x] > 0)
	{
		isPop[lined.y][lined.x] = true;
		if (isBlock[lined.y][lined.x] == isBlock[lined.y - 1][lined.x])
		{
			isPop[lined.y - 1][lined.x] = true;
			count++;
		}

		//아래 검사
		while (isBlock[lined.y + 1][lined.x] == lined.color)
		{
			isPop[lined.y + 1][lined.x] = true;
			count++;
			lined.y++;

			while (isBlock[lined.y][lined.x - 1] == lined.color)
			{
				isPop[lined.y][lined.x - 1] = true;
				count++;
				lined.x--;
			}
			lined.x = xFix;
			while (isBlock[lined.y][lined.x + 1] == lined.color)
			{
				isPop[lined.y][lined.x + 1] = true;
				count++;
				lined.x++;
			}
			lined.x = xFix;
		}
		//좌우 검사
		lined.y = yFix;
		while (isBlock[lined.y][lined.x - 1] == lined.color)
		{
			isPop[lined.y][lined.x - 1] = true;
			count++;
			lined.x--;

			//왼쪽 상단에 블럭이 연결되어 있는지
			while (isBlock[lined.y - 1][lined.x] == lined.color)
			{
				isPop[lined.y - 1][lined.x] = true;
				count++;
				lined.y--;
			}
			lined.y = yFix;
			//아래측에서 블록이 있다면
			while (isBlock[lined.y + 1][lined.x] == lined.color)
			{
				if(!isPop[lined.y+1][lined.x])
				{
					isPop[lined.y + 1][lined.x] == true;
					count++;
				}
				lined.y++;
			}
			lined.y = yFix;
		}
		lined.x = xFix;
		while (isBlock[lined.y][lined.x + 1] == lined.color)
		{
			isPop[lined.y][lined.x + 1] = true;
			count++;
			lined.x++;
			//오른쪽 상단에 블럭이 연결되어 있는지
			while (isBlock[lined.y - 1][lined.x] == lined.color)
			{
				isPop[lined.y - 1][lined.x] = true;
				count++;
				lined.y--;
			}
			lined.y = yFix;
			//아래측에서 블록이 있다면
			while (isBlock[lined.y + 1][lined.x] == lined.color)
			{
				if (!isPop[lined.y + 1][lined.x])
				{
					isPop[lined.y + 1][lined.x] == true;
					count++;
				}
				lined.y++;
			}
			lined.y = yFix;
		}

		if (count >= 4)	//횟수가 4개 이상이면 제거
		{
			for (int y = 0; y < 24; y++)
				for (int x = 0; x < 11; x++)
					if (isPop[y][x])
						isBlock[y][x] = 0;

		}
		else
			return;
	}
}