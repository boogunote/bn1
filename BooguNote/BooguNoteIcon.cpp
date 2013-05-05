#include "stdafx.h"

#include "BooguNoteIcon.h"
#include "BooguNoteConfig.h"
#include "BooguNoteText.h"
#include "BooguNoteView.h"


extern CBooguNoteConfig g_config;

CBooguNoteIcon::CBooguNoteIcon(int _x, int _y, CBooguNoteText* pText)
{
	m_pTextBlock = pText;
	x = _x;
	y = _y;
	width = 16;
	height = 16;
	state = TEXT_ICON_PLAINTEXT;
	m_pTextBlock = pText;
}

void CBooguNoteIcon::DoPaint(HDC hdc)
{
	CDCHandle dc(hdc);
	if (TEXT_ICON_PLAINTEXT != state)
	{
		if(g_config.imageList.m_hImageList!=NULL)
		{
			g_config.imageList.Draw(hdc, state, x, y, ILD_TRANSPARENT);
		}
		
	}
}