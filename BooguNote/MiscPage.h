
#pragma once

#include "BooguNoteConfig.h"
//#include <vector>
using namespace std;


extern CBooguNoteConfig g_config;

class CMiscPage : public CPropertyPageImpl<CMiscPage>,
                            public CWinDataExchange<CMiscPage>
{
public:
    enum { IDD = IDD_DIALOG_MISC };
	//CListViewCtrl m_wlcShortcut;
	bool m_bDirty;
	CComboBox m_combBox;
	//bool bLaunchAtStartup;
	//int nSaveAllElapse;

    // Construction
    CMiscPage();

    // Maps
    BEGIN_MSG_MAP(CMiscPage)
        MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		CHAIN_MSG_MAP(CPropertyPageImpl<CMiscPage>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CMiscPage)
		DDX_UINT_RANGE(IDC_EDIT_SIDE_WORDS, g_config.eachSideWidthOfFoundString, 1, 10000)
		DDX_CHECK(IDC_CHECK_PASTE_WITH_SYMBOL, g_config.bCopyTextWithSymbol)
		DDX_UINT_RANGE(IDC_EDIT_PASTE_INDENT, g_config.nCopyIndent, 1, 10000)
		DDX_CHECK(IDC_CHECK_DOCURL, g_config.bGlobalCopyWithDocUrl)
		DDX_COMBO_INDEX(IDC_COMBO_BLOCK_STATE, g_config.nBlockStateAfterGlobalPaste)
    END_DDX_MAP()

    // Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
