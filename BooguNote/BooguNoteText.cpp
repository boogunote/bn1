/**************************************************************************
   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   Copyright 2000 Microsoft Corporation.  All Rights Reserved.
**************************************************************************/


/*************************************************************************
 HOST.C	-- Text Host for Windowless Rich Edit Control 
 
***************************************************************************/

#include "stdafx.h"
#include "windows.h"
#include "stddef.h"
#include "windowsx.h"
#include "richedit.h"
#include "memory.h"
#include <imm.h>
//#include "tstxtsrv.h"
#include <tchar.h>
#include <time.h>


//#include "BooguNoteWidget.h"
#include "BooguNoteText.h"
//#include "BooguNoteView.h"
#include "BooguNoteConfig.h"
#include "BooguNoteHandle.h"
#include "BooguNoteIcon.h"

extern CBooguNoteConfig g_config;

//class CBooguNoteView;

BOOL fInAssert = FALSE;


// HIMETRIC units per inch (used for conversion)
#define HIMETRIC_PER_INCH 2540

// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
{
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
{
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

/******************************************************************************/


/*******************************************************************************/

// These constants are for backward compatibility. They are the 
// sizes used for initialization and reset in RichEdit 1.0

const LONG cInitTextMax = (32 * 1024) - 1;
const LONG cResetTextMax = (64 * 1024);


#define ibPed 0
#define SYS_ALTERNATE 0x20000000



INT imeEnabled;
INT	cxBorder, cyBorder;	    // GetSystemMetricx(SM_CXBORDER)...
INT	cxDoubleClk, cyDoubleClk;   // Double click distances
INT	cxHScroll, cxVScroll;	    // Width/height of scrlbar arw bitmap
INT	cyHScroll, cyVScroll;	    // Width/height of scrlbar arw bitmap
INT	dct;			    // Double Click Time in milliseconds
INT     nScrollInset;
COLORREF crAuto = 0;

LONG CBooguNoteText::xWidthSys = 0;    		            // average char width of system font
LONG CBooguNoteText::yHeightSys = 0;				// height of system font
LONG CBooguNoteText::yPerInch = 0;				// y pixels per inch
LONG CBooguNoteText::xPerInch = 0;				// x pixels per inch


EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
  };

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
  };


EXTERN_C const IID IID_ITextEditControl = { /* f6642620-d266-11ce-a89e-00aa006cadc5 */
    0xf6642620,
    0xd266,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
  };

void GetSysParms(void)
{
	crAuto		= GetSysColor(COLOR_WINDOWTEXT);
	cxBorder	= GetSystemMetrics(SM_CXBORDER);	// Unsizable window border
	cyBorder	= GetSystemMetrics(SM_CYBORDER);	//  widths
	cxHScroll	= GetSystemMetrics(SM_CXHSCROLL);	// Scrollbar-arrow bitmap 
	cxVScroll	= GetSystemMetrics(SM_CXVSCROLL);	//  dimensions
	cyHScroll	= GetSystemMetrics(SM_CYHSCROLL);	//
	cyVScroll	= GetSystemMetrics(SM_CYVSCROLL);	//
	cxDoubleClk	= GetSystemMetrics(SM_CXDOUBLECLK);
	cyDoubleClk	= GetSystemMetrics(SM_CYDOUBLECLK);
	imeEnabled	= GetSystemMetrics(SM_IMMENABLED);
	dct			= GetDoubleClickTime();
    
    nScrollInset =
        GetProfileIntA( "windows", "ScrollInset", DD_DEFSCROLLINSET );
}

HRESULT InitDefaultCharFormat(CHARFORMAT * pcf, HFONT hfont) 
{
	HWND hwnd;
	LOGFONT lf;
	HDC hdc;
	LONG yPixPerInch;

	// Get LOGFONT for default font
	if (!hfont)
		hfont = (HFONT)GetStockObject(SYSTEM_FONT);

	// Get LOGFONT for passed hfont
	if (!GetObject(hfont, sizeof(LOGFONT), &lf))
		return E_FAIL;

	// Set CHARFORMAT structure
	pcf->cbSize = sizeof(CHARFORMAT);
	
	hwnd = GetDesktopWindow();
	hdc = GetDC(hwnd);
	yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
	pcf->yHeight = -PointsToLogical(g_config.fontSize) * LY_PER_INCH / yPixPerInch;
	ReleaseDC(hwnd, hdc);

	pcf->yOffset = 0;
	pcf->crTextColor = crAuto;// RGB(220,220,220);

	/*pcf->dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;*/
	pcf->dwEffects &= ~(CFE_PROTECTED | CFE_LINK);

	if(lf.lfWeight < FW_BOLD)
		pcf->dwEffects &= ~CFE_BOLD;
	if(!lf.lfItalic)
		pcf->dwEffects &= ~CFE_ITALIC;
	if(!lf.lfUnderline)
		pcf->dwEffects &= ~CFE_UNDERLINE;
	if(!lf.lfStrikeOut)
		pcf->dwEffects &= ~CFE_STRIKEOUT;

	pcf->dwMask = CFM_ALL | CFM_COLOR | CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE | CFM_LINK;
	pcf->bCharSet = DEFAULT_CHARSET;
	pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef UNICODE
	_tcscpy(pcf->szFaceName, lf.lfFaceName);
#else
	//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
	MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif
	
	return S_OK;
}

HRESULT InitDefaultParaFormat(PARAFORMAT2 * ppf) 
{	
	memset(ppf, 0, sizeof(PARAFORMAT));

	ppf->cbSize = sizeof(PARAFORMAT);
	ppf->dwMask = PFM_ALL;
	ppf->wAlignment = PFA_LEFT;
	ppf->cTabCount = 1;
	ppf->rgxTabs[0] = lDefaultTab;

	return S_OK;
}



LRESULT MapHresultToLresult(HRESULT hr, UINT msg)
{
	LRESULT lres = hr;

	switch(msg)
	{
	case EM_GETMODIFY:


		lres = (hr == S_OK) ? -1 : 0;

		break;

		// These messages must return TRUE/FALSE rather than an hresult.
	case EM_UNDO:
	case WM_UNDO:
	case EM_CANUNDO:
	case EM_CANPASTE:
	case EM_LINESCROLL:

		// Hresults are backwards from TRUE and FALSE so we need
		// to do that remapping here as well.

		lres = (hr == S_OK) ? TRUE : FALSE;

		break;

	case EM_EXLINEFROMCHAR:
	case EM_LINEFROMCHAR:

		// If success, then hr a number. If error, it s/b 0.
		lres = SUCCEEDED(hr) ? (LRESULT) hr : 0;
		break;
			
	case EM_LINEINDEX:

		// If success, then hr a number. If error, it s/b -1.
		lres = SUCCEEDED(hr) ? (LRESULT) hr : -1;
		break;	

	default:
		lres = (LRESULT) hr;		
	}

	return lres;
}


BOOL GetIconic(HWND hwnd) 
{
	while(hwnd)
	{
		if(::IsIconic(hwnd))
			return TRUE;
		hwnd = GetParent(hwnd);
	}
	return FALSE;
}


CBooguNoteText::CBooguNoteText(int _left, int _top, int _right, int _bottom)
{
	ZeroMemory(&pnc, sizeof(CBooguNoteText) - offsetof(CBooguNoteText, pnc));

	cchTextMost = cInitTextMax;
    
	RECT rc;

	rc.bottom = _bottom;
	rc.right  = _right;
	rc.top = _top;
	rc.left = _left;
	
	SetClientRect(&rc, TRUE);

	fTransparent = TRUE;

	m_bHovering = false;
	m_bSelected = false;
	m_bClipped = false;
	//m_bFocused = false;
	m_bShown = true;
	m_nExpandState = TEXT_BLOCK_WIDE;

	m_szCache = NULL;

	m_pTextHandle = NULL;
	m_pTextIcon = NULL;
	m_bBold = false;
	m_bFile = false;
	m_bShowBranch = false;

	m_TextClr = 0x00000000;
	m_BkgrdClr = 0x00FFFFFF;

	m_hFileIcon = NULL;

	m_ModifyTime = 0;
}

CBooguNoteText::~CBooguNoteText()
{
	// Revoke our drop target
	RevokeDragDrop();

	pserv->OnTxInPlaceDeactivate();

	int nRef = pserv->Release();

	if (NULL != m_szCache)
		delete[] m_szCache;

	if (NULL != m_pTextHandle)
		delete m_pTextHandle;
	if (NULL != m_pTextHandle)
		delete m_pTextIcon;
	if (NULL != m_hFileIcon)
	{
		DestroyIcon(m_hFileIcon);
	}
}

