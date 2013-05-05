// BooguNoteView.cpp : implementation of the CBooguNoteView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Winreg.h>
//#include "resource.h"
#include "BooguNoteText.h"
#include "BooguNoteWidget.h"
#include "BooguNoteHandle.h"
#include "BooguNoteIcon.h"
#include "BooguNoteConfig.h"
#include "BooguNoteView.h"
#include "SearchDlg.h"
#include "RepairShortcut.h"
#include <typeinfo>
#include <string>
#include <comutil.h>
#include "filedialogfilter.h"
#include "Common.h"
#include "legacy_xml.h"
#include "CXml/Xml.h"
#include "CXml/Xsl.h"
#include <atlenc.h>
#include <sys/timeb.h>
#include <time.h>

using namespace JWXml;

using namespace std;

UINT BOOGUNOTE_TYPE = 0;

extern CBooguNoteConfig g_config;
extern CUtil	g_util;
extern TCHAR strExePath[MAX_PATH+10];

extern CSearchDlg* g_pSearchDlg;
extern CString g_SearchDir;
extern CString g_SearchWords;

HRESULT CreateHost(
	CBooguNoteView* pParentWindow, 
	const CREATESTRUCT *pcs,
	PNOTIFY_CALL pnc,
	CBooguNoteText **pptec)
{
	HRESULT hr = E_FAIL;
    GdiSetBatchLimit(1);

	CBooguNoteText *phost = new CBooguNoteText(pcs->x, pcs->y, pcs->cx, pcs->cy);

	if(phost)
	{
		if (phost->Init(pParentWindow->m_hWnd,&pParentWindow->m_ptOffset,&pParentWindow->m_pTextBlockFocused, pcs, pnc))
		{
			*pptec = phost;
			hr = S_OK;
		}
	}

	if (FAILED(hr))
	{
		delete phost;
	}

	return TRUE;
}

HRESULT CreateTextControl(
	CBooguNoteView* pParentWindow,
	HWND hwndParentParent,
	DWORD dwStyle,
	TCHAR *pszText,
	LONG lLeft,
	LONG lTop,
	LONG lWidth,
	LONG lHeight,
	CBooguNoteText **pptec)
{
	CREATESTRUCT cs;

	// No text for text control yet
	cs.lpszName = pszText;

	// No parent for this window
	cs.hwndParent = hwndParentParent;

	// Style of control that we want
	cs.style = dwStyle;
	cs.dwExStyle = 0;

	// Rectangle for the control
	cs.x = lLeft;
	cs.y = lTop;
	cs.cy = lHeight;
	cs.cx = lWidth;

	// Create our handler for the text control
	return CreateHost(pParentWindow, &cs, NULL, pptec);
}
//std::vector<std::basic_string<TCHAR>> Split (const std::basic_string<TCHAR> &inString,
//                                  const std::basic_string<TCHAR> &separator)
//{
//   std::vector<std::basic_string<TCHAR>> returnVector;
//   std::basic_string<TCHAR>::size_type start = 0;
//   std::basic_string<TCHAR>::size_type end = 0;
//
//   while ((end=inString.find (separator, start)) != std::string::npos)
//   {
//      returnVector.push_back (inString.substr (start, end-start));
//      start = end+separator.size();
//   }
//
//   returnVector.push_back (inString.substr (start));
//
//   return returnVector;
//
//} 

// Associates an image with a menu item
void CBooguNoteView::AssociateImage(CMenuItemInfo& mii, MenuItemData * pMI)
{
    switch (mii.wID)
    {
    case ID_POPUPMENU_TODO:
        pMI->iImage = 0;
        break;
    case ID_POPUPMENU_DONE:
        pMI->iImage = 1;
        break;
    case ID_POPUPMENU_CROSS:
        pMI->iImage = 2;
        break;
    case ID_POPUPMENU_STAR:
        pMI->iImage = 3;
        break;
	case ID_POPUPMENU_QUESTION:
		pMI->iImage = 4;
        break;
	case ID_POPUPMENU_WARNING:
		pMI->iImage = 5;
		break;
	case ID_POPUPMENU_IDEA:
		pMI->iImage = 6;
		break;
	default:
        pMI->iImage = -1;
		break;
    }
}

BOOL CBooguNoteView::PreTranslateMessage(MSG* pMsg)
{
	if(m_haccelerator != NULL)
    {
        if(::TranslateAccelerator(m_hWnd, m_haccelerator, pMsg))
            return TRUE;
    }
    //return CWindow::IsDialogMessage(pMsg);

	//pMsg;
	return FALSE;
}

BOOL CBooguNoteView::OnIdle()
{
	return FALSE;
}

void CBooguNoteView::DrawTranspRoundRect(HDC hDC, CRect &rc)
{
	CRect  rect;
	rect.SetRect( 0, 0, rc.Width(), rc.Height());
	CDC compDC;
	compDC.CreateCompatibleDC(hDC);
	if (compDC.m_hDC==NULL)
	{
		DWORD e = GetLastError();
	}
	CBitmap compBmp;
	compBmp.CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	compDC.SelectBitmap(compBmp);
	CBrush whiteBrush;
	whiteBrush.CreateSolidBrush(RGB(0xFF,0xFF,0xFF));
	compDC.FillRect(&rect,whiteBrush);
	//CPen solidPen;
	//solidPen.CreatePen(PS_SOLID, 1, RGB(0,0,0XFF));
	//compDC.SelectPen(solidPen);
	CBrush blueBrush;
    blueBrush.CreateSolidBrush(g_config.clrScrollBar);
	compDC.SelectBrush(blueBrush);
	compDC.RoundRect(&rect, CPoint(10,10));

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 0, 0 }; 
	blend.SourceConstantAlpha = g_config.nScrollBarTransparentRatio*255/100;

	AlphaBlend(hDC, rc.left, rc.top, rc.Width(), rc.Height(), compDC, 0, 0, rc.Width(), rc.Height(), blend);
	compBmp.DeleteObject();
	compDC.DeleteDC();

	CDCHandle dc(hDC);
	CPen penScrollBarBorder;
	penScrollBarBorder.CreatePen(PS_SOLID, 1, g_config.clrScrollBarBorder);
	dc.SelectPen(penScrollBarBorder);
	dc.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));
	dc.RoundRect(&rc, CPoint(10,10));
	//DeleteDC(compDC);
}

LRESULT CBooguNoteView::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	ATLASSERT(::IsWindow(m_hWnd));
	CPaintDC realDC(m_hWnd);

	CRect rcWindow;
	GetWindowRect(&rcWindow);

	ScreenToClient(&rcWindow);
	CMemoryDC dc(realDC.m_hDC, rcWindow);
	//realDC.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y);
	dc.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y);
	DoPaint(dc.m_hDC);
	//Paint Scrollbar
	CBrush thumbBrush;
	thumbBrush.CreateSolidBrush(g_config.clrThumbnail);
	
	CPen penThumbnailBorder;
	penThumbnailBorder.CreatePen(PS_SOLID, 1, g_config.clrThumbnailBorder);
	
	realDC.SetViewportOrg(0, 0);
	dc.SetViewportOrg(0, 0);
	if (m_nVRange>rcWindow.Height())
	{
		DrawTranspRoundRect(dc.m_hDC, m_rcVScroll);
		dc.SelectPen(penThumbnailBorder);
		dc.SelectBrush(thumbBrush);
		dc.RoundRect(&m_rcVThumb, CPoint(8,8));
	}
	if (g_config.bEnableHScrollBar && m_nHRange>rcWindow.Width())
	{
		DrawTranspRoundRect(dc.m_hDC, m_rcHScroll);
		dc.SelectPen(penThumbnailBorder);
		dc.SelectBrush(thumbBrush);
		dc.RoundRect(&m_rcHThumb, CPoint(8,8));
	}
	bHandled = TRUE;
	return 0;
}

int CBooguNoteView::ShowBranch(int nIndex, CDCHandle& dc)
{
	if (!g_config.bShowAllBranchLines)
	{
		if (!m_textBlockList[nIndex]->m_bShown || !m_textBlockList[nIndex]->m_bShowBranch)
		{
			++nIndex;
			return nIndex;
		}
	}
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, g_config.clrBullet/*RGB(230,230,230)*/);
	dc.SelectPen(pen);

	int nBeginIndex = nIndex;
	int nEndIndex = nIndex;
	int nLevel = m_textBlockList[nIndex]->m_nLevel;
	int x1 = m_textBlockList[nBeginIndex]->m_pTextHandle->x + m_textBlockList[nBeginIndex]->m_pTextHandle->width/2;
	int i = nIndex+1;
	for(; i<m_textBlockList.size();)
	{
		if (!m_textBlockList[i]->m_bShown)
		{
			++i;
			continue;
		}
		else if (m_textBlockList[i]->m_nLevel-1 == nLevel)
		{
			//draw horizon line
			int y1 = m_textBlockList[i]->m_pTextHandle->y + m_textBlockList[i]->m_pTextHandle->width/2;
			int x2 = m_textBlockList[i]->m_pTextHandle->x;
			int y2 = y1;
			dc.MoveTo(x1,y1);
			dc.LineTo(x2,y2);
			nEndIndex = i;
			i = ShowBranch(i, dc);
		}
		else if (m_textBlockList[i]->m_nLevel-1 > nLevel)
		{
			////draw horizon line
			//int y1 = m_textBlockList[i]->m_pTextHandle->y + m_textBlockList[i]->m_pTextHandle->width/2;
			//int x2 = m_textBlockList[i]->m_pTextHandle->x;
			//int y2 = y1;
			//dc.MoveTo(x1,y1);
			//dc.LineTo(x2,y2);
			//nEndIndex = i;
			i = ShowBranch(i, dc);
		}
		else if (m_textBlockList[i]->m_nLevel <= nLevel)
		{
			break;
		}
	}
	if (nBeginIndex!=nEndIndex)
	{
		//draw vertical line
		int x1 = m_textBlockList[nBeginIndex]->m_pTextHandle->x + m_textBlockList[nBeginIndex]->m_pTextHandle->width/2;
		int y1 = m_textBlockList[nBeginIndex]->m_pTextHandle->y + m_textBlockList[nBeginIndex]->m_pTextHandle->width;
		int x2 = x1;
		int y2 = m_textBlockList[nEndIndex]->m_pTextHandle->y + m_textBlockList[nEndIndex]->m_pTextHandle->width/2;
		dc.MoveTo(x1,y1);
		dc.LineTo(x2,y2);
	}
	return i;
}

void CBooguNoteView::DoPaint(HDC hdc)
{
	CDCHandle dc(hdc);
	//empty whole view.
	CRect rc, rcWindow;
	GetWindowRect(&rcWindow);
	rc.SetRect(m_ptOffset.x, m_ptOffset.y, m_ptOffset.x+rcWindow.Width(), m_ptOffset.y + rcWindow.Height());

	//dc.SelectPen((HPEN)GetStockObject(WHITE_PEN));
	//dc.SelectBrush((HBRUSH)GetStockObject(WHITE_BRUSH));
	//dc.Rectangle(&rc);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, g_config.clrViewBackground/*RGB(230,230,230)*/);
	dc.SelectPen(pen);
	CBrush brush;
    brush.CreateSolidBrush(g_config.clrViewBackground);
	dc.SelectBrush(brush);
	dc.Rectangle(&rc);

	//dc.SetBkColor(RGB(77,77,77));

	RECT const * prc=NULL;
	
	dc.SelectPen((HPEN)GetStockObject(NULL_PEN));
	dc.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));

	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if (g_config.bEnterURLDetect)
	    {
	       (*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0); 
	    }
	    else
	    {
	       (*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, FALSE, 0, 0); 
	    }
		
		if ((*ppTextBlock)->m_bShown)
		{
			(*ppTextBlock)->DoPaint(dc.m_hDC);
			//draw right strip for a text block
			if (g_config.bShowTextBlockRightBorder && ((!(*ppTextBlock)->m_bSelected && (*ppTextBlock) != m_pTextBlockFocused) &&TEXT_BLOCK_SHRINK!=(*ppTextBlock)->m_nExpandState))
			{
				CPen pen;
				pen.CreatePen(PS_SOLID, 1, g_config.clrTextBlockRightBorder/*RGB(230,230,230)*/);
				dc.SelectPen(pen);
				CRect rcControl;
				(*ppTextBlock)->GetControlRect(&rcControl);
				CRect rcParent;
				GetClientRect(rcParent);
				if (rcControl.right>(m_ptOffset.x + rcParent.Width()/*-::GetSystemMetrics(SM_CXSIZEFRAME)*/-1))
					rcControl.right = m_ptOffset.x + rcParent.Width()/*-::GetSystemMetrics(SM_CXSIZEFRAME)*/-1;
				dc.MoveTo(rcControl.right, rcControl.top, 0);
				dc.LineTo(rcControl.right+2/*+PointsToLogical(g_config.fontSize)/2*/, rcControl.top);
				dc.MoveTo(rcControl.right, rcControl.top, 0);
				dc.LineTo(rcControl.right, rcControl.bottom);
				dc.MoveTo(rcControl.right+2, rcControl.top, 0);
				dc.LineTo(rcControl.right+2, rcControl.bottom+1);
				dc.MoveTo(rcControl.right, rcControl.bottom, 0);
				dc.LineTo(rcControl.right+2/*+PointsToLogical(g_config.fontSize)/2*/, rcControl.bottom);
				//dc.SelectPen((HPEN)GetStockObject(NULL_PEN));
			}			
		}
	}

	for(int i=0; i<m_textBlockList.size();)
	{
		i = ShowBranch(i, dc);
	}
}

bool CBooguNoteView::OpenFile()
{
	if(!LoadData())
		if(!LoadData0006())
			if(!LoadData0005())
				if (!LoadOldData0004())
					if (!LoadOldData0003())
					{
						MessageBox(_T("不能打开该文件"));
						return false;
					}
	CalcLayout();
	Invalidate(TRUE);
	//m_bInitialized = true;
	return true;
}

void CBooguNoteView::LoadAccel()
{
	if(NULL!=m_haccelerator)
	{
		DestroyAcceleratorTable(m_haccelerator);
	}
	m_haccelerator = CreateAcceleratorTable(g_config.AccelView, g_config.nAccelViewCount);
}

LRESULT CBooguNoteView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//m_haccelerator = AtlLoadAccelerators(IDR_VIEW);
	LoadAccel();
	CCoolContextMenu<CBooguNoteView>::GetSystemSettings();
	//m_ImageList.Attach(g_config.hImageList);
	BOOGUNOTE_TYPE = RegisterClipboardFormat(_T("Boogu Content"));
	//test single line height
	CBooguNoteText* pTextBlockTest = CreateTextBlock(0,0,g_config.wideWidth,_T("Test"));; 
	pTextBlockTest->m_nExpandState = TEXT_BLOCK_NARROW;
	pTextBlockTest->m_bShown = true;
	pTextBlockTest->m_pTextHandle->state = TEXT_HANDLE_NULL;
	pTextBlockTest->m_nLevel = 0;
	//m_textBlockList.push_back(pTextBlock);
	//pTextBlock->m_bSelected = true;
	//m_pTextBlockFocused = pTextBlock;
	pTextBlockTest->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	m_nSingleLineHeight = pTextBlockTest->GetClientRect()->bottom - pTextBlockTest->GetClientRect()->top;
	delete pTextBlockTest;

	if (m_fileDirectory != _T(""))
	{
		m_bDirty = false;
		if(!OpenFile())
		{
			PostMessage(WM_CLOSE);
			return -1;
		}
	}
	else
	{
		CBooguNoteText* pTextBlock = NULL; 
		if(g_config.bWideTextBlock)
		{
			pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
		}
		else
		{
			pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		pTextBlock->m_bShown = true;
		pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
		pTextBlock->m_nLevel = 0;
		m_textBlockList.push_back(pTextBlock);
		//pTextBlock->m_bSelected = true;
		m_pTextBlockFocused = pTextBlock;
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
		//m_nSingleLineHeight = m_pTextBlockFocused->GetClientRect()->bottom - m_pTextBlockFocused->GetClientRect()->top;
		//m_pTextBlockFocused->RegisterDragDrop();
		m_nLastClicked = 0;
		CalcLayout();
		CalcScrollBar();
		m_bDirty = true;
		//GetParent().SendMessageW(WM_NCPAINT);
	}
	m_bInitialized = true;
	DragAcceptFiles();
	//m_pToolTip = new CToolTipCtrl(m_hWnd);
	//m_pToolTip->Create(m_hWnd);
	//m_pToolTip->AddTool(m_hWnd, _T("Test"));
	//CreateToolTipForRect(m_hWnd);
	return 0;
}
bool CBooguNoteView::LoadOldData0003()
{
	SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	
	//MessageBox(_T("ddfdf"));
	HANDLE hFile = CreateFileW(m_fileDirectory, GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//DWORD error = GetLastError();
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	char * lpvFile = (char *)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	UINT bufSize = dwFileSize*3;
	TCHAR* _tbuf = new TCHAR[bufSize];
	int nLength = MultiByteToWideChar(CP_UTF8, 0, lpvFile, dwFileSize, _tbuf, bufSize);

	bool haveNode = false;

	//test version
	TCHAR _tVersion[5];
	//_tcsncpy(_tVersion, _tbuf, 5);
	_tVersion[0] = _tbuf[0];
	_tVersion[1] = _tbuf[1];
	_tVersion[2] = _tbuf[2];
	_tVersion[3] = _tbuf[3];
	_tVersion[4] = _T('\0');
	CString szVersion(_tVersion);
	if (_T("0003")==szVersion)
	{
		basic_string<TCHAR> text(_tbuf);
		vector< basic_string<TCHAR> > vecLines(Split(text, _T("\n")));
		vector< basic_string<TCHAR> >::iterator p = vecLines.begin();
		++p;
		while (vecLines.end() != p)
		{
			haveNode = true;
			UINT exp, level, status;
			_stscanf(p->c_str(), _T("%11d %10d %10d"), &exp, &level, &status);
			//CTextBlock* pTb = (CTextBlock*)(index);
			int width = g_config.wideWidth;
			if (vecLines.end()-1 != p)
				(*p)[(*p).length()-1] = _T('\0');
			CBooguNoteText* pTextBlock = CreateTextBlock(0,0, width, (TCHAR*)((UINT(p->c_str()))+(11*3+1)*sizeof(TCHAR)));
			//convert handle state
			#define IDB_BITMAP_EXPAND               202
			#define IDB_BITMAP_CLOSE                203
			#define IDB_BITMAP_LEAF                 206
			if (exp == IDB_BITMAP_CLOSE)
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_CLOSED;
			else if (exp == IDB_BITMAP_EXPAND)
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
			else if (exp == IDB_BITMAP_LEAF)
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			pTextBlock->m_pTextIcon->state = status;
			pTextBlock->m_nLevel = level;
			pTextBlock->m_bShown = true;
			pTextBlock->m_bBold = false;
			pTextBlock->DetectFileBlock();
			m_textBlockList.push_back(pTextBlock);
			//if (TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState)
			//	ShrinkTextBlock(pTextBlock);
			++p;
		}
		m_pTextBlockFocused = m_textBlockList[0];
		//m_pTextBlockFocused->RegisterDragDrop();
		m_nLastClicked = 0;
		m_bDirty = true;
		delete[] _tbuf;
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return true;
	}
	else
	{
		delete[] _tbuf;
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return false;
	}
}

bool CBooguNoteView::LoadData()
{
	CString fn(m_fileDirectory);
	int dot = fn.ReverseFind(_T('.'));
	CString ext = fn;
	ext.Delete(0, dot+1);
	ext.MakeLower();
	if (0==ext.Compare(_T("boo")))
	{
		return LoadBooData();
	}
	else if (0==ext.Compare(_T("mm")))
	{
		return LoadMMData();
	}
	return false;
}

bool CBooguNoteView::LoadData0006()
{
	CString fn(m_fileDirectory);
	int dot = fn.ReverseFind(_T('.'));
	CString ext = fn;
	ext.Delete(0, dot+1);
	ext.MakeLower();
	if (0==ext.Compare(_T("boo")))
	{
		return LoadBooData0006();
	}
	else if (0==ext.Compare(_T("mm")))
	{
		return LoadMMData();
	}
	return false;
}

bool CBooguNoteView::LoadData0005()
{
	CString fn(m_fileDirectory);
	int dot = fn.ReverseFind(_T('.'));
	CString ext = fn;
	ext.Delete(0, dot+1);
	ext.MakeLower();
	if (0==ext.Compare(_T("boo")))
	{
		return LoadBooData0005();
	}
	else if (0==ext.Compare(_T("mm")))
	{
		return LoadMMData();
	}
	return false;
}

bool CBooguNoteView::LoadMMData()
{
	//CBooguNoteView* pView = CreateNewView();
	//pView->m_pTextBlockFocused = NULL;
	XML x(m_fileDirectory);
	XMLElement* r = x.GetRootElement();
	r = r->GetChildren()[0];
	int nC = r->GetChildrenNum();
	if (nC >0 )
	{
		int nLevel = 0;
		for(int i = 0 ; i < nC ; i++)
		{
			XMLElement* ch = r->GetChildren()[i];
			//int nV = ch->GetVariableNum();
			int nMaxElName = ch->GetElementName(0);
			char* n = new char[nMaxElName + 1];
			ch->GetElementName(n);
			if (strcmp(n, "node") !=0)
			{
				delete [] n;
				continue;
			}
			delete [] n;
			int nValueLen = ch->FindVariableZ("TEXT")->GetValue(0, 1);
			char* pValueBuf = new char[nValueLen + 1];
			ch->FindVariableZ("TEXT")->GetValue(pValueBuf, 1);
			pValueBuf[nValueLen] = '\0';
			int nDecLen = XML::XMLDecode(pValueBuf, 0)*10;
			char* pDecValue = new char[nDecLen+10];
			memset(pDecValue, 0, sizeof(char)*(nDecLen+10));
			XML::XMLDecode(pValueBuf, pDecValue);
			int nLength = MultiByteToWideChar(CP_UTF8, 0, pDecValue, nDecLen+10, 0, 0);
			TCHAR* utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pDecValue, nDecLen+10,utf8buf, nLength+1);

			CBooguNoteText* pTextBlock = NULL;
			if (g_config.bWideTextBlock)
			{
				pTextBlock = CreateTextBlock(0,0, g_config.wideWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else
			{
				int TextBlockWidth = g_config.narrowWidth - (nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
				pTextBlock = CreateTextBlock(0,0, TextBlockWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_pTextIcon->state = GetXmlIcon(ch);//TEXT_ICON_PLAINTEXT;
			pTextBlock->m_nLevel = nLevel;
			pTextBlock->m_bShown = true;
			pTextBlock->m_bBold = false;
			m_textBlockList.push_back(pTextBlock);
			pTextBlock->DetectFileBlock();

			ProcessMMNodes(ch, nLevel);
			//fprintf(stdout,"\t Child %u: Variables: %u , Name: %s\r\n",i,nV,n);
			delete [] utf8buf;
			delete [] pDecValue;
			delete [] pValueBuf;
		}
	}
	else
	{
		CBooguNoteText* pTextBlock = NULL; 
		if(g_config.bWideTextBlock)
		{
			pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
		}
		else
		{
			pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		pTextBlock->m_bShown = true;
		pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
		pTextBlock->m_nLevel = 0;
		m_textBlockList.push_back(pTextBlock);
		//pTextBlock->m_bSelected = true;
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
		m_pTextBlockFocused = pTextBlock;
		//m_pTextBlockFocused->RegisterDragDrop();
		m_nLastClicked = 0;
		CalcLayout();
		CalcScrollBar();
		m_bDirty = true;
	}

	//ProcessMMNodes(r, -1);


	//delete pView->m_textBlockList[0];
	//m_textBlockList.erase(pView->m_textBlockList.begin());
	CalcLayout();
	return true;
}

void CBooguNoteView::ProcessMMNodes(XMLElement* r, int nLevel)
{
	nLevel++;
	int nC = r->GetChildrenNum();
	if (nC <=0 )
		return;
	for(int i = 0 ; i < nC ; i++)
	{
		XMLElement* ch = r->GetChildren()[i];
		//int nV = ch->GetVariableNum();
		int nMaxElName = ch->GetElementName(0);
		char* n = new char[nMaxElName + 1];
		ch->GetElementName(n);
		if (strcmp(n, "node") !=0)
		{
			delete [] n;
			continue;
		}
		delete [] n;
		int nValueLen = ch->FindVariableZ("TEXT")->GetValue(0, 1);
		char* pValueBuf = new char[nValueLen + 1];
		ch->FindVariableZ("TEXT")->GetValue(pValueBuf, 1);
		pValueBuf[nValueLen] = '\0';
		int nDecLen = XML::XMLDecode(pValueBuf, 0)*10;
		char* pDecValue = new char[nDecLen];
		memset(pDecValue, 0, sizeof(char)*nDecLen);
		XML::XMLDecode(pValueBuf, pDecValue);
		int nLength = MultiByteToWideChar(CP_UTF8, 0, pDecValue, nDecLen, 0, 0);
		TCHAR* utf8buf = new TCHAR[nLength+1];
		MultiByteToWideChar(CP_UTF8, 0, pDecValue, nDecLen,utf8buf, nLength+1);

		CBooguNoteText* pTextBlock = NULL;
		if (g_config.bWideTextBlock)
		{
			pTextBlock = CreateTextBlock(0,0, g_config.wideWidth, utf8buf);
			pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
		}
		else
		{
			int TextBlockWidth = g_config.narrowWidth - (nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
			pTextBlock = CreateTextBlock(0,0, TextBlockWidth, utf8buf);
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		
		pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
		pTextBlock->m_pTextIcon->state = GetXmlIcon(ch);//TEXT_ICON_PLAINTEXT;
		pTextBlock->m_nLevel = nLevel;
		pTextBlock->m_bShown = true;
		pTextBlock->m_bBold = false;
		pTextBlock->DetectFileBlock();
		m_textBlockList.push_back(pTextBlock);
		pTextBlock->DetectFileBlock();

		ProcessMMNodes(ch, nLevel);
		//fprintf(stdout,"\t Child %u: Variables: %u , Name: %s\r\n",i,nV,n);
		delete [] utf8buf;
		delete [] pDecValue;
		delete [] pValueBuf;
	}
}

void CBooguNoteView::ProcessBooNodes(XMLElement* r, int nDefaultLevel)
{
	int nC = r->GetChildrenNum();
	if (nC <=0 )
		return;
	for(int i = 0 ; i < nC ; i++)
	{
		XMLElement* ch = r->GetChildren()[i];
		int nMaxElName = ch->GetElementName(0);
		char* n = new char[nMaxElName + 1];
		ch->GetElementName(n);
		if (strcmp(n, "item") !=0)
		{
			delete [] n;
			continue;
		}
		delete [] n;

		XMLVariable* v = NULL;
		//content
		int nValueLen = ch->FindVariableZ("content")->GetValue(0);
		char* pValueBuf = new char[nValueLen + 1];
		memset(pValueBuf, '\0', nValueLen+1);
		ch->FindVariableZ("content")->GetValue(pValueBuf);
		pValueBuf[nValueLen] = '\0';
		int nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
		TCHAR* utf8buf = new TCHAR[nLength+1];
		MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
		
		delete [] pValueBuf;
		
		CBooguNoteText* pTextBlock = NULL;

		//计算层数
        XMLVariable* vLevel = NULL;
        int ilevel = 0;          
        vLevel  = ch->FindVariableZ("level");
        if (NULL == vLevel )
        {
            ilevel = nDefaultLevel;
        }
        else
        {
            ilevel = vLevel->GetValueInt();
        }
        int iCreateTextBlockWidth = g_config.narrowWidth - (ilevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
			
		
		//block
		v = ch->FindVariableZ("block");
		if (NULL == v)
		{
			pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			TCHAR* blockBuf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,blockBuf, nLength+1);
			
			if (_tcscmp(blockBuf, _T("wide"))==0)
			{
				pTextBlock = CreateTextBlock(0,0, g_config.wideWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else if (_tcscmp(blockBuf, _T("narrow"))==0)
			{
				pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			else if (_tcscmp(blockBuf, _T("shrink"))==0)
			{
				pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_SHRINK;
			}
			delete [] blockBuf;
			delete [] pValueBuf;
		}
		delete [] utf8buf;

		//branch
		v = ch->FindVariableZ("branch");
		if (NULL == v)
		{
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("none"))==0)
			{
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			}
			else if (_tcscmp(utf8buf, _T("open"))==0)
			{
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
			}
			else if (_tcscmp(utf8buf, _T("close"))==0)
			{
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_CLOSED;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//icon
		v = ch->FindVariableZ("icon");
		if (NULL == v)
		{
			pTextBlock->m_pTextIcon->state = TEXT_ICON_PLAINTEXT;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("none"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_PLAINTEXT;
			}
			else if (_tcscmp(utf8buf, _T("flag"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_TODO;
			}
			else if (_tcscmp(utf8buf, _T("tick"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_DONE;
			}
			else if (_tcscmp(utf8buf, _T("cross"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_CROSS;
			}
			else if (_tcscmp(utf8buf, _T("star"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_STAR;
			}
			else if (_tcscmp(utf8buf, _T("question"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_QUESTION;
			}
			else if (_tcscmp(utf8buf, _T("warning"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_WARNING;
			}
			else if (_tcscmp(utf8buf, _T("idea"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_IDEA;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}
		
		//level
		v = ch->FindVariableZ("level");
		if (NULL == v)
		{
			pTextBlock->m_nLevel = nDefaultLevel;
		}
		else
		{
			pTextBlock->m_nLevel = v->GetValueInt();
		}

		//shown
		v = ch->FindVariableZ("IsShown");
		if (NULL == v)
		{
			pTextBlock->m_bShown = true;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("true"))==0)
			{
				pTextBlock->m_bShown = true;
			}
			else if (_tcscmp(utf8buf, _T("false"))==0)
			{
				pTextBlock->m_bShown = false;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//bold
		v = ch->FindVariableZ("IsBold");
		if (NULL == v)
		{
			pTextBlock->m_bBold = false;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("true"))==0)
			{
				pTextBlock->m_bBold = true;
			}
			else if (_tcscmp(utf8buf, _T("false"))==0)
			{
				pTextBlock->m_bBold = false;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//show branch
		v = ch->FindVariableZ("ShowBranch");
		if (NULL == v)
		{
			pTextBlock->m_bShowBranch = false;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("true"))==0)
			{
				pTextBlock->m_bShowBranch = true;
			}
			else if (_tcscmp(utf8buf, _T("false"))==0)
			{
				pTextBlock->m_bShowBranch = false;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//Text Color
		v = ch->FindVariableZ("TextColor");
		if (NULL == v)
		{
			pTextBlock->m_TextClr = 0x00000000;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			StringToBinary(utf8buf, sizeof(COLORREF)*2+1, (BYTE*)&pTextBlock->m_TextClr);
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//Background Color
		v = ch->FindVariableZ("BkgrdColor");
		if (NULL == v)
		{
			pTextBlock->m_BkgrdClr = 0x00FFFFFF;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			StringToBinary(utf8buf, sizeof(COLORREF)*2+1, (BYTE*)&pTextBlock->m_BkgrdClr);
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		v = ch->FindVariableZ("ModifyTime");
		if (NULL == v)
		{
			pTextBlock->m_ModifyTime = 0;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			tm timeinfo;
			sscanf_s(pValueBuf, "%d-%d-%d %d:%d:%d", &(timeinfo.tm_year), &(timeinfo.tm_mon), &(timeinfo.tm_mday), &(timeinfo.tm_hour), &(timeinfo.tm_min), &(timeinfo.tm_sec));
			timeinfo.tm_year -= 1900;
			timeinfo.tm_mon  -= 1;
			pTextBlock->m_ModifyTime = mktime(&timeinfo);
		}

		CHARFORMAT2 cf;
		cf.cbSize = sizeof(CHARFORMAT2);
		pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
		cf.dwMask = CFM_COLOR|CFM_BACKCOLOR;
		cf.dwEffects &= ~CFE_AUTOBACKCOLOR;
		cf.dwEffects &= ~~CFE_AUTOCOLOR;
		cf.crTextColor = pTextBlock->m_TextClr;
		cf.crBackColor = pTextBlock->m_BkgrdClr;
		pTextBlock->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);

		m_textBlockList.push_back(pTextBlock);
		if (TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState)
			ShrinkTextBlock(pTextBlock);
		if (pTextBlock->m_bBold)
		{
			CHARFORMAT cf;
			cf.cbSize = sizeof(CHARFORMAT);
			pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
			cf.dwEffects |= CFE_BOLD;
			pTextBlock->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
		}
		pTextBlock->DetectFileBlock();
		////isfile
		//v = ch->FindVariableZ("IsFile");
		//if (NULL == v)
		//{
		//	pTextBlock->DetectFileBlock();
		//}
		//else
		//{
		//	nValueLen = v->GetValue(0);
		//	pValueBuf = new char[nValueLen + 1];
		//	memset(pValueBuf, '\0', nValueLen+1);
		//	v->GetValue(pValueBuf);
		//	pValueBuf[nValueLen] = '\0';
		//	nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
		//	utf8buf = new TCHAR[nLength+1];
		//	MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
		//	if (_tcscmp(utf8buf, _T("true"))==0)
		//	{
		//		pTextBlock->m_bFile = true;
		//	}
		//	else if (_tcscmp(utf8buf, _T("false"))==0)
		//	{
		//		pTextBlock->m_bFile = false;
		//		pTextBlock->DetectFileBlock();
		//	}
		//	delete [] utf8buf;
		//	delete [] pValueBuf;
		//}
		

		ProcessBooNodes(ch, nDefaultLevel+1);
	}
}

void CBooguNoteView::ProcessBooNodes0005(XMLElement* r, int nDefaultLevel)
{
	int nC = r->GetChildrenNum();
	if (nC <=0 )
		return;
	for(int i = 0 ; i < nC ; i++)
	{
		XMLElement* ch = r->GetChildren()[i];
		int nMaxElName = ch->GetElementName(0);
		char* n = new char[nMaxElName + 1];
		ch->GetElementName(n);
		if (strcmp(n, "item") !=0)
		{
			delete [] n;
			continue;
		}
		delete [] n;

		XMLVariable* v = NULL;
		//content
		int nValueLen = ch->FindVariableZ("content")->GetValue(0);
		char* pValueBuf = new char[nValueLen + 1];
		memset(pValueBuf, '\0', nValueLen+1);
		ch->FindVariableZ("content")->GetValue(pValueBuf);
		pValueBuf[nValueLen] = '\0';
		int nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
		TCHAR* utf8buf = new TCHAR[nLength+1];
		MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
		
		delete [] pValueBuf;
		
		CBooguNoteText* pTextBlock = NULL;

		//
		XMLVariable* vLevel = NULL;
        int ilevel = 0;          
        vLevel  = ch->FindVariableZ("level");
        if (NULL == vLevel )
        {
            ilevel = nDefaultLevel;
        }
        else
        {
            ilevel = v->GetValueInt();
        }
        int iCreateTextBlockWidth = g_config.narrowWidth - (ilevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
		
		//block
		v = ch->FindVariableZ("block");
		if (NULL == v)
		{
			pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			TCHAR* blockBuf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,blockBuf, nLength+1);
			
			if (_tcscmp(blockBuf, _T("wide"))==0)
			{
				pTextBlock =CreateTextBlock(0,0, g_config.wideWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else if (_tcscmp(blockBuf, _T("narrow"))==0)
			{
				pTextBlock =CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			else if (_tcscmp(blockBuf, _T("shrink"))==0)
			{
				pTextBlock =CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
				pTextBlock->m_nExpandState = TEXT_BLOCK_SHRINK;
			}
			delete [] blockBuf;
			delete [] pValueBuf;
		}
		delete [] utf8buf;

		//branch
		v = ch->FindVariableZ("branch");
		if (NULL == v)
		{
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("none"))==0)
			{
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			}
			else if (_tcscmp(utf8buf, _T("open"))==0)
			{
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
			}
			else if (_tcscmp(utf8buf, _T("close"))==0)
			{
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_CLOSED;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//icon
		v = ch->FindVariableZ("icon");
		if (NULL == v)
		{
			pTextBlock->m_pTextIcon->state = TEXT_ICON_PLAINTEXT;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("none"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_PLAINTEXT;
			}
			else if (_tcscmp(utf8buf, _T("flag"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_TODO;
			}
			else if (_tcscmp(utf8buf, _T("tick"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_DONE;
			}
			else if (_tcscmp(utf8buf, _T("cross"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_CROSS;
			}
			else if (_tcscmp(utf8buf, _T("star"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_STAR;
			}
			else if (_tcscmp(utf8buf, _T("question"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_QUESTION;
			}
			else if (_tcscmp(utf8buf, _T("warning"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_WARNING;
			}
			else if (_tcscmp(utf8buf, _T("idea"))==0)
			{
				pTextBlock->m_pTextIcon->state = TEXT_ICON_IDEA;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}
		
		//level
		v = ch->FindVariableZ("level");
		if (NULL == v)
		{
			pTextBlock->m_nLevel = nDefaultLevel;
		}
		else
		{
			pTextBlock->m_nLevel = v->GetValueInt();
		}

		//shown
		v = ch->FindVariableZ("IsShown");
		if (NULL == v)
		{
			pTextBlock->m_bShown = true;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("true"))==0)
			{
				pTextBlock->m_bShown = true;
			}
			else if (_tcscmp(utf8buf, _T("false"))==0)
			{
				pTextBlock->m_bShown = false;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//bold
		v = ch->FindVariableZ("IsBold");
		if (NULL == v)
		{
			pTextBlock->m_bBold = false;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("true"))==0)
			{
				pTextBlock->m_bBold = true;
			}
			else if (_tcscmp(utf8buf, _T("false"))==0)
			{
				pTextBlock->m_bBold = false;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		//show branch
		v = ch->FindVariableZ("ShowBranch");
		if (NULL == v)
		{
			pTextBlock->m_bShowBranch = false;
		}
		else
		{
			nValueLen = v->GetValue(0);
			pValueBuf = new char[nValueLen + 1];
			memset(pValueBuf, '\0', nValueLen+1);
			v->GetValue(pValueBuf);
			pValueBuf[nValueLen] = '\0';
			nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
			utf8buf = new TCHAR[nLength+1];
			MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
			if (_tcscmp(utf8buf, _T("true"))==0)
			{
				pTextBlock->m_bShowBranch = true;
			}
			else if (_tcscmp(utf8buf, _T("false"))==0)
			{
				pTextBlock->m_bShowBranch = false;
			}
			delete [] utf8buf;
			delete [] pValueBuf;
		}

		m_textBlockList.push_back(pTextBlock);
		if (TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState)
			ShrinkTextBlock(pTextBlock);
		if (pTextBlock->m_bBold)
		{
			CHARFORMAT cf;
			cf.cbSize = sizeof(CHARFORMAT);
			pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
			cf.dwEffects |= CFE_BOLD;
			pTextBlock->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
		}
		pTextBlock->DetectFileBlock();
		////isfile
		//v = ch->FindVariableZ("IsFile");
		//if (NULL == v)
		//{
		//	pTextBlock->DetectFileBlock();
		//}
		//else
		//{
		//	nValueLen = v->GetValue(0);
		//	pValueBuf = new char[nValueLen + 1];
		//	memset(pValueBuf, '\0', nValueLen+1);
		//	v->GetValue(pValueBuf);
		//	pValueBuf[nValueLen] = '\0';
		//	nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
		//	utf8buf = new TCHAR[nLength+1];
		//	MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
		//	if (_tcscmp(utf8buf, _T("true"))==0)
		//	{
		//		pTextBlock->m_bFile = true;
		//	}
		//	else if (_tcscmp(utf8buf, _T("false"))==0)
		//	{
		//		pTextBlock->m_bFile = false;
		//		pTextBlock->DetectFileBlock();
		//	}
		//	delete [] utf8buf;
		//	delete [] pValueBuf;
		//}
		

		ProcessBooNodes0005(ch, nDefaultLevel+1);
	}
}

bool CBooguNoteView::LoadBooData()
{
	HANDLE hFile = CreateFileW(m_fileDirectory, GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	BYTE * lpvFile = (BYTE *)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (0 == strncmp((char*)lpvFile, XML_HEADER, strlen(XML_HEADER)-3)) // -3 for xml library adding chars.
	{
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);

		XML x(m_fileDirectory);
		XMLElement* r = x.GetRootElement();
		//r = r->GetChildren()[0];
		int nC = r->GetChildrenNum();
		if (nC >0 )
		{
			if (BOO_FILE_VERSION != r->FindVariableZ("version")->GetValueInt())
			{
				return false;
			}

			if (NULL != r->FindVariableZ("DefaultSaveDir"))
			{			
				int nValueLen = r->FindVariableZ("DefaultSaveDir")->GetValue(0);
				char* pValueBuf = new char[nValueLen + 1];
				memset(pValueBuf, '\0', nValueLen+1);
				r->FindVariableZ("DefaultSaveDir")->GetValue(pValueBuf);
				pValueBuf[nValueLen] = '\0';
				int nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
				TCHAR* utf8buf = new TCHAR[nLength+1];
				MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);

				m_strDefaultSaveDir = utf8buf;

				delete [] pValueBuf;
				delete [] utf8buf;
			}

			if (NULL != r->FindVariableZ("DefaultSaveExtension"))
			{
				int nValueLen = r->FindVariableZ("DefaultSaveExtension")->GetValue(0);
				char* pValueBuf = new char[nValueLen + 1];
				memset(pValueBuf, '\0', nValueLen+1);
				r->FindVariableZ("DefaultSaveExtension")->GetValue(pValueBuf);
				pValueBuf[nValueLen] = '\0';
				int nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
				TCHAR* utf8buf = new TCHAR[nLength+1];
				MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);

				m_strDefaultImgExtension = utf8buf;

				delete [] pValueBuf;
				delete [] utf8buf;
			}

			ProcessBooNodes(r, 0);
		}
		else
		{
			CBooguNoteText* pTextBlock = NULL; 
			if(g_config.bWideTextBlock)
			{
				pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else
			{
				pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			pTextBlock->m_bShown = true;
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nLevel = 0;
			m_textBlockList.push_back(pTextBlock);
			//pTextBlock->m_bSelected = true;
			m_pTextBlockFocused = pTextBlock;
			m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);

			//m_pTextBlockFocused->RegisterDragDrop();
			m_nLastClicked = 0;
			CalcLayout();
			CalcScrollBar();
			m_bDirty = true;
		}
		CalcLayout();
		return true;
	}
	else
	{
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return false;
	}
}


bool CBooguNoteView::LoadBooData0006()
{
	HANDLE hFile = CreateFileW(m_fileDirectory, GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	BYTE * lpvFile = (BYTE *)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (0 == strncmp((char*)lpvFile, XML_HEADER, strlen(XML_HEADER)-3)) // -3 for xml library adding chars.
	{
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);

		XML x(m_fileDirectory);
		XMLElement* r = x.GetRootElement();
		//r = r->GetChildren()[0];
		int nC = r->GetChildrenNum();
		if (nC >0 )
		{
			if (6 != r->FindVariableZ("version")->GetValueInt())
			{
				return false;
			}
			ProcessBooNodes(r, 0);
			m_bDirty = true;
		}
		else
		{
			CBooguNoteText* pTextBlock = NULL; 
			if(g_config.bWideTextBlock)
			{
				pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else
			{
				pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			pTextBlock->m_bShown = true;
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nLevel = 0;
			m_textBlockList.push_back(pTextBlock);
			//pTextBlock->m_bSelected = true;
			m_pTextBlockFocused = pTextBlock;
			m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	
			//m_pTextBlockFocused->RegisterDragDrop();
			m_nLastClicked = 0;
			CalcLayout();
			CalcScrollBar();
			m_bDirty = true;
		}
		CalcLayout();
		return true;
	}
	else
	{
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return false;
	}

	
}

bool CBooguNoteView::LoadBooData0005()
{
	HANDLE hFile = CreateFileW(m_fileDirectory, GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	BYTE * lpvFile = (BYTE *)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (0 == strncmp((char*)lpvFile, XML_HEADER, strlen(XML_HEADER)-3)) // -3 for xml library adding chars.
	{
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);

		XML x(m_fileDirectory);
		XMLElement* r = x.GetRootElement();
		//r = r->GetChildren()[0];
		int nC = r->GetChildrenNum();
		if (nC >0 )
		{
			if (5 != r->FindVariableZ("version")->GetValueInt())
			{
				return false;
			}
			ProcessBooNodes0005(r, 0);
			m_bDirty = true;
		}
		else
		{
			CBooguNoteText* pTextBlock = NULL; 
			if(g_config.bWideTextBlock)
			{
				pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else
			{
				pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			pTextBlock->m_bShown = true;
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nLevel = 0;
			m_textBlockList.push_back(pTextBlock);
			//pTextBlock->m_bSelected = true;
			m_pTextBlockFocused = pTextBlock;
			m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	
			//m_pTextBlockFocused->RegisterDragDrop();
			m_nLastClicked = 0;
			CalcLayout();
			CalcScrollBar();
			m_bDirty = true;
		}
		CalcLayout();
		return true;
	}
	else
	{
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return false;
	}

	
}

bool CBooguNoteView::LoadOldData0004()
{
	SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HANDLE hFile = CreateFileW(m_fileDirectory, GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	BYTE * lpvFile = (BYTE *)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	DWORD dwVersion = (*((DWORD*)lpvFile));
	if (dwVersion == 0x0004)
	{
		int nCount = (*((int*)(lpvFile+sizeof(DWORD))));
		BYTE* index = lpvFile + (sizeof(DWORD)+sizeof(int));
		for (int i=0; i<nCount; ++i)
		{
			CTextBlock* pTb = (CTextBlock*)(index);
			int width = 0;
			if ( TEXT_BLOCK_WIDE == pTb->m_TextBlockState)
			{
				width = g_config.wideWidth;
			}
			else
			{
				width = g_config.narrowWidth;
			}
			CBooguNoteText* pTextBlock = CreateTextBlock(0,0, width, (TCHAR*)(index+sizeof(CTextBlock)));
			pTextBlock->m_pTextHandle->state = pTb->m_TextHandleState;
			pTextBlock->m_nExpandState = pTb->m_TextBlockState;
			pTextBlock->m_pTextIcon->state = pTb->m_TextIcon;
			pTextBlock->m_nLevel = pTb->m_nTextLevel;
			pTextBlock->m_bShown = pTb->m_bShown;
			pTextBlock->m_bBold = pTb->m_bBold;
			
			m_textBlockList.push_back(pTextBlock);
			if (TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState)
				ShrinkTextBlock(pTextBlock);
			if (pTextBlock->m_bBold)
			{
				CHARFORMAT cf;
				cf.cbSize = sizeof(CHARFORMAT);
				pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				cf.dwEffects |= CFE_BOLD;
				pTextBlock->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
			}
			pTextBlock->DetectFileBlock();
			index += (pTb->m_nTextLength*sizeof(TCHAR)+sizeof(CTextBlock));
		}
		m_pTextBlockFocused = m_textBlockList[0];
		//m_pTextBlockFocused->RegisterDragDrop();
		m_nLastClicked = 0;
		m_bDirty = true;
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return true;
	}
	else
	{
		UnmapViewOfFile(lpvFile);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return false;
	}
}

LRESULT CBooguNoteView::OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	ClientToDevice(pt);
	// See if this is over a control
	if (m_bRButtonDown)
	{
		SetCursor(g_util.hCsrSizeAll);
	}
	else
	{
		if (m_pTextBlockFocused)
		{
			if(!m_pTextBlockFocused->DoSetCursor(NULL, &pt))
			{
				SetCursor(g_util.hCsrArrow);
			}
		}
		else
		{
			SetCursor(g_util.hCsrArrow);
		}
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	//delete m_pToolTip;
	// Clear out our data
	//if (NULL!=m_pTextBlockFocused)
	//	m_pTextBlockFocused->RevokeDragDrop();
	m_pTextBlockFocused = NULL;
	int nTextBlockSize = m_textBlockList.size();
	for (int i=0; i<nTextBlockSize; ++i)
	{
		//m_textBlockList[i]->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)0, 0);
		delete m_textBlockList[i];
		//m_textBlockList[i]->Release();
		//m_textBlockList.erase(m_textBlockList.begin());
	}
	m_textBlockList.empty();
	int s = undoList.size();
	for (int i=0; i< s; ++i)
	{
		delete undoList[i];
	}
	undoIndexList.empty();
	SetWindowLong(0, 0);
	bHandled = FALSE;
	return 1;
}

LRESULT CBooguNoteView::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	//return DefWindowProc(uMsg, wParam, lParam);
	return 0;
}

LRESULT CBooguNoteView::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if ((WORD)wParam != VK_ESCAPE)
	{
		if (m_pTextBlockFocused!=NULL && !m_pTextBlockFocused->m_bFile && TEXT_BLOCK_SHRINK !=m_pTextBlockFocused->m_nExpandState)
		{
			time(&(m_pTextBlockFocused->m_ModifyTime));
			if (!m_bDirty)
			{
				m_bDirty = true;
				GetParent().SendMessage(WM_NCPAINT);
			}
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CBooguNoteView::OnImeChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (m_pTextBlockFocused!=NULL)
	{
		time(&(m_pTextBlockFocused->m_ModifyTime));
		if (!m_bDirty)
		{
			m_bDirty = true;
			GetParent().SendMessage(WM_NCPAINT);
		}
	}
	bHandled = TRUE;
	return 0;
}

void CBooguNoteView::CreateNextBlock(bool bNarrow)
{
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock) == m_pTextBlockFocused)
		{
			(*ppTextBlock)->m_bSelected = false;
			(*ppTextBlock)->m_bHovering = false;
			int nLevel = (*ppTextBlock)->m_nLevel;
			while(((ppTextBlock+1) != m_textBlockList.end())&&((*(ppTextBlock+1))->m_nLevel>nLevel))
				++ppTextBlock;
			CBooguNoteText* pTextBlock = NULL; 
			//if(g_config.bWideTextBlock)
			if(bNarrow)
			{
				pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-(nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder),_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			else
			{
				pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			pTextBlock->m_bShown = true;
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nLevel = nLevel;
			if (ppTextBlock == m_textBlockList.end())
				++ppTextBlock;//adjust to the last element.
			m_nLastClicked = ppTextBlock - m_textBlockList.begin() +1;
			m_textBlockList.insert(++ppTextBlock, pTextBlock);
			pTextBlock->m_bSelected = false;
			if (NULL!=m_pTextBlockFocused)
			{
				//m_pTextBlockFocused->RevokeDragDrop();
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
			}
			m_pTextBlockFocused = pTextBlock;
			//m_pTextBlockFocused->RegisterDragDrop();
			m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
			//CalcLayout();
			//CalcScrollBar();
			break;
		}
	}
	m_bDirty = true;
	//GetParent().SendMessageW(WM_NCPAINT);
}

CBooguNoteText* CBooguNoteView::CreateChildBlock(bool bNarrow)
{
	CBooguNoteText* pTextBlock = NULL;
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock) == m_pTextBlockFocused)
		{
			(*ppTextBlock)->m_bSelected = false;
			(*ppTextBlock)->m_bHovering = false;
			(*ppTextBlock)->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
			int nLevel = (*ppTextBlock)->m_nLevel;

			//if(g_config.bWideTextBlock)
			if(bNarrow)
			{
				pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-((nLevel+1)*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder),_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			else
			{
				pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			pTextBlock->m_bShown = true;
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nLevel = nLevel+1;
			pTextBlock->m_bSelected = false;
			
			if (NULL!=m_pTextBlockFocused)
			{
				//m_pTextBlockFocused->RevokeDragDrop();
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
			}
			m_pTextBlockFocused = pTextBlock;
			//m_pTextBlockFocused->RegisterDragDrop();
			m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
			if ((ppTextBlock+1)!=m_textBlockList.end())
			{
				m_nLastClicked = ppTextBlock - m_textBlockList.begin() +1;
				m_textBlockList.insert(++ppTextBlock, pTextBlock);
			}
			else
			{
				m_textBlockList.push_back(pTextBlock);
				m_nLastClicked = m_textBlockList.size()-1;
			}
			//CalcLayout();
			//CalcScrollBar();
			break;
		}
	}
	m_bDirty = true;
	return pTextBlock;
	//GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::CreatePrevBlock()
{
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock) == m_pTextBlockFocused)
		{
			(*ppTextBlock)->m_bSelected = false;
			(*ppTextBlock)->m_bHovering = false;
			int nLevel = (*ppTextBlock)->m_nLevel;
			//while(((ppTextBlock+1) != m_textBlockList.end())&&((*(ppTextBlock+1))->m_nLevel>nLevel))
			//	++ppTextBlock;
			CBooguNoteText* pTextBlock = NULL; 
			if(g_config.bWideTextBlock)
			{
				pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else
			{
				pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-(nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder),_T(""));
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
			}
			pTextBlock->m_bShown = true;
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nLevel = nLevel;
			m_nLastClicked = ppTextBlock - m_textBlockList.begin();
			m_textBlockList.insert(ppTextBlock, pTextBlock);
			pTextBlock->m_bSelected = false;
			if (NULL!=m_pTextBlockFocused)
			{
				//m_pTextBlockFocused->RevokeDragDrop();
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
			}
			m_pTextBlockFocused = pTextBlock;
			//m_pTextBlockFocused->RegisterDragDrop();
			m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
			CalcLayout();
			CalcScrollBar();
			break;
		}
	}
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::CreateParentBlock()
{
	if (NULL != m_pTextBlockFocused)
	{
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_pTextBlockFocused == m_textBlockList[i])
			{
				m_pTextBlockFocused->m_bSelected = false;
				int nLevel = m_textBlockList[i]->m_nLevel;
				if (nLevel>0)
					--nLevel;//Parent Level
				int nInsertPoint = m_textBlockList.size();
				for (++i; i<m_textBlockList.size(); ++i)
				{
					if (m_textBlockList[i]->m_nLevel <= nLevel)
					{
						nInsertPoint = i;
						break;
					}
				}
				CBooguNoteText* pTextBlock = NULL; 
				if(g_config.bWideTextBlock)
				{
					pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
					pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
				}
				else
				{
					pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-(nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder),_T(""));
					pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
				}
				pTextBlock->m_bShown = true;
				pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
				pTextBlock->m_nLevel = nLevel;
				m_nLastClicked = nInsertPoint;
				m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint, pTextBlock);
				pTextBlock->m_bSelected = false;
				
				if (NULL!=m_pTextBlockFocused)
				{
					//m_pTextBlockFocused->RevokeDragDrop();
					m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
				}
				m_pTextBlockFocused = pTextBlock;
				//m_pTextBlockFocused->RegisterDragDrop();
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
				CalcLayout();
				CalcScrollBar();
				break;
			}
		}
	}
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

LRESULT CBooguNoteView::OnLocateFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if (NULL!=m_pTextBlockFocused && m_pTextBlockFocused->m_bFile)
	{
		TCHAR szTempCache[MAX_PATH*3];
		if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
		{
			_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
		}
		else if (L'.' == m_pTextBlockFocused->m_szCache[0])
		{
			if (!m_fileDirectory.IsEmpty())
			{
				swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
				PathRemoveFileSpec(szTempCache);
				PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
			}
		}
		else
		{
			_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
		}
		CString str = _T(" /e, /select,\"");
		str += szTempCache;
		str += _T("\"");
		//_tsystem(str.GetBuffer(_MAX_PATH + 100));
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), str, NULL, SW_SHOWNORMAL);
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMoveFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if (NULL!=m_pTextBlockFocused && m_pTextBlockFocused->m_bFile)
	{
		CString str = _T(" /select,\"");
		str += m_pTextBlockFocused->m_szCache;
		str += _T("\"");
		//_tsystem(str.GetBuffer(_MAX_PATH + 100));
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), str, NULL, SW_SHOWNORMAL);
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if (NULL == g_pSearchDlg && _T("") != m_fileDirectory)
	{
		//m_bEnableNcActive = TRUE;
		g_pSearchDlg = new CSearchDlg();
		/*WritePrivateProfileString(_T("Search"),_T("SearchDir"),m_fileDirectory.GetBuffer(MAX_PATH),g_config.strConfigFileName);
		g_SearchDir = m_fileDirectory;*/
		g_pSearchDlg->m_searchDir = m_fileDirectory;
		g_pSearchDlg->m_searchWords = g_SearchWords;
		g_pSearchDlg->Create(this->m_hWnd, IDD_DIALOG_SEARCH);
		g_pSearchDlg->ShowWindow(SW_SHOW);
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnEditFileBlock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if (NULL!=m_pTextBlockFocused && m_pTextBlockFocused->m_bFile)
	{
		m_pTextBlockFocused->m_bFile = true;
		m_pTextBlockFocused->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)m_pTextBlockFocused->m_szCache, 0);
		CHARFORMAT cf;
		cf.cbSize = sizeof(CHARFORMAT);
		m_pTextBlockFocused->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
		cf.dwEffects &= ~CFE_UNDERLINE;
		cf.crTextColor = RGB(0,0,0);
		m_pTextBlockFocused->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
		m_pTextBlockFocused->SetReadOnly(FALSE);
		m_pTextBlockFocused->m_bFile = false;
		if (NULL != m_pTextBlockFocused->m_pBitmap)
		{
			delete m_pTextBlockFocused->m_pBitmap;
			m_pTextBlockFocused->m_pBitmap = NULL;
			m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
			//m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFONT, (WPARAM)g_config.hFontNormal, 0);
			ShowCaret();
		}
		if (NULL != m_pTextBlockFocused->m_szCache)
		{
			delete []m_pTextBlockFocused->m_szCache;
			m_pTextBlockFocused->m_szCache = NULL;
		}
		CalcLayout();
		CalcScrollBar();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnSplitTextBlock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	SpliteTextBlock();
	bHandled = TRUE;
	return 0;
}

void CBooguNoteView::SpliteTextBlock()
{
	if (NULL!=m_pTextBlockFocused && TEXT_BLOCK_SHRINK != m_pTextBlockFocused->m_nExpandState)
	{
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_pTextBlockFocused == m_textBlockList[i])
			{
				CHARRANGE cr = { 0, 0 };
				(m_pTextBlockFocused->GetTextServices())->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr,0);
				if (cr.cpMin == cr.cpMax)
				{
					int nSplitPoint = cr.cpMin;
					BSTR  bstrCache;
					m_textBlockList[i]->GetTextServices()->TxGetText(&bstrCache);
					int length = SysStringLen(bstrCache);

					TCHAR* BlockText = new TCHAR[length+1];
					_stprintf(BlockText, _T("%s"), (LPCTSTR)bstrCache);
					//TCHAR a = pTextBlock->m_szCache[length-1];
					//remove the last return code.
					if ((_T('\r') == BlockText[length-1]) || (_T('\n') == BlockText[length-1]))
						BlockText[length-1] = _T('\0');
					for(int i=nSplitPoint-1; i>=0; --i)
					{
						if ((_T('\r') == BlockText[i]) || (_T('\n') == BlockText[i]) || (11 == BlockText[i]))
						{
							BlockText[i] = _T('\0');
						}
						else
						{
							break;
						}
					}
					for(; nSplitPoint<length-1; nSplitPoint++)
					{
						if ((_T('\r') == BlockText[nSplitPoint]) || (_T('\n') == BlockText[nSplitPoint]) || 11 == BlockText[nSplitPoint])
						{
							BlockText[nSplitPoint] = _T('\0');
						}
						else
						{
							break;
						}
					}
					m_textBlockList[i]->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)&BlockText[nSplitPoint], 0);
					time(&(m_textBlockList[i]->m_ModifyTime));
					m_textBlockList[i]->DetectFileBlock();
					//Create Previous Text Block
					int nInsertPoint = i;
					CBooguNoteText* pTextBlock = NULL; 
					if(TEXT_BLOCK_WIDE == m_textBlockList[i]->m_nExpandState)
					{
						pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
						pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
					}
					else
					{
						pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-(m_textBlockList[i]->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder),_T(""));
						pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
					}
					pTextBlock->m_bShown = true;
					pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
					pTextBlock->m_pTextIcon->state =  m_textBlockList[i]->m_pTextIcon->state;
					pTextBlock->m_bBold = m_textBlockList[i]->m_bBold;
					if (pTextBlock->m_bBold)
					{
						CHARFORMAT cf;
						cf.cbSize = sizeof(CHARFORMAT);
						pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
						cf.dwEffects |= CFE_BOLD;
						pTextBlock->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
					}
					pTextBlock->m_nLevel = m_textBlockList[i]->m_nLevel;
					pTextBlock->m_bShowBranch = m_textBlockList[i]->m_bShowBranch;


					//set color
					pTextBlock->m_TextClr = m_textBlockList[i]->m_TextClr;
					pTextBlock->m_BkgrdClr = m_textBlockList[i]->m_BkgrdClr;
					CHARFORMAT2 cf;
					cf.cbSize = sizeof(CHARFORMAT2);
					pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwMask = CFM_COLOR|CFM_BACKCOLOR;
					cf.dwEffects &= ~CFE_AUTOBACKCOLOR;
					cf.dwEffects &= ~~CFE_AUTOCOLOR;
					cf.crTextColor = pTextBlock->m_TextClr;
					cf.crBackColor = pTextBlock->m_BkgrdClr;
					pTextBlock->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);


					m_nLastClicked = nInsertPoint;
					m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint, pTextBlock);

					HideCaret();
					BlockText[nSplitPoint] = _T('\0');
					pTextBlock->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)BlockText, 0);
					time(&(pTextBlock->m_ModifyTime));
					pTextBlock->DetectFileBlock();
					delete []BlockText;
					SysFreeString(bstrCache);
				}
				break;
			}
		}
	}
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

LRESULT CBooguNoteView::OnOpenCloseSubBlock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i] == m_pTextBlockFocused)
		{
			if (TEXT_HANDLE_EXPAND == m_textBlockList[i]->m_pTextHandle->state || TEXT_BLOCK_SHRINK !=(m_textBlockList[i]->m_nExpandState) )
			{
				CloseBlocks(i);
				StateShrink();
				
			}
			else if (TEXT_HANDLE_CLOSED == m_textBlockList[i]->m_pTextHandle->state || TEXT_BLOCK_NARROW  !=(m_textBlockList[i]->m_nExpandState) )
			{
				ExpandBlocks(i);
				StateNarrow();
			}

			CalcLayout();
			//bHitted = true;
			//bHandled = TRUE;
			m_bDirty = true;
			//GetParent().SendMessageW(WM_NCPAINT);
			break;
		}
	}
	return 0;
}


LRESULT CBooguNoteView::OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	DeleteSelectedBlock();
	bHandled = TRUE;
	return 0;
}


void CBooguNoteView::SetTextBold()
{
	CHARRANGE cr = { 0, 0 };
	if (NULL!=m_pTextBlockFocused)
		(m_pTextBlockFocused->GetTextServices())->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr,0);

	if (cr.cpMin == cr.cpMax)
	{
		bool bHaveSelected = false;
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				bHaveSelected = true;
				break;
			}
		}

		if (!bHaveSelected)
		{
			for (int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i] == m_pTextBlockFocused)
				{
					m_textBlockList[i]->m_bSelected = true;
					if ((i+1)<m_textBlockList.size() && m_textBlockList[i+1]->m_bShown == false)
					{
						int nCurrLevel = m_pTextBlockFocused->m_nLevel;
						i++;
						for (; i<m_textBlockList.size()&&m_textBlockList[i]->m_nLevel > nCurrLevel; ++i)
						{
							m_textBlockList[i]->m_bSelected = true;
						}
					}
					break;
				}
			}
		}
	}
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i]->m_bSelected&&m_textBlockList[i]->m_bShown)
		{
			if (!m_textBlockList[i]->m_bBold)
			{
				m_textBlockList[i]->m_bBold = true;
				CHARFORMAT cf;
				cf.cbSize = sizeof(CHARFORMAT);
				m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				cf.dwEffects |= CFE_BOLD;
				m_textBlockList[i]->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
			}
			else
			{
				m_textBlockList[i]->m_bBold = false;
				CHARFORMAT cf;
				cf.cbSize = sizeof(CHARFORMAT);
				m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				cf.dwEffects &= ~CFE_BOLD;
				m_textBlockList[i]->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
			}
		}
	}
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::DeleteSelectedBlock()
{
	bool bHaveSelected = false;
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i]->m_bSelected)
		{
			bHaveSelected = true;
			break;
		}
	}
	if (!bHaveSelected && NULL == m_pTextBlockFocused)
	{
		return ;
	}

	if (!bHaveSelected)
	{
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i] == m_pTextBlockFocused)
			{
				m_textBlockList[i]->m_bSelected = true;
				if ((i+1)<m_textBlockList.size() && m_textBlockList[i+1]->m_bShown == false)
				{
					int nCurrLevel = m_pTextBlockFocused->m_nLevel;
					i++;
					for (; i<m_textBlockList.size()&&m_textBlockList[i]->m_nLevel > nCurrLevel; ++i)
					{
						m_textBlockList[i]->m_bSelected = true;
					}
				}
				break;
			}
		}
	}

	//record deleting blocks into log.
	CString fileDirectory = g_config.szLogFolder;
	if (fileDirectory != _T(""))
	{
		fileDirectory += _T("\\BooguNote.log");
		//detect if over sized.
		HANDLE hFile = CreateFileW(fileDirectory, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (GetFileSize(hFile, NULL)>4*1024*1024)
		{
			for(int j=0; ;j++)
			{
				CString tempDirectory;
				tempDirectory.Format(_T("%s\\BooguNote%d.log"), g_config.szLogFolder, j);
				if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(tempDirectory))
				{
					CloseHandle(hFile);
					hFile = NULL;
					MoveFile(fileDirectory, tempDirectory);
					break;
				}
			}
			
		}
		else
		{
			CloseHandle(hFile);
			hFile = NULL;
		}
		

		hFile = CreateFileW(fileDirectory, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD le = GetLastError();
		
		SYSTEMTIME systime;
		GetLocalTime(&systime);

		TCHAR szPrev[1024];
		_stprintf(szPrev, _T("------------------------Delete----------------------\r\n%s\r\n%04u-%02u-%02u %02u:%02u:%02u.%04u\r\n\r\n"),
			m_fileDirectory.GetBuffer(MAX_PATH),
			systime.wYear,
			systime.wMonth,
			systime.wDay,
			systime.wHour,
			systime.wMinute,
			systime.wSecond,
			systime.wMilliseconds);
		m_fileDirectory.ReleaseBuffer();
		DWORD dwBytesWritten = WideCharToMultiByte(CP_UTF8, 0, szPrev, -1, 0, 0, NULL, NULL);
		char* acpbuf = new char[dwBytesWritten];
		int lBytesWritten = WideCharToMultiByte(CP_UTF8, 0, szPrev, -1, acpbuf, dwBytesWritten, NULL, NULL);
		SetFilePointer(hFile, 0, NULL, FILE_END);
		DWORD dwFileBytesWritten;
		WriteFile(hFile, acpbuf, lBytesWritten, &dwFileBytesWritten, NULL);
		delete acpbuf;
		
		for(int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				BSTR  bstrCache;
				m_textBlockList[i]->GetTextServices()->TxGetText(&bstrCache);
				int nBlockTextLength = SysStringLen(bstrCache);
				TCHAR* Buf = new TCHAR[nBlockTextLength+10];
				_stprintf(Buf, _T("%s\n\r\n\r\n"), (LPCTSTR)bstrCache);
				SysFreeString(bstrCache);
				DWORD dwBytesWritten = WideCharToMultiByte(CP_UTF8, 0, Buf, -1, 0, 0, NULL, NULL);
				char* acpbuf = new char[dwBytesWritten];
				int lBytesWritten = WideCharToMultiByte(CP_UTF8, 0, Buf, -1, acpbuf, dwBytesWritten, NULL, NULL);
				SetFilePointer(hFile, 0, NULL, FILE_END);
				DWORD dwFileBytesWritten;
				WriteFile(hFile, acpbuf, lBytesWritten, &dwFileBytesWritten, NULL);
				delete acpbuf;
				delete Buf;
			}
		}
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}
	int s = undoList.size();
	for (int i=0; i< s; ++i)
	{
		delete undoList[i];
	}
	undoList.clear();
	undoIndexList.clear();
	for(int i=0; i < m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i]->m_bSelected)
		{
			//store deleting item.
			undoList.push_back(m_textBlockList[i]);
			undoIndexList.push_back(i);
			//m_textBlockList.erase(m_textBlockList.begin()+i);
			//if (NULL!=m_pTextBlockFocused)
				//m_pTextBlockFocused->RevokeDragDrop();
			m_pTextBlockFocused = NULL;
			HideCaret();
			m_bDirty = true;
		}
	}
	for (int i=undoIndexList.size()-1; i>=0; --i)
	{
		m_textBlockList.erase(m_textBlockList.begin()+undoIndexList[i]);
	}
	if (m_textBlockList.empty())
	{
		CBooguNoteText* pTextBlock = NULL; 
		if(g_config.bWideTextBlock)
		{
			pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
		}
		else
		{
			pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		pTextBlock->m_bShown = true;
		m_textBlockList.push_back(pTextBlock);
		//if (NULL!=m_pTextBlockFocused)
		//	m_pTextBlockFocused->RevokeDragDrop();
		m_pTextBlockFocused = pTextBlock;
		//m_pTextBlockFocused->RegisterDragDrop();
		m_nLastClicked = 0;
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
		HideCaret();
	}
	else
	{
		//set new focused block
		int nextFocusIndex = undoIndexList[0];
		if (nextFocusIndex<0)
			nextFocusIndex = 0;
		else if (nextFocusIndex>m_textBlockList.size()-1)
			nextFocusIndex = m_textBlockList.size()-1;
		m_nLastClicked = nextFocusIndex;
		//if (NULL!=m_pTextBlockFocused)
		//	m_pTextBlockFocused->RevokeDragDrop();
		m_pTextBlockFocused = m_textBlockList[nextFocusIndex];
		//m_pTextBlockFocused->RegisterDragDrop();
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
		HideCaret();
	}

	m_bCancelAutoAdjust = true;
	CalcLayout();
	CalcScrollBar();
	Invalidate();
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::MoveSelectedItemsLeft()
{
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		//第一个元素不操作
		if (m_textBlockList.begin() == ppTextBlock)
		{
			continue;
		}
		if (((*ppTextBlock)->m_bSelected) || ((*ppTextBlock) == m_pTextBlockFocused))
		{
			if (0!=(*ppTextBlock)->m_nLevel)
			{
				int CurrLevel = (*ppTextBlock)->m_nLevel;
				int nHandleState = (*ppTextBlock)->m_pTextHandle->state;
				(*ppTextBlock)->m_nLevel -= 1;
				if (TEXT_HANDLE_CLOSED == nHandleState)
				{
					++ppTextBlock;
					for (; ((ppTextBlock != m_textBlockList.end())&&((*ppTextBlock)->m_nLevel>CurrLevel)); ppTextBlock++ )
					{
						(*ppTextBlock)->m_nLevel -= 1;
					}
					--ppTextBlock;
				}
				m_bDirty = true;
				GetParent().SendMessageW(WM_NCPAINT);
			}
		}
	}
	HideCaret();
	CalcLayout();
	CalcScrollBar();
	if (NULL!=m_pTextBlockFocused)
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	Invalidate();
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::MoveSelectedItemsRight()
{
	for(int i=0; i<m_textBlockList.size(); ++i)
	{
		if (0 == i)
		{
			continue;
		}
		if ((m_textBlockList[i]->m_bSelected) || (m_textBlockList[i] == m_pTextBlockFocused))
		{
			if (m_textBlockList[i]->m_nLevel <= m_textBlockList[i-1]->m_nLevel)
			{
				int CurrLevel = m_textBlockList[i]->m_nLevel;
				int nHandleState = m_textBlockList[i]->m_pTextHandle->state;
				m_textBlockList[i]->m_nLevel += 1;
				if (TEXT_HANDLE_CLOSED == nHandleState)
				{
					++i;
					for (; ((i<m_textBlockList.size())&&(m_textBlockList[i]->m_nLevel>CurrLevel)); ++i)
					{
						m_textBlockList[i]->m_nLevel += 1;
					}
					--i;
				}
			}
			else
			{
				m_textBlockList[i]->m_bSelected = false;
			}
			
		}
	}
	
	HideCaret();
	CalcLayout();
	CalcScrollBar();
	if (NULL!=m_pTextBlockFocused)
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	Invalidate();
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::MoveSelectedItemsUp()
{
	bool bMovable = true;
	int nBottomSelected = -1;
	if (NULL == m_pTextBlockFocused)
	{
		if (!(*m_textBlockList.begin())->m_bSelected)
		{
			
			for(int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i]->m_bSelected)
				{
					if (nBottomSelected!=-1)
					{
						//exist unselected blocks in selected blockes.
						if (i-nBottomSelected>1)
						{
							bMovable = false;
							break;
						}
					}
					nBottomSelected = i;
				}
			}
			if (-1 == nBottomSelected)
			{
				bMovable = false;
			}
		}
	}
	else
	{
		if ((*m_textBlockList.begin()) == m_pTextBlockFocused)
		{
			bMovable = false;
		}
		else
		{
			for (int i=0; i<m_textBlockList.size(); ++i)
				if (m_pTextBlockFocused == m_textBlockList[i])
				{
					if (0 == i)
					{
						bMovable = false;
					}
					else
					{
						if (TEXT_HANDLE_CLOSED == m_textBlockList[i]->m_pTextHandle->state)
						{
							m_textBlockList[i]->m_bSelected = true;
							int nLevel = m_textBlockList[i]->m_nLevel;
							++i;
							for (; i<m_textBlockList.size(); ++i)
							{
								if (m_textBlockList[i]->m_nLevel>nLevel)
								{
									m_textBlockList[i]->m_bSelected = true;
									nBottomSelected = i;
								}
								else
								{
									break;
								}
							}
							//
						}
						else
						{
							m_textBlockList[i]->m_bSelected = true;
							nBottomSelected = i;
						}
					}
					break;
				}
		}
	}
	if (bMovable)
	{
		int nTopSelected = -1;
		for(int i=0; i < m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				nTopSelected = i;
				break;
			}
		}
		int nInsertPoint = nTopSelected-1;
		if (nInsertPoint>=0)
		{
			//int nInsertPoint = nBottomSelected+2;
			if (nInsertPoint>0&&!m_textBlockList[nInsertPoint]->m_bShown)
			{
				for (;nInsertPoint>=0 && !m_textBlockList[nInsertPoint]->m_bShown; --nInsertPoint);
			}
			int nLevelDelta = m_textBlockList[nInsertPoint]->m_nLevel - m_textBlockList[nTopSelected]->m_nLevel;
			//m_textBlockList[nTopSelected]->m_nLevel = m_textBlockList[nInsertPoint]->m_nLevel;
			for (int i=0; i<nBottomSelected-nTopSelected+1; ++i)
			{
				m_textBlockList[nTopSelected+i]->m_nLevel += nLevelDelta;
				m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint+i, m_textBlockList[nTopSelected+i]);
				m_textBlockList.erase(m_textBlockList.begin()+nTopSelected+i+1);

				//m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint, m_textBlockList[nTopSelected]);
				//m_textBlockList.erase(m_textBlockList.begin()+nTopSelected);
			}
			if (NULL != m_pTextBlockFocused)
			{
				for(int i=0; i < m_textBlockList.size(); ++i)
				{
					m_textBlockList[i]->m_bSelected = false;
				}
			}
		}
	}
	else
	{
		if (NULL != m_pTextBlockFocused)
		{
			for(int i=0; i < m_textBlockList.size(); ++i)
			{
				m_textBlockList[i]->m_bSelected = false;
			}
		}
		return;
	}
	HideCaret();
	CalcLayout();
	CalcScrollBar();
	if (NULL!=m_pTextBlockFocused)
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	Invalidate();
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::MoveSelectedItemsTop()
{
	bool bMovable = true;
	int nBottomSelected = -1;
	if (NULL == m_pTextBlockFocused)
	{
		if (!(*m_textBlockList.begin())->m_bSelected)
		{
			
			for(int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i]->m_bSelected)
				{
					if (nBottomSelected!=-1)
					{
						//exist unselected blocks in selected blockes.
						if (i-nBottomSelected>1)
						{
							bMovable = false;
							break;
						}
					}
					nBottomSelected = i;
				}
			}
			if (-1 == nBottomSelected)
			{
				bMovable = false;
			}
		}
	}
	else
	{
		if ((*m_textBlockList.begin()) == m_pTextBlockFocused)
		{
			bMovable = false;
		}
		else
		{
			for (int i=0; i<m_textBlockList.size(); ++i)
				if (m_pTextBlockFocused == m_textBlockList[i])
				{
					if (0 == i)
					{
						bMovable = false;
					}
					else
					{
						if (TEXT_HANDLE_CLOSED == m_textBlockList[i]->m_pTextHandle->state)
						{
							m_textBlockList[i]->m_bSelected = true;
							int nLevel = m_textBlockList[i]->m_nLevel;
							++i;
							for (; i<m_textBlockList.size(); ++i)
							{
								if (m_textBlockList[i]->m_nLevel>nLevel)
								{
									m_textBlockList[i]->m_bSelected = true;
									nBottomSelected = i;
								}
								else
								{
									break;
								}
							}
							//
						}
						else
						{
							m_textBlockList[i]->m_bSelected = true;
							nBottomSelected = i;
						}
					}
					break;
				}
		}
	}
	if (bMovable)
	{
		int nTopSelected = -1;
		for(int i=0; i < m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				nTopSelected = i;
				break;
			}
		}
		int nInsertPoint = 0; //插入到第一个的位置上
		if (nInsertPoint>=0)
		{
			//int nInsertPoint = nBottomSelected+2;
			if (nInsertPoint>0&&!m_textBlockList[nInsertPoint]->m_bShown)
			{
				for (;nInsertPoint>=0 && !m_textBlockList[nInsertPoint]->m_bShown; --nInsertPoint);
			}
			int nLevelDelta = m_textBlockList[nInsertPoint]->m_nLevel - m_textBlockList[nTopSelected]->m_nLevel;
			//m_textBlockList[nTopSelected]->m_nLevel = m_textBlockList[nInsertPoint]->m_nLevel;
			for (int i=0; i<nBottomSelected-nTopSelected+1; ++i)
			{
				m_textBlockList[nTopSelected+i]->m_nLevel += nLevelDelta;
				m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint+i, m_textBlockList[nTopSelected+i]);
				m_textBlockList.erase(m_textBlockList.begin()+nTopSelected+i+1);

				//m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint, m_textBlockList[nTopSelected]);
				//m_textBlockList.erase(m_textBlockList.begin()+nTopSelected);
			}
			if (NULL != m_pTextBlockFocused)
			{
				for(int i=0; i < m_textBlockList.size(); ++i)
				{
					m_textBlockList[i]->m_bSelected = false;
				}
			}
		}
	}
	else
	{
		if (NULL != m_pTextBlockFocused)
		{
			for(int i=0; i < m_textBlockList.size(); ++i)
			{
				m_textBlockList[i]->m_bSelected = false;
			}
		}
		return;
	}
	HideCaret();
	CalcLayout();
	CalcScrollBar();
	if (NULL!=m_pTextBlockFocused)
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	Invalidate();
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::MoveSelectedItemsDown()
{
	bool bMovable = true;
	int nBottomSelected = -1;
	if (NULL == m_pTextBlockFocused)
	{
		if (!(*(m_textBlockList.end()-1))->m_bSelected)
		{
			
			for(int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i]->m_bSelected)
				{
					if (nBottomSelected!=-1)
					{
						//exist unselected blocks in selected blockes.
						if (i-nBottomSelected>1)
						{
							bMovable = false;
							break;
						}
						//if (!m_textBlockList[i]->m_bSelected && !m_textBlockList[i]->m_bShown)
						//{
						//	bMovable = false;
						//	break;
						//}
					}
					nBottomSelected = i;
				}
			}
			if (-1 == nBottomSelected)
			{
				bMovable = false;
			}
		}
	}
	else
	{
		if ((*(m_textBlockList.end()-1)) == m_pTextBlockFocused)
		{
			bMovable = false;
		}
		else
		{
			for (int i=0; i<m_textBlockList.size(); ++i)
				if (m_pTextBlockFocused == m_textBlockList[i])
				{
					if (TEXT_HANDLE_CLOSED == m_textBlockList[i]->m_pTextHandle->state)
					{
						m_textBlockList[i]->m_bSelected = true;
						int nLevel = m_textBlockList[i]->m_nLevel;
						++i;
						for (; i<m_textBlockList.size(); ++i)
						{
							if (m_textBlockList[i]->m_nLevel>nLevel)
							{
								m_textBlockList[i]->m_bSelected = true;
								nBottomSelected = i;
							}
							else
							{
								break;
							}
						}
						if (i>=m_textBlockList.size())
						{
							bMovable = false;
						}
					}
					else
					{
						m_textBlockList[i]->m_bSelected = true;
						nBottomSelected = i;
					}

					/*m_textBlockList[i]->m_bSelected = true;
					nBottomSelected = i;
					++i;
					for (;i<m_textBlockList.size()&&!m_textBlockList[i]->m_bShown; ++i)
					{
						m_textBlockList[i]->m_bSelected = true;
						nBottomSelected = i;
					}
					if (i == m_textBlockList.size())
						bMovable = false;*/
					break;
				}
		}
	}
	if (bMovable)
	{
		int nTopSelected = -1;
		for(int i=0; i < m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				nTopSelected = i;
				break;
			}
		}
		int nInsertPoint = nBottomSelected+2;
		if (nInsertPoint<=m_textBlockList.size()-1&&!m_textBlockList[nInsertPoint]->m_bShown)
		{
			for (;nInsertPoint< m_textBlockList.size() && !m_textBlockList[nInsertPoint]->m_bShown; ++nInsertPoint);
		}
		int nLevelDelta = 0;
		if (nInsertPoint>=m_textBlockList.size())
		{
			nLevelDelta = - m_textBlockList[nTopSelected]->m_nLevel;
			//m_textBlockList[nTopSelected]->m_nLevel = 0;
		}
		else
		{
			nLevelDelta = m_textBlockList[nInsertPoint]->m_nLevel - m_textBlockList[nTopSelected]->m_nLevel;
			//m_textBlockList[nTopSelected]->m_nLevel = m_textBlockList[nInsertPoint]->m_nLevel;
			
		}
		for (int i=0; i<nBottomSelected-nTopSelected+1; ++i)
		{
			m_textBlockList[nTopSelected]->m_nLevel += nLevelDelta;
			m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint, m_textBlockList[nTopSelected]);
			m_textBlockList.erase(m_textBlockList.begin()+nTopSelected);
		}
		if (NULL != m_pTextBlockFocused)
		{
			for(int i=0; i < m_textBlockList.size(); ++i)
			{
				m_textBlockList[i]->m_bSelected = false;
			}
		}
	}
	else
	{
		if (NULL != m_pTextBlockFocused)
		{
			for(int i=0; i < m_textBlockList.size(); ++i)
			{
				m_textBlockList[i]->m_bSelected = false;
			}
		}
		return;
	}
	HideCaret();
	CalcLayout();
	CalcScrollBar();
	if (NULL!=m_pTextBlockFocused)
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	Invalidate();
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

void CBooguNoteView::MoveSelectedItemsBottom()
{
	bool bMovable = true;
	int nBottomSelected = -1;
	if (NULL == m_pTextBlockFocused)
	{
		if (!(*(m_textBlockList.end()-1))->m_bSelected)
		{
			
			for(int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i]->m_bSelected)
				{
					if (nBottomSelected!=-1)
					{
						//exist unselected blocks in selected blockes.
						if (i-nBottomSelected>1)
						{
							bMovable = false;
							break;
						}
						//if (!m_textBlockList[i]->m_bSelected && !m_textBlockList[i]->m_bShown)
						//{
						//	bMovable = false;
						//	break;
						//}
					}
					nBottomSelected = i;
				}
			}
			if (-1 == nBottomSelected)
			{
				bMovable = false;
			}
		}
	}
	else
	{
		if ((*(m_textBlockList.end()-1)) == m_pTextBlockFocused)
		{
			bMovable = false;
		}
		else
		{
			for (int i=0; i<m_textBlockList.size(); ++i)
				if (m_pTextBlockFocused == m_textBlockList[i])
				{
					if (TEXT_HANDLE_CLOSED == m_textBlockList[i]->m_pTextHandle->state)
					{
						m_textBlockList[i]->m_bSelected = true;
						int nLevel = m_textBlockList[i]->m_nLevel;
						++i;
						for (; i<m_textBlockList.size(); ++i)
						{
							if (m_textBlockList[i]->m_nLevel>nLevel)
							{
								m_textBlockList[i]->m_bSelected = true;
								nBottomSelected = i;
							}
							else
							{
								break;
							}
						}
						if (i>=m_textBlockList.size())
						{
							bMovable = false;
						}
					}
					else
					{
						m_textBlockList[i]->m_bSelected = true;
						nBottomSelected = i;
					}

					/*m_textBlockList[i]->m_bSelected = true;
					nBottomSelected = i;
					++i;
					for (;i<m_textBlockList.size()&&!m_textBlockList[i]->m_bShown; ++i)
					{
						m_textBlockList[i]->m_bSelected = true;
						nBottomSelected = i;
					}
					if (i == m_textBlockList.size())
						bMovable = false;*/
					break;
				}
		}
	}
	if (bMovable)
	{
		int nTopSelected = -1;
		for(int i=0; i < m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				nTopSelected = i;
				break;
			}
		}
		int nInsertPoint = m_textBlockList.size();//插入到最后一个位置上
		if (nInsertPoint<=m_textBlockList.size()-1&&!m_textBlockList[nInsertPoint]->m_bShown)
		{
			for (;nInsertPoint< m_textBlockList.size() && !m_textBlockList[nInsertPoint]->m_bShown; ++nInsertPoint);
		}
		int nLevelDelta = 0;
		if (nInsertPoint>=m_textBlockList.size())
		{
			nLevelDelta = - m_textBlockList[nTopSelected]->m_nLevel;
			//m_textBlockList[nTopSelected]->m_nLevel = 0;
		}
		else
		{
			nLevelDelta = m_textBlockList[nInsertPoint]->m_nLevel - m_textBlockList[nTopSelected]->m_nLevel;
			//m_textBlockList[nTopSelected]->m_nLevel = m_textBlockList[nInsertPoint]->m_nLevel;
			
		}
		for (int i=0; i<nBottomSelected-nTopSelected+1; ++i)
		{
			m_textBlockList[nTopSelected]->m_nLevel += nLevelDelta;
			m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint, m_textBlockList[nTopSelected]);
			m_textBlockList.erase(m_textBlockList.begin()+nTopSelected);
		}
		if (NULL != m_pTextBlockFocused)
		{
			for(int i=0; i < m_textBlockList.size(); ++i)
			{
				m_textBlockList[i]->m_bSelected = false;
			}
		}
	}
	else
	{
		if (NULL != m_pTextBlockFocused)
		{
			for(int i=0; i < m_textBlockList.size(); ++i)
			{
				m_textBlockList[i]->m_bSelected = false;
			}
		}
		return;
	}
	HideCaret();
	CalcLayout();
	CalcScrollBar();
	if (NULL!=m_pTextBlockFocused)
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	Invalidate();
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
}

//void CBooguNoteView::MoveSelectedItemsDown()
//{
//	bool bMovable = false;
//	if (NULL == m_pTextBlockFocused)
//	{
//		if (!(*(m_textBlockList.end()-1))->m_bSelected)
//		{
//			int nCount = 0;
//			for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
//			{
//				if ((*ppTextBlock)->m_bSelected)
//					++nCount;
//				
//			}
//			if (nCount<=1)
//				bMovable = true;
//		}
//	}
//	else
//	{
//		if ((*(m_textBlockList.end()-1)) != m_pTextBlockFocused)
//		{
//			bMovable = true;
//			for (int i=0; i<m_textBlockList.size(); ++i)
//				if (m_pTextBlockFocused == m_textBlockList[i]&&(i+1)<m_textBlockList.size()&&!m_textBlockList[i+1]->m_bShown)
//				{
//					bMovable = false;
//					break;
//				}
//		}
//	}
//	if (bMovable)
//	{
//		for(int i=0; i < m_textBlockList.size(); ++i)
//		{
//			if ((m_textBlockList[i] == m_pTextBlockFocused)||m_textBlockList[i]->m_bSelected)
//			{
//				while((i+1)<m_textBlockList.size())
//				{
//					CBooguNoteText* Temp = m_textBlockList[i];
//					m_textBlockList[i] = m_textBlockList[i+1];
//					m_textBlockList[i+1] = Temp;
//					++i;
//					if ((i+1)<m_textBlockList.size() && m_textBlockList[i+1]->m_bShown)
//						break;
//				}
//				if (i!=m_textBlockList.size())
//				{
//					m_nLastClicked = i+1;
//				}
//				HideCaret();
//				CalcLayout();
//				CalcScrollBar();
//				Invalidate();
//				m_bDirty = true;
//				GetParent().SendMessageW(WM_NCPAINT);
//				break;
//			}
//		}
//	}
//	else
//		return;
//	HideCaret();
//	CalcLayout();
//	CalcScrollBar();
//	if (NULL!=m_pTextBlockFocused)
//		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
//	Invalidate();
//	m_bDirty = true;
//	GetParent().SendMessageW(WM_NCPAINT);
//}

LRESULT CBooguNoteView::OnCreateChild(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CreateChildBlock(!g_config.bWideTextBlock);
	CalcLayout();
	CalcScrollBar();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnCreateNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CreateNextBlock(!g_config.bWideTextBlock);
	CalcLayout();
	CalcScrollBar();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnCreatePrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CreatePrevBlock();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnCreateParent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CreateParentBlock();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnCreateNewLine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	SendMessage(WM_KEYDOWN, VK_RETURN);
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMoveLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	MoveSelectedItemsLeft();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMoveRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	MoveSelectedItemsRight();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMoveUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	MoveSelectedItemsUp();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMoveTop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	MoveSelectedItemsTop();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMoveDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	MoveSelectedItemsDown();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMoveBottom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	MoveSelectedItemsBottom();
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((WORD)wParam == VK_UP && NULL != m_pTextBlockFocused)
	{
		bool bJump = false;
		if (m_pTextBlockFocused->m_bFile && NULL != m_pTextBlockFocused->m_pBitmap)
		{
			bJump = true;
		}
		else
		{
			CPoint ptCaret;
			GetCaretPos(&ptCaret);
			ClientToDevice(ptCaret);
			int nCaretHeight = -PointsToLogical(g_config.fontSize);
			//jump to upper block
			if (((abs(m_pTextBlockFocused->GetClientRect()->top-ptCaret.y)<nCaretHeight))&&(m_pTextBlockFocused!=m_textBlockList[0]))
			{
				bJump = true;
			}
		}
		if (bJump)
		{
			int index = GetCurrFocusedBlock();
			CleanAllSelected();
			m_nLastClicked = index;
			--index;
			while (index>=0&&!m_textBlockList[index]->m_bShown)
				--index;
			if (index>=0)
			{
				m_nLastClicked = index;
				if (NULL!=m_pTextBlockFocused)
				{
					m_pTextBlockFocused->GetTextServices()->TxSendMessage(WM_KILLFOCUS,0,0,0);
					//m_pTextBlockFocused->RevokeDragDrop();
					m_pTextBlockFocused->DetectFileBlock();
					CalcLayout();
					CalcScrollBar();
				}
				m_pTextBlockFocused = m_textBlockList[index];
				//m_pTextBlockFocused->RegisterDragDrop();
				m_textBlockList[index]->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
				//AutoAdjustFocusedBlockPosition();
			}
			Invalidate();
			bHandled = TRUE;
		}
		else
		{
			bHandled = FALSE;
		}
		AutoAdjustFocusedBlockPosition();
	}
	//jump to lower block
	else if ((WORD)wParam == VK_DOWN && NULL != m_pTextBlockFocused)
	{
		bool bJump = false;
		if (m_pTextBlockFocused->m_bFile && NULL != m_pTextBlockFocused->m_pBitmap)
		{
			bJump = true;
		}
		else
		{
			CPoint ptCaret;
			GetCaretPos(&ptCaret);
			ClientToDevice(ptCaret);
			int nCaretHeight = -PointsToLogical(g_config.fontSize);
			if (((abs(m_pTextBlockFocused->GetClientRect()->bottom-ptCaret.y)<2*nCaretHeight))&&(m_pTextBlockFocused!=m_textBlockList[m_textBlockList.size()-1]))
			{
				bJump = true;
			}
		}
		if (bJump)
		{
			int index = GetCurrFocusedBlock();
			CleanAllSelected();
			m_nLastClicked = index;
			++index;
			while (index<m_textBlockList.size()&&!m_textBlockList[index]->m_bShown)
				++index;
			if (index<m_textBlockList.size())
			{
				m_nLastClicked = index;
				if (NULL!=m_pTextBlockFocused)
				{
					m_pTextBlockFocused->GetTextServices()->TxSendMessage(WM_KILLFOCUS,0,0,0);
					//m_pTextBlockFocused->RevokeDragDrop();
					m_pTextBlockFocused->DetectFileBlock();
					CalcLayout();
					CalcScrollBar();
				}
				m_pTextBlockFocused = m_textBlockList[index];
				//m_pTextBlockFocused->RegisterDragDrop();
				m_textBlockList[index]->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
				//AutoAdjustFocusedBlockPosition();
			}
			Invalidate();
			bHandled = TRUE;
		}
		else
		{
			bHandled = FALSE;
		}
		AutoAdjustFocusedBlockPosition();
	}
	////clean all selected state.
	//else if ((WORD)wParam == VK_ESCAPE)
	//{
	//	CleanAllClipped();
	//	bHandled = TRUE;
	//}
	else if ( (WORD)wParam == VK_APPS)
	{
		POINT pt;
		GetCaretPos(&pt);
		ClientToScreen(&pt);
		//ScreenToClient(&pt);
		//ClientToDevice(pt);
		PopupMenu(pt);
	}
	else if ((WORD)wParam == VK_RETURN && NULL != m_pTextBlockFocused)
	{
		if (m_pTextBlockFocused->m_bFile)
		{
			TCHAR szTempCache[MAX_PATH*3];
			if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
			{
				_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
			}
			else if (L'.' == m_pTextBlockFocused->m_szCache[0])
			{
				if (!m_fileDirectory.IsEmpty())
				{
					swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
					PathRemoveFileSpec(szTempCache);
					PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
				}
			}
			else
			{
				_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
			}
			CString fn(szTempCache);
			int dot = fn.ReverseFind(_T('.'));
			CString ext = fn;
			ext.Delete(0, dot+1);
			ext.MakeLower();
			if (ext == _T("boo"))
			{
				if(g_config.bBooOpenInSameInstance)
				{
					GetParent().SendMessage(WM_USER_MAINFRM_OPENFILE, (WPARAM)szTempCache);
				}
				else
				{
					ShellExecute(NULL, NULL, szTempCache, NULL, NULL, SW_SHOW);
				}
			}
			else
			{
				ShellExecute(NULL, NULL, szTempCache, NULL, NULL, SW_SHOW);
			}
		}
		else
		{
			bHandled = FALSE;
		}
	}
	else
	{
		bHandled = FALSE;
	}
	return 0;
}

int CBooguNoteView::GetCurrFocusedBlock()
{
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i] == m_pTextBlockFocused)
			return i;
	}
}

LRESULT CBooguNoteView::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if ((m_pTextBlockFocused && ((WORD)wParam != VK_CONTROL) && ((WORD)wParam != VK_SHIFT)))
	{
		AutoAdjustFocusedBlockPosition();
	}
	bHandled = FALSE;
	return 0;
}

void CBooguNoteView::PageUp()
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	int delta = rcWindow.Height()/2;
	//if (m_ptOffset.y>(m_nVRange-rcWindow.Height()))
	//	m_ptOffset.y=m_nVRange-rcWindow.Height();
	if (m_ptOffset.y-delta<0)
		m_ptOffset.y=0;
	else
		m_ptOffset.y -= delta;
	HideCaret();
	CalcScrollBar();
	Invalidate();
}

LRESULT CBooguNoteView::OnPageUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PageUp();
	return 0;
}

void CBooguNoteView::PageDown()
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	int delta = rcWindow.Height()/2;
	if (m_ptOffset.y + delta>(m_nVRange-rcWindow.Height()))
		m_ptOffset.y=m_nVRange-rcWindow.Height();
	else
		m_ptOffset.y += delta;
	//if (m_ptOffset.y-delta<0)
	//	m_ptOffset.y=0;
	HideCaret();
	CalcScrollBar();
	Invalidate();
}

LRESULT CBooguNoteView::OnPageDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PageDown();
	return 0;
}

void CBooguNoteView::PageLeft()
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	int delta = rcWindow.Width()/2;
	//if (m_ptOffset.y>(m_nVRange-rcWindow.Height()))
	//	m_ptOffset.y=m_nVRange-rcWindow.Height();
	if (m_ptOffset.x-delta<0)
		m_ptOffset.x=0;
	else
		m_ptOffset.x -= delta;
	HideCaret();
	CalcScrollBar();
	Invalidate();
}

LRESULT CBooguNoteView::OnPageLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PageLeft();
	return 0;
}

void CBooguNoteView::PageRight()
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	int delta = rcWindow.Width()/2;
	if (m_ptOffset.x + delta>(m_nHRange-rcWindow.Width()))
		m_ptOffset.x=m_nHRange-rcWindow.Width();
	else
		m_ptOffset.x += delta;
	//if (m_ptOffset.y-delta<0)
	//	m_ptOffset.y=0;
	HideCaret();
	CalcScrollBar();
	Invalidate();
}

LRESULT CBooguNoteView::OnPageRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PageRight();
	return 0;
}

LRESULT CBooguNoteView::OnHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CleanAllSelected();
	m_ptOffset.y=0;
	m_nLastClicked = 0;
	//if (NULL!=m_pTextBlockFocused)
		//m_pTextBlockFocused->RevokeDragDrop();
	m_pTextBlockFocused = m_textBlockList[0];
	//m_pTextBlockFocused->RegisterDragDrop();
	m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	CalcScrollBar();
	Invalidate();
	return 0;
}

LRESULT CBooguNoteView::OnTimeStamp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (NULL != m_pTextBlockFocused)
		m_pTextBlockFocused->TimeStamp();
	return 0;
}

LRESULT CBooguNoteView::OnEnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CleanAllSelected();
	int i=m_textBlockList.size()-1;
	for (; i>=0; --i)
	{
		if (m_textBlockList[i]->m_bShown)
		{
			break;
		}
	}
	m_nLastClicked = i;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	m_ptOffset.y=m_nVRange-rcWindow.Height();
	//if (NULL!=m_pTextBlockFocused)
		//m_pTextBlockFocused->RevokeDragDrop();
	m_pTextBlockFocused = m_textBlockList[i];
	//m_pTextBlockFocused->RegisterDragDrop();
	m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	CalcScrollBar();
	Invalidate();
	return 0;
}

LRESULT CBooguNoteView::OnLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_ptOffset.x = 0;
	CalcScrollBar();
	Invalidate();
	return 0;
}


void CBooguNoteView::AutoAdjustFocusedBlockPosition()
{
	CPoint ptCaret;
	BOOL bGet = GetCaretPos(&ptCaret);
	bool bModified = false;
	//follow caret position to move block position
	if (ptCaret.x<=0 && ptCaret.y<=0 && m_pTextBlockFocused)
	{
		CRect rcWindow;
		GetWindowRect(rcWindow);
		CRect prcTBClient =	m_pTextBlockFocused->GetClientRect();
		if (prcTBClient.top - m_ptOffset.y>(rcWindow.Height() - m_nSingleLineHeight /*- - ::GetSystemMetrics(SM_CYSIZEFRAME)*/ - g_config.hostBorder -30))//why mul 2 here? because, in CalcLayout() "m_nHRange += GetSystemMetrics(SM_CXSIZEFRAME);"
		{
			//int nOffset = (rcWindow.Height()+PointsToLogical(g_config.fontSize) /*- ::GetSystemMetrics(SM_CYSIZEFRAME) */- g_config.hostBorder - 30 - ptCaret.y);
			m_ptOffset.y = prcTBClient.top + 2*m_nSingleLineHeight + g_config.nScrollbarWidth + g_config.nScrollbarMargin - rcWindow.Height();
			//ptCaret.y += nOffset;
			bModified = true;
		}
		if (prcTBClient.top<(g_config.hostBorder))//why mul 2 here? because, in CalcLayout() "m_nHRange += GetSystemMetrics(SM_CXSIZEFRAME);"
		{
			//int nOffset = ( g_config.hostBorder - ptCaret.y);
			m_ptOffset.y = prcTBClient.top;
			//ptCaret.y += nOffset;
			bModified = true;
		}
	}
	else if (/*ptCaret.y>0 && */!m_bCancelAutoAdjust && m_pTextBlockFocused /*&& !m_pTextBlockFocused->m_bFile*/)
	{
		//m_pTextBlockFocused->TxWindowProc(m_hWnd, uMsg, wParam, lParam);
		
		//TCHAR __buf[1024];
		//_stprintf(__buf, _T("AutoAdjustFocusedBlockPosition: %d, %d"), ptCaret.x, ptCaret.y);
		//OutputDebugString(__buf);
		CRect rcWindow;
		GetWindowRect(rcWindow);
		
		if (g_config.bAutoAdjustFocusedTextBlockPos&&(TEXT_BLOCK_NARROW ==m_pTextBlockFocused->m_nExpandState))
		{
			CRect prcTBClient =	m_pTextBlockFocused->GetClientRect();
			//CRect rcWindow;
			//GetClientRect(&rcWindow);
			int nOffsetX = 0;
			if ((prcTBClient.left- (m_nSingleLineHeight+g_config.hostBorder))<m_ptOffset.x)//why mul 2 here? because, in CalcLayout() "m_nHRange += GetSystemMetrics(SM_CXSIZEFRAME);"
			{
				nOffsetX = prcTBClient.left - (m_nSingleLineHeight+2+g_config.hostBorder) - m_ptOffset.x;
				m_ptOffset.x = prcTBClient.left - (m_nSingleLineHeight+2+g_config.hostBorder);
			}
			else if (prcTBClient.right>(rcWindow.Width()/*-::GetSystemMetrics(SM_CXSIZEFRAME)*2- g_config.hostBorder*/))//why mul 2 here? because, in CalcLayout() "m_nHRange += GetSystemMetrics(SM_CXSIZEFRAME);"
			{
				nOffsetX = prcTBClient.right + g_config.hostBorder - rcWindow.Width() - m_ptOffset.x;
				m_ptOffset.x = prcTBClient.right - rcWindow.Width();
			}
			
			//CPoint ptCaret;
			//GetCaretPos(&ptCaret);
			ptCaret.x += nOffsetX;
			if (0!=nOffsetX)
			{
				bModified = true;
			}
			//SetCaretPos(ptCaret.x, ptCaret.y);
		}
		else
		{
			if (ptCaret.x>(rcWindow.Width()-(g_config.nBlockMargin+g_config.nHandleWidth)-10))
			{
				int nOffset = (rcWindow.Width()-(g_config.nBlockMargin+g_config.nHandleWidth)-10/*-::GetSystemMetrics(SM_CXSIZEFRAME)*2*/ - g_config.hostBorder - ptCaret.x);
				m_ptOffset.x -= nOffset;
				ptCaret.x += nOffset;
				bModified = true;
			}
			if (ptCaret.x<2)
			{
				int nOffset = 2- ptCaret.x; 
				m_ptOffset.x -= nOffset;
				ptCaret.x += nOffset;
				bModified = true;
			}
		}
		if (ptCaret.y>(rcWindow.Height()-m_nSingleLineHeight /*- - ::GetSystemMetrics(SM_CYSIZEFRAME)*/ - g_config.hostBorder -30))//why mul 2 here? because, in CalcLayout() "m_nHRange += GetSystemMetrics(SM_CXSIZEFRAME);"
		{
			int nOffset = (rcWindow.Height()-m_nSingleLineHeight /*- ::GetSystemMetrics(SM_CYSIZEFRAME) */- g_config.hostBorder - 30 - ptCaret.y);
			m_ptOffset.y -= nOffset;
			ptCaret.y += nOffset;
			bModified = true;
		}
		if (ptCaret.y<(g_config.hostBorder))//why mul 2 here? because, in CalcLayout() "m_nHRange += GetSystemMetrics(SM_CXSIZEFRAME);"
		{
			int nOffset = ( g_config.hostBorder - ptCaret.y);
			m_ptOffset.y -= nOffset;
			ptCaret.y += nOffset;
			bModified = true;
		}
		SetCaretPos(ptCaret.x, ptCaret.y);
		
		///CalcLayout();
		
	}
	
	if (bModified)
	{
		//OutputDebugString(_T("bModified"));
		CalcScrollBar();
		Invalidate();
	}
	m_bCancelAutoAdjust = false;
}

void CBooguNoteView::SetSelectedItemIcon(DWORD Icon)
{
	if (NULL == m_pTextBlockFocused)
	{
		int nCount = 0;
		for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
		{
			if ((*ppTextBlock)->m_bSelected)
				++nCount;
		}
		//if (nCount==1)
		//{
			for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
			{
				if ((*ppTextBlock)->m_bSelected)
				{
					(*ppTextBlock)->m_pTextIcon->state = Icon;
					HideCaret();
					CalcLayout();
					Invalidate();
					//break;
				}
			}
		//}
	}
	else
	{
		m_pTextBlockFocused->m_pTextIcon->state = Icon;
		HideCaret();
		CalcLayout();
		Invalidate();
	}
}

LRESULT CBooguNoteView::OnPopupMenuPlainText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_PLAINTEXT);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupMenuTodo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_TODO);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupMenuDone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_DONE);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupMenuCross(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_CROSS);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupMenuIdea(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_IDEA);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupMenuStar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_STAR);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupMenuQuestion(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_QUESTION);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupMenuWarning(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetSelectedItemIcon(TEXT_ICON_WARNING);
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}

void CBooguNoteView::Paste()
{
	if (NULL != m_pTextBlockFocused /*&& TEXT_BLOCK_SHRINK!=m_pTextBlockFocused->m_nExpandState*/)
	{
		if (IsClipboardFormatAvailable(BOOGUNOTE_TYPE))
		{
			//get insert point
			int nInsertPoint = 0;
			int nCurrLevel = 0;
			for (; nInsertPoint<m_textBlockList.size(); ++nInsertPoint)
			{
				if (m_pTextBlockFocused == m_textBlockList[nInsertPoint])
				{
					nCurrLevel = m_textBlockList[nInsertPoint]->m_nLevel;
					do
					{
						nInsertPoint++;
					}
					while (nInsertPoint<m_textBlockList.size()&& m_textBlockList[nInsertPoint]->m_nLevel>nCurrLevel);
					
					//if (nInsertPoint<0)
					//{
					//	nInsertPoint=0;
					//}
					break;
				}
			}

			nInsertPoint--;

			BSTR bstrCache;
			m_textBlockList[nInsertPoint]->GetTextServices()->TxGetText(&bstrCache);
			int nTextLength = SysStringLen(bstrCache);
			//if the past point is an empty line.
			bool bInsertPointIsEmpty = false;
			if (1 == nTextLength && !(m_textBlockList[nInsertPoint]->m_bFile && NULL != m_textBlockList[nInsertPoint]->m_pBitmap))
			{
				bInsertPointIsEmpty = true;
			}
			SysFreeString(bstrCache);

			if (!bInsertPointIsEmpty || TEXT_BLOCK_SHRINK==m_pTextBlockFocused->m_nExpandState)
			{
				nInsertPoint++;
			}
			else
			{
				delete m_textBlockList[nInsertPoint];
				m_textBlockList.erase(m_textBlockList.begin()+nInsertPoint);
				m_pTextBlockFocused = NULL;
			}
			

			//get xml
			XMLElement* r = XML::Paste(NULL, BOOGUNOTE_TYPE);
			
			int nC = r->GetChildrenNum();
			if (nC <=0 )
				return;
			for(int i = 0 ; i < nC ; i++)
			{
				XMLElement* ch = r->GetChildren()[i];
				int nMaxElName = ch->GetElementName(0);
				char* n = new char[nMaxElName + 1];
				ch->GetElementName(n);
				if (strcmp(n, "item") !=0)
				{
					delete [] n;
					continue;
				}
				delete [] n;

				XMLVariable* v = NULL;
				int nValueLen = -1;
				char* pValueBuf = NULL;
				int nLength = -1;
				TCHAR* utf8buf = NULL;
			
				//content
				nValueLen = ch->FindVariableZ("content")->GetValue(0);
				pValueBuf = new char[nValueLen + 1];
				memset(pValueBuf, '\0', nValueLen+1);
				ch->FindVariableZ("content")->GetValue(pValueBuf);
				pValueBuf[nValueLen] = '\0';
				nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
				utf8buf = new TCHAR[nLength+1];
				MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
				
				delete [] pValueBuf;
				
				CBooguNoteText* pTextBlock = NULL;
                
                XMLVariable* vLevel = NULL;
                int ilevel = 0;          
                vLevel  = ch->FindVariableZ("level");
                if (NULL == vLevel )
                {
                    ilevel = 0;
                }
                else
                {
                    ilevel = vLevel ->GetValueInt() + nCurrLevel;
                }
                int iCreateTextBlockWidth = g_config.narrowWidth - (ilevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
				
				//block
				v = ch->FindVariableZ("block");
				if (NULL == v)
				{
					pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
					pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					TCHAR* blockBuf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,blockBuf, nLength+1);
					
					if (_tcscmp(blockBuf, _T("wide"))==0)
					{
						pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
						pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
					}
					else if (_tcscmp(blockBuf, _T("narrow"))==0)
					{
						pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
						pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
					}
					else if (_tcscmp(blockBuf, _T("shrink"))==0)
					{
						pTextBlock = CreateTextBlock(0,0, iCreateTextBlockWidth, utf8buf);
						pTextBlock->m_nExpandState = TEXT_BLOCK_SHRINK;
					}
					delete [] blockBuf;
					delete [] pValueBuf;
				}
				delete [] utf8buf;

				//branch
				v = ch->FindVariableZ("branch");
				if (NULL == v)
				{
					pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					utf8buf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
					if (_tcscmp(utf8buf, _T("none"))==0)
					{
						pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
					}
					else if (_tcscmp(utf8buf, _T("open"))==0)
					{
						pTextBlock->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
					}
					else if (_tcscmp(utf8buf, _T("close"))==0)
					{
						pTextBlock->m_pTextHandle->state = TEXT_HANDLE_CLOSED;
					}
					delete [] utf8buf;
					delete [] pValueBuf;
				}

				//icon
				v = ch->FindVariableZ("icon");
				if (NULL == v)
				{
					pTextBlock->m_pTextIcon->state = TEXT_ICON_PLAINTEXT;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					utf8buf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
					if (_tcscmp(utf8buf, _T("none"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_PLAINTEXT;
					}
					else if (_tcscmp(utf8buf, _T("flag"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_TODO;
					}
					else if (_tcscmp(utf8buf, _T("tick"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_DONE;
					}
					else if (_tcscmp(utf8buf, _T("cross"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_CROSS;
					}
					else if (_tcscmp(utf8buf, _T("star"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_STAR;
					}
					else if (_tcscmp(utf8buf, _T("question"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_QUESTION;
					}
					else if (_tcscmp(utf8buf, _T("warning"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_WARNING;
					}
					else if (_tcscmp(utf8buf, _T("idea"))==0)
					{
						pTextBlock->m_pTextIcon->state = TEXT_ICON_IDEA;
					}
					delete [] utf8buf;
					delete [] pValueBuf;
				}
				
				//level
				v = ch->FindVariableZ("level");
				if (NULL == v)
				{
					pTextBlock->m_nLevel = 0;
				}
				else
				{
					pTextBlock->m_nLevel = v->GetValueInt() + nCurrLevel;
				}

				//shown
				v = ch->FindVariableZ("IsShown");
				if (NULL == v)
				{
					pTextBlock->m_bShown = true;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					utf8buf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
					if (_tcscmp(utf8buf, _T("true"))==0)
					{
						pTextBlock->m_bShown = true;
					}
					else if (_tcscmp(utf8buf, _T("false"))==0)
					{
						pTextBlock->m_bShown = false;
					}
					delete [] utf8buf;
					delete [] pValueBuf;
				}

				//show branch
				v = ch->FindVariableZ("ShowBranch");
				if (NULL == v)
				{
					pTextBlock->m_bShowBranch = false;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					utf8buf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
					if (_tcscmp(utf8buf, _T("true"))==0)
					{
						pTextBlock->m_bShowBranch = true;
					}
					else if (_tcscmp(utf8buf, _T("false"))==0)
					{
						pTextBlock->m_bShowBranch = false;
					}
					delete [] utf8buf;
					delete [] pValueBuf;
				}

				//IsBold
				v = ch->FindVariableZ("IsBold");
				if (NULL == v)
				{
					pTextBlock->m_bBold = false;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					utf8buf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
					if (_tcscmp(utf8buf, _T("true"))==0)
					{
						pTextBlock->m_bBold = true;
					}
					else if (_tcscmp(utf8buf, _T("false"))==0)
					{
						pTextBlock->m_bBold = false;
					}
					delete [] utf8buf;
					delete [] pValueBuf;
				}

				//Text Color
				v = ch->FindVariableZ("TextColor");
				if (NULL == v)
				{
					pTextBlock->m_TextClr = 0x00000000;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					utf8buf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
					StringToBinary(utf8buf, sizeof(COLORREF)*2+1, (BYTE*)&pTextBlock->m_TextClr);
					delete [] utf8buf;
					delete [] pValueBuf;
				}

				//Background Color
				v = ch->FindVariableZ("BkgrdColor");
				if (NULL == v)
				{
					pTextBlock->m_BkgrdClr = 0x00FFFFFF;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					nLength = MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1, 0, 0);
					utf8buf = new TCHAR[nLength+1];
					MultiByteToWideChar(CP_UTF8, 0, pValueBuf, nValueLen+1,utf8buf, nLength+1);
					StringToBinary(utf8buf, sizeof(COLORREF)*2+1, (BYTE*)&pTextBlock->m_BkgrdClr);
					delete [] utf8buf;
					delete [] pValueBuf;
				}
				
				v = ch->FindVariableZ("ModifyTime");
				if (NULL == v)
				{
					pTextBlock->m_ModifyTime = 0;
				}
				else
				{
					nValueLen = v->GetValue(0);
					pValueBuf = new char[nValueLen + 1];
					memset(pValueBuf, '\0', nValueLen+1);
					v->GetValue(pValueBuf);
					pValueBuf[nValueLen] = '\0';
					tm timeinfo;
					sscanf_s(pValueBuf, "%d-%d-%d %d:%d:%d", &(timeinfo.tm_year), &(timeinfo.tm_mon), &(timeinfo.tm_mday), &(timeinfo.tm_hour), &(timeinfo.tm_min), &(timeinfo.tm_sec));
					timeinfo.tm_year -= 1900;
					timeinfo.tm_mon  -= 1;
					pTextBlock->m_ModifyTime = mktime(&timeinfo);
				}

				CHARFORMAT2 cf;
				cf.cbSize = sizeof(CHARFORMAT2);
				pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				cf.dwMask = CFM_COLOR|CFM_BACKCOLOR;
				cf.dwEffects &= ~CFE_AUTOBACKCOLOR;
				cf.dwEffects &= ~~CFE_AUTOCOLOR;
				cf.crTextColor = pTextBlock->m_TextClr;
				cf.crBackColor = pTextBlock->m_BkgrdClr;
				pTextBlock->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);

				//m_textBlockList.push_back(pTextBlock);
				if (TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState)
					ShrinkTextBlock(pTextBlock);
				if (pTextBlock->m_bBold)
				{
					CHARFORMAT cf;
					cf.cbSize = sizeof(CHARFORMAT);
					pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwEffects |= CFE_BOLD;
					pTextBlock->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
				}
				pTextBlock->DetectFileBlock();
				m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint,pTextBlock);
				
			}
			CalcLayout();
			CalcScrollBar();
			Invalidate();
			::SendMessage(GetClipboardOwner(), WM_USER_DEL_CLIPPED, (WPARAM)this, 0);
			
		}
		else if (IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			(m_pTextBlockFocused->GetTextServices())->TxSendMessage(EM_PASTESPECIAL, CF_TEXT, 0, 0);
			time(&(m_pTextBlockFocused->m_ModifyTime));
		}
		else if (IsClipboardFormatAvailable(CF_DIB))
		{
			TCHAR bufDir[MAX_PATH];
			//GetPrivateProfileString(_T("Image"),_T("SaveDir"), _T("C:\\"),bufDir, MAX_PATH, g_config.strConfigFileName);

			//CString fileDirectory;
			SYSTEMTIME systime;
			GetLocalTime(&systime);
			TCHAR szFileName[1024];
			_stprintf(szFileName, _T("%04u%02u%02u_%02u%02u%02u_%04u"), systime.wYear,
				systime.wMonth,
				systime.wDay,
				systime.wHour,
				systime.wMinute,
				systime.wSecond,
				systime.wMilliseconds);

			CString fileDirectory;
			CString ext;
			if (m_strDefaultSaveDir.IsEmpty())
			{
				CFileDialogFilter strFilter(_T("(*.png)||(*.jpg)||(*.gif)||(*.tiff)||"));
				CFileDialog fd(FALSE, _T("png"), szFileName,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, strFilter);
				//fd.m_ofn.Flags = OFN_DONTADDTORECENT|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
				//fm.ReleaseBuffer();
				if (IDCANCEL == fd.DoModal())
				{
					return;
				}
				fileDirectory = fd.m_szFileName;
				CString strExt = fileDirectory;
				int dot = strExt.ReverseFind(_T('.'));
				ext = strExt;
				ext.Delete(0, dot+1);
				ext.MakeLower();
			}
			else
			{
				fileDirectory = m_strDefaultSaveDir;
				PathAddBackslash(fileDirectory.GetBuffer(MAX_PATH));
				fileDirectory.ReleaseBuffer();
				PathAppend(fileDirectory.GetBuffer(MAX_PATH), szFileName);
				fileDirectory.ReleaseBuffer();
				fileDirectory = fileDirectory + L"." + m_strDefaultImgExtension;
				ext = m_strDefaultImgExtension;
			}
			

			CLSID clsid;
			if (0==ext.Compare(_T("png")))
			{
				GetEncoderClsid(L"image/png", &clsid);
			}
			else if (0==ext.Compare(_T("jpeg")) || 0==ext.Compare(_T("jpg")))
			{
				GetEncoderClsid(L"image/jpeg", &clsid);
			}
			else if (0==ext.Compare(_T("gif")))
			{
				GetEncoderClsid(L"image/gif", &clsid);
			}
			else if (0==ext.Compare(_T("tiff")))
			{
				GetEncoderClsid(L"image/tiff", &clsid);
			}
			else if (0==ext.Compare(_T("bmp")))
			{
				GetEncoderClsid(L"image/bmp", &clsid);
			}
			else
			{
				return;
			}

			GLOBALHANDLE    hGMem ;
			LPBITMAPINFO    lpBI ;
			void*            pDIBBits;
	    
			if (OpenClipboard())
			{
				hGMem = GetClipboardData(CF_DIB) ;
				lpBI = (LPBITMAPINFO)GlobalLock(hGMem);
				// point to DIB bits after BITMAPINFO object

				pDIBBits = (void*)(((char*)(lpBI + 1)) - 4); // - 4 refer to http://topic.csdn.net/t/20050531/10/4047827.html
				Bitmap bmp(lpBI, pDIBBits);
				bmp.Save(fileDirectory, &clsid);

				//get insert point
				int nInsertPoint = 0;
				int nCurrLevel = 0;
				for (; nInsertPoint<m_textBlockList.size(); ++nInsertPoint)
				{
					if (m_pTextBlockFocused == m_textBlockList[nInsertPoint])
					{
						nCurrLevel = m_textBlockList[nInsertPoint]->m_nLevel;
						do
						{
							nInsertPoint++;
						}
						while (nInsertPoint<m_textBlockList.size()&& m_textBlockList[nInsertPoint]->m_nLevel>nCurrLevel);
						
						//if (nInsertPoint<0)
						//{
						//	nInsertPoint=0;
						//}
						break;
					}
				}

				nInsertPoint--;

				BSTR bstrCache;
				m_textBlockList[nInsertPoint]->GetTextServices()->TxGetText(&bstrCache);
				int nTextLength = SysStringLen(bstrCache);
				//if the past point is an empty line.
				bool bInsertPointIsEmpty = false;
				if (1 == nTextLength && !(m_textBlockList[nInsertPoint]->m_bFile && NULL != m_textBlockList[nInsertPoint]->m_pBitmap))
				{
					bInsertPointIsEmpty = true;
				}
				SysFreeString(bstrCache);

				if (!bInsertPointIsEmpty || TEXT_BLOCK_SHRINK==m_pTextBlockFocused->m_nExpandState)
				{
					nInsertPoint++;
				}
				else
				{
					delete m_textBlockList[nInsertPoint];
					m_textBlockList.erase(m_textBlockList.begin()+nInsertPoint);
					m_pTextBlockFocused = NULL;
				}

				//CString strRelativePath;
				//PathRelativePathTo(strRelativePath.GetBuffer(MAX_PATH),
				//	m_fileDirectory,
				//	FILE_ATTRIBUTE_NORMAL,
				//	fileDirectory,
				//	FILE_ATTRIBUTE_NORMAL);
				//strRelativePath.ReleaseBuffer();
				//if (!strRelativePath.IsEmpty())
				//{
				//	fileDirectory = strRelativePath;
				//}

				CBooguNoteText* pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-(nCurrLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder), fileDirectory);
				//pTextBlock->m_pTextHandle->state = pTb->m_TextHandleState;
				pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
				//pTextBlock->m_pTextIcon->state = pTb->m_TextIcon;
				pTextBlock->m_nLevel = nCurrLevel;
				//pTextBlock->m_bShown = pTb->m_bShown;
				//pTextBlock->m_bBold = pTb->m_bBold;
				//pTextBlock->m_bSelected = true;
				m_textBlockList.insert(m_textBlockList.begin()+nInsertPoint,pTextBlock);
				time(&(pTextBlock->m_ModifyTime));
				pTextBlock->DetectFileBlock();

				m_pTextBlockFocused = pTextBlock;
				//for(int k=MAX_PATH-1; k>=0; --k)
				//{
				//	if (_T('\\') == fileDirectory[k])
				//	{
				//		fd.m_szFileName[k+1] = _T('\0');
				//		break;
				//	}
				//}
				//BOOL r = WritePrivateProfileString(_T("Image"),_T("SaveDir"), fd.m_szFileName,g_config.strConfigFileName);
				//DWORD dw = GetLastError();
				GlobalUnlock(hGMem) ;
				CloseClipboard() ;
				CalcLayout();
				CalcScrollBar();
				Invalidate();
			}
		}
		else if (IsClipboardFormatAvailable(CF_HDROP))
		{
			if (OpenClipboard())
			{
				GLOBALHANDLE    hGMem ;
				HDROP    hDrop;
				hGMem = GetClipboardData(CF_HDROP) ;
				hDrop = (HDROP)GlobalLock(hGMem);
				PasteFiles(hDrop);
				GlobalUnlock(hGMem) ;
				CloseClipboard();
			}
		}
		else
		{
			//(m_pTextBlockFocused->GetTextServices())->TxSendMessage(WM_PASTE, 0, 0, 0);
		}
		
		m_bDirty = true;
		GetParent().SendMessageW(WM_NCPAINT);
	}
}
LRESULT CBooguNoteView::OnPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//((aseciChildControl.ptwh)->GetTextServices())->TxSendMessage(WM_PASTE, 0, 0, 0);
	Paste();
	bHandled = FALSE;
	return 0;
}

LRESULT CBooguNoteView::OnCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CleanAllClipped();
	CHARRANGE cr = { 0, 0 };
	if (NULL!=m_pTextBlockFocused)
	{
		(m_pTextBlockFocused->GetTextServices())->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr,0);
		//m_pTextBlockFocused->m_bSelected = true;
	}

	if (cr.cpMin == cr.cpMax)
	{
		bool bHaveSelected = false;
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				m_textBlockList[i]->m_bClipped = true;
				m_textBlockList[i]->SetDisabled(TRUE);
				bHaveSelected = true;
			}
		}

		if (!bHaveSelected)
		{
			for (int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i] == m_pTextBlockFocused)
				{
					m_textBlockList[i]->m_bClipped = true;
					m_textBlockList[i]->m_bSelected = true;
					m_textBlockList[i]->SetDisabled(TRUE);
					if ((i+1)<m_textBlockList.size() && m_textBlockList[i+1]->m_bShown == false)
					{
						int nCurrLevel = m_pTextBlockFocused->m_nLevel;
						i++;
						for (; i<m_textBlockList.size()&&m_textBlockList[i]->m_nLevel > nCurrLevel; ++i)
						{
							m_textBlockList[i]->m_bClipped = true;
							m_textBlockList[i]->m_bSelected = true;
							m_textBlockList[i]->SetDisabled(TRUE);
						}
					}
					break;
				}
			}
		}

		if ( !OpenClipboard() )
		{
		  MessageBox( _T("不能打开剪贴板"), _T("警告"), MB_OK|MB_ICONSTOP);
		  return 0;
		}

		if( !EmptyClipboard() )
		{
			MessageBox( _T("不能清空剪贴板"), _T("警告"), MB_OK|MB_ICONSTOP);
			return 0;
		}
		CopyToBlock();
		CloseClipboard();
		if (NULL!=m_pTextBlockFocused)
		{
			m_pTextBlockFocused->m_bSelected = false;
			m_pTextBlockFocused = NULL;
		}
		Invalidate();
	}
	else
		(m_pTextBlockFocused->GetTextServices())->TxSendMessage(WM_CUT, 0, 0, 0);
	
	return 0;
}

LRESULT CBooguNoteView::OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHARRANGE cr = { 0, 0 };
	if (NULL!=m_pTextBlockFocused)
		(m_pTextBlockFocused->GetTextServices())->TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr,0);
	if (cr.cpMin == cr.cpMax)
	{
		if ( !OpenClipboard() )
		{
		  MessageBox( _T("不能打开剪贴板"), _T("警告"), MB_OK|MB_ICONSTOP);
		  return 0;
		}

		if( !EmptyClipboard() )
		{
			MessageBox( _T("不能清空剪贴板"), _T("警告"), MB_OK|MB_ICONSTOP);
			return 0;
		}
		////clean clipped status
		//m_Clipped = false;
		bool bHaveSelected = false;
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected)
			{
				bHaveSelected = true;
				break;
			}
		}

		if (!bHaveSelected)
		{
			for (int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i] == m_pTextBlockFocused)
				{
					m_textBlockList[i]->m_bSelected = true;
					if ((i+1)<m_textBlockList.size() && m_textBlockList[i+1]->m_bShown == false)
					{
						int nCurrLevel = m_pTextBlockFocused->m_nLevel;
						i++;
						for (;  i<m_textBlockList.size()&&m_textBlockList[i]->m_nLevel > nCurrLevel; ++i)
						{
							m_textBlockList[i]->m_bSelected = true;
						}
					}
					break;
				}
			}
		}

		CopyToBlock();
		CopyToText();

		CloseClipboard();
		/*PaintWholeView();*/
	}
	else
	{
		if (NULL!=m_pTextBlockFocused)
			(m_pTextBlockFocused->GetTextServices())->TxSendMessage(WM_COPY, 0, 0, 0);
	}
	return 0;
}

LRESULT CBooguNoteView::OnUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pTextBlockFocused!=NULL)
	{
		(m_pTextBlockFocused->GetTextServices())->TxSendMessage(EM_UNDO, 0, 0, 0);
	}
	else
	{
		for(int i=0; i<undoList.size(); ++i)
		{
			m_textBlockList.insert(m_textBlockList.begin()+undoIndexList[i], undoList[i]);
		}
		undoList.clear();
		undoIndexList.clear();
		CalcLayout();
		CalcScrollBar();
	}
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}

LRESULT CBooguNoteView::OnRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pTextBlockFocused!=NULL)
	{
		(m_pTextBlockFocused->GetTextServices())->TxSendMessage(EM_REDO, 0, 0, 0);
		m_bDirty = true;
		GetParent().SendMessageW(WM_NCPAINT);
	}
	return 0;
}

LRESULT CBooguNoteView::OnBold(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetTextBold();
	return 0;
}

LRESULT CBooguNoteView::OnLBottonUp(UINT uMsg, WPARAM wParam, LPARAM &lParam, BOOL& bHandled)
{
	if (GetCapture() == m_hWnd)
	{
		ReleaseCapture();
	}
	CPoint originalPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	//if (!m_VThumbDraged && !m_HThumbDraged && FALSE == m_rcVScroll.PtInRect(originalPoint) && FALSE == m_rcHScroll.PtInRect(originalPoint))
	//{
	//	AutoAdjustFocusedBlockPosition();
	//}
	int x = GET_X_LPARAM(lParam) -m_ptOffset.x;
	int y = GET_Y_LPARAM(lParam) -m_ptOffset.y;
	lParam = MAKELPARAM((WORD)x, (WORD)y);
	m_VThumbDraged = false;
	m_HThumbDraged = false;
	//GetParent().SendMessage(WM_NCPAINT);//for redraw caption
	bHandled = FALSE;
	return 0;
}


LRESULT CBooguNoteView::OnLBottonDown(UINT uMsg, WPARAM wParam, LPARAM &lParam, BOOL& bHandled)
{
	GetParent().SendMessageW(WM_NCPAINT);
	bHandled = FALSE;
	LPARAM oritinallParam = lParam;
	CPoint originalPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	int x = GET_X_LPARAM(lParam) +m_ptOffset.x;
	int y = GET_Y_LPARAM(lParam) +m_ptOffset.y;
	lParam = MAKELPARAM((WORD)x, (WORD)y);
	if (TRUE == m_rcVThumb.PtInRect(originalPoint))
	{
		SetCapture();
		m_VThumbDraged = true;
		m_ptDragStart.x = x; 
		m_ptDragStart.y = y; 
		DeviceToClient(m_ptDragStart);
		m_nOriginalOffsetY = m_ptOffset.y;
		bHandled = TRUE;
		return 0;

	}
	else if (TRUE == m_rcHThumb.PtInRect(originalPoint))
	{
		SetCapture();
		m_HThumbDraged = true;
		m_ptDragStart.x = x; 
		m_ptDragStart.y = y; 
		DeviceToClient(m_ptDragStart);
		m_nOriginalOffsetX = m_ptOffset.x;
		bHandled = TRUE;
		return 0;
	}
	else if (TRUE == m_rcVScroll.PtInRect(originalPoint))//测试是否点中纵向滚动条
	{
		if ((originalPoint.y>m_rcVScroll.top)&&(originalPoint.y<m_rcVThumb.top))
		{
			PageUp();
		}
		else if ((originalPoint.y<m_rcVScroll.bottom)&&(originalPoint.y>m_rcVThumb.bottom))
		{
			PageDown();
		}
		//int nAvailableLength = 125-25-2*2;
		//if (originalPoint.y<(m_rcVScroll.top+2+(m_rcVThumb.Height()/2)))
		//{
		//	originalPoint.y = m_rcVScroll.top+2+(m_rcVThumb.Height()/2);
		//}
		//else if (originalPoint.y>(m_rcVScroll.bottom-2-(m_rcVThumb.Height()/2)))
		//{
		//	originalPoint.y = m_rcVScroll.bottom-2-(m_rcVThumb.Height()/2);
		//}
		//CPoint ptCaret;
		//GetCaretPos(&ptCaret);
		//ptCaret.y += m_ptOffset.y;
		//m_ptOffset.y = ((originalPoint.y - m_rcVThumb.Height()/2 - (m_rcVScroll.top+2)))*m_nVRange/nAvailableLength;
		//ptCaret.y -= m_ptOffset.y;
		//SetCaretPos(ptCaret.x, ptCaret.y);
		//CalcScrollBar();
		//Invalidate(FALSE);
		bHandled = TRUE;
		return 0;
	}
	else if (g_config.bEnableHScrollBar && TRUE == m_rcHScroll.PtInRect(originalPoint))//测试是否点中横向滚动条
	{
		if ((originalPoint.x>m_rcHScroll.left)&&(originalPoint.x<m_rcHThumb.left))
		{
			PageLeft();
		}
		else if ((originalPoint.x<m_rcHScroll.right)&&(originalPoint.x>m_rcHThumb.right))
		{
			PageRight();
		}
		/*int nAvailableLength = 125-25-2*2;
		if (originalPoint.x<(m_rcHScroll.left+2+(m_rcHThumb.Width()/2)))
		{
			originalPoint.x = m_rcHScroll.left+2+(m_rcHThumb.Width()/2);
		}
		else if (originalPoint.x>(m_rcHScroll.right-2-(m_rcHThumb.Width()/2)))
		{
			originalPoint.x = m_rcHScroll.right-2-(m_rcHThumb.Width()/2);
		}
		CPoint ptCaret;
		GetCaretPos(&ptCaret);
		ptCaret.x += m_ptOffset.x;
		m_ptOffset.x = ((originalPoint.x - m_rcHThumb.Width()/2 - (m_rcHScroll.left+5)))*m_nHRange/nAvailableLength;
		ptCaret.x -= m_ptOffset.x;
		SetCaretPos(ptCaret.x, ptCaret.y);
		CalcScrollBar();
		Invalidate(FALSE);*/
		bHandled = TRUE;
		return 0;
	}
	else
	{
		bool bHitted = false;
		bool bAltPressed = false;
		bool bShiftPressed = false;
		bool bControlPressed = false;
		//测试是否有按键
		if ((GetKeyState(VK_LMENU)<0||GetKeyState(VK_RMENU)<0))
		{
			bAltPressed = true;
		}
		else if ((GetKeyState(VK_LSHIFT)<0||GetKeyState(VK_RSHIFT)<0))
		{
			bShiftPressed = true;
		}
		else if ((GetKeyState(VK_LCONTROL)<0||GetKeyState(VK_RCONTROL)<0))
		{
			bControlPressed = true;
		}
		int index = 0;
		for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock, ++index)
		{
			if ((*ppTextBlock)->m_bShown)//查找所有可见的文字块
			{
				if (bAltPressed)
				{
					if ((*ppTextBlock)->HitTest(x, y) || (*ppTextBlock)->m_pTextHandle->HitTest(x, y))
					{
						m_nLastClicked = index;
						if (!(*ppTextBlock)->m_bSelected)
						{
							int nCurrLevel = (*ppTextBlock)->m_nLevel;
							(*ppTextBlock)->m_bSelected = true;
							++ppTextBlock;
							while (ppTextBlock != m_textBlockList.end() && (*ppTextBlock)->m_nLevel > nCurrLevel)
							{
								(*ppTextBlock)->m_bSelected = true;
								++ppTextBlock;
							}
						}
						else
						{
							int nCurrLevel = (*ppTextBlock)->m_nLevel;
							(*ppTextBlock)->m_bSelected = false;
							++ppTextBlock;
							while (ppTextBlock != m_textBlockList.end() && (*ppTextBlock)->m_nLevel > nCurrLevel)
							{
								(*ppTextBlock)->m_bSelected = false;
								++ppTextBlock;
							}
						}
						if (NULL != m_pTextBlockFocused)
						{
							//m_pTextBlockFocused->RevokeDragDrop();
							//m_pTextBlockFocused->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
							m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
							m_pTextBlockFocused = NULL;
						}
						bHitted = true;
						break;
					}
				}
				else if (bControlPressed)
				{
					if ((*ppTextBlock)->HitTest(x, y) || (*ppTextBlock)->m_pTextHandle->HitTest(x, y))
					{
						m_nLastClicked = index;
						if (TEXT_HANDLE_CLOSED != (*ppTextBlock)->m_pTextHandle->state)
						{
							if (!(*ppTextBlock)->m_bSelected)
							{
								(*ppTextBlock)->m_bSelected = true;
							}
							else
							{
								(*ppTextBlock)->m_bSelected = false;
							}
						}
						else
						{
							if (!(*ppTextBlock)->m_bSelected)
							{
								int nCurrLevel = (*ppTextBlock)->m_nLevel;
								(*ppTextBlock)->m_bSelected = true;
								++ppTextBlock;
								while (ppTextBlock != m_textBlockList.end() && (*ppTextBlock)->m_nLevel > nCurrLevel)
								{
									(*ppTextBlock)->m_bSelected = true;
									++ppTextBlock;
								}
							}
							else
							{
								int nCurrLevel = (*ppTextBlock)->m_nLevel;
								(*ppTextBlock)->m_bSelected = false;
								++ppTextBlock;
								while (ppTextBlock != m_textBlockList.end() && (*ppTextBlock)->m_nLevel > nCurrLevel)
								{
									(*ppTextBlock)->m_bSelected = false;
									++ppTextBlock;
								}
							}
						}
						if (NULL != m_pTextBlockFocused)
						{
							//m_pTextBlockFocused->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
							m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
							//m_pTextBlockFocused->RevokeDragDrop();
							m_pTextBlockFocused = NULL;
						}
						bHitted = true;
						break;
					}
				}
				else if (bShiftPressed)
				{
					if ((*ppTextBlock)->HitTest(x, y) || (*ppTextBlock)->m_pTextHandle->HitTest(x, y))
					{
						if (index == m_nLastClicked)
						{
							bHitted = true;
							break;
						}
						CleanAllSelected();
						int nBegin = -1;
						int nEnd = -1;
						if (index < m_nLastClicked)
						{
							nBegin = index;
							nEnd = m_nLastClicked;
						}
						else
						{
							nBegin = m_nLastClicked;
							nEnd = index;
						}
						while (nBegin != nEnd)
						{
							m_textBlockList[nBegin]->m_bSelected = true;
							++nBegin;
						}
						m_textBlockList[nEnd]->m_bSelected = true;
						ppTextBlock = m_textBlockList.begin()+nEnd;
						if (TEXT_HANDLE_CLOSED == (*ppTextBlock)->m_pTextHandle->state)
						{
							int nCurrLevel = (*ppTextBlock)->m_nLevel;
							(*ppTextBlock)->m_bSelected = true;
							++ppTextBlock;
							while (ppTextBlock != m_textBlockList.end() && (*ppTextBlock)->m_nLevel > nCurrLevel)
							{
								(*ppTextBlock)->m_bSelected = true;
								++ppTextBlock;
							}
						}
						if (NULL != m_pTextBlockFocused)
						{
							//m_pTextBlockFocused->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
							m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
							//m_pTextBlockFocused->RevokeDragDrop();
							m_pTextBlockFocused = NULL;
						}
						bHitted = true;
						break;
					}
				}
				else //left click without associated key 没有按键, 单纯点击
				{
					if ((*ppTextBlock)->HitTest(x, y)/* && */)
					{
						if (m_pTextBlockFocused != (*ppTextBlock))//单纯点击, 选中了某个文字块
						{
							CleanAllSelected();
							m_nLastClicked = index;
							if (NULL!=m_pTextBlockFocused)
							{
								m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS,(WPARAM)m_hWnd,0);
								//m_pTextBlockFocused->RevokeDragDrop();
								m_pTextBlockFocused->DetectFileBlock();
								CalcLayout();
								CalcScrollBar();
							}
							m_pTextBlockFocused = (*ppTextBlock);
							//m_pTextBlockFocused->RegisterDragDrop();
							(*ppTextBlock)->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
							bHandled = FALSE;
							bHitted = true;
						}
						else
						{
							//如果是文件文字块, 且是双击, 则打开链接的文件
							if (m_pTextBlockFocused->m_bFile && WM_LBUTTONDBLCLK == uMsg)
							{
								TCHAR szTempCache[MAX_PATH*3];
								if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
								{
									_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
								}
								else if (L'.' == m_pTextBlockFocused->m_szCache[0])
								{
									if (!m_fileDirectory.IsEmpty())
									{
										swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
										PathRemoveFileSpec(szTempCache);
										PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
									}
								}
								else
								{
									_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
								}
								CString fn(szTempCache);
								int dot = fn.ReverseFind(_T('.'));
								CString ext = fn;
								ext.Delete(0, dot+1);
								ext.MakeLower();
								if (ext == _T("boo"))
								{
									if(g_config.bBooOpenInSameInstance)
									{
										GetParent().SendMessage(WM_USER_MAINFRM_OPENFILE, (WPARAM)szTempCache);
									}
									else
									{
										ShellExecute(NULL, NULL, szTempCache, NULL, NULL, SW_SHOW);
									}
								}
								else
								{
									ShellExecute(NULL, NULL, szTempCache, NULL, NULL, SW_SHOW);
								}
								bHandled = TRUE;
							}
							else
							{
								m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
								bHandled = FALSE;
								bHitted = true;
							}
						}
					}
					//else if ((*ppTextBlock)->m_bShown && (*ppTextBlock)->m_pTextHandle->HitTest(x, y))
					else if ((*ppTextBlock)->m_pTextHandle->HitTest(x, y))
					{
						int index = ppTextBlock - m_textBlockList.begin();//找到当前点中的文字块的编号
						
						//int nCurrLevel = (*ppTextBlock)->m_nLevel;
						if (TEXT_HANDLE_EXPAND == (*ppTextBlock)->m_pTextHandle->state)
						{
							CloseBlocks(index);
							if(g_config.bEnterSHRINK_SELF)
							{
							    ShrinkPointTextBlock(*ppTextBlock);
						    }
						}
						else if (TEXT_HANDLE_CLOSED == (*ppTextBlock)->m_pTextHandle->state)
						{
							ExpandBlocks(index);
							if(g_config.bEnterSHRINK_SELF)
							{
							    NarrowPointTextBlock(*ppTextBlock);
							}
						}
						else if (TEXT_HANDLE_NULL == (*ppTextBlock)->m_pTextHandle->state)
						{
							if ((*ppTextBlock)->m_bFile)
							{
								if (NULL!=m_pTextBlockFocused)
								{
									m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS,(WPARAM)m_hWnd,0);
									//m_pTextBlockFocused->RevokeDragDrop();
									m_pTextBlockFocused->DetectFileBlock();
								}
								m_pTextBlockFocused = (*ppTextBlock);
								BOOL bHandled;
								OnEditFileBlock(0, 0, 0, bHandled);
							}
							else
							{
								SendMessage(WM_RBUTTONDOWN, wParam, oritinallParam);
							}
						}
						m_bDirty = true;
						//GetParent().SendMessageW(WM_NCPAINT);

						CalcLayout();
						CalcScrollBar();
						bHitted = true;
						bHandled = TRUE;
						break;
					}
					else
					{
					}
				}
			}
		}
		if (!bHitted)
		{
			CleanAllSelected();
		}
		Invalidate(FALSE);
	}
	GetParent().SetFocus();
	SetFocus();

	return 0;
}

//void CBooguNoteView::PopupToolTip(CBooguNoteText* pTB)
//{
//	// Create a tooltip.
//	if (NULL == m_hwndTT)
//	{
//		m_hwndTT = CreateWindowEx(WS_EX_TOPMOST,
//			TOOLTIPS_CLASS, NULL,
//			WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
//			CW_USEDEFAULT, CW_USEDEFAULT,
//			CW_USEDEFAULT, CW_USEDEFAULT,
//			m_hWnd, NULL, _Module.m_hInst,NULL);
//	}
//
//	::SetWindowPos(m_hwndTT, HWND_TOPMOST,
//		0, 0, 0, 0,
//		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
//
//	// Set up "tool" information.
//	// In this case, the "tool" is the entire parent window.
//	TOOLINFO ti = { 0 };
//	ti.cbSize = sizeof(TOOLINFO);
//	ti.uFlags = TTF_SUBCLASS;
//	ti.hwnd = m_hWnd;
//	ti.hinst = _Module.m_hInst;
//	ti.lpszText = pTB->m_szCache;//TEXT("This is your tooltip string.");;
//	ti.rect = *(pTB->GetClientRect());
//	if (pTB->m_bFile && pTB->m_pBitmap !=NULL)
//	{
//		ti.rect.right = ti.rect.left + pTB->m_pBitmap->GetWidth();
//		ti.rect.bottom = ti.rect.top + pTB->m_pBitmap->GetHeight();
//	}
//	ti.rect.left -= m_ptOffset.x;
//	ti.rect.right -= m_ptOffset.x;
//	ti.rect.top -= m_ptOffset.y;
//	ti.rect.bottom -= m_ptOffset.y;
//	//GetClientRect (&ti.rect);
//
//	// Associate the tooltip with the "tool" window.
//	SendMessage(m_hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
//}
//
//void CBooguNoteView::DeleteToolTip()
//{
//	if (NULL != m_hwndTT)
//	{
//		TOOLINFO ti = { 0 };
//		ti.cbSize = sizeof(TOOLINFO);
//		ti.uFlags = TTF_SUBCLASS;
//		ti.hwnd = m_hWnd;
//		ti.hinst = _Module.m_hInst;
//		SendMessage(m_hwndTT, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
//		::DestroyWindow(m_hwndTT);
//		m_hwndTT = NULL;
//	}
//}

void CBooguNoteView::CloseBlocks(int index)
{
	int nCurrLevel = m_textBlockList[index]->m_nLevel;
	if (TEXT_HANDLE_EXPAND == m_textBlockList[index]->m_pTextHandle->state)
	{
		m_textBlockList[index]->m_pTextHandle->state = TEXT_HANDLE_CLOSED;
		++index;
		int nSize = m_textBlockList.size();
		while (index<nSize && m_textBlockList[index]->m_nLevel > nCurrLevel)
		{
			if (m_textBlockList[index] == m_pTextBlockFocused)
			{
				//if (NULL!=m_pTextBlockFocused)
					//m_pTextBlockFocused->RevokeDragDrop();
				m_pTextBlockFocused = NULL;
				(m_textBlockList[index]->GetTextServices())->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
				//break;
			}
			if (m_textBlockList[index]->m_bSelected)
			{
				m_textBlockList[index]->m_bSelected = false;
				//if (NULL!=m_pTextBlockFocused)
				//	m_pTextBlockFocused->RevokeDragDrop();
				m_pTextBlockFocused = NULL;
			}
			m_textBlockList[index]->m_bShown = false;
			++index;
		}
	}
	CalcLayout();
	Invalidate();
}

void CBooguNoteView::ExpandBlocks(int index)
{
	int nCurrLevel = m_textBlockList[index]->m_nLevel;
	if (TEXT_HANDLE_CLOSED == m_textBlockList[index]->m_pTextHandle->state)
	{
		m_textBlockList[index]->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
		++index;
		int nSize = m_textBlockList.size();
		while (index<nSize && m_textBlockList[index]->m_nLevel > nCurrLevel)
		{
			if (TEXT_HANDLE_CLOSED == m_textBlockList[index]->m_pTextHandle->state)
			{
				int nCurrLevel2 = m_textBlockList[index]->m_nLevel;
				m_textBlockList[index]->m_bShown = true;
				++index;
				while (index<nSize && m_textBlockList[index]->m_nLevel > nCurrLevel2)
				{
					++index;
				}
			}
			else
			{
				m_textBlockList[index]->m_bShown = true;
				++index;
			}
		}
	}
	CalcLayout();
	Invalidate();
}

void CBooguNoteView::CalcLayout()
{
	
	m_nHRange = 0;
	m_nVRange = 0;
	int nBlockStart_Y = g_config.ptStart.y+g_config.nBlockMargin;
	//int nHandleWidth = 10;-PointsToLogical(g_config.fontSize);
	//int nLastIndent = -PointsToLogical(g_config.fontSize);
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	m_textBlockList[0]->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
	//int nTextHeight = -PointsToLogical(cf.yHeight/20);
	int nTextHeight = m_nSingleLineHeight;// -PointsToLogical(g_config.fontSize);
	//should be that, to keep bullet in center vertically
	if (nTextHeight%2 != 1)
	{
		nTextHeight -= 1;
	}
	int nTextHandleOffsetY = (nTextHeight-g_config.nHandleWidth)/2+g_config.hostBorder;
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		//determine
		if (((ppTextBlock+1) != m_textBlockList.end()) && ((*ppTextBlock)->m_nLevel < (*(ppTextBlock+1))->m_nLevel))
		{
			if ((*(ppTextBlock+1))->m_bShown)
			{
				(*ppTextBlock)->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
			}
			else
			{
				(*ppTextBlock)->m_pTextHandle->state = TEXT_HANDLE_CLOSED;
			}
		}
		else
		{
			(*ppTextBlock)->m_pTextHandle->state = TEXT_HANDLE_NULL;
		}
		(*ppTextBlock)->m_pTextHandle->x = g_config.ptStart.x + (*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder);
		
		(*ppTextBlock)->m_pTextHandle->y = nBlockStart_Y + nTextHandleOffsetY;//nBlockStart_Y+2+g_config.hostBorder;
		CRect* prc = NULL;
		prc = (CRect*)(*ppTextBlock)->GetClientRect();
		CRect rcImage;
		if ((*ppTextBlock)->m_bFile && (*ppTextBlock)->m_pBitmap != NULL)
		{
			rcImage.SetRect(prc->left, prc->top, prc->left + (*ppTextBlock)->m_pBitmap->GetWidth(), prc->top+(*ppTextBlock)->m_pBitmap->GetHeight());
			prc = &rcImage;
		}
		int nTextBlockIndent = 0;
		if (TEXT_ICON_PLAINTEXT!=(*ppTextBlock)->m_pTextIcon->state)
		{
			//if (prc->Height()>((*ppTextBlock)->m_pTextHandle->width/*actually height*/+2*g_config.nIconWidth))//if more than 1 line
			//{
			//	(*ppTextBlock)->m_pTextIcon->x = (*ppTextBlock)->m_pTextHandle->x + ((*ppTextBlock)->m_pTextHandle->width-g_config.nIconWidth)/2-1;
			//	(*ppTextBlock)->m_pTextIcon->y = (*ppTextBlock)->m_pTextHandle->y + 4 +(*ppTextBlock)->m_pTextHandle->width/*actually it's height*/;
			//	nTextBlockIndent = g_config.ptStart.x + ((*ppTextBlock)->m_nLevel+1)*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder + 3;
			//}
			//else//text only 1 line, so show icon and handle in the same line.
			{
				(*ppTextBlock)->m_pTextIcon->x = g_config.ptStart.x + ((*ppTextBlock)->m_nLevel+1)*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder + 1;
				(*ppTextBlock)->m_pTextIcon->y = (*ppTextBlock)->m_pTextHandle->y -3;
				nTextBlockIndent = (*ppTextBlock)->m_pTextIcon->x + (*ppTextBlock)->m_pTextIcon->width +1;
			}
		}
		else
		{
			nTextBlockIndent = g_config.ptStart.x + ((*ppTextBlock)->m_nLevel+1)*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder +1;
		}
		if ((*ppTextBlock)->m_bFile && (*ppTextBlock)->m_pBitmap == NULL )
		{
			nTextBlockIndent += FILE_ICON_WIDTH;
		}
		//if Image, switch back
		if ((*ppTextBlock)->m_bFile && (*ppTextBlock)->m_pBitmap != NULL)
		{
			prc = (CRect*)(*ppTextBlock)->GetClientRect();
		}
		prc->MoveToXY(nTextBlockIndent,nBlockStart_Y);
		(*ppTextBlock)->SetClientRect(prc, TRUE);
		//if Image, switch again
		if ((*ppTextBlock)->m_bFile && (*ppTextBlock)->m_pBitmap != NULL)
		{
			rcImage.SetRect(prc->left, prc->top, prc->left + (*ppTextBlock)->m_pBitmap->GetWidth(), prc->top+(*ppTextBlock)->m_pBitmap->GetHeight());
			prc = &rcImage;
		}
		if (prc->right >= m_nHRange)
			m_nHRange = prc->right;
		nBlockStart_Y += prc->Height()+g_config.nBlockMargin;
		(*ppTextBlock)->m_bShown = true;

		if (TEXT_HANDLE_CLOSED == (*ppTextBlock)->m_pTextHandle->state)
		{
			int nCurrLevel = (*ppTextBlock)->m_nLevel;
			++ppTextBlock;
			
			while (ppTextBlock != m_textBlockList.end() && (*ppTextBlock)->m_nLevel > nCurrLevel)
			{
				(*ppTextBlock)->m_bShown = false;
				++ppTextBlock;
			}
			--ppTextBlock;
		}
		//nBlockStart_Y =+ 4;
		//if ((*ppTextBlock)->m_bShown)
		//{
			//(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_REQUESTRESIZE, 0, 0, 0);
			
		//}
	}
	m_nHRange += g_config.nScrollbarWidth + 1;
	m_nVRange = nBlockStart_Y + 150;
	//m_nVRange = nBlockStart_Y+g_config.hostBorder;

	CRect rc;
	GetWindowRect(&rc);
	if (m_nVRange < m_ptOffset.y+rc.Height())
	{
		m_ptOffset.y = m_nVRange - rc.Height();
		if (m_ptOffset.y<0)
			m_ptOffset.y = 0;
	}

	if (m_nHRange < m_ptOffset.x+rc.Width())
	{
		m_ptOffset.x = m_nHRange - rc.Width();
		if (m_ptOffset.x<0)
			m_ptOffset.x = 0;
	}
}

LRESULT CBooguNoteView::OnLBottonDoubleClick(UINT uMsg, WPARAM wParam, LPARAM &lParam, BOOL& bHandled)
{
	LPARAM orglParam = lParam;
	OnLBottonDown(uMsg, wParam, lParam, bHandled);
	int x = GET_X_LPARAM(orglParam) +m_ptOffset.x;
	int y = GET_Y_LPARAM(orglParam) +m_ptOffset.y;
	lParam = MAKELPARAM((WORD)x, (WORD)y);
	if ((m_pTextBlockFocused!=NULL) && !m_pTextBlockFocused->HitTest(x, y))
	{
		bHandled = TRUE;
	}
	else
	{
		if ((m_pTextBlockFocused!=NULL) && m_pTextBlockFocused->m_bFile)
		{
			bHandled = TRUE;
		}
		else
		{
			bHandled = FALSE;
		}
		
	}
	return 0;
}

LRESULT CBooguNoteView::OnRBottonDoubleClick(UINT uMsg, WPARAM wParam, LPARAM &lParam, BOOL& bHandled)
{
	OnRBottonDown(uMsg, wParam, lParam, bHandled);
	//int x = GET_X_LPARAM(lParam) +m_ptOffset.x;
	//int y = GET_Y_LPARAM(lParam) +m_ptOffset.y;
	//lParam = MAKELPARAM((WORD)x, (WORD)y);
	//bHandled = FALSE;
	return 0;
}

LRESULT CBooguNoteView::OnRBottonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	GetParent().SendMessageW(WM_NCPAINT);
	bHandled = TRUE;
	int x = GET_X_LPARAM(lParam); 
	int y = GET_Y_LPARAM(lParam); 
	

	bool bHited = false;
	vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin();
	for(; ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
	    if (g_config.bEnterURLDetect)
	    {
	       (*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0); 
	    }
	    else
	    {
	       (*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, FALSE, 0, 0); 
	    }		
		
		if ((*ppTextBlock)->m_bShown && ((*ppTextBlock)->m_pTextHandle->HitTest(x+m_ptOffset.x, y+m_ptOffset.y) || ((*ppTextBlock)->HitTest(x+m_ptOffset.x, y+m_ptOffset.y) && (GetKeyState(VK_RCONTROL)<0||GetKeyState(VK_LCONTROL)<0))))
		{
			if (NULL != m_pTextBlockFocused)
			{
				//m_pTextBlockFocused->RevokeDragDrop();
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
				m_pTextBlockFocused = NULL;
			}
			m_pTextBlockFocused = (*ppTextBlock);
			bHited = true;
			
			
			
			
			if (TEXT_BLOCK_NARROW == (*ppTextBlock)->m_nExpandState)
			{
				if ((*ppTextBlock)->m_bFile)
				{
					if (NULL != (*ppTextBlock)->m_pBitmap)
					{
						TCHAR szTempCache[MAX_PATH*3];
						if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
						{
							_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
						}
						else if (L'.' == m_pTextBlockFocused->m_szCache[0])
						{
							if (!m_fileDirectory.IsEmpty())
							{
								swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
								PathRemoveFileSpec(szTempCache);
								PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
							}
						}
						else
						{
							_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
						}
						CString fileDirectory = szTempCache;
						int dot = fileDirectory.ReverseFind(_T('.'));
						CString ext = fileDirectory;
						ext.Delete(0, dot+1);
						ext.MakeLower();
						
						if (0==ext.Compare(_T("png"))||
							0==ext.Compare(_T("jpeg")) ||
							0==ext.Compare(_T("jpg")) ||
							0==ext.Compare(_T("gif")) ||
							0==ext.Compare(_T("tiff")) ||
							0==ext.Compare(_T("bmp")))
						{
							Image tempImage(szTempCache);
							//if (tempImage.GetLastStatus() != Ok)
							if (tempImage.GetLastStatus() != Ok)
							{
							}
							else
							{
								(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
								CSize sz(0,0);
								if (tempImage.GetHeight()>g_config.nBigThumbnailHeight)
								{
									sz.cy = g_config.nBigThumbnailHeight;
									sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
								}
								else
								{
									sz.cx = tempImage.GetWidth();
									sz.cy = tempImage.GetHeight();
								}
								delete (*ppTextBlock)->m_pBitmap;
								(*ppTextBlock)->m_pBitmap = new Bitmap(sz.cx, sz.cy);
								Graphics imageGraphics((*ppTextBlock)->m_pBitmap);
								imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
								//RECT* prc = (*ppTextBlock)->GetClientRect();
								//CRect rc(prc);
								//rc.right = rc.left + sz.cx;
								//rc.bottom = rc.top + sz.cy;
								//(*ppTextBlock)->SetClientRect(&rc);
							}
						}
					}
					(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_WIDE;
				}
				else
				{
					//RECT* prc = (*ppTextBlock)->GetClientRect();
					//CRect rc(prc);
					//rc.right = rc.left + g_config.wideWidth;
					//(*ppTextBlock)->SetClientRect(&rc);
					//(*ppTextBlock)->SetReadOnly(FALSE);
					if (g_config.bEnterWideTextBlock)
					{
						RECT* prc = (*ppTextBlock)->GetClientRect();
						CRect rc(prc);
						rc.right = rc.left + g_config.wideWidth;
						(*ppTextBlock)->SetClientRect(&rc);
						(*ppTextBlock)->SetReadOnly(FALSE);
						(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_WIDE;
					}
					else
					{
						ShrinkTextBlock((*ppTextBlock));
					}
				}
				
			}
			else if (TEXT_BLOCK_WIDE == (*ppTextBlock)->m_nExpandState)
			{
				if ((*ppTextBlock)->m_bFile)
				{
					if (NULL != (*ppTextBlock)->m_pBitmap)
					{
						TCHAR szTempCache[MAX_PATH*3];
						if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
						{
							_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
						}
						else if (L'.' == m_pTextBlockFocused->m_szCache[0])
						{
							if (!m_fileDirectory.IsEmpty())
							{
								swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
								PathRemoveFileSpec(szTempCache);
								PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
							}
						}
						else
						{
							_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
						}
						CString fileDirectory = szTempCache;
						int dot = fileDirectory.ReverseFind(_T('.'));
						CString ext = fileDirectory;
						ext.Delete(0, dot+1);
						ext.MakeLower();
						
						if (0==ext.Compare(_T("png"))||
							0==ext.Compare(_T("jpeg")) ||
							0==ext.Compare(_T("jpg")) ||
							0==ext.Compare(_T("gif")) ||
							0==ext.Compare(_T("tiff")) ||
							0==ext.Compare(_T("bmp")))
						{
							Image tempImage(szTempCache);
							//if (tempImage.GetLastStatus() != Ok)
							if (tempImage.GetLastStatus() != Ok)
							{
							}
							else
							{
								(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
								CSize sz(0,0);
								if (tempImage.GetHeight()>g_config.nSmallThumbnailHeight)
								{
									sz.cy = g_config.nSmallThumbnailHeight;
									sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
								}
								else
								{
									sz.cx = tempImage.GetWidth();
									sz.cy = tempImage.GetHeight();
								}
								delete (*ppTextBlock)->m_pBitmap;
								(*ppTextBlock)->m_pBitmap = new Bitmap(sz.cx, sz.cy);
								Graphics imageGraphics((*ppTextBlock)->m_pBitmap);
								imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
							}
						}
					}
					else
					{
						RECT* prc = (*ppTextBlock)->GetClientRect();
						CRect rc(prc);
						rc.right = rc.left + g_config.narrowWidth-((*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
						
						(*ppTextBlock)->SetClientRect(&rc);
					}
					(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
				}
				else
				{
					ShrinkTextBlock(*ppTextBlock);
					//set mouse pointer position.
					if (!(*ppTextBlock)->m_pTextHandle->HitTest(x+m_ptOffset.x, y+m_ptOffset.y))
					{
						RECT* _rc = (*ppTextBlock)->GetClientRect();
						int nY = _rc->top - m_ptOffset.y;
						bool bMoveView = false;
						if (nY<0)
						{
							nY = 0;
							bMoveView = true;
						}
						CPoint pt;
						GetCursorPos(&pt);
						int x = pt.x;
						ScreenToClient(&pt);
						pt.y = nY + (_rc->bottom - _rc->top)/2;
						//DeviceToClient(pt);
						ClientToScreen(&pt);
						SetCursorPos(x, pt.y);
						if (bMoveView)
						{
							m_ptOffset.y = _rc->top - g_config.hostBorder;
						}
					}
					//(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0);
					//(*ppTextBlock)->SetReadOnly(TRUE);
				}
			}
			else if (TEXT_BLOCK_SHRINK == (*ppTextBlock)->m_nExpandState)
			{
				RECT* prc = (*ppTextBlock)->GetClientRect();
				CRect rc(prc);
				rc.right = rc.left + g_config.narrowWidth-((*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);

				(*ppTextBlock)->SetClientRect(&rc);
				(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
				CHARFORMAT cf;
				cf.cbSize = sizeof(CHARFORMAT);
				(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				cf.dwEffects &= ~CFE_UNDERLINE;
				(*ppTextBlock)->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
				(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(*ppTextBlock)->m_szCache, 0);
				(*ppTextBlock)->SetReadOnly(FALSE);
			}
			/*if (NULL != m_pTextBlockFocused)
			{
				m_pTextBlockFocused->RevokeDragDrop();
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
				m_pTextBlockFocused = NULL;
			}
			m_pTextBlockFocused = (*ppTextBlock);*/
			//(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0);

			m_bDirty = true;
			GetParent().SendMessageW(WM_NCPAINT);
			break;
		}
	}
	
	if (bHited)
	{
		m_bCancelAutoAdjust = true;
		m_bCancelRButtonUp = true;
		CalcLayout();
		CalcScrollBar();
		Invalidate(FALSE); //flush size into text block and repaint
	}
	else
	{
		m_bRButtonDown = true;
		SetCapture();
		m_ptDragStart.x = x; 
		m_ptDragStart.y = y; 
		//ClientToScreen(&m_ptDragStart);
	}
	SetCursor(g_util.hCsrSizeAll);
	//m_bCancelRButtonUp = false;
	return 0;
}

LRESULT CBooguNoteView::OnStateNarrow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	StateNarrow();
	return 0;
}

void CBooguNoteView::StateNarrow()
{
	vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin();
	for(; ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock)->m_bShown && (TEXT_BLOCK_NARROW != (*ppTextBlock)->m_nExpandState) &&((*ppTextBlock)->m_bSelected || (*ppTextBlock) == m_pTextBlockFocused))
		{
			if ((*ppTextBlock)->m_bFile && (*ppTextBlock)->m_pBitmap != NULL)
			{
				TCHAR szTempCache[MAX_PATH*3];
				if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
				{
					_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
				}
				else if (L'.' == m_pTextBlockFocused->m_szCache[0])
				{
					if (!m_fileDirectory.IsEmpty())
					{
						swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
						PathRemoveFileSpec(szTempCache);
						PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
					}
				}
				else
				{
					_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
				}
				CString fileDirectory = szTempCache;
				int dot = fileDirectory.ReverseFind(_T('.'));
				CString ext = fileDirectory;
				ext.Delete(0, dot+1);
				ext.MakeLower();
				
				if (0==ext.Compare(_T("png"))||
					0==ext.Compare(_T("jpeg")) ||
					0==ext.Compare(_T("jpg")) ||
					0==ext.Compare(_T("gif")) ||
					0==ext.Compare(_T("tiff")) ||
					0==ext.Compare(_T("bmp")))
				{
					Image tempImage(szTempCache);
					//if (tempImage.GetLastStatus() != Ok)
					if (tempImage.GetLastStatus() != Ok)
					{
					}
					else
					{
						(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
						CSize sz(0,0);
						if (tempImage.GetHeight()>g_config.nSmallThumbnailHeight)
						{
							sz.cy = g_config.nSmallThumbnailHeight;
							sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
						}
						else
						{
							sz.cx = tempImage.GetWidth();
							sz.cy = tempImage.GetHeight();
						}
						delete (*ppTextBlock)->m_pBitmap;
						(*ppTextBlock)->m_pBitmap = new Bitmap(sz.cx, sz.cy);
						Graphics imageGraphics((*ppTextBlock)->m_pBitmap);
						imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
					}
				}
			}
			else
			{
				if (TEXT_BLOCK_SHRINK == (*ppTextBlock)->m_nExpandState)
				{
					RECT* prc = (*ppTextBlock)->GetClientRect();
					CRect rc(prc);
				    rc.right = rc.left + g_config.narrowWidth-((*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);                   
                                                                                                                                        					

                    (*ppTextBlock)->SetClientRect(&rc);
                    //(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
                    CHARFORMAT cf;
                	cf.cbSize = sizeof(CHARFORMAT);
					(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwEffects &= ~CFE_UNDERLINE;
					(*ppTextBlock)->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
					(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(*ppTextBlock)->m_szCache, 0);
					(*ppTextBlock)->SetReadOnly(FALSE);
				}
				else
				{
					RECT* prc = (*ppTextBlock)->GetClientRect();
					CRect rc(prc);
					rc.right = rc.left + g_config.narrowWidth-((*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);                                                                                                                                        

					(*ppTextBlock)->SetClientRect(&rc);
					//(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
					CHARFORMAT cf;
					cf.cbSize = sizeof(CHARFORMAT);
					/*(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwEffects &= ~CFE_UNDERLINE;
					(*ppTextBlock)->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);*/
					
					//(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(*ppTextBlock)->m_szCache, 0);
					(*ppTextBlock)->SetReadOnly(FALSE);
					/*(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0);*/
				}
			}
			(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
		}
	}
	m_bCancelAutoAdjust = true;
	CalcLayout();
	CalcScrollBar();
	Invalidate(FALSE); //flush size into text block and repaint
}

LRESULT CBooguNoteView::OnStateWide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	StateWide();
	return 0;
}

void CBooguNoteView::StateWide()
{
	vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin();
	for(; ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock)->m_bShown  && (TEXT_BLOCK_WIDE != (*ppTextBlock)->m_nExpandState) && ((*ppTextBlock)->m_bSelected || (*ppTextBlock) == m_pTextBlockFocused))
		{
			if ((*ppTextBlock)->m_bFile && (*ppTextBlock)->m_pBitmap != NULL)
			{
				TCHAR szTempCache[MAX_PATH*3];
				if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
				{
					_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
				}
				else if (L'.' == m_pTextBlockFocused->m_szCache[0])
				{
					if (!m_fileDirectory.IsEmpty())
					{
						swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
						PathRemoveFileSpec(szTempCache);
						PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
					}
				}
				else
				{
					_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
				}
				CString fileDirectory = szTempCache;
				int dot = fileDirectory.ReverseFind(_T('.'));
				CString ext = fileDirectory;
				ext.Delete(0, dot+1);
				ext.MakeLower();
				
				if (0==ext.Compare(_T("png"))||
					0==ext.Compare(_T("jpeg")) ||
					0==ext.Compare(_T("jpg")) ||
					0==ext.Compare(_T("gif")) ||
					0==ext.Compare(_T("tiff")) ||
					0==ext.Compare(_T("bmp")))
				{
					Image tempImage(szTempCache);
					//if (tempImage.GetLastStatus() != Ok)
					if (tempImage.GetLastStatus() != Ok)
					{
					}
					else
					{
						(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
						CSize sz(0,0);
						if (tempImage.GetHeight()>g_config.nBigThumbnailHeight)
						{
							sz.cy = g_config.nBigThumbnailHeight;
							sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
						}
						else
						{
							sz.cx = tempImage.GetWidth();
							sz.cy = tempImage.GetHeight();
						}
						delete (*ppTextBlock)->m_pBitmap;
						(*ppTextBlock)->m_pBitmap = new Bitmap(sz.cx, sz.cy);
						Graphics imageGraphics((*ppTextBlock)->m_pBitmap);
						imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
					}
				}
			}
			else
			{
				if (TEXT_BLOCK_SHRINK == (*ppTextBlock)->m_nExpandState)
				{
					RECT* prc = (*ppTextBlock)->GetClientRect();
					CRect rc(prc);
					rc.right = rc.left + g_config.wideWidth;
					(*ppTextBlock)->SetClientRect(&rc);
					//(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_WIDE;
					CHARFORMAT cf;
					cf.cbSize = sizeof(CHARFORMAT);
					(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwEffects &= ~CFE_UNDERLINE;
					(*ppTextBlock)->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
					(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(*ppTextBlock)->m_szCache, 0);
					(*ppTextBlock)->SetReadOnly(FALSE);
				}
				else
				{
					RECT* prc = (*ppTextBlock)->GetClientRect();
					CRect rc(prc);
					rc.right = rc.left + g_config.wideWidth;
					(*ppTextBlock)->SetClientRect(&rc);
					//(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_WIDE;
					CHARFORMAT cf;
					cf.cbSize = sizeof(CHARFORMAT);
					/*(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwEffects &= ~CFE_UNDERLINE;
					(*ppTextBlock)->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);*/
					
					//(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(*ppTextBlock)->m_szCache, 0);
					(*ppTextBlock)->SetReadOnly(FALSE);
					/*(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0);*/
				}
			}
			(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_WIDE;
		}
	}
	m_bCancelAutoAdjust = true;
	CalcLayout();
	CalcScrollBar();
	Invalidate(FALSE); //flush size into text block and repaint
}
LRESULT CBooguNoteView::OnStateShrink(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	StateShrink();
	return 0;
}

void CBooguNoteView::StateShrink()
{
	vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin();
	for(; ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock)->m_bShown  && (TEXT_BLOCK_SHRINK != (*ppTextBlock)->m_nExpandState) && ((*ppTextBlock)->m_bSelected || (*ppTextBlock) == m_pTextBlockFocused))
		{
			ShrinkTextBlock(*ppTextBlock);
		}
	}
	m_bCancelAutoAdjust = true;
	CalcLayout();
	CalcScrollBar();
	Invalidate(FALSE); //flush size into text block and repaint
}

LRESULT CBooguNoteView::OnRBottonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	if (m_bCancelRButtonUp)
	{
		m_bCancelRButtonUp = false;
	}
	else
	{
		
		CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 

		// Convert client coordinates to screen coordinates 
		
		int x = pt.x;
		int y = pt.y;
		bool bCleanOtherSelection = true;
		for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
		{
			if ((*ppTextBlock)->m_bShown && (*ppTextBlock)->HitTest(x+m_ptOffset.x, y+m_ptOffset.y))
			{
				if ((*ppTextBlock)->m_bSelected)
					bCleanOtherSelection = false;
				else
				{
					//(*ppTextBlock)->m_bSelected = true;
					if (NULL!=m_pTextBlockFocused)
					{
						m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
						//m_pTextBlockFocused->RevokeDragDrop();
					}
					m_pTextBlockFocused = (*ppTextBlock);
					//m_pTextBlockFocused->RegisterDragDrop();
					m_nLastClicked = ppTextBlock - m_textBlockList.begin();
					(*ppTextBlock)->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
					Invalidate();
				}
				break;
			}
		}
		if (bCleanOtherSelection)
		{
			for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
			{
				if ((*ppTextBlock)->m_bShown)
				{
					if (!(*ppTextBlock)->HitTest(x+m_ptOffset.x, y+m_ptOffset.y))
					{
						(*ppTextBlock)->m_bSelected = false;
					}
				}
			}
		}
		if ((m_menuEdit.m_hMenu == NULL)&&(m_bRButtonDown))
		{
			ClientToScreen(&pt);
			PopupMenu(pt);
		}
	}
	if (GetCapture() == m_hWnd)
	{
		ReleaseCapture();
	}
	if (m_bRButtonDown)
	{
		m_bRButtonDown = false;
	}
	return 0;
}

void CBooguNoteView::PopupMenu(const CPoint& pt)
{
	// Load the menu resource 
	if (!m_menuEdit.LoadMenu(IDR_CONTEXTMENU))
	{
		ATLTRACE(_T("Menu resource was not loaded successfully!\n"));
	}

	// TrackPopupMenu cannot display the menu bar so get 
	// a handle to the first shortcut menu. 
	CMenuHandle menuPopup = m_menuEdit.GetSubMenu(0);
	
	if (NULL!=m_pTextBlockFocused && m_pTextBlockFocused->m_bFile)
	{
		CMenuItemInfo mi;
		mi.fMask = MIIM_TYPE;
		mi.fType = MFT_SEPARATOR;
		menuPopup.InsertMenuItemW(0, TRUE, &mi);

		//mi.fMask = MIIM_STRING|MIIM_ID|MIIM_STATE;
		//mi.fState = MFS_ENABLED;
		//mi.wID = ID_VIEW_MOVE_FILE;
		//mi.dwTypeData = _T("Move File");
		//mi.cch = lstrlen(mi.dwTypeData);
		//menuPopup.InsertMenuItemW(0, TRUE, &mi);

		mi.fMask = MIIM_STRING|MIIM_ID|MIIM_STATE;
		mi.fState = MFS_ENABLED;
		mi.wID = ID_VIEW_LOCATE_FILE;
		mi.dwTypeData = _T("打开所在文件夹");
		mi.cch = lstrlen(mi.dwTypeData);
		menuPopup.InsertMenuItemW(0, TRUE, &mi);

		mi.fMask = MIIM_STRING|MIIM_ID|MIIM_STATE;
		mi.fState = MFS_DEFAULT|MFS_ENABLED;
		mi.wID = ID_VIEW_EDIT_FILE_BLOCK;
		mi.dwTypeData = _T("编辑快捷方式");
		mi.cch = lstrlen(mi.dwTypeData);
		menuPopup.InsertMenuItemW(0, TRUE, &mi);

		menuPopup.DeleteMenu(ID_STATE_SHRINK, MF_BYCOMMAND);
	}
	if (_T("") == m_fileDirectory)
	{
		menuPopup.DeleteMenu(ID_POPUPMENU_FIND, MF_BYCOMMAND);
	}
	UpdateMenuKeys(menuPopup.m_hMenu);

	if (menuPopup.m_hMenu == NULL)
	{
		ATLTRACE(_T("Submenu was not retrieved successfully!\n"));
	}

	// Display the shortcut menu
	if (!menuPopup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON,
		(int)pt.x, (int)pt.y, m_hWnd))
	{
		ATLTRACE(_T("Context menu was not displayed successfully!\n"));
	}

	// Destroy the menu and free any memory that the menu occupies
	menuPopup.DestroyMenu();
	m_menuEdit.DestroyMenu();
	m_menuEdit.m_hMenu = NULL;
}

LRESULT CBooguNoteView::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//if (NULL != m_pTextBlockFocused)
	//{
	//	m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, (WPARAM)m_hWnd, 0);
	//}
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetFocus();
	SetActiveWindow();
	if (NULL != m_pTextBlockFocused)
	{
		CPoint pt;
		GetCaretPos(&pt);
		LPARAM l = MAKELPARAM(pt.x, pt.y);
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM &lParam, BOOL& bHandled)
{
	//CPoint originalPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	if (true == m_VThumbDraged)
	{
		int nAvailableLength = g_config.nVScrollbarLength-2*2;
		int nThumbHeight = rcWindow.Height()*nAvailableLength/m_nVRange;
		int nMoveRange = nAvailableLength - nThumbHeight;
		int nOriginalY = m_nOriginalOffsetY*nMoveRange/(m_nVRange - rcWindow.Height());
		

		//ScreenToClient(&m_ptDragStart);
		//int deltaX = GET_X_LPARAM(lParam)-m_ptDragStart.x;
		int deltaY = GET_Y_LPARAM(lParam)-m_ptDragStart.y;
		if ((deltaY>0)&&(nOriginalY+deltaY > nMoveRange))
		{
			deltaY = nMoveRange - nOriginalY;
		}
		else if ((deltaY<0)&&(nOriginalY< -deltaY))
		{
			deltaY = -nOriginalY;
			//if (deltaY>0)
			//{
			//	deltaY=0;
			//}
		}
		

		CPoint ptCaret;
		GetCaretPos(&ptCaret);
		ptCaret.y += m_ptOffset.y;
		m_ptOffset.y = (deltaY + nOriginalY)*(m_nVRange - rcWindow.Height())/nMoveRange;
		ptCaret.y -= m_ptOffset.y;
		SetCaretPos(ptCaret.x, ptCaret.y);
		CalcScrollBar();
		Invalidate(FALSE);
		bHandled = TRUE;
		return 0;
	}
	else if (true == m_HThumbDraged)
	{
		int nAvailableLength = g_config.nHScrollbarLength-2*2;
		int nThumbWidth = rcWindow.Width()*nAvailableLength/m_nHRange;
		int nMoveRange = nAvailableLength - nThumbWidth;
		int nOriginalX = m_nOriginalOffsetX*nMoveRange/(m_nHRange - rcWindow.Width());
		
		//ScreenToClient(&m_ptDragStart);
		int deltaX = GET_X_LPARAM(lParam)-m_ptDragStart.x;
		//int deltaY = GET_Y_LPARAM(lParam)-m_ptDragStart.y;
		if ((deltaX>0)&&(nOriginalX+deltaX > nMoveRange))
		{
			deltaX = nMoveRange - nOriginalX;
		}
		else if ((deltaX<0)&&(nOriginalX< -deltaX))
		{
			deltaX = -nOriginalX;
			//if (deltaY>0)
			//{
			//	deltaY=0;
			//}
		}
		CPoint ptCaret;
		GetCaretPos(&ptCaret);
		ptCaret.x += m_ptOffset.x;
		m_ptOffset.x = (deltaX + nOriginalX)*(m_nHRange - rcWindow.Width())/nMoveRange;
		ptCaret.x -= m_ptOffset.x;
		SetCaretPos(ptCaret.x, ptCaret.y);
		CalcScrollBar();
		Invalidate(FALSE);
		bHandled = TRUE;
		return 0;
	}
	else if (m_bRButtonDown)
	{
		//ScreenToClient(&m_ptDragStart);
		int deltaX = GET_X_LPARAM(lParam)-m_ptDragStart.x;
		int deltaY = GET_Y_LPARAM(lParam)-m_ptDragStart.y;
		//bound restrict
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		bool bCancelMove = false;
		if ((deltaX>0)&&(m_ptOffset.x - deltaX)<0)
		{
			m_ptOffset.x = 0;
			deltaX = 0;
		}
		else if ((deltaX<0)&&((m_ptOffset.x + rcWindow.Width() - deltaX)>m_nHRange))
		{
			if (m_nHRange>rcWindow.Width())
				m_ptOffset.x = m_nHRange-rcWindow.Width();
			deltaX = 0;
			//if (deltaX>0)
			//{
			//	bCancelMove = true;
			//}
		}
		else
		{
			m_ptOffset.x -= deltaX;
		}
		if ((deltaY>0)&&((m_ptOffset.y - deltaY)<0))
		{
			m_ptOffset.y = 0;
			deltaY = 0;
		}
		else if ((deltaY<0)&&((m_ptOffset.y + rcWindow.Height() - deltaY)>m_nVRange))
		{
			if ((m_nVRange>rcWindow.Height()))
				m_ptOffset.y = m_nVRange-rcWindow.Height();
			deltaY = 0;
			//if (deltaY>0)
			//{
			//	bCancelMove = true;
			//}
		}
		else
		{
			m_ptOffset.y -= deltaY;
		}
		//ScrollWindowEx(deltaX, deltaY, NULL, NULL, NULL, NULL, SW_ERASE);
		m_ptDragStart.x = GET_X_LPARAM(lParam);
		m_ptDragStart.y = GET_Y_LPARAM(lParam);
		//ClientToScreen(&m_ptDragStart);

		//calculate scrollbar and thumb size and position.
		//CalcLayout();
		CalcScrollBar();
		
		Invalidate(FALSE);

		//Move Caret
		CPoint ptCaret;
		GetCaretPos(&ptCaret);
		ptCaret.x += deltaX;
		ptCaret.y += deltaY;
		SetCaretPos(ptCaret.x, ptCaret.y);

		if (abs(deltaX)>0 || abs(deltaY)>0)
			m_bCancelRButtonUp = true;
	}
	else
	{
		int x = GET_X_LPARAM(lParam) +m_ptOffset.x;
		int y = GET_Y_LPARAM(lParam) +m_ptOffset.y;
		lParam = MAKELPARAM((WORD)x, (WORD)y);

		//bool bArrowCursor = true;
		for(vector<CBooguNoteText*>::iterator pWidget = m_textBlockList.begin(); pWidget != m_textBlockList.end(); ++pWidget)
		{
			CBooguNoteText* pTextBlock = (*pWidget);
			if (pTextBlock->m_bShown)
			{
				if (pTextBlock->HitTest(x, y))
				{
					if (pTextBlock->m_bFile)
					{
						::SetCursor(g_util.hCsrHand);
						//PopupToolTip(pTextBlock);
					}
					else
					{
						//DeleteToolTip();
					}
					break;
				}
				else if (pTextBlock->m_pTextHandle->HitTest(x, y))
				{
					Invalidate(FALSE);
				}
				else
				{
					//if (pTextBlock->m_bShown && pTextBlock->m_bHovering)
					//{
					//	bRedraw = true;
					//}
					//pTextBlock->m_bHovering = false;
				}
				/*if (pTextBlock->m_pTextHandle->HitTest(x, y))
				{
					bRedraw = true;
				}*/
			}
		}
		//if (bRedraw)
		//{
			//Invalidate(FALSE);
		//}
	}
	GetParent().SendMessage(WM_MOUSEMOVE);
	//TRACKMOUSEEVENT tme;
	//tme.cbSize = sizeof TRACKMOUSEEVENT;
	//tme.dwFlags = TME_HOVER|TME_LEAVE;
	//tme.dwHoverTime = 1;
	//tme.hwndTrack = m_hWnd;
	//TrackMouseEvent(&tme);
	bHandled = FALSE;
	return 0;
}

LRESULT CBooguNoteView::OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	if (MK_CONTROL != fwKeys)
	{
		if (m_nVRange<=rcWindow.Height())
		{
			fwKeys = MK_CONTROL;
		}
		else
		{
			CPoint ptCaret;
			GetCaretPos(&ptCaret);
			ptCaret.y += m_ptOffset.y;
		
			m_ptOffset.y -= zDelta;
			if (m_ptOffset.y>(m_nVRange-rcWindow.Height()))
				m_ptOffset.y=m_nVRange-rcWindow.Height();
			if (m_ptOffset.y<0)
				m_ptOffset.y=0;

			ptCaret.y -= m_ptOffset.y;
			SetCaretPos(ptCaret.x, ptCaret.y);
		}
	}
	if (MK_CONTROL == fwKeys)
	{
		CPoint ptCaret;
		GetCaretPos(&ptCaret);
		ptCaret.x += m_ptOffset.x;
		if (zDelta>0)
			m_ptOffset.x -= 5;
		else
			m_ptOffset.x += 5;
		if (m_ptOffset.x>(m_nHRange-rcWindow.Width()))
			m_ptOffset.x=m_nHRange-rcWindow.Width();
		if (m_ptOffset.x<0)
			m_ptOffset.x=0;
		ptCaret.x -= m_ptOffset.x;
		SetCaretPos(ptCaret.x, ptCaret.y);
	}

	CalcScrollBar();

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	DWORD lparam = MAKELONG(pt.x, pt.y);
	SendMessage(WM_MOUSEMOVE,0,(LPARAM)lparam);

	Invalidate(FALSE);
	return 0;
}

LRESULT CBooguNoteView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM &lParam, BOOL& bHandled)
{
	CRect rcWindow;
	GetParent().GetClientRect(&rcWindow);
	//pParentWindow->GetClientRect(&rcWindow);
	//GetWindowRect(&rcWindow);
	if ((m_ptOffset.x>0)&&(m_ptOffset.x+rcWindow.Width()>m_nHRange))
	{
		m_ptOffset.x = m_nHRange - rcWindow.Width();
		if (m_ptOffset.x<0)
			m_ptOffset.x = 0;
	}
	if ((m_ptOffset.y>0)&&(m_ptOffset.y+rcWindow.Height()>m_nVRange))
	{
		m_ptOffset.y = m_nVRange - rcWindow.Height();
		if (m_ptOffset.y<0)
			m_ptOffset.y = 0;
	}
	if (g_config.bAutoWidth)
	{	
	    //g_config.narrowWidth = (rcWindow.Width()-(g_config.ptStart.x + (-PointsToLogical(g_config.fontSize)))) - g_config.nScrollbarWidth -1;
	    g_config.narrowWidth = rcWindow.Width()-(g_config.ptStart.x + g_config.nHandleWidth+2+1+ g_config.nScrollbarWidth+1+16+2);//16是图标的大小对应 (*ppTextBlock)->m_pTextIcon->width
	}
	if (g_config.bAutoScrollBarLength)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		g_config.nVScrollbarLength = rcClient.Height()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth)-0;
		g_config.nHScrollbarLength = rcClient.Width()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth)-0;
	}
	//if (g_config.narrowWidth > GetSystemMetrics(SM_CXFULLSCREEN)*3/4)
	//	g_config.narrowWidth = GetSystemMetrics(SM_CXFULLSCREEN)*3/4;
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if (NULL == (*ppTextBlock)->m_pBitmap)
		{
			RECT* prc = (*ppTextBlock)->GetClientRect();
			CRect rc(prc);
			if (TEXT_BLOCK_NARROW == (*ppTextBlock)->m_nExpandState)
			{
				rc.right = rc.left + g_config.narrowWidth-((*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
			}
			else if (TEXT_BLOCK_WIDE == (*ppTextBlock)->m_nExpandState)
			{
				rc.right = rc.left + g_config.wideWidth;
			}
			(*ppTextBlock)->SetClientRect(&rc);
		}
	}
	CalcLayout();
	CalcScrollBar();
	bHandled = TRUE;
	return 1;
}

LRESULT CBooguNoteView::OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	for(vector<CBooguNoteText*>::iterator pWidget = m_textBlockList.begin(); pWidget != m_textBlockList.end(); ++pWidget)
	{
		CBooguNoteText* pTextBlock = (*pWidget);
		if (pTextBlock->m_bShown && pTextBlock->m_bHovering)
		{
			pTextBlock->m_bHovering = false;
			Invalidate(FALSE);
			break;
		}
	}
	
	return 0;
}


void CBooguNoteView::CalcScrollBar()
{
	if (g_config.bAutoScrollBarLength)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		g_config.nVScrollbarLength = rcClient.Height()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth)-0;
		g_config.nHScrollbarLength = rcClient.Width()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth)-0;
	}
	CRect rcWindow;
	GetClientRect(&rcWindow);
	//for Vertical Scrollbar
	if (g_config.bEnableHScrollBar)
	{
		m_rcVScroll.SetRect(rcWindow.Width()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth), rcWindow.Height()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth+g_config.nVScrollbarLength), rcWindow.Width()-g_config.nScrollbarMargin, rcWindow.Height()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth));
	}
	else
	{
		m_rcVScroll.SetRect(rcWindow.Width()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth), rcWindow.Height()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth+g_config.nVScrollbarLength), rcWindow.Width()-g_config.nScrollbarMargin, rcWindow.Height()-(g_config.nScrollbarMargin/*+g_config.nScrollbarWidth*/));
	}
	int nAvailableLength = m_rcVScroll.Height()-2*2;//g_config.nVScrollbarLength-2*2;
	int nVThumbLength = (nAvailableLength*rcWindow.Height()/m_nVRange);
	if (nVThumbLength<8)
		nVThumbLength = 8;
	else if (nVThumbLength>nAvailableLength)
		nVThumbLength=nAvailableLength;
	m_rcVThumb = m_rcVScroll;
	m_rcVThumb.left += 2;
	m_rcVThumb.right -= 2;
	if(m_nVRange == rcWindow.Height())
		m_rcVThumb.top += 2;
	else
		m_rcVThumb.top += (((nAvailableLength-nVThumbLength)*m_ptOffset.y/(m_nVRange-rcWindow.Height())) + 2);
	m_rcVThumb.bottom = m_rcVThumb.top+nVThumbLength;
	if (m_rcVThumb.bottom > (m_rcVScroll.bottom-2))
	{
		m_rcVThumb.bottom = m_rcVScroll.bottom-2;
		m_rcVThumb.top = m_rcVThumb.bottom - nVThumbLength;
	}

	//for Horizontal Scrollbar
	if (m_nVRange<=rcWindow.Height())
	{
		m_rcHScroll.SetRect(rcWindow.Width()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth+g_config.nHScrollbarLength), rcWindow.Height()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth), rcWindow.Width()-(g_config.nScrollbarMargin/*+g_config.nScrollbarWidth*/), rcWindow.Height()-g_config.nScrollbarMargin);
	}
	else
	{
		m_rcHScroll.SetRect(rcWindow.Width()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth+g_config.nHScrollbarLength), rcWindow.Height()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth), rcWindow.Width()-(g_config.nScrollbarMargin+g_config.nScrollbarWidth), rcWindow.Height()-g_config.nScrollbarMargin);
	}
	nAvailableLength = m_rcHScroll.Width()-2*2;//g_config.nHScrollbarLength-2*2;
	int nHThumbLength = (nAvailableLength*rcWindow.Width()/m_nHRange);
	if (nHThumbLength<8)
		nHThumbLength = 8;
	else if (nHThumbLength>nAvailableLength)
		nHThumbLength=nAvailableLength;
	m_rcHThumb = m_rcHScroll;
	if ((m_nHRange == rcWindow.Width()))
		m_rcHThumb.left+=2;
	else
		m_rcHThumb.left += (((nAvailableLength-nHThumbLength)*m_ptOffset.x/(m_nHRange-rcWindow.Width())) + 2);
	m_rcHThumb.right = m_rcHThumb.left+nHThumbLength;
	if (m_rcHThumb.right > (m_rcHScroll.right-2))
	{
		m_rcHThumb.right = m_rcHScroll.right-2;
		m_rcHThumb.left = m_rcHThumb.right - nHThumbLength;
	}
	m_rcHThumb.top += 2;
	m_rcHThumb.bottom -= 2;
}

LRESULT CBooguNoteView::OnClean(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CleanAllSelected();
	if (NULL!=m_pTextBlockFocused)
	{
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS,(WPARAM)m_hWnd,0);
		//m_pTextBlockFocused->RevokeDragDrop();
		m_pTextBlockFocused->DetectFileBlock();
		m_pTextBlockFocused = NULL;
		CalcLayout();
		CalcScrollBar();
	}
	CleanAllClipped();
	//Invalidate();
	return 0;
}

int CBooguNoteView::SaveAs()
{
    char tmpbuf[128];
    memset(tmpbuf,'\0',sizeof(tmpbuf));
    CString fm;
    struct tm *newtime;
    time_t long_time;
    time(&long_time);
    newtime=localtime(&long_time);
    strftime(tmpbuf,128,"%Y%m%d ",newtime);
    fm=tmpbuf;

	CString fileDirectory;
	CFileDialogFilter strFilter(_T("HTML文件 只有文本 (*.htm)||HTML文件 拷贝文件 (*.htm)||BooguNote文件 纯文本(*.boo)||BooguNote文件 拷贝文件(*.boo)||纯文本 (*.txt)||FreeMind 文件 (*.mm)||"));
	CFileDialog fd(FALSE, _T(""), fm,   OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, strFilter);
	//fd.m_ofn.Flags = OFN_DONTADDTORECENT|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
	if (IDCANCEL == fd.DoModal())
	{
		return 1;
	}
	fileDirectory = fd.m_szFileName;

	CString fn(fileDirectory);
	int dot = fn.ReverseFind(_T('.'));
	CString ext = fn;
	ext.Delete(0, dot+1);
	ext.MakeLower();
	if (0==ext.Compare(_T("boo")))
	{
		if (4 == fd.m_ofn.nFilterIndex)
		{
			int ret = SaveBooFileWithAttachments(fileDirectory.GetBuffer(_MAX_PATH));
			fileDirectory.ReleaseBuffer();
		}
		else
		{
			int ret = SaveBooFile(m_textBlockList, fileDirectory.GetBuffer(_MAX_PATH), false);
			fileDirectory.ReleaseBuffer();
		}
	}
	else if (0==ext.Compare(_T("htm")))
	{
		if (2 == fd.m_ofn.nFilterIndex)
		{
			int ret = SaveHtmlFileWithAttachments(fileDirectory.GetBuffer(_MAX_PATH));
			fileDirectory.ReleaseBuffer();
		}
		else
		{
			int ret = SaveHtmlFile(fileDirectory.GetBuffer(_MAX_PATH));
			fileDirectory.ReleaseBuffer();
		}
		
	}
	else if (0==ext.Compare(_T("mm")))
	{
		int ret = SaveMMFile(fileDirectory.GetBuffer(_MAX_PATH), false);
		fileDirectory.ReleaseBuffer();
	}
	else if (0==ext.Compare(_T("txt")))
	{
		SaveTxtFile(fileDirectory.GetBuffer(_MAX_PATH));
		fileDirectory.ReleaseBuffer();
	}
	else
	{
		if (1 == fd.m_ofn.nFilterIndex)
		{
			fileDirectory += _T(".htm");
			int ret = SaveHtmlFile(fileDirectory.GetBuffer(_MAX_PATH));
			fileDirectory.ReleaseBuffer();
		}
		else if (2 == fd.m_ofn.nFilterIndex)
		{
			fileDirectory += _T(".htm");
			int ret = SaveHtmlFileWithAttachments(fileDirectory.GetBuffer(_MAX_PATH));
			fileDirectory.ReleaseBuffer();
		}
		else if (3 == fd.m_ofn.nFilterIndex)
		{
			fileDirectory += _T(".boo");
			int ret = SaveBooFile(m_textBlockList, fileDirectory.GetBuffer(_MAX_PATH), false);
			fileDirectory.ReleaseBuffer();
		}
		else if (4 == fd.m_ofn.nFilterIndex)
		{
			fileDirectory += _T(".boo");
			int ret = SaveBooFileWithAttachments(fileDirectory.GetBuffer(_MAX_PATH));
			fileDirectory.ReleaseBuffer();
		}
		else if (5 == fd.m_ofn.nFilterIndex)
		{
			fileDirectory += _T(".txt");
			SaveTxtFile(fileDirectory.GetBuffer(_MAX_PATH));
			fileDirectory.ReleaseBuffer();
		}
		else if (6 == fd.m_ofn.nFilterIndex)
		{
			fileDirectory += _T(".mm");
			int ret = SaveMMFile(fileDirectory.GetBuffer(_MAX_PATH), false);
			fileDirectory.ReleaseBuffer();
		}
		
	}
	
	return 0;
}

LRESULT CBooguNoteView::OnSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return SaveAs();
}

LRESULT CBooguNoteView::OnSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Save();
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}

int CBooguNoteView::Save()
{
	if (_T("") != m_fileDirectory)
	{
		CString fn(m_fileDirectory);
		int dot = fn.ReverseFind(_T('.'));
		CString ext = fn;
		ext.Delete(0, dot+1);
		ext.MakeLower();
		int ret = 0;
		if (0==ext.Compare(_T("boo")))
		{
			ret = SaveBooFile(m_textBlockList, m_fileDirectory.GetBuffer(_MAX_PATH), false);
			if (g_config.bSaveToTxtFile)
			{
				m_fileDirectory.ReleaseBuffer();
				CString txtDir = m_fileDirectory;
				txtDir.Delete(txtDir.GetLength()-4, 4);
				txtDir += _T(".txt");
				SaveTxtFile(txtDir.GetBuffer(_MAX_PATH));
				txtDir.ReleaseBuffer();
			}
		}
		else if (0==ext.Compare(_T("mm")))
		{
			ret = SaveMMFile(m_fileDirectory.GetBuffer(_MAX_PATH), false);
			m_fileDirectory.ReleaseBuffer();
		}
		if (m_bDirty)
		{
			m_bDirty = false;
			GetParent().SendMessageW(WM_NCPAINT);
		}
	}
	else
	{

            char tmpbuf[128];
            memset(tmpbuf,'\0',sizeof(tmpbuf));
            CString fm;
            struct tm *newtime;
            time_t long_time;
            time(&long_time);
            newtime=localtime(&long_time);
            strftime(tmpbuf,128,"%Y%m%d ",newtime);
            fm=tmpbuf;
		
		CString fileDirectory;
		CFileDialogFilter strFilter(_T("Boogu Notes (*.boo)||FreeMind Notes (*.mm)||"));
		CFileDialog fd(FALSE, _T(""), fm,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, strFilter);
		//fd.m_ofn.Flags = OFN_DONTADDTORECENT|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
		if (IDCANCEL == fd.DoModal())
		{
			return 1;
		}
		fileDirectory = fd.m_szFileName;

		CString fn(fileDirectory);
		int dot = fn.ReverseFind(_T('.'));
		CString ext = fn;
		ext.Delete(0, dot+1);
		ext.MakeLower();
		if (0==ext.Compare(_T("boo")))
		{
			int ret = SaveBooFile(m_textBlockList, fileDirectory.GetBuffer(_MAX_PATH), true);
			fileDirectory.ReleaseBuffer();
			if (g_config.bSaveToTxtFile)
			{
				m_fileDirectory.ReleaseBuffer();
				CString txtDir = m_fileDirectory;
				txtDir.Delete(txtDir.GetLength()-4, 4);
				txtDir += _T(".txt");
				SaveTxtFile(txtDir.GetBuffer(_MAX_PATH));
				txtDir.ReleaseBuffer();
			}
		}
		else if (0==ext.Compare(_T("mm")))
		{
			int ret = SaveMMFile(fileDirectory.GetBuffer(_MAX_PATH), true);
			fileDirectory.ReleaseBuffer();
		}
		else
		{
			if (1 == fd.m_ofn.nFilterIndex)
			{
				fileDirectory += _T(".boo");
				int ret = SaveBooFile(m_textBlockList, fileDirectory.GetBuffer(_MAX_PATH), true);
				fileDirectory.ReleaseBuffer();
				if (g_config.bSaveToTxtFile)
				{
					m_fileDirectory.ReleaseBuffer();
					CString txtDir = m_fileDirectory;
					txtDir.Delete(txtDir.GetLength()-4, 4);
					txtDir += _T(".txt");
					SaveTxtFile(txtDir.GetBuffer(_MAX_PATH));
					txtDir.ReleaseBuffer();
				}
			}
			else if (2 == fd.m_ofn.nFilterIndex)
			{
				fileDirectory += _T(".mm");
				int ret = SaveMMFile(fileDirectory.GetBuffer(_MAX_PATH), true);
				fileDirectory.ReleaseBuffer();
			}
		}
		/*SaveBooFile(_T(""));
		if (g_config.bSaveToTxtFile)
		{
			m_fileDirectory.ReleaseBuffer();
			CString txtDir = m_fileDirectory;
			txtDir.Delete(txtDir.GetLength()-4, 4);
			txtDir += _T(".txt");
			SaveTxtFile(txtDir.GetBuffer(_MAX_PATH));
			txtDir.ReleaseBuffer();
		}*/
	}
	return 0;
}

int CBooguNoteView::SaveMMFile(TCHAR* szFilePath, bool bUpdateFileName)
{
	if (NULL == szFilePath)
		return -1;
	CString fileDirectory(szFilePath);
	if (bUpdateFileName)
	{
		m_fileDirectory = fileDirectory;
		GetParent().SendMessageW(WM_USER_FILERENAME);
	}
	XML x;
	XMLElement* r = new XMLElement(0, "<map />");//m_XML->GetRootElement();
	x.SetRootElement(r);
	XMLElement* node = new XMLElement(r, "<node />");
	if (NULL!=szFilePath)
	{
		CString fileName = m_fileDirectory;
		fileName.Delete(0, fileName.ReverseFind(_T('\\'))+1);
		fileName.Delete(fileName.ReverseFind(_T('.')), fileName.GetLength()-1);

		int nFileNameLen = fileName.GetLength();//_tcslen(szFilePath);
		wchar_t* szEscBuf = new wchar_t[nFileNameLen*10+2];
		int nEscBufLen = EscapeXML(fileName.GetBuffer(_MAX_PATH), nFileNameLen, szEscBuf, nFileNameLen*10, ATL_ESC_FLAG_NONE);
		fileName.ReleaseBuffer();
		szEscBuf[nEscBufLen] = _T('\0');
		BOOL bDefault;
		int nAsciLen = WideCharToMultiByte(CP_ACP, 0, szEscBuf, nEscBufLen, 0, 0, " ", &bDefault);
		char* szAsci = new char[nAsciLen+2];
		WideCharToMultiByte(CP_ACP, 0, szEscBuf, nEscBufLen, szAsci, nAsciLen+1, " ", &bDefault);
		szAsci[nAsciLen] = '\0';
		delete[] szEscBuf;
		node->AddVariable("TEXT",szAsci);
		delete[] szAsci;
	}
	else
	{
		node->AddVariable("TEXT","");
	}
	r->AddElement(node);
	CreateMMNodes(node, 0, 0);
	x.Save(szFilePath);
	return 0;
}

void CBooguNoteView::CreateMMNodes(XMLElement* r, int nLevel, int nStartPoint)
{
	for (int i=nStartPoint; i<m_textBlockList.size(); ++i)
	{
		if (nLevel == m_textBlockList[i]->m_nLevel)
		{
			XMLElement* node = new XMLElement(r, "<node />");
			TCHAR* Buf = NULL;
			int nBlockTextLength = 0;
			if (m_textBlockList[i]->m_bFile || TEXT_BLOCK_SHRINK==m_textBlockList[i]->m_nExpandState)
			{
				nBlockTextLength = _tcslen(m_textBlockList[i]->m_szCache);
				Buf = new TCHAR[nBlockTextLength+1];
				_stprintf(Buf, _T("%s"), m_textBlockList[i]->m_szCache);
			}
			else
			{
				BSTR  bstrCache;
				m_textBlockList[i]->GetTextServices()->TxGetText(&bstrCache);
				nBlockTextLength = SysStringLen(bstrCache);
				Buf = new TCHAR[nBlockTextLength+1];
				_stprintf(Buf, _T("%s"), (LPCTSTR)bstrCache);
				SysFreeString(bstrCache);
			}
			//exclude final '\r'.
			if (_T('\r') == Buf[nBlockTextLength-1])
			{
				--nBlockTextLength;
				Buf[nBlockTextLength] = _T('\0');
			}
			wchar_t* szEscBuf = new wchar_t[nBlockTextLength*10+2];
			int nEscBufLen = EscapeXML(Buf, nBlockTextLength, szEscBuf, nBlockTextLength*10,ATL_ESC_FLAG_ATTR);
			szEscBuf[nEscBufLen] = _T('\0');
			delete[] Buf;
			BOOL bDefault;
			int nAsciLen = WideCharToMultiByte(CP_ACP, 0, szEscBuf, nEscBufLen, 0, 0, " ", &bDefault);
			char* szAsci = new char[nAsciLen+2];
			WideCharToMultiByte(CP_ACP, 0, szEscBuf, nEscBufLen, szAsci, nAsciLen+1, " ", &bDefault);
			szAsci[nAsciLen] = '\0';
			delete[] szEscBuf;
			XMLVariable* v = new XMLVariable();
			v->SetName("TEXT", 1);
			v->SetValue(szAsci, 1);
			node->AddVariable(v);
			delete[] szAsci;
			if (TEXT_ICON_PLAINTEXT != m_textBlockList[i]->m_pTextIcon->state)
			{
				XMLElement* icon = new XMLElement(node, "<icon />");
				XMLVariable* v1 = new XMLVariable();
				v1->SetName("BUILTIN", 1);
				if (TEXT_ICON_TODO == m_textBlockList[i]->m_pTextIcon->state)
				{
					v1->SetValue("flag", 1);
				}
				else if (TEXT_ICON_DONE == m_textBlockList[i]->m_pTextIcon->state)
				{
					v1->SetValue("button_ok", 1);
				}
				else if (TEXT_ICON_CROSS == m_textBlockList[i]->m_pTextIcon->state)
				{
					v1->SetValue("button_cancel", 1);
				}
				else if (TEXT_ICON_STAR == m_textBlockList[i]->m_pTextIcon->state)
				{
					v1->SetValue("bookmark", 1);
				}
				else if (TEXT_ICON_QUESTION == m_textBlockList[i]->m_pTextIcon->state)
				{
					v1->SetValue("help", 1);
				}
				else if (TEXT_ICON_WARNING == m_textBlockList[i]->m_pTextIcon->state)
				{
					v1->SetValue("messagebox_warning", 1);
				}
				else if (TEXT_ICON_IDEA == m_textBlockList[i]->m_pTextIcon->state)
				{
					v1->SetValue("idea", 1);
				}
				//delete icon;
				//delete v1;

				icon->AddVariable(v1);
				node->AddElement(icon);
			}
			r->AddElement(node);
			if (i+1<m_textBlockList.size()&&m_textBlockList[i+1]->m_nLevel>nLevel)
			{
				CreateMMNodes( node, m_textBlockList[i+1]->m_nLevel, i+1);
			}
		}
		else if (nLevel > m_textBlockList[i]->m_nLevel)
		{
			break;
		}
	}
}

int CBooguNoteView::SaveTxtFile(TCHAR* szFilePath)
{
	CString fileDirectory(szFilePath);

	if (fileDirectory == _T(""))
	{
		return -1;
	}
typedef TCHAR* PTCHAR;
	TCHAR** BlockBufferList = new PTCHAR[m_textBlockList.size()];
	int nLength = 0;
	int nBlockIndex= 0;
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		//if ((*ppTextBlock)->m_bSelected)
		//{
		TCHAR* Buf = NULL;
		int nBlockTextLength = 0;
		if ((*ppTextBlock)->m_bFile || TEXT_BLOCK_SHRINK==(*ppTextBlock)->m_nExpandState)
		{
			nBlockTextLength = _tcslen((*ppTextBlock)->m_szCache)+1;
			Buf = new TCHAR[nBlockTextLength+1+1];//+1 for '-' and '+'
			_stprintf(Buf, _T(" %s"), (*ppTextBlock)->m_szCache);
		}
		else
		{
			BSTR  bstrCache;
			(*ppTextBlock)->GetTextServices()->TxGetText(&bstrCache);
			nBlockTextLength = SysStringLen(bstrCache);
			Buf = new TCHAR[nBlockTextLength+1+1];//+1 for '-' and '+'
			_stprintf(Buf, _T(" %s"), (LPCTSTR)bstrCache);
			SysFreeString(bstrCache);
		}
		if (g_config.bCopyTextWithSymbol)
		{
			if ((ppTextBlock+1)!=m_textBlockList.end() && (*ppTextBlock)->m_nLevel<(*(ppTextBlock+1))->m_nLevel)
				Buf[0] = _T('+');
			else
				Buf[0] = _T('-');
		}
		else
		{
			Buf[0] = _T(' ');
		}
		//exclude final '\r'.
		if (_T('\r') == Buf[nBlockTextLength-1])
		{
			--nBlockTextLength;
			Buf[nBlockTextLength] = _T('\0');
		}
		//get number of enter key.
		int nEnterCount = 0;
		for(int i=0; i<nBlockTextLength; ++i)
		{
			if (_T('\r') == Buf[i])
			{
				++nEnterCount;
			}
		}
		int nMoreRoom = nEnterCount*((*ppTextBlock)->m_nLevel*g_config.nCopyIndent+1)+2;//+2 means '\r' and '\n'
		if (0!=(*ppTextBlock)->m_nLevel)//for head indent
		{
			nMoreRoom += ((*ppTextBlock)->m_nLevel*g_config.nCopyIndent);
		}
		int nRealBlockTextLength = nBlockTextLength + nMoreRoom + 1;//+1 for null terminal
		TCHAR* BlockTextString = new TCHAR[nRealBlockTextLength];
		int j=0;
		int nIncident = (*ppTextBlock)->m_nLevel/* - nMinIncident*/;
		for (int k=0; k<nIncident*g_config.nCopyIndent-1; ++k, ++j)
		{
			BlockTextString[j] = _T(' ');
		}
		//for(int k=0; k<nIncident; ++k/*, ++j*/)
		//{
		//	for (int l=0; l<g_config.nCopyIndent; ++l, ++j)
		//	{
		//		BlockTextString[j] = _T(' ');
		//	}
		//	--j;
		//}
		for(int i=0; i<nBlockTextLength; ++i, ++j)
		{
			if (_T('\r') == Buf[i])
			{
				BlockTextString[j] = _T('\r');
				++j;
				BlockTextString[j] = _T('\n');
				++j;
				int nIncident = (*ppTextBlock)->m_nLevel/* - nMinIncident*/;
				for(int k=0; k<nIncident; ++k, ++j)
				{
					for (int l=0; l<g_config.nCopyIndent; ++l, ++j)
					{
						BlockTextString[j] = _T(' ');
					}
					--j;
				}
				--j;
				
				//++j;
				//i+=1;
			}
			else
				BlockTextString[j] = Buf[i];
		}
		BlockTextString[j] = _T('\r');
		++j;
		BlockTextString[j] = _T('\n');
		++j;
		BlockTextString[j] = _T('\0');
		//BlockTextString[nRealBlockTextLength-3] = _T('\0');
		BlockBufferList[nBlockIndex] = BlockTextString;
		nLength+= (nRealBlockTextLength-1);//whole length doesn't need null terminal
		delete Buf;
		++nBlockIndex;
		//}
	}
	if (0 == nLength)
		return -1;

	++nLength; //and null termial length;
	TCHAR* pData = new TCHAR[nLength+10];

	for(int i=0, index=0; i<nBlockIndex; ++i)
	{
		_tcscpy((TCHAR*)(pData+index),BlockBufferList[i]);
		index += _tcslen(BlockBufferList[i]);
		//++index;
	}

	//CString txtDir = fileDirectory;
	//txtDir.Delete(txtDir.GetLength()-4, 4);
	HANDLE hFile = CreateFileW(fileDirectory, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD le = GetLastError();
	UINT nCodePage = -1;
	if (g_config.nCodePage == 0)
	{
		nCodePage = CP_ACP;
	}
	else
	{
		nCodePage = CP_UTF8;
	}
	DWORD dwBytesWritten = WideCharToMultiByte(nCodePage, 0, pData, -1, 0, 0, NULL, NULL);
	char* utf8buf = new char[dwBytesWritten];
	int lBytesWritten = WideCharToMultiByte(nCodePage, 0, pData, -1, utf8buf, dwBytesWritten, NULL, NULL);
	DWORD dwRt = GetLastError();
	DWORD dwFileBytesWritten;
	WriteFile(hFile, utf8buf, lBytesWritten, &dwFileBytesWritten, NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	for (int i=0; i< nBlockIndex; ++i)
		delete[] BlockBufferList[i];
	delete[] BlockBufferList;
	delete[] pData;
	delete[] utf8buf;
	return 0;
}

int CBooguNoteView::SaveHtmlFile(TCHAR* szFilePath)
{
	CXml xml;
	CXsl xsl;

	TCHAR lpTempPathBuffer[MAX_PATH];
	DWORD dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
		lpTempPathBuffer); // buffer for path 
	if (dwRetVal > MAX_PATH || (dwRetVal == 0))
	{
		MessageBox(L"创建临时文件失败", L"警告", MB_OK|MB_ICONWARNING);
		return -1;
	}
	TCHAR szTempFileName[MAX_PATH];
	UINT uRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
		TEXT("BOO"),     // temp file name prefix 
		0,                // create unique name 
		szTempFileName);  // buffer for name 

	SaveBooFile(m_textBlockList, szTempFileName, false, true);
	
	if( !xml.Open(szTempFileName) )
	{
		MessageBox(L"打开临时生成的boo文件失败", L"警告", MB_OK|MB_ICONWARNING);
		return -1;
	}

	//DetectXslFile();

	TCHAR szPath[MAX_PATH+10];
	GetModuleFileName(_Module.m_hInst, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathAppend(szPath, L"BooguNote.xsl");

	if( !xsl.Open(szPath) )
	{
		MessageBox(L"\"BooguNote.xsl\" 有问题，不能使用。", L"警告", MB_OK|MB_ICONWARNING);
		return -1;
	}

	::DeleteFile(szFilePath);
	xsl.TransformToFile( xml, szFilePath);
	DeleteFile(szTempFileName);
	return 0;
}

int CBooguNoteView::SaveBooFileWithAttachments(TCHAR* szFilePath)
{
	if (NULL == szFilePath)
		return -1;
	CString fileDirectory(szFilePath);

	char szBuf[128];
	sprintf(szBuf, "<root version=\"%d\"/>", BOO_FILE_VERSION);
	XML x;
	XMLElement* r = new XMLElement(0, szBuf);//m_XML->GetRootElement();
	x.SetRootElement(r);
	XMLHeader* h = new XMLHeader(XML_HEADER);
	x.SetHeader(h);

	XMLVariable* dir = new XMLVariable();
	dir->SetName("DefaultSaveDir");
	dir->SetValue(W(m_strDefaultSaveDir));
	r->AddVariable(dir);

	XMLVariable* ext = new XMLVariable();
	ext->SetName("DefaultSaveExtension");
	ext->SetValue(W(m_strDefaultImgExtension));
	r->AddVariable(ext);

	CString strFilePath = szFilePath;

	//PathStripPath(strFileName.GetBuffer(MAX_PATH));
	//strFileName.ReleaseBuffer();

	PathRemoveExtension(strFilePath.GetBuffer(MAX_PATH));
	strFilePath.ReleaseBuffer();

	strFilePath = strFilePath + L"_files\\";

	CreateDirectory(strFilePath, NULL);

	if (!m_textBlockList.empty() && 0 != m_textBlockList[0]->m_nLevel)
	{
		m_textBlockList[0]->m_nLevel = 0;
		CalcLayout();
	}
	CreateBooNodesWithAttachments(r, 0, 0, strFilePath);

	x.Save(szFilePath);
}

int CBooguNoteView::SaveHtmlFileWithAttachments(TCHAR* szFilePath)
{
	if (NULL == szFilePath)
		return -1;
	CString fileDirectory(szFilePath);

	char szBuf[128];
	sprintf(szBuf, "<root version=\"%d\"/>", BOO_FILE_VERSION);
	XML x;
	XMLElement* r = new XMLElement(0, szBuf);//m_XML->GetRootElement();
	x.SetRootElement(r);
	XMLHeader* h = new XMLHeader(XML_HEADER);
	x.SetHeader(h);

	XMLVariable* dir = new XMLVariable();
	dir->SetName("DefaultSaveDir");
	dir->SetValue(W(m_strDefaultSaveDir));
	r->AddVariable(dir);

	XMLVariable* ext = new XMLVariable();
	ext->SetName("DefaultSaveExtension");
	ext->SetValue(W(m_strDefaultImgExtension));
	r->AddVariable(ext);

	CString strFilePath = szFilePath;

	//PathStripPath(strFileName.GetBuffer(MAX_PATH));
	//strFileName.ReleaseBuffer();

	PathRemoveExtension(strFilePath.GetBuffer(MAX_PATH));
	strFilePath.ReleaseBuffer();

	strFilePath = strFilePath + L"_files\\";

	CreateDirectory(strFilePath, NULL);

	if (!m_textBlockList.empty() && 0 != m_textBlockList[0]->m_nLevel)
	{
		m_textBlockList[0]->m_nLevel = 0;
		CalcLayout();
	}
	CreateBooNodesWithAttachments(r, 0, 0, strFilePath);

	//准备转换
	CXml xml;
	CXsl xsl;
	//保存成临时boo文件
	TCHAR lpTempPathBuffer[MAX_PATH];
	DWORD dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
		lpTempPathBuffer); // buffer for path 
	if (dwRetVal > MAX_PATH || (dwRetVal == 0))
	{
		MessageBox(L"创建临时文件失败", L"警告", MB_OK|MB_ICONWARNING);
		return -1;
	}
	TCHAR szTempFileName[MAX_PATH];
	UINT uRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
		TEXT("BOO"),     // temp file name prefix 
		0,                // create unique name 
		szTempFileName);  // buffer for name 
	x.Save(szTempFileName);

	//转换
	if( !xml.Open(szTempFileName) )
	{
		MessageBox(L"打开临时生成的boo文件失败", L"警告", MB_OK|MB_ICONWARNING);
		return -1;
	}

	//DetectXslFile();

	TCHAR szPath[MAX_PATH+10];
	GetModuleFileName(_Module.m_hInst, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathAppend(szPath, L"BooguNote.xsl");

	if( !xsl.Open(szPath) )
	{
		MessageBox(L"\"BooguNote.xsl\" 有问题，不能使用。", L"警告", MB_OK|MB_ICONWARNING);
		return -1;
	}

	::DeleteFile(szFilePath);
	xsl.TransformToFile( xml, szFilePath);
	DeleteFile(szTempFileName);
	return 0;
}

int CBooguNoteView::SaveBooFile(vector<CBooguNoteText*> &textBlockList, TCHAR* szFilePath, bool bUpdateFileName, bool bSaveFullPath)
{
	if (NULL == szFilePath)
		return -1;
	CString fileDirectory(szFilePath);
	if (bUpdateFileName)
	{
		m_fileDirectory = fileDirectory;
		GetParent().SendMessageW(WM_USER_FILERENAME);
	}
	char szBuf[128];
	sprintf(szBuf, "<root version=\"%d\"/>", BOO_FILE_VERSION);
	XML x;
	XMLElement* r = new XMLElement(0, szBuf);//m_XML->GetRootElement();
	x.SetRootElement(r);
	XMLHeader* h = new XMLHeader(XML_HEADER);
	x.SetHeader(h);

	XMLVariable* dir = new XMLVariable();
	dir->SetName("DefaultSaveDir");
	dir->SetValue(W(m_strDefaultSaveDir));
	r->AddVariable(dir);

	XMLVariable* ext = new XMLVariable();
	ext->SetName("DefaultSaveExtension");
	ext->SetValue(W(m_strDefaultImgExtension));
	r->AddVariable(ext);

	CString strFileFolder = m_fileDirectory; //注意这里必须要用m_fileDirectory，因为图片路径是这个
	PathRemoveFileSpec(strFileFolder.GetBuffer(MAX_PATH));
	strFileFolder.ReleaseBuffer();

	if (!textBlockList.empty() && 0 != textBlockList[0]->m_nLevel)
	{
		textBlockList[0]->m_nLevel = 0;
		CalcLayout();
	}
	CreateBooNodes(textBlockList, r, 0, 0, bSaveFullPath, strFileFolder);
	x.Save(szFilePath);
	return 0;
}

void CBooguNoteView::CreateBooNodesWithAttachments(XMLElement* r, int nLevel, int nStartPoint, const CString& strFolder)
{
	USES_CONVERSION;

	for (int i=nStartPoint; i<m_textBlockList.size(); ++i)
	{
		if (nLevel == m_textBlockList[i]->m_nLevel)
		{
			XMLElement* node = new XMLElement(r, "<item />");
			TCHAR* Buf = NULL;
			int nBlockTextLength = 0;
			if (m_textBlockList[i]->m_bFile || TEXT_BLOCK_SHRINK==m_textBlockList[i]->m_nExpandState)
			{
				nBlockTextLength = _tcslen(m_textBlockList[i]->m_szCache);
				Buf = new TCHAR[nBlockTextLength+1];
				_stprintf(Buf, _T("%s"), m_textBlockList[i]->m_szCache);
			}
			else
			{
				BSTR  bstrCache;
				m_textBlockList[i]->GetTextServices()->TxGetText(&bstrCache);
				nBlockTextLength = SysStringLen(bstrCache);
				Buf = new TCHAR[nBlockTextLength+100];
				_stprintf(Buf, _T("%s"), (LPCTSTR)bstrCache);
				SysFreeString(bstrCache);
			}
			//exclude final '\r'.
			if (_T('\r') == Buf[nBlockTextLength-1])
			{
				--nBlockTextLength;
				Buf[nBlockTextLength] = _T('\0');
			}
			CString strTemp = Buf;
			if (m_textBlockList[i]->m_bFile)
			{
				if (strTemp.GetLength() >= 2 && L'\\' == strTemp[0] && L'\\' != strTemp[1])
				{
					strTemp = g_config.szRootStorageDir + strTemp; 
				}
				else if (L'.' == strTemp[0])
				{
					CString filePath = m_fileDirectory;
					PathRemoveFileSpec(filePath.GetBuffer(MAX_PATH));
					filePath.ReleaseBuffer();
					if (!filePath.IsEmpty())
					{
						CString strBooFilePath = filePath;
						PathCombine(filePath.GetBuffer(MAX_PATH), strBooFilePath, strTemp);
						filePath.ReleaseBuffer();
						strTemp = filePath;
					}
				}

				//拷贝的源
				CString strCopyFrom = strTemp;

				//得到拷贝的目的
				CString strCopyFileName = strTemp;
				PathStripPath(strCopyFileName.GetBuffer(MAX_PATH));
				strCopyFileName.ReleaseBuffer();

				CString strCopyTo = strFolder;
				PathAppend(strCopyTo.GetBuffer(MAX_PATH), strCopyFileName);
				strCopyTo.ReleaseBuffer();
				if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(strCopyTo))
				{
					int nPos=strCopyFileName.ReverseFind( '.'); 
					CString strExtension=strCopyFileName.Right(strCopyFileName.GetLength()-nPos);
					CString strFileNameWithoutExtention = strCopyFileName.Left(nPos);
					for (int i=0; ;i++)
					{
						CString strNum;
						strNum.Format(L"[%d]", i);
						strCopyFileName = strFileNameWithoutExtention + strNum + strExtension;
						strCopyTo = strFolder;
						PathAppend(strCopyTo.GetBuffer(MAX_PATH), strCopyFileName);
						if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strCopyTo))
						{
							break;
						}
					}
				}

				//进行拷贝
				CopyFile(strCopyFrom, strCopyTo, FALSE);

				//重新设置路径
				CString strNewRelativePath = strFolder;
				PathRemoveBackslash(strNewRelativePath.GetBuffer(MAX_PATH));
				strNewRelativePath.ReleaseBuffer();
				PathStripPath(strNewRelativePath.GetBuffer(MAX_PATH));
				strNewRelativePath.ReleaseBuffer();
				strTemp = L".\\" + strNewRelativePath + L"\\";
				strTemp += strCopyFileName;
			}
			
			
			//strTemp.Replace(L"<", L"&lt;");
			//strTemp.Replace(L">", L"&gt;");
			//strTemp.Replace(L"&", L"&amp;");
			//strTemp.Replace(L"'", L"&apos;");
			//strTemp.Replace(L"\"", L"&quot;");
			strTemp.Replace(L"\r", L"&#xA;");
			/*wchar_t* szEscBuf = new wchar_t[nBlockTextLength*10+2];
			int nEscBufLen = EscapeXML(Buf, nBlockTextLength, szEscBuf, nBlockTextLength*10,ATL_ESC_FLAG_NONE);
			szEscBuf[nEscBufLen] = _T('\0');*/
			
			//BOOL bDefault;
			//int nMB = WideCharToMultiByte(CP_UTF8, 0, Buf, -1, 0, 0, " ", &bDefault);
			//char* szMB = new char[nMB+2];
			//WideCharToMultiByte(CP_UTF8, 0, Buf, nBlockTextLength, szMB, nMB+1, " ", &bDefault);
			//szMB[nMB] = '\0';
			
			
			XMLVariable* content = new XMLVariable();
			content->SetName("content"/*, 1*/);
			//v->SetValue(W(szEscBuf), 1);
			content->SetValue(W(strTemp)/*, 1*/);
			node->AddVariable(content);
			delete[] Buf;
			//delete[] szEscBuf;
			//delete[] szMB;
			
			XMLVariable* icon = new XMLVariable();
			icon->SetName("icon");
			if (TEXT_ICON_PLAINTEXT == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("none");
			}
			else if (TEXT_ICON_TODO == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("flag");
			}
			else if (TEXT_ICON_DONE == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("tick");
			}
			else if (TEXT_ICON_CROSS == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("cross");
			}
			else if (TEXT_ICON_STAR == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("star");
			}
			else if (TEXT_ICON_QUESTION == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("question");
			}
			else if (TEXT_ICON_WARNING == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("warning");
			}
			else if (TEXT_ICON_IDEA == m_textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("idea");
			}
			//delete icon;
			//delete v1;

			node->AddVariable(icon);
			
			XMLVariable* branchstate = new XMLVariable();
			branchstate->SetName("branch");
			if (TEXT_HANDLE_NULL == m_textBlockList[i]->m_pTextHandle->state)
			{
				branchstate->SetValue("none");
			}
			else if (TEXT_HANDLE_EXPAND == m_textBlockList[i]->m_pTextHandle->state)
			{
				branchstate->SetValue("open");
			}
			else if (TEXT_HANDLE_CLOSED == m_textBlockList[i]->m_pTextHandle->state)
			{
				branchstate->SetValue("close");
			}
			node->AddVariable(branchstate);
			
			XMLVariable* blockstate = new XMLVariable();
			blockstate->SetName("block");
			if (TEXT_BLOCK_NARROW == m_textBlockList[i]->m_nExpandState)
			{
				blockstate->SetValue("narrow");
			}
			else if (TEXT_BLOCK_SHRINK == m_textBlockList[i]->m_nExpandState)
			{
				blockstate->SetValue("shrink");
			}
			else if (TEXT_BLOCK_WIDE == m_textBlockList[i]->m_nExpandState)
			{
				blockstate->SetValue("wide");
			}
			node->AddVariable(blockstate);

			XMLVariable* level = new XMLVariable();
			level->SetName("level", 1);
			level->SetValueInt(m_textBlockList[i]->m_nLevel);
			node->AddVariable(level);

			XMLVariable* shownstate = new XMLVariable();
			shownstate->SetName("IsShown");
			if (m_textBlockList[i]->m_bShown)
			{
				shownstate->SetValue("true");
			}
			else
			{
				shownstate->SetValue("false");
			}
			node->AddVariable(shownstate);

			XMLVariable* bold = new XMLVariable();
			bold->SetName("IsBold");
			if (m_textBlockList[i]->m_bBold)
			{
				bold->SetValue("true");
			}
			else
			{
				bold->SetValue("false");
			}
			node->AddVariable(bold);

			XMLVariable* showbranch = new XMLVariable();
			showbranch->SetName("ShowBranch");
			if (m_textBlockList[i]->m_bShowBranch)
			{
				showbranch->SetValue("true");
			}
			else
			{
				showbranch->SetValue("false");
			}
			node->AddVariable(showbranch);

			char cBuf[1024];
			XMLVariable* textClr = new XMLVariable();
			textClr->SetName("TextColor");
			BinaryToStringA((BYTE*)&m_textBlockList[i]->m_TextClr, sizeof(COLORREF), cBuf);
			textClr->SetValue(cBuf);
			node->AddVariable(textClr);

			XMLVariable* bkgrdClr = new XMLVariable();
			bkgrdClr->SetName("BkgrdColor");
			BinaryToStringA((BYTE*)&m_textBlockList[i]->m_BkgrdClr, sizeof(COLORREF), cBuf);
			bkgrdClr->SetValue(cBuf);
			node->AddVariable(bkgrdClr);

			if (0!=m_textBlockList[i]->m_ModifyTime)
			{
				XMLVariable* timeModify = new XMLVariable();
				timeModify->SetName("ModifyTime");
				struct tm * timeinfo;
				timeinfo = localtime ( &(m_textBlockList[i]->m_ModifyTime));
				sprintf(cBuf, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
				timeModify->SetValue(cBuf);
				node->AddVariable(timeModify);
			}

			

			XMLVariable* IsFile = new XMLVariable();
			IsFile->SetName("IsFile");
			if (m_textBlockList[i]->m_bFile)
			{
				IsFile->SetValue("true");

				XMLVariable* IsImage = new XMLVariable();
				IsImage->SetName("IsImage");
				if (NULL != m_textBlockList[i]->m_pBitmap)
				{
					IsImage->SetValue("true");
				}
				else
				{
					IsImage->SetValue("false");
				}
				node->AddVariable(IsImage);
			}
			else
			{
				IsFile->SetValue("false");
			}
			node->AddVariable(IsFile);

			r->AddElement(node);
			if (i+1<m_textBlockList.size()&&m_textBlockList[i+1]->m_nLevel>nLevel)
			{
				CreateBooNodesWithAttachments( node, m_textBlockList[i+1]->m_nLevel, i+1, strFolder);
			}
		}
		else if (nLevel > m_textBlockList[i]->m_nLevel)
		{
			break;
		}
	}
}

void CBooguNoteView::CreateBooNodes(vector<CBooguNoteText*> &textBlockList, XMLElement* r, int nLevel, int nStartPoint, bool bSaveFullPath, const CString& strFolder)
{
	USES_CONVERSION;

	for (int i=nStartPoint; i<textBlockList.size(); ++i)
	{
		if (nLevel == textBlockList[i]->m_nLevel)
		{
			XMLElement* node = new XMLElement(r, "<item />");
			TCHAR* Buf = NULL;
			int nBlockTextLength = 0;
			if (textBlockList[i]->m_bFile || TEXT_BLOCK_SHRINK==textBlockList[i]->m_nExpandState)
			{
				nBlockTextLength = _tcslen(textBlockList[i]->m_szCache);
				Buf = new TCHAR[nBlockTextLength+1];
				_stprintf(Buf, _T("%s"), textBlockList[i]->m_szCache);
			}
			else
			{
				BSTR  bstrCache;
				textBlockList[i]->GetTextServices()->TxGetText(&bstrCache);
				nBlockTextLength = SysStringLen(bstrCache);
				Buf = new TCHAR[nBlockTextLength+100];
				_stprintf(Buf, _T("%s"), (LPCTSTR)bstrCache);
				SysFreeString(bstrCache);
			}
			//exclude final '\r'.
			if (_T('\r') == Buf[nBlockTextLength-1])
			{
				--nBlockTextLength;
				Buf[nBlockTextLength] = _T('\0');
			}
			CString strTemp = Buf;
			if (bSaveFullPath && textBlockList[i]->m_bFile)
			{
				if (strTemp.GetLength() >= 2 && L'\\' == strTemp[0] && L'\\' != strTemp[1])
				{
					strTemp = g_config.szRootStorageDir + strTemp; 
				}
				else if (L'.' == strTemp[0])
				{
					if (!strFolder.IsEmpty())
					{
						CString strRelativePath = strTemp;
						PathCombine(strTemp.GetBuffer(MAX_PATH), strFolder, strRelativePath);
					}
				}
			}
			//strTemp.Replace(L"<", L"&lt;");
			//strTemp.Replace(L">", L"&gt;");
			//strTemp.Replace(L"&", L"&amp;");
			//strTemp.Replace(L"'", L"&apos;");
			//strTemp.Replace(L"\"", L"&quot;");
			strTemp.Replace(L"\r", L"&#xA;");
			/*wchar_t* szEscBuf = new wchar_t[nBlockTextLength*10+2];
			int nEscBufLen = EscapeXML(Buf, nBlockTextLength, szEscBuf, nBlockTextLength*10,ATL_ESC_FLAG_NONE);
			szEscBuf[nEscBufLen] = _T('\0');*/
			
			//BOOL bDefault;
			//int nMB = WideCharToMultiByte(CP_UTF8, 0, Buf, -1, 0, 0, " ", &bDefault);
			//char* szMB = new char[nMB+2];
			//WideCharToMultiByte(CP_UTF8, 0, Buf, nBlockTextLength, szMB, nMB+1, " ", &bDefault);
			//szMB[nMB] = '\0';
			
			
			XMLVariable* content = new XMLVariable();
			content->SetName("content"/*, 1*/);
			//v->SetValue(W(szEscBuf), 1);
			content->SetValue(W(strTemp)/*, 1*/);
			node->AddVariable(content);
			delete[] Buf;
			//delete[] szEscBuf;
			//delete[] szMB;
			
			XMLVariable* icon = new XMLVariable();
			icon->SetName("icon");
			if (TEXT_ICON_PLAINTEXT == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("none");
			}
			else if (TEXT_ICON_TODO == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("flag");
			}
			else if (TEXT_ICON_DONE == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("tick");
			}
			else if (TEXT_ICON_CROSS == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("cross");
			}
			else if (TEXT_ICON_STAR == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("star");
			}
			else if (TEXT_ICON_QUESTION == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("question");
			}
			else if (TEXT_ICON_WARNING == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("warning");
			}
			else if (TEXT_ICON_IDEA == textBlockList[i]->m_pTextIcon->state)
			{
				icon->SetValue("idea");
			}
			//delete icon;
			//delete v1;

			node->AddVariable(icon);
			
			XMLVariable* branchstate = new XMLVariable();
			branchstate->SetName("branch");
			if (TEXT_HANDLE_NULL == textBlockList[i]->m_pTextHandle->state)
			{
				branchstate->SetValue("none");
			}
			else if (TEXT_HANDLE_EXPAND == textBlockList[i]->m_pTextHandle->state)
			{
				branchstate->SetValue("open");
			}
			else if (TEXT_HANDLE_CLOSED == textBlockList[i]->m_pTextHandle->state)
			{
				branchstate->SetValue("close");
			}
			node->AddVariable(branchstate);
			
			XMLVariable* blockstate = new XMLVariable();
			blockstate->SetName("block");
			if (TEXT_BLOCK_NARROW == textBlockList[i]->m_nExpandState)
			{
				blockstate->SetValue("narrow");
			}
			else if (TEXT_BLOCK_SHRINK == textBlockList[i]->m_nExpandState)
			{
				blockstate->SetValue("shrink");
			}
			else if (TEXT_BLOCK_WIDE == textBlockList[i]->m_nExpandState)
			{
				blockstate->SetValue("wide");
			}
			node->AddVariable(blockstate);

			XMLVariable* level = new XMLVariable();
			level->SetName("level", 1);
			level->SetValueInt(textBlockList[i]->m_nLevel);
			node->AddVariable(level);

			XMLVariable* shownstate = new XMLVariable();
			shownstate->SetName("IsShown");
			if (textBlockList[i]->m_bShown)
			{
				shownstate->SetValue("true");
			}
			else
			{
				shownstate->SetValue("false");
			}
			node->AddVariable(shownstate);

			XMLVariable* bold = new XMLVariable();
			bold->SetName("IsBold");
			if (textBlockList[i]->m_bBold)
			{
				bold->SetValue("true");
			}
			else
			{
				bold->SetValue("false");
			}
			node->AddVariable(bold);

			XMLVariable* showbranch = new XMLVariable();
			showbranch->SetName("ShowBranch");
			if (textBlockList[i]->m_bShowBranch)
			{
				showbranch->SetValue("true");
			}
			else
			{
				showbranch->SetValue("false");
			}
			node->AddVariable(showbranch);

			char cBuf[1024];
			XMLVariable* textClr = new XMLVariable();
			textClr->SetName("TextColor");
			BinaryToStringA((BYTE*)&textBlockList[i]->m_TextClr, sizeof(COLORREF), cBuf);
			textClr->SetValue(cBuf);
			node->AddVariable(textClr);

			XMLVariable* bkgrdClr = new XMLVariable();
			bkgrdClr->SetName("BkgrdColor");
			BinaryToStringA((BYTE*)&textBlockList[i]->m_BkgrdClr, sizeof(COLORREF), cBuf);
			bkgrdClr->SetValue(cBuf);
			node->AddVariable(bkgrdClr);

			if (0!=textBlockList[i]->m_ModifyTime)
			{
				XMLVariable* timeModify = new XMLVariable();
				timeModify->SetName("ModifyTime");
				struct tm * timeinfo;
				timeinfo = localtime ( &(textBlockList[i]->m_ModifyTime));
				sprintf(cBuf, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
				timeModify->SetValue(cBuf);
				node->AddVariable(timeModify);
			}

			

			XMLVariable* IsFile = new XMLVariable();
			IsFile->SetName("IsFile");
			if (textBlockList[i]->m_bFile)
			{
				IsFile->SetValue("true");

				XMLVariable* IsImage = new XMLVariable();
				IsImage->SetName("IsImage");
				if (NULL != textBlockList[i]->m_pBitmap)
				{
					IsImage->SetValue("true");
				}
				else
				{
					IsImage->SetValue("false");
				}
				node->AddVariable(IsImage);
			}
			else
			{
				IsFile->SetValue("false");
			}
			node->AddVariable(IsFile);

			r->AddElement(node);
			if (i+1<textBlockList.size()&&textBlockList[i+1]->m_nLevel>nLevel)
			{
				CreateBooNodes(textBlockList, node, textBlockList[i+1]->m_nLevel, i+1, bSaveFullPath, strFolder);
			}
		}
		else if (nLevel > textBlockList[i]->m_nLevel)
		{
			break;
		}
	}
}

void CBooguNoteView::PasteFiles(HDROP hDrop)
{
	if (NULL != m_pTextBlockFocused)
	{
		int nIndex = 0;
		for (; nIndex<m_textBlockList.size(); ++nIndex)
		{
			if (m_textBlockList[nIndex] == m_pTextBlockFocused)
			{
				break;
			}
		}

		int nLevel = -1;
		if (nIndex >= m_textBlockList.size())
		{
			nIndex = m_textBlockList.size()-1;//adjust to the last element.
			nLevel = 0;
		}
		else
		{
			//bypass hiden sub tree
			if (TEXT_HANDLE_NULL != m_textBlockList[nIndex]->m_pTextHandle->state)
			{
				int nCurrLevel = m_textBlockList[nIndex]->m_nLevel;
				++nIndex;
				if (nIndex<m_textBlockList.size())
				{
					
					for (; nIndex<m_textBlockList.size(); ++nIndex)
					{
						if (m_textBlockList[nIndex]->m_nLevel<=nCurrLevel)
						{
							break;
						}
					}
					if (nIndex >= m_textBlockList.size())
					{
						nIndex = m_textBlockList.size()-1;//adjust to the last element.
					}
					else if (nIndex<=0)
					{
						nIndex = 0;
					}
					else
					{
						--nIndex;
					}
				}
				nLevel = nCurrLevel;
			}
			else
			{
				nLevel = m_textBlockList[nIndex]->m_nLevel;
			}
			BSTR bstrCache;
			m_textBlockList[nIndex]->GetTextServices()->TxGetText(&bstrCache);
			int nTextLength = SysStringLen(bstrCache);
			//if the past point is an empty line.
			if (1 == nTextLength && !m_textBlockList[nIndex]->m_bFile)
			{
				delete m_textBlockList[nIndex];
				m_textBlockList.erase(m_textBlockList.begin()+nIndex);
				m_pTextBlockFocused = NULL;
				nIndex--;
			}
		}

		UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		CString strFileName;
		for (UINT i=0; i<nFiles; ++i, ++nIndex)
		{
			DragQueryFile((HDROP)hDrop, i, strFileName.GetBuffer(MAX_PATH), MAX_PATH);
			strFileName.ReleaseBuffer();
			CBooguNoteText* pTextBlock = NULL; 

			//CString strRelativePath;
			//PathRelativePathTo(strRelativePath.GetBuffer(MAX_PATH),
			//	m_fileDirectory,
			//	FILE_ATTRIBUTE_NORMAL,
			//	strFileName,
			//	FILE_ATTRIBUTE_NORMAL);
			//strRelativePath.ReleaseBuffer();
			//if (!strRelativePath.IsEmpty())
			//{
			//	strFileName = strRelativePath;
			//}

			pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-(nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder),strFileName);
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;

			pTextBlock->m_bShown = true;
			pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
			pTextBlock->m_nLevel = nLevel;
			if (nIndex == m_textBlockList.size())
				++nIndex;//adjust to the last element.
			m_nLastClicked = nIndex;
			if (m_textBlockList.size()>0)
			{
				m_textBlockList.insert(m_textBlockList.begin()+1+nIndex, pTextBlock);
			}
			else
			{
				m_textBlockList.push_back(pTextBlock);
			}
			pTextBlock->m_bSelected = false;
			time(&(pTextBlock->m_ModifyTime));
			pTextBlock->DetectFileBlock();
		}

		CalcLayout();
		CalcScrollBar();
		m_bDirty = true;
		GetParent().SendMessageW(WM_NCPAINT);
	}
}

LRESULT CBooguNoteView::OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	ClientToDevice(pt);
	int nIndex = 0;
	for (; nIndex<m_textBlockList.size(); ++nIndex)
	{
		if (m_textBlockList[nIndex]->m_bShown && m_textBlockList[nIndex]->HitTest(pt.x, pt.y))
		{
			break;
		}
	}

	int nLevel = -1;
	if (nIndex >= m_textBlockList.size())
	{
		nIndex = m_textBlockList.size()-1;//adjust to the last element.
		nLevel = 0;
	}
	else
	{
		//bypass hiden sub tree
		if (TEXT_HANDLE_NULL != m_textBlockList[nIndex]->m_pTextHandle->state)
		{
			int nCurrLevel = m_textBlockList[nIndex]->m_nLevel;
			++nIndex;
			if (nIndex<m_textBlockList.size())
			{
				
				for (; nIndex<m_textBlockList.size(); ++nIndex)
				{
					if (m_textBlockList[nIndex]->m_nLevel<=nCurrLevel)
					{
						break;
					}
				}
				if (nIndex >= m_textBlockList.size())
				{
					nIndex = m_textBlockList.size()-1;//adjust to the last element.
				}
			}
			nLevel = nCurrLevel;
		}
		else
		{
			nLevel = m_textBlockList[nIndex]->m_nLevel;
		}
		BSTR bstrCache;
		m_textBlockList[nIndex]->GetTextServices()->TxGetText(&bstrCache);
		int nTextLength = SysStringLen(bstrCache);
		//if the past point is an empty line.
		if (1 == nTextLength && !m_textBlockList[nIndex]->m_bFile)
		{
			delete m_textBlockList[nIndex];
			m_textBlockList.erase(m_textBlockList.begin()+nIndex);
			m_pTextBlockFocused = NULL;
			nIndex--;
		}
	}

	

	if (NULL!=m_pTextBlockFocused)
	{
		//m_pTextBlockFocused->RevokeDragDrop();
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS, 0, 0);
	}
	if (nIndex>0)
	{
		m_pTextBlockFocused = m_textBlockList[nIndex];
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	}

	UINT nFiles = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
	CString strFileName;
	for (UINT i=0; i<nFiles; ++i, ++nIndex)
	{
		DragQueryFile((HDROP)wParam, i, strFileName.GetBuffer(MAX_PATH), MAX_PATH);
		strFileName.ReleaseBuffer();
		CBooguNoteText* pTextBlock = NULL; 

		//CString strRelativePath;
		//PathRelativePathTo(strRelativePath.GetBuffer(MAX_PATH),
		//	m_fileDirectory,
		//	FILE_ATTRIBUTE_NORMAL,
		//	strFileName,
		//	FILE_ATTRIBUTE_NORMAL);
		//strRelativePath.ReleaseBuffer();
		//if (!strRelativePath.IsEmpty())
		//{
		//	strFileName = strRelativePath;
		//}

		pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth-(nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder),strFileName);
		pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		
		pTextBlock->m_bShown = true;
		pTextBlock->m_pTextHandle->state = TEXT_HANDLE_NULL;
		pTextBlock->m_nLevel = nLevel;
		if (nIndex == m_textBlockList.size())
			++nIndex;//adjust to the last element.
		m_nLastClicked = nIndex;
		if (m_textBlockList.size()>0)
		{
			m_textBlockList.insert(m_textBlockList.begin()+ 1 + nIndex, pTextBlock);
		}
		else
		{
			m_textBlockList.push_back(pTextBlock);
		}
		pTextBlock->m_bSelected = false;
		time(&(pTextBlock->m_ModifyTime));
		pTextBlock->DetectFileBlock();
	}

	CalcLayout();
	CalcScrollBar();
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	bHandled = TRUE;
	return 0;
}

LRESULT CBooguNoteView::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetParent().SendMessage(WM_COMMAND, ID_FILE_OPEN, 0);
	return 0;
}

LRESULT CBooguNoteView::OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nPos;
	UINT uId;

    // Call CCoolContextMenu's implementation
    CCoolContextMenu<CBooguNoteView>::OnInitMenuPopup(uMsg, wParam, lParam, bHandled);

	// Set the cursor shape to an arrow
	//HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);
	//ATLASSERT(hCursor);
	::SetCursor(g_util.hCsrArrow);

	CMenuHandle menuPopup = (HMENU)wParam;
	ATLASSERT(menuPopup.m_hMenu != NULL);

    for (nPos = 0; nPos < menuPopup.GetMenuItemCount(); nPos++) 
	{ 
		uId = GetMenuItemID(menuPopup, nPos); 

		EnableMenuItem(menuPopup, uId, MF_BYCOMMAND | MF_ENABLED);
	}

    return 0;
}


LRESULT CBooguNoteView::OnUserRequestResize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_bInitialized && !m_textBlockList.empty())
	{
		//if (!m_bDirty)
		//{
		//	m_bDirty = true;
		//	GetParent().SendMessage(WM_NCPAINT);
		//}
		//else
		//{
			CalcLayout();
			CalcScrollBar();
			Invalidate();
		//}
	}
	return 0;
}

LRESULT CBooguNoteView::OnUserDelClipped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	bool bDel = false;
	for(int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i]->m_bClipped)
		{
			delete m_textBlockList[i];
			m_textBlockList.erase(m_textBlockList.begin()+i);
			--i;
			bDel = true;
		}
	}
	if (m_textBlockList.empty())
	{
		CBooguNoteText* pTextBlock = NULL; 
		if(g_config.bWideTextBlock)
		{
			pTextBlock = CreateTextBlock(0,0,g_config.wideWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_WIDE;
		}
		else
		{
			pTextBlock = CreateTextBlock(0,0,g_config.narrowWidth,_T(""));
			pTextBlock->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		pTextBlock->m_bShown = true;
		m_textBlockList.push_back(pTextBlock);
		//if (NULL!=m_pTextBlockFocused)
			//m_pTextBlockFocused->RevokeDragDrop();
		m_pTextBlockFocused = pTextBlock;
		//m_pTextBlockFocused->RegisterDragDrop();
		m_nLastClicked = 0;
		pTextBlock->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
	}
	if (bDel)
	{
		m_bDirty = true;
		CalcLayout();
		CalcScrollBar();
		//Invalidate();
		GetParent().SendMessage(WM_NCPAINT);
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////

CBooguNoteText* CBooguNoteView::CreateTextBlock(int x, int y, int width, LPCTSTR initString)
{
	CBooguNoteText* pText = NULL;
	HRESULT hr = CreateTextControl(this,this->GetParent(),ES_MULTILINE, _T(""),	x,y,width,1,&pText);
	pText->m_pParentWindow = this;

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("failed to create windowless text control")) ;
	}
	pText->m_pTextHandle = new CBooguNoteHandle(0, 0, pText);
	pText->m_pTextIcon = new CBooguNoteIcon(0, 0, pText);
	//get currect line height.
	if (NULL == initString)
	{
		pText->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(" "), 0);
		pText->GetTextServices()->TxSendMessage(EM_REQUESTRESIZE, 0, 0, 0);
		pText->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
	}
	else
	{
		pText->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)initString, 0);
		pText->GetTextServices()->TxSendMessage(EM_REQUESTRESIZE, 0, 0, 0);
	}
	return pText;
}

void CBooguNoteView::CopyToText()
{
	int nCount = 0;
	int nMinIncident = 10000;
	if (NULL == m_pTextBlockFocused)
	{
		//get selected block count and most left block incident level.
		for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
		{
			if((*ppTextBlock)->m_bSelected)
			{
				++nCount;
				if ((*ppTextBlock)->m_nLevel<nMinIncident)
				{
					nMinIncident = (*ppTextBlock)->m_nLevel;
				}
			}
		}
		if (nCount == 0)
			return;
	}
	else
	{
		nMinIncident= m_pTextBlockFocused->m_nLevel;
		m_pTextBlockFocused->m_bSelected = true;
	}
	/*for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if((*ppTextBlock)->m_bSelected)
		{
			if ((*ppTextBlock)->m_nLevel<nMinIncident)
			{
				nMinIncident = (*ppTextBlock)->m_nLevel;
			}
		}
	}*/
	// Get the currently selected data
typedef TCHAR* PTCHAR;
	TCHAR** BlockBufferList = new PTCHAR[m_textBlockList.size()];
	int nLength = 0;
	int nBlockIndex= 0;
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock)->m_bSelected)
		{
			TCHAR* Buf = NULL;
			int nBlockTextLength = 0;
			if (TEXT_BLOCK_SHRINK==(*ppTextBlock)->m_nExpandState)
			{
				nBlockTextLength = _tcslen((*ppTextBlock)->m_szCache)+1;
				Buf = new TCHAR[nBlockTextLength+1+1];//+1 for '-' and '+'
				_stprintf(Buf, _T(" %s"), (*ppTextBlock)->m_szCache);
			}
			else if ((*ppTextBlock)->m_bFile)
			{
				if (_T('\\') == (*ppTextBlock)->m_szCache[0] && _T('\\') != (*ppTextBlock)->m_szCache[1])
				{
					nBlockTextLength = _tcslen(g_config.szRootStorageDir) + _tcslen((*ppTextBlock)->m_szCache)+1;
					Buf = new TCHAR[nBlockTextLength+1+1];//+1 for '-' and '+'
					_stprintf(Buf, _T(" %s%s"), g_config.szRootStorageDir, (*ppTextBlock)->m_szCache);
				}
				else
				{
					nBlockTextLength = _tcslen((*ppTextBlock)->m_szCache)+1;
					Buf = new TCHAR[nBlockTextLength+1+1];//+1 for '-' and '+'
					_stprintf(Buf, _T(" %s"), (*ppTextBlock)->m_szCache);
				}
			}
			else
			{
				BSTR  bstrCache;
				(*ppTextBlock)->GetTextServices()->TxGetText(&bstrCache);
				nBlockTextLength = SysStringLen(bstrCache);
				Buf = new TCHAR[nBlockTextLength+1+1];//+1 for '-' and '+'
				_stprintf(Buf, _T(" %s"), (LPCTSTR)bstrCache);
				SysFreeString(bstrCache);
			}
			if (g_config.bCopyTextWithSymbol)
			{
				if ((ppTextBlock+1)!=m_textBlockList.end() && (*(ppTextBlock+1))->m_bSelected && (*ppTextBlock)->m_nLevel<(*(ppTextBlock+1))->m_nLevel)
					Buf[0] = _T('+');
				else
					Buf[0] = _T('-');
			}
			else
			{
				Buf[0] = _T(' ');
			}
			//exclude final '\r'.
			if (_T('\r') == Buf[nBlockTextLength-1])
			{
				--nBlockTextLength;
				Buf[nBlockTextLength] = _T('\0');
			}
			//get number of enter key.
			int nEnterCount = 0;
			for(int i=0; i<nBlockTextLength; ++i)
			{
				if (_T('\r') == Buf[i])
				{
					++nEnterCount;
				}
			}
			int nMoreRoom = nEnterCount*((*ppTextBlock)->m_nLevel*g_config.nCopyIndent+1)+2;//+2 means '\r' and '\n'
			if (0!=(*ppTextBlock)->m_nLevel)//for head indent
			{
				nMoreRoom += ((*ppTextBlock)->m_nLevel*g_config.nCopyIndent);
			}
			int nRealBlockTextLength = nBlockTextLength + nMoreRoom + 1;//+1 for null terminal
			TCHAR* BlockTextString = new TCHAR[nRealBlockTextLength];
			int j=0;
			int nIncident = (*ppTextBlock)->m_nLevel - nMinIncident;
			for (int k=0; k<nIncident*g_config.nCopyIndent-1; ++k, ++j)
			{
				BlockTextString[j] = _T(' ');
			}
			/*for(int k=0; k<nIncident; ++k, ++j)
			{
				for (int l=0; l<g_config.nCopyIndent; ++l, ++j)
				{
					BlockTextString[j] = _T(' ');
				}
				--j;
			}*/
			for(int i=0; i<nBlockTextLength; ++i, ++j)
			{
				if (_T('\r') == Buf[i])
				{
					BlockTextString[j] = _T('\r');
					++j;
					BlockTextString[j] = _T('\n');
					++j;
					int nIncident = (*ppTextBlock)->m_nLevel - nMinIncident;
					for(int k=0; k<nIncident; ++k, ++j)
					{
						for (int l=0; l<g_config.nCopyIndent; ++l, ++j)
						{
							BlockTextString[j] = _T(' ');
						}
						--j;
					}
					--j;
					
					//++j;
					//i+=1;
				}
				else
					BlockTextString[j] = Buf[i];
			}
			BlockTextString[j] = _T('\r');
			++j;
			BlockTextString[j] = _T('\n');
			++j;
			BlockTextString[j] = _T('\0');
			//BlockTextString[nRealBlockTextLength-3] = _T('\0');
			BlockBufferList[nBlockIndex] = BlockTextString;
			nLength+= (nRealBlockTextLength-1);//whole length doesn't need null terminal
			delete Buf;
			++nBlockIndex;
		}
	}
	if (0 == nLength)
		return;

	++nLength; //and null termial length;
	HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, nLength*sizeof(TCHAR));
	TCHAR* pData = (TCHAR*)GlobalLock(hGlob);

	for(int i=0, index=0; i<nBlockIndex; ++i)
	{
		_tcscpy((TCHAR*)(pData+index),BlockBufferList[i]);
		index += _tcslen(BlockBufferList[i]);
		//++index;
	}

	
	//memcpy(pData, buf, index);
	GlobalUnlock(hGlob);

	// For the appropriate data formats...
	if ( ::SetClipboardData( CF_UNICODETEXT, hGlob ) == NULL )
	{
		CString msg;
		msg.Format(_T("不能把数据传给剪贴板, 错误号: %d"), GetLastError());
		MessageBox( msg, _T("警告"), MB_OK|MB_ICONSTOP);
		CloseClipboard();
		GlobalFree(hGlob);
	}
	for (int i=0; i< nBlockIndex; ++i)
		delete[] BlockBufferList[i];
	delete[] BlockBufferList;

	if (NULL != m_pTextBlockFocused)
	{
		m_pTextBlockFocused->m_bSelected = false;
	}
}

void CBooguNoteView::CleanAllSelected()
{
	if (NULL!=m_pTextBlockFocused)
	{
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]==m_pTextBlockFocused)
			{
				m_nFocusedTextBlockBeforeKillFocus = i;
				break;
			}
		}

		//m_pTextBlockFocused->RevokeDragDrop();
		//m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_KILLFOCUS,(WPARAM)m_hWnd,0);
		//m_pTextBlockFocused->DetectFileBlock();
	}
	else
	{
		HideCaret();
	}
	bool bMeeted = false;
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if (!bMeeted)
		{
			if ((*ppTextBlock)->m_bSelected)
			{
				m_pTextBlockFocused = (*ppTextBlock);
				bMeeted = true;
			}
		}
		(*ppTextBlock)->m_bSelected = false;
	}
}

void CBooguNoteView::CleanAllClipped()
{
	//bool bMeeted = false;
	for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		//(*ppTextBlock)->m_bSelected = false;
		if ((*ppTextBlock)->m_bClipped)
		{
			//if (!bMeeted)
			//{
			//	m_pTextBlockFocused = (*ppTextBlock);
			//	bMeeted = true;
			//}
			(*ppTextBlock)->m_bClipped = false;
			(*ppTextBlock)->SetDisabled(FALSE);
		}
	}
	Invalidate();
}

void CBooguNoteView::SerializeTextBlock(CBooguNoteText* pTextBlock, XMLElement* r, int minIncident)
{
	XMLElement* node = new XMLElement(r, "<item />");
	TCHAR* Buf = NULL;
	int nBlockTextLength = 0;

	//get content
	if (pTextBlock->m_bFile || TEXT_BLOCK_SHRINK==pTextBlock->m_nExpandState)
	{
		nBlockTextLength = _tcslen(pTextBlock->m_szCache);
		Buf = new TCHAR[nBlockTextLength+1];
		_stprintf(Buf, _T("%s"), pTextBlock->m_szCache);
		
	}
	else
	{
		BSTR  bstrCache;
		pTextBlock->GetTextServices()->TxGetText(&bstrCache);
		nBlockTextLength = SysStringLen(bstrCache);
		Buf = new TCHAR[nBlockTextLength+1];
		_stprintf(Buf, _T("%s"), (LPCTSTR)bstrCache);
		SysFreeString(bstrCache);
		//pTextBlock->get_RTFText(&Buf);
		//exclude final '\r'.
		if (_T('\r') == Buf[nBlockTextLength-1])
		{
			--nBlockTextLength;
			Buf[nBlockTextLength] = _T('\0');
		}
		
	}

	
	XMLVariable* content = new XMLVariable();
	content->SetName("content"/*, 1*/);
	//v->SetValue(W(szEscBuf), 1);
	content->SetValue(W(Buf)/*, 1*/);
	node->AddVariable(content);
	delete[] Buf;
	//delete[] szEscBuf;
	//delete[] szMB;
	
	XMLVariable* icon = new XMLVariable();
	icon->SetName("icon");
	if (TEXT_ICON_PLAINTEXT == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("none");
	}
	else if (TEXT_ICON_TODO == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("flag");
	}
	else if (TEXT_ICON_DONE == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("tick");
	}
	else if (TEXT_ICON_CROSS == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("cross");
	}
	else if (TEXT_ICON_STAR == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("star");
	}
	else if (TEXT_ICON_QUESTION == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("question");
	}
	else if (TEXT_ICON_WARNING == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("warning");
	}
	else if (TEXT_ICON_IDEA == pTextBlock->m_pTextIcon->state)
	{
		icon->SetValue("idea");
	}
	//delete icon;
	//delete v1;

	node->AddVariable(icon);
	
	XMLVariable* branchstate = new XMLVariable();
	branchstate->SetName("branch");
	if (TEXT_HANDLE_NULL == pTextBlock->m_pTextHandle->state)
	{
		branchstate->SetValue("none");
	}
	else if (TEXT_HANDLE_EXPAND == pTextBlock->m_pTextHandle->state)
	{
		branchstate->SetValue("open");
	}
	else if (TEXT_HANDLE_CLOSED == pTextBlock->m_pTextHandle->state)
	{
		branchstate->SetValue("close");
	}
	node->AddVariable(branchstate);
	
	XMLVariable* blockstate = new XMLVariable();
	blockstate->SetName("block");
	if (TEXT_BLOCK_NARROW == pTextBlock->m_nExpandState)
	{
		blockstate->SetValue("narrow");
	}
	else if (TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState)
	{
		blockstate->SetValue("shrink");
	}
	else if (TEXT_BLOCK_WIDE == pTextBlock->m_nExpandState)
	{
		blockstate->SetValue("wide");
	}
	node->AddVariable(blockstate);

	XMLVariable* level = new XMLVariable();
	level->SetName("level", 1);
	level->SetValueInt(pTextBlock->m_nLevel - minIncident);
	node->AddVariable(level);

	XMLVariable* shownstate = new XMLVariable();
	shownstate->SetName("IsShown");
	if (pTextBlock->m_bShown)
	{
		shownstate->SetValue("true");
	}
	else
	{
		shownstate->SetValue("false");
	}
	node->AddVariable(shownstate);

	XMLVariable* bold = new XMLVariable();
	bold->SetName("IsBold");
	if (pTextBlock->m_bBold)
	{
		bold->SetValue("true");
	}
	else
	{
		bold->SetValue("false");
	}
	node->AddVariable(bold);

	XMLVariable* showbranch = new XMLVariable();
	showbranch->SetName("ShowBranch");
	if (pTextBlock->m_bShowBranch)
	{
		showbranch->SetValue("true");
	}
	else
	{
		showbranch->SetValue("false");
	}
	node->AddVariable(showbranch);

	char cBuf[1024];
	XMLVariable* textClr = new XMLVariable();
	textClr->SetName("TextColor");
	BinaryToStringA((BYTE*)&pTextBlock->m_TextClr, sizeof(COLORREF), cBuf);
	textClr->SetValue(cBuf);
	node->AddVariable(textClr);

	XMLVariable* bkgrdClr = new XMLVariable();
	bkgrdClr->SetName("BkgrdColor");
	BinaryToStringA((BYTE*)&pTextBlock->m_BkgrdClr, sizeof(COLORREF), cBuf);
	bkgrdClr->SetValue(cBuf);
	node->AddVariable(bkgrdClr);

	if (0!=pTextBlock->m_ModifyTime)
	{
		XMLVariable* timeModify = new XMLVariable();
		timeModify->SetName("ModifyTime");
		struct tm * timeinfo;
		timeinfo = localtime ( &(pTextBlock->m_ModifyTime));
		sprintf(cBuf, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		timeModify->SetValue(cBuf);
		node->AddVariable(timeModify);
	}

	/*XMLVariable* IsFile = new XMLVariable();
	IsFile->SetName("IsFile");
	if (m_textBlockList[i]->m_bFile)
	{
		IsFile->SetValue("true");
	}
	else
	{
		IsFile->SetValue("false");
	}
	node->AddVariable(IsFile);*/

	r->AddElement(node);
	/*CTextBlock tb;
	tb.m_TextHandleState = pTextBlock->m_pTextHandle->state;
	tb.m_TextBlockState = pTextBlock->m_nExpandState;
	tb.m_TextIcon = pTextBlock->m_pTextIcon->state;
	tb.m_nTextLevel = pTextBlock->m_nLevel;
	tb.m_bShown = pTextBlock->m_bShown;
	tb.m_bBold = pTextBlock->m_bBold;
	tb.m_bShowBranch = pTextBlock->m_bShowBranch;
	BSTR  bstrCache;
	if (pTextBlock->m_bFile || (TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState))
	{
		tb.m_nTextLength = lstrlen(pTextBlock->m_szCache)+1;
	}
	else
	{
		pTextBlock->GetTextServices()->TxGetText(&bstrCache);
		tb.m_nTextLength = SysStringLen(bstrCache);
	}
	nLength = sizeof(CTextBlock) + sizeof(TCHAR)*(tb.m_nTextLength+1);
	pBuf = new BYTE[nLength];
	memcpy(pBuf, (BYTE*)&tb, sizeof(CTextBlock));
	if (pTextBlock->m_bFile || TEXT_BLOCK_SHRINK == pTextBlock->m_nExpandState)
	{
		memcpy(pBuf+sizeof(CTextBlock), (BYTE*)pTextBlock->m_szCache, tb.m_nTextLength*sizeof(TCHAR));
	}
	else
	{
		_stprintf((TCHAR*)(pBuf+sizeof(CTextBlock)), _T("%s"), (LPCTSTR)bstrCache);
		SysFreeString(bstrCache);
	}
	(*((TCHAR*)(pBuf+sizeof(CTextBlock)+(tb.m_nTextLength-1)*sizeof(TCHAR)))) = _T('\0');
	nLength -= sizeof(TCHAR);*/
}

void CBooguNoteView::UnserializeBuffer(BYTE* &pBuf)
{
	
}

void CBooguNoteView::ShrinkTextBlock(CBooguNoteText* pTextBlock)
{
   if(!pTextBlock->m_bFile)
   {
   	BSTR  bstrCache;
   	pTextBlock->GetTextServices()->TxGetText(&bstrCache);
   	int length = SysStringLen(bstrCache);
   
   	int nHeadLength = g_config.nHeadLength;
   		
   	if (NULL != pTextBlock->m_szCache)
   	{
   		delete [] pTextBlock->m_szCache;
   		pTextBlock->m_szCache = NULL;
   	}
   	pTextBlock->m_szCache = new TCHAR[length+1];
   	_stprintf(pTextBlock->m_szCache, _T("%s"), (LPCTSTR)bstrCache);
   	TCHAR a = pTextBlock->m_szCache[length-1];
   	//remove the last return code.
   	if ((_T('\r') == pTextBlock->m_szCache[length-1]) || (_T('\n') == pTextBlock->m_szCache[length-1]))
   		pTextBlock->m_szCache[length-1] = _T('\0');
   	
   	TCHAR* pTemp = new TCHAR[nHeadLength + 100]();
   	int i =0;
   	bool bAddEllipsis = true;
   	if (g_config.nHeadLength<length)
   	{
   		for (; i<nHeadLength; ++i)
   		{
   			if ((_T('\n')!=pTextBlock->m_szCache[i])&&(_T('\r')!=pTextBlock->m_szCache[i]))
   				pTemp[i] = pTextBlock->m_szCache[i];
   			else
   				break;
   		}
   		
   	}
   	else
   	{
   		bool bMeetReturn = false;
   		for (; i<length; ++i)
   		{
   			if ((_T('\n')!=pTextBlock->m_szCache[i])&&(_T('\r')!=pTextBlock->m_szCache[i]))
   				pTemp[i] = pTextBlock->m_szCache[i];
   			else
   			{
   				bMeetReturn = true;
   				break;
   			}
   		}
   		if (!bMeetReturn)
   			bAddEllipsis = false;
   	}
   
   	if (bAddEllipsis)
   	{
   		pTemp[i++] = _T('.');
   		pTemp[i++] = _T('.');
   		pTemp[i++] = _T('.');
   	}
   	
   	if (g_config.bShowCharCountInShrinkTB)
   		_stprintf(&pTemp[i], _T("(%d)\0"), length);
   	else
   		pTemp[i++] = _T('\0');
   	pTextBlock->m_nExpandState = TEXT_BLOCK_SHRINK;
   	//pTextBlock->OnSetFont(g_config.hFontItalic);
   	CHARFORMAT cf;
   	cf.cbSize = sizeof(CHARFORMAT);
   	pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
   	cf.dwEffects |= CFE_UNDERLINE;
   	pTextBlock->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
   	pTextBlock->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)pTemp, 0);
   	RECT* prc = pTextBlock->GetClientRect();
   	CRect rc(prc);
   	HDC hdc = GetDC();
   	//CSize size;
       //GetTextExtentPoint32(hdc, pTemp, lstrlen(pTemp), &size);	
   	//rc.right = rc.left + size.cx;
   	int ilen = lstrlen(pTemp);	
   	int iXpixel = g_config.fontSize *3 /4 *2;
   	rc.right = rc.left + iXpixel*ilen +2;
   	pTextBlock->SetClientRect(&rc, TRUE);
   	pTextBlock->TxWindowProc(m_hWnd, WM_PAINT, (WPARAM)hdc, 0);
   	delete [] pTemp;
   	ReleaseDC(hdc);
   	SysFreeString(bstrCache);
   
   	pTextBlock->SetReadOnly(TRUE);
   }
}

void CBooguNoteView::ShrinkPointTextBlock(CBooguNoteText* pTextBlock)
{
    if(!pTextBlock->m_bFile)
    {
    	BSTR  bstrCache;
    	pTextBlock->GetTextServices()->TxGetText(&bstrCache);
    	int length = SysStringLen(bstrCache);
    
    	int nHeadLength = g_config.nHeadLength;
    		
    	if (NULL != pTextBlock->m_szCache)
    	{
    		delete [] pTextBlock->m_szCache;
    		pTextBlock->m_szCache = NULL;
    	}
    	pTextBlock->m_szCache = new TCHAR[length+1];
    	_stprintf(pTextBlock->m_szCache, _T("%s"), (LPCTSTR)bstrCache);
    	TCHAR a = pTextBlock->m_szCache[length-1];
    	//remove the last return code.
    	if ((_T('\r') == pTextBlock->m_szCache[length-1]) || (_T('\n') == pTextBlock->m_szCache[length-1]))
    		pTextBlock->m_szCache[length-1] = _T('\0');
    	
    	TCHAR* pTemp = new TCHAR[nHeadLength + 100]();
    	int i =0;
    	bool bAddEllipsis = true;
    	if (g_config.nHeadLength<length)
    	{
    		for (; i<nHeadLength; ++i)
    		{
    			if ((_T('\n')!=pTextBlock->m_szCache[i])&&(_T('\r')!=pTextBlock->m_szCache[i]))
    				pTemp[i] = pTextBlock->m_szCache[i];
    			else
    				break;
    		}
    		
    	}
    	else
    	{
    		bool bMeetReturn = false;
    		for (; i<length; ++i)
    		{
    			if ((_T('\n')!=pTextBlock->m_szCache[i])&&(_T('\r')!=pTextBlock->m_szCache[i]))
    				pTemp[i] = pTextBlock->m_szCache[i];
    			else
    			{
    				bMeetReturn = true;
    				break;
    			}
    		}
    		if (!bMeetReturn)
    			bAddEllipsis = false;
    	}
    
    	if (bAddEllipsis)
    	{
    		pTemp[i++] = _T('.');
    		pTemp[i++] = _T('.');
    		pTemp[i++] = _T('.');
    	}
    	
    	if (g_config.bShowCharCountInShrinkTB)
    		_stprintf(&pTemp[i], _T("(%d)\0"), length);
    	else
    		pTemp[i++] = _T('\0');
    	pTextBlock->m_nExpandState = TEXT_BLOCK_SHRINK;
    	//pTextBlock->OnSetFont(g_config.hFontItalic);
    	CHARFORMAT cf;
    	cf.cbSize = sizeof(CHARFORMAT);
    	pTextBlock->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
    	cf.dwEffects |= CFE_UNDERLINE;
    	pTextBlock->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
    	pTextBlock->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)pTemp, 0);
    	RECT* prc = pTextBlock->GetClientRect();
    	CRect rc(prc);
    	HDC hdc = GetDC();
    	//CSize size;
    	//GetTextExtentPoint32(hdc, pTemp, lstrlen(pTemp), &size);
    	//rc.right = rc.left + size.cx;
    	
    	int ilen = lstrlen(pTemp);	
    	int iXpixel = g_config.fontSize *3 /4 *2;
    	rc.right = rc.left + iXpixel*ilen +2;
    	
    	pTextBlock->SetClientRect(&rc, TRUE);
    	pTextBlock->TxWindowProc(m_hWnd, WM_PAINT, (WPARAM)hdc, 0);
    	delete [] pTemp;
    	ReleaseDC(hdc);
    	SysFreeString(bstrCache);
    
    	pTextBlock->SetReadOnly(TRUE);
    }
}

CString CBooguNoteView::GetFileName()
{
	CString fileName = m_fileDirectory;
	fileName.Delete(0, fileName.ReverseFind(_T('\\'))+1);
	return fileName;
}

void CBooguNoteView::CopyToBlock()
{
	int nCount = 0;
	int nMinIncident = 10000;
	if (NULL == m_pTextBlockFocused)
	{
		//get selected block count and most left block incident level.
		for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
		{
			if((*ppTextBlock)->m_bSelected)
			{
				++nCount;
				if ((*ppTextBlock)->m_nLevel<nMinIncident)
				{
					nMinIncident = (*ppTextBlock)->m_nLevel;
				}
			}
		}
		if (nCount == 0)
			return;
	}
	else
	{
		nMinIncident= m_pTextBlockFocused->m_nLevel;
		m_pTextBlockFocused->m_bSelected = true;
	}
	
	//typedef BYTE* PBYTE;
	//BYTE** BufPointerList = new PBYTE[nCount];
	//int* BufLengthList = new int[nCount];

	char szBuf[128];
	sprintf(szBuf, "<root version=\"%d\"/>", BOO_FILE_VERSION);
	XML x;
	XMLElement* r = new XMLElement(0, szBuf);//m_XML->GetRootElement();
	x.SetRootElement(r);
	XMLHeader* h = new XMLHeader(XML_HEADER);
	x.SetHeader(h);

	XMLVariable* dir = new XMLVariable();
	dir->SetName("DefaultSaveDir");
	dir->SetValue(W(m_strDefaultSaveDir));
	r->AddVariable(dir);

	XMLVariable* ext = new XMLVariable();
	ext->SetName("DefaultSaveExtension");
	ext->SetValue(W(m_strDefaultImgExtension));
	r->AddVariable(ext);

	int i = 0;
	//for(vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin(); ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	for(int i = m_textBlockList.size()-1; i>=0; --i)
	{
		if(m_textBlockList[i]->m_bSelected)
		{
			SerializeTextBlock(m_textBlockList[i], r, nMinIncident);
		}
	}

	int nWholeLength = 0;

	size_t M = r->MemoryUsage();
	Z<char> d(M);
	r->Export((FILE*)d.operator char *(),1,XML_SAVE_MODE_DEFAULT,XML_TARGET_MODE_MEMORY);
	size_t S = strlen(d);

	//OpenClipboard(0);
	//EmptyClipboard();

	HGLOBAL hG =
		GlobalAlloc(GMEM_MOVEABLE, S + 10);
	void *pp = GlobalLock(hG);
	//lstrcpyA((char *)pp, d.operator char *());
	strcpy((char*)pp,d.operator char *());
	GlobalUnlock(hG);
	SetClipboardData(BOOGUNOTE_TYPE, hG);
	//CloseClipboard();


	///////////////////////////////////////r->Copy(BOOGUNOTE_TYPE);
	//delete h;
	//delete r;
	if (NULL != m_pTextBlockFocused)
	{
		m_pTextBlockFocused->m_bSelected = false;
	}
	/*for(int j=0; j<nCount; ++j)
	{
		nWholeLength += BufLengthList[j];
	}
	nWholeLength += sizeof(int);
	BYTE* pBuf = new BYTE[nWholeLength];
	(*((int*)pBuf)) = nCount;
	int index = sizeof(int);
	for (int j=0; j<nCount; ++j)
	{
		memcpy(pBuf+index, BufPointerList[j], BufLengthList[j]);
		index += BufLengthList[j];
	}*/
	//HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, nWholeLength);
	//BYTE* pData = (BYTE*)GlobalLock(hGlob);
	//memcpy(pData, pBuf, nWholeLength);
	//GlobalUnlock(hGlob);
	//delete pBuf;
	//for (int i=0; i< nCount; ++i)
	//	delete[] BufPointerList[i];
	//delete[] BufPointerList;
	//delete[] BufLengthList;

	// For the appropriate data formats...
	//if ( ::SetClipboardData( BOOGUNOTE_TYPE, hGlob ) == NULL )
	//{
	//	CString msg;
	//	msg.Format(_T("不能把数据传给剪贴板, 错误号: %d"), GetLastError());
	//	MessageBox( msg, _T("警告"), MB_OK|MB_ICONSTOP);
	//	CloseClipboard();
	//	GlobalFree(hGlob);
	//}
}

LRESULT CBooguNoteView::OnSelectAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bHandled = TRUE;
	m_pTextBlockFocused = NULL;
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		m_textBlockList[i]->m_bSelected = true;
	}
	Invalidate();
	return 0;
}

LRESULT CBooguNoteView::OnSwitch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bHandled = TRUE;
	if (NULL == m_pTextBlockFocused)
	{
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected && m_textBlockList[i]->m_bShown)
			{
				SwitchState(m_textBlockList[i]);
			}

		}
	}
	else
	{
		SwitchState(m_pTextBlockFocused);
	}
	CalcLayout();
	CalcScrollBar();
	Invalidate();
	return 0;
}

void CBooguNoteView::SwitchState(CBooguNoteText* pTB)
{
	if (g_config.bEnterURLDetect)
	{
	   pTB->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0); 
	}
	else
	{
	   pTB->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, FALSE, 0, 0); 
	}
	
	if (TEXT_BLOCK_NARROW == pTB->m_nExpandState)
	{
		if (pTB->m_bFile && NULL != pTB->m_pBitmap)
		{
			Image tempImage(pTB->m_szCache);
			//if (tempImage.GetLastStatus() != Ok)
			if (tempImage.GetLastStatus() != Ok)
			{
			}
			else
			{
				pTB->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
				CSize sz(0,0);
				if (tempImage.GetHeight()>g_config.nBigThumbnailHeight)
				{
					sz.cy = g_config.nBigThumbnailHeight;
					sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
				}
				else
				{
					sz.cx = tempImage.GetWidth();
					sz.cy = tempImage.GetHeight();
				}
				delete pTB->m_pBitmap;
				pTB->m_pBitmap = new Bitmap(sz.cx, sz.cy);
				Graphics imageGraphics(pTB->m_pBitmap);
				imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
			}
			pTB->m_nExpandState = TEXT_BLOCK_WIDE;
		}
		else
		{
			if (g_config.bEnterWideTextBlock)
			{
				RECT* prc = pTB->GetClientRect();
				CRect rc(prc);
				rc.right = rc.left + g_config.wideWidth;
				pTB->SetClientRect(&rc);
				pTB->SetReadOnly(FALSE);
				pTB->m_nExpandState = TEXT_BLOCK_WIDE;
			}
			else
			{
				ShrinkTextBlock(pTB);
			}
		}
		
	}
	else if (TEXT_BLOCK_WIDE == pTB->m_nExpandState)
	{
		if (pTB->m_bFile)
		{
			if (NULL != pTB->m_pBitmap)
			{
				Image tempImage(pTB->m_szCache);
				//if (tempImage.GetLastStatus() != Ok)
				if (tempImage.GetLastStatus() != Ok)
				{
				}
				else
				{
					pTB->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
					CSize sz(0,0);
					if (tempImage.GetHeight()>g_config.nSmallThumbnailHeight)
					{
						sz.cy = g_config.nSmallThumbnailHeight;
						sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
					}
					else
					{
						sz.cx = tempImage.GetWidth();
						sz.cy = tempImage.GetHeight();
					}
					delete pTB->m_pBitmap;
					pTB->m_pBitmap = new Bitmap(sz.cx, sz.cy);
					Graphics imageGraphics(pTB->m_pBitmap);
					imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
				}
			}
			else
			{
				RECT* prc = pTB->GetClientRect();
				CRect rc(prc);
				rc.right = rc.left + g_config.narrowWidth-(pTB->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
				pTB->SetClientRect(&rc);
			}
			pTB->m_nExpandState = TEXT_BLOCK_NARROW;
		}
		else
		{
			ShrinkTextBlock(pTB);
		}
	}
	else if (TEXT_BLOCK_SHRINK == pTB->m_nExpandState)
	{
		RECT* prc = pTB->GetClientRect();
		CRect rc(prc);
		rc.right = rc.left + g_config.narrowWidth-(pTB->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
		pTB->SetClientRect(&rc);
		pTB->m_nExpandState = TEXT_BLOCK_NARROW;
		CHARFORMAT cf;
		cf.cbSize = sizeof(CHARFORMAT);
		pTB->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
		cf.dwEffects &= ~CFE_UNDERLINE;
		pTB->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
		pTB->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)pTB->m_szCache, 0);
		pTB->SetReadOnly(FALSE);
		if (NULL != pTB->m_szCache)
		{
			delete []pTB->m_szCache;
			pTB->m_szCache = NULL;
		}
	}
}

void CBooguNoteView::NarrowPointTextBlock(CBooguNoteText* pTB)
{
	if (g_config.bEnterURLDetect)
	{
	   pTB->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0); 
	}
	else
	{
	   pTB->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, FALSE, 0, 0); 
	}
	
	if (TEXT_BLOCK_NARROW == pTB->m_nExpandState)
	{		
	}
	else if (TEXT_BLOCK_WIDE == pTB->m_nExpandState
	    || TEXT_BLOCK_SHRINK == pTB->m_nExpandState)
	{
		RECT* prc = pTB->GetClientRect();
		CRect rc(prc);
		rc.right = rc.left + g_config.narrowWidth-(pTB->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
		pTB->SetClientRect(&rc);
		pTB->m_nExpandState = TEXT_BLOCK_NARROW;
		CHARFORMAT cf;
		cf.cbSize = sizeof(CHARFORMAT);
		pTB->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
		cf.dwEffects &= ~CFE_UNDERLINE;
		pTB->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
		pTB->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)pTB->m_szCache, 0);
		pTB->SetReadOnly(FALSE);
		if (NULL != pTB->m_szCache)
		{
			delete []pTB->m_szCache;
			pTB->m_szCache = NULL;
		}
	}
}

LRESULT CBooguNoteView::OnIconPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pTextBlockFocused!=NULL)
	{
		if (m_pTextBlockFocused->m_pTextIcon->state != TEXT_ICON_BEGAIN)
		{
			m_pTextBlockFocused->m_pTextIcon->state--;
		}
		else
		{
			m_pTextBlockFocused->m_pTextIcon->state = TEXT_ICON_END;
		}
	}
	m_bDirty = true;
	CalcLayout();
	GetParent().SendMessage(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnIconNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pTextBlockFocused!=NULL)
	{
		if (m_pTextBlockFocused->m_pTextIcon->state != TEXT_ICON_END)
		{
			m_pTextBlockFocused->m_pTextIcon->state++;
		}
		else
		{
			m_pTextBlockFocused->m_pTextIcon->state = TEXT_ICON_BEGAIN;
		}
	}
	m_bDirty = true;
	CalcLayout();
	GetParent().SendMessage(WM_NCPAINT);
	return 0;
}
LRESULT CBooguNoteView::OnPopupmenuBranch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (NULL != m_pTextBlockFocused)
		m_pTextBlockFocused->m_bSelected = true;

	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i]->m_bSelected)
		{
			m_textBlockList[i]->m_bShowBranch = true;
		}
	}

	if (NULL != m_pTextBlockFocused)
		m_pTextBlockFocused->m_bSelected = false;

	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);

	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuUnbranch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (NULL != m_pTextBlockFocused)
		m_pTextBlockFocused->m_bSelected = true;

	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i]->m_bSelected)
		{
			m_textBlockList[i]->m_bShowBranch = false;
		}
	}

	if (NULL != m_pTextBlockFocused)
		m_pTextBlockFocused->m_bSelected = false;

	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);

	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuExpandall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	StateNarrow();
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i] == m_pTextBlockFocused)
		{
			if (m_textBlockList[i]->m_pTextHandle->state != TEXT_HANDLE_NULL)
			{
				int nLevel = m_pTextBlockFocused->m_nLevel;
				++i;
				for ( ; i<m_textBlockList.size(); ++i)
				{
					if (m_textBlockList[i]->m_nLevel > nLevel)
					{
						if (m_textBlockList[i]->m_pTextHandle->state == TEXT_HANDLE_CLOSED)
							m_textBlockList[i]->m_pTextHandle->state = TEXT_HANDLE_EXPAND;
						m_textBlockList[i]->m_bShown = true;
					}
				}
			}
			break;
		}
	}

	CalcLayout();
	Invalidate();

	return 0;
}

LRESULT CBooguNoteView::OnOperationJumptoparentnode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (NULL != m_pTextBlockFocused)
	{
		int nIndex = GetCurrFocusedBlock();
		int nCurrLevel = m_textBlockList[nIndex]->m_nLevel;
		if (0 != nCurrLevel && 0 != nIndex)
		{
			for(int i = nIndex; i>=0; --i)
			{
				if ((nCurrLevel-1) == m_textBlockList[i]->m_nLevel)
				{
					m_pTextBlockFocused = m_textBlockList[i];
					SetCaretPos(m_pTextBlockFocused->GetClientRect()->left - m_ptOffset.x, m_pTextBlockFocused->GetClientRect()->top - m_ptOffset.y);
					Invalidate();
					break;
				}
			}
		}
	}
	AutoAdjustFocusedBlockPosition();

	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuTextcolor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTabButton;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	
	if (NULL!=m_pTextBlockFocused)
	{
		if (TEXT_BLOCK_SHRINK != m_pTextBlockFocused->m_nExpandState && !m_pTextBlockFocused->m_bFile)
		{
			if (ChooseColor(&cc)==TRUE) 
			{
				CHARFORMAT2 cf;
				cf.cbSize = sizeof(CHARFORMAT2);
				m_pTextBlockFocused->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				cf.dwMask = CFM_COLOR;
				cf.dwEffects &= ~CFE_AUTOCOLOR;
				cf.crTextColor = cc.rgbResult;
				m_pTextBlockFocused->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
				m_bDirty = true;
				m_pTextBlockFocused->m_TextClr = cc.rgbResult;
				GetParent().SendMessageW(WM_NCPAINT);
			}
		}
	}
	else
	{
		if (ChooseColor(&cc)==TRUE) 
		{
			CHARFORMAT2 cf;
			cf.cbSize = sizeof(CHARFORMAT2);
			
			for (int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i]->m_bSelected && (TEXT_BLOCK_SHRINK != m_textBlockList[i]->m_nExpandState && !m_textBlockList[i]->m_bFile))
				{
					m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwMask = CFM_COLOR;
					cf.dwEffects &= ~CFE_AUTOCOLOR;
					cf.crTextColor = cc.rgbResult;
					m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					m_textBlockList[i]->m_TextClr = cc.rgbResult;
				}
			}
			m_bDirty = true;
			GetParent().SendMessageW(WM_NCPAINT);
		}
	}

	return 0;

	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuBkgrdcolor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	//HWND hwnd;                      // owner window
	//HBRUSH hbrush;                  // brush handle
	DWORD rgbCurrent;        // initial color selection

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = g_config.clrTabButton;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	
	if (NULL!=m_pTextBlockFocused)
	{
		if (TEXT_BLOCK_SHRINK != m_pTextBlockFocused->m_nExpandState && !m_pTextBlockFocused->m_bFile)
		{
			if (ChooseColor(&cc)==TRUE) 
			{
				CHARFORMAT2 cf;
				cf.cbSize = sizeof(CHARFORMAT2);
				m_pTextBlockFocused->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				cf.dwMask = CFM_BACKCOLOR;
				cf.dwEffects &= ~CFE_AUTOBACKCOLOR;
				cf.crBackColor = cc.rgbResult;
				m_pTextBlockFocused->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
				m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
				m_bDirty = true;
				m_pTextBlockFocused->m_BkgrdClr = cc.rgbResult;
				GetParent().SendMessageW(WM_NCPAINT);
			}
		}
	}
	else
	{
		if (ChooseColor(&cc)==TRUE) 
		{
			CHARFORMAT2 cf;
			cf.cbSize = sizeof(CHARFORMAT2);
			
			for (int i=0; i<m_textBlockList.size(); ++i)
			{
				if (m_textBlockList[i]->m_bSelected && (TEXT_BLOCK_SHRINK != m_textBlockList[i]->m_nExpandState && !m_textBlockList[i]->m_bFile))
				{
					m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwMask = CFM_BACKCOLOR;
					cf.dwEffects &= ~CFE_AUTOBACKCOLOR;
					cf.crBackColor = cc.rgbResult;
					m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					m_textBlockList[i]->m_BkgrdClr = cc.rgbResult;
				}
			}
			m_bDirty = true;
			GetParent().SendMessageW(WM_NCPAINT);
		}
	}

	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuMovefiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	vector<CBooguNoteText*> moveList;
	if (NULL != m_pTextBlockFocused && m_pTextBlockFocused->m_bFile)
	{
		moveList.push_back(m_pTextBlockFocused);
	}
	else
	{
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected && m_textBlockList[i]->m_bFile)
			{
				moveList.push_back(m_textBlockList[i]);
			}
		}
	}
	if (moveList.size()>0)
	{
		CFolderDialog fd(NULL, NULL, BIF_USENEWUI);
		fd.SetInitialFolder(g_config.szRootStorageDir);
		if (IDOK == fd.DoModal())
		{
			CString tempStorageRootStr(g_config.szRootStorageDir);
			tempStorageRootStr.MakeLower();

			int nRootStorageDirLen = _tcsclen(g_config.szRootStorageDir);

			LPITEMIDLIST pidlTarget	= fd.GetSelectedItem();
			TCHAR pszPath[65536];
			SHGetPathFromIDList(pidlTarget, pszPath);   // Make sure it is a path
			CString folderPath = pszPath;

			if (L'\\' != folderPath.Right(1))
			{
				folderPath += L'\\';
			}

			for(int i=0; i<moveList.size(); ++i)
			{
				CString filePath;
				if (L'\\' == moveList[i]->m_szCache[0])
				{
					filePath = g_config.szRootStorageDir;
					filePath += moveList[i]->m_szCache;
				}
				else if (L'.' == moveList[i]->m_szCache[0])
				{
					if (!m_fileDirectory.IsEmpty())
					{
						CString strFolder = m_fileDirectory;
						PathRemoveFileSpec(strFolder.GetBuffer(MAX_PATH));
						strFolder.ReleaseBuffer();
						PathCombine(filePath.GetBuffer(MAX_PATH), strFolder, moveList[i]->m_szCache);
						filePath.ReleaseBuffer();
					}
				}
				else
				{
					filePath = moveList[i]->m_szCache;
				}

				CString filePathBeforeMove = filePath;
				filePath.Delete(0, filePath.ReverseFind(_T('\\'))+1);
				CString filePathAfterMove = folderPath + filePath;

				if (filePathBeforeMove != filePathAfterMove)
				{
					TCHAR* szFrom = new TCHAR[1024*1024];
					_stprintf(szFrom, _T("%s*\0\0"), filePathBeforeMove.GetBuffer(_MAX_PATH + 100));
					filePathBeforeMove.ReleaseBuffer();
					TCHAR* szTo = new TCHAR[1024*1024];
					CString tempFolderPath = folderPath;
					if (tempFolderPath.Right(1) == _T('\\'))
					{
						tempFolderPath.TrimRight(_T('\\'));
					}
					_stprintf(szTo, _T("%s\0\0"), tempFolderPath.GetBuffer(_MAX_PATH + 100));
					tempFolderPath.ReleaseBuffer();
					SHFILEOPSTRUCT shfo;
					shfo.hwnd = m_hWnd;
					shfo.lpszProgressTitle = szFrom;
					shfo.wFunc = FO_MOVE;
					shfo.fFlags = FOF_MULTIDESTFILES;
					shfo.pTo = szTo;
					shfo.pFrom = szFrom;
					
					int iRC = SHFileOperation(&shfo);

					delete [] szFrom;
					delete [] szTo;

					if (!shfo.fAnyOperationsAborted)
					{

						//filePath.Delete(0, filePath.ReverseFind(_T('\\'))+1);
						//filePath = folderPath + filePath;
						
						
						CString tempfileDirectory = filePathAfterMove;
						tempfileDirectory.MakeLower();
						if (0 == tempfileDirectory.Find(tempStorageRootStr))
						{
							filePathAfterMove.Delete(0, nRootStorageDirLen);	
						}
						delete [] moveList[i]->m_szCache;
						moveList[i]->m_szCache = new TCHAR[filePathAfterMove.GetLength()+100];
						_stprintf(moveList[i]->m_szCache, _T("%s"), filePathAfterMove.GetBuffer(MAX_PATH));
						filePathAfterMove.ReleaseBuffer();
					}
					else
					{
						break;
					}
				}
			}
			m_bDirty = true;
			GetParent().SendMessage(WM_NCPAINT);
		}
	}

	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuCopyfiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	vector<CBooguNoteText*> moveList;
	if (NULL != m_pTextBlockFocused && m_pTextBlockFocused->m_bFile)
	{
		moveList.push_back(m_pTextBlockFocused);
	}
	else
	{
		for (int i=0; i<m_textBlockList.size(); ++i)
		{
			if (m_textBlockList[i]->m_bSelected && m_textBlockList[i]->m_bFile)
			{
				moveList.push_back(m_textBlockList[i]);
			}
		}
	}
	if (moveList.size()>0)
	{
		int nLen = 0;
		int nLenRoot = lstrlen(g_config.szRootStorageDir);
		for (int i=0; i<moveList.size(); ++i)
		{
			nLen = nLen + MAX_PATH/*lstrlen(moveList[i]->m_szCache)*/ + nLenRoot + 10;
		}
		DROPFILES dobj = { 20, { 0, 0 }, 0, 1 };
		int nGblLen = sizeof(dobj) + nLen + 5;//lots of nulls and multibyte_char
		HGLOBAL hGbl = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE, nGblLen);
		char* sData = (char*)::GlobalLock(hGbl);
		memcpy( sData, &dobj, 20 );
		wchar_t* sWStr = (wchar_t*)(sData+20);
		for( int i = 0; i < moveList.size(); ++i )
		{
			CString filePath;
			if (L'\\' == moveList[i]->m_szCache[0])
			{
				filePath = g_config.szRootStorageDir;
				filePath += moveList[i]->m_szCache;
			}
			else if (L'.' == moveList[i]->m_szCache[0])
			{
				if (!m_fileDirectory.IsEmpty())
				{
					CString strFolder = m_fileDirectory;
					PathRemoveFileSpec(strFolder.GetBuffer(MAX_PATH));
					strFolder.ReleaseBuffer();
					PathCombine(filePath.GetBuffer(MAX_PATH), strFolder, moveList[i]->m_szCache);
					filePath.ReleaseBuffer();
				}
			}
			else
			{
				filePath = moveList[i]->m_szCache;
			}
			wcscpy(sWStr, filePath.GetBuffer(0));
			filePath.ReleaseBuffer();
			sWStr += filePath.GetLength();
			(*sWStr) = L'\0';
			++sWStr;
		}
		(*sWStr) = L'\0';
		::GlobalUnlock(hGbl);

		if( OpenClipboard() )
		{
			EmptyClipboard();
			SetClipboardData( CF_HDROP, hGbl );
			CloseClipboard();
		}
	}

	return 0;
}


LRESULT CBooguNoteView::OnPopupmenuOpenBooInExplorer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!m_fileDirectory.IsEmpty())
	{
		CString str = _T(" /e, /select,\"");
		str += m_fileDirectory;
		str += _T("\"");
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), str, NULL, SW_SHOWNORMAL);
	}
	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuExtract(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	vector<CBooguNoteText*> textBlockList;
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if (m_textBlockList[i]->m_bSelected || m_textBlockList[i] == m_pTextBlockFocused)
		{
			textBlockList.push_back(m_textBlockList[i]);
		}
	}
	
	if (!textBlockList.empty())
	{
		//调整缩进
		int nFirstBlockIndent = textBlockList[0]->m_nLevel;
		for (int j=0; j<textBlockList.size(); j++)
		{
			int nIndent = textBlockList[j]->m_nLevel;
			int i=j;//要从自己开始
			for (; i<textBlockList.size(); ++i)
			{
				int a1 = nIndent;
				int a2 = textBlockList[i]->m_nLevel;
				if (a1<=a2)
				{
					textBlockList[i]->m_nLevel -= nIndent;
				}
				else
				{
					break;
				}
			}
			if (i == textBlockList.size())
			{
				j=i;
			}
			else
			{
				i--;
				j = i;
			}
		}
		
	
		//得到第一行文字
		int nBlockTextLength = 0;
		TCHAR* Buf=NULL;
		if (textBlockList[0]->m_bFile || TEXT_BLOCK_SHRINK==textBlockList[0]->m_nExpandState)
		{
			nBlockTextLength = _tcslen(textBlockList[0]->m_szCache);
			Buf = new TCHAR[nBlockTextLength+1];
			_stprintf(Buf, _T("%s"), textBlockList[0]->m_szCache);
		}
		else
		{
			BSTR  bstrCache;
			textBlockList[0]->GetTextServices()->TxGetText(&bstrCache);
			nBlockTextLength = SysStringLen(bstrCache);
			Buf = new TCHAR[nBlockTextLength+100];
			_stprintf(Buf, _T("%s"), (LPCTSTR)bstrCache);
			SysFreeString(bstrCache);
		}
		//exclude final '\r'.
		if (_T('\r') == Buf[nBlockTextLength-1])
		{
			--nBlockTextLength;
			Buf[nBlockTextLength] = _T('\0');
		}
		CString strFileName = Buf;
		if (NULL != Buf)
		{
			delete []Buf;
		}

	// 	PathAddExtension(strFileName.GetBuffer(MAX_PATH), L".boo");
	// 	strFileName.ReleaseBuffer();

		strFileName.Replace(_T('\\'),_T('_'));
		strFileName.Replace(_T('\/'),_T('_'));
		strFileName.Replace(_T(':'),_T('_'));
		strFileName.Replace(_T('*'),_T('_'));
		strFileName.Replace(_T('?'),_T('_'));
		strFileName.Replace(_T('"'),_T('_'));
		strFileName.Replace(_T('<'),_T('_'));
		strFileName.Replace(_T('>'),_T('_'));
		strFileName.Replace(_T('|'),_T('_'));
		strFileName.Replace(_T('~'),_T('_'));
		strFileName.Replace(_T('\r'),_T(' '));
		strFileName.Replace(_T('\n'),_T(' '));

		CString fileDirectory;
		CFileDialogFilter strFilter(_T("BooguNote文件 (*.boo)"));
		CFileDialog fd(FALSE, _T("boo"), strFileName.Left(50),   OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, strFilter);
		//fd.m_ofn.Flags = OFN_DONTADDTORECENT|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST;
		if (IDCANCEL == fd.DoModal())
		{
			return 1;
		}
		fileDirectory = fd.m_szFileName;

		int ret = SaveBooFile(textBlockList, fileDirectory.GetBuffer(_MAX_PATH), false);

		//建立文件节点
		textBlockList[0]->m_nLevel = nFirstBlockIndent;
		textBlockList[0]->m_bShown = true;
		textBlockList[0]->m_pTextHandle->state = TEXT_HANDLE_NULL;
		textBlockList[0]->m_bSelected = false;
		textBlockList[0]->m_bFile = false;
		textBlockList[0]->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)((LPCTSTR)fileDirectory), 0);
		textBlockList[0]->GetTextServices()->TxSendMessage(EM_REQUESTRESIZE, 0, 0, 0);
		textBlockList[0]->DetectFileBlock();

		if (textBlockList.size()>1)
		{
			DeleteSelectedBlock();
		}

		m_pTextBlockFocused = textBlockList[0];
		m_nLastClicked = 0;
		m_pTextBlockFocused->TxWindowProc(m_hWnd, WM_SETFOCUS, 0, 0);
		
		m_bDirty = true;

		CalcLayout();
		CalcScrollBar();
		GetParent().SendMessageW(WM_NCPAINT);
		GetParent().PostMessage(WM_USER_REFRESH_FILE_TREE_VIEW);
		
	}
	return 0;
}

LRESULT CBooguNoteView::OnUserGetFileDir(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return (LRESULT)&m_fileDirectory;
}

LRESULT CBooguNoteView::OnPopupmenuDefaultSaveDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CaptureSetting();
	return 0;
}

int CBooguNoteView::CaptureSetting()
{
	CDefaultSaveDirDlg dlg;
	dlg.m_strText = m_strDefaultSaveDir;
	dlg.m_strExtension = m_strDefaultImgExtension;
	if (IDOK == dlg.DoModal())
	{
		m_strDefaultSaveDir = dlg.m_strText;
		m_strDefaultImgExtension = dlg.m_strExtension;
		PathAddBackslash(m_strDefaultSaveDir.GetBuffer(MAX_PATH));
		m_strDefaultSaveDir.ReleaseBuffer();
		Save();
	}
    return 0;
}

LRESULT CBooguNoteView::OnPopupmenuRelativeDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if ((m_textBlockList[i]->m_bSelected || m_textBlockList[i] == m_pTextBlockFocused) && m_textBlockList[i]->m_bFile && L'.' != m_textBlockList[i]->m_szCache[0])
		{
			CString filePath;
			if (L'\\' ==m_textBlockList[i]->m_szCache[0])
			{
				filePath = g_config.szRootStorageDir;
				filePath += m_textBlockList[i]->m_szCache;
			}
			else
			{
				filePath = m_textBlockList[i]->m_szCache;
			}
			CString strRelativePath;
			PathRelativePathTo(strRelativePath.GetBuffer(MAX_PATH),
				m_fileDirectory,
				FILE_ATTRIBUTE_NORMAL,
				filePath,
				FILE_ATTRIBUTE_NORMAL);
			strRelativePath.ReleaseBuffer();
			if (!strRelativePath.IsEmpty())
			{
				filePath = strRelativePath;
			}
			delete [] m_textBlockList[i]->m_szCache;
			m_textBlockList[i]->m_szCache = new TCHAR[filePath.GetLength()+100];
			_stprintf(m_textBlockList[i]->m_szCache, _T("%s"), filePath.GetBuffer(MAX_PATH));
			filePath.ReleaseBuffer();
		}
	}
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}

LRESULT CBooguNoteView::OnPopupmenuAbsoluteDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	for (int i=0; i<m_textBlockList.size(); ++i)
	{
		if ((m_textBlockList[i]->m_bSelected || m_textBlockList[i] == m_pTextBlockFocused) && m_textBlockList[i]->m_bFile && L'.' == m_textBlockList[i]->m_szCache[0])
		{
			CString filePath = m_fileDirectory;
			PathRemoveFileSpec(filePath.GetBuffer(MAX_PATH));
			filePath.ReleaseBuffer();
			if (!filePath.IsEmpty())
			{
				CString strBooFilePath = filePath;
				PathCombine(filePath.GetBuffer(MAX_PATH), strBooFilePath, m_textBlockList[i]->m_szCache);
				filePath.ReleaseBuffer();
			}

			//Detect Storage Root
			int nRootStorageDirLen = _tcsclen(g_config.szRootStorageDir);
			if (nRootStorageDirLen>0)
			{
				CString fileDirectory = filePath;
				CString tempStorageRootStr(g_config.szRootStorageDir);
				tempStorageRootStr.MakeLower();
				if (_T('\\')!=tempStorageRootStr[tempStorageRootStr.GetLength()-1])
				{
					tempStorageRootStr += _T("\\");
				}
				CString tempfileDirectory = fileDirectory;
				tempfileDirectory.MakeLower();
				if (0 == tempfileDirectory.Find(tempStorageRootStr))
				{

					fileDirectory.Delete(0, nRootStorageDirLen);
					_stprintf(filePath.GetBuffer(MAX_PATH), _T("%s"), fileDirectory);
					filePath.ReleaseBuffer();
				}
			}

			delete [] m_textBlockList[i]->m_szCache;
			m_textBlockList[i]->m_szCache = new TCHAR[filePath.GetLength()+100];
			_stprintf(m_textBlockList[i]->m_szCache, _T("%s"), filePath.GetBuffer(MAX_PATH));
			filePath.ReleaseBuffer();
		}
	}
	m_bDirty = true;
	GetParent().SendMessageW(WM_NCPAINT);
	return 0;
}

LRESULT CBooguNoteView::OnTextColor(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	COLOR_PAIR PreDefineColor[] = 
	{
		{ ID_POPUPMENU_COLOR_B_0, RGB(255, 0, 0), RGB(255, 255, 255) },
		{ ID_POPUPMENU_COLOR_B_1, RGB(250, 125, 0), RGB(255, 255, 255) },
		{ ID_POPUPMENU_COLOR_B_2, RGB(0, 128, 0), RGB(255, 255, 255) },
		{ ID_POPUPMENU_COLOR_B_3, RGB(0, 0, 255), RGB(255, 255, 255) },
		{ ID_POPUPMENU_COLOR_B_4, RGB(0, 0, 0), RGB(255, 255, 255) },
		{ ID_POPUPMENU_COLOR_B_5, RGB(192, 192, 192), RGB(255, 255, 255) },

		{ ID_POPUPMENU_COLOR_A_0, RGB(0, 97, 0), RGB(198, 239, 206) },
		{ ID_POPUPMENU_COLOR_A_1, RGB(156, 0, 6), RGB(255, 199, 206) },
		{ ID_POPUPMENU_COLOR_A_2, RGB(156, 101, 0), RGB(255, 235, 156) },
		{ ID_POPUPMENU_COLOR_A_3, RGB(63, 63, 118), RGB(255, 204, 153) },

		{ ID_POPUPMENU_COLOR_C_0, RGB(255, 255, 255), RGB(79, 129, 189) },
		{ ID_POPUPMENU_COLOR_C_1, RGB(255, 255, 255), RGB(192, 80, 77) },
		{ ID_POPUPMENU_COLOR_C_2, RGB(255, 255, 255), RGB(155, 187, 89) },
		{ ID_POPUPMENU_COLOR_C_3, RGB(255, 255, 255), RGB(128, 100, 162) },
		{ ID_POPUPMENU_COLOR_C_4, RGB(255, 255, 255), RGB(75, 172, 198) },
		{ ID_POPUPMENU_COLOR_C_5, RGB(255, 255, 255), RGB(247, 150, 70) },

		{ ID_POPUPMENU_COLOR_D_0, RGB(0, 0, 0), RGB(184, 204, 228) },
		{ ID_POPUPMENU_COLOR_D_1, RGB(0, 0, 0), RGB(230, 184, 183) },
		{ ID_POPUPMENU_COLOR_D_2, RGB(0, 0, 0), RGB(216, 228, 188) },
		{ ID_POPUPMENU_COLOR_D_3, RGB(0, 0, 0), RGB(204, 192, 218) },
		{ ID_POPUPMENU_COLOR_D_4, RGB(0, 0, 0), RGB(183, 222, 232) },
		{ ID_POPUPMENU_COLOR_D_5, RGB(0, 0, 0), RGB(252, 213, 180) },
	};

	for (int nColorIndex=0; nColorIndex<sizeof(PreDefineColor)/sizeof(COLOR_PAIR); nColorIndex++)
	{
		if (((unsigned int)wID) == PreDefineColor[nColorIndex].uMsgId)
		{
			CHARFORMAT2 cf;
			cf.cbSize = sizeof(CHARFORMAT2);

			for (int i=0; i<m_textBlockList.size(); ++i)
			{
				if ((m_textBlockList[i]->m_bSelected || m_textBlockList[i] == m_pTextBlockFocused) && (TEXT_BLOCK_SHRINK != m_textBlockList[i]->m_nExpandState && !m_textBlockList[i]->m_bFile))
				{
					m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwMask = CFM_COLOR | CFM_BACKCOLOR;
					cf.dwEffects &= ~(CFE_AUTOCOLOR | CFE_AUTOBACKCOLOR);
					cf.crTextColor = PreDefineColor[nColorIndex].TextColor;
					cf.crBackColor = PreDefineColor[nColorIndex].BkgColor;
					m_textBlockList[i]->GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					m_textBlockList[i]->m_TextClr = PreDefineColor[nColorIndex].TextColor;
					m_textBlockList[i]->m_BkgrdClr = PreDefineColor[nColorIndex].BkgColor;
				}
			}
			m_bDirty = true;
			GetParent().SendMessageW(WM_NCPAINT);
			break;
		}
	}
	
	return 0;
}


BOOL CDefaultSaveDirDlg::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	m_cbFileType = GetDlgItem(IDC_COMBO_EXTENSION);
	m_cbFileType.InsertString(0,_T("tiff"));
	m_cbFileType.InsertString(0,_T("gif"));
	m_cbFileType.InsertString(0,_T("jpg"));
	m_cbFileType.InsertString(0,_T("png"));

	if (L"png" == m_strExtension)
	{
		m_cbFileType.SetCurSel(0);
	}
	else if (L"jpg" == m_strExtension)
	{
		m_cbFileType.SetCurSel(1);
	}
	else if (L"gif" == m_strExtension)
	{
		m_cbFileType.SetCurSel(2);
	}
	else if (L"tiff" == m_strExtension)
	{
		m_cbFileType.SetCurSel(3);
	}
	else
	{
		m_cbFileType.SetCurSel(0);
	}

	DoDataExchange(false);
	return TRUE;
}
LRESULT CDefaultSaveDirDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(true);
	if (m_strText.IsEmpty() || PathIsDirectory(m_strText))
	{
		int nSaveType =m_cbFileType.GetCurSel();
		if (0 == nSaveType)
		{
			m_strExtension = L"png";
		}
		else if (1 == nSaveType)
		{
			m_strExtension = L"jpg";
		}
		if (2 == nSaveType)
		{
			m_strExtension = L"gif";
		}
		if (3 == nSaveType)
		{
			m_strExtension = L"tiff";
		}
		EndDialog(wID);
	}
	else
	{
		MessageBox(L"该路径不存在，请重新输入", MB_OK);
	}
	return 0 ;
}
LRESULT CDefaultSaveDirDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return -1;
}
LRESULT CBooguNoteView::OnPopupmenuRepairShortcut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CRepairShortcutDlg dlgRepair;
	if (IDOK == dlgRepair.DoModal())
	{
		for (int i=0; i<m_textBlockList.size(); i++)
		{
			if (!m_textBlockList[i]->m_bFile)
			{
				BSTR  bstrCache;
				m_textBlockList[i]->GetTextServices()->TxGetText(&bstrCache);
				int length = SysStringLen(bstrCache);
				TCHAR* szCache = new TCHAR[length+1];
				_stprintf(szCache, _T("%s"), (LPCTSTR)bstrCache);

				//remove the last return code.
				if ((_T('\r') == szCache[length-1]) || (_T('\n') == szCache[length-1]))
				{
					szCache[length-1] = _T('\0');
					length--;
				}
				if (length < _MAX_PATH && length > 3)
				{
					if ((L'.'==szCache[0]&&L'\\'==szCache[1]) ||
						(L'.'==szCache[0]&&L'.'==szCache[1]&&L'\\'==szCache[2]))
					{
						FindInFiles(PathFindFileName(szCache), basic_string<TCHAR>(LPCTSTR(dlgRepair.m_strSearchDir)), m_textBlockList[i]); 
					}
				}
				delete []szCache;
			}
		}
		m_bDirty = true;
		GetParent().SendMessageW(WM_NCPAINT);
		MessageBox(L"修复完毕！", L"通知", MB_OK);
		
	}

	return 0;
}
static bool NameCompare(basic_string<TCHAR> e1, basic_string<TCHAR> e2)
{
	return e1 > e2;
}

LRESULT CBooguNoteView::FindInFiles(LPCTSTR szFileName, basic_string<TCHAR> searchDir, CBooguNoteText* pTextBlock)
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
		return 0;
	}
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
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
			if (0 == wcscmp(szFileName, PathFindFileName(ffd.cFileName)))
			{
				pTextBlock->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(searchDir+ffd.cFileName).c_str(), 0);
				pTextBlock->DetectFileBlock();
			}
// 			basic_string<TCHAR> bsFile(searchDir);
// 			CString fn(ffd.cFileName);
// 			int dot = fn.ReverseFind(_T('.'));
// 			CString ext = fn;
// 			ext.Delete(0, dot+1);
// 			ext.MakeLower();
// 			if (0==ext.Compare(_T("boo")))
// 			{
// 				bsFile+=ffd.cFileName;
// 				fileNameList.push_back(bsFile);
// 			}
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	//sort(fileNameList.begin(), fileNameList.end(), NameCompare);
	//vector< basic_string<TCHAR> >::iterator p = fileNameList.begin();
	//while(p!=fileNameList.end())
	//{
	//	SearchInFile(searchWord, (*p), bMatchCase);
	//	p++;
	//}

	sort(subDirList.begin(), subDirList.end(), NameCompare);
	if (subDirList.size()>0)
	{
		vector< basic_string<TCHAR> >::iterator p = subDirList.begin();
		while(p!=subDirList.end())
		{
			FindInFiles(szFileName, (*p), pTextBlock);
			p++;
		}
	}
	FindClose(hFind);
}
LRESULT CBooguNoteView::OnPopupmenuShare(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetParent().PostMessage(WM_USER_SHARE);

	return 0;
}

LRESULT CBooguNoteView::OnStateNarrowAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    for (int i=0; i<m_textBlockList.size(); ++i)
    {
        if ((TEXT_HANDLE_CLOSED == m_textBlockList[i]->m_pTextHandle->state) && (0 ==m_textBlockList[i]->m_nLevel))
        {
            ExpandBlocks(i);
        }       
        m_bDirty = true;
    }


	vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin();
	for(; ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock)->m_bShown && (TEXT_BLOCK_NARROW != (*ppTextBlock)->m_nExpandState))
		{
			if ((*ppTextBlock)->m_bFile && (*ppTextBlock)->m_pBitmap != NULL)
			{
				TCHAR szTempCache[MAX_PATH*3];
				if (_T('\\') == m_pTextBlockFocused->m_szCache[0])
				{
					_stprintf(szTempCache, _T("%s%s"), g_config.szRootStorageDir, m_pTextBlockFocused->m_szCache);
				}
				else if (L'.' == m_pTextBlockFocused->m_szCache[0])
				{
					if (!m_fileDirectory.IsEmpty())
					{
						swprintf_s(szTempCache, MAX_PATH, L"%s", m_fileDirectory);
						PathRemoveFileSpec(szTempCache);
						PathAppend(szTempCache, m_pTextBlockFocused->m_szCache);
					}
				}
				else
				{
					_stprintf(szTempCache, _T("%s"), m_pTextBlockFocused->m_szCache);
				}
				CString fileDirectory = szTempCache;
				int dot = fileDirectory.ReverseFind(_T('.'));
				CString ext = fileDirectory;
				ext.Delete(0, dot+1);
				ext.MakeLower();
				
				if (0==ext.Compare(_T("png"))||
					0==ext.Compare(_T("jpeg")) ||
					0==ext.Compare(_T("jpg")) ||
					0==ext.Compare(_T("gif")) ||
					0==ext.Compare(_T("tiff")) ||
					0==ext.Compare(_T("bmp")))
				{
					Image tempImage(szTempCache);
					//if (tempImage.GetLastStatus() != Ok)
					if (tempImage.GetLastStatus() != Ok)
					{
					}
					else
					{
						(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)_T(""), 0);
						CSize sz(0,0);
						if (tempImage.GetHeight()>g_config.nSmallThumbnailHeight)
						{
							sz.cy = g_config.nSmallThumbnailHeight;
							sz.cx = tempImage.GetWidth()*sz.cy/tempImage.GetHeight();
						}
						else
						{
							sz.cx = tempImage.GetWidth();
							sz.cy = tempImage.GetHeight();
						}
						delete (*ppTextBlock)->m_pBitmap;
						(*ppTextBlock)->m_pBitmap = new Bitmap(sz.cx, sz.cy);
						Graphics imageGraphics((*ppTextBlock)->m_pBitmap);
						imageGraphics.DrawImage(&tempImage,0, 0, sz.cx, sz.cy);
					}
				}
			}
			else
			{
				if (TEXT_BLOCK_SHRINK == (*ppTextBlock)->m_nExpandState)
				{
					RECT* prc = (*ppTextBlock)->GetClientRect();
					CRect rc(prc);
					rc.right = rc.left + g_config.narrowWidth-((*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
					(*ppTextBlock)->SetClientRect(&rc);
					//(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
					CHARFORMAT cf;
					cf.cbSize = sizeof(CHARFORMAT);
					(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwEffects &= ~CFE_UNDERLINE;
					(*ppTextBlock)->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
					(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(*ppTextBlock)->m_szCache, 0);
					(*ppTextBlock)->SetReadOnly(FALSE);
				}
				else
				{
					RECT* prc = (*ppTextBlock)->GetClientRect();
					CRect rc(prc);
					rc.right = rc.left + g_config.narrowWidth-((*ppTextBlock)->m_nLevel*(g_config.nHandleWidth+2+g_config.hostBorder) - g_config.hostBorder);
					(*ppTextBlock)->SetClientRect(&rc);
					//(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
					CHARFORMAT cf;
					cf.cbSize = sizeof(CHARFORMAT);
					/*(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf, 0);
					cf.dwEffects &= ~CFE_UNDERLINE;
					(*ppTextBlock)->TxWindowProc(m_hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);*/
					
					//(*ppTextBlock)->GetTextServices()->TxSendMessage(WM_SETTEXT, 0, (LPARAM)(*ppTextBlock)->m_szCache, 0);
					(*ppTextBlock)->SetReadOnly(FALSE);
					/*(*ppTextBlock)->GetTextServices()->TxSendMessage(EM_AUTOURLDETECT, TRUE, 0, 0);*/
				}
			}
			(*ppTextBlock)->m_nExpandState = TEXT_BLOCK_NARROW;
		}
	}
	m_bCancelAutoAdjust = true;
	CalcLayout();
	CalcScrollBar();
	Invalidate(FALSE); //flush size into text block and repaint
	return 0;
}

LRESULT CBooguNoteView::OnStateShrinkAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    for (int i=0; i<m_textBlockList.size(); ++i)
    {
        if ((TEXT_HANDLE_EXPAND == m_textBlockList[i]->m_pTextHandle->state) && (0 ==m_textBlockList[i]->m_nLevel))
        {
            CloseBlocks(i);
        }        
        m_bDirty = true;
    }


	vector<CBooguNoteText*>::iterator ppTextBlock = m_textBlockList.begin();
	for(; ppTextBlock != m_textBlockList.end(); ++ppTextBlock)
	{
		if ((*ppTextBlock)->m_bShown  && (TEXT_BLOCK_SHRINK != (*ppTextBlock)->m_nExpandState))
		{
			ShrinkTextBlock(*ppTextBlock);
		}
	}
	m_bCancelAutoAdjust = true;
	CalcLayout();
	CalcScrollBar();
	Invalidate(FALSE); //flush size into text block and repaint

	return 0;
}
