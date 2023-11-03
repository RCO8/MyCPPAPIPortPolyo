#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("MBTI 맞추기");

HDC hdc;
TCHAR mbti[16][5] = {
	TEXT("ENFJ"),TEXT("ENFP"),TEXT("ENTJ"),TEXT("ENTP"),
	TEXT("ESFJ"),TEXT("ESFP"),TEXT("ESTJ"),TEXT("ESTP"),
	TEXT("INFJ"),TEXT("INFP"),TEXT("INTJ"),TEXT("INTP"),
	TEXT("ISFJ"),TEXT("ISFP"),TEXT("ISTJ"),TEXT("ISTP")
};
int correct[4];	//mbti정답
int answer[4] = { 0,0,0,0 };	//현재 답
int credit = 10;		//횟수
int score = 100;	//기본점수
int addonScore = 0;	//추가점수
bool good = false;	//정답확인

void Reset();
void Setting();

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
	WndClass.lpszMenuName = NULL;		//메뉴이름
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);		//윈도우 클래스 등록

	hWnd = CreateWindow(lpszClass,		//윈도우 클래스 이름
		lpszClass,		//타이틀바에 출력될문자열
		WS_OVERLAPPEDWINDOW,	//윈도우 스타일
		0,		//윈도우 좌상x좌표
		0,		//윈도우 좌상y좌표
		600,		//윈도우 폭
		400,		//윈도우 높이
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
	HBRUSH SelectedBrush, UnselectedBrush, AnswerBrush, OldBrush;
	TCHAR str[50];
	HFONT hFont, OldFont;

	int tmp = 0;
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

		srand(time(NULL));
		Reset();
		return 0;
	case WM_COMMAND:
		switch (wParam)	//버튼 선택
		{
		case 0:	//E
			answer[0] = answer[0] > 0 ? 0 : 1; break;
		case 1:	//I
			answer[0] = answer[0] < 0 ? 0 : -1; break;
		case 2:	//N
			answer[1] = answer[1] > 0 ? 0 : 1; break;
		case 3:	//S
			answer[1] = answer[1] < 0 ? 0 : -1; break;
		case 4:	//F
			answer[2] = answer[2] > 0 ? 0 : 1; break;
		case 5:	//T
			answer[2] = answer[2] < 0 ? 0 : -1; break;
		case 6:	//J
			answer[3] = answer[3] > 0 ? 0 : 1; break;
		case 7:	//P
			answer[3] = answer[3] < 0 ? 0 : -1; break;
		case 10:
			//정답제출
			if (addonScore == 0) MessageBox(hWnd, TEXT("버튼을 누르고 확인하세요"), TEXT("Error"), MB_OK);
			else
			{
				score--;
				SetTimer(hWnd, 1, 1000, NULL);
				//이후 한번더 누르지 않게 결과 나올때 까지 비활성
			}
			break;
		}
		addonScore = pow(2, abs(answer[0]) + abs(answer[1]) + abs(answer[2]) + abs(answer[3]));
		InvalidateRect(hWnd, NULL, 1);
		return 0;
	case WM_TIMER:
		//1초후 공개 정답이면 MBTI설명 보여주고 정답 확인후 addonScore를 score에 추가
		for (int i = 0; i < 4; i++)
		{
			wsprintf(str, TEXT("맞췄습니다!! \n다시하시겠습니까?"));
			if (answer[i] == 0)
				continue;
			else if (correct[i] != answer[i])
			{
				tmp = 0;
				if (correct[0] < 0) tmp += 8;
				if (correct[1] < 0) tmp += 4;
				if (correct[2] < 0) tmp += 2;
				if (correct[3] < 0) tmp += 1;

				wsprintf(str, TEXT("틀렸습니다!! \n정답은 %s 입니다. \n다시하시겠습니까?"),mbti[0]);
				good = false;
				break;
			}
		}
		KillTimer(hWnd, 1);
		//메세지박스 띄우기 (MBTI설명)
		if (MessageBox(hWnd, str, TEXT("결과"), MB_OKCANCEL) == IDOK)
		{	//계속하기
			if (good)
				score += addonScore;
			Setting();
		}
		else {
			//끝내고 초기화
			//Reset();
		}
		//이후에 버튼 초기화
		InvalidateRect(hWnd, NULL, 1);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		UnselectedBrush = CreateSolidBrush(RGB(255, 255, 255));
		SelectedBrush = CreateSolidBrush(RGB(0, 255, 255));
		AnswerBrush = CreateSolidBrush(RGB(255, 255, 0));
		hFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
			VARIABLE_PITCH | FF_ROMAN, TEXT("돋움"));
		OldFont = (HFONT)SelectObject(hdc, hFont);
		SetTextAlign(hdc, TA_CENTER);

		wsprintf(str, TEXT("나의 MBTI는 ?"));
		TextOut(hdc, 80, 10, str, lstrlen(str));

		//추가점수
		if (answer[0] != 0)	//버튼을 선택되면 배점 증감
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

		//MBIT 정답판
		for (int i = 0; i < 16; i++)	//정답판 그리기
		{
			/*
			i가 0~7까지 E 8부터가 I
			i를 4로 나눈 몫이 짝수이면 N 홀수면 S
			i를 4로 나눈 나머지가 1 이하이면 F 그이상은 T
			i가 홀수이면 J 짝수면 P
			선택된 사각형은 파란색, 정답이 나온 사각형은 노란색
			*/

			if ((answer[0] < 0 && i < 8) || (answer[0] > 0 && i > 7))
				OldBrush = (HBRUSH)SelectObject(hdc, UnselectedBrush);
			else if ((answer[1] < 0 && (i / 4) % 2 == 0) || (answer[1] > 0 && (i / 4) % 2 == 1))
				OldBrush = (HBRUSH)SelectObject(hdc, UnselectedBrush);
			else if ((answer[2] < 0 && (i % 4) < 2) || (answer[2] > 0 && (i % 4) > 1))
				OldBrush = (HBRUSH)SelectObject(hdc, UnselectedBrush);
			else if ((answer[3] < 0 && i % 2 == 0) || (answer[3] > 0 && i % 2 == 1))
				OldBrush = (HBRUSH)SelectObject(hdc, UnselectedBrush);
			else OldBrush = (HBRUSH)SelectObject(hdc, SelectedBrush);

			Rectangle(hdc, 10 + (100 * (i % 4)), 150 + ((i / 4) * 50), 10 + (100 * (i % 4) + 100), 200 + ((i / 4) * 50));
			wsprintf(str, TEXT("%s"), mbti[i]);
			TextOut(hdc, 60 + (100 * (i % 4)), 160 + ((i / 4) * 50), str, lstrlen(str));
		}

		//점수
		SetTextAlign(hdc, TA_LEFT);
		TextOut(hdc, 430, 150, TEXT("점수"), 2);
		wsprintf(str, TEXT("%d"), score);
		TextOut(hdc, 480, 150, str, lstrlen(str));
		//크레딧
		SetTextAlign(hdc, TA_LEFT);
		TextOut(hdc, 430, 180, TEXT("크레딧"), 3);
		wsprintf(str, TEXT("%d"), credit);
		TextOut(hdc, 500, 180, str, lstrlen(str));

		for (int i = 0; i < 4; i++)
		{
			wsprintf(str, TEXT("%d"), correct[i]);
			TextOut(hdc, 430, 220 + (i * 20), str, lstrlen(str));
		}

		SelectObject(hdc, OldFont);
		DeleteObject(AnswerBrush);
		DeleteObject(SelectedBrush);
		DeleteObject(UnselectedBrush);
		DeleteObject(hFont);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void Reset()
{
	for (int i = 0; i < 4; i++)
	{
		answer[i] = 0;
		correct[i] = 0;
	}
	credit = 10;
	score = 100;
	Setting();
}

void Setting()
{
	good = true;

	if (credit == 0) Reset();
	else credit -= 1;

	addonScore = 0;
	for (int i = 0; i < 4; i++)
	{
		answer[i] = 0;
		correct[i] = rand() % 2;
		if (correct[i] == 0) correct[i] = -1;
	}
}