BOOL CBooguNoteText::HitTest(int x, int y)
{
	RECT* prc = GetClientRect();
	if (m_bFile && m_pBitmap != NULL)
	{
		if ( x>=prc->left && x<=prc->left+m_pBitmap->GetWidth() && y>=prc->top && y<=prc->top+m_pBitmap->GetHeight())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if ( x>=prc->left && x<=prc->right && y>=prc->top && y<=prc->bottom)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

////////////////////// Create/Init/Destruct Commands ///////////////////////


/*
 *	CBooguNoteText::Init
 *
 *	Purpose:
 *		Initializes this CBooguNoteText
 *
 *	TODO:	Format Cache logic needs to be cleaned up. 
 */
BOOL CBooguNoteText::Init(
	HWND h_wnd,
	POINT* pptOffset,
	CBooguNoteText** ppFocused,
	const CREATESTRUCT *pcs,
	PNOTIFY_CALL p_nc)
{
    HDC hdc;
    //HFONT hfontOld;
    TEXTMETRIC tm;
	IUnknown *pUnk;
	HRESULT hr;

	// Initialize Reference count
	cRefs = 1;

	// Set up the notification callback
	pnc = p_nc;	
	
	hwnd = h_wnd;
	m_pptOffset = pptOffset;
	m_ppTextBlockFocused = ppFocused;

	//m_nHostBorder = nHostBorder;

	// Create and cache CHARFORMAT for this control
	if(FAILED(InitDefaultCharFormat(&cf, g_config.hFontNormal)))
		goto err;

	// Create and cache PARAFORMAT for this control
	if(FAILED(InitDefaultParaFormat(&pf)))
		goto err;

 	// edit controls created without a window are multiline by default
	// so that paragraph formats can be
	dwStyle = ES_MULTILINE;
	fHidden = TRUE;

	// edit controls are rich by default
	fRich = TRUE;
	
	if(pcs)
	{
		hwndParent = pcs->hwndParent;
		dwExStyle = pcs->dwExStyle;
		dwStyle = pcs->style;

		fBorder = !!(dwStyle & WS_BORDER);

		if(dwStyle & ES_SUNKEN)
		{
			fBorder = TRUE;
		}

		if (!(dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)))
		{
			fWordWrap = TRUE;
		}
	}

	if(!(dwStyle & ES_LEFT))
	{
		if(dwStyle & ES_CENTER)
			pf.wAlignment = PFA_CENTER;
		else if(dwStyle & ES_RIGHT)
			pf.wAlignment = PFA_RIGHT;
	}

    // Init system metrics
	hdc = GetDC(hwnd);
    if(!hdc)
        goto err;

   	//hfontOld = (HFONT)SelectObject(hdc, GetStockObject(SYSTEM_FONT));

	//if(!hfontOld)
	//	goto err;

	GetTextMetrics(hdc, &tm);
	//SelectObject(hdc, hfontOld);

	xWidthSys = (INT) tm.tmAveCharWidth;
   yHeightSys = (INT) tm.tmHeight;
	xPerInch = GetDeviceCaps(hdc, LOGPIXELSX); 
	yPerInch =	GetDeviceCaps(hdc, LOGPIXELSY); 

	ReleaseDC(hwnd, hdc);

	// At this point the border flag is set and so is the pixels per inch
	// so we can initalize the inset.
	SetDefaultInset();

	fInplaceActive = TRUE;

	HINSTANCE hDll = ::LoadLibrary(L"MSFTEDIT.DLL");
	
	SetLastError(0);
	PCreateTextServices  pFun  = (PCreateTextServices)::GetProcAddress(hDll,"CreateTextServices");
	DWORD ttt = GetLastError();

	// Create Text Services component
	if(FAILED((pFun)(NULL, this, &pUnk)))
		goto err;
	//// Create Text Services component
	//if(FAILED(CreateTextServices(NULL, this, &pUnk)))
	//	goto err;

	hr = pUnk->QueryInterface(IID_ITextServices,(void **)&pserv);

	// Whether the previous call succeeded or failed we are done
	// with the private interface.
	pUnk->Release();

	if(FAILED(hr))
	{
		goto err;
	}

	// Set window text
	if(pcs && pcs->lpszName)
	{
#ifdef UNICODE		
		if(FAILED(pserv->TxSetText((TCHAR *)pcs->lpszName)))
			goto err;
#else
		//have to thunk the string to a unicode string.
		WCHAR wsz_name[MAX_PATH] ;
		ZeroMemory(wsz_name, MAX_PATH *sizeof WCHAR) ;
		MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, lstrlen(pcs->lpszName),(LPWSTR)&wsz_name, MAX_PATH) ;
		if(FAILED(pserv->TxSetText((LPWSTR)&wsz_name)))
			goto err;
#endif
	}

	rcClient.left = pcs->x;
	rcClient.top = pcs->y;
	rcClient.right = pcs->x + pcs->cx;
	rcClient.bottom = pcs->y + pcs->cy;

	// The extent matches the full client rectangle in HIMETRIC
	sizelExtent.cx = DXtoHimetricX(pcs->cx - 2 * g_config.hostBorder, xPerInch);
	sizelExtent.cy = DYtoHimetricY(pcs->cy - 2 * g_config.hostBorder, yPerInch);

	// notify Text Services that we are in place active
	if(FAILED(pserv->OnTxInPlaceActivate(&rcClient)))
		goto err;
	
	// Hide all scrollbars to start
	if(hwnd && !(dwStyle & ES_DISABLENOSCROLL))
	{
		LONG dwStyle = GetWindowLong(hwnd, GWL_STYLE);
		dwStyle &= ~(WS_VSCROLL | WS_HSCROLL);
		SetWindowLong(hwnd, GWL_STYLE, dwStyle);
	}

	if (!(dwStyle & ES_READONLY))
	{
		// This isn't a read only window so we need a drop target.
		//RegisterDragDrop();
	}

	//HFONT hFont = EasyCreateFont(g_config.fontSize, FALSE, DEFAULT_QUALITY, g_config.fontName);
	//HFONT hFont = CreateFont(PointsToLogical(g_config.fontSize), 
	//				  0, 0, 0, 
	//				  0,
	//				  TRUE,0,0,DEFAULT_CHARSET,0,0,
	//				  DEFAULT_QUALITY,
	//				  0,
	//				  g_config.fontName);
	//GetTextServices()->TxSendMessage(WM_SETFONT, (WPARAM)g_config.hFontNormal, 0, 0);

	DWORD FontStyle = 0;
	GetTextServices()->TxSendMessage(EM_GETLANGOPTIONS, 0, 0, (LRESULT *)&FontStyle);
	if(FontStyle & IMF_DUALFONT)
		FontStyle &= ~ IMF_DUALFONT;
	GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, (LPARAM)FontStyle, 0);

	//cf.bCharSet
	//GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf, 0);

	//set event
	DWORD mask = GetTextServices()->TxSendMessage(EM_GETEVENTMASK, 0, 0, 0);
	GetTextServices()->TxSendMessage(EM_SETEVENTMASK, 0, mask|ENM_REQUESTRESIZE|ENM_LINK, 0);
	//set url detect
	GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0);
	//set text limit to 128M KB
	GetTextServices()->TxSendMessage(EM_EXLIMITTEXT, 0, (LPARAM)0X07FFFFFF, 0);

	CHARFORMAT2 cf;
	cf.cbSize = sizeof(CHARFORMAT2);
	GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
	cf.dwMask = CFM_COLOR|CFM_BACKCOLOR;
	cf.dwEffects &= ~CFE_AUTOBACKCOLOR;
	cf.dwEffects &= ~~CFE_AUTOCOLOR;
	cf.crTextColor = m_TextClr;
	cf.crBackColor = m_BkgrdClr;
	GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);

	return TRUE;

err:
	return FALSE;
}


/////////////////////////////////  IUnknown ////////////////////////////////


HRESULT CBooguNoteText::QueryInterface(REFIID riid, void **ppvObject)
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_ITextEditControl))
	{
		*ppvObject = (ITextEditControl *) this;
		AddRef();
		hr = S_OK;
	}
	else if (IsEqualIID(riid, IID_IUnknown) 
		|| IsEqualIID(riid, IID_ITextHost)) 
	{
		AddRef();
		*ppvObject = (ITextHost *) this;
		hr = S_OK;
	}

	return hr;
}

ULONG CBooguNoteText::AddRef(void)
{
	return ++cRefs;
}

ULONG CBooguNoteText::Release(void)
{
	ULONG c_Refs = --cRefs;

	if (c_Refs == 0)
	{
		delete this;
	}

	return c_Refs;
}


//////////////////////////////// Properties ////////////////////////////////


TXTEFFECT CBooguNoteText::TxGetEffects() const
{
	return (dwStyle & ES_SUNKEN) ? TXTEFFECT_SUNKEN : TXTEFFECT_NONE;
}


//////////////////////////// System API wrapper ////////////////////////////



///////////////////////  Windows message dispatch methods  ///////////////////////////////


