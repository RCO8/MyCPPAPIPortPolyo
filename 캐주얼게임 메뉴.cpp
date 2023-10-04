#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Class");

HDC hdc;

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
HWND selectTeam, personal, group, roundCount, winCount, setCount, setPlayer;
int plPos = 1, winPos = 3;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	TCHAR str[128];
	RECT rt;
	int temp;
	switch (iMessage) {
	case WM_CREATE:
		//모드설정
		CreateWindow(TEXT("button"), TEXT("Select Mode"), WS_CHILD | WS_VISIBLE |
			BS_GROUPBOX, 5, 5, 225, 50, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("어드벤처"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 10, 20, 100, 30, hWnd, (HMENU)100, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("서바이벌"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 20, 100, 30, hWnd, (HMENU)101, g_hInst, NULL);
		CheckRadioButton(hWnd, 100, 101, 101);

		//규칙설정
		selectTeam = CreateWindow(TEXT("button"), TEXT("Rule"), WS_CHILD | WS_VISIBLE |
			BS_GROUPBOX, 5, 75, 225, 130, hWnd, (HMENU)0, g_hInst, NULL);
		personal = CreateWindow(TEXT("button"), TEXT("개인전"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 10, 90, 100, 30, hWnd, (HMENU)110, g_hInst, NULL);
		group = CreateWindow(TEXT("button"), TEXT("팀전"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 90, 100, 30, hWnd, (HMENU)111, g_hInst, NULL);
		roundCount = CreateWindow(TEXT("button"), TEXT("회차전"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 10, 130, 100, 30, hWnd, (HMENU)120, g_hInst, NULL);
		winCount = CreateWindow(TEXT("button"), TEXT("승점전"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 130, 100, 30, hWnd, (HMENU)121, g_hInst, NULL);
		CheckRadioButton(hWnd, 120, 121, 120);
		setCount = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 180, 180, 20, hWnd, (HMENU)122, g_hInst, NULL);
		SetScrollRange(setCount, SB_CTL, 1, 5, TRUE);
		SetScrollPos(setCount, SB_CTL, 3, TRUE);
		
		//인원설정
		setPlayer = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 5, 230, 220, 20, hWnd, (HMENU)130, g_hInst, NULL);
		SetScrollRange(setPlayer, SB_CTL, 1, 8, TRUE);
		SetScrollPos(setPlayer, SB_CTL, 1, TRUE);

		//시작 및 종료
		CreateWindow(TEXT("button"), TEXT("시작"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 270, 70, 30, hWnd, (HMENU)140, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("옵션"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 270, 70, 30, hWnd, (HMENU)141, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("종료"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 270, 70, 30, hWnd, (HMENU)142, g_hInst, NULL);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 100:	//어드벤처
			ShowWindow(selectTeam, 0);
			ShowWindow(personal, 0);
			ShowWindow(group, 0);
			ShowWindow(roundCount, 0);
			ShowWindow(winCount, 0);
			ShowWindow(setCount, 0);
			break;
		case 101:
			ShowWindow(selectTeam, 1);
			ShowWindow(personal, 1);
			ShowWindow(group, 1);
			ShowWindow(roundCount, 1);
			ShowWindow(winCount, 1);
			ShowWindow(setCount, 1);
			break;
		case 140:
			MessageBox(hWnd, TEXT("시작"), TEXT("Game"), MB_OK);
			break;
		case 141:
			MessageBox(hWnd, TEXT("옵션"), TEXT("Game"), MB_OK);
			break;
		case 142:
			MessageBox(hWnd, TEXT("종료"), TEXT("Game"), MB_OK);
			break;
		}
		return 0;
	case WM_HSCROLL:
		if ((HWND)lParam == setCount) temp = winPos;
		if ((HWND)lParam == setPlayer) temp = plPos;
		switch (LOWORD(wParam)) {
		case SB_LINELEFT:
			temp = max(1, temp - 1);
			break;
		case SB_LINERIGHT:
			temp = min(10, temp + 1);
			break;
		case SB_PAGELEFT:
			temp = max(1, temp - 10);
			break;
		case SB_PAGERIGHT:
			temp = min(10, temp + 10);
			break;
		case SB_THUMBTRACK:
			temp = HIWORD(wParam);
			break;
		}
		if ((HWND)lParam == setCount) winPos = temp;
		if ((HWND)lParam == setPlayer) plPos = temp;
		SetScrollPos((HWND)lParam, SB_CTL, temp, TRUE);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		wsprintf(str, TEXT("%2d"), winPos);
		TextOut(hdc, 200, 180, str, lstrlen(str));

		wsprintf(str, TEXT("%d Player"), plPos);
		TextOut(hdc, 230, 230, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
