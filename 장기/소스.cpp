#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("장기");

HDC hdc;
HDC hMemDC[16];
HBITMAP hbrick[16];
HBITMAP brick[16];
int i;
const int smallImgLength = 38;
const int largeImgLength = 48;
const int giantImgLength = 56;
const int startPoint = 30;
const int digit = 70;
int turn = 1;	//1 : 초나라, 2 : 한나라
char tower[10][10] = {	//판에 세울 말들 (소문자 : 한나라, 대문자 : 초나라)
	"cehv vehc",
	"    k    ",
	" p     p ",
	"s s s s s",
	"         ",
	"         ",
	"S S S S S",
	" P     P ",
	"    K    ",
	"CEHV VEHC"
};
char towerTmp[10][10];
char targetPoint[10][10]; //말을 움직이게 하기위한 찍을 지점
static char setTower;	//알 선택
int tx, ty;	//알 이동
static int preX, preY;	//알 출발점
int x, y;

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

void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);
void DrawBitmap(HDC hdc);

void DrawBoard();	//판을 그린다
void RemovePoint();	//점을 모두 지운다
void DrawInCase(int t);	//각 궁전안에 있는 말들을 움직이게 하려고
void DrawKnight();	//상, 마 이동
void DrawVihicle();	//차 이동
void DrawSniper();	//포 이동
char DrawPoint(int x, int y);
void MoveTower();	//말이 선택되었을 때

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	TCHAR str[256];
	switch (iMessage) {
	case WM_CREATE:
		hdc = GetDC(hWnd);

		for (i = 1; i <= 15; i++)
		{
			hMemDC[i] = CreateCompatibleDC(hdc);
			hbrick[i]=LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1+i-1)); // 리소스 ID값이 연속적으로 정렬된경우
			brick[i] = (HBITMAP)SelectObject(hMemDC[i], hbrick[i]);
		}
		for (x = 0; x < 9; x++)
			for (y = 0; y < 10; y++)
				targetPoint[y][x] = ' ';
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		tx = LOWORD(lParam) / digit;
		ty = HIWORD(lParam) / digit;
		if (turn == 1)
		{
			switch (tower[ty][tx])
			{
				case 'K':
					MoveTower();
					DrawInCase(1);
					break;
				case 'C':
					MoveTower();
					DrawVihicle();
					break;
				case 'P':
					MoveTower();
					DrawSniper();
					break;
				case 'H':
					MoveTower();
					DrawKnight();
					break;
				case 'E':
					MoveTower();
					DrawKnight();
					break;
				case 'V':
					MoveTower();
					DrawInCase(1);
					break;
				case 'S':
					MoveTower();
					if (ty > 0)	//앞
						targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
					if(tx > 0)	//좌
						targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
					if(tx < 9)	//우
						targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
					break;
			}
		}
		else if (turn == 2)
		{
			switch(tower[ty][tx])
			{
				case 'k':
					MoveTower();
					DrawInCase(2);
					break;
				case 'c':
					MoveTower();
					DrawVihicle();
					break;
				case 'p':
					MoveTower();
					DrawSniper();
					break;
				case 'h':
					MoveTower();
					DrawKnight();
					break;
				case 'e':
					MoveTower();
					DrawKnight();
					break;
				case 'v':
					MoveTower();
					DrawInCase(2);
					break;
				case 's':
					MoveTower();
					if (ty > 0)
						targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
					if (tx > 0)
						targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
					if (tx < 9)
						targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
					break;
			}
		}
		if (targetPoint[ty][tx] == '*')
		{
			bool ChoWin = tower[ty][tx] == 'k' && turn == 1;
			bool HanWin = tower[ty][tx] == 'K' && turn == 2;
			tower[ty][tx] = setTower;
			tower[preY][preX] = ' ';
			RemovePoint();
			//왕을 잡으면 승리판정
			if (ChoWin || HanWin)
			{
				if(ChoWin)
					wsprintf(str, TEXT("초나라 승리!!!"));
				else if(HanWin)
					wsprintf(str, TEXT("한나라 승리!!!"));

				InvalidateRect(hWnd, NULL, true);
				MessageBox(hWnd, str, TEXT("게임 끝"), MB_OK);
				PostQuitMessage(0);
			}
			turn = turn == 1 ? 2 : 1;
		}
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_RBUTTONDOWN:
		hdc = GetDC(hWnd);
		RemovePoint();
		setTower = NULL;
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		DrawBoard();

		return 0;
	case WM_DESTROY:
		for (i = 1; i <= 15; i++)
			SelectObject(hMemDC[i], brick[i]);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void MoveTower()
{
	RemovePoint();
	preX = tx; preY = ty;
	setTower = tower[ty][tx];
}