LRESULT CBooguNoteText::TxWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT	lres = 0;
	HRESULT hr;

	switch(msg)
	{
	case WM_NCCALCSIZE:
		// we can't rely on WM_WININICHANGE so we use WM_NCCALCSIZE since
		// changing any of these should trigger a WM_NCCALCSIZE
		GetSysParms();
		break;

	case WM_KEYDOWN:
		lres = OnKeyDown((WORD) wparam, (DWORD) lparam);
		if(lres != 0)
			goto serv;
		break;		   

	//case WM_UNICHAR:
	//case WM_IME_CHAR:
	//	break;
	case WM_CHAR:
		lres = OnChar((WORD) wparam, (DWORD) lparam);
		//TxInvalidateRect(NULL, FALSE);
		if(lres != 0)
			goto serv;
		break;

	case WM_SYSCOLORCHANGE:
		OnSysColorChange();

		// Notify the text services that there has been a change in the 
		// system colors.
		goto serv;

	case WM_GETDLGCODE:
		lres = OnGetDlgCode(wparam, lparam);
		break;

	case EM_HIDESELECTION:
		if((BOOL)lparam)
		{
			DWORD dwPropertyBits = 0;

			if((BOOL)wparam)
			{
				dwStyle &= ~(DWORD) ES_NOHIDESEL;
				dwPropertyBits = TXTBIT_HIDESELECTION;
			}
			else
				dwStyle |= ES_NOHIDESEL;

			// Notify text services of change in status.
			pserv->OnTxPropertyBitsChange(TXTBIT_HIDESELECTION, 
				dwPropertyBits);
		}

		goto serv;



    case EM_LIMITTEXT:

        lparam = wparam;

        // Intentionally fall through. These messages are duplicates.

	case EM_EXLIMITTEXT:

        if (lparam == 0)
        {
            // 0 means set the control to the maximum size. However, because
            // 1.0 set this to 64K will keep this the same value so as not to
            // supprise anyone. Apps are free to set the value to be above 64K.
            lparam = (LPARAM) cResetTextMax;
        }

		cchTextMost = (LONG) lparam;
		pserv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, 
					TXTBIT_MAXLENGTHCHANGE);

		break;

	case EM_SETREADONLY:
		OnSetReadOnly(BOOL(wparam));
		lres = 1;
		break;

	case EM_GETEVENTMASK:
		lres = OnGetEventMask();
		break;

	case EM_SETEVENTMASK:
		OnSetEventMask((DWORD) lparam);
		goto serv;

	case EM_GETOPTIONS:
		lres = OnGetOptions();
		break;

	case EM_SETOPTIONS:
		OnSetOptions((WORD) wparam, (DWORD) lparam);
		lres = (dwStyle & ECO_STYLES);
		if(fEnableAutoWordSel)
			lres |= ECO_AUTOWORDSELECTION;
		break;

	case WM_SETFONT:
		lres = OnSetFont((HFONT) wparam);
		break;

	case EM_SETRECT:
        OnSetRect((LPRECT)lparam);
        break;
        
	case EM_GETRECT:
        OnGetRect((LPRECT)lparam);
        break;

	case EM_SETBKGNDCOLOR:

		lres = (LRESULT) crBackground;
		fNotSysBkgnd = !wparam;
		crBackground = (COLORREF) lparam;

		if(wparam)
			crBackground = GetSysColor(COLOR_WINDOW);

		// Notify the text services that color has changed
		pserv->TxSendMessage(WM_SYSCOLORCHANGE, 0, 0, 0);

		if(lres != (LRESULT) crBackground)
			TxInvalidateRect(NULL, TRUE);

		break;

	case EM_SETCHARFORMAT:

		if(!FValidCF((CHARFORMAT *) lparam))
		{
			return 0;
		}

		if(wparam & SCF_SELECTION)
			goto serv;								// Change selection format
		OnSetCharFormat((CHARFORMAT *) lparam);		// Change default format
		break;

	case EM_SETPARAFORMAT:
		if(!FValidPF((PARAFORMAT2 *) lparam))
		{
			return 0;
		}

		// check to see if we're setting the default.
		// either SCF_DEFAULT will be specified *or* there is no
		// no text in the document (richedit1.0 behaviour).
		if (!(wparam & SCF_DEFAULT))
		{
			hr = pserv->TxSendMessage(WM_GETTEXTLENGTH, 0, 0, 0);

			if (hr == 0)
			{
				wparam |= SCF_DEFAULT;
			}
		}

		if(wparam & SCF_DEFAULT)
		{								
			OnSetParaFormat((PARAFORMAT2 *) lparam);	// Change default format
		}
		else
		{
			goto serv;								// Change selection format
		}
		break;

    case WM_SETTEXT:

        // For RichEdit 1.0, the max text length would be reset by a settext so 
        // we follow pattern here as well.

		hr = pserv->TxSendMessage(msg, wparam, lparam, 0);

        if (SUCCEEDED(hr))
        {
            // Update succeeded.
            LONG cNewText = _tcslen((LPCTSTR) lparam);

            // If the new text is greater than the max set the max to the new
            // text length.
            if (cNewText > cchTextMost)
            {
                cchTextMost = cNewText;                
            }

			lres = 1;
        }

        break;

	case WM_SIZE:
		//lres = OnSize(hwnd, wparam, LOWORD(lparam), HIWORD(lparam));
		//lres = -1;
		break;

	case WM_WINDOWPOSCHANGING:
		lres = ::DefWindowProc(hwnd, msg, wparam, lparam);

		if(TxGetEffects() == TXTEFFECT_SUNKEN)
			OnSunkenWindowPosChanging(hwnd, (WINDOWPOS *) lparam);
		break;

	case WM_SETCURSOR:
		//Only set cursor when over us rather than a child; this
		//			helps prevent us from fighting it out with an inplace child
		if((HWND)wparam == hwnd)
		{
			if(!(lres = ::DefWindowProc(hwnd, msg, wparam, lparam)))
			{
				POINT pt;
				GetCursorPos(&pt);
				::ScreenToClient(hwnd, &pt);
				pserv->OnTxSetCursor(
					DVASPECT_CONTENT,	
					-1,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,			// Client rect - no redraw 
					pt.x, 
					pt.y);
				lres = TRUE;
			}
		}
		break;

	case WM_SHOWWINDOW:
		hr = OnTxVisibleChange((BOOL)wparam);
		break;

	case WM_NCPAINT:

		lres = ::DefWindowProc(hwnd, msg, wparam, lparam);

		if(TxGetEffects() == TXTEFFECT_SUNKEN)
		{
			HDC hdc = GetDC(hwnd);

			if(hdc)
			{
				DrawSunkenBorder(hwnd, hdc);
				ReleaseDC(hwnd, hdc);
			}
		}
		break;

	case WM_PAINT:
		{
			//OutputDebugString(TEXT("WM_PAINT")) ;
			// Put a frame around the control so it can be seen
			/*FrameRect((HDC) wparam, &rcClient, 
				(HBRUSH) GetStockObject(BLACK_BRUSH));*/
			//FillRect((HDC) wparam, &rcClient,(HBRUSH) GetStockObject(GRAY_BRUSH));
			CPen pen;
			/*if (m_bHovering && !((*m_ppTextBlockFocused) == this))
			{
				pen.CreatePen(PS_DOT, 1, g_config.clrThemeDeep);
			}
			else */if ((*m_ppTextBlockFocused) == this)
			{
				pen.CreatePen(PS_SOLID, 1, g_config.clrTextBlockBorder);
			}
			else if (m_bSelected)
			{
				pen.CreatePen(PS_DOT, 1, g_config.clrTextBlockBorder);
			}
			else
			{
				pen.CreatePen(PS_NULL, 1, RGB(0,0,0));
				//pen.CreatePen(PS_NULL, 1, g_config.clrThemeLight);
			}
			SelectObject((HDC) wparam, pen);
			if (m_bSelected && ! ((*m_ppTextBlockFocused) == this))
			{
				SelectObject((HDC) wparam, (HBRUSH)GetStockObject(DC_BRUSH));
				SetDCBrushColor((HDC) wparam, g_config.clrSelectedTextBlockBackground);
			}
			else if ((*m_ppTextBlockFocused) == this)
			{
				SelectObject((HDC) wparam, (HBRUSH)GetStockObject(DC_BRUSH));
				SetDCBrushColor((HDC) wparam, g_config.clrFocusedTextBlockBackground);
				//SetDCBrushColor((HDC) wparam, g_config.clrThemeLight);
				
			}
			else
			{
				SelectObject((HDC) wparam, (HBRUSH)GetStockObject(DC_BRUSH));
				SetDCBrushColor((HDC) wparam, g_config.clrUnSelectedTextBlockBackground);
			}

			//RoundRect((HDC) wparam, rcClient.left-1, rcClient.top-2, rcClient.right, rcClient.bottom+2, 4/*g_config.hostBorder*4*/, 4/*g_config.hostBorder*4*/);

			RECT rcControl;
			RECT *prc = NULL;
			LONG lViewId = TXTVIEW_ACTIVE;

			if (!fInplaceActive)
			{
				GetControlRect(&rcControl);
				prc = &rcControl;
				lViewId = TXTVIEW_INACTIVE;
			}
			/*DPtoLP((HDC) wparam, (PPOINT)prc, 2);*/
			// Remember wparam is actually the hdc and lparam is the update
			// rect because this message has been preprocessed by the window.
			if (m_bFile && NULL != m_pBitmap)
			{
				RoundRect((HDC) wparam, rcClient.left-2, rcClient.top-2, rcClient.left+ m_pBitmap->GetWidth()+2, rcClient.top+m_pBitmap->GetHeight()+2, 4, 4);
				Graphics graphics((HDC) wparam);
				graphics.DrawImage(m_pBitmap, rcClient.left, rcClient.top, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
					//rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

			}
			else
			{
				RoundRect((HDC) wparam, rcClient.left-1, rcClient.top-2, rcClient.right, rcClient.bottom+2, 4, 4);
				pserv->TxDraw(
	    			DVASPECT_CONTENT,  		// Draw Aspect
					/*-1*/0,						// Lindex
					NULL,					// Info for drawing optimazation
					NULL,					// target device information
	        		(HDC) wparam,			// Draw device HDC
	        		NULL, 				   	// Target device HDC
					(RECTL *) prc,			// Bounding client rectangle
					NULL, 					// Clipping rectangle for metafiles
					(RECT *) NULL,		// Update rectangle
					NULL, 	   				// Call back function
					NULL,					// Call back parameter
					lViewId);				// What view of the object	
			}

			/*if(TxGetEffects() == TXTEFFECT_SUNKEN)
				DrawSunkenBorder(hwnd, (HDC) wparam);*/
		}

		break;
	case WM_MOUSEMOVE:
		{
			//int x = GET_X_LPARAM(lparam) +m_pptOffset->x;
			//int y = GET_Y_LPARAM(lparam) +m_pptOffset->y;
			//lparam = MAKELPARAM((WORD)x, (WORD)y);
			goto serv;
		}
		break;
	case WM_SETFOCUS:
		{
			if (NULL != m_pBitmap)
			{
				//OutputDebugString(_T("::HideCaret(hwnd);"));
				::HideCaret(hwnd);
			}
			else
			{
				goto serv;
			}
		}
		break;
	case WM_KILLFOCUS:
		{
			DetectFileBlock();
			goto serv;
		}
	default:
serv:
		{
			hr = pserv->TxSendMessage(msg, wparam, lparam, &lres);

			if (hr == S_FALSE)
			{
				//lres = ::DefWindowProc(hwnd, msg, wparam, lparam);
			}
		}
	}

	return lres;
}

