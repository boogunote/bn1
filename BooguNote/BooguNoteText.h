/**************************************************************************
   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   Copyright 2000 Microsoft Corporation.  All Rights Reserved.
**************************************************************************/



/**************************************************************************
 HOST.H  Text Host for Window's Rich Edit Control 
**************************************************************************/


#ifndef _HOST_H_
#define _HOST_H_


#include "textserv.h"
#include <gdiplus.h>
#pragma comment (lib,"gdiplus.lib")
using namespace Gdiplus;

#define FValidCF(_pcf) ((_pcf)->cbSize == sizeof(CHARFORMAT))
#define FValidPF(_ppf) ((_ppf)->cbSize == sizeof(PARAFORMAT2))

typedef void (* PNOTIFY_CALL)(int iNotify);

class CBooguNoteHandle;
class CBooguNoteIcon;

#define LY_PER_INCH   1440

EXTERN_C const IID IID_ITextEditControl;

/**************************************************************************
 	TXTEFFECT
 
 	@enum	Defines different background styles control
**************************************************************************/
enum TXTEFFECT {
	TXTEFFECT_NONE = 0,				//@emem	no special backgoround effect
	TXTEFFECT_SUNKEN,				//@emem	draw a "sunken 3-D" look
};


interface ITextEditControl : public IUnknown
{
	virtual LRESULT	TxWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};

class CBooguNoteView;

class CTextBlock
{
public:
	DWORD m_TextHandleState;
	DWORD m_TextBlockState;
	DWORD m_TextIcon;
	DWORD m_nTextLevel;
	DWORD m_nTextLength;
	bool m_bShown;
	bool m_bBold;
	bool m_bShowBranch;
};

/**************************************************************************
 	CBooguNoteText

 	@class	Text Host for Window's Rich Edit Control implementation class
/**************************************************************************/
#include "BooguNoteWidget.h"
class CBooguNoteText : public ITextHost, public ITextEditControl, public CBooguNoteWidget
{

/**************************************************************************
 Member Functions
**************************************************************************/
private:
	CBooguNoteText(){};
public:

	CBooguNoteText(int _left, int _top, int _right, int _bottom);

	// Initialization
	BOOL Init(HWND h_wnd, POINT* pptOffset, CBooguNoteText** ppFocused, const CREATESTRUCT *pcs, PNOTIFY_CALL p_nc);

	~CBooguNoteText();

	BOOL CBooguNoteText::HitTest(int x, int y);

	ITextServices * GetTextServices(void) { return pserv; }

	void SetClientRect(RECT *prc, BOOL fUpdateExtent = TRUE);

	RECT * GetClientRect() { return &rcClient; }

	void SetExtent(SIZEL *psizelExtent, BOOL fNotify) 
   { 
	   // Set our extent
		sizelExtent = *psizelExtent; 

		// Notify the host that the extent has changed
		if (fNotify)
		{
		   pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
		}
	}

	PNOTIFY_CALL SetNotifyCall(PNOTIFY_CALL p_nc)
	{
	   PNOTIFY_CALL pncRet = pnc;
		pnc = p_nc;
		return pncRet;
	}

	BOOL GetWordWrap(void) { return fWordWrap; }

	void SetWordWrap(BOOL fWordWrap)
	{
	   fWordWrap = fWordWrap;
		pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
   }

	BOOL GetReadOnly();

	void SetReadOnly(BOOL fReadOnly);

	BOOL GetAllowBeep();

	void SetAllowBeep(BOOL fAllowBeep);

	void SetViewInset(RECT *prc);

	WORD GetDefaultAlign();

	void SetDefaultAlign(WORD wNewAlign);

	BOOL GetRichTextFlag();

	void SetRichTextFlag(BOOL fNew);

	LONG GetDefaultLeftIndent();

	void SetDefaultLeftIndent(LONG lNewIndent);

	BOOL SetSaveSelection(BOOL fSaveSelection);

	SIZEL *GetExtent(void) { return &sizelExtent; }

	HRESULT OnTxInPlaceDeactivate();

	HRESULT OnTxInPlaceActivate(LPCRECT prcClient);

	BOOL GetActiveState(void) { return fInplaceActive; }

	BOOL DoSetCursor(RECT *prc, POINT *pt);

	void SetTransparent(BOOL fTransparent);

	void GetControlRect(LPRECT prc);

	LONG SetAccelPos(LONG laccelpos);

	WCHAR SetPasswordChar(WCHAR chPasswordChar);

	void SetDisabled(BOOL fOn);

	LONG SetSelBarWidth(LONG lSelBarWidth);

	BOOL GetTimerState();

	static LONG GetXPerInch(void) { return xPerInch; }

	static LONG	GetYPerInch(void) { return yPerInch; }

	void DoPaint(HDC hdc);

