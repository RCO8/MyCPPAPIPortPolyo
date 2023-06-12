#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Puyo Puyo");

HWND hWnd;		//������ �ڵ�
HDC hdc;
HDC hMemDC[6];
HBITMAP hbrick[6];
HBITMAP brick[6];
int i;

#define Random(x) (rand() % x)+1;

struct Block {
	int x, y;
	int color;	//1~5������ ����
} nowBlock, nowBlock2, nextBlock, nextBlock2;
int downSpeed = 500;
int isBlock[24][11];
int score;
bool playable = true;	//����� �� ����� �ڵ������� ������ �� �������̵���

void DrawMap();
void Turn();
void Moving(int direction);
void MakeNewBlock();
void PopBlocks(Block lined);
void FallBlocks();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
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
		CW_USEDEFAULT,		//������ �»�x��ǥ
		CW_USEDEFAULT,		//������ �»�y��ǥ
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

	switch (iMessage) {
	case WM_CREATE:
		hdc = GetDC(hWnd);

		for (i = 1; i <= 5; i++)
		{
			hMemDC[i] = CreateCompatibleDC(hdc);
			// hbrick[i]=LoadBitmap(g_hInst, MAKEINTRESOURCE(first_BITMAP+i-1)); ���ҽ� ID���� ���������� ���ĵȰ��
		}
		// or
		hbrick[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		hbrick[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		hbrick[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
		hbrick[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
		hbrick[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));

		for (i = 1; i <= 5; i++)
			brick[i] = (HBITMAP)SelectObject(hMemDC[i], hbrick[i]);

		SetTimer(hWnd, 1, downSpeed, NULL);	//��Ʈ��Ÿ�̸�

		for (int y = 0; y < 24; y++)
			for (int x = 0; x < 11; x++)
				isBlock[y][x] = 0;

		score = 0;

		srand(time(NULL));
		nowBlock.x = 5;
		nowBlock.y = 2;
		nowBlock.color = Random(5);
		nowBlock2.x = nowBlock.x;
		nowBlock2.y = nowBlock.y - 1;
		nowBlock2.color = Random(5);

		nextBlock.x = 14;
		nextBlock.y = 3;
		nextBlock.color = Random(5);
		nextBlock2.x = nextBlock.x;
		nextBlock2.y = nextBlock.y - 1;
		nextBlock2.color = Random(5);
		
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_KEYDOWN:
		if (playable)
		{
			switch (wParam)
			{
				case VK_UP:
					Turn();
					break;
				case VK_DOWN:
					downSpeed = 100;
					break;
				case VK_LEFT:
					Moving(-1);
					break;
				case VK_RIGHT:
					Moving(1);
					break;
			}
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_DOWN:
			downSpeed = 500;
			break;
		}
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			SetTimer(hWnd, 1, downSpeed, NULL);
			playable = true;
			if ((nowBlock.y == 23 || nowBlock2.y == 23) ||
				(isBlock[nowBlock.y + 1][nowBlock.x] > 0 || isBlock[nowBlock2.y + 1][nowBlock2.x] > 0))	//�ٴ��̳� ������ ��Ͽ� �������
			{
				isBlock[nowBlock.y][nowBlock.x] = nowBlock.color;
				isBlock[nowBlock2.y][nowBlock2.x] = nowBlock2.color;

				//�Ʒ� ������� �ִٸ�
				bool blockEmpty = isBlock[nowBlock.y + 1][nowBlock.x] < 1 && nowBlock.y < 23;
				bool blockEmpty2 = isBlock[nowBlock2.y + 1][nowBlock2.x] < 1 && nowBlock2.y < 23;
				if (blockEmpty || blockEmpty2)
				{
					KillTimer(hWnd, 1);
					SetTimer(hWnd, 2, downSpeed, NULL);
					break;
				}

				//�Ȱ������� ����� 4�� �̻� ����Ǹ�
				PopBlocks(nowBlock);
				PopBlocks(nowBlock2);

				KillTimer(hWnd, 1);
				SetTimer(hWnd, 3, downSpeed, NULL);
			}
			nowBlock.y++;
			nowBlock2.y++;
			break;
		case 2:	//����� ��� ������ ��
			SetTimer(hWnd, 2, downSpeed, NULL);
			DrawMap();
			playable = false;
			if (isBlock[nowBlock.y + 1][nowBlock.x] < 1 && nowBlock.y < 23)
			{
				isBlock[nowBlock.y][nowBlock.x] = 0;
				isBlock[nowBlock.y + 1][nowBlock.x] = nowBlock.color;
				nowBlock.y++;
			}
			else if (isBlock[nowBlock2.y + 1][nowBlock2.x] < 1 && nowBlock2.y < 23)
			{
				isBlock[nowBlock2.y][nowBlock2.x] = 0;
				isBlock[nowBlock2.y + 1][nowBlock2.x] = nowBlock2.color;
				nowBlock2.y++;
			}
			else
			{
				//�Ȱ������� ����� 4�� �̻� ����Ǹ�
				PopBlocks(nowBlock);
				PopBlocks(nowBlock2);

				KillTimer(hWnd, 2);
				SetTimer(hWnd, 3, downSpeed, NULL);
			}
			break;
		case 3:	//�����ϰ� �� ��������
			SetTimer(hWnd, 3, downSpeed, NULL);
			playable = false;
			for (int x = 0; x < 11; x++)
				for (int y = 22; y >= 0; y--)
				{
					if (isBlock[y + 1][x] < 1 && y+1 < 23)
					{
						isBlock[y + 1][x] = isBlock[y][x];
						isBlock[y][x] = 0;
					}
				}

			//���� ���� �����
			MakeNewBlock();

			KillTimer(hWnd, 3);
			SetTimer(hWnd, 1, downSpeed, NULL);
			break;
		}
	
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawMap();

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		for (i = 1; i <= 5; i++)
			SelectObject(hMemDC[i], brick[i]);
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
void DrawMap()
{
	hdc = GetDC(hWnd);
	// 11 x 24 ���ȭ��
	Rectangle(hdc, 19, 19, 12 * 20 + 1, 25 * 20 + 1);
	//������ ǥ��
	Rectangle(hdc, 260, 19, 340, 100);
	SetTextAlign(hdc, TA_CENTER);
	TextOut(hdc, 300, 18, TEXT("NEXT"), 4);

	TCHAR n[2];
	//���� �� ǥ��
	for (int y = 0; y < 24; y++)
		for (int x = 0; x < 11; x++)
		{
			if(isBlock[y][x]> 0)
			BitBlt(hdc, (x + 1) * 20, (y + 1) * 20, 20, 20, hMemDC[isBlock[y][x]], 0, 0, SRCCOPY);
			wsprintf(n, TEXT("%d"), isBlock[y][x]);
			TextOut(hdc, (15 * x) + 500, (15 * y) + 20, n, lstrlen(n));
		}

	if (playable)
	{
		BitBlt(hdc, (nowBlock.x + 1) * 20, nowBlock.y * 20, 300, 375, hMemDC[nowBlock.color], 0, 0, SRCCOPY);
		BitBlt(hdc, (nowBlock2.x + 1) * 20, nowBlock2.y * 20, 300, 375, hMemDC[nowBlock2.color], 0, 0, SRCCOPY);
	}
	BitBlt(hdc, nextBlock.x * 20, nextBlock.y * 20, 300, 375, hMemDC[nextBlock.color], 0, 0, SRCCOPY);
	BitBlt(hdc, nextBlock2.x * 20, nextBlock2.y * 20, 300, 375, hMemDC[nextBlock2.color], 0, 0, SRCCOPY);

	//���� ǥ��

}

void MakeNewBlock()
{
	nowBlock.x = 5;
	nowBlock.y = 2;
	nowBlock.color = nextBlock.color;
	nowBlock2.x = nowBlock.x;
	nowBlock2.y = nowBlock.y - 1;
	nowBlock2.color = nextBlock2.color;

	nextBlock.color = Random(5);
	nextBlock2.color = Random(5);
}

void Turn()
{
	if (nowBlock.y - nowBlock2.y > 0)	//�߽ɺ���� ���� ������
	{
		//������ ���� �پ� ������ �߽ɺ��� �������� �з���
		if (nowBlock.x == 10 || isBlock[nowBlock.y][nowBlock.x + 1] > 0)
		{
			nowBlock.x -= 1;
			nowBlock2.x -= 1;
		}
		nowBlock2.x = nowBlock.x + 1;
		nowBlock2.y = nowBlock.y;
	}
	else if (nowBlock.y - nowBlock2.y < 0)	//�߽ɺ�� �Ʒ��� ������
	{
		//���� ���� �پ� ������ �߽ɺ��� ���������� �з���
		if (nowBlock.x == 0 || isBlock[nowBlock.y][nowBlock.x - 1] > 0)
		{
			nowBlock.x += 1;
			nowBlock2.x += 1;
		}
		nowBlock2.x = nowBlock.x - 1;
		nowBlock2.y = nowBlock.y;
	}
	else if (nowBlock.x - nowBlock2.x > 0)	//�߽ɺ���� ���ʿ� ������
	{
		nowBlock2.x = nowBlock.x;
		nowBlock2.y = nowBlock.y - 1;
	}
	else if (nowBlock.x - nowBlock2.x < 0)	//�߽ɺ���� �����ʿ� ������
	{
		nowBlock2.x = nowBlock.x;
		nowBlock2.y = nowBlock.y + 1;
	}
}

void Moving(int direction)
{
	bool blockLimit, blockLimit2;
	bool sameHorizontal;

	bool sideBlock = isBlock[nowBlock.y][nowBlock.x + direction] > 0;
	bool sideBlock2 = isBlock[nowBlock2.y][nowBlock2.x + direction] > 0;

	if (direction < 0)	//���ʹ���
	{
		blockLimit = nowBlock.x < nowBlock2.x && nowBlock.x > 0;
		blockLimit2 = nowBlock.x > nowBlock2.x && nowBlock2.x > 0;
		sameHorizontal = nowBlock.x > 0 && nowBlock2.x > 0;

		if (blockLimit || blockLimit2 || sameHorizontal)	//���� �پ�����
		{
			if (!sideBlock && !sideBlock2)	//���� ���� ���θ��� ������
			{
				nowBlock.x--;
				nowBlock2.x--;
			}
		}
	}
	else if (direction > 0)	//�����ʹ���
	{
		blockLimit = nowBlock.x > nowBlock2.x && nowBlock.x < 10;
		blockLimit2 = nowBlock.x < nowBlock2.x && nowBlock2.x < 10;
		sameHorizontal = nowBlock.x < 10 && nowBlock2.x < 10;

		if (blockLimit || blockLimit2 || sameHorizontal)	//���� �پ�����
		{
			if (!sideBlock && !sideBlock2)	//���� ���� ���θ��� ������
			{
				nowBlock.x++;
				nowBlock2.x++;
			}
		}
	}
}

void PopBlocks(Block lined)	//���� ����� 4�� �̻����� ����Ǹ� ����
{
	int count = 1;
	bool isPop[24][11];
	const int xFix = lined.x, yFix = lined.y;

	for (int y = 0; y < 24; y++)
		for (int x = 0; x < 11; x++)
			isPop[y][x] = false;

	if (isBlock[lined.y][lined.x] > 0)
	{
		isPop[lined.y][lined.x] = true;
		if (isBlock[lined.y][lined.x] == isBlock[lined.y - 1][lined.x])
		{
			isPop[lined.y - 1][lined.x] = true;
			count++;
		}

		//�Ʒ� �˻�
		while (isBlock[lined.y + 1][lined.x] == lined.color)
		{
			isPop[lined.y + 1][lined.x] = true;
			count++;
			lined.y++;

			while (isBlock[lined.y][lined.x - 1] == lined.color)
			{
				isPop[lined.y][lined.x - 1] = true;
				count++;
				lined.x--;
			}
			lined.x = xFix;
			while (isBlock[lined.y][lined.x + 1] == lined.color)
			{
				isPop[lined.y][lined.x + 1] = true;
				count++;
				lined.x++;
			}
			lined.x = xFix;
		}
		//�¿� �˻�
		lined.y = yFix;
		while (isBlock[lined.y][lined.x - 1] == lined.color)
		{
			isPop[lined.y][lined.x - 1] = true;
			count++;
			lined.x--;

			//���� ��ܿ� ���� ����Ǿ� �ִ���
			while (isBlock[lined.y - 1][lined.x] == lined.color)
			{
				isPop[lined.y - 1][lined.x] = true;
				count++;
				lined.y--;
			}
			lined.y = yFix;
			//�Ʒ������� ����� �ִٸ�
			while (isBlock[lined.y + 1][lined.x] == lined.color)
			{
				if(!isPop[lined.y+1][lined.x])
				{
					isPop[lined.y + 1][lined.x] == true;
					count++;
				}
				lined.y++;
			}
			lined.y = yFix;
		}
		lined.x = xFix;
		while (isBlock[lined.y][lined.x + 1] == lined.color)
		{
			isPop[lined.y][lined.x + 1] = true;
			count++;
			lined.x++;
			//������ ��ܿ� ���� ����Ǿ� �ִ���
			while (isBlock[lined.y - 1][lined.x] == lined.color)
			{
				isPop[lined.y - 1][lined.x] = true;
				count++;
				lined.y--;
			}
			lined.y = yFix;
			//�Ʒ������� ����� �ִٸ�
			while (isBlock[lined.y + 1][lined.x] == lined.color)
			{
				if (!isPop[lined.y + 1][lined.x])
				{
					isPop[lined.y + 1][lined.x] == true;
					count++;
				}
				lined.y++;
			}
			lined.y = yFix;
		}

		if (count >= 4)	//Ƚ���� 4�� �̻��̸� ����
		{
			for (int y = 0; y < 24; y++)
				for (int x = 0; x < 11; x++)
					if (isPop[y][x])
						isBlock[y][x] = 0;

		}
		else
			return;
	}
}