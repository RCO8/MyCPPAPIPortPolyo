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
char targetPoint[10][10]; //말을 움직이게 하기위한 찍을 지점
static char setTower;	//알 선택
int tx, ty;	//알 이동
int preX, preY;	//알 출발점
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

void RemovePoint();
void DrawInCase(int t);	//각 궁전안에 있는 말들을 움직이게 하려고
void DrawKnight();
void DrawVihicle();
char DrawPoint(int x, int y)
{
	//t는 턴상태이며 같은 말끼리 찍어지지 않게 하려고
	if (turn == 1)	//초나라 차례
		return tower[y][x] >= 'A' && tower[y][x] <= 'Z' ? NULL : '*';
	else if (turn == 2)	//한나라 차례
		return tower[y][x] >= 'a' && tower[y][x] <= 'z' ? NULL : '*';
}
void MoveTower();

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
			int tmp = 1;
			preX = tx;
			preY = ty;
			switch (tower[ty][tx])
			{
				case 'K':
					RemovePoint();
					setTower = tower[ty][tx]; 
					DrawInCase(1);
					break;
				case 'C':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawVihicle();
					break;
				case 'P':
					MessageBox(hWnd, TEXT("포 선택"), TEXT("초나라"), MB_OK);
					break;
				case 'H':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawKnight();
					break;
				case 'E':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawKnight();
					break;
				case 'V':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawInCase(1);
					break;
				case 'S':
					RemovePoint();
					setTower = tower[ty][tx];
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
			preX = tx;
			preY = ty;
			switch(tower[ty][tx])
			{
				case 'k':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawInCase(2);
					break;
				case 'c':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawVihicle();
					break;
				case 'h':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawKnight();
					break;
				case 'e':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawKnight();
					break;
				case 'v':
					RemovePoint();
					setTower = tower[ty][tx];
					DrawInCase(2);
					break;
				case 's':
					RemovePoint();
					setTower = tower[ty][tx];
					if (ty > 0)
						targetPoint[ty + 1][tx] = '*';
					if (tx > 0)
						targetPoint[ty][tx - 1] = '*';
					if (tx < 9)
						targetPoint[ty][tx + 1] = '*';
					break;
			}
		}
		if (targetPoint[ty][tx] == '*')
		{
			tower[ty][tx] = setTower;
			tower[preY][preX] = ' ';
			RemovePoint();
			turn = turn == 1 ? 2 : 1;
		}
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_RBUTTONDOWN:
		hdc = GetDC(hWnd);
		RemovePoint();
		setTower = NULL;
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//판 그리기
		for(y=0;y<9;y++)
			for(x=0;x<8;x++)
				Rectangle(hdc, startPoint + (x* digit), startPoint + (y* digit),
					(startPoint+digit) + (x * digit), (startPoint + digit) + (y* digit));
		//중앙선
		SetPixel(hdc, startPoint, startPoint,RGB(0,255,0));
		MoveToEx(hdc, startPoint + (digit * 3), startPoint, NULL);
		LineTo(hdc, startPoint + (digit * 5), startPoint + (digit * 2));
		MoveToEx(hdc, startPoint + (digit * 5), startPoint, NULL);
		LineTo(hdc, startPoint + (digit * 3), startPoint + (digit * 2));

		MoveToEx(hdc, startPoint + (digit * 3), startPoint + (digit * 7), NULL);
		LineTo(hdc, startPoint + (digit * 5), startPoint + (digit * 9));
		MoveToEx(hdc, startPoint + (digit * 5), startPoint + (digit * 7), NULL);
		LineTo(hdc, startPoint + (digit * 3), startPoint + (digit * 9));

		//말 그리기
		for(y=0;y<10;y++)
			for(x=0;x<9;x++)
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
		return 0;
	case WM_DESTROY:
		for (i = 1; i <= 15; i++)
			SelectObject(hMemDC[i], brick[i]);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
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
	//위
	for (j = 1; j < 10; j++)
	{
		if (targetPoint[ty - j][tx] != ' ')
			return;
		targetPoint[ty - j][tx] = DrawPoint(tx, ty - j);
	}
	//아래
	for (i = 1; i < 10; i++)
	{
		if (targetPoint[ty + i][tx] != ' ')
			return;
		targetPoint[ty + i][tx] = DrawPoint(tx, ty + i);
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
	SelectObject(hMemDC2, hbrick[1]);

	//가상의 hMemDC2를 실제 hMemDC에 복사
	BitBlt(hMemDC, 0, 0, 1024, 768, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//투명색으로 빨간색으로 지정 후 hMemDC에 복사

	TransBlt(hMemDC, 0, 350, hbrick[2], RGB(255, 255, 255));
	//실제의 이미지를 그린다
	BitBlt(hdc, 0, 0, 1024, 768, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBackBit);
	DeleteDC(hMemDC);
}

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