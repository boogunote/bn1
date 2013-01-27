#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "TextBlockPage.h"
#include "BooguNoteConfig.h"

extern CBooguNoteConfig g_config;


CTextBlockPage::CTextBlockPage() : m_bDirty(false)
{
    m_psp.dwFlags |= PSP_USEICONID;
    //m_psp.pszIcon = MAKEINTRESOURCE(IDI_TABICON);
    m_psp.hInstance = _Module.GetResourceInstance();

	bAutoWidth = g_config.bAutoWidth;
	bShowCharCountInShrinkTB = g_config.bShowCharCountInShrinkTB;
	narrowWidth = g_config.narrowWidth;
	wideWidth = g_config.wideWidth;
	nHandleWidth = g_config.nHandleWidth;
	nHeadLength = g_config.nHeadLength;
}

//////////////////////////////////////////////////////////////////////
// Message handlers
BOOL CTextBlockPage::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	DoDataExchange();
	ShowScrollBarItem();

	return TRUE;
}

LRESULT CTextBlockPage::OnBnClickedCheckAutoNarrowWidth(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (bAutoWidth)
	{
		bAutoWidth = false;
	}
	else
	{
		bAutoWidth = true;
	}
	ShowScrollBarItem();
	return 0;
}


void CTextBlockPage::ShowScrollBarItem()
{
	if (bAutoWidth)
	{
		((CEdit)GetDlgItem(IDC_EDIT_NARROW_WIDTH)).EnableWindow(FALSE);
	}
	else
	{
		((CEdit)GetDlgItem(IDC_EDIT_NARROW_WIDTH)).EnableWindow(TRUE);
	}
}

LRESULT CTextBlockPage::OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
	if (lppsn->hdr.code == PSN_APPLY)
	{
		if (!m_bDirty && ((GetDlgItemInt(IDC_EDIT_NARROW_WIDTH) != g_config.narrowWidth) ||
			(GetDlgItemInt(IDC_EDIT_WIDE_WIDTH) != g_config.wideWidth) ||
			(GetDlgItemInt(IDC_EDIT_SQUARE_BULLET_SIZE) != g_config.nHandleWidth)) ||
			(GetDlgItemInt(IDC_EDIT_SHRINK_CHARS) != g_config.nHeadLength))
		{
			m_bDirty = true;
		}
		bool bRefreshed = false;
		if ((GetDlgItemInt(IDC_EDIT_NARROW_WIDTH) != g_config.narrowWidth) ||
			(GetDlgItemInt(IDC_EDIT_WIDE_WIDTH) != g_config.wideWidth) ||
			(GetDlgItemInt(IDC_EDIT_SQUARE_BULLET_SIZE) != g_config.nHandleWidth)||
			(GetDlgItemInt(IDC_EDIT_SHRINK_CHARS) != g_config.nHeadLength))
		{
			GetParent().GetParent().PostMessage(WM_USER_MAINFRM_REFRESH);
			bRefreshed = true;
		}
		DoDataExchange(TRUE);
		if (((bAutoWidth != g_config.bAutoWidth) || (bShowCharCountInShrinkTB != g_config.bShowCharCountInShrinkTB)) && !bRefreshed)
		{
			GetParent().GetParent().PostMessage(WM_USER_MAINFRM_REFRESH);
		}

		g_config.bAutoWidth = bAutoWidth;
		g_config.bShowCharCountInShrinkTB = bShowCharCountInShrinkTB;
		g_config.narrowWidth = narrowWidth;
		g_config.wideWidth = wideWidth;
		g_config.nHandleWidth = nHandleWidth;
		g_config.nHeadLength = nHeadLength;
	}
	bHandled = FALSE;
	return 0;
}