#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "MainFramePage.h"

extern CBooguNoteConfig g_config;
//extern int g_nMainFrameState;

//////////////////////////////////////////////////////////////////////
// Construction

CMainFramePage::CMainFramePage() : m_bDirty(false)
{
    m_psp.dwFlags |= PSP_USEICONID;
    //m_psp.pszIcon = MAKEINTRESOURCE(IDI_TABICON);
    m_psp.hInstance = _Module.GetResourceInstance();
	bLaunchAtStartup = g_config.bLaunchAtStartup;
	nSaveAllElapse = g_config.nSaveAllElapse/1000;
}



//////////////////////////////////////////////////////////////////////
// Message handlers
BOOL CMainFramePage::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	//m_cbFrameState = GetDlgItem(IDC_COMBO_FRAME_STATE);
	//m_cbFrameState.InsertString(0,_T("Normal"));
	//m_cbFrameState.InsertString(1,_T("Dock"));
	//m_cbFrameState.InsertString(2,_T("Dock&AutoHide"));
	//m_cbFrameState.InsertString(3,_T("Hiden"));
	m_cbTxtEncoding = GetDlgItem(IDC_COMBO_TXT_ENCODING);
	m_cbTxtEncoding.InsertString(0,_T("ANSI"));
	m_cbTxtEncoding.InsertString(1,_T("UTF-8"));
	//m_cbTxtEncoding.SetCurSel(g_nMainFrameState);
	//if (bLaunchAtStartup)
	//{
	//	//GetDlgItem(IDC_COMBO_FRAME_STATE).EnableWindow(TRUE);
	//}
	//else
	//{
	//	//GetDlgItem(IDC_COMBO_FRAME_STATE).EnableWindow(FALSE);
	//}

	if (g_config.bUseWorkingDirAsRootStrorageDir)
	{
		GetDlgItem(IDC_EDIT_STORAGE_ROOT).EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STORAGE_ROOT).SetWindowText(_T(""));
	}
	else
	{
		GetDlgItem(IDC_EDIT_STORAGE_ROOT).EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STORAGE_ROOT).SetWindowText(g_config.szRootStorageDir);
	}
	DoDataExchange();

	return TRUE;
}

LRESULT CMainFramePage::OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
	if (lppsn->hdr.code == PSN_APPLY)
	{
		
		DoDataExchange(TRUE);
		if (g_config.bLaunchAtStartup!=bLaunchAtStartup /*|| g_nMainFrameState != m_cbFrameState.GetCurSel()*/)
		{
			g_config.bLaunchAtStartup=bLaunchAtStartup;
			SetLaunchStrategy(bLaunchAtStartup);
		}
		if (nSaveAllElapse > 3600)
			nSaveAllElapse = 3600;
		if (nSaveAllElapse < 1)
			nSaveAllElapse = 1;
		int nChecked = IsDlgButtonChecked(IDC_CHECK_WORK_DIR);
		if (nChecked!=BST_CHECKED)
		{
			//Detect Storage Root
			int nRootStorageDirLen = _tcsclen(g_config.szRootStorageDir);
			if (nRootStorageDirLen>0 && _T(':') == g_config.szRootStorageDir[1] && INVALID_FILE_ATTRIBUTES != GetFileAttributes(g_config.szRootStorageDir))
			{
				int len = _tcsclen(g_config.szRootStorageDir);
				for (int i=len-1; i>=0; --i)
				{
					if (_T('\\') == g_config.szRootStorageDir[i])
					{
						g_config.szRootStorageDir[i] = _T('\0');
					}
					else
					{
						break;
					}
				}
				g_config.bUseWorkingDirAsRootStrorageDir = false;
				WritePrivateProfileString(_T("MainFrame"),_T("UseWorkingDirAsRootStrorageDir"),_T("00"),g_config.strConfigFileName);
				WritePrivateProfileString(_T("MainFrame"),_T("RootStorageDir"),g_config.szRootStorageDir,g_config.strConfigFileName);
			}
			else
			{
				MessageBox( _T("无效“相对路径起始”，使用BooguNote.exe所在路径作为“相对路径起始”"), _T("警告"), MB_OK|MB_ICONEXCLAMATION);
				g_config.bUseWorkingDirAsRootStrorageDir = true;
				WritePrivateProfileString(_T("MainFrame"),_T("UseWorkingDirAsRootStrorageDir"),_T("01"),g_config.strConfigFileName);
				g_config.szRootStorageDir[0]=_T('\0');
				WritePrivateProfileString(_T("MainFrame"),_T("RootStorageDir"),_T(""),g_config.strConfigFileName);
			}
			
		}
		else
		{
			g_config.bUseWorkingDirAsRootStrorageDir = true;
			WritePrivateProfileString(_T("MainFrame"),_T("UseWorkingDirAsRootStrorageDir"),_T("01"),g_config.strConfigFileName);
			g_config.szRootStorageDir[0]=_T('\0');
			WritePrivateProfileString(_T("MainFrame"),_T("RootStorageDir"),_T(""),g_config.strConfigFileName);
		}
	}
	bHandled = FALSE;
	return 0;
}
LRESULT CMainFramePage::OnBnClickedCheckStartup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//int nChecked = IsDlgButtonChecked(IDC_CHECK_STARTUP);
	//if (nChecked==BST_CHECKED)
	//{
	//	GetDlgItem(IDC_COMBO_FRAME_STATE).EnableWindow(TRUE);
	//	m_cbFrameState.SetCurSel(1);
	//}
	//else
	//{
	//	GetDlgItem(IDC_COMBO_FRAME_STATE).EnableWindow(FALSE);
	//}
	return 0;
}

LRESULT CMainFramePage::OnBnClickedCheckWorkDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nChecked = IsDlgButtonChecked(IDC_CHECK_WORK_DIR);
	if (nChecked==BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_STORAGE_ROOT).EnableWindow(FALSE);
		TCHAR _strExePath[MAX_PATH+10];
		GetModuleFileName(_Module.m_hInst, _strExePath, MAX_PATH);
		CString strPath = _strExePath;
		strPath.Delete(strPath.ReverseFind(_T('\\'))+1, strPath.GetLength());

		GetDlgItem(IDC_EDIT_STORAGE_ROOT).SetWindowText(_T(""));
		_tcsncpy(g_config.szRootStorageDir,strPath.GetBuffer(MAX_PATH), MAX_PATH);
		strPath.ReleaseBuffer();
	}
	else
	{
		GetDlgItem(IDC_EDIT_STORAGE_ROOT).EnableWindow(TRUE);
	}
	return 0;
}
