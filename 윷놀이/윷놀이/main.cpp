#include <windows.h>
#include <ctime>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("������");

HDC hdc;

//option
bool isPlaying = false;
int mouseX, mouseY;

//��
struct step
{
	step* next;
	int x, y;
	int length;
} Step[29];
void SetSteps()
{
	//�ձ�� ���� ��
	Step[0].x = 10;
	Step[0].y = 500;
	Step[0].length = 50;
	Step[0].next = &Step[1];

	Step[1].x = 75;
	Step[1].y = 510;
	Step[1].length = 30;
	Step[2].x = 120;
	Step[2].y = 510;
	Step[2].length = 30;
	Step[3].x = 165;
	Step[3].y = 510;
	Step[3].length = 30;
	Step[4].x = 210;
	Step[4].y = 510;
	Step[4].length = 30;

	Step[5].x = 255;
	Step[5].y = 500;
	Step[5].length = 50;

	Step[6].x = 265;
	Step[6].y = 460;
	Step[6].length = 30;
	Step[7].x = 265;
	Step[7].y = 415;
	Step[7].length = 30;
	Step[8].x = 265;
	Step[8].y = 370;
	Step[8].length = 30;
	Step[9].x = 265;
	Step[9].y = 325;
	Step[9].length = 30;

	Step[10].x = 255;
	Step[10].y = 260;
	Step[10].length = 50;

	Step[11].x = 75;
	Step[11].y = 270;
	Step[11].length = 30;
	Step[12].x = 120;
	Step[12].y = 270;
	Step[12].length = 30;
	Step[13].x = 165;
	Step[13].y = 270;
	Step[13].length = 30;
	Step[14].x = 210;
	Step[14].y = 270;
	Step[14].length = 30;

	Step[15].x = 10;
	Step[15].y = 260;
	Step[15].length = 50;

	Step[16].x = 20;
	Step[16].y = 325;
	Step[16].length = 30;
	Step[17].x = 20;
	Step[17].y = 370;
	Step[17].length = 30;
	Step[18].x = 20;
	Step[18].y = 415;
	Step[18].length = 30;
	Step[19].x = 20;
	Step[19].y = 460;
	Step[19].length = 30;

	//�밢������ ���� ��
	Step[20].x = 225;
	Step[20].y = 470;
	Step[20].length = 30;
	Step[21].x = 190;
	Step[21].y = 440;
	Step[21].length = 30;

	Step[22].x = 225;
	Step[22].y = 310;
	Step[22].length = 30;
	Step[23].x = 190;
	Step[23].y = 340;
	Step[23].length = 30;

	Step[24].x = 60;
	Step[24].y = 310;
	Step[24].length = 30;
	Step[25].x = 100;
	Step[25].y = 340;
	Step[25].length = 30;

	Step[26].x = 135;
	Step[26].y = 380;
	Step[26].length = 50;

	Step[27].x = 100;
	Step[27].y = 440;
	Step[27].length = 30;
	Step[28].x = 60;
	Step[28].y = 470;
	Step[28].length = 30;
}
//��
int Turn = 1;	//���� ����
int Players = 2;	//�ִ� �ο���
int IDS = 2;	//�ִ� �� ����
int playerColors[4][3] = { {255,0,0,},{0,80,255}, {30,30,30},{255,240,20} };
struct player
{
	int color[3];
	step position[4];
	int together = 1;
	int x[4], y[4];
	int nowStep = 0;
	void SetPosition(step getStep, int idx)
	{
		position[idx] = getStep;
		x[idx] = position[idx].x;
		y[idx] = position[idx].y;
	}
}PlayerIdx[4];
void InitPlayers()
{
	PlayerIdx[0].color[0] = 255;
	PlayerIdx[0].color[1] = 0;
	PlayerIdx[0].color[2] = 0;

	PlayerIdx[1].color[0] = 0;
	PlayerIdx[1].color[1] = 80;
	PlayerIdx[1].color[2] = 255;

	PlayerIdx[2].color[0] = 30;
	PlayerIdx[2].color[1] = 30;
	PlayerIdx[2].color[2] = 30;

	PlayerIdx[3].color[0] = 255;
	PlayerIdx[3].color[1] = 240;
	PlayerIdx[3].color[2] = 20;

	//��ǥ ����
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
		{
			PlayerIdx[i].x[j] = 10 + (j * 30);
			PlayerIdx[i].y[j] = 10 + (i * 30);
		}
}
//��
int yoot[4] = { 0,0,0,0 };	//1�� ������ �ִ� ���� 0�� �������ִ� ����
int resultStep = 0;	//������ ���� ����

