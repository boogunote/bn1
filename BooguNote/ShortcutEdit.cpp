#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "ShortcutEdit.h"
#include "BooguNoteConfig.h"

extern CBooguNoteConfig g_config;

BYTE vkMap[] = {0x00, VK_BACK, VK_TAB, VK_RETURN, VK_CAPITAL, VK_ESCAPE, VK_SPACE, VK_PRIOR, VK_NEXT, VK_END, VK_HOME, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN ,
VK_SNAPSHOT, VK_INSERT, VK_DELETE, 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,
0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,VK_F11,VK_F12,
VK_OEM_1, VK_OEM_PLUS, VK_OEM_MINUS, VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7};
int nVkMapSize = sizeof(vkMap);

//////////////////////////////////////////////////////////////////////
// Construction

CShortcutEdit::CShortcutEdit() : m_bCtrl(false), m_bAlt(false),m_bShift(false),m_bWin(false),m_nKeyIndex(0),m_bDirty(false),m_bShowWin(false)
{

}


//////////////////////////////////////////////////////////////////////
// Message handlers
BOOL CShortcutEdit::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	m_combBox = GetDlgItem(IDC_COMBO_KEY);
	for (int i=0; i< nVkMapSize; ++i)
	{
		m_combBox.InsertString(i,NameFromVKey(vkMap[i]));
	}
	if (m_bShowWin)
	{
		GetDlgItem(IDC_CHECK_WIN).EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_WIN).EnableWindow(FALSE);
	}
	/*m_combBox.InsertString(0,_T(""));
	m_combBox.InsertString(1,_T("0"));
	m_combBox.InsertString(2,_T("1"));*/
	DoDataExchange(false);
	return TRUE;
}

LRESULT CShortcutEdit::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool bCtrl = m_bCtrl;
	bool bAlt = m_bAlt;
	bool bShift = m_bShift;
	int nKeyIndex = m_nKeyIndex;
    DoDataExchange(true);
	if (bCtrl != m_bCtrl || bAlt != m_bAlt || bShift != m_bShift || nKeyIndex != m_nKeyIndex)
	{
		m_bDirty = true;
	}
	else
	{
		m_bDirty = false;
	}
	EndDialog(wID);
	return 0;
}

LRESULT CShortcutEdit::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return -1;
}