void DrawInCase(int t)
{
	if (t == 1) {
		switch (tx)
		{
		case 3: //왼쪽
			if (ty == 7)
			{
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty + 1][tx + 1] = DrawPoint(tx + 1, ty + 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			else if (ty == 8)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
			}
			else if (ty == 9)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty - 1][tx + 1] = DrawPoint(tx + 1, ty - 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			break;
		case 4: //가운데
			if (ty == 7)
			{
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			else if (ty == 8)
			{
				targetPoint[ty - 1][tx - 1] = DrawPoint(tx - 1, ty - 1);
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty - 1][tx + 1] = DrawPoint(tx + 1, ty - 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
				targetPoint[ty + 1][tx - 1] = DrawPoint(tx - 1, ty + 1);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty + 1][tx + 1] = DrawPoint(tx + 1, ty + 1);
			}
			else if (ty == 9)
			{
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			break;
		case 5: //오른쪽
			if (ty == 7)
			{
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty + 1][tx - 1] = DrawPoint(tx - 1, ty + 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
			}
			else if (ty == 8)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
			}
			else if (ty == 9)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty - 1][tx - 1] = DrawPoint(tx - 1, ty - 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
			}
			break;
		}
	}
	else if (t == 2) {
		switch (tx)
		{
		case 3: //왼쪽
			if (ty == 0)
			{
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty + 1][tx + 1] = DrawPoint(tx + 1, ty + 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			else if (ty == 1)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
			}
			else if (ty == 2)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty - 1][tx + 1] = DrawPoint(tx + 1, ty - 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			break;
		case 4: //가운데
			if (ty == 0)
			{
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			else if (ty == 1)
			{
				targetPoint[ty - 1][tx - 1] = DrawPoint(tx - 1, ty - 1);
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty - 1][tx + 1] = DrawPoint(tx + 1, ty - 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
				targetPoint[ty + 1][tx - 1] = DrawPoint(tx - 1, ty + 1);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty + 1][tx + 1] = DrawPoint(tx + 1, ty + 1);
			}
			else if (ty == 2)
			{
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
			}
			break;
		case 5: //오른쪽
			if (ty == 0)
			{
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
				targetPoint[ty + 1][tx - 1] = DrawPoint(tx - 1, ty + 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
			}
			else if (ty == 1)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
				targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
			}
			else if (ty == 2)
			{
				targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
				targetPoint[ty - 1][tx - 1] = DrawPoint(tx - 1, ty - 1);
				targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
			}
			break;
		}
	}
}

void DrawVihicle()
{
	int j;
	for (j = ty - 1; j >= 0; j--)	//위
	{
		targetPoint[j][tx] = DrawPoint(tx, j);
		if (tower[j][tx] != ' ')
			break;
	}
	for (j = ty + 1; j < 10; j++)	//아래
	{
		targetPoint[j][tx] = DrawPoint(tx, j);
		if (tower[j][tx] != ' ')
			break;
	}
	for (j = tx - 1; j >= 0; j--)	//왼쪽
	{
		targetPoint[ty][j] = DrawPoint(j, ty);
		if (tower[ty][j] != ' ')
			break;
	}
	for (j = tx + 1; j < 9; j++)	//오른쪽
	{
		targetPoint[ty][j] = DrawPoint(j, ty);
		if (tower[ty][j] != ' ')
			break;
	}
}