//��ư
struct button
{
	int x, y;
	int w, h;
	TCHAR text[100];
	void DrawButton(HDC hdc)
	{
		Rectangle(hdc, x, y, x + w, y + h);
		SetTextAlign(hdc, TA_CENTER);
		TextOut(hdc, x + (w / 2), y + 15, text, lstrlen(text));
	}
} StartButton, ThrowButton;

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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);		//�޴��̸�
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HBRUSH MyBrush, OldBrush;
	TCHAR str[20];
	switch (iMessage) {
	case WM_CREATE:
		//�� ����
		SetSteps();
		//��ư ����
		StartButton.x = 200;
		StartButton.y = 10;
		StartButton.w = 150;
		StartButton.h = 50;
		wsprintf(StartButton.text, TEXT("�����ϱ�"));
		ThrowButton = StartButton;
		wsprintf(ThrowButton.text, TEXT("������"));

		//�� ���� �� ��ǥ ����
		InitPlayers();

		//���� ����
		srand(time(NULL));
		return 0;
	case WM_COMMAND:	//�����ϱ� ���� ����
		if (!isPlaying)
			switch (wParam)
			{
				case ID_PLAYERS2: Players = 2; break;
				case ID_PLAYERS3: Players = 3; break;
				case ID_PLAYERS4: Players = 4; break;

				case ID_ROLE1: IDS = 1; break;
				case ID_ROLE2: IDS = 2; break;
				case ID_ROLE3: IDS = 3; break;
				case ID_ROLE4: IDS = 4; break;
			}
		else MessageBox(hWnd, TEXT("�÷������Դϴ�!!!"), TEXT("���"), MB_OK);
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_LBUTTONDOWN:
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);

		//��ư Ŭ����
		if(isPlaying)
		{
			resultStep = 0;
			if (mouseX >= ThrowButton.x && mouseX <= ThrowButton.x + ThrowButton.w)
				if (mouseY >= ThrowButton.y && mouseY <= ThrowButton.y + ThrowButton.h)
				{
					//������
					for (int i = 0; i < 4; i++)
					{
						yoot[i] = rand() % 2;
						resultStep += yoot[i];
					}

					InvalidateRect(hWnd, NULL, true);
					switch (resultStep)
					{
						case 1: MessageBox(hWnd, TEXT("��"), TEXT("��ĭ"), MB_OK); break;
						case 2: MessageBox(hWnd, TEXT("��"), TEXT("��ĭ"), MB_OK); break;
						case 3: MessageBox(hWnd, TEXT("��"), TEXT("��ĭ"), MB_OK); break;
						case 4: MessageBox(hWnd, TEXT("��"), TEXT("�ѹ���"), MB_OK); break;
						case 0: 
							resultStep = 5;
							MessageBox(hWnd, TEXT("��"), TEXT("�ѹ���"), MB_OK); break;
					}
					
					//�� �̵�
					//������ ���� ������ ���� (ó����)
					PlayerIdx[Turn - 1].position[IDS - 1] = Step[resultStep];
					PlayerIdx[Turn - 1].x[IDS - 1] = PlayerIdx[Turn - 1].position[IDS - 1].x;
					PlayerIdx[Turn - 1].y[IDS - 1] = PlayerIdx[Turn - 1].position[IDS - 1].y;

					//���࿡ ��ġ�� ������� �����ִٸ�


					//���� ����
					if (resultStep > 0 && resultStep < 4)
					{
						Turn++;
						if (Turn > Players) Turn = 1;
					}
				}
			InvalidateRect(hWnd, NULL, true);
		}
		else
		{
			if (mouseX >= StartButton.x && mouseX <= StartButton.x + StartButton.w)
				if (mouseY >= StartButton.y && mouseY <= StartButton.y + StartButton.h)
				{
					isPlaying = true;	//���۹�ư
					//1P���� ������ġ��
					PlayerIdx[Turn - 1].SetPosition(Step[0], IDS - 1);
				}
			InvalidateRect(hWnd, NULL, true);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//���� �׸���
		for (int i = 0; i < 29; i++)
			Ellipse(hdc, Step[i].x, Step[i].y, Step[i].x + Step[i].length, Step[i].y + Step[i].length);

		//�� �׸���
		for (int i = 0; i < Players; i++)
		{
			MyBrush = CreateSolidBrush(RGB(PlayerIdx[i].color[0], PlayerIdx[i].color[1], PlayerIdx[i].color[2]));
			OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);
			for (int j = 0; j < IDS; j++)
				Ellipse(hdc, PlayerIdx[i].x[j], PlayerIdx[i].y[j], PlayerIdx[i].x[j] + 30, PlayerIdx[i].y[j] + 30);
			SelectObject(hdc, OldBrush);
			DeleteObject(MyBrush);
		}

		//�� �׸���
		for (int i = 0; i < 4; i++)
		{
			if (yoot[i]) MyBrush = CreateSolidBrush(RGB(255,255,0));
			else MyBrush = CreateSolidBrush(RGB(128,128,0));

			OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);
			Rectangle(hdc, 350 + (i * 50) + 10, 250, 400 + (i * 50), 550);

			SelectObject(hdc, OldBrush);
			DeleteObject(MyBrush);
		}

		//��ư
		if (isPlaying)
		{
			ThrowButton.DrawButton(hdc);
			wsprintf(str, TEXT("%dP ����"), Turn);
			TextOut(hdc, ThrowButton.x, ThrowButton.y + ThrowButton.h, str, lstrlen(str));
		}
		else StartButton.DrawButton(hdc);

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
