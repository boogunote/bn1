#include "stdafx.h"
#include "FileTreeView.h"
#include "BooguNoteConfig.h"

extern CBooguNoteConfig g_config;

BOOL CFileTreeView::PreTranslateMessage(MSG* pMsg)
{
	return m_Org.PreTranslateMessage(pMsg);
}

LRESULT CFileTreeView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_editDir.Create(m_hWnd, rcDefault, 0, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE);
	
	HFONT hFont = CreateFont(15, 
		0, 0, 0, 
		0,
		0,0,0,DEFAULT_CHARSET,0,0,
		DEFAULT_QUALITY,
		0,
		L"Arial");
	m_editDir.SetFont(hFont);
	m_editDir.m_strDir = g_config.szFileManDir;
	m_editDir.SetWindowText((LPCTSTR)m_editDir.m_strDir);

	m_bnFresh.Create(m_hWnd, rcDefault, L"刷新", WS_CHILD | WS_VISIBLE , WS_EX_CLIENTEDGE);
	m_bnFresh.SetFont(hFont);
	m_bnUp.Create(m_hWnd, rcDefault, L"返回上级", WS_CHILD | WS_VISIBLE , WS_EX_CLIENTEDGE);
	m_bnUp.SetFont(hFont);

	m_Org.Create(m_hWnd, 0, g_config.szFileManDir);
	//m_Org.SetRootFolder(m_editDir.m_strDir.GetBuffer(MAX_PATH));
	//m_editDir.m_strDir.ReleaseBuffer();
	return 0;
}

LRESULT CFileTreeView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	//PostQuitMessage(0);
	RecordMemberVariable();
	return 0;
}

void CFileTreeView::RecordMemberVariable()
{
	_tcscpy(g_config.szFileManDir, m_Org.GetRootFolder().c_str());
	PathAddBackslash(g_config.szFileManDir);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	g_config.nFileManLeft = rcWindow.left;
	g_config.nFileManTop = rcWindow.top;
	g_config.nFileManWidth = rcWindow.Width();
	g_config.nFileManHeight = rcWindow.Height();
	g_config.WriteConfigToIni();
}

LRESULT CFileTreeView::OnUserDirUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_editDir.m_strDir.IsEmpty())
	{
		if (!PathIsRoot((LPCWSTR)m_editDir.m_strDir))
		{
			PathRemoveBackslash(m_editDir.m_strDir.GetBuffer(MAX_PATH));
			PathRemoveFileSpec(m_editDir.m_strDir.GetBuffer(MAX_PATH));
			PathAddBackslash(m_editDir.m_strDir.GetBuffer(MAX_PATH));
			m_editDir.m_strDir.ReleaseBuffer();
		}
		else
		{
			m_editDir.m_strDir = L"";
		}
		m_editDir.SetWindowText((LPCWSTR)(m_editDir.m_strDir));
		m_editDir.SetSel(m_editDir.m_strDir.GetLength(), m_editDir.m_strDir.GetLength());
		TreeView_SelectItem(m_Org.m_hWnd, NULL);
		m_Org.SetRootFolder((LPCWSTR)m_editDir.m_strDir);
	}
	RecordMemberVariable();
	return 0;
}

LRESULT CFileTreeView::OnUserChangeDir(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR szDir[MAX_PATH*2];
	m_editDir.GetWindowText(szDir, MAX_PATH*2);
	CString strDir = szDir;
	strDir.TrimLeft();
	strDir.TrimRight();
	strDir.Replace(L'/', L'\\');
	if (strDir.IsEmpty())
	{
		m_Org.SetRootFolder((LPCTSTR)strDir);
		m_editDir.m_strDir = strDir;
	}
	else
	{
		//CString tmp = strDir.Right(1);
		if (L"\\" != strDir.Right(1))
		{
			strDir += L"\\";
		}
		DWORD hFillAtt = GetFileAttributes((LPCWSTR)strDir);
		if (INVALID_FILE_ATTRIBUTES == hFillAtt)
		{
			MessageBox( _T("Invalidat directory."), _T("Warning"), MB_OK|MB_ICONEXCLAMATION);
		}
		else
		{
			m_editDir.m_strDir = strDir;
			//::PostMessage(GetParent(), WM_USER_CHANG_DIR, 0, 0);
			//m_strDir = szDir;
		}
		m_Org.SetRootFolder((LPCWSTR)m_editDir.m_strDir);
	}
	m_editDir.SetWindowText((LPCWSTR)(m_editDir.m_strDir));
	m_editDir.SetSel(m_editDir.m_strDir.GetLength(), m_editDir.m_strDir.GetLength());
	//m_editDir.SetFocus();
	RecordMemberVariable();
	
	bHandled = FALSE;
	return 0;
}