void DrawKnight()
{
	if (tower[ty - 1][tx] == ' ')
	{
		if (setTower == 'e' || setTower == 'E')
		{
			if (tower[ty - 2][tx - 1] == ' ')
				targetPoint[ty - 3][tx - 2] = DrawPoint(tx - 2, ty - 3);
			if (tower[ty - 2][tx + 1] == ' ')
				targetPoint[ty - 3][tx + 2] = DrawPoint(tx + 2, ty - 3);
		}
		else if (setTower == 'h' || setTower == 'H')
		{
			targetPoint[ty - 2][tx - 1] = DrawPoint(tx - 1, ty - 2);
			targetPoint[ty - 2][tx + 1] = DrawPoint(tx + 1, ty - 2);
		}
	}
	if (tower[ty + 1][tx] == ' ')
	{
		if (setTower == 'e' || setTower == 'E')
		{
			if (tower[ty + 2][tx - 1] == ' ')
				targetPoint[ty + 3][tx - 2] = DrawPoint(tx - 2, ty + 3);
			if (tower[ty + 2][tx + 1] == ' ')
				targetPoint[ty + 3][tx + 2] = DrawPoint(tx + 2, ty + 3);
		}
		else if (setTower == 'h' || setTower == 'H')
		{
			targetPoint[ty + 2][tx - 1] = DrawPoint(tx - 1, ty + 2);
			targetPoint[ty + 2][tx + 1] = DrawPoint(tx + 1, ty + 2);
		}
	}
	if (tower[ty][tx - 1] == ' ')
	{
		if (setTower == 'e' || setTower == 'E')
		{
			if (tower[ty - 1][tx - 2] == ' ')
				targetPoint[ty - 2][tx - 3] = DrawPoint(tx - 3, ty - 2);
			if (tower[ty + 1][tx - 2] == ' ')
				targetPoint[ty + 2][tx - 3] = DrawPoint(tx - 3, ty + 2);
		}
		else if (setTower == 'h' || setTower == 'H') 
		{
			targetPoint[ty - 1][tx - 2] = DrawPoint(tx - 2, ty - 1);
			targetPoint[ty + 1][tx - 2] = DrawPoint(tx - 2, ty + 1);
		}
	}
	if (tower[ty][tx + 1] == ' ')
	{
		if (setTower == 'e' || setTower == 'E')
		{
			if (tower[ty - 1][tx + 2] == ' ')
				targetPoint[ty - 2][tx + 3] = DrawPoint(tx + 3, ty - 2);
			if (tower[ty + 1][tx + 2] == ' ')
				targetPoint[ty + 2][tx + 3] = DrawPoint(tx + 3, ty + 2);
		}
		else if (setTower == 'h' || setTower == 'H')
		{
			targetPoint[ty - 1][tx + 2] = DrawPoint(tx + 2, ty - 1);
			targetPoint[ty + 1][tx + 2] = DrawPoint(tx + 2, ty + 1);
		}
	}
}

void DrawSniper()
{
	int enableMove, j;
	//앞에 포를 제외한 말이 있는지
	for (j = ty - 1; j >= 0; j--)	//위
	{
		if (tower[j][tx] == 'p' || tower[j][tx] == 'P')
			break;
		if (tower[j][tx] != ' ')
		{
			enableMove = j - 1;
			for (j = enableMove; j >= 0; j--)
			{
				targetPoint[j][tx] = DrawPoint(tx, j);
				if (tower[j][tx] != ' ' || tower[j - 1][tx] == 'p' || tower[j - 1][tx] == 'P')
					break;
			}
			break;
		}
	}
	for (j = ty + 1; j < 10; j++)	//아래
	{
		if (tower[j][tx] == 'p' || tower[j][tx] == 'P')
			break;
		if (tower[j][tx] != ' ')
		{
			enableMove = j + 1;
			for (j = enableMove; j < 10; j++)
			{
				targetPoint[j][tx] = DrawPoint(tx, j);
				if (tower[j][tx] != ' ' || tower[j + 1][tx] == 'p' || tower[j + 1][tx] == 'P')
					break;
			}
			break;
		}
	}
	for (j = tx - 1; j >= 0; j--)	//왼쪽
	{
		if (tower[ty][j] == 'p' || tower[ty][j] == 'P')
			break;
		if (tower[ty][j] != ' ')
		{
			enableMove = j - 1;
			for (j = enableMove; j >= 0; j--)
			{
				targetPoint[ty][j] = DrawPoint(j, ty);
				if (tower[ty][j] != ' ' || tower[ty][j - 1] == 'p' || tower[ty][j - 1] == 'P')
					break;
			}
			break;
		}
	}
	for (j = tx + 1; j < 9; j++)	//오른쪽
	{
		if (tower[ty][j] == 'p' || tower[ty][j] == 'P')
			break;
		if (tower[ty][j] != ' ')
		{
			enableMove = j + 1;
			for (j = enableMove; j < 9; j++)
			{
				targetPoint[ty][j] = DrawPoint(j, ty);
				if (tower[ty][j] != ' ' || tower[ty][j + 1] == 'p' || tower[ty][j + 1] == 'P')
					break;
			}
			break;
		}
	}
}