void CBooguNoteText::DetectFileBlock()
{
	if (!m_bFile)
	{
		if (TEXT_BLOCK_SHRINK != m_nExpandState)
		{
			//OutputDebugString(_T("WM_KILLFOCUS\n"));
			BSTR  bstrCache;
			pserv->TxGetText(&bstrCache);
			int length = SysStringLen(bstrCache);
			TCHAR* szCache = new TCHAR[length+1];
			_stprintf(szCache, _T("%s"), (LPCTSTR)bstrCache);
			
			//remove the last return code.
			if ((_T('\r') == szCache[length-1]) || (_T('\n') == szCache[length-1]))
			{
				szCache[length-1] = _T('\0');
				length--;
			}
			if (length < _MAX_PATH && length > 3)
			{
				//Detect Storage Root
// 				int nRootStorageDirLen = _tcsclen(g_config.szRootStorageDir);
// 				if (nRootStorageDirLen>0)
// 				{
// 					if (_T(':') == szCache[1] && INVALID_FILE_ATTRIBUTES != GetFileAttributes(szCache))
// 					{
// 						CString fileDirectory(szCache);
// 						CString tempStorageRootStr(g_config.szRootStorageDir);
// 						tempStorageRootStr.MakeLower();
// 						if (_T('\\')!=tempStorageRootStr[tempStorageRootStr.GetLength()-1])
// 						{
// 							tempStorageRootStr += _T("\\");
// 						}
// 						CString tempfileDirectory = fileDirectory;
// 						tempfileDirectory.MakeLower();
// 						if (0 == tempfileDirectory.Find(tempStorageRootStr))
// 						{
// 							
// 							fileDirectory.Delete(0, nRootStorageDirLen);
// 							_stprintf(szCache, _T("%s"), fileDirectory.GetBuffer(MAX_PATH));
// 							fileDirectory.ReleaseBuffer();
// 						}
// 					}
// 				}
				//Process Storage Root
				TCHAR szTempCache[MAX_PATH*3];
				if (_T('\\') == szCache[0] && _T('\\') != szCache[1])
				{
					_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, szCache);
				}
				else if (L'.' == szCache[0])
				{
					CString* pfileDirectory = (CString *)((CWindow*)m_pParentWindow->SendMessage(WM_USER_GET_FILE_DIR, 0, 0));
					if (NULL != pfileDirectory && !pfileDirectory->IsEmpty())
					{
						swprintf_s(szTempCache, MAX_PATH, L"%s", *pfileDirectory);
						PathRemoveFileSpec(szTempCache);
						PathAppend(szTempCache, szCache);
					}
					//Process Breaked Link
					if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(szTempCache))
					{
						CString strStem = szCache;
						strStem.MakeReverse();
						strStem.Delete(strStem.Find(L"\\."), strStem.GetLength());
						strStem.MakeReverse();
						
						//in same directory
						CString strRelative = L".\\" + strStem;
						CString strRoot = *pfileDirectory;
						PathRemoveFileSpec(strRoot.GetBuffer(MAX_PATH));
						strRoot.ReleaseBuffer();
						PathAddBackslash(strRoot.GetBuffer(MAX_PATH));
						strRoot.ReleaseBuffer();

						CString strFullPath = strRoot;
						PathAppend(strFullPath.GetBuffer(MAX_PATH), strRelative);
						strFullPath.ReleaseBuffer();
						if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(strFullPath))
						{
							swprintf_s(szTempCache, MAX_PATH, L"%s", strFullPath);
							if (szCache)
							{
								delete []szCache;
								szCache = new TCHAR[MAX_PATH*3];
								swprintf_s(szCache, MAX_PATH, L"%s", strRelative);
							}
						}
						else
						{
							//count \ number
							int nBackSlash=0; 
							for(int i=-1;i < strRoot.GetLength();i++) 
							{ 
								i=strRoot.Find('\\',i+1); 
								if (-1 == i)
								{
									break;
								}
								nBackSlash++; 
							}
							nBackSlash--;
							if (nBackSlash>0)
							{
								strRelative = strStem;
								for (int i=0; i<nBackSlash; i++)
								{
									strRelative = L"..\\" + strRelative;
									CString strFullPath = strRoot;
									PathAppend(strFullPath.GetBuffer(MAX_PATH), strRelative);
									strFullPath.ReleaseBuffer();
									if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(strFullPath))
									{
										swprintf_s(szTempCache, MAX_PATH, L"%s", strFullPath);
										if (szCache)
										{
											delete []szCache;
											szCache = new TCHAR[MAX_PATH*3];
											swprintf_s(szCache, MAX_PATH, L"%s", strRelative);
										}
										break;
									}
								}
							}
						}
					}
					
				}
				else
				{
					_stprintf(szTempCache, _T("%s"), szCache);
				}
				
				if (_T(':') == szTempCache[1] && INVALID_FILE_ATTRIBUTES != GetFileAttributes(szTempCache))
				{
					int nTempCacheLen = _tcsclen(szTempCache);
					int i=nTempCacheLen-1;
					for (; i>=0; i--)
					{
						if (_T('\\') == szTempCache[i] || _T('\/') == szTempCache[i])
						{
							++i;
							break;
						}
					}
					if (0 > i)
						return;
					TCHAR* szFileName = &(szTempCache[i]);
					m_bFile = true;
					if (NULL != m_szCache)
					{
						delete []m_szCache;
						m_szCache = NULL;
					}
					m_szCache = szCache;
					////detect image
					//for (i=length-1; i>=0; i--)
					//{
					//	if (_T('.') == szCache[i])
					//	{
					//		++i;
					//		break;
					//	}
					//}
					if (NULL == m_pBitmap)
					{
						CString fileDirectory = szCache;
						int dot = fileDirectory.ReverseFind(_T('.'));
						CString ext = fileDirectory;
						ext.Delete(0, dot+1);
						ext.MakeLower();
						
						if (0==ext.Compare(_T("png"))||
							0==ext.Compare(_T("jpeg")) ||
							0==ext.Compare(_T("jpg")) ||
							0==ext.Compare(_T("gif")) ||
							0==ext.Compare(_T("tiff")) ||
							0==ext.Compare(_T("bmp")))
						{
							Image tempImage(szTempCache);
							//if (tempImage.GetLastStatus() != Ok)
							if (tempImage.GetLastStatus() != Ok)
							{
							}
							else
							{
								pserv->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
								CSize sz(0,0);
								if (TEXT_BLOCK_NARROW == m_nExpandState)
								{
									if (tempImage.GetHeight()>g_config.nSmallThumbnailHeight)
									{
										sz.cy = g_config.nSmallThumbnailHeight;
										sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
									}
									else
									{
										sz.cx = tempImage.GetWidth();
										sz.cy = tempImage.GetHeight();
									}
								}
								else if (TEXT_BLOCK_WIDE == m_nExpandState)
								{
									if (tempImage.GetHeight()>g_config.nBigThumbnailHeight)
									{
										sz.cy = g_config.nBigThumbnailHeight;
										sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
									}
									else
									{
										sz.cx = tempImage.GetWidth();
										sz.cy = tempImage.GetHeight();
									}
								}
								//CSize sz(g_config.szSmallThumbnail.cx, (tempImage.GetHeight()*g_config.szSmallThumbnail.cx)/tempImage.GetWidth());
								m_pBitmap = new Bitmap(sz.cx, sz.cy);
								Graphics imageGraphics(m_pBitmap);
								imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
								////m_pBitmap = tempImage.GetThumbnailImage(g_config.szBigThumbnail.cx, (tempImage.GetHeight()*g_config.szBigThumbnail.cx)/tempImage.GetWidth());
								////rcClient.left = pcs->x;
								////rcClient.top = pcs->y;
								//rcClient.right = rcClient.left + sz.cx;
								//rcClient.bottom = rcClient.top + sz.cy;
								//SetClientRect(&rcClient);
							}
						}
					}
					if (NULL == m_pBitmap)
					{
						//Remove Extension
						CString strFileNameWithoutExtension = szFileName;
						TCHAR* pFileName = strFileNameWithoutExtension.GetBuffer(MAX_PATH);
						PathRemoveExtension(pFileName);
						pserv->TxSendMessage(WM_SETTEXT, 0, (LPARAM)pFileName, 0);
						strFileNameWithoutExtension.ReleaseBuffer();

						m_TextClr = RGB(0,0,255);
						m_BkgrdClr = RGB(255,255,255);
						m_bBold = false;
						
						CHARFORMAT2 cf;
						cf.cbSize = sizeof(CHARFORMAT2);
						pserv->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
						cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_BOLD;
						cf.dwEffects &= ~(CFE_AUTOCOLOR | CFE_AUTOBACKCOLOR);
						cf.crTextColor = m_TextClr;
						cf.crBackColor = m_BkgrdClr;
						cf.dwEffects |= CFE_UNDERLINE;
						cf.dwEffects &=~CFE_BOLD;
						pserv->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);

// 						CHARFORMAT cf;
// 						cf.cbSize = sizeof(CHARFORMAT);
// 						pserv->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
// 						
// 						cf.crTextColor = RGB(0,0,255);
// 						TxWindowProc(hwnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
						SetReadOnly(TRUE);
					}
					//Convert to Relative Path
					CString filePath;
					if (L'\\' ==m_szCache[0])
					{
						filePath = g_config.szRootStorageDir;
						filePath += m_szCache;
					}
					else
					{
						filePath = m_szCache;
					}
					CString* pfileDirectory = (CString *)((CWindow*)m_pParentWindow->SendMessage(WM_USER_GET_FILE_DIR, 0, 0));
					CString strRelativePath;
					PathRelativePathTo(strRelativePath.GetBuffer(MAX_PATH),
						*pfileDirectory,
						FILE_ATTRIBUTE_NORMAL,
						filePath,
						FILE_ATTRIBUTE_NORMAL);
					strRelativePath.ReleaseBuffer();
					if (!strRelativePath.IsEmpty())
					{
						filePath = strRelativePath;
					}
					delete [] m_szCache;
					m_szCache = new TCHAR[filePath.GetLength()+100];
					_stprintf(m_szCache, _T("%s"), filePath.GetBuffer(MAX_PATH));
					filePath.ReleaseBuffer();
				}
				else
				{
					m_bFile = false;
					delete []szCache;
				}
			}
			else
			{
				m_bFile = false;
			}
			SysFreeString(bstrCache);
		}
		else
		{
			m_bFile = false;
		}
	}

	if (m_bFile && NULL == m_pBitmap)
	{
		SHFILEINFO sfi;
		ZeroMemory(&sfi, sizeof(SHFILEINFO));
		if (_T('\\') == m_szCache[0])
		{
			CString tmpPath = g_config.szRootStorageDir;
			tmpPath += m_szCache;
			WCHAR szTempPath[MAX_PATH] = L"";
			PathCanonicalize(szTempPath, tmpPath.GetBuffer(MAX_PATH));
			tmpPath.ReleaseBuffer();
			SHGetFileInfo(szTempPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON);
			
		}
		else if (L'.' == m_szCache[0])
		{
			CString* pfileDirectory = (CString *)((CWindow*)m_pParentWindow->SendMessage(WM_USER_GET_FILE_DIR, 0, 0));
			if (NULL != pfileDirectory && !pfileDirectory->IsEmpty())
			{
				CString strRealPath;
				CString strBooPath = *pfileDirectory;
				PathRemoveFileSpec(strBooPath.GetBuffer(MAX_PATH));
				strBooPath.ReleaseBuffer();
				PathCombine(strRealPath.GetBuffer(MAX_PATH), strBooPath.GetBuffer(MAX_PATH), m_szCache);
				strBooPath.ReleaseBuffer();
				strRealPath.ReleaseBuffer();

				if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(strRealPath))
				{
					SHGetFileInfo(strRealPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON);
				}
				
			}
		}
		else
		{
			SHGetFileInfo(m_szCache, 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON);
		}
		if (m_hFileIcon)
		{
			DestroyIcon(m_hFileIcon);
			m_hFileIcon = NULL;
		}
		m_hFileIcon = sfi.hIcon;
	}
	else
	{
		m_hFileIcon = NULL;
	}
}
	

