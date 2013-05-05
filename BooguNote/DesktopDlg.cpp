#include "stdafx.h"
#include "resource.h"
#include "DesktopDlg.h"
#include "dibapi.h"
#include "Common.h"

extern CUtil g_util;


CDesktopDlg::CDesktopDlg()
{
	m_hBmpDesktop = NULL;
	m_hDcMemory = NULL;
	m_rcSelect.left = -1;
	m_rcSelect.right = -1;
	m_rcSelect.top = -1;
	m_rcSelect.bottom = -1;
	m_bIsSelect = false;
	m_bIsSelectDone = false;
	m_hPen = CreatePen(PS_SOLID, 1, RGB(255,0,0));
}

CDesktopDlg::~CDesktopDlg()
{
	if (NULL != m_hBmpDesktop)
	{
		DeleteObject(m_hBmpDesktop);
		m_hBmpDesktop = NULL;
	}
	if (NULL != m_hDcMemory)
	{
		DeleteDC(m_hDcMemory);
		m_hDcMemory = NULL;
	}
	if (NULL != m_hPen)
	{
		DeleteObject(m_hPen);
		m_hPen = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Message handlers
BOOL CDesktopDlg::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	ShowWindow(SW_HIDE);

	HDC hDcDesktop = CreateDC(L"DISPLAY", NULL, NULL, NULL);

	m_szDesktop.cx = GetDeviceCaps(hDcDesktop, HORZRES);
	m_szDesktop.cy = GetDeviceCaps(hDcDesktop, VERTRES);

	if (NULL != m_hBmpDesktop)
	{
		DeleteObject(m_hBmpDesktop);
		m_hBmpDesktop = NULL;
	}
	if (NULL != m_hDcMemory)
	{
		DeleteDC(m_hDcMemory);
		m_hDcMemory = NULL;
	}

	m_hDcMemory = CreateCompatibleDC(hDcDesktop);
	m_hBmpDesktop = CreateCompatibleBitmap(hDcDesktop, m_szDesktop.cx, m_szDesktop.cy);
	HBITMAP pBmpOld = (HBITMAP)SelectObject(m_hDcMemory, m_hBmpDesktop);
	BitBlt(m_hDcMemory, 0,0,m_szDesktop.cx, m_szDesktop.cy, hDcDesktop, 0, 0, SRCCOPY);
	DeleteDC(hDcDesktop);
	MoveWindow(0, 0, m_szDesktop.cx, m_szDesktop.cy);
	ShowWindow(SW_SHOW);
	return TRUE;
}

LRESULT CDesktopDlg::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	//draw captured desktop
	PAINTSTRUCT ps;
	HDC dlgDC = ::BeginPaint(m_hWnd, &ps);

	HDC hDcMemory = CreateCompatibleDC(dlgDC);
	HBITMAP hBmpDlg = CreateCompatibleBitmap(dlgDC, m_szDesktop.cx, m_szDesktop.cy);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDcMemory, hBmpDlg);
	BitBlt(hDcMemory, 0,0,m_szDesktop.cx, m_szDesktop.cy, m_hDcMemory, 0, 0, SRCCOPY);

	if (m_bIsSelect || m_bIsSelectDone)
	{
		//draw selected rectangle
		HPEN hOldPen = (HPEN)SelectObject(hDcMemory, m_hPen);

		::MoveToEx(hDcMemory, m_rcSelect.left, m_rcSelect.top, NULL);
		::LineTo(hDcMemory, m_rcSelect.right, m_rcSelect.top);
		::LineTo(hDcMemory, m_rcSelect.right, m_rcSelect.bottom);
		::LineTo(hDcMemory, m_rcSelect.left, m_rcSelect.bottom);
		::LineTo(hDcMemory, m_rcSelect.left, m_rcSelect.top);

		SelectObject(hDcMemory, hOldPen);
		//::DeleteObject(hPen);
	}

	BitBlt(dlgDC, 0,0,m_szDesktop.cx, m_szDesktop.cy, 
	hDcMemory, 
	0,0,
	SRCCOPY);

	SelectObject(hDcMemory, hBmpOld);
	::DeleteObject(hBmpDlg);
	::DeleteDC(hDcMemory);
	::EndPaint(m_hWnd, &ps);
	bHandled = TRUE;
	return 0;
}

LRESULT CDesktopDlg::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bIsSelectDone)
	{
		m_bIsSelect = true;
		m_bIsSelectDone = false;
		m_rcSelect.left = GET_X_LPARAM(lParam);
		m_rcSelect.right = GET_X_LPARAM(lParam);
		m_rcSelect.top = GET_Y_LPARAM(lParam);
		m_rcSelect.bottom = GET_Y_LPARAM(lParam);
		Invalidate(FALSE);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CDesktopDlg::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bIsSelect)
	{
		m_rcSelect.right = GET_X_LPARAM(lParam);
		m_rcSelect.bottom = GET_Y_LPARAM(lParam);
		Invalidate(FALSE);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CDesktopDlg::OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bIsSelectDone)
	{
		m_rcSelect.right = GET_X_LPARAM(lParam);
		m_rcSelect.bottom = GET_Y_LPARAM(lParam);
		Invalidate(FALSE);
		m_bIsSelect = false;
		m_bIsSelectDone = true;
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CDesktopDlg::OnLButtonDblClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_rcSelect.left != m_rcSelect.right && m_rcSelect.top != m_rcSelect.bottom)
	{
		CDC dcMem;
		dcMem.CreateCompatibleDC(NULL);
		CBitmap bitmap;
		BITMAP bm;
		GetObject(m_hBmpDesktop, sizeof(bm), &bm);
		bm.bmWidth = m_rcSelect.Width();
		bm.bmHeight = m_rcSelect.Height();
		bitmap.CreateBitmapIndirect(&bm);
		HBITMAP hBmpOld = dcMem.SelectBitmap(bitmap.m_hBitmap);
		
		dcMem.StretchBlt(0,0, m_rcSelect.Width(), m_rcSelect.Height(),
			m_hDcMemory, m_rcSelect.left, m_rcSelect.top,
			m_rcSelect.Width(), m_rcSelect.Height(), SRCCOPY);
		
		HBITMAP hBmp = (HBITMAP)bitmap.Detach();
		dcMem.SelectBitmap(hBmpOld);


		// save to the clipboard
		if(::OpenClipboard(m_hWnd))
		{
			::EmptyClipboard();
			  ::GdiFlush();

			  HANDLE     hDib    = NULL;
			  HPALETTE ghPal   = NULL;
			  if (hBmp)
			  {
				 hDib = BitmapToDIB(hBmp, ghPal);
				 if (hDib)
				 {
					HANDLE hResult = ::SetClipboardData(CF_DIB, hDib);
					if (hResult == NULL)
					{
					  // _ShowLastError();
					}
				 }
				 else
				 {
					MessageBeep(0);
				 }
			  }
			  ::CloseClipboard();


			//::EmptyClipboard();
			//hDib = BitmapToDIB(m_hBitmap, ghPal);
			//::SetClipboardData(CF_BITMAP, hBmp);
			//::CloseClipboard();
		}
		EndDialog(IDOK);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CDesktopDlg::OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bIsSelectDone)
	{
		SetCursor(g_util.hCrsCross);
	}
	else
	{
		SetCursor(g_util.hCsrHand);
	}
	bHandled = TRUE;
	return 0;
}