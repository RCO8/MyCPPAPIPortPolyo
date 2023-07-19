#include <windows.h>
#include <fstream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Class");

HDC hdc;
HWND buttons[6];
HWND input;

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

TCHAR buf[1024] = TEXT(" ");
DWORD dwRead, dwWrite;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HANDLE files[3];
	switch (iMessage) {
	case WM_CREATE:
		buttons[0] = CreateWindow(TEXT("button"), TEXT("저장1"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 50, 30, hWnd, (HMENU)0, g_hInst, NULL);
		buttons[1] = CreateWindow(TEXT("button"), TEXT("로드1"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 60, 10, 50, 30, hWnd, (HMENU)1, g_hInst, NULL);
		buttons[2] = CreateWindow(TEXT("button"), TEXT("저장2"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 40, 50, 30, hWnd, (HMENU)2, g_hInst, NULL);
		buttons[3] = CreateWindow(TEXT("button"), TEXT("로드2"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 60, 40, 50, 30, hWnd, (HMENU)3, g_hInst, NULL);
		buttons[4] = CreateWindow(TEXT("button"), TEXT("저장3"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 50, 30, hWnd, (HMENU)4, g_hInst, NULL);
		buttons[5] = CreateWindow(TEXT("button"), TEXT("로드3"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 60, 70, 50, 30, hWnd, (HMENU)5, g_hInst, NULL);
		input = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 170, 40, 120, 30, hWnd, (HMENU)7, g_hInst, NULL);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0:
			files[0] = CreateFile(TEXT("Data1.txt"), GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			WriteFile(files[0], buf, lstrlen(buf)*2, &dwWrite, NULL);
			CloseHandle(files[0]);
			break;
		case 1:
			files[0] = CreateFile(TEXT("Data1.txt"), GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (files[0] != INVALID_HANDLE_VALUE) {
				ReadFile(files[0], buf, lstrlen(buf), &dwRead, NULL);
				CloseHandle(files[0]);
			}
			else
				MessageBox(hWnd, TEXT("파일을 불러오지 못했습니다!!"), TEXT("Error"), MB_OK);
			break;

		case 2:
			files[1] = CreateFile(TEXT("Data2.txt"), GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			WriteFile(files[1], buf, lstrlen(buf) * 2, &dwWrite, NULL);
			CloseHandle(files[1]);
			break;
		case 3:
			files[1] = CreateFile(TEXT("Data2.txt"), GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (files[1] != INVALID_HANDLE_VALUE) {
				ReadFile(files[1], buf, lstrlen(buf), &dwRead, NULL);
				CloseHandle(files[1]);
			}
			else
				MessageBox(hWnd, TEXT("파일을 불러오지 못했습니다!!"), TEXT("Error"), MB_OK);
			break;

		case 4:
			files[2] = CreateFile(TEXT("Data3.txt"), GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			WriteFile(files[2], buf, lstrlen(buf)*2, &dwWrite, NULL);
			CloseHandle(files[2]);
		case 5:
			files[2] = CreateFile(TEXT("Data3.txt"), GENERIC_READ, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (files[2] != INVALID_HANDLE_VALUE) {
				ReadFile(files[2], buf, lstrlen(buf), &dwRead, NULL);
				CloseHandle(files[2]);
			}
			else
				MessageBox(hWnd, TEXT("파일을 불러오지 못했습니다!!"), TEXT("Error"), MB_OK);
			break;

		case 7:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				//입력한 내용을 buf에 저장
				GetWindowText(input, buf, 1024);
				SetWindowText(hWnd, buf);
			}
			return 0;
		}
		InvalidateRect(hWnd, NULL, 1);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 500, buf, lstrlen(buf));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}