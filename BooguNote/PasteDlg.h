
#pragma once

#include "BooguNoteConfig.h"

extern CBooguNoteConfig g_config;

class CPasteDlg : public CDialogImpl<CPasteDlg>,
                            public CWinDataExchange<CPasteDlg>
{
public:
    enum { IDD = IDD_PASTE };
	CEditImpl   m_wndText;
	CString		m_strText;
	CEditImpl   m_wndPasteDir;
	CString		m_strPasteDir;
	CComboBox	m_cbFileType;
	CString		m_strContent;
	CString		m_strTitle;
	CString		m_fileDir;

	char*		m_szUnicodeText;
	char*		m_szRTF;
	char*		m_szHTML;
	char*		m_szURL;

    // Construction
	CPasteDlg()
	{
		m_szUnicodeText = NULL;
		m_szRTF = NULL;
		m_szHTML = NULL;
		m_szURL = NULL;
	};

	~CPasteDlg()
	{
		if (NULL != m_szUnicodeText)
		{
			delete m_szUnicodeText;
			m_szUnicodeText = NULL;
		}
		if (NULL != m_szRTF)
		{
			delete m_szRTF;
			m_szRTF = NULL;
		}
		if (NULL != m_szHTML)
		{
			delete m_szHTML;
			m_szHTML = NULL;
		}
		if (NULL != m_szURL)
		{
			delete m_szURL;
			m_szURL = NULL;
		}
	};

    // Maps
    BEGIN_MSG_MAP(CPasteDlg)
        MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_CHECK_DIRECT_PASTE, BN_CLICKED, OnBnClickedCheckDirectPaste)
		//REFLECT_NOTIFICATIONS()
		COMMAND_HANDLER(IDC_BUTTON_BROWSE, BN_CLICKED, OnBnClickedButtonBrowse)
	END_MSG_MAP()

    BEGIN_DDX_MAP(CPasteDlg)
		//DDX_CHECK(IDC_CHECK_DIRECT_PASTE, g_config.bDirectPaste)
        DDX_CONTROL(IDC_EDIT_CONTENT, m_wndText)
		DDX_TEXT(IDC_EDIT_CONTENT, m_strText)
		DDX_CONTROL(IDC_EDIT_DIR, m_wndPasteDir)
		DDX_TEXT(IDC_EDIT_DIR, m_strPasteDir)
    END_DDX_MAP()

    // Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	LRESULT    OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT    OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    // Property page notification handlers
    //int OnApply();
	LRESULT OnBnClickedCheckDirectPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
