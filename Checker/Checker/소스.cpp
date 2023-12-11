#include <windows.h>
//#include <cmath>//각을 구할때 필요한 헤더
#include "resource.h"
//#include <mmsystem.h>//사운드를 넣을때 필요한 헤더
//#include <commctrl.h>//공통 컨트롤을 위한(프로그래스 바) 헤더
//#pragma comment(lib."winmm.lib")//멀티미디어 함수를 사용하기 위한 lib파일

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//인스턴스 핸들
LPCTSTR lpszClass = TEXT("Checker");
HWND hWndMain;

//비트맵 리소스 핸들(배경 및 아이콘)
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
void ClearPoint()	//목표점 제거
{
	int i, j;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
			if (Board[j][i] == '5')
				Board[j][i] = ' ';
}
void CheckWinner(HWND getWnd, bool t)	//승패 검사
{
	TCHAR msg[20];
	char turnNode[2];

	if (t)
	{
		wsprintf(msg, TEXT("파란 말이 이겼습니다."));
		turnNode[0] = '2';
		turnNode[1] = '4';
	}
	else
	{
		wsprintf(msg, TEXT("빨간 말이 이겼습니다."));
		turnNode[0] = '1';
		turnNode[1] = '3';
	}

	for(int i=0;i<8;i++)
		for (int j = 0; j < 8; j++)
		{
			if (Board[i][j] == turnNode[0] || Board[i][j] == turnNode[1])
				return;
		}
	MessageBox(getWnd, msg, TEXT("게임 끝"), MB_OK);
}

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
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
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
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
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
	TCHAR str[20];
	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1024, 768);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//비트맵 리소스들을 Backgroud[]배열에 등록
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//배경 리소스
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//기타 리소스
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//기타 리소스
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//기타 리소스
		background[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));//기타 리소스
		background[6] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));//기타 리소스
		background[7] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7));//기타 리소스
		SetTimer(hWnd, 1, 50, NULL);
		//배경음 지정.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);

		return 0;
	case WM_LBUTTONDOWN:
		mPointX = LOWORD(lParam) / 50;
		mPointY = HIWORD(lParam) / 50;
		if (turn)	//Blue
		{
			if (Board[mPointY][mPointX] == '2' || Board[mPointY][mPointX] == '4')	//파란 말을 클릭
			{
				selectMouseX = mPointX;
				selectMouseY = mPointY;

				ClearPoint();

				//왼쪽 대각선
				if (Board[mPointY - 1][mPointX - 1] == ' ')
					Board[mPointY - 1][mPointX - 1] = '5';
				else if (Board[mPointY - 1][mPointX - 1] == '1' || Board[mPointY - 1][mPointX - 1] == '3')	//앞에 상대가 있으면
					if(Board[mPointY - 2][mPointX - 2] == ' ')
					Board[mPointY - 2][mPointX - 2] = '5';

				//오른쪽 대각선
				if (Board[mPointY - 1][mPointX + 1] == ' ')
					Board[mPointY - 1][mPointX + 1] = '5';
				else if (Board[mPointY - 1][mPointX + 1] == '1' || Board[mPointY - 1][mPointX + 1] == '3')
					if (Board[mPointY - 2][mPointX + 2] == ' ')
						Board[mPointY - 2][mPointX + 2] = '5';

				if (Board[mPointY][mPointX] == '4')	//뒤로 이동
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

				//왼쪽 대각선
				if (Board[mPointY + 1][mPointX - 1] == ' ')
					Board[mPointY + 1][mPointX - 1] = '5';
				else if (Board[mPointY + 1][mPointX - 1] == '2' || Board[mPointY + 1][mPointX - 1] == '4')	//앞에 상대가 있으면
					if (Board[mPointY + 2][mPointX - 2] == ' ')
						Board[mPointY + 2][mPointX - 2] = '5';

				//오른쪽 대각선
				if (Board[mPointY + 1][mPointX + 1] == ' ')
					Board[mPointY + 1][mPointX + 1] = '5';
				else if (Board[mPointY + 1][mPointX + 1] == '2' || Board[mPointY + 1][mPointX + 1] == '4')
					if (Board[mPointY + 2][mPointX + 2] == ' ')
						Board[mPointY + 2][mPointX + 2] = '5';

				if (Board[mPointY][mPointX] == '3')	//뒤로 이동
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
				//말을 잡을 때
				if (mPointY < selectMouseY)	//위로 올라갈 때
				{
					if (mPointX < selectMouseX)
						if (Board[mPointY + 1][mPointX + 1] == '1' || Board[mPointY + 1][mPointX + 1] == '3')	//왼쪽으로 이동
							Board[mPointY + 1][mPointX + 1] = ' ';
					if(mPointX > selectMouseX)
						if (Board[mPointY + 1][mPointX - 1] == '1' || Board[mPointY + 1][mPointX - 1] == '3')	//오른쪽으로 이동
							Board[mPointY + 1][mPointX - 1] = ' ';
				}
				else if (mPointY > selectMouseY)
				{
					if(mPointX < selectMouseX)
						if (Board[mPointY - 1][mPointX + 1] == '1' || Board[mPointY - 1][mPointX + 1] == '3')	//왼쪽으로 이동
							Board[mPointY - 1][mPointX + 1] = ' ';
					if(mPointX > selectMouseX)
						if (Board[mPointY - 1][mPointX - 1] == '1' || Board[mPointY - 1][mPointX - 1] == '3')	//오른쪽으로 이동
							Board[mPointY - 1][mPointX - 1] = ' ';
				}

				//말이 맨끝까지 도착하면 변신
				if (Board[0][mPointX] == '2')
					Board[mPointY][mPointX] = '4';
				turn = false;
			}
			else
			{
				if (mPointY > selectMouseY)
				{
					if(mPointX < selectMouseX)
						if (Board[mPointY - 1][mPointX + 1] == '2' || Board[mPointY - 1][mPointX + 1] == '4')	//왼쪽으로 이동
							Board[mPointY - 1][mPointX + 1] = ' ';
					if (mPointX > selectMouseX)
						if (Board[mPointY - 1][mPointX - 1] == '2' || Board[mPointY - 1][mPointX - 1] == '4')	//오른쪽으로 이동
							Board[mPointY - 1][mPointX - 1] = ' ';
				}
				else if (mPointY < selectMouseY)
				{
					if(mPointX < selectMouseX)
						if (Board[mPointY + 1][mPointX + 1] == '2' || Board[mPointY + 1][mPointX + 1] == '4')	//왼쪽으로 이동
							Board[mPointY + 1][mPointX + 1] = ' ';
					if (mPointX > selectMouseX)
						if (Board[mPointY + 1][mPointX - 1] == '2' || Board[mPointY + 1][mPointX - 1] == '4')	//오른쪽으로 이동
							Board[mPointY + 1][mPointX - 1] = ' ';
				}

				//말이 맨끝까지 도착하면 변신
				if (Board[7][mPointX] == '1')
					Board[mPointY][mPointX] = '3';
				turn = true;
			}

			//각 말개수 검사
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
	hBackBit = CreateCompatibleBitmap(hdc, 400, 400);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//임시 비트맵 이미지를 가질 가상 DC핸들 지정
	hMemDC2 = CreateCompatibleDC(hdc);
	//배경을 선택

	//가상의 hMemDC2를 실제 hMemDC에 복사
	BitBlt(hMemDC, 0, 0, 400, 400, hMemDC2, 0, 0, SRCCOPY);
	DeleteDC(hMemDC2);

	//판 그리기
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

	//말 그리기
	

	//실제의 이미지를 그린다
	BitBlt(hdc, 0, 0, 400, 400, hMemDC, 0, 0, SRCCOPY);

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