LRESULT CFileTreeView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect clientRect;
	GetClientRect(clientRect);

	CRect rcEditDir = clientRect;
	rcEditDir.top = 1;
	rcEditDir.bottom  =rcEditDir.top + 22;
	rcEditDir.right = clientRect.right - 100;
	m_editDir.MoveWindow(rcEditDir, FALSE);

	CRect rcBnFresh;
	rcBnFresh = rcEditDir;
	rcBnFresh.left = clientRect.right - 100;
	rcBnFresh.right = clientRect.right - 60;
	m_bnFresh.MoveWindow(rcBnFresh, FALSE);

	CRect rcBnUp;
	rcBnUp = rcEditDir;
	rcBnUp.left = clientRect.right-60;
	rcBnUp.right = clientRect.right;
	m_bnUp.MoveWindow(rcBnUp, FALSE);

	CRect rcOrg = clientRect;
	rcOrg.top = rcEditDir.bottom +2;
	m_Org.MoveWindow(rcOrg, FALSE);
	bHandled = FALSE;
	return 0;
}

LRESULT CFileTreeView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFileTreeView* pT = static_cast<CFileTreeView*>(this);
	ATLASSERT(::IsWindow(pT->m_hWnd));

	PAINTSTRUCT ps;
	BeginPaint(&ps);
	DoPaint(ps.hdc, ps.rcPaint);
	EndPaint(&ps);

	return 0;
}

void CFileTreeView::DoPaint( CDCHandle dc, RECT rcPaint)
{
}

LRESULT CFileTreeView::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (ID_POPMENU_OPEN == wParam)
	{
		std::wstring path = m_Org.GetSelectedPath();
		ShellExecute(NULL, _T("open"), path.c_str(), 0, 0, SW_SHOWNORMAL);
	}
	else if (ID_POPMENU_OPENIN == wParam)
	{
		std::wstring path = m_Org.GetSelectedPath();
		if (0 == wcscmp(L".boo", PathFindExtension(path.c_str())))
		{
			::SendMessage(GetParent(), WM_USER_MAINFRM_OPENFILE, (WPARAM)path.c_str(), 0);
		}
	}
	else if (ID_POPMENU_USEASROOT == wParam)
	{
		std::wstring path = m_Org.GetSelectedPath();
		CString strDir = path.c_str();
		strDir.TrimLeft();
		strDir.TrimRight();
		strDir.Replace(L'/', L'\\');
		if (strDir.IsEmpty())
		{
			m_Org.SetRootFolder(NULL);
			m_editDir.m_strDir = strDir;
		}
		else
		{
			//CString tmp = strDir.Right(1);
			if (L"\\" != strDir.Right(1))
			{
				strDir += L"\\";
			}
			DWORD hFillAtt = GetFileAttributes((LPCWSTR)strDir);
			if (INVALID_FILE_ATTRIBUTES == hFillAtt)
			{
				MessageBox( _T("Invalidat directory."), _T("Warning"), MB_OK|MB_ICONEXCLAMATION);
			}
			else
			{
				m_editDir.m_strDir = strDir;
				//::PostMessage(GetParent(), WM_USER_CHANG_DIR, 0, 0);
				//m_strDir = szDir;
			}
		}
		m_editDir.SetWindowText((LPCWSTR)(m_editDir.m_strDir));
		m_editDir.SetSel(m_editDir.m_strDir.GetLength(), m_editDir.m_strDir.GetLength());
		//m_editDir.SetFocus();

		TreeView_SelectItem(m_Org.m_hWnd, NULL);
		m_Org.SetRootFolder((LPCWSTR)m_editDir.m_strDir);
		RecordMemberVariable();
	}
	return 0;
}

//LRESULT CFileTreeView::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = FALSE;
//	UINT state = LOWORD(wParam);
//	// Now determine if we're getting or losing activation
//	switch (state)
//	{
//	case WA_ACTIVE:
//	case WA_CLICKACTIVE:
//		::SendMessage(GetParent().m_hWnd, WM_USER_CHILD_ACTIVE, 0, 0);
//		break;
//	default:
//		break;
//	}
//	return 0;
//}