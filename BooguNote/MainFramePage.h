
#pragma once

#include "BooguNoteConfig.h"
//#include <vector>
using namespace std;


extern CBooguNoteConfig g_config;

class CMainFramePage : public CPropertyPageImpl<CMainFramePage>,
                            public CWinDataExchange<CMainFramePage>
{
public:
    enum { IDD = IDD_DIALOG_MAINFRAME };
	//CListViewCtrl m_wlcShortcut;
	bool m_bDirty;
	//CComboBox m_cbFrameState;
	CComboBox m_cbTxtEncoding;
	bool bLaunchAtStartup;
	int nSaveAllElapse;

    // Construction
    CMainFramePage();

    // Maps
    BEGIN_MSG_MAP(CMainFramePage)
        MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		COMMAND_HANDLER(IDC_CHECK_STARTUP, BN_CLICKED, OnBnClickedCheckStartup)
		COMMAND_HANDLER(IDC_CHECK_WORK_DIR, BN_CLICKED, OnBnClickedCheckWorkDir)
		CHAIN_MSG_MAP(CPropertyPageImpl<CMainFramePage>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CMainFramePage)
		//DDX_UINT_RANGE(IDC_EDIT_SIDE_WORDS, g_config.eachSideWidthOfFoundString, 1, 10000)
		//DDX_CHECK(IDC_CHECK_PASTE_WITH_SYMBOL, g_config.bCopyTextWithSymbol)
		//DDX_UINT_RANGE(IDC_EDIT_PASTE_INDENT, g_config.nCopyIndent, 1, 10000)
		//DDX_CHECK(IDC_CHECK_DOCURL, g_config.bGlobalCopyWithDocUrl)
		//DDX_COMBO_INDEX(IDC_COMBO_BLOCK_STATE, g_config.nBlockStateAfterGlobalPaste)
		DDX_CHECK(IDC_CHECK_STARTUP, bLaunchAtStartup)
		DDX_UINT_RANGE(IDC_SAVEALL_ELAPSE, nSaveAllElapse, 1, 10000)
		DDX_CHECK(IDC_CHECK_SAVE_TXT, g_config.bSaveToTxtFile)
		DDX_CHECK(IDC_CHECK_WORK_DIR, g_config.bUseWorkingDirAsRootStrorageDir)
		if (!g_config.bUseWorkingDirAsRootStrorageDir)
		{
			DDX_TEXT(IDC_EDIT_STORAGE_ROOT, g_config.szRootStorageDir)
		}
		DDX_TEXT(IDC_EDIT_LOG_FOLDER, g_config.szLogFolder)
		DDX_TEXT(IDC_EDIT_CAPTIONTEXT, g_config.szCaptionString)
		DDX_CHECK(IDC_CHECK_OPEN_BOO_IN_SAME_INSTANCE, g_config.bBooOpenInSameInstance)
		DDX_COMBO_INDEX(IDC_COMBO_TXT_ENCODING, g_config.nCodePage)
    END_DDX_MAP()

    // Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedCheckStartup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckWorkDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
