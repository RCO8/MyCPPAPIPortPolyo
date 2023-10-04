#include <windows.h>
#include <cmath>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("MBTI ���߱�");

HDC hdc;
TCHAR mbti[16][5] = { 
	TEXT("ENFJ"),TEXT("ENFP"),TEXT("ENTJ"),TEXT("ENTP"),
	TEXT("ESFJ"),TEXT("ESFP"),TEXT("ESTJ"),TEXT("ESTP"),
	TEXT("INFJ"),TEXT("INFP"),TEXT("INTJ"),TEXT("INTP"),
	TEXT("ISFJ"),TEXT("ISFP"),TEXT("ISTJ"),TEXT("ISTP")
};
bool correct[16];	//mbti����
int answer[4] = { 0,0,0,0 };	//���� ��
int count = 10;		//Ƚ��
int score = 100;	//�⺻����
int addonScore = 0;	//�߰�����

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
		600,		//������ ��
		400,		//������ ����
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	TCHAR str[30];
	HFONT hFont, OldFont;
	switch (iMessage) {
	case WM_CREATE:
		CreateWindow(TEXT("button"), TEXT("E"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 50, 100, 40, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("I"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 90, 100, 40, hWnd, (HMENU)1, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("N"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 110, 50, 100, 40, hWnd, (HMENU)2, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("S"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 110, 90, 100, 40, hWnd, (HMENU)3, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("F"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 210, 50, 100, 40, hWnd, (HMENU)4, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("T"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 210, 90, 100, 40, hWnd, (HMENU)5, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("J"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 310, 50, 100, 40, hWnd, (HMENU)6, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("P"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 310, 90, 100, 40, hWnd, (HMENU)7, g_hInst, NULL);

		CreateWindow(TEXT("button"), TEXT("OK"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 430, 50, 70, 80, hWnd, (HMENU)10, g_hInst, NULL);
		return 0;
	case WM_COMMAND:
		switch (wParam)
		{
		case 0:	//E
			answer[0] = answer[0] > 0 ? 0 : 1;
			break;
		case 1:	//I
			answer[0] = answer[0] < 0 ? 0 : -1;
			break;
		case 2:	//N
			answer[1] = answer[1] > 0 ? 0 : 1;
			break;
		case 3:	//S
			answer[1] = answer[1] < 0 ? 0 : -1;
			break;
		case 4:	//F
			answer[2] = answer[2] > 0 ? 0 : 1;
			break;
		case 5:	//T
			answer[2] = answer[2] < 0 ? 0 : -1;
			break;
		case 6:	//J
			answer[3] = answer[3] > 0 ? 0 : 1;
			break;
		case 7:
			answer[3] = answer[3] < 0 ? 0 : -1;
			break;
		case 10:
			//��������
			if (addonScore == 0) MessageBox(hWnd, TEXT("��ư�� ������ Ȯ���ϼ���"), TEXT("Error"), MB_OK);
			break;
		}
		addonScore = abs(answer[0]) + abs(answer[1]) + abs(answer[2]) + abs(answer[3]);
		InvalidateRect(hWnd, NULL, 1);
		return 0;
	case WM_TIMER:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
			VARIABLE_PITCH | FF_ROMAN, TEXT("����"));
		OldFont = (HFONT)SelectObject(hdc, hFont);
		SetTextAlign(hdc, TA_CENTER);

		wsprintf(str, TEXT("���� MBTI�� ?"));
		TextOut(hdc, 80, 10, str, lstrlen(str));

		if (answer[0] != 0)
		{
			wsprintf(str, TEXT("x2"));

			if (answer[1] != 0)
			{
				wsprintf(str, TEXT("x4"));
				if (answer[2] != 0)
				{
					wsprintf(str, TEXT("x8"));
					if (answer[3] != 0) wsprintf(str, TEXT("x16"));
				}
				else if (answer[3] != 0) wsprintf(str, TEXT("x8"));
			}
			else if (answer[2] != 0)
			{
				wsprintf(str, TEXT("x4"));
				if (answer[3] != 0) wsprintf(str, TEXT("x8"));
			}
			else if (answer[3] != 0) wsprintf(str, TEXT("x4"));
		}
		else if (answer[1] != 0)
		{
			wsprintf(str, TEXT("x2"));
			if (answer[2] != 0)
			{
				wsprintf(str, TEXT("x4"));
				if (answer[3] != 0) wsprintf(str, TEXT("x8"));
			}
			else if (answer[3] != 0) wsprintf(str, TEXT("x4"));
		}
		else if (answer[2] != 0)
		{
			wsprintf(str, TEXT("x2"));
			if (answer[3] != 0) wsprintf(str, TEXT("x4"));
		}
		else if (answer[3] != 0) wsprintf(str, TEXT("x2"));
		else wsprintf(str, TEXT("x0"));
		TextOut(hdc, 530, 50, str, lstrlen(str));

		for (int i = 0; i < 16; i++)
		{
			/*
			i�� 0~7���� E 8���Ͱ� I

			i�� Ȧ���̸� J ¦���� P
			���õ� �簢���� �Ķ���, ������ ���� �簢���� �����
			*/
			Rectangle(hdc, 10 + (100 * (i % 4)), 150 + ((i / 4) * 50), 10 + (100 * (i % 4) + 100), 200 + ((i / 4) * 50));
			wsprintf(str, TEXT("%s"), mbti[i]);
			TextOut(hdc, 60 + (100 * (i % 4)), 160 + ((i / 4) * 50), str, lstrlen(str));
		}
		SelectObject(hdc, OldFont);
		DeleteObject(hFont);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
