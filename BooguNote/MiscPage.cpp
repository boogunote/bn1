#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "MiscPage.h"

extern CBooguNoteConfig g_config;

//////////////////////////////////////////////////////////////////////
// Construction

CMiscPage::CMiscPage() : m_bDirty(false)
{
    m_psp.dwFlags |= PSP_USEICONID;
    //m_psp.pszIcon = MAKEINTRESOURCE(IDI_TABICON);
    m_psp.hInstance = _Module.GetResourceInstance();
	//bLaunchAtStartup = g_config.bLaunchAtStartup;
	//nSaveAllElapse = g_config.nSaveAllElapse/1000;
}



//////////////////////////////////////////////////////////////////////
// Message handlers
BOOL CMiscPage::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	m_combBox = GetDlgItem(IDC_COMBO_BLOCK_STATE);
	m_combBox.InsertString(0,_T("Õ­"));
	m_combBox.InsertString(1,_T("¿í"));
	m_combBox.InsertString(2,_T("ÊÕËõ"));
	DoDataExchange();

	return TRUE;
}

LRESULT CMiscPage::OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPPSHNOTIFY lppsn = (LPPSHNOTIFY) lParam;
	if (lppsn->hdr.code == PSN_APPLY)
	{
		
		DoDataExchange(TRUE);
		//if (g_config.bLaunchAtStartup!=bLaunchAtStartup)
		//{
		//	g_config.bLaunchAtStartup=bLaunchAtStartup;
		//	SetLaunchStrategy();
		//}
		/*if (nSaveAllElapse > 3600)
			nSaveAllElapse = 3600;
		if (nSaveAllElapse < 1)
			nSaveAllElapse = 1;*/
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
		WritePrivateProfileString(_T("MainFrame"),_T("RootStorageDir"),g_config.szRootStorageDir,g_config.strConfigFileName);
	}
	bHandled = FALSE;
	return 0;
}