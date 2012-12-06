// PreferenceDlg.cpp: implementation of the CPreferenceDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "PreferenceDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction

CPreferenceDlg::CPreferenceDlg ( _U_STRINGorID title, UINT uStartPage,
                               HWND hWndParent ) :
  CPropertySheetImpl<CPreferenceDlg> ( title, uStartPage, hWndParent ),
  m_bCentered(false)
{
    m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;

	AddPage ( m_pgUISetting );
	AddPage ( m_pgTextBlock );
	AddPage ( m_pgMainFrame );
	AddPage ( m_pgColor );
    AddPage ( m_pgShortcut );
	AddPage ( m_pgMisc );
}


//////////////////////////////////////////////////////////////////////
// Message handlers

void CPreferenceDlg::OnShowWindow ( BOOL bShowing, int nReason )
{
    if ( bShowing && !m_bCentered )
        {
        m_bCentered = true;
        CenterWindow ( m_psh.hwndParent );
        }
	//SetModified(m_pgShortcut.m_hWnd);
}

//
//int CPreferenceDlg::OnApply()
//{
//    return 0;//DoDataExchange(true) ? PSNRET_NOERROR : PSNRET_INVALID;
//}
