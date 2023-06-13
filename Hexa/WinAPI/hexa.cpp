#include <turboc.h>
#include<math.h>
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define PGUP 73
#define PGDN 81
#define ESC 27
#define BX 5
#define BY 1
#define BW 10
#define BH 20
#define max(x,y) ((x>y)?x:y)
#define min(x,y) ((x>y)?y:x)

#define randomize () srand((unsigned)time(NULL))

void DrawScreen();
void DrawBoard();
BOOL ProcessKey();
void PrintBrick(BOOL Show);
int GetAround(int x, int y);
BOOL MoveDown();
void TestFull();
void DrawNext();
void PrintInfo();
void MakeNewBrick();

enum { EMPTY, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, WALL };
char* arTile[] = { ". ","■","●","★","♣","☎","◆","♠","♥","♨","♬","□" };
int board[BW + 2][BH + 2];
int nx, ny;
int brick[3];
int nbrick[3];
int score;
int bricknum;
int level;

int main()
{
	//int nFrame, nStay;
	//int x, y;

	//setcursortype(NOCURSOR);
	//randomize();

	for (; 3;) {
		clrscr();
		for (x = 0; x < BW + 2; x++) {
			for (y = 0; y < BH + 2; y++) {
				board[x][y] = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;
			}
		}
		DrawScreen();
		nFrame = 20;
		score = 0;
		bricknum = 0;

		MakeNewBrick();
		for (; 1;) {
			bricknum++;
			memcpy(brick, nbrick, sizeof(brick));
			MakeNewBrick();
			DrawNext();
			nx = BW / 2;
			ny = 3;
			PrintBrick(TRUE);

			if (GetAround(nx, ny) != EMPTY) break;
			nStay = nFrame;
			for (; 2;) {
				if (--nStay == 0) {
					nStay = nFrame;
					if (MoveDown()) break;
				}
				if (ProcessKey()) break;
				delay(500 / 20);
			}
			if (bricknum % 10 == 0 && nFrame > 5) {
				nFrame--;
			}
		}
		clrscr();
		gotoxy(30, 12); puts("G A M E  O V E R");
		gotoxy(25, 14); puts("다시 시작하려면 Y를 누르세요");
		if (tolower(getch()) != 'y') break;
	}
	setcursortype(NORMALCURSOR);
}