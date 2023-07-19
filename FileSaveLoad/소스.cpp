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

TCHAR buf[1024] = TEXT(" ");
DWORD dwRead, dwWrite;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HANDLE files[3];
	switch (iMessage) {
	case WM_CREATE:
		buttons[0] = CreateWindow(TEXT("button"), TEXT("����1"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 50, 30, hWnd, (HMENU)0, g_hInst, NULL);
		buttons[1] = CreateWindow(TEXT("button"), TEXT("�ε�1"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 60, 10, 50, 30, hWnd, (HMENU)1, g_hInst, NULL);
		buttons[2] = CreateWindow(TEXT("button"), TEXT("����2"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 40, 50, 30, hWnd, (HMENU)2, g_hInst, NULL);
		buttons[3] = CreateWindow(TEXT("button"), TEXT("�ε�2"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 60, 40, 50, 30, hWnd, (HMENU)3, g_hInst, NULL);
		buttons[4] = CreateWindow(TEXT("button"), TEXT("����3"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 50, 30, hWnd, (HMENU)4, g_hInst, NULL);
		buttons[5] = CreateWindow(TEXT("button"), TEXT("�ε�3"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 60, 70, 50, 30, hWnd, (HMENU)5, g_hInst, NULL);
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
				MessageBox(hWnd, TEXT("������ �ҷ����� ���߽��ϴ�!!"), TEXT("Error"), MB_OK);
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
				MessageBox(hWnd, TEXT("������ �ҷ����� ���߽��ϴ�!!"), TEXT("Error"), MB_OK);
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
				MessageBox(hWnd, TEXT("������ �ҷ����� ���߽��ϴ�!!"), TEXT("Error"), MB_OK);
			break;

		case 7:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				//�Է��� ������ buf�� ����
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