///////////////////////////////  Keyboard Messages  //////////////////////////////////


LRESULT CBooguNoteText::OnKeyDown(WORD vkey, DWORD dwFlags)
{
	/*TCHAR str[1024];
	_stprintf(str, _T("OnKeyDown: %x"), vkey);
	OutputDebugString(str);*/
	switch(vkey)
	{
	case VK_ESCAPE:
		if(fInDialogBox)
		{
			PostMessage(hwndParent, WM_CLOSE, 0, 0);
			return 0;
		}
		break;
	
	case VK_RETURN:
		if(fInDialogBox && !(GetKeyState(VK_CONTROL) & 0x8000) 
				&& !(dwStyle & ES_WANTRETURN))
		{
			// send to default button
			LRESULT id;
			HWND hwndT;

			id = SendMessage(hwndParent, DM_GETDEFID, 0, 0);
			if(LOWORD(id) &&
				(hwndT = GetDlgItem(hwndParent, LOWORD(id))))
			{
				SendMessage(hwndParent, WM_NEXTDLGCTL, (WPARAM) hwndT, (LPARAM) 1);
				if(GetFocus() != hwnd)
					PostMessage(hwndT, WM_KEYDOWN, (WPARAM) VK_RETURN, 0);
			}
			return 0;
		}
		break;

	case VK_TAB:
		if(fInDialogBox) 
		{
			SendMessage(hwndParent, WM_NEXTDLGCTL, 
							!!(GetKeyState(VK_SHIFT) & 0x8000), 0);
			return 0;
		}
		break;
	}
	return 1;
}

#define CTRL(_ch) (_ch - 'A' + 1)

LRESULT CBooguNoteText::OnChar(WORD vkey, DWORD dwFlags)
{
	/*TCHAR str[1024];
	_stprintf(str, _T("OnChar: %x"), vkey);
	OutputDebugString(str);*/
	switch(vkey)
	{
	// Ctrl-Return generates Ctrl-J (LF), treat it as an ordinary return
	case CTRL('J'):
	case VK_RETURN:
		if(fInDialogBox && !(GetKeyState(VK_CONTROL) & 0x8000)
				 && !(dwStyle & ES_WANTRETURN))
			return 0;
		break;

	case VK_TAB:
		if(fInDialogBox && !(GetKeyState(VK_CONTROL) & 0x8000))
			return 0;
	}
	
	return 1;
}


////////////////////////////////////  View rectangle //////////////////////////////////////


void CBooguNoteText::OnGetRect(LPRECT prc)
{
    RECT rcInset;

	// Get view inset (in HIMETRIC)
    TxGetViewInset(&rcInset);

	// Convert the himetric inset to pixels
	rcInset.left = HimetricXtoDX(rcInset.left, xPerInch);
	rcInset.top = HimetricYtoDY(rcInset.top , yPerInch);
	rcInset.right = HimetricXtoDX(rcInset.right, xPerInch);
	rcInset.bottom = HimetricYtoDY(rcInset.bottom, yPerInch);
    
	// Get client rect in pixels
    TxGetClientRect(prc);

	// Modify the client rect by the inset 
    prc->left += rcInset.left;
    prc->top += rcInset.top;
    prc->right -= rcInset.right;
    prc->bottom -= rcInset.bottom;
}

void CBooguNoteText::OnSetRect(LPRECT prc)
{
	RECT rcClient;
	
	if(!prc)
	{
		SetDefaultInset();
	}	
	else	
    {
    	// For screen display, the following intersects new view RECT
    	// with adjusted client area RECT
    	TxGetClientRect(&rcClient);

        // Adjust client rect
        // Factors in space for borders
        if(fBorder)
        {																					  
    	    rcClient.top		+= yHeightSys / 4;
    	    rcClient.bottom 	-= yHeightSys / 4 - 1;
    	    rcClient.left		+= xWidthSys / 2;
    	    rcClient.right	-= xWidthSys / 2;
        }
	
        // Ensure we have minimum width and height
        rcClient.right = max(rcClient.right, rcClient.left + xWidthSys);
        rcClient.bottom = max(rcClient.bottom, rcClient.top + yHeightSys);

        // Intersect the new view rectangle with the 
        // adjusted client area rectangle
        if(!IntersectRect(&rcViewInset, &rcClient, prc))
    	    rcViewInset = rcClient;

        // compute inset in pixels
        rcViewInset.left -= rcClient.left;
        rcViewInset.top -= rcClient.top;
        rcViewInset.right = rcClient.right - rcViewInset.right;
        rcViewInset.bottom = rcClient.bottom - rcViewInset.bottom;

		// Convert the inset to himetric that must be returned to ITextServices
        rcViewInset.left = DXtoHimetricX(rcViewInset.left, xPerInch);
        rcViewInset.top = DYtoHimetricY(rcViewInset.top, yPerInch);
        rcViewInset.right = DXtoHimetricX(rcViewInset.right, xPerInch);
        rcViewInset.bottom = DYtoHimetricY(rcViewInset.bottom, yPerInch);
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, 
    	TXTBIT_VIEWINSETCHANGE);
}



////////////////////////////////////  System notifications  //////////////////////////////////


void CBooguNoteText::OnSysColorChange()
{
	crAuto = GetSysColor(COLOR_WINDOWTEXT);
	if(!fNotSysBkgnd)
		crBackground = GetSysColor(COLOR_WINDOW);
	TxInvalidateRect(NULL, TRUE);
}

LRESULT CBooguNoteText::OnGetDlgCode(WPARAM wparam, LPARAM lparam)
{
	LRESULT lres = DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTTAB;

	if(dwStyle & ES_MULTILINE)
		lres |= DLGC_WANTALLKEYS;

	if(!(dwStyle & ES_SAVESEL))
		lres |= DLGC_HASSETSEL;

	if(lparam)
		fInDialogBox = TRUE;

	if(lparam &&
		((WORD) wparam == VK_BACK))
	{
		lres |= DLGC_WANTMESSAGE;
	}

	return lres;
}


/////////////////////////////////  Other messages  //////////////////////////////////////


LRESULT CBooguNoteText::OnGetOptions() const
{
	LRESULT lres = (dwStyle & ECO_STYLES);

	if(fEnableAutoWordSel)
		lres |= ECO_AUTOWORDSELECTION;
	
	return lres;
}

