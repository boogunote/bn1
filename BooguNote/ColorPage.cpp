#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "ColorPage.h"
#include "BooguNoteConfig.h"

extern CBooguNoteConfig g_config;

//////////////////////////////////////////////////////////////////////
// Construction

CColorPage::CColorPage()
{
    m_psp.dwFlags |= PSP_USEICONID;
    m_psp.hInstance = _Module.GetResourceInstance();

}



//////////////////////////////////////////////////////////////////////
// Message handlers
BOOL CColorPage::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	DoDataExchange();
	//ShowScrollBarItem();

	return TRUE;
}

LRESULT CColorPage::OnStaticColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
    HBRUSH hBrush = 0;
	//CWindowDC(GetDlgItem(IDC_STATIC_DEEP)).SetBkColor(g_config.clrThemeDeep);
	//CWindow* p = &(GetDlgItem(IDC_STATIC_DEEP));
	if ((HWND)lParam == GetDlgItem(IDC_STATIC_TAB_BUTTON).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrTabButton);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TAB_CAPTION_TEXT).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrTabCaptionText);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TAB_BACKGROUND).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrTabBackground);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TAB_BORDER).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrTabBorder);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TAB_SHADE).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrTabButtonShade);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_BULLET).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrBullet);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_BUTTON_SELECTED_BACKGROUND).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrSelectedTextBlockBackground);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TB_TEXTBLOCK_BORDER).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrTextBlockBorder);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TB_RIGHTBORDER).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrTextBlockRightBorder);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TB_SCROLLBAR).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrScrollBar);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_SCROLLBAR_BORDER).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrScrollBarBorder);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TB_THUMBNAIL).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrThumbnail);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_THUMBNAIL_BORDER).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrThumbnailBorder);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_FRAME_BKG).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrFrame);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_FRAME_INNER_BORDER).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrFrameInnerBorder);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_FRAME_OUTER_BORDER).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrFrameOuterBorder);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_FRAME_CAPTION_TITLE).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrCaptionText);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_CAPTION_BUTTON).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrCaptionButton);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TB_TEXTBLOCK_UNSELECTED).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrUnSelectedTextBlockBackground);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_TB_TEXTBLOCK_FOCUSED).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrFocusedTextBlockBackground);
	}
	else if ((HWND)lParam == GetDlgItem(IDC_STATIC_VIEW_BKG).m_hWnd)
	{
		hBrush = CreateSolidBrush(g_config.clrViewBackground);
	}

	return (LRESULT)hBrush;
}

LRESULT CColorPage::OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
	if (lppsn->hdr.code == PSN_APPLY)
	{
		DoDataExchange(TRUE);
		if (g_config.nScrollBarTransparentRatio > 100)
			g_config.nScrollBarTransparentRatio = 100;
		if (g_config.nScrollBarTransparentRatio < 1)
			g_config.nScrollBarTransparentRatio = 1;
	}
	bHandled = FALSE;
	return 0;
}
LRESULT CColorPage::OnBnClickedButtonTabButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTabButton;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrTabButton = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTabCaptionText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTabCaptionText;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrTabCaptionText = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTabBackground(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTabBackground;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrTabBackground = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}


	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTabShade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTabButtonShade;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrTabButtonShade = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTbBullet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrBullet;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrBullet = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTbTextblockBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTextBlockBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrTextBlockBorder = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}


	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTbScrollbar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrScrollBar;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrScrollBar = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonSelectedBackground(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrSelectedTextBlockBackground;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrSelectedTextBlockBackground = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTbRightborder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTextBlockRightBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrTextBlockRightBorder = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonThumbnail(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrThumbnail;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrThumbnail = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonFrameBkg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrFrame;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrFrame = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonFrameInnerBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrFrameInnerBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrFrameInnerBorder = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonFrameOuterBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrFrameOuterBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrFrameOuterBorder = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonFrameCaptionTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrCaptionText;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrCaptionText = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonCaptionButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrCaptionButton;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrCaptionButton = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTabBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTabBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrTabBorder = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonScrollbarBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrScrollBarBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrScrollBarBorder = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}


	return 0;
}

LRESULT CColorPage::OnBnClickedButtonThumbnailBorder2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrThumbnailBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrThumbnailBorder = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTbTextblockUnselected(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrThumbnailBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrUnSelectedTextBlockBackground = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonTbTextblockFocused(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrThumbnailBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrFocusedTextBlockBackground = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CColorPage::OnBnClickedButtonViewBkg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrThumbnailBorder;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		g_config.clrViewBackground = cc.rgbResult;
		Invalidate();
		GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}
