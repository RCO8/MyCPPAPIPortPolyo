//이건 일반코딩으로 구현한 소스
//빈 프로젝트 항목에 전체 복사 붙여넣기만 하면 됨

#include <windows.h>
#include <stdlib.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Class");

HDC hdc;

int round = 2;
int numberBoard[3][12];
int questNumber;
int resultNumber = 12 * round + 1;

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
		0,		//윈도우 좌상x좌표
		0,		//윈도우 좌상y좌표
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
	TCHAR tmp[10];
	int xMouse, yMouse;
	switch (iMessage) {
	case WM_CREATE:
		questNumber = rand() % (resultNumber + 1);

		for (int j = 0; j < round; j++)
			for (int i = 0; i < 12; i++)
				numberBoard[j][i] = (j * 12) + i + 1;
		return 0;
	case WM_LBUTTONDOWN:
		xMouse = LOWORD(lParam);
		yMouse = HIWORD(lParam);

		for (int j = 0; j < round; j++)
			for (int i = 0; i < 12; i++)
				if ((xMouse >= (i * 40) + 100 && xMouse <= (i * 40) + 140) && (yMouse >= (j * 40) + 10 && yMouse <= (j * 40) + 50))
					if (resultNumber - questNumber == numberBoard[j][i])
						if (MessageBox(hWnd, TEXT("통과"), TEXT(".."), MB_OK) == IDOK)
							questNumber = rand() % (resultNumber + 1);

		InvalidateRect(hWnd, NULL, true);

		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		Rectangle(hdc, 10, 10, 50, 50);
		wsprintf(tmp, TEXT("%d"), questNumber);
		TextOut(hdc, 20, 20, tmp, lstrlen(tmp));

		for (int j = 0; j < round; j++)
			for (int i = 0; i < 12; i++)
			{
				Rectangle(hdc, (i * 40) + 100, (j * 40) + 10, (i * 40) + 140, (j * 40) + 50);
				wsprintf(tmp, TEXT("%d"), numberBoard[j][i]);
				TextOut(hdc, (i * 40) + 110, (j * 40) + 20, tmp, lstrlen(tmp));
			}

		Rectangle(hdc, 630, 10, 670, 50);
		wsprintf(tmp, TEXT("%d"), resultNumber);
		TextOut(hdc, 640, 20, tmp, 2);

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