void CBooguNoteText::OnSetOptions(WORD wOp, DWORD eco)
{
	const BOOL fAutoWordSel = !!(eco & ECO_AUTOWORDSELECTION);
	DWORD dwStyleNew = dwStyle;
	DWORD dw_Style = 0 ;

	DWORD dwChangeMask = 0;

	// single line controls can't have a selection bar
	// or do vertical writing
	if(!(dw_Style & ES_MULTILINE))
	{
#ifdef DBCS
		eco &= ~(ECO_SELECTIONBAR | ECO_VERTICAL);
#else
		eco &= ~ECO_SELECTIONBAR;
#endif
	}
	dw_Style = (eco & ECO_STYLES);

	switch(wOp)
	{
	case ECOOP_SET:
		dwStyleNew = ((dwStyleNew) & ~ECO_STYLES) | dwStyle;
		fEnableAutoWordSel = fAutoWordSel;
		break;

	case ECOOP_OR:
		dwStyleNew |= dw_Style;
		if(fAutoWordSel)
			fEnableAutoWordSel = TRUE;
		break;

	case ECOOP_AND:
		dwStyleNew &= (dw_Style | ~ECO_STYLES);
		if(fEnableAutoWordSel && !fAutoWordSel)
			fEnableAutoWordSel = FALSE;
		break;

	case ECOOP_XOR:
		dwStyleNew ^= dw_Style;
		fEnableAutoWordSel = (!fEnableAutoWordSel != !fAutoWordSel);
		break;
	}

	if(fEnableAutoWordSel != (unsigned)fAutoWordSel)
	{
		dwChangeMask |= TXTBIT_AUTOWORDSEL; 
	}

	if(dwStyleNew != dw_Style)
	{
		DWORD dwChange = dwStyleNew ^ dw_Style;
#ifdef DBCS
		USHORT	usMode;
#endif

		dwStyle = dwStyleNew;
		SetWindowLong(hwnd, GWL_STYLE, dwStyleNew);

		if(dwChange & ES_NOHIDESEL)	
		{
			dwChangeMask |= TXTBIT_HIDESELECTION;
		}

		if(dwChange & ES_READONLY)
		{
			dwChangeMask |= TXTBIT_READONLY;

			// Change drop target state as appropriate.
			//if (dwStyleNew & ES_READONLY)
			//{
			//	RevokeDragDrop();
			//}
			//else
			//{
			//	RegisterDragDrop();
			//}
		}

		if(dwChange & ES_VERTICAL)
		{
			dwChangeMask |= TXTBIT_VERTICAL;
		}

		// no action require for ES_WANTRETURN
		// no action require for ES_SAVESEL
		// do this last
		if(dwChange & ES_SELECTIONBAR)
		{
			lSelBarWidth = 212;
			dwChangeMask |= TXTBIT_SELBARCHANGE;
		}
	}

	if (dwChangeMask)
	{
		DWORD dwProp = 0;
		TxGetPropertyBits(dwChangeMask, &dwProp);
		pserv->OnTxPropertyBitsChange(dwChangeMask, dwProp);
	}
}

void CBooguNoteText::OnSetReadOnly(BOOL fReadOnly)
{
	DWORD dwUpdatedBits = 0;

	if(fReadOnly)
	{
		dwStyle |= ES_READONLY;

		// Turn off Drag Drop 
		//RevokeDragDrop();
		dwUpdatedBits |= TXTBIT_READONLY;
	}
	else
	{
		dwStyle &= ~(DWORD) ES_READONLY;

		// Turn drag drop back on
		//RegisterDragDrop();	
	}

	pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, dwUpdatedBits);
}

void CBooguNoteText::OnSetEventMask(DWORD mask)
{
	LRESULT lres = (LRESULT) dwEventMask;
	dwEventMask = (DWORD) mask;

}


LRESULT CBooguNoteText::OnGetEventMask() const
{
	return (LRESULT) dwEventMask;
}

/*
 *	CBooguNoteText::OnSetFont(hfont)
 *
 *	Purpose:	
 *		Set new font from hfont
 *
 *	Arguments:
 *		hfont	new font (NULL for system font)
 */
BOOL CBooguNoteText::OnSetFont(HFONT hfont)
{
	if(SUCCEEDED(InitDefaultCharFormat(&cf, hfont)))
	{
		/*pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);*/
		//pserv->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
		return TRUE;
	}

	return FALSE;
}

/*
 *	CBooguNoteText::OnSetCharFormat(pcf)
 *
 *	Purpose:	
 *		Set new default CharFormat
 *
 *	Arguments:
 *		pch		ptr to new CHARFORMAT
 */
BOOL CBooguNoteText::OnSetCharFormat(CHARFORMAT *pcf)
{
	cf = *pcf;
	pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);

	return TRUE;
}

/*
 *	CBooguNoteText::OnSetParaFormat(ppf)
 *
 *	Purpose:	
 *		Set new default ParaFormat
 *
 *	Arguments:
 *		pch		ptr to new PARAFORMAT
 */
BOOL CBooguNoteText::OnSetParaFormat(PARAFORMAT *pPF)
{
	pf = *(PARAFORMAT2 *)pPF;									// Copy it

	pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 
		TXTBIT_PARAFORMATCHANGE);

	return TRUE;
}



////////////////////////////  Event firing  /////////////////////////////////



void * CBooguNoteText::CreateNmhdr(UINT uiCode, LONG cb)
{
	NMHDR *pnmhdr;

	pnmhdr = (NMHDR*) new char[cb];
	if(!pnmhdr)
		return NULL;

	memset(pnmhdr, 0, cb);

	pnmhdr->hwndFrom = hwnd;
	pnmhdr->idFrom = GetWindowLong(hwnd, GWL_ID);
	pnmhdr->code = uiCode;

	return (VOID *) pnmhdr;
}


////////////////////////////////////  Helpers  /////////////////////////////////////////
void CBooguNoteText::SetDefaultInset()
{
    // Generate default view rect from client rect.
    if(fBorder)
    {
        // Factors in space for borders
  	    rcViewInset.top = DYtoHimetricY(yHeightSys / 4, yPerInch);
   	 rcViewInset.bottom	= DYtoHimetricY(yHeightSys / 4 - 1, yPerInch);
   	 rcViewInset.left = DXtoHimetricX(xWidthSys / 2, xPerInch);
   	 rcViewInset.right = DXtoHimetricX(xWidthSys / 2, xPerInch);
    }
    else
    {
		rcViewInset.top = rcViewInset.left =
		rcViewInset.bottom = rcViewInset.right = 0;
	}
}


/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC CBooguNoteText::TxImmGetContext(void)
{
	HIMC himc;
	
	if (0!=imeEnabled)
	{
		himc = ImmGetContext( hwnd );
	}
	else
	{
		himc = NULL;
	}

	return himc;

}

void CBooguNoteText::TxImmReleaseContext(HIMC himc)
{
	ImmReleaseContext( hwnd, himc );
}

void CBooguNoteText::RevokeDragDrop(void)
{
	if (fRegisteredForDrop)
	{
		::RevokeDragDrop(hwnd);
		fRegisteredForDrop = FALSE;
	}
}

void CBooguNoteText::RegisterDragDrop(void)
{
	IDropTarget *pdt;

	if(!fRegisteredForDrop && pserv->TxGetDropTarget(&pdt) == NOERROR)
	{
		HRESULT hr = ::RegisterDragDrop(hwnd, pdt);

		if(hr == NOERROR)
		{	
			fRegisteredForDrop = TRUE;
		}

		pdt->Release();
	}
}

VOID DrawRectFn(
	HDC hdc, 
	RECT *prc, 
	INT icrTL, 
	INT icrBR,
	BOOL fBot, 
	BOOL fRght)
{
	COLORREF cr;
	COLORREF crSave;
	RECT rc;

	cr = GetSysColor(icrTL);
	crSave = SetBkColor(hdc, cr);

	// top
	rc = *prc;
	rc.bottom = rc.top + 1;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

	// left
	rc.bottom = prc->bottom;
	rc.right = rc.left + 1;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

	if(icrTL != icrBR)
	{
		cr = GetSysColor(icrBR);
		SetBkColor(hdc, cr);
	}

	// right
	rc.right = prc->right;
	rc.left = rc.right - 1;
	if(!fBot)
		rc.bottom -= cyHScroll;
	if(fRght)
	{
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}

	// bottom
	if(fBot)
	{
		rc.left = prc->left;
		rc.top = rc.bottom - 1;
		if(!fRght)
			rc.right -= cxVScroll;
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}
	SetBkColor(hdc, crSave);
}

#define cmultBorder 1

VOID CBooguNoteText::OnSunkenWindowPosChanging(HWND hwnd, WINDOWPOS *pwndpos)
{
	if(fVisible)
	{
		RECT rc;
		HWND hwndParent;

		GetWindowRect(hwnd, &rc);
		InflateRect(&rc, cxBorder * cmultBorder, cyBorder * cmultBorder);
		hwndParent = GetParent(hwnd);
		MapWindowPoints(HWND_DESKTOP, hwndParent, (POINT *) &rc, 2);
		InvalidateRect(hwndParent, &rc, FALSE);
	}
}


VOID CBooguNoteText::DrawSunkenBorder(HWND hwnd, HDC hdc)
{
	RECT rc;
	RECT rcParent;
	DWORD dwScrollBars;
	HWND hwndParent;

	GetWindowRect(hwnd, &rc);
    hwndParent = GetParent(hwnd);
	rcParent = rc;
	MapWindowPoints(HWND_DESKTOP, hwndParent, (POINT *)&rcParent, 2);
	InflateRect(&rcParent, cxBorder, cyBorder);
	OffsetRect(&rc, -rc.left, -rc.top);

	// draw inner rect
	TxGetScrollBars(&dwScrollBars);
	DrawRectFn(hdc, &rc, COLOR_WINDOWFRAME, COLOR_BTNFACE,
		!(dwScrollBars & WS_HSCROLL), !(dwScrollBars & WS_VSCROLL));

	// draw outer rect
	hwndParent = GetParent(hwnd);
	hdc = GetDC(hwndParent);
	DrawRectFn(hdc, &rcParent, COLOR_BTNSHADOW, COLOR_BTNHIGHLIGHT,
		TRUE, TRUE);
	ReleaseDC(hwndParent, hdc);
}

