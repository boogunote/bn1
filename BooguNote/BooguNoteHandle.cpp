#include "stdafx.h"
#include "BooguNoteHandle.h"
#include "BooguNoteConfig.h"
#include "BooguNoteView.h"

extern CBooguNoteConfig g_config;

CBooguNoteHandle::CBooguNoteHandle(int _x, int _y,  CBooguNoteText* pText)
{
	m_pTextBlock = pText;
	x = _x;
	y = _y;
	height = g_config.nHandleWidth;//-PointsToLogical(g_config.fontSize)*12/13;
	if (height%2 != 1)
		height += 1;
	width = height;
	state = 0;
}

void CBooguNoteHandle::DoPaint(HDC hdc)
{
	CDCHandle dc(hdc);
	CBrush deepBrush;
    deepBrush.CreateSolidBrush(g_config.clrBullet);
	dc.SelectBrush(deepBrush);
	/*CPen lightPen;
	lightPen.CreatePen(PS_SOLID, 1, g_config.clrThemeLight);*/
	CPen deepPen;
	deepPen.CreatePen(PS_SOLID, 1, g_config.clrBullet);
	//CPen darkPen;
	//darkPen.CreatePen(PS_SOLID, 1, g_config.clrThemeDark);
	//CPen whitePen;
	//whitePen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
	
	if (TEXT_HANDLE_CLOSED == state)
	{
		dc.SelectPen(deepPen);
		dc.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));
		//dc.SelectBrush(deepBrush);
		//dc.RoundRect(x, y, x+width, y+height, 3, 3);
		dc.Rectangle(x, y, x+width, y+height);
		//dc.SelectPen(whitePen);
		//dc.SelectPen(darkPen);
		dc.MoveTo(x+2, y+height/2);
		dc.LineTo(x+width-2, y+height/2);
		dc.MoveTo(x+width/2, y+2);
		dc.LineTo(x+width/2, y+height-2);
	}
	else if (TEXT_HANDLE_EXPAND == state)
	{
		dc.SelectPen(deepPen);
		dc.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));
		//dc.SelectBrush(deepBrush);
		//dc.RoundRect(x, y, x+width, y+height, 3, 3);
		dc.Rectangle(x, y, x+width, y+height);
		//dc.SelectPen(whitePen);
		//dc.SelectPen(darkPen);
		dc.MoveTo(x+2, y+height/2);
		dc.LineTo(x+width-2, y+height/2);
	}
	else
	{
		dc.SelectPen(deepPen);
		dc.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));
		//dc.SelectBrush(deepBrush);
		//dc.RoundRect(x, y, x+width, y+height, 3, 3);
		dc.Rectangle(x, y, x+width, y+height);
		//dc.Draw3dRect(x, y, width, height, g_config.clrThemeDeep, g_config.clrThemeDeep);
		//dc.Ellipse(x, y, x+width, y+height);
	}
	CRect rc(x-1, y-1, x+width+1, y+height+1);
	CPoint pt;
	GetCursorPos(&pt);
	//if (m_pTextBlock->m_bHovering)
	m_pTextBlock->m_pParentWindow->ScreenToClient(&pt);
	((CBooguNoteView*)(m_pTextBlock->m_pParentWindow))->ClientToDevice(pt);
	if (HitTest(pt.x, pt.y))
		dc.DrawFocusRect(&rc);
}