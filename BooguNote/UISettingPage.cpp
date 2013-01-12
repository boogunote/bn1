#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "UISettingPage.h"
#include "BooguNoteConfig.h"

extern CBooguNoteConfig g_config;

//////////////////////////////////////////////////////////////////////
// Construction

CUISettingPage::CUISettingPage() : m_bDirty(false)
{
    m_psp.dwFlags |= PSP_USEICONID;
    //m_psp.pszIcon = MAKEINTRESOURCE(IDI_TABICON);
    m_psp.hInstance = _Module.GetResourceInstance();

	nVScrollbarLength = g_config.nVScrollbarLength;
	nHScrollbarLength = g_config.nHScrollbarLength;
	nScrollbarWidth = g_config.nScrollbarWidth;
	bAutoScrollBarLength = g_config.bAutoScrollBarLength;
	bEnableHScrollBar = g_config.bEnableHScrollBar;
	nBigImageHeight = g_config.nBigThumbnailHeight;
	nSmallImageHeight = g_config.nSmallThumbnailHeight;
}



//////////////////////////////////////////////////////////////////////
// Message handlers
BOOL CUISettingPage::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	DoDataExchange();
	ShowScrollBarItem();

	return TRUE;
}

//int CShortcutPage::OnApply()
//{
//    return DoDataExchange(true) ? PSNRET_NOERROR : PSNRET_INVALID;
//}

LRESULT CUISettingPage::OnDeepStaticColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
    HBRUSH hBrush = 0;
	////CWindowDC(GetDlgItem(IDC_STATIC_DEEP)).SetBkColor(g_config.clrThemeDeep);
	////CWindow* p = &(GetDlgItem(IDC_STATIC_DEEP));
	//if ((HWND)lParam == GetDlgItem(IDC_STATIC_DEEP).m_hWnd)
	//{
	//	hBrush = CreateSolidBrush(g_config.clrThemeDeep);
	//}
	//else if ((HWND)lParam == GetDlgItem(IDC_STATIC_LIGHT).m_hWnd)
	//{
	//	hBrush = CreateSolidBrush(g_config.clrThemeLight);
	//}
	//else if ((HWND)lParam == GetDlgItem(IDC_STATIC_DARK).m_hWnd)
	//{
	//	hBrush = CreateSolidBrush(g_config.clrThemeDark);
	//}
	return (LRESULT)hBrush;
}

LRESULT CUISettingPage::OnBnClickedButtonView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSEFONT cf;
	LOGFONT lf;
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof (cf);
	cf.hwndOwner = m_hWnd;
	cf.lpLogFont = &lf;
	GetObject(g_config.hFontNormal, sizeof(LOGFONT), cf.lpLogFont);
	cf.Flags = CF_SCREENFONTS;
	if (ChooseFont(&cf)==TRUE)
	{
		g_config.fontSize = cf.iPointSize/10;
		if (cf.lpLogFont->lfFaceName[0]!=_T('\0'))
		{
			_tcsncpy(g_config.fontName,cf.lpLogFont->lfFaceName, LF_FACESIZE);
		}
		DoDataExchange();
		g_config.hFontNormal = CreateFont(PointsToLogical(g_config.fontSize), 
			  0, 0, 0, 
			  0,
			  0,0,0,DEFAULT_CHARSET,0,0,
			  DEFAULT_QUALITY,
			  0,
			  g_config.fontName);
		GetParent().GetParent().PostMessage(WM_USER_MAINFRM_REFRESH);
	}
	return 0;
}

LRESULT CUISettingPage::OnBnClickedButtonTabname(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSEFONT cf;
	LOGFONT lf;
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof (cf);
	cf.hwndOwner = m_hWnd;
	cf.lpLogFont = &lf;
	GetObject(g_config.hTabFontBold, sizeof(LOGFONT), cf.lpLogFont);
	cf.Flags = CF_SCREENFONTS;
	if (ChooseFont(&cf)==TRUE)
	{
		g_config.tabFontSize = cf.iPointSize/10;
		if (cf.lpLogFont->lfFaceName[0]!=_T('\0'))
		{
			_tcsncpy(g_config.tabFontName,cf.lpLogFont->lfFaceName, LF_FACESIZE);
		}
		DoDataExchange();
		g_config.hTabFontBold = CreateFont(PointsToLogical(g_config.tabFontSize), 
			  0, 0, 0, 
			  FW_BOLD,
			  0,0,0,DEFAULT_CHARSET,0,0,
			  DEFAULT_QUALITY,
			  0,
			  g_config.tabFontName);
		GetParent().GetParent().PostMessage(WM_USER_MAINFRM_REFRESH);
		//GetParent().GetParent().SendMessage(WM_NCPAINT);
	}

	return 0;
}

LRESULT CUISettingPage::OnBnClickedButtonDeep(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CHOOSECOLOR cc;                 // common dialog box structure 
	//static COLORREF acrCustClr[16]; // array of custom colors 
	////HWND hwnd;                      // owner window
	////HBRUSH hbrush;                  // brush handle
	//static DWORD rgbCurrent;        // initial color selection

	//// Initialize CHOOSECOLOR 
	//ZeroMemory(&cc, sizeof(cc));
	//cc.lStructSize = sizeof(cc);
	//cc.hwndOwner = m_hWnd;
	//cc.lpCustColors = (LPDWORD) acrCustClr;
	//cc.rgbResult = g_config.clrThemeDeep;
	//cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	// 
	//if (ChooseColor(&cc)==TRUE) 
	//{
	//	g_config.clrThemeDeep = cc.rgbResult;
	//	Invalidate();
	//	GetParent().GetParent().SendMessage(WM_NCPAINT);
	//}
	return 0;
}

