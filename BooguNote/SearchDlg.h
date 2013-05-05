// searchdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <string>
#include "BooguNoteConfig.h"
#include "legacy_xml.h"
#include "Common.h"

extern CBooguNoteConfig g_config;

struct ItemData	            // menu item data
{
	CString filename;
	int		lineNumber;
};

class CSearchDlg : public CDialogImpl<CSearchDlg>,
					public CWinDataExchange<CSearchDlg>,
					public CDialogResize<CSearchDlg>
{
public:
	enum { IDD = IDD_DIALOG_SEARCH };

	CEditImpl   m_wndSearchDir;
	CString		m_searchDir;
	CEditImpl   m_wndSearchWords;
	CString		m_searchWords;
	CComboBox	m_cbIcon;
	int			m_nIconIndex;

	BEGIN_DDX_MAP(CSearchDlg)
        DDX_CONTROL(IDC_EDIT_SEARCHWORDS, m_wndSearchWords)
		DDX_TEXT(IDC_EDIT_SEARCHWORDS, m_searchWords)
		DDX_CONTROL(IDC_EDIT_SEARCHDIR, m_wndSearchDir)
		DDX_TEXT(IDC_EDIT_SEARCHDIR, m_searchDir)
    END_DDX_MAP()

	BEGIN_DLGRESIZE_MAP(CSearchDlg)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LIST_REPORT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_EDIT_SEARCHDIR, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_SEARCHWORDS, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CSearchDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		//MESSAGE_HANDLER(WM_CHAR, OnChar)
		//MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		COMMAND_ID_HANDLER(IDOK, OnSearch)
		COMMAND_ID_HANDLER(IDC_BUTTON_BROWSE, OnBrowse)
		CHAIN_MSG_MAP(CDialogResize<CSearchDlg>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_nIconIndex = 0;

		CenterWindow(GetParent());
		DlgResize_Init();
		TCHAR buf[MAX_PATH];
		/*GetPrivateProfileString(_T("Search"),_T("SearchDir"), _T(""),buf, MAX_PATH, g_config.strConfigFileName);
		m_searchDir = buf;*/
		DoDataExchange(false);
		m_cbIcon = GetDlgItem(IDC_COMBO_ICON);
		m_cbIcon.InsertString(0,_T("想法"));
		m_cbIcon.InsertString(0,_T("警告"));
		m_cbIcon.InsertString(0,_T("问号"));
		m_cbIcon.InsertString(0,_T("星号"));
		m_cbIcon.InsertString(0,_T("叉号"));
		m_cbIcon.InsertString(0,_T("完成"));
		m_cbIcon.InsertString(0,_T("待办"));
		m_cbIcon.InsertString(0,_T("文本"));
		m_cbIcon.InsertString(0,_T(""));
		return TRUE;
	}

	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		for (int j=0; j<((CListBox)(GetDlgItem(IDC_LIST_REPORT))).GetCount(); j++)
		{
			ItemData* p = (ItemData*)((CListBox)(GetDlgItem(IDC_LIST_REPORT))).GetItemDataPtr(j);
			if (p!=NULL)
			{
				delete p;
			}
		}
		PostMessage(WM_USER_SEARCHDLG_CLOSED);
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnBrowse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog fd(NULL, NULL, BIF_BROWSEINCLUDEFILES);
		fd.SetInitialFolder(m_searchDir.GetBuffer(65536));
		m_searchDir.ReleaseBuffer();
		if (IDOK == fd.DoModal())
		{
			LPITEMIDLIST pidlTarget	= fd.GetSelectedItem();
			TCHAR pszPath[65536];
			SHGetPathFromIDList(pidlTarget, pszPath);   // Make sure it is a path
			m_searchDir = pszPath;
			DoDataExchange(false, IDC_EDIT_SEARCHDIR  );
			//CoTaskMemFree(pidlTarget);
		}
		return 0;
	}

	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if ( !DoDataExchange(true) )
			return -1;
		m_nIconIndex =m_cbIcon.GetCurSel();
		m_searchWords.MakeLower();
		m_searchWords.TrimLeft();
		m_searchWords.TrimRight();

		DWORD hFillAtt = GetFileAttributes(m_searchDir);
		if (INVALID_FILE_ATTRIBUTES == hFillAtt)
		{
			MessageBox( _T("搜索路径不正确"), _T("警告"), MB_OK|MB_ICONEXCLAMATION);
			return -1;
		}
		else if (m_searchWords.IsEmpty() && 0 >= m_nIconIndex)
		{
			MessageBox( _T("请输入搜索词或者选择要搜索的图标"), _T("警告"), MB_OK|MB_ICONEXCLAMATION);
		}
		else
		{
			WritePrivateProfileString(_T("Search"),_T("SearchDir"),m_searchDir.GetBuffer(MAX_PATH),g_config.strConfigFileName);
			m_searchDir.ReleaseBuffer();

			if (FILE_ATTRIBUTE_DIRECTORY == hFillAtt) // search directory
			{
				for (int j=0; j<((CListBox)(GetDlgItem(IDC_LIST_REPORT))).GetCount(); j++)
				{
					ItemData* p = (ItemData*)((CListBox)(GetDlgItem(IDC_LIST_REPORT))).GetItemDataPtr(j);
					if (p!=NULL)
					{
						delete p;
					}
				}
				((CListBox)(GetDlgItem(IDC_LIST_REPORT))).ResetContent();
				if (m_searchDir.GetAt(m_searchDir.GetLength()-1)!=_T('\\'))
				{
					m_searchDir+=_T('\\');
				}

				FindInFiles(basic_string<TCHAR>(LPCTSTR(m_searchWords)), basic_string<TCHAR>(LPCTSTR(m_searchDir)), true, false); 
			}
			else // search single file
			{
				CString fileDirectory = m_searchDir;
				int dot = fileDirectory.ReverseFind(_T('.'));
				CString ext = fileDirectory;
				ext.Delete(0, dot+1);
				ext.MakeLower();
				
				if (0==ext.Compare(_T("boo")))
				{
					((CListBox)(GetDlgItem(IDC_LIST_REPORT))).ResetContent();
					SearchInFile(basic_string<TCHAR>(LPCTSTR(m_searchWords)), basic_string<TCHAR>(LPCTSTR(m_searchDir)), false);
				}
			}
			if (0==((CListBox)(GetDlgItem(IDC_LIST_REPORT))).GetCount())
			{
				MessageBox( _T("没有找到"), _T("警告"), MB_OK|MB_ICONEXCLAMATION);
			}
		}
		
		return 0;
	}

	LRESULT FindInFiles(basic_string<TCHAR> &searchWord, basic_string<TCHAR> &searchDir, const bool &bSearchSubDir, const bool &bMatchCase)
	{
		WIN32_FIND_DATA ffd;
		//TCHAR szDir[MAX_PATH];
		HANDLE hFind = INVALID_HANDLE_VALUE;
		vector< basic_string<TCHAR> > subDirList;
		vector< basic_string<TCHAR> > fileNameList;

		//searchDir+=_T('*');
		hFind = FindFirstFile((searchDir+_T('*')).c_str(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE) 
		{
			MessageBox( _T("搜索路径不对"), _T("警告"), MB_OK|MB_ICONEXCLAMATION);
			return 0;
		}
		do
		{
			if (bSearchSubDir && (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if ((_tcscmp(ffd.cFileName, _T("."))==0)||(_tcscmp(ffd.cFileName, _T(".."))==0))
				{
					continue;
				}
				else
				{
					basic_string<TCHAR> bsDir(searchDir);
					bsDir+=ffd.cFileName;
					bsDir+=_T('\\');
					subDirList.push_back(bsDir);
				}
			}
			else
			{
				basic_string<TCHAR> bsFile(searchDir);
				CString fn(ffd.cFileName);
				int dot = fn.ReverseFind(_T('.'));
				CString ext = fn;
				ext.Delete(0, dot+1);
				ext.MakeLower();
				if (0==ext.Compare(_T("boo")))
				{
					bsFile+=ffd.cFileName;
					fileNameList.push_back(bsFile);
				}
			}
		}
		while (FindNextFile(hFind, &ffd) != 0);

		sort(fileNameList.begin(), fileNameList.end(), NameCompare);
		vector< basic_string<TCHAR> >::iterator p = fileNameList.begin();
		while(p!=fileNameList.end())
		{
			SearchInFile(searchWord, (*p), bMatchCase);
			p++;
		}
		
		sort(subDirList.begin(), subDirList.end(), NameCompare);
		if (bSearchSubDir && (subDirList.size()>0))
		{
			vector< basic_string<TCHAR> >::iterator p = subDirList.begin();
			while(p!=subDirList.end())
			{
				FindInFiles(searchWord, (*p), bSearchSubDir, bMatchCase);
				p++;
			}
		}
		FindClose(hFind);
		return 0;
	}

	static bool NameCompare(basic_string<TCHAR> e1, basic_string<TCHAR> e2)
	{
		return e1 > e2;
	}

	//LRESULT SearchInFile(vector<basic_string<TCHAR>> &searchWordList, const TCHAR* fileName, const bool &bMatchCase)
	LRESULT SearchInFile(basic_string<TCHAR> &searchWord, basic_string<TCHAR> &fileName, const bool &bMatchCase)
	{
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

		HANDLE hFile = CreateFileW(fileName.c_str(), GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		BYTE * lpvFile = (BYTE *)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		if (0 == strncmp((char*)lpvFile, XML_HEADER, strlen(XML_HEADER)-3)) // -3 for xml library adding chars.
		{
			UnmapViewOfFile(lpvFile);
			CloseHandle(hMap);
			CloseHandle(hFile);

			XML x(fileName.c_str());
			XMLElement* r = x.GetRootElement();

			int nC = r->GetAllChildrenNum();
			if (nC >0 )
			{
				typedef XMLElement* XMLElement_Ptr;
				XMLElement_Ptr* xeList = new XMLElement_Ptr[nC];
				r->GetAllChildren(xeList);

				bool bFoundInFile = false;
				for (int i=0; i<nC; ++i)
				{
					
					int nValueLen = 0;
					char* pValueBuf = NULL;
					int nLength = 0;
					TCHAR* utf8buf = NULL;
					XMLVariable* v = NULL;

					bool bFoundByIcon = false;
					bool bFoundInTextBlock = false;
					if (m_nIconIndex >0)
					{
						//测试图标
						v = xeList[i]->FindVariableZ("icon");
						if (NULL != v)
						{
							nValueLen = v->GetValue(0);
							pValueBuf = new char[nValueLen + 1];
							memset(pValueBuf, '\0', nValueLen+1);
							v->GetValue(pValueBuf);
							pValueBuf[nValueLen] = '\0';
							nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
							utf8buf = new TCHAR[nLength+1];
							MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
							int nIndex = -1;
							if (_tcscmp(utf8buf, _T("none"))==0)
							{
								nIndex = 1;
							}
							else if (_tcscmp(utf8buf, _T("flag"))==0)
							{
								nIndex = 2;
							}
							else if (_tcscmp(utf8buf, _T("tick"))==0)
							{
								nIndex = 3;
							}
							else if (_tcscmp(utf8buf, _T("cross"))==0)
							{
								nIndex = 4;
							}
							else if (_tcscmp(utf8buf, _T("star"))==0)
							{
								nIndex = 5;
							}
							else if (_tcscmp(utf8buf, _T("question"))==0)
							{
								nIndex = 6;
							}
							else if (_tcscmp(utf8buf, _T("warning"))==0)
							{
								nIndex = 7;
							}
							else if (_tcscmp(utf8buf, _T("idea"))==0)
							{
								nIndex = 8;
							}

							delete [] utf8buf;
							delete [] pValueBuf;
							if (nIndex == m_nIconIndex)
							{
								bFoundByIcon = true;
							}
						}
					}

					if (!searchWord.empty())
					{
						nValueLen = xeList[i]->FindVariableZ("content")->GetValue(0);
						char* pValueBuf = new char[nValueLen + 1];
						memset(pValueBuf, '\0', nValueLen+1);
						xeList[i]->FindVariableZ("content")->GetValue(pValueBuf);
						pValueBuf[nValueLen] = '\0';
						nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
						utf8buf = new TCHAR[nLength+1];
						MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);

						CString strBlock(utf8buf);

						delete [] pValueBuf;
						delete [] utf8buf;

						CString orgBlock = strBlock;
						strBlock.MakeLower();


						int nFound = strBlock.Find(searchWord.c_str());
						if (nFound!=-1)
						{
							bFoundInTextBlock = true;
							if (0 >= m_nIconIndex || bFoundByIcon)
							{
								if (!bFoundInFile)
								{
									ItemData* pItemData = new ItemData;
									pItemData->filename = fileName.c_str();
									pItemData->lineNumber = -1;
									((CListBox)(GetDlgItem(IDC_LIST_REPORT))).SetItemDataPtr(((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString(fileName.c_str()), pItemData);
									((CListBox)(GetDlgItem(IDC_LIST_REPORT))).SetItemDataPtr(((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString(_T("")), NULL);
								}
								bFoundInFile = true;
								ItemData* pItemData = new ItemData;
								pItemData->filename = fileName.c_str();
								pItemData->lineNumber = i;
								CString str(_T("                "));
								int nLeft = nFound - g_config.eachSideWidthOfFoundString;
								int nLength = searchWord.length() + g_config.eachSideWidthOfFoundString*2;
								CString strShow = orgBlock.Mid(nLeft, nLength);
								strShow.Replace(_T('\n'), _T(' '));
								strShow.Replace(_T('\r'), _T(' '));
								strShow = _T("...") + strShow;
								strShow = strShow + _T("...");
								str+=strShow;//(TCHAR*)((TCHAR*)(index+sizeof(CTextBlock)));
								//((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString((TCHAR*)((UINT(p->c_str()))+(11*3+1)*sizeof(TCHAR)));
								((CListBox)(GetDlgItem(IDC_LIST_REPORT))).SetItemDataPtr(((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString(str), pItemData);
							}
						}
						/*					}*/
					}
					

					
					//必须要用户不输搜索词的时候才行
					if (searchWord.empty() && bFoundByIcon)
					{
						
						//取内容
						nValueLen = xeList[i]->FindVariableZ("content")->GetValue(0);
						char* pValueBuf = new char[nValueLen + 1];
						memset(pValueBuf, '\0', nValueLen+1);
						xeList[i]->FindVariableZ("content")->GetValue(pValueBuf);
						pValueBuf[nValueLen] = '\0';
						nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
						utf8buf = new TCHAR[nLength+1];
						MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);

						CString strBlock(utf8buf);

						delete [] pValueBuf;
						delete [] utf8buf;
						//取内容结束

						if (!bFoundInFile)
						{
							ItemData* pItemData = new ItemData;
							pItemData->filename = fileName.c_str();
							pItemData->lineNumber = -1;
							((CListBox)(GetDlgItem(IDC_LIST_REPORT))).SetItemDataPtr(((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString(fileName.c_str()), pItemData);
							((CListBox)(GetDlgItem(IDC_LIST_REPORT))).SetItemDataPtr(((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString(_T("")), NULL);
						}
						bFoundInFile = true;
						ItemData* pItemData = new ItemData;
						pItemData->filename = fileName.c_str();
						pItemData->lineNumber = i;
						CString str(_T("                "));

						CString strShow = strBlock.Left(g_config.eachSideWidthOfFoundString*4);
						strShow.Replace(_T('\n'), _T(' '));
						strShow.Replace(_T('\r'), _T(' '));
						strShow = _T("...") + strShow;
						strShow = strShow + _T("...");
						str+=strShow;//(TCHAR*)((TCHAR*)(index+sizeof(CTextBlock)));
						//((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString((TCHAR*)((UINT(p->c_str()))+(11*3+1)*sizeof(TCHAR)));
						((CListBox)(GetDlgItem(IDC_LIST_REPORT))).SetItemDataPtr(((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString(str), pItemData);
					}
				}
				delete [] xeList;

				if (bFoundInFile)
					((CListBox)(GetDlgItem(IDC_LIST_REPORT))).SetItemDataPtr(((CListBox)(GetDlgItem(IDC_LIST_REPORT))).AddString(_T("")), NULL);
				
			}
			else
			{
				return false;
			}
			return true;
		}
		else
		{
			UnmapViewOfFile(lpvFile);
			CloseHandle(hMap);
			CloseHandle(hFile);
			return false;
		}
		return 0;
	}

	std::vector<std::basic_string<TCHAR>> Split (const std::basic_string<TCHAR> &inString,
                                  const std::basic_string<TCHAR> &separator)
	{
	   std::vector<std::basic_string<TCHAR>> returnVector;
	   std::basic_string<TCHAR>::size_type start = 0;
	   std::basic_string<TCHAR>::size_type end = 0;

	   while ((end=inString.find (separator, start)) != std::string::npos)
	   {
		  returnVector.push_back (inString.substr (start, end-start));
		  start = end+separator.size();
	   }

	   returnVector.push_back (inString.substr (start));

	   return returnVector;

	} 
};
