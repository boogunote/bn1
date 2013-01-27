#include "stdafx.h"
#include "Common.h"
#include "resource.h"
#include "ShortcutPage.h"
#include "ShortcutEdit.h"
#include "BooguNoteConfig.h"

extern CBooguNoteConfig g_config;
extern BYTE vkMap[];
extern int nVkMapSize;

//////////////////////////////////////////////////////////////////////
// Construction

CShortcutPage::CShortcutPage() : m_bDirty(false)
{
    m_psp.dwFlags |= PSP_USEICONID;
    //m_psp.pszIcon = MAKEINTRESOURCE(IDI_TABICON);
    m_psp.hInstance = _Module.GetResourceInstance();
}

bool CShortcutPage::ConfictDetect(ACCEL& accel, bool bGlobalKey, int nIndex)
{
	if (0x00 == accel.key)
		return false;

	CString keyName;
	if (bGlobalKey)
	{
		keyName = GetHumanReadableGlobalAccel(accel);
	}
	else
	{
		keyName = GetHumanReadableAccel(accel);
	}
	for(int i=0; i<g_config.nAccelGlobalCount; ++i)
	{
		if (i!=nIndex && keyName == GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]))
		{
			TCHAR szCommandName[1024];
			m_wlcShortcut.GetItemText(i,0, szCommandName, 1024);
			CString strMsg = _T("��ղ����õĿ�ݼ� \"");
			strMsg += keyName;
			strMsg += _T("\" �빦�� \"");
			strMsg += szCommandName;
			strMsg += _T("\" �Ŀ�ݼ���ͻ");
			MessageBox(strMsg, _T("����"), MB_OK);
			return true;
		}
	}
	for(int i=0; i<g_config.nAccelMainFrmCount; ++i)
	{
		if (((g_config.nAccelGlobalCount+i)!=nIndex) && keyName == GetHumanReadableAccel(g_config.AccelMainFrm[i]))
		{
			TCHAR szCommandName[1024];
			m_wlcShortcut.GetItemText(g_config.nAccelGlobalCount+i,0, szCommandName, 1024);
			CString strMsg = _T("��ղ����õĿ�ݼ� \"");
			strMsg += keyName;
			strMsg += _T("\" �빦�� \"");
			strMsg += szCommandName;
			strMsg += _T("\" �Ŀ�ݼ���ͻ");
			MessageBox(strMsg, _T("����"), MB_OK);
			return true;
		}
	}
	for(int i=0; i<g_config.nAccelViewCount; ++i)
	{
		if (((g_config.nAccelGlobalCount+g_config.nAccelMainFrmCount+i)!=nIndex)&&keyName == GetHumanReadableAccel(g_config.AccelView[i]))
		{
			TCHAR szCommandName[1024];
			m_wlcShortcut.GetItemText(g_config.nAccelGlobalCount+g_config.nAccelMainFrmCount+i,0, szCommandName, 1024);
			CString strMsg = _T("��ղ����õĿ�ݼ� \"");
			strMsg += keyName;
			strMsg += _T("\" �빦�� \"");
			strMsg += szCommandName;
			strMsg += _T("\" �Ŀ�ݼ���ͻ");
			MessageBox(strMsg, _T("����"), MB_OK);
			return true;
		}
	}
	return false;

}

