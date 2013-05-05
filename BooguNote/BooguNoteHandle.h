#pragma once
#include "BooguNoteWidget.h"
#include "BooguNoteText.h"

class CBooguNoteHandle : public CBooguNoteWidget
{
public:
	int x;
	int y;
	int width;
	int height;
	int state;
	CBooguNoteText *m_pTextBlock;
private:
	CBooguNoteHandle(){};
public:
	CBooguNoteHandle(int _x, int _y, CBooguNoteText* pText);
	~CBooguNoteHandle(){};

	BOOL HitTest(int _x, int _y)
	{
		if (_x>=x&&_x<=(x+width)&&_y>=y&&_y<=(y+height))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void DoPaint(HDC hdc);

};