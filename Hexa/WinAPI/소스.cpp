#include <windows.h>
#include <math.h>
#define BX 5
#define BY 1
#define BW 10
#define BH 20
#define max(x,y) ((x>y)?x:y)
#define min(x,y) ((x>y)?y:x)

#define randomize () srand((unsigned)time(NULL))
#define random(n) (rand() % n)

void DrawScreen();
void DrawBoard();
void PrintBrick(BOOL Show);
int GetAround(int x, int y);
BOOL MoveDown();
void TestFull();
void DrawNext();
void PrintInfo();
void MakeNewBrick();

HBITMAP arTile[12];
//arTile[] = { ". ","��","��","��","��","��","��","��","��","��","��","��" };
enum { EMPTY, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, WALL };
int board[BW + 2][BH + 2];
int nx, ny;
int brick[3];
int nbrick[3];
int score;
int bricknum;
int level;

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
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//������ �귯�� �ڵ�
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	int nFrame, nStay;
	int x, y;
	level = 6;

	TCHAR text[128];

	switch (iMessage) {
	case WM_CREATE:
		level = 6;
		return 0;
	case WM_KEYDOWN:
		int t;
		switch (wParam) {
			case VK_LEFT:
				if (GetAround(nx - 1, ny) == EMPTY) {
				PrintBrick(FALSE);
				nx--;
				PrintBrick(TRUE);
				}
				break;
			case VK_RIGHT:
				if (GetAround(nx + 1, ny) == EMPTY) {
					PrintBrick(FALSE);
					nx++;
					PrintBrick(TRUE);
				}
				break;
			case VK_UP:
				PrintBrick(FALSE);
				t = brick[0];
				brick[0] = brick[1];
				brick[1] = brick[2];
				brick[2] = t;
				PrintBrick(TRUE);
				break;
			case VK_DOWN:
				if (MoveDown()) {
					return TRUE;
				}
				break;
			case VK_PRIOR:
				if (level < 10) {
					level++;
					PrintInfo();
				}
				break;
			case VK_NEXT:
				if (level > 2) {
					level--;
					PrintInfo();
				}
				break;
			case 'P':
				wsprintf(text, TEXT("Tetris ��� ����. �ٽ� �����Ϸ��� �ƹ� Ű�� ��������."));
				TextOut(hdc, 15, 10, text, lstrlen(text));
				DrawScreen();
				PrintBrick(TRUE);
				break;
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawScreen();
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawScreen()
{
	int x, y;
	TCHAR text[128];

	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			/*BitBlt(hdc, BX + x * 2, BY + y, 20, 20, )
			gotoxy(BX + x * 2, BY + y);
			puts(arTile[board[x][y]]);*/
		}
	}

	wsprintf(text, TEXT("Hexa API Edit"));					TextOut(hdc, 50, 3, text, lstrlen(text));
	wsprintf(text, TEXT("�¿�:�̵�, ��:ȸ��, �Ʒ�:����"));	TextOut(hdc, 50, 5, text, lstrlen(text));
	wsprintf(text, TEXT("����:���� ����, ESC:����"));		TextOut(hdc, 50, 6, text, lstrlen(text));
	wsprintf(text, TEXT("P:����,PgUp,PgDn:���̵� ����"));	TextOut(hdc, 50, 7, text, lstrlen(text));

	DrawNext();
	PrintInfo();
}

void DrawBoard()
{
	int x, y;

	for (x = 1; x < BW + 1; x++) {
		for (y = 1; y < BH + 1; y++) {
			gotoxy(BX + x * 2, BY + y);
			puts(arTile[board[x][y]]);
		}
	}
}

void PrintBrick(BOOL Show)
{
	int i;

	for (i = 0; i < 3; i++) {
		gotoxy(BX + nx * 2, BY + ny + i);
		puts(arTile[Show ? brick[i] : EMPTY]);
	}
}

