#pragma once

#define PLAY1_CONTROLL 101	//�÷��̾�1 ����
#define PLAY2_CONTROLL 102	//�÷��̾�2 ����

const int maxStage = 50;
const int tileGrid = 16;
int nowLevel = 0;
int stage = 1;
char stageTiles[10][13][17] = {
	//stage 1
	{
		"................",
		"................",
		"................",
		"....xxxxxxxx....",
		"..xxxxxxxxxxxx..",
		"................",
		"xxxxxxx..xxxxxxx",
		"................",
		"...xxxxxxxxxx...",
		"................",
		"xxxx..xxxx..xxxx",
		"................",
		"xxxxxxxxxxxxxxxx"
	},
	{
		"................",
		"................",
		"...xxxxxxx..xx..",
		"............xx..",
		"..xx..xxxxxxxx..",
		"..xx............",
		"..xxxxxxxx..xx..",
		"............xx..",
		"..xx..xxxxxxxx..",
		"..xx............",
		"..xxxxxxxxxxx...",
		"................",
		"xxxxxxxxxxxxxxxx"
	},
	{
		"................",
		"................",
		"...xxxxxxxxxx...",
		"................",
		"xx..xxxxxxxx..xx",
		"................",
		"x..xx..xx..xx..x",
		"...xx......xx...",
		"x..xx..xx..xx..x",
		"................",
		"xxx..xxxxxx..xxx",
		"................",
		"xxxxxxxxxxxxxxxx"
	}
};
int moveFloor = 0;	//���� ���������� �̵��� ��

int x = 0, y = 0;

/*
��Ʈ���� �ʿ���� ���κ��� �����ټ� �ִ� �Լ�.
TransparentBle�Լ��� ���Ͽ� �ӵ��� ������ �پ.
*/
//* TransBlt �Լ�
// ���� ��Ʈ���� ����ϴ� �Լ�. (x,y)�� ��� ��ġ. clrMask�� ������� �ش�.
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask)
{
	BITMAP bm;
	COLORREF cColor;
	HBITMAP bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC		hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT	ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hbitmap);
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;
	DPtoLP(hdcTemp, &ptSize, 1);

	hdcBack = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem = CreateCompatibleDC(hdc);
	hdcSave = CreateCompatibleDC(hdc);

	bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndMem = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	bmBackOld = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld = (HBITMAP)SelectObject(hdcSave, bmSave);

	SetMapMode(hdcTemp, GetMapMode(hdc));

	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	cColor = SetBkColor(hdcTemp, clrMask);

	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	SetBkColor(hdcTemp, cColor);

	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, x, y, SRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);
	BitBlt(hdc, x, y, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}

//��Ʈ���� ��Ī��Ű��
void MirrorBlt(HDC hdc, int x, int y, HBITMAP hbitmap, int mirrorX, int mirrorY)
{
	BITMAP bit;

	GetObject(hbitmap, sizeof(BITMAP), &bit);
}