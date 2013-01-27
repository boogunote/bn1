#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "PasteDlg.h"
#include <time.h>

extern UINT CF_HTML;
extern UINT _CF_RTF;

BOOL CPasteDlg::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	//CenterWindow();
	HWND hCurrGlobalActiveWindow = GetForegroundWindow();
	CWindow* pwin = new CWindow(hCurrGlobalActiveWindow);
	CRect rect;
	pwin->GetWindowRect(&rect);
	delete pwin;
	CRect myRect;
	GetWindowRect(&myRect);
	MoveWindow(rect.left+(rect.Width()-myRect.Width())/2, rect.top+(rect.Height()-myRect.Height())/2, myRect.Width(), myRect.Height());
	SetForegroundWindow(this->m_hWnd);
	m_strPasteDir = g_config.szPasteDir;

	int nIndex = 0;
	int nSelIndex = 0;
	m_cbFileType = GetDlgItem(IDC_COMBO_FILE_TYPE);
	if (IsClipboardFormatAvailable(_CF_RTF))
	{
		m_cbFileType.InsertString(nIndex,_T(".rtf (Save selected content as \"Rich Text Format\" text."));
		m_cbFileType.SetItemData(nIndex, 0);
		if (0 == g_config.nSaveType)
		{
			nSelIndex = nIndex;
		}
		nIndex++;
		if (OpenClipboard())
		{
			HGLOBAL hMem = GetClipboardData(_CF_RTF); 
			if (hMem != NULL) 
			{
				char* pBuf = (char*)GlobalLock(hMem);
				int nLength = strlen(pBuf)+1;
				m_szRTF = new char[nLength];
				strncpy(m_szRTF, pBuf, nLength);
			}
			GlobalUnlock(hMem);
		}
		CloseClipboard();
	}
	if (IsClipboardFormatAvailable(CF_HTML))
	{
		m_cbFileType.InsertString(nIndex,_T(".htm (Save selected content on web page)"));
		m_cbFileType.SetItemData(nIndex, 1);
		if (1 == g_config.nSaveType)
		{
			nSelIndex = nIndex;
		}
		nIndex++;
		m_cbFileType.InsertString(nIndex,_T(".htm (Save WHOLE web page)"));
		m_cbFileType.SetItemData(nIndex, 2);
		if (2 == g_config.nSaveType)
		{
			nSelIndex = nIndex;
		}
		nIndex++;
		if (OpenClipboard())
		{
			HGLOBAL hMem = GetClipboardData(CF_HTML); 
			if (hMem != NULL) 
			{
				char* pBuf = (char*)GlobalLock(hMem);
				char pPart1[] = "<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
				char* pPart2 = strchr(pBuf, '>')+1;
				int nLength = strlen(pBuf)+strlen(pPart1)+1;
				m_szHTML = new char[nLength];
				sprintf(m_szHTML, "%s%s", pPart1, pPart2);
				//strncpy(m_szHTML, pBuf, nLength);

				//get URL
				char  szSourceURL[] = "SourceURL";
				char* pSourceURLStart = strstr((char*)pBuf, szSourceURL);
				if (NULL != pSourceURLStart)
				{
					pSourceURLStart += strlen(szSourceURL);
					pSourceURLStart++;
					char* pSourceURLEnd = strstr(pSourceURLStart, "\r\n");
					UINT nLength = pSourceURLEnd - pSourceURLStart;
					if (NULL != pSourceURLEnd)
					{
						m_szURL = new char[nLength + 200];
						strncpy(m_szURL, pSourceURLStart,nLength+1);
						m_szURL[nLength+1] = '\0';
					}
				}
			}
			GlobalUnlock(hMem);
		}
		CloseClipboard();
	}
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		m_cbFileType.InsertString(nIndex,_T(".txt (Save selected content as plain text file.)"));
		m_cbFileType.SetItemData(nIndex, 3);
		if (3 == g_config.nSaveType)
		{
			nSelIndex = nIndex;
		}
		nIndex++;
		if (OpenClipboard())
		{
			HGLOBAL hMem = GetClipboardData(CF_TEXT); 
			if (hMem != NULL) 
			{
				char* pBuf = (char*)GlobalLock(hMem);
				int nLength = strlen(pBuf)+1;
				m_szUnicodeText = new char[nLength];
				strncpy(m_szUnicodeText, pBuf, nLength);
			}
			GlobalUnlock(hMem);
		}
		CloseClipboard();
	}
	m_cbFileType.SetCurSel(nSelIndex);

	//if (g_config.bDirectPaste)
	//{
	//	m_strText = m_strContent;
	//	m_cbFileType.EnableWindow(FALSE);
	//	GetDlgItem(IDC_EDIT_DIR).EnableWindow(FALSE);
	//	//GetDlgItem(IDC_EDIT_CONTENT_DESCRIPTION).SetWindowText(_T("Content:"));
	//}
	//else
	//{
		TCHAR tmpbuf[128];
		_tzset();
		_wstrtime_s( tmpbuf, 128 );
		CString fm;
		fm = tmpbuf;
		//int pos=fm.ReverseFind(_T(':'));
		//fm.Delete(pos, fm.GetLength()-pos);
		_tstrdate_s( tmpbuf, 128 );
		fm = _T(" ") + fm;
		fm = tmpbuf + fm;
		fm = _T("[") + fm;
		fm = fm + _T("]");
		fm.Replace(_T(':'),_T('-'));
		fm.Replace(_T('/'),_T('_'));
		m_strText = m_strTitle + fm;
		m_cbFileType.EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_DIR).EnableWindow(TRUE);
		//GetDlgItem(IDC_EDIT_CONTENT_DESCRIPTION).SetWindowText(_T("File Name:"));
	//}
	DoDataExchange(false);
	return TRUE;
}

