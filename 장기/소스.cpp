#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("���");

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
int turn = 1;	//1 : �ʳ���, 2 : �ѳ���
char tower[10][10] = {	//�ǿ� ���� ���� (�ҹ��� : �ѳ���, �빮�� : �ʳ���)
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
char towerTmp[10][10];
char targetPoint[10][10]; //���� �����̰� �ϱ����� ���� ����
static char setTower;	//�� ����
int tx, ty;	//�� �̵�
static int preX, preY;	//�� �����
int x, y;

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

void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);
void DrawBitmap(HDC hdc);

void DrawBoard();	//���� �׸���
void RemovePoint();	//���� ��� �����
void DrawInCase(int t);	//�� �����ȿ� �ִ� ������ �����̰� �Ϸ���
void DrawKnight();	//��, �� �̵�
void DrawVihicle();	//�� �̵�
void DrawSniper();	//�� �̵�
char DrawPoint(int x, int y);
void MoveTower();	//���� ���õǾ��� ��

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
			hbrick[i]=LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1+i-1)); // ���ҽ� ID���� ���������� ���ĵȰ��
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
			switch (tower[ty][tx])
			{
				case 'K':
					MoveTower();
					DrawInCase(1);
					break;
				case 'C':
					MoveTower();
					DrawVihicle();
					break;
				case 'P':
					MoveTower();
					DrawSniper();
					break;
				case 'H':
					MoveTower();
					DrawKnight();
					break;
				case 'E':
					MoveTower();
					DrawKnight();
					break;
				case 'V':
					MoveTower();
					DrawInCase(1);
					break;
				case 'S':
					MoveTower();
					if (ty > 0)	//��
						targetPoint[ty - 1][tx] = DrawPoint(tx, ty - 1);
					if(tx > 0)	//��
						targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
					if(tx < 9)	//��
						targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
					break;
			}
		}
		else if (turn == 2)
		{
			switch(tower[ty][tx])
			{
				case 'k':
					MoveTower();
					DrawInCase(2);
					break;
				case 'c':
					MoveTower();
					DrawVihicle();
					break;
				case 'p':
					MoveTower();
					DrawSniper();
					break;
				case 'h':
					MoveTower();
					DrawKnight();
					break;
				case 'e':
					MoveTower();
					DrawKnight();
					break;
				case 'v':
					MoveTower();
					DrawInCase(2);
					break;
				case 's':
					MoveTower();
					if (ty > 0)
						targetPoint[ty + 1][tx] = DrawPoint(tx, ty + 1);
					if (tx > 0)
						targetPoint[ty][tx - 1] = DrawPoint(tx - 1, ty);
					if (tx < 9)
						targetPoint[ty][tx + 1] = DrawPoint(tx + 1, ty);
					break;
			}
		}
		if (targetPoint[ty][tx] == '*')
		{
			bool ChoWin = tower[ty][tx] == 'k' && turn == 1;
			bool HanWin = tower[ty][tx] == 'K' && turn == 2;
			tower[ty][tx] = setTower;
			tower[preY][preX] = ' ';
			RemovePoint();
			//���� ������ �¸�����
			if (ChoWin || HanWin)
			{
				if(ChoWin)
					wsprintf(str, TEXT("�ʳ��� �¸�!!!"));
				else if(HanWin)
					wsprintf(str, TEXT("�ѳ��� �¸�!!!"));

				InvalidateRect(hWnd, NULL, true);
				MessageBox(hWnd, str, TEXT("���� ��"), MB_OK);
				PostQuitMessage(0);
			}
			turn = turn == 1 ? 2 : 1;
		}
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_RBUTTONDOWN:
		hdc = GetDC(hWnd);
		RemovePoint();
		setTower = NULL;
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		DrawBoard();

		return 0;
	case WM_DESTROY:
		for (i = 1; i <= 15; i++)
			SelectObject(hMemDC[i], brick[i]);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void MoveTower()
{
	RemovePoint();
	preX = tx; preY = ty;
	setTower = tower[ty][tx];
}