LRESULT CBooguNoteText::OnSize(HWND hwnd, WORD fwSizeType, int nWidth, int nHeight)
{
	// Update our client rectangle
	rcClient.right = rcClient.left + nWidth;
	rcClient.bottom = rcClient.top + nHeight;

	if(!fVisible)
	{
		fIconic = GetIconic(hwnd);
		if(!fIconic)
			fResized = TRUE;
	}
	else
	{
		if(GetIconic(hwnd))
		{
			fIconic = TRUE;
		}
		else
		{
			pserv->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, 
				TXTBIT_CLIENTRECTCHANGE);

			if(fIconic)
			{
				InvalidateRect(hwnd, NULL, FALSE);
				fIconic = FALSE;
			}
			
			if(TxGetEffects() == TXTEFFECT_SUNKEN)	// Draw borders
				DrawSunkenBorder(hwnd, NULL);
		}
	}
	return 0;
}

HRESULT CBooguNoteText::OnTxVisibleChange(BOOL fVisible)
{
	fVisible = fVisible;

	if(!fVisible && fResized)
	{
		RECT rc;
		// Control was resized while hidden,
		// need to really resize now
		TxGetClientRect(&rc);
		fResized = FALSE;
		pserv->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, 
			TXTBIT_CLIENTRECTCHANGE);
	}

	return S_OK;
}



//////////////////////////// ITextHost Interface  ////////////////////////////

HDC CBooguNoteText::TxGetDC()
{
	return ::GetDC(hwnd);
}


int CBooguNoteText::TxReleaseDC(HDC hdc)
{
	return ::ReleaseDC (hwnd, hdc);
}


BOOL CBooguNoteText::TxShowScrollBar(INT fnBar,	BOOL fShow)
{
	return ::ShowScrollBar(hwnd, fnBar, fShow);
}

BOOL CBooguNoteText::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
	return ::EnableScrollBar(hwnd, fuSBFlags, fuArrowflags) ;//SB_HORZ, ESB_DISABLE_BOTH);
}


BOOL CBooguNoteText::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	return ::SetScrollRange(hwnd, fnBar, nMinPos, nMaxPos, fRedraw);
}


BOOL CBooguNoteText::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
{
	return ::SetScrollPos(hwnd, fnBar, nPos, fRedraw);
}

void CBooguNoteText::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	if (prc!=NULL)
	{
		RECT rc;
		rc.left = prc->left - m_pptOffset->x;
		rc.right = prc->right - m_pptOffset->x;
		rc.top = prc->top - m_pptOffset->y;
		rc.bottom = prc->bottom - m_pptOffset->y;
		::InvalidateRect(hwnd, &rc, fMode);
	}
	else
	{
		::InvalidateRect(hwnd, prc, fMode);
	}
}

void CBooguNoteText::TxViewChange(BOOL fUpdate) 
{
	::UpdateWindow (hwnd);
}


BOOL CBooguNoteText::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	return ::CreateCaret (hwnd, hbmp, xWidth, yHeight);
}


BOOL CBooguNoteText::TxShowCaret(BOOL fShow)
{
	if(fShow)
		return ::ShowCaret(hwnd);
	else
		return ::HideCaret(hwnd);
}

BOOL CBooguNoteText::TxSetCaretPos(INT x, INT y)
{
	/*POINT pt;
	pt.x=x;
	pt.y=y;*/
	////GetWindowOrgEx(TxGetDC(), &org);
	//TCHAR str[1024];
	//_stprintf(str, _T("TxSetCaretPos: x:%d; y:%d; before"), pt.x, pt.y);
	//OutputDebugString(str);
	//DPtoLP(TxGetDC(), &pt, 1);
	//_stprintf(str, _T("TxSetCaretPos: x:%d; y:%d; after"), pt.x, pt.y);
	//OutputDebugString(str);
	return ::SetCaretPos(x-m_pptOffset->x, y-m_pptOffset->y);
	//return ::SetCaretPos(x, y);
}


BOOL CBooguNoteText::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	fTimer = TRUE;
	return ::SetTimer(hwnd, idTimer, uTimeout, NULL);
}


void CBooguNoteText::TxKillTimer(UINT idTimer)
{
	::KillTimer(hwnd, idTimer);
	fTimer = FALSE;
}

void CBooguNoteText::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
{
	::ScrollWindowEx(hwnd, dx, dy, lprcScroll, lprcClip, hrgnUpdate, lprcUpdate, fuScroll);
}

void CBooguNoteText::TxSetCapture(BOOL fCapture)
{
	if (fCapture)
		::SetCapture(hwnd);
	else
		::ReleaseCapture();
}

void CBooguNoteText::TxSetFocus()
{
	if (NULL == m_pBitmap)
	{
		::SetFocus(hwnd);
	}
	else
	{
		::HideCaret(hwnd);
	}
}

void CBooguNoteText::TxSetCursor(HCURSOR hcur,	BOOL fText)
{
	::SetCursor(hcur);
}

BOOL CBooguNoteText::TxScreenToClient(LPPOINT lppt)
{
	return ::ScreenToClient(hwnd, lppt);	
}

BOOL CBooguNoteText::TxClientToScreen(LPPOINT lppt)
{
	return ::ClientToScreen(hwnd, lppt);
}

HRESULT CBooguNoteText::TxActivate(LONG *plOldState)
{
    return S_OK;
}

HRESULT CBooguNoteText::TxDeactivate(LONG lNewState)
{
    return S_OK;
}
    

HRESULT CBooguNoteText::TxGetClientRect(LPRECT prc)
{
	GetControlRect(prc);

	return NOERROR;
}


HRESULT CBooguNoteText::TxGetViewInset(LPRECT prc) 
{

    *prc = rcViewInset;
    
    return NOERROR;	
}

HRESULT CBooguNoteText::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	*ppCF = &cf;
	return NOERROR;
}

HRESULT CBooguNoteText::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	*ppPF = (PARAFORMAT*)&pf;
	return NOERROR;
}


COLORREF CBooguNoteText::TxGetSysColor(int nIndex) 
{
	if (nIndex == COLOR_WINDOW)
	{
		if(!fNotSysBkgnd)
			return GetSysColor(COLOR_WINDOW);
		return crBackground;
	}

	return GetSysColor(nIndex);
}



HRESULT CBooguNoteText::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	*pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
	//*pstyle = TXTBACK_TRANSPARENT;
	return NOERROR;
}


HRESULT CBooguNoteText::TxGetMaxLength(DWORD *pLength)
{
	*pLength = cchTextMost;
	return NOERROR;
}



HRESULT CBooguNoteText::TxGetScrollBars(DWORD *pdwScrollBar)
{
	*pdwScrollBar =  dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
						ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

	return NOERROR;
}


HRESULT CBooguNoteText::TxGetPasswordChar(TCHAR *pch)
{
#ifdef UNICODE
	*pch = chPasswordChar;
#else
	WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
	return NOERROR;
}

HRESULT CBooguNoteText::TxGetAcceleratorPos(LONG *pcp)
{
	*pcp = laccelpos;
	return S_OK;
} 										   

HRESULT CBooguNoteText::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
	return S_OK;
}


HRESULT CBooguNoteText::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
	pf = *(PARAFORMAT2 *)ppf;
	return S_OK;
}


HRESULT CBooguNoteText::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
	DWORD dwProperties = 0;

	if (fRich)
	{
		dwProperties = TXTBIT_RICHTEXT;
	}

	if (dwStyle & ES_MULTILINE)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (dwStyle & ES_READONLY)
	{
		dwProperties |= TXTBIT_READONLY;
	}


	if (dwStyle & ES_PASSWORD)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (!(dwStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

	if (fEnableAutoWordSel)
	{
		dwProperties |= TXTBIT_AUTOWORDSEL;
	}

	if (fVertical)
	{
		dwProperties |= TXTBIT_VERTICAL;
	}
					
	if (fWordWrap)
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}

	if (fAllowBeep)
	{
		dwProperties |= TXTBIT_ALLOWBEEP;
	}

	if (fSaveSelection)
	{
		dwProperties |= TXTBIT_SAVESELECTION;
	}

	*pdwBits = dwProperties & dwMask; 
	return NOERROR;
}


HRESULT CBooguNoteText::TxNotify(DWORD iNotify, void *pv)
{
	if( iNotify == EN_REQUESTRESIZE )
	{
		RECT rc;
		REQRESIZE *preqsz = (REQRESIZE *)pv;

		GetControlRect(&rc);
		//rc = *GetClientRect();
		if ((rc.bottom != rc.top + preqsz->rc.bottom) || (rc.right != rc.left + preqsz->rc.right))
		{
			RECT invalidateRect;
			if ((preqsz->rc.bottom - preqsz->rc.top)>(rc.bottom-rc.top) ||(preqsz->rc.right - preqsz->rc.left)>(rc.right-rc.left))
			{
				invalidateRect.bottom = rc.top + preqsz->rc.bottom + g_config.hostBorder;
				invalidateRect.right = rc.left + preqsz->rc.right + g_config.hostBorder;
				invalidateRect.top =  rc.top - g_config.hostBorder;
				invalidateRect.left = rc.left - g_config.hostBorder;
			}
			else
			{
				invalidateRect = rc;
				invalidateRect.top -= g_config.hostBorder;
				invalidateRect.left -= g_config.hostBorder;
				invalidateRect.bottom += g_config.hostBorder;
				invalidateRect.right += g_config.hostBorder;
			}

			rc.bottom = rc.top + preqsz->rc.bottom + g_config.hostBorder;
			rc.right  = rc.left + preqsz->rc.right + g_config.hostBorder;
			rc.top -= g_config.hostBorder;
			rc.left -= g_config.hostBorder;
			SetClientRect(&rc, TRUE);

			invalidateRect.top -= m_pptOffset->y;
			invalidateRect.bottom -= m_pptOffset->y;
			invalidateRect.left -= m_pptOffset->x;
			invalidateRect.right -= m_pptOffset->x;
			InvalidateRect(hwnd, &invalidateRect, TRUE);
		}
		SendMessage(hwnd, WM_USER_REQUESTRESIZE, (WPARAM)this, 0);
		
		return S_OK;
	}
	else if (iNotify == EN_LINK)
	{
		ENLINK* enLink = (ENLINK*)pv;
		if ((WM_LBUTTONDOWN == enLink->msg)/*&&((GetKeyState(VK_LMENU)<0||GetKeyState(VK_RMENU)<0))*/)
		{
			TCHAR link[65536];
			TEXTRANGE txRange;
			txRange.chrg = enLink->chrg;
			txRange.lpstrText = link;
			(GetTextServices())->TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&txRange,0);
			//GetTextRange(enLink->chrg.cpMin, enLink->chrg.cpMax, link);
			ShellExecute(NULL, _T("open"), link, 0, 0, SW_SHOWNORMAL);
		}
	}

	// Forward this to the container
	if (pnc)
	{
		(*pnc)(iNotify);
	}

	return S_OK;
}



