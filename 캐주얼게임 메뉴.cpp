#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Class");

HDC hdc;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;		//������ �ڵ�
	MSG Message;		//�޽��� ����ü
	WNDCLASS WndClass;	//������ Ŭ���� ����ü
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;			//������Ŭ���� ������ ����
	WndClass.cbWndExtra = 0;			//������ ������ ����
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//������ �귯�� �ڵ�
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);			//Ŀ�� �ڵ�
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);			//������ �ڵ�
	WndClass.hInstance = hInstance;		//�ν��Ͻ� �ڵ�
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	//������ ���ν��� ����
	WndClass.lpszClassName = lpszClass;	//������ Ŭ���� �̸�
	WndClass.lpszMenuName = NULL;		//�޴��̸�
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);		//������ Ŭ���� ���

	hWnd = CreateWindow(lpszClass,		//������ Ŭ���� �̸�
		lpszClass,		//Ÿ��Ʋ�ٿ� ��µɹ��ڿ�
		WS_OVERLAPPEDWINDOW,	//������ ��Ÿ��
		0,		//������ �»�x��ǥ
		0,		//������ �»�y��ǥ
		1024,		//������ ��
		768,		//������ ����
		NULL,			//�θ� ������ �ڵ�
		(HMENU)NULL,		//�޴� Ȥ�� �ڽ� �������� �ĺ���
		hInstance,		//�����츦 ������ �ν��Ͻ� �ڵ� 
		NULL 			//CREATESTRUCT ����ü�� ���� ���޵Ǵ� ��
	);


	if (!hWnd) return(FALSE);		//������ �������н� ����

	ShowWindow(hWnd, nCmdShow);		//������ ȭ�鿡 ��Ÿ����
	UpdateWindow(hWnd);			//������ Ŭ���̾�Ʈ ������ ĥ�Ѵ�.
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);	//Ű���� �޽�������
		DispatchMessage(&Message);	//�޽����� �ش����ν����� ������
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
		//��弳��
		CreateWindow(TEXT("button"), TEXT("Select Mode"), WS_CHILD | WS_VISIBLE |
			BS_GROUPBOX, 5, 5, 225, 50, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("��庥ó"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 10, 20, 100, 30, hWnd, (HMENU)100, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("�����̹�"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 20, 100, 30, hWnd, (HMENU)101, g_hInst, NULL);
		CheckRadioButton(hWnd, 100, 101, 101);

		//��Ģ����
		selectTeam = CreateWindow(TEXT("button"), TEXT("Rule"), WS_CHILD | WS_VISIBLE |
			BS_GROUPBOX, 5, 75, 225, 130, hWnd, (HMENU)0, g_hInst, NULL);
		personal = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 10, 90, 100, 30, hWnd, (HMENU)110, g_hInst, NULL);
		group = CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 90, 100, 30, hWnd, (HMENU)111, g_hInst, NULL);
		roundCount = CreateWindow(TEXT("button"), TEXT("ȸ����"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 10, 130, 100, 30, hWnd, (HMENU)120, g_hInst, NULL);
		winCount = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 120, 130, 100, 30, hWnd, (HMENU)121, g_hInst, NULL);
		CheckRadioButton(hWnd, 120, 121, 120);
		setCount = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 180, 180, 20, hWnd, (HMENU)122, g_hInst, NULL);
		SetScrollRange(setCount, SB_CTL, 1, 5, TRUE);
		SetScrollPos(setCount, SB_CTL, 3, TRUE);
		
		//�ο�����
		setPlayer = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 5, 230, 220, 20, hWnd, (HMENU)130, g_hInst, NULL);
		SetScrollRange(setPlayer, SB_CTL, 1, 8, TRUE);
		SetScrollPos(setPlayer, SB_CTL, 1, TRUE);

		//���� �� ����
		CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 270, 70, 30, hWnd, (HMENU)140, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("�ɼ�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 270, 70, 30, hWnd, (HMENU)141, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 270, 70, 30, hWnd, (HMENU)142, g_hInst, NULL);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 100:	//��庥ó
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
			MessageBox(hWnd, TEXT("����"), TEXT("Game"), MB_OK);
			break;
		case 141:
			MessageBox(hWnd, TEXT("�ɼ�"), TEXT("Game"), MB_OK);
			break;
		case 142:
			MessageBox(hWnd, TEXT("����"), TEXT("Game"), MB_OK);
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