void DrawInCase(int t)
{
	if (t == 1) {
		switch (tx)
		{
		case 3: //����
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
		case 4: //���
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
		case 5: //������
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
		case 3: //����
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
		case 4: //���
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
		case 5: //������
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
	for (j = ty - 1; j >= 0; j--)	//��
	{
		targetPoint[j][tx] = DrawPoint(tx, j);
		if (tower[j][tx] != ' ')
			break;
	}
	for (j = ty + 1; j < 10; j++)	//�Ʒ�
	{
		targetPoint[j][tx] = DrawPoint(tx, j);
		if (tower[j][tx] != ' ')
			break;
	}
	for (j = tx - 1; j >= 0; j--)	//����
	{
		targetPoint[ty][j] = DrawPoint(j, ty);
		if (tower[ty][j] != ' ')
			break;
	}
	for (j = tx + 1; j < 9; j++)	//������
	{
		targetPoint[ty][j] = DrawPoint(j, ty);
		if (tower[ty][j] != ' ')
			break;
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

void DrawSniper()
{
	int enableMove, j;
	//�տ� ���� ������ ���� �ִ���
	for (j = ty - 1; j >= 0; j--)	//��
	{
		if (tower[j][tx] == 'p' || tower[j][tx] == 'P')
			break;
		if (tower[j][tx] != ' ')
		{
			enableMove = j - 1;
			for (j = enableMove; j >= 0; j--)
			{
				targetPoint[j][tx] = DrawPoint(tx, j);
				if (tower[j][tx] != ' ' || tower[j - 1][tx] == 'p' || tower[j - 1][tx] == 'P')
					break;
			}
			break;
		}
	}
	for (j = ty + 1; j < 10; j++)	//�Ʒ�
	{
		if (tower[j][tx] == 'p' || tower[j][tx] == 'P')
			break;
		if (tower[j][tx] != ' ')
		{
			enableMove = j + 1;
			for (j = enableMove; j < 10; j++)
			{
				targetPoint[j][tx] = DrawPoint(tx, j);
				if (tower[j][tx] != ' ' || tower[j + 1][tx] == 'p' || tower[j + 1][tx] == 'P')
					break;
			}
			break;
		}
	}
	for (j = tx - 1; j >= 0; j--)	//����
	{
		if (tower[ty][j] == 'p' || tower[ty][j] == 'P')
			break;
		if (tower[ty][j] != ' ')
		{
			enableMove = j - 1;
			for (j = enableMove; j >= 0; j--)
			{
				targetPoint[ty][j] = DrawPoint(j, ty);
				if (tower[ty][j] != ' ' || tower[ty][j - 1] == 'p' || tower[ty][j - 1] == 'P')
					break;
			}
			break;
		}
	}
	for (j = tx + 1; j < 9; j++)	//������
	{
		if (tower[ty][j] == 'p' || tower[ty][j] == 'P')
			break;
		if (tower[ty][j] != ' ')
		{
			enableMove = j + 1;
			for (j = enableMove; j < 9; j++)
			{
				targetPoint[ty][j] = DrawPoint(j, ty);
				if (tower[ty][j] != ' ' || tower[ty][j + 1] == 'p' || tower[ty][j + 1] == 'P')
					break;
			}
			break;
		}
	}
}

char DrawPoint(int x, int y)
{
	//t�� �ϻ����̸� ���� ������ ������� �ʰ� �Ϸ���
	if (turn == 1)	//�ʳ��� ����
		return (tower[y][x] >= 'A' && tower[y][x] <= 'Z') || (setTower == 'P' && tower[y][x] == 'p') ? NULL : '*';
	else if (turn == 2)	//�ѳ��� ����
		return (tower[y][x] >= 'a' && tower[y][x] <= 'z') || (setTower == 'p' && tower[y][x] == 'P') ? NULL : '*';
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

void DrawBoard()
{
	//�� �׸���
	for (y = 0; y < 9; y++)
		for (x = 0; x < 8; x++)
			Rectangle(hdc, startPoint + (x * digit), startPoint + (y * digit),
				(startPoint + digit) + (x * digit), (startPoint + digit) + (y * digit));

	//�߾Ӽ�
	SetPixel(hdc, startPoint, startPoint, RGB(0, 255, 0));
	MoveToEx(hdc, startPoint + (digit * 3), startPoint, NULL);
	LineTo(hdc, startPoint + (digit * 5), startPoint + (digit * 2));
	MoveToEx(hdc, startPoint + (digit * 5), startPoint, NULL);
	LineTo(hdc, startPoint + (digit * 3), startPoint + (digit * 2));

	MoveToEx(hdc, startPoint + (digit * 3), startPoint + (digit * 7), NULL);
	LineTo(hdc, startPoint + (digit * 5), startPoint + (digit * 9));
	MoveToEx(hdc, startPoint + (digit * 5), startPoint + (digit * 7), NULL);
	LineTo(hdc, startPoint + (digit * 3), startPoint + (digit * 9));

	//�� �׸���
	for (y = 0; y < 10; y++)
		for (x = 0; x < 9; x++)
		{
			switch (tower[y][x])
			{
				//�ѳ���
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
				//�ʳ���
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
}