HRESULT CBooguNoteText::TxGetExtent(LPSIZEL lpExtent)
{

	// Calculate the length & convert to himetric
	*lpExtent = sizelExtent;

	return S_OK;
}

HRESULT	CBooguNoteText::TxGetSelectionBarWidth (LONG *plSelBarWidth)
{
	*plSelBarWidth = lSelBarWidth;
	return S_OK;
}


BOOL CBooguNoteText::GetReadOnly()
{
	return (dwStyle & ES_READONLY) != 0;
}

void CBooguNoteText::SetReadOnly(BOOL fReadOnly)
{
	if (fReadOnly)
	{
		dwStyle |= ES_READONLY;
	}
	else
	{
		dwStyle &= ~ES_READONLY;
	}

	// Notify control of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, 
		fReadOnly ? TXTBIT_READONLY : 0);
}

BOOL CBooguNoteText::GetAllowBeep()
{
	return fAllowBeep;
}

void CBooguNoteText::SetAllowBeep(BOOL fAllowBeep)
{
	fAllowBeep = fAllowBeep;

	// Notify control of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
		fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

void CBooguNoteText::SetViewInset(RECT *prc)
{
	rcViewInset = *prc;

	// Notify control of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, 0);
}

WORD CBooguNoteText::GetDefaultAlign()
{
	return pf.wAlignment;
}


void CBooguNoteText::SetDefaultAlign(WORD wNewAlign)
{
	pf.wAlignment = wNewAlign;

	// Notify control of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL CBooguNoteText::GetRichTextFlag()
{
	return fRich;
}

void CBooguNoteText::SetRichTextFlag(BOOL fNew)
{
	fRich = fNew;

	// Notify control of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
		fNew ? TXTBIT_RICHTEXT : 0);
}

LONG CBooguNoteText::GetDefaultLeftIndent()
{
	return pf.dxOffset;
}


void CBooguNoteText::SetDefaultLeftIndent(LONG lNewIndent)
{
	pf.dxOffset = lNewIndent;

	// Notify control of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void CBooguNoteText::SetClientRect(RECT *prc, BOOL fUpdateExtent) 
{
	// If the extent matches the client rect then we assume the extent should follow
	// the client rect.
	LONG lTestExt = DYtoHimetricY(
		(rcClient.bottom - rcClient.top)  - 2 * g_config.hostBorder, yPerInch);

	if (fUpdateExtent 
		&& (sizelExtent.cy == lTestExt))
	{
		sizelExtent.cy = DXtoHimetricX((prc->bottom - prc->top) - 2 * g_config.hostBorder, 
			xPerInch);
		sizelExtent.cx = DYtoHimetricY((prc->right - prc->left) - 2 * g_config.hostBorder,
			yPerInch);
	}

	rcClient = *prc; 
}

BOOL CBooguNoteText::SetSaveSelection(BOOL f_SaveSelection)
{
	BOOL fResult = f_SaveSelection;

	fSaveSelection = f_SaveSelection;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
		fSaveSelection ? TXTBIT_SAVESELECTION : 0);

	return fResult;		
}

HRESULT	CBooguNoteText::OnTxInPlaceDeactivate()
{
	HRESULT hr = pserv->OnTxInPlaceDeactivate();

	if (SUCCEEDED(hr))
	{
		fInplaceActive = FALSE;
	}

	return hr;
}

HRESULT	CBooguNoteText::OnTxInPlaceActivate(LPCRECT prcClient)
{
	fInplaceActive = TRUE;

	HRESULT hr = pserv->OnTxInPlaceActivate(prcClient);

	if (FAILED(hr))
	{
		fInplaceActive = FALSE;
	}

	return hr;
}

BOOL CBooguNoteText::DoSetCursor(RECT *prc, POINT *pt)
{
	RECT rc = prc ? *prc : rcClient;

	// Give some space for our border
	rc.top += g_config.hostBorder;
	rc.bottom -= g_config.hostBorder;
	rc.left += g_config.hostBorder;
	rc.right -= g_config.hostBorder;

	// Is this in our rectangle?
	if (PtInRect(&rc, *pt))
	{
		RECT *prcClient = (!fInplaceActive || prc) ? &rc : NULL;

		HDC hdc = GetDC(hwnd);

		pserv->OnTxSetCursor(
			DVASPECT_CONTENT,	
			-1,
			NULL,
			NULL,
			hdc,
			NULL,
			prcClient,
			pt->x, 
			pt->y);

		ReleaseDC(hwnd, hdc);

		return TRUE;
	}

	return FALSE;
}

void CBooguNoteText::GetControlRect(
	LPRECT prc			//@parm	Where to put client coordinates
)
{
	// Give some space for our border
	prc->top = rcClient.top + g_config.hostBorder;
	prc->bottom = rcClient.bottom - g_config.hostBorder;
	prc->left = rcClient.left + g_config.hostBorder;
	prc->right = rcClient.right - g_config.hostBorder;
}

void CBooguNoteText::SetTransparent(BOOL f_Transparent)
{
	fTransparent = f_Transparent;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG CBooguNoteText::SetAccelPos(LONG l_accelpos)
{
	LONG laccelposOld = l_accelpos;

	laccelpos = l_accelpos;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

	return laccelposOld;
}

WCHAR CBooguNoteText::SetPasswordChar(WCHAR ch_PasswordChar)
{
	WCHAR chOldPasswordChar = chPasswordChar;

	chPasswordChar = ch_PasswordChar;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
		(chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

	return chOldPasswordChar;
}

void CBooguNoteText::SetDisabled(BOOL fOn)
{
	cf.dwMask	  |= CFM_COLOR	   | CFM_DISABLED;
	cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

	if( !fOn )
	{
		cf.dwEffects &= ~CFE_DISABLED;
	}
	
	pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}

LONG CBooguNoteText::SetSelBarWidth(LONG l_SelBarWidth)
{
	LONG lOldSelBarWidth = lSelBarWidth;

	lSelBarWidth = l_SelBarWidth;

	if (lSelBarWidth)
	{
		dwStyle |= ES_SELECTIONBAR;
	}
	else
	{
		dwStyle &= (~ES_SELECTIONBAR);
	}

	pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

	return lOldSelBarWidth;
}

BOOL CBooguNoteText::GetTimerState()
{
	return fTimer;
}

void CBooguNoteText::DoPaint(HDC hdc)
{
	m_pTextHandle->DoPaint(hdc);
	m_pTextIcon->DoPaint(hdc);
	//draw file icon
	if (m_bFile && NULL == m_pBitmap && NULL != m_hFileIcon)
	{
		Bitmap fileIcon(m_hFileIcon);
		Graphics imageGraphics(hdc);
		imageGraphics.DrawImage(&fileIcon, rcClient.left - FILE_ICON_WIDTH, m_pTextHandle->y -3);
	}
	TxWindowProc(m_hHostWindow, WM_PAINT, WPARAM(hdc), 0);
}

void CBooguNoteText::CopyFromTextBlock(const CTextBlock& TB)
{
	m_pTextHandle->state = TB.m_TextHandleState;
	m_nExpandState =TB.m_TextBlockState;
	m_pTextIcon->state = TB.m_TextIcon;
	m_nLevel = TB.m_nTextLevel;
	m_bShown =TB.m_bShown;
	m_bBold = TB.m_bBold;
}

void CBooguNoteText::CopyToTextBlock(CTextBlock& TB)
{
	TB.m_TextHandleState = m_pTextHandle->state;
	TB.m_TextBlockState = m_nExpandState;
	TB.m_TextIcon = m_pTextIcon->state;
	TB.m_nTextLevel = m_nLevel;
	TB.m_bShown = m_bShown;
	TB.m_bBold = m_bBold;
}

void CBooguNoteText::TimeStamp()
{
	CHARRANGE cr = { 0, 0 };
	pserv->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
	TCHAR tmpbuf[128];
	_tzset();
	_wstrtime_s( tmpbuf, 128 );
	CString fm;
	fm = tmpbuf;
	int pos=fm.ReverseFind(_T(':'));
	fm.Delete(pos, fm.GetLength()-pos);
	_tstrdate_s( tmpbuf, 128 );
	fm = _T(" ") + fm;
	fm = tmpbuf + fm;
	fm = _T("[") + fm;
	fm = fm + _T("]");
	pserv->TxSendMessage(EM_REPLACESEL, (WPARAM) FALSE, (LPARAM)fm.GetBuffer(128), 0);
	fm.ReleaseBuffer();
	//InsertText(cr.cpMin, fm, TRUE);
}