	void CBooguNoteText::CopyFromTextBlock(const CTextBlock& TB);

	void CopyToTextBlock(CTextBlock& TB);


	// -----------------------------
	//	IUnknown interface
	// -----------------------------

   virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
   virtual ULONG _stdcall AddRef(void);
   virtual ULONG _stdcall Release(void);

	// -----------------------------
	//	ITextEditControl interface
	// -----------------------------
	virtual LRESULT TxWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// -----------------------------
	//	ITextHost interface
	// -----------------------------
	//@cmember Get the DC for the host
	virtual HDC TxGetDC();

	//@cmember Release the DC gotten from the host
	virtual INT TxReleaseDC(HDC hdc);
	
	//@cmember Show the scroll bar
	virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);

	//@cmember Enable the scroll bar
	virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);

	//@cmember Set the scroll range
	virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);

	//@cmember Set the scroll position
	virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);

	//@cmember InvalidateRect
	virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);

	//@cmember Send a WM_PAINT to the window
	virtual void TxViewChange(BOOL fUpdate);
	
	//@cmember Create the caret
	virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);

	//@cmember Show the caret
	virtual BOOL TxShowCaret(BOOL fShow);

	//@cmember Set the caret position
	virtual BOOL TxSetCaretPos(INT x, INT y);

	//@cmember Create a timer with the specified timeout
	virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);

	//@cmember Destroy a timer
	virtual void TxKillTimer(UINT idTimer);

	//@cmember Scroll the content of the specified window's client area
	virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
	
	//@cmember Get mouse capture
	virtual void TxSetCapture(BOOL fCapture);

	//@cmember Set the focus to the text window
	virtual void TxSetFocus();

	//@cmember Establish a new cursor shape
	virtual void TxSetCursor(HCURSOR hcur, BOOL fText);

	//@cmember Converts screen coordinates of a specified point to the client coordinates 
	virtual BOOL TxScreenToClient (LPPOINT lppt);

	//@cmember Converts the client coordinates of a specified point to screen coordinates
	virtual BOOL TxClientToScreen (LPPOINT lppt);

	//@cmember Request host to activate text services
	virtual HRESULT TxActivate( LONG * plOldState );

	//@cmember Request host to deactivate text services
   	virtual HRESULT TxDeactivate( LONG lNewState );

	//@cmember Retrieves the coordinates of a window's client area
	virtual HRESULT TxGetClientRect(LPRECT prc);

	//@cmember Get the view rectangle relative to the inset
	virtual HRESULT TxGetViewInset(LPRECT prc);

	//@cmember Get the default character format for the text
	virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );

	//@cmember Get the default paragraph format for the text
	virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);

	//@cmember Get the background color for the window
	virtual COLORREF TxGetSysColor(int nIndex);

	//@cmember Get the background (either opaque or transparent)
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);

	//@cmember Get the maximum length for the text
	virtual HRESULT TxGetMaxLength(DWORD *plength);

	//@cmember Get the bits representing requested scroll bars for the window
	virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);

	//@cmember Get the character to display for password input
	virtual HRESULT TxGetPasswordChar(TCHAR *pch);

	//@cmember Get the accelerator character
	virtual HRESULT TxGetAcceleratorPos(LONG *pcp);

	//@cmember Get the native size
    virtual HRESULT TxGetExtent(LPSIZEL lpExtent);

	//@cmember Notify host that default character format has changed
	virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);

	//@cmember Notify host that default paragraph format has changed
	virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);

	//@cmember Bulk access to bit properties
	virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);

	//@cmember Notify host of events
	virtual HRESULT TxNotify(DWORD iNotify, void *pv);

	//@cmember Get the undo manager for the host

	// FE Support Routines for handling the Input Method Context
	virtual HIMC TxImmGetContext(void);
	virtual void TxImmReleaseContext(HIMC himc);

	//@cmember Returns HIMETRIC size of the control bar.
	virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

public:


	// helpers
	void *CreateNmhdr(UINT uiCode, LONG cb);
	void	RevokeDragDrop(void);
	void	RegisterDragDrop(void);
	void	DrawSunkenBorder(HWND hwnd, HDC hdc);
	VOID  OnSunkenWindowPosChanging(HWND hwnd, WINDOWPOS *pwndpos);
	LRESULT OnSize(HWND hwnd, WORD fwSizeType, int nWidth, int nHeight);
	TXTEFFECT TxGetEffects() const;
	HRESULT	OnTxVisibleChange(BOOL fVisible);
	void	SetDefaultInset();

	// Keyboard messages
	LRESULT	OnKeyDown(WORD vKey, DWORD dwFlags);
	LRESULT	OnChar(WORD vKey, DWORD dwFlags);
	
	// System notifications
	void 	OnSysColorChange();
	LRESULT OnGetDlgCode(WPARAM wparam, LPARAM lparam);

	// Other messages
	LRESULT OnGetOptions() const;
	void	OnSetOptions(WORD wOp, DWORD eco);
	LRESULT	OnGetEventMask() const;
	void    OnSetEventMask(DWORD dwMask);
	void	OnSetReadOnly(BOOL fReadOnly);
	BOOL	OnSetFont(HFONT hFont);
	BOOL	OnSetCharFormat(CHARFORMAT *pcf);
	BOOL	OnSetParaFormat(PARAFORMAT *ppf);
	void	OnGetRect(LPRECT prc);
	void	OnSetRect(LPRECT prc);
	void	TimeStamp();
	void	DetectFileBlock();