//////////////////////////////////////////////////////////////////////
// Message handlers
LRESULT CShortcutPage::OnDClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)
{
	bHandled = TRUE;
	CShortcutEdit shtEditDlg;
	int nIndex = m_wlcShortcut.GetNextItem(-1,LVNI_FOCUSED);
	if (nIndex < g_config.nAccelGlobalCount)
	{
		if (g_config.AccelGlobal[nIndex].fVirt & MOD_CONTROL)
			shtEditDlg.m_bCtrl = true;
		if (g_config.AccelGlobal[nIndex].fVirt & MOD_ALT )
			shtEditDlg.m_bAlt = true;
		if (g_config.AccelGlobal[nIndex].fVirt & MOD_SHIFT )
			shtEditDlg.m_bShift = true;
		if (g_config.AccelGlobal[nIndex].fVirt & MOD_WIN )
			shtEditDlg.m_bWin = true;
		shtEditDlg.m_bShowWin = true;
		for (int i=0; i<nVkMapSize; ++i)
		{
			if (vkMap[i]==g_config.AccelGlobal[nIndex].key)
			{
				shtEditDlg.m_nKeyIndex = i;
				break;
			}
		}
	}
	else if ((g_config.nAccelGlobalCount<=nIndex) && (nIndex < g_config.nAccelGlobalCount+g_config.nAccelMainFrmCount))
	{
		if (g_config.AccelMainFrm[nIndex-g_config.nAccelGlobalCount].fVirt & FCONTROL)
			shtEditDlg.m_bCtrl = true;
		if (g_config.AccelMainFrm[nIndex-g_config.nAccelGlobalCount].fVirt & FALT )
			shtEditDlg.m_bAlt = true;
		if (g_config.AccelMainFrm[nIndex-g_config.nAccelGlobalCount].fVirt & FSHIFT )
			shtEditDlg.m_bShift = true;
		shtEditDlg.m_bShowWin = false;
		for (int i=0; i<nVkMapSize; ++i)
		{
			if (vkMap[i]==g_config.AccelMainFrm[nIndex-g_config.nAccelGlobalCount].key)
			{
				shtEditDlg.m_nKeyIndex = i;
				break;
			}
		}
	}
	else
	{
		if (g_config.AccelView[nIndex-g_config.nAccelGlobalCount-g_config.nAccelMainFrmCount].fVirt & FCONTROL)
			shtEditDlg.m_bCtrl = true;
		if (g_config.AccelView[nIndex-g_config.nAccelGlobalCount-g_config.nAccelMainFrmCount].fVirt & FALT )
			shtEditDlg.m_bAlt = true;
		if (g_config.AccelView[nIndex-g_config.nAccelGlobalCount-g_config.nAccelMainFrmCount].fVirt & FSHIFT )
			shtEditDlg.m_bShift = true;
		shtEditDlg.m_bShowWin = false;
		for (int i=0; i<nVkMapSize; ++i)
		{
			if (vkMap[i]==g_config.AccelView[nIndex-g_config.nAccelGlobalCount-g_config.nAccelMainFrmCount].key)
			{
				shtEditDlg.m_nKeyIndex = i;
				break;
			}
		}
	}

	if (IDOK == shtEditDlg.DoModal())
	{
		if (shtEditDlg.m_bDirty)
		{
			if (nIndex < g_config.nAccelGlobalCount)
			{
				ACCEL accel;
				accel.fVirt = 0;
				if (shtEditDlg.m_bCtrl)
					 accel.fVirt |= MOD_CONTROL;
				if (shtEditDlg.m_bAlt)
					 accel.fVirt |= MOD_ALT;
				if (shtEditDlg.m_bShift)
					 accel.fVirt |= MOD_SHIFT;
				if (shtEditDlg.m_bWin)
					 accel.fVirt |= MOD_WIN;
				accel.key = vkMap[shtEditDlg.m_nKeyIndex];

				if (!ConfictDetect(accel, true, nIndex))
				{
					g_config.AccelGlobal[nIndex].fVirt = accel.fVirt;
					g_config.AccelGlobal[nIndex].key = accel.key;
					m_wlcShortcut.SetItemText(nIndex, 1, GetHumanReadableGlobalAccel(g_config.AccelGlobal[nIndex]));
				}
			}
			else if ((g_config.nAccelGlobalCount<=nIndex) && (nIndex < g_config.nAccelGlobalCount+g_config.nAccelMainFrmCount))
			{
				ACCEL accel;
				accel.fVirt = FVIRTKEY;
				if (shtEditDlg.m_bCtrl)
					accel.fVirt |= FCONTROL;
				if (shtEditDlg.m_bAlt)
					accel.fVirt |= FALT;
				if (shtEditDlg.m_bShift)
					accel.fVirt |= FSHIFT;

				accel.key = vkMap[shtEditDlg.m_nKeyIndex];

				if (!ConfictDetect(accel, false, nIndex))
				{
					g_config.AccelMainFrm[nIndex-g_config.nAccelGlobalCount].fVirt = accel.fVirt;
					g_config.AccelMainFrm[nIndex-g_config.nAccelGlobalCount].key = accel.key;
					m_wlcShortcut.SetItemText(nIndex, 1, GetHumanReadableAccel(accel));
				}
			}
			else
			{
				ACCEL accel;
				accel.fVirt = FVIRTKEY;
				if (shtEditDlg.m_bCtrl)
					accel.fVirt |= FCONTROL;
				if (shtEditDlg.m_bAlt)
					accel.fVirt |= FALT;
				if (shtEditDlg.m_bShift)
					accel.fVirt |= FSHIFT;

				accel.key = vkMap[shtEditDlg.m_nKeyIndex];

				if (!ConfictDetect(accel, false, nIndex))
				{
					g_config.AccelView[nIndex-g_config.nAccelGlobalCount-g_config.nAccelMainFrmCount].fVirt = accel.fVirt;
					g_config.AccelView[nIndex-g_config.nAccelGlobalCount-g_config.nAccelMainFrmCount].key = accel.key;
					m_wlcShortcut.SetItemText(nIndex, 1, GetHumanReadableAccel(accel));
				}
			}
			
			m_bDirty = true;
		}
	}
	return 0;
}

BOOL CShortcutPage::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	m_wlcShortcut = GetDlgItem(IDC_LIST_SHORTCUT);
	m_wlcShortcut.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	//m_wlcShortcut.SetExtendedListViewStyle(LVS_EX_LABELTIP /*| LVS_EX_TWOCLICKACTIVATE*/);
	//LVCOLUMN LvCol; // Make Coluom struct for ListView
 //   memset(&LvCol,0,sizeof(LvCol)); // Reset Coluom
	//LvCol.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM; // Type of mask
	//LvCol.cx=0x28;                                // width between each coloum
	//LvCol.pszText=_T("Item");                     // First Header
	//LvCol.cx=0x42;
	//SendMessage(m_wlcShortcut,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol); // Insert/Show the coloum
	//LvCol.pszText=_T("Sub Item1");                          // Next coloum
 //   SendMessage(m_wlcShortcut,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol); // ...
	//LvCol.pszText=_T("Sub Item2");                       //
	// in detailed view i MUST add a column else nothing will show
	CRect rc;
	m_wlcShortcut.GetClientRect(&rc);
	int width = rc.Width();
	if(!(m_wlcShortcut.GetStyle() & WS_VSCROLL))
		width -= GetSystemMetrics(SM_CXVSCROLL); // avoids appearence of horizontal one
	m_wlcShortcut.InsertColumn(0, _T("��ݼ�"), LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM, width/3, 0);
	m_wlcShortcut.InsertColumn(0, _T("����"), LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM, width*2/3, 0);	
    //DoDataExchange();


	CString name;
	LVITEM LvItem;  // ListView Item struct
	LvItem.mask=LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of test

	//-------------Global Shortcut---------------------------------------------------
	int i=0;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("���ߴ���"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����ȫ������/���ȫ������"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ȫ��ճ��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ȫ�ֽ���"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�����Զ����ش���"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ȫ��ճ�����ı�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	//-----------------MainFrame Shortcut------------------------------------------------------
	++i;
	int nOffset = i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�½��ĵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("���ĵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�ϸ��ĵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�¸��ĵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("��ʾ�ĵ��б�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ѡ��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�رյ�ǰ�ĵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("���ļ�������"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	//-----------------View Shortcut------------------------------------------------------
	++i;
	nOffset = i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ճ��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("��������"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�ع�����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����ʱ���"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);
	
	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�����ĵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����ĵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�Ӵ����ֿ�������"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ȡ��ѡ�����ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("���������ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�����¸����ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�����ϸ����ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("���������ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ɾ��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�ѽ����ƶ���������ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�ѽ����ƶ�����ײ����ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("������ͼ����ʾ��ͼ����߽�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����ѡ�����ֿ�����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����ѡ�����ֿ�����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����ѡ�����ֿ�����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����ѡ�����ֿ�����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�����ֿ�������һ��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�Ϸ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�·�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�ҷ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ѡ���������ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("ѭ���л����ֿ�״̬��խ->��->����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�л����ֿ�ͼ�굽�Ҽ�ͼ�����е���һ��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�л����ֿ�ͼ�굽�Ҽ�ͼ�����е���һ��"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�ָ����ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("չ��/�����ڵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("����"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("��ʾ�������ֿ������"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�����������ֿ������"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("��ȫչ���ڵ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("�ƶ����㵽�����ֿ�"); // Text to display (can be from a char variable) (Items)
m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("��������ɫ"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("���ñ���ɫ"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("������ֿ�"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	return TRUE;
}

//int CShortcutPage::OnApply()
//{
//    return DoDataExchange(true) ? PSNRET_NOERROR : PSNRET_INVALID;
//}