LRESULT CPasteDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	g_config.nSaveType =m_cbFileType.GetItemData(m_cbFileType.GetCurSel());
	DoDataExchange(true);
	//if (!g_config.bDirectPaste)
	//{
		if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(m_strPasteDir))
		{
			_tcscpy(g_config.szPasteDir, m_strPasteDir.GetBuffer(MAX_PATH*8));
			m_strPasteDir.ReleaseBuffer();
			m_strTitle = m_strText;
			m_strTitle.Replace(_T('\\'),_T('_'));
			m_strTitle.Replace(_T('\/'),_T('_'));
			m_strTitle.Replace(_T(':'),_T('_'));
			m_strTitle.Replace(_T('*'),_T('_'));
			m_strTitle.Replace(_T('?'),_T('_'));
			m_strTitle.Replace(_T('"'),_T('_'));
			m_strTitle.Replace(_T('<'),_T('_'));
			m_strTitle.Replace(_T('>'),_T('_'));
			m_strTitle.Replace(_T('|'),_T('_'));
			m_strTitle.Replace(_T('\r'),_T(' '));
			m_strTitle.Replace(_T('\n'),_T(' '));

			m_fileDir = m_strPasteDir;
			if ( _T('\\') != m_fileDir[m_fileDir.GetLength()-1])
			{
				m_fileDir+=_T('\\');
			}
			m_fileDir += m_strTitle;

			char* szText = NULL;
			if (0 == g_config.nSaveType)
			{
				szText = m_szRTF;
				m_fileDir += _T(".rtf");
			}
			else if (1 == g_config.nSaveType)
			{
				//szText = strchr(m_szHTML, '<');
				szText = m_szHTML;
				m_fileDir += _T(".htm");
			}
			else if (2 == g_config.nSaveType)
			{
				m_fileDir += _T(".htm");
			}
			else if (3 == g_config.nSaveType)
			{
				szText = m_szUnicodeText;
				m_fileDir += _T(".txt");
			}
			if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_fileDir))
			{
				//save mht file
				if (2 == g_config.nSaveType)
				{
					URLDownloadToFile(0, CA2W(m_szURL), m_fileDir, 0, 0);

					//IWebBrowser2*    pBrowser2;

				 //  CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, 
					//				   IID_IWebBrowser2, (void**)&pBrowser2);
				 //  if (pBrowser2)
				 //  {
					//   CComVariant v;  // empty variant

					//   BSTR bstrURL = SysAllocString(CA2W(m_szURL));

					//   VARIANT va;
					//   va.vt = VT_BSTR;
					//   va.bstrVal = fileDir.AllocSysString();


					//   HRESULT hr = pBrowser2->Navigate(bstrURL, &v, &v, &v, &v);
					//   Sleep(10000);
					//   hr = pBrowser2->ExecWB(OLECMDID_SAVEAS,OLECMDEXECOPT_DODEFAULT,&va,&v); 
					//   //pBrowser2->Quit();

					//   SysFreeString(bstrURL);
					//   pBrowser2->Release();
					//   SysFreeString(va.bstrVal);
				 //  }

				}
				else
				{
					HANDLE hFile = CreateFileW(m_fileDir, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					//DWORD dwBytesWritten = WideCharToMultiByte(CP_UTF8, 0, szText, -1, 0, 0, NULL, NULL);
					//char* utf8buf = new char[dwBytesWritten];
					//int lBytesWritten = WideCharToMultiByte(CP_UTF8, 0, szText, -1, utf8buf, dwBytesWritten, NULL, NULL);
					int lBytesWritten = strlen(szText);
					DWORD dwFileBytesWritten;
					WriteFile(hFile, szText, lBytesWritten, &dwFileBytesWritten, NULL);
					//delete utf8buf;
					SetEndOfFile(hFile);
					CloseHandle(hFile);
				}
				EndDialog(wID);
			}
			else
			{
				MessageBox(_T("文件重名，请另外选择一个文件名"), _T("警告"), MB_OK|MB_ICONWARNING);
			}
			
		}
		else
		{
			MessageBox(_T("无效保存路径"), _T("警告"), MB_OK|MB_ICONWARNING);
		}
	//}
	//else
	//{
	//	EndDialog(wID);
	//}
	return 0;
}

