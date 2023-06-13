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
//arTile[] = { ". ","■","●","★","♣","☎","◆","♠","♥","♨","♬","□" };
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
	HWND hWnd;		//윈도우 핸들
	MSG Message;		//메시지 구조체
	WNDCLASS WndClass;	//윈도우 클래스 구조체
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;			//윈도우클레스 데이터 영역
	WndClass.cbWndExtra = 0;			//윈도우 데이터 영역
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//바탕색 브러쉬 핸들
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
				wsprintf(text, TEXT("Tetris 잠시 중지. 다시 시작하려면 아무 키나 누르세요."));
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
	wsprintf(text, TEXT("좌우:이동, 위:회전, 아래:내림"));	TextOut(hdc, 50, 5, text, lstrlen(text));
	wsprintf(text, TEXT("공백:전부 내림, ESC:종료"));		TextOut(hdc, 50, 6, text, lstrlen(text));
	wsprintf(text, TEXT("P:정지,PgUp,PgDn:난이도 조절"));	TextOut(hdc, 50, 7, text, lstrlen(text));

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

	// 배열에 기록
	for (i = 0; i < 3; i++) {
		board[nx][ny + i] = brick[i];
	}

	for (;;) {
		// 연속 무늬 점검
		memset(Mark, 0, sizeof(Mark));
		Remove = FALSE;
		for (y = 1; y < BH + 1; y++) {
			for (x = 1; x < BW + 1; x++) {
				t = board[x][y];
				if (t == EMPTY) continue;

				// 수평
				if (board[x - 1][y] == t && board[x + 1][y] == t) {
					for (i = -1; i <= 1; i++) Mark[x + i][y] = TRUE;
					Remove = TRUE;
				}
				// 수직
				if (board[x][y - 1] == t && board[x][y + 1] == t) {
					for (i = -1; i <= 1; i++) Mark[x][y + i] = TRUE;
					Remove = TRUE;
				}
				// 우하향
				if (board[x - 1][y - 1] == t && board[x + 1][y + 1] == t) {
					for (i = -1; i <= 1; i++) Mark[x + i][y + i] = TRUE;
					Remove = TRUE;
				}
				// 좌하향
				if (board[x + 1][y - 1] == t && board[x - 1][y + 1] == t) {
					for (i = -1; i <= 1; i++) Mark[x - i][y + i] = TRUE;
					Remove = TRUE;
				}
			}
		}

		if (Remove == FALSE) return;

		// 제거 애니메이션
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

		// 연속된 무늬 삭제
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
	gotoxy(50, 9); printf("난이도 : %d  ", level);
	gotoxy(50, 10); printf("점수 : %d     ", score);
	gotoxy(50, 11); printf("벽돌 : %d 개  ", bricknum);
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