LRESULT CUISettingPage::OnBnClickedButtonLight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CHOOSECOLOR cc;                 // common dialog box structure 
	//static COLORREF acrCustClr[16]; // array of custom colors 
	////HWND hwnd;                      // owner window
	////HBRUSH hbrush;                  // brush handle
	//static DWORD rgbCurrent;        // initial color selection

	//// Initialize CHOOSECOLOR 
	//ZeroMemory(&cc, sizeof(cc));
	//cc.lStructSize = sizeof(cc);
	//cc.hwndOwner = m_hWnd;
	//cc.lpCustColors = (LPDWORD) acrCustClr;
	//cc.rgbResult = g_config.clrThemeLight;
	//cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	// 
	//if (ChooseColor(&cc)==TRUE) 
	//{
	//	g_config.clrThemeLight = cc.rgbResult;
	//	Invalidate();
	//	GetParent().GetParent().SendMessage(WM_NCPAINT);
	//}

	return 0;
}

LRESULT CUISettingPage::OnBnClickedButtonDark(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CHOOSECOLOR cc;                 // common dialog box structure 
	//static COLORREF acrCustClr[16]; // array of custom colors 
	////HWND hwnd;                      // owner window
	////HBRUSH hbrush;                  // brush handle
	//static DWORD rgbCurrent;        // initial color selection

	//// Initialize CHOOSECOLOR 
	//ZeroMemory(&cc, sizeof(cc));
	//cc.lStructSize = sizeof(cc);
	//cc.hwndOwner = m_hWnd;
	//cc.lpCustColors = (LPDWORD) acrCustClr;
	//cc.rgbResult = g_config.clrThemeDark;
	//cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	// 
	//if (ChooseColor(&cc)==TRUE) 
	//{
	//	g_config.clrThemeDark = cc.rgbResult;
	//	Invalidate();
	//	GetParent().GetParent().SendMessage(WM_NCPAINT);
	//}

	return 0;
}

LRESULT CUISettingPage::OnBnClickedCheckAutoScrollbarLength(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (bAutoScrollBarLength)
	{
		bAutoScrollBarLength = false;
		
	}
	else
	{
		bAutoScrollBarLength = true;
	}
	//g_config.bAutoScrollBarLength = ~g_config.bAutoScrollBarLength;
	ShowScrollBarItem();
	m_bDirty = true;
	return 0;
}

LRESULT CUISettingPage::OnBnClickedCheckEnableBottomScrollbar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (bEnableHScrollBar)
	{
		bEnableHScrollBar = false;
	}
	else
	{
		bEnableHScrollBar = true;
	}
	//g_config.bEnableHScrollBar = ~g_config.bEnableHScrollBar;
	ShowScrollBarItem();
	m_bDirty = true;

	return 0;
}

void CUISettingPage::ShowScrollBarItem()
{
	if (bAutoScrollBarLength)
	{
		((CEdit)GetDlgItem(IDC_EDIT_RIGHT_SCROLLBAR_LEN)).EnableWindow(FALSE);
		((CEdit)GetDlgItem(IDC_EDIT_BOTTOM_SCROLLBAR_LEN)).EnableWindow(FALSE);
	}
	else
	{
		((CEdit)GetDlgItem(IDC_EDIT_RIGHT_SCROLLBAR_LEN)).EnableWindow(TRUE);
		if (bEnableHScrollBar)
		{
			((CEdit)GetDlgItem(IDC_EDIT_BOTTOM_SCROLLBAR_LEN)).EnableWindow(TRUE);
		}
		else
		{
			((CEdit)GetDlgItem(IDC_EDIT_BOTTOM_SCROLLBAR_LEN)).EnableWindow(FALSE);
		}
	}
}

LRESULT CUISettingPage::OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
	if (lppsn->hdr.code == PSN_APPLY)
	{
		if (!m_bDirty && ((GetDlgItemInt(IDC_EDIT_RIGHT_SCROLLBAR_LEN) != g_config.nVScrollbarLength) ||
			(GetDlgItemInt(IDC_EDIT_BOTTOM_SCROLLBAR_LEN) != g_config.nHScrollbarLength) ||
			(GetDlgItemInt(IDC_EDIT_SCROLLBAR_WIDTH) != g_config.nScrollbarWidth) ||
			(GetDlgItemInt(IDC_EDIT_NARROW_WIDTH) != g_config.narrowWidth) ||
			(GetDlgItemInt(IDC_EDIT_WIDE_WIDTH) != g_config.wideWidth) ||
			(GetDlgItemInt(IDC_EDIT_SQUARE_BULLET_SIZE) != g_config.nHandleWidth)) ||
			(GetDlgItemInt(IDC_EDIT_SHRINK_CHARS) != g_config.nHeadLength) ||
			(GetDlgItemInt(IDC_EDIT_BIG_IMAGE_HEIGHT) != g_config.nBigThumbnailHeight) ||
			(GetDlgItemInt(IDC_EDIT_SMALL_IMAGE_HEIGHT) != g_config.nSmallThumbnailHeight))
		{
			m_bDirty = true;
		}
		DoDataExchange(TRUE);

		g_config.nVScrollbarLength = nVScrollbarLength;
		g_config.nHScrollbarLength = nHScrollbarLength;
		g_config.nScrollbarWidth = nScrollbarWidth;
		g_config.bAutoScrollBarLength = bAutoScrollBarLength;
		g_config.bEnableHScrollBar = bEnableHScrollBar;
		g_config.nBigThumbnailHeight = nBigImageHeight;
		g_config.nSmallThumbnailHeight = nSmallImageHeight;
	}
	bHandled = FALSE;
	return 0;
}