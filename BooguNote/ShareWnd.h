// searchdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <string>
#include "BooguNoteConfig.h"
#include "Common.h"

extern CBooguNoteConfig g_config;

class CShareWnd : public CWindowImpl<CShareWnd>
{
public:
	CAxWindow				m_wndIE;
public:
	BEGIN_MSG_MAP(CFileTreeView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		//MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		const DWORD dwIEStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN |
			WS_HSCROLL | WS_VSCROLL,
			dwIEExStyle = WS_EX_CLIENTEDGE;


		m_wndIE.Create ( m_hWnd, rcDefault, _T("http://127.0.0.1/ko/article/"), dwIEStyle, dwIEExStyle );
// 		CComPtr<IWebBrowser2> pWB2;
// 		HRESULT hr;
// 		hr = m_wndIE.QueryControl ( &pWB2 );
// 		DWORD dw = GetLastError();
// 		CComVariant v;  // empty variant
// 		pWB2->Navigate ( CComBSTR(L"http://127.0.0.1/ko/article/"), &v, &v, &v, &v );
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect rc;
		GetClientRect(&rc);
		m_wndIE.SetWindowPos(NULL, &rc, SWP_NOZORDER);
		return 0;
	}

	//LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};