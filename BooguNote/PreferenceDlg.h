#pragma once

#include <algorithm>
#include <string>
#include "BooguNoteConfig.h"
#include "ShortcutPage.h"
#include "UISettingPage.h"
#include "MiscPage.h"
#include "MainFramePage.h"
#include "ColorPage.h"
#include "TextBlockPage.h"

extern CBooguNoteConfig g_config;

class CPreferenceDlg : public CPropertySheetImpl<CPreferenceDlg>
{
public:
	public:
    // Construction
    CPreferenceDlg ( _U_STRINGorID title = (LPCTSTR) NULL, 
                    UINT uStartPage = 0, HWND hWndParent = NULL );

    // Maps
    BEGIN_MSG_MAP(CPreferenceDlg)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        CHAIN_MSG_MAP(CPropertySheetImpl<CPreferenceDlg>)
    END_MSG_MAP()

    // Message handlers
    void OnShowWindow ( BOOL bShowing, int nReason );
	LRESULT OnOK()
	{
		m_pgUISetting.DoDataExchange(TRUE);
		return IDOK;
	}
	// Property page notification handlers
    //int OnApply();

    // Property pages
	CMainFramePage		  m_pgMainFrame;
    CShortcutPage         m_pgShortcut;
	CUISettingPage		  m_pgUISetting;
	CMiscPage			  m_pgMisc;
	CTextBlockPage		  m_pgTextBlock;
	CColorPage			  m_pgColor;
    //CPropertyPage<IDD_ABOUTBOX> m_pgAbout;

    // Implementation
protected:
    bool m_bCentered;
};