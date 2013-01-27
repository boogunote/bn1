
#pragma once





class CShortcutEdit : public CDialogImpl<CShortcutEdit>,
                            public CWinDataExchange<CShortcutEdit>
{
public:
    enum { IDD = IDD_SHORTCUT_EDIT };
	// DDX variables
	bool m_bCtrl;
	bool m_bAlt;
	bool m_bShift;
	bool m_bWin;
	int m_nKeyIndex;
	CComboBox m_combBox;
	bool m_bDirty;
	bool m_bShowWin;

    // Construction
    CShortcutEdit();

    // Maps
    BEGIN_MSG_MAP(CShortcutEdit)
        MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		REFLECT_NOTIFICATIONS()
        //CHAIN_MSG_MAP<CDialogImpl<CShortcutEdit>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CShortcutPage)
        DDX_CHECK(IDC_CHECK_CTRL, m_bCtrl)
        DDX_CHECK(IDC_CHECK_ATL, m_bAlt)
		DDX_CHECK(IDC_CHECK_SHIFT, m_bShift)
		DDX_CHECK(IDC_CHECK_WIN, m_bWin)
		DDX_COMBO_INDEX(IDC_COMBO_KEY, m_nKeyIndex)
    END_DDX_MAP()

    // Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	LRESULT    OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT    OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    // Property page notification handlers
    //int OnApply();
};