int GetAround(int x, int y)
{
	int i, k = EMPTY;

	for (i = 0; i < 3; i++) {
		k = max(k, board[x][y + i]);
	}
	return k;
}

BOOL MoveDown()
{
	if (GetAround(nx, ny + 1) != EMPTY) {
		TestFull();
		return TRUE;
	}
	PrintBrick(FALSE);
	ny++;
	PrintBrick(TRUE);
	return FALSE;
}

void TestFull()
{
	int i, x, y;
	int t, ty;
	BOOL Remove;
	static int arScoreInc[] = { 0,1,3,7,15,30,100,500 };
	int count = 0;
	BOOL Mark[BW + 2][BH + 2];

	// �迭�� ���
	for (i = 0; i < 3; i++) {
		board[nx][ny + i] = brick[i];
	}

	for (;;) {
		// ���� ���� ����
		memset(Mark, 0, sizeof(Mark));
		Remove = FALSE;
		for (y = 1; y < BH + 1; y++) {
			for (x = 1; x < BW + 1; x++) {
				t = board[x][y];
				if (t == EMPTY) continue;

				// ����
				if (board[x - 1][y] == t && board[x + 1][y] == t) {
					for (i = -1; i <= 1; i++) Mark[x + i][y] = TRUE;
					Remove = TRUE;
				}
				// ����
				if (board[x][y - 1] == t && board[x][y + 1] == t) {
					for (i = -1; i <= 1; i++) Mark[x][y + i] = TRUE;
					Remove = TRUE;
				}
				// ������
				if (board[x - 1][y - 1] == t && board[x + 1][y + 1] == t) {
					for (i = -1; i <= 1; i++) Mark[x + i][y + i] = TRUE;
					Remove = TRUE;
				}
				// ������
				if (board[x + 1][y - 1] == t && board[x - 1][y + 1] == t) {
					for (i = -1; i <= 1; i++) Mark[x - i][y + i] = TRUE;
					Remove = TRUE;
				}
			}
		}

		if (Remove == FALSE) return;

		// ���� �ִϸ��̼�
		for (i = 0; i < 6; i++) {
			for (y = 1; y < BH + 1; y++) {
				for (x = 1; x < BW + 1; x++) {
					if (board[x][y] != EMPTY && Mark[x][y] == TRUE) {
						gotoxy(BX + x * 2, BY + y);
						puts(arTile[i % 2 ? EMPTY : board[x][y]]);
					}
				}
			}
			delay(150);
		}

		// ���ӵ� ���� ����
		for (y = 1; y < BH + 1; y++) {
			for (x = 1; x < BW + 1; x++) {
				if (board[x][y] != EMPTY && Mark[x][y] == TRUE) {
					for (ty = y; ty > 1; ty--) {
						board[x][ty] = board[x][ty - 1];
					}
					board[x][1] = EMPTY;
					count++;
				}
			}
		}

		DrawBoard();
		score += arScoreInc[min(count / 3, 7)];
		PrintInfo();
	}
}

void DrawNext()
{
	int x, y, i;

	for (x = 50; x <= 70; x += 2) {
		for (y = 12; y <= 18; y++) {
			gotoxy(x, y);
			puts(arTile[(x == 50 || x == 70 || y == 12 || y == 18) ? WALL : EMPTY]);
		}
	}

	for (i = 0; i < 3; i++) {
		gotoxy(60, 14 + i);
		puts(arTile[nbrick[i]]);
	}
}

void PrintInfo()
{
	gotoxy(50, 9); printf("���̵� : %d  ", level);
	gotoxy(50, 10); printf("���� : %d     ", score);
	gotoxy(50, 11); printf("���� : %d ��  ", bricknum);
}

void MakeNewBrick()
{
	int i;

	do {
		for (i = 0; i < 3; i++) {
			nbrick[i] = random(level) + 1;
		}
	} while (nbrick[0] == nbrick[1] && nbrick[1] == nbrick[2] && nbrick[0] == nbrick[2]);
}


