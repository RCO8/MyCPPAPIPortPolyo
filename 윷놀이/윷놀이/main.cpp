#include <windows.h>
#include <ctime>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("윷놀이");

HDC hdc;

//option
bool isPlaying = false;
int mouseX, mouseY;

//판
struct step
{
	step* next;
	int x, y;
	int length;
} Step[29];
void SetSteps()
{
	//먼길로 가는 길
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

	//대각선으로 가는 길
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
//말
int Turn = 1;	//현재 차례
int Players = 2;	//최대 인원수
int IDS = 2;	//최대 말 개수
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

	//좌표 설정
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
		{
			PlayerIdx[i].x[j] = 10 + (j * 30);
			PlayerIdx[i].y[j] = 10 + (i * 30);
		}
}
//윷
int yoot[4] = { 0,0,0,0 };	//1은 뒤집어 있는 상태 0은 엎어져있는 상태
int resultStep = 0;	//던질때 나온 숫자

//버튼
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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);		//메뉴이름
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
	HBRUSH MyBrush, OldBrush;
	TCHAR str[20];
	switch (iMessage) {
	case WM_CREATE:
		//판 설정
		SetSteps();
		//버튼 설정
		StartButton.x = 200;
		StartButton.y = 10;
		StartButton.w = 150;
		StartButton.h = 50;
		wsprintf(StartButton.text, TEXT("시작하기"));
		ThrowButton = StartButton;
		wsprintf(ThrowButton.text, TEXT("던지기"));

		//말 색깔 및 좌표 설정
		InitPlayers();

		//랜덤 적용
		srand(time(NULL));
		return 0;
	case WM_COMMAND:	//시작하기 전에 설정
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
		else MessageBox(hWnd, TEXT("플레이중입니다!!!"), TEXT("경고"), MB_OK);
		InvalidateRect(hWnd, NULL, true);
		return 0;
	case WM_LBUTTONDOWN:
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);

		//버튼 클릭시
		if(isPlaying)
		{
			resultStep = 0;
			if (mouseX >= ThrowButton.x && mouseX <= ThrowButton.x + ThrowButton.w)
				if (mouseY >= ThrowButton.y && mouseY <= ThrowButton.y + ThrowButton.h)
				{
					//던지기
					for (int i = 0; i < 4; i++)
					{
						yoot[i] = rand() % 2;
						resultStep += yoot[i];
					}

					InvalidateRect(hWnd, NULL, true);
					switch (resultStep)
					{
						case 1: MessageBox(hWnd, TEXT("도"), TEXT("한칸"), MB_OK); break;
						case 2: MessageBox(hWnd, TEXT("개"), TEXT("두칸"), MB_OK); break;
						case 3: MessageBox(hWnd, TEXT("걸"), TEXT("세칸"), MB_OK); break;
						case 4: MessageBox(hWnd, TEXT("윷"), TEXT("한번더"), MB_OK); break;
						case 0: 
							resultStep = 5;
							MessageBox(hWnd, TEXT("모"), TEXT("한번더"), MB_OK); break;
					}
					
					//말 이동
					//마지막 것이 앞으로 가게 (처음에)
					PlayerIdx[Turn - 1].position[IDS - 1] = Step[resultStep];
					PlayerIdx[Turn - 1].x[IDS - 1] = PlayerIdx[Turn - 1].position[IDS - 1].x;
					PlayerIdx[Turn - 1].y[IDS - 1] = PlayerIdx[Turn - 1].position[IDS - 1].y;

					//만약에 위치가 갈림길로 나와있다면


					//다음 차례
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
					isPlaying = true;	//시작버튼
					//1P말이 시작위치로
					PlayerIdx[Turn - 1].SetPosition(Step[0], IDS - 1);
				}
			InvalidateRect(hWnd, NULL, true);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//말판 그리기
		for (int i = 0; i < 29; i++)
			Ellipse(hdc, Step[i].x, Step[i].y, Step[i].x + Step[i].length, Step[i].y + Step[i].length);

		//말 그리기
		for (int i = 0; i < Players; i++)
		{
			MyBrush = CreateSolidBrush(RGB(PlayerIdx[i].color[0], PlayerIdx[i].color[1], PlayerIdx[i].color[2]));
			OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);
			for (int j = 0; j < IDS; j++)
				Ellipse(hdc, PlayerIdx[i].x[j], PlayerIdx[i].y[j], PlayerIdx[i].x[j] + 30, PlayerIdx[i].y[j] + 30);
			SelectObject(hdc, OldBrush);
			DeleteObject(MyBrush);
		}

		//윷 그리기
		for (int i = 0; i < 4; i++)
		{
			if (yoot[i]) MyBrush = CreateSolidBrush(RGB(255,255,0));
			else MyBrush = CreateSolidBrush(RGB(128,128,0));

			OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);
			Rectangle(hdc, 350 + (i * 50) + 10, 250, 400 + (i * 50), 550);

			SelectObject(hdc, OldBrush);
			DeleteObject(MyBrush);
		}

		//버튼
		if (isPlaying)
		{
			ThrowButton.DrawButton(hdc);
			wsprintf(str, TEXT("%dP 차례"), Turn);
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