//
//	Data Members
//

private:

   static LONG xWidthSys;
   static LONG yHeightSys;
	static LONG xPerInch;
	static LONG yPerInch;

	PNOTIFY_CALL pnc;				// Notify container of the control of changes

	ULONG	cRefs;					// Reference Count

	HWND	hwnd;					// control window
	HWND	hwndParent;			// parent window

	ITextServices	*pserv;		// pointer to Text Services object

// Properties

	DWORD		dwStyle;				// style bits
	DWORD		dwExStyle;				// extended style bits

	unsigned	fBorder			:1;	// control has border
	unsigned	fCustRect			:1;	// client changed format rect
	unsigned	fInBottomless		:1;	// inside bottomless callback
	unsigned	fInDialogBox		:1;	// control is in a dialog box
	unsigned	fEnableAutoWordSel	:1;	// enable Word style auto word selection?
	unsigned	fVertical			:1;	// vertical writing
	unsigned	fIconic			:1;	// control window is iconic
	unsigned	fHidden			:1;	// control window is hidden
	unsigned	fNotSysBkgnd		:1;	// not using system background color
	unsigned	fWindowLocked		:1;	// window is locked (no update)
	unsigned	fRegisteredForDrop	:1; // whether host has registered for drop
	unsigned	fVisible			:1;	// Whether window is visible or not.
	unsigned	fResized			:1;	// resized while hidden
	unsigned	fWordWrap			:1;	// Whether control should word wrap
	unsigned	fAllowBeep			:1;	// Whether beep is allowed
	unsigned	fRich				:1;	// Whether control is rich text
	unsigned	fSaveSelection		:1;	// Whether to save the selection when inactive
	unsigned	fInplaceActive		:1; // Whether control is inplace active
	unsigned	fTransparent		:1; // Whether control is transparent
	unsigned	fTimer				:1;	// A timer is set

	LONG		lSelBarWidth;			// Width of the selection bar

	COLORREF 	crBackground;			// background color
	LONG  		cchTextMost;			// maximum text size
	DWORD		   dwEventMask;			// Event mask to pass on to parent window

	LONG		icf;
	LONG		ipf;

	RECT		rcClient;				// Client Rect for this control

   RECT     rcViewInset;           // view rect inset 

	SIZEL		sizelExtent;			// Extent array
public:
	CHARFORMAT	cf;					// Default character format

	PARAFORMAT2	pf;					// Default paragraph format

	LONG		laccelpos;				// Accelerator position

	WCHAR		chPasswordChar;		// Password character	

	COLORREF	m_TextClr;
	COLORREF	m_BkgrdClr;
public:
	//int		m_nHostBorder;
	POINT*	m_pptOffset;
	CBooguNoteText** m_ppTextBlockFocused;
	CWindow* m_pParentWindow;
	bool		m_bHovering;
	bool		m_bSelected;
	/*bool		m_bFocused;*/
	CBooguNoteHandle* m_pTextHandle;
	CBooguNoteIcon* m_pTextIcon;
	int		m_nLevel;
	bool	m_bShown;
	int		m_nExpandState;
	TCHAR*	m_szCache;
	bool	m_bClipped;
	bool	m_bBold;
	bool	m_bFile;
	bool	m_bShowBranch;
	Image	*m_pBitmap; 
	HICON		m_hFileIcon;
	time_t	m_ModifyTime;
};


#define ECO_STYLES (ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_NOHIDESEL | \
						ECO_READONLY | ECO_WANTRETURN | ECO_SAVESEL | \
						ECO_SELECTIONBAR)

// Window creation/destruction
HRESULT CreateTextControl(
	CBooguNoteView* pParentWindow,
	HWND hwndParentParent,
	DWORD dwStyle,
	TCHAR *pszText,
	LONG lLeft,
	LONG lTop,
	LONG lWidth,
	LONG lHeight,
	CBooguNoteText **pptec);

int PointsToLogical(int nPointSize);

void GetSysParms(void);

#endif // _HOST_H_