char DrawPoint(int x, int y)
{
	//t는 턴상태이며 같은 말끼리 찍어지지 않게 하려고
	if (turn == 1)	//초나라 차례
		return (tower[y][x] >= 'A' && tower[y][x] <= 'Z') || (setTower == 'P' && tower[y][x] == 'p') ? NULL : '*';
	else if (turn == 2)	//한나라 차례
		return (tower[y][x] >= 'a' && tower[y][x] <= 'z') || (setTower == 'p' && tower[y][x] == 'P') ? NULL : '*';
}

void RemovePoint()
{
	for (y = 0; y < 10; y++)
		for (x = 0; x < 9; x++)
		{
			if (targetPoint[y][x] == ' ')
				continue;
			targetPoint[y][x] = ' ';
		}
}

void DrawBoard()
{
	//판 그리기
	for (y = 0; y < 9; y++)
		for (x = 0; x < 8; x++)
			Rectangle(hdc, startPoint + (x * digit), startPoint + (y * digit),
				(startPoint + digit) + (x * digit), (startPoint + digit) + (y * digit));

	//중앙선
	SetPixel(hdc, startPoint, startPoint, RGB(0, 255, 0));
	MoveToEx(hdc, startPoint + (digit * 3), startPoint, NULL);
	LineTo(hdc, startPoint + (digit * 5), startPoint + (digit * 2));
	MoveToEx(hdc, startPoint + (digit * 5), startPoint, NULL);
	LineTo(hdc, startPoint + (digit * 3), startPoint + (digit * 2));

	MoveToEx(hdc, startPoint + (digit * 3), startPoint + (digit * 7), NULL);
	LineTo(hdc, startPoint + (digit * 5), startPoint + (digit * 9));
	MoveToEx(hdc, startPoint + (digit * 5), startPoint + (digit * 7), NULL);
	LineTo(hdc, startPoint + (digit * 3), startPoint + (digit * 9));

	//말 그리기
	for (y = 0; y < 10; y++)
		for (x = 0; x < 9; x++)
		{
			switch (tower[y][x])
			{
				//한나라
			case 'k':
				BitBlt(hdc, startPoint - (giantImgLength / 2) + (digit * x), startPoint - (giantImgLength / 2) + (digit * y),
					giantImgLength, giantImgLength, hMemDC[1], 0, 0, SRCCOPY);
				break;
			case 'c':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[2], 0, 0, SRCCOPY);
				break;
			case 'p':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[3], 0, 0, SRCCOPY);
				break;
			case 'h':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[4], 0, 0, SRCCOPY);
				break;
			case 'e':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[5], 0, 0, SRCCOPY);
				break;
			case 'v':
				BitBlt(hdc, startPoint - (smallImgLength / 2) + (digit * x), startPoint - (smallImgLength / 2) + (digit * y),
					smallImgLength, smallImgLength, hMemDC[6], 0, 0, SRCCOPY);
				break;
			case 's':
				BitBlt(hdc, startPoint - (smallImgLength / 2) + (digit * x), startPoint - (smallImgLength / 2) + (digit * y),
					smallImgLength, smallImgLength, hMemDC[7], 0, 0, SRCCOPY);
				break;
				//초나라
			case 'K':
				BitBlt(hdc, startPoint - (giantImgLength / 2) + (digit * x), startPoint - (giantImgLength / 2) + (digit * y),
					giantImgLength, giantImgLength, hMemDC[8], 0, 0, SRCCOPY);
				break;
			case 'C':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[9], 0, 0, SRCCOPY);
				break;
			case 'P':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[10], 0, 0, SRCCOPY);
				break;
			case 'H':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[11], 0, 0, SRCCOPY);
				break;
			case 'E':
				BitBlt(hdc, startPoint - (largeImgLength / 2) + (digit * x), startPoint - (largeImgLength / 2) + (digit * y),
					largeImgLength, largeImgLength, hMemDC[12], 0, 0, SRCCOPY);
				break;
			case 'V':
				BitBlt(hdc, startPoint - (smallImgLength / 2) + (digit * x), startPoint - (smallImgLength / 2) + (digit * y),
					smallImgLength, smallImgLength, hMemDC[13], 0, 0, SRCCOPY);
				break;
			case 'S':
				BitBlt(hdc, startPoint - (smallImgLength / 2) + (digit * x), startPoint - (smallImgLength / 2) + (digit * y),
					smallImgLength, smallImgLength, hMemDC[14], 0, 0, SRCCOPY);
				break;
			}
			if (targetPoint[y][x] == '*')
			{
				BitBlt(hdc, startPoint - 10 + (digit * x), startPoint - 10 + (digit * y),
					20, 20, hMemDC[15], 0, 0, SRCCOPY);
			}
		}
}