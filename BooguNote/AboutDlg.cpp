// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	CString title = _T("BooguNote\n");
	title += BOOGUNOTE_VERSION;
	GetDlgItem(IDC_STATIC_TITLE).SetWindowText(title);
	m_googleGroup.SubclassWindow(GetDlgItem(IDC_HYPERLINK_GOOGLEGROUP));
	m_googleGroup.SetHyperLink(L"http://boogu.me/index_zh.htm");
	m_emailAddress.SubclassWindow(GetDlgItem(IDC_HYPERLINK_EMAILADDRESS));
	m_emailAddress.SetHyperLink(L"mailto:boogunote@gmail.com");
	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}