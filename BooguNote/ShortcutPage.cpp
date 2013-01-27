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
			CString strMsg = _T("你刚才设置的快捷键 \"");
			strMsg += keyName;
			strMsg += _T("\" 与功能 \"");
			strMsg += szCommandName;
			strMsg += _T("\" 的快捷键冲突");
			MessageBox(strMsg, _T("警告"), MB_OK);
			return true;
		}
	}
	for(int i=0; i<g_config.nAccelMainFrmCount; ++i)
	{
		if (((g_config.nAccelGlobalCount+i)!=nIndex) && keyName == GetHumanReadableAccel(g_config.AccelMainFrm[i]))
		{
			TCHAR szCommandName[1024];
			m_wlcShortcut.GetItemText(g_config.nAccelGlobalCount+i,0, szCommandName, 1024);
			CString strMsg = _T("你刚才设置的快捷键 \"");
			strMsg += keyName;
			strMsg += _T("\" 与功能 \"");
			strMsg += szCommandName;
			strMsg += _T("\" 的快捷键冲突");
			MessageBox(strMsg, _T("警告"), MB_OK);
			return true;
		}
	}
	for(int i=0; i<g_config.nAccelViewCount; ++i)
	{
		if (((g_config.nAccelGlobalCount+g_config.nAccelMainFrmCount+i)!=nIndex)&&keyName == GetHumanReadableAccel(g_config.AccelView[i]))
		{
			TCHAR szCommandName[1024];
			m_wlcShortcut.GetItemText(g_config.nAccelGlobalCount+g_config.nAccelMainFrmCount+i,0, szCommandName, 1024);
			CString strMsg = _T("你刚才设置的快捷键 \"");
			strMsg += keyName;
			strMsg += _T("\" 与功能 \"");
			strMsg += szCommandName;
			strMsg += _T("\" 的快捷键冲突");
			MessageBox(strMsg, _T("警告"), MB_OK);
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
	m_wlcShortcut.InsertColumn(0, _T("快捷键"), LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM, width/3, 0);
	m_wlcShortcut.InsertColumn(0, _T("功能"), LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM, width*2/3, 0);	
    //DoDataExchange();


	CString name;
	LVITEM LvItem;  // ListView Item struct
	LvItem.mask=LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of test

	//-------------Global Shortcut---------------------------------------------------
	int i=0;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("靠边窗口"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("窗口全局隐藏/解除全局隐藏"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("全局粘贴"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("全局焦点"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("靠边自动隐藏窗口"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("全局粘贴到文本"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("截屏"); // Text to display (can be from a char variable) (Items)
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
	LvItem.pszText=_T("新建文档"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("打开文档"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("上个文档"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("下个文档"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("搜索"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("显示文档列表"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("选项"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("关闭当前文档"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelMainFrm[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("打开文件管理窗口"); // Text to display (can be from a char variable) (Items)
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
	LvItem.pszText=_T("拷贝"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("剪切"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("粘贴"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("重做操作"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("回滚操作"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("插入时间戳"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);
	
	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("保存文档"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("另存文档"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("加粗文字块中文字"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("取消选择文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("创建子文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("创建下个文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("创建上个文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("创建父文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("删除"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("把焦点移动到最顶部文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("把焦点移动到最底部文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("右移视图，显示视图最左边界"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("将被选择文字块上移"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("将被选择文字块下移"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("将被选择文字块左移"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("将被选择文字块右移"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("在文字块中新起一行"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("上翻"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("下翻"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("左翻"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("右翻"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("选择所有文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("循环切换文字块状态，窄->宽->收缩"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("切换文字块图标到右键图标列中的上一个"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("切换文字块图标到右键图标列中的下一个"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("分割文字块"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("展开/收缩节点"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("搜索"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("显示与子文字块的连线"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("隐藏与子文字块的连线"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("完全展开节点"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("移动焦点到父文字块"); // Text to display (can be from a char variable) (Items)
m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("设置文字色"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("设置背景色"); // Text to display (can be from a char variable) (Items)
	m_wlcShortcut.InsertItem(&LvItem);
	LvItem.iSubItem=1;
	name = GetHumanReadableAccel(g_config.AccelView[i-nOffset]);
	LvItem.pszText=name.GetBuffer(0xFF);
	name.ReleaseBuffer();
	m_wlcShortcut.SetItem(&LvItem);

	++i;	

	LvItem.iItem=i;          // choose item  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=_T("另存文字块"); // Text to display (can be from a char variable) (Items)
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