LRESULT CPasteDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return -1;
}
LRESULT CPasteDlg::OnBnClickedCheckDirectPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nChecked = IsDlgButtonChecked(IDC_CHECK_STARTUP);
	if (nChecked==BST_CHECKED)
	{
		m_wndText.SetWindowText(m_strContent);
		m_cbFileType.EnableWindow(FALSE);
		m_wndPasteDir.EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CONTENT_DESCRIPTION).SetWindowText(_T("Content:"));
	}
	else
	{
		TCHAR tmpbuf[128];
		_tzset();
		_wstrtime_s( tmpbuf, 128 );
		CString fm;
		fm = tmpbuf;
		//int pos=fm.ReverseFind(_T(':'));
		//fm.Delete(pos, fm.GetLength()-pos);
		_tstrdate_s( tmpbuf, 128 );
		fm = _T(" ") + fm;
		fm = tmpbuf + fm;
		fm = _T("[") + fm;
		fm = fm + _T("]");
		fm.Replace(_T(':'),_T('-'));
		fm.Replace(_T('/'),_T('_'));
		m_wndText.SetWindowText(m_strTitle+fm);
		m_cbFileType.EnableWindow(TRUE);
		m_wndPasteDir.EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CONTENT_DESCRIPTION).SetWindowText(_T("File Name:"));
	}

	return 0;
}

LRESULT CPasteDlg::OnBnClickedButtonBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFolderDialog fd(NULL, NULL, BIF_USENEWUI);
	fd.SetInitialFolder(m_strPasteDir.GetBuffer(65536));
	m_strPasteDir.ReleaseBuffer();
	if (IDOK == fd.DoModal())
	{
		LPITEMIDLIST pidlTarget	= fd.GetSelectedItem();
		TCHAR pszPath[65536];
		SHGetPathFromIDList(pidlTarget, pszPath);   // Make sure it is a path
		m_strPasteDir = pszPath;
		m_wndPasteDir.SetWindowText(m_strPasteDir);
		//DoDataExchange(false, IDC_EDIT_SEARCHDIR  );
		//CoTaskMemFree(pidlTarget);
	}

	return 0;
}
