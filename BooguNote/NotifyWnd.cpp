// NotifyWnd.cpp: implementation of the CNotifyWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NotifyWnd.h"
#include "BooguNoteConfig.h"
#include "shobjidl.h"
#include "Common.h"

extern CBooguNoteConfig g_config;
extern CUtil	g_util;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LRESULT CNotifyWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( ModifyStyleEx(0,WS_EX_LAYERED ))
	{
		SetTimer(NOTIFY_TIMER_ID ,30);
		m_bTimerActive=TRUE;
	}
	m_refWndColor=g_config.clrTabBackground;//GetSysColor(COLOR_ACTIVECAPTION);
	//pass these bitmaps in this sequence, ie. Normal pos.,positin while ,mouse move,
	// button presed position
//	UINT nBMPs[3]={IDB_BMP_NORMAL,IDB_BMP_HOVER,IDB_BMP_PRESS};
	//m_pButton=new CBmpButton(nBMPs);
	//RECT r1 ;
	//r1 .top=5;
	//r1.left=m_nWidth-15;
	//r1.right= m_nWidth-5;
	//r1.bottom= 15;

	//m_pButton->Create(m_hWnd,r1);
	//m_pButton->ShowWindow(SW_SHOW);
	//m_pButton->UpdateWindow();
    return 1;
}
LRESULT CNotifyWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (0 == m_nState)
	{
		m_nCurrentOpacity += 28;
		if (m_nCurrentOpacity >= m_nOpacity)
		{
			m_nCurrentOpacity = m_nOpacity;
			m_nState = 1;
			m_nTick = 60;
		}
		ChangeOpacity(m_nCurrentOpacity);
	}
	else if (1 == m_nState)
	{
		m_nTick--;
		if (m_nTick<0)
		{
			m_nState = 2;
		}
	}
	else if (2 == m_nState)
	{
		if ( m_nCurrentOpacity <= 0 )
		{
			//done, kill timer, and  close window
			KillTimer(NOTIFY_TIMER_ID );
			m_bTimerActive=FALSE;
			m_nCurrentOpacity=0;
			//m_nIncrement=30;
			DestroyWindow();
			m_nState = 0;
		}
		else
		{
			ChangeOpacity(m_nCurrentOpacity);
			m_nCurrentOpacity -= 8;
		}
	}
	//if (m_nCurrentOpacity >= m_nOpacity)
	//{
	//	//now start to dissappear
	//	m_nIncrement = - 2;
	//	//m_nCurrentOpacity = m_nOpacity;
	//}
	//m_nCurrentOpacity += m_nIncrement;
	//if (m_nCurrentOpacity>m_nOpacity)
	//{
	//	m_nCurrentOpacity = m_nOpacity;
	//}
	//if ( m_nCurrentOpacity <= 0 )
	//{
	//	//done, kill timer, and  close window
	//	KillTimer(NOTIFY_TIMER_ID );
	//	m_bTimerActive=FALSE;
	//	m_nCurrentOpacity=0;
	//	m_nIncrement=30;
	//	DestroyWindow();
	//}
	//else
	//{
	//	ChangeOpacity(m_nCurrentOpacity);
	//}
	return 0;
}
LRESULT CNotifyWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( ! IsIconic() )
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(&ps);
		RECT rcClient;
		GetClientRect(&rcClient);
		//use color, used by system for coloring titile bar
		HBRUSH hbr=CreateSolidBrush(m_refWndColor);
		FillRect(hDC,&rcClient,hbr);
		//done with brush
		DeleteObject(hbr);
		rcClient.top =(rcClient.bottom/2) - 17 ;
		//rcClient.bottom-=10;
		
		CWindowDC dcWindow(this->m_hWnd);
		//make text background transparent
		dcWindow.SetBkMode(TRANSPARENT);
		dcWindow.SetTextColor(g_config.clrTabCaptionText);
		dcWindow.SelectFont(g_config.hTabFontBold);
		dcWindow.DrawText(m_strNotify,-1,&rcClient, DT_CENTER|DT_VCENTER);
		CPen penTabButton;
		penTabButton.CreatePen(PS_SOLID, 1, g_config.clrTabButton);
		dcWindow.SelectPen(penTabButton);
		dcWindow.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));
		GetClientRect(&rcClient);
		dcWindow.Rectangle(&rcClient);
		EndPaint(&ps);
		bHandled=TRUE;
	}
	return 1;
}
void CNotifyWnd::CreateNotifyWindow()
{
	RECT rcClient;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcClient, 0);
	rcClient.bottom=rcClient.bottom-1;
	rcClient.right=rcClient.right-1;
	rcClient.top=rcClient.bottom-m_nHeight;
	rcClient.left=rcClient.right-m_nWidth;
	Create(NULL,rcClient);

	ITaskbarList* pTaskbar;
	CoCreateInstance(CLSID_TaskbarList,0, CLSCTX_INPROC_SERVER,IID_ITaskbarList,(void**)&pTaskbar);
	pTaskbar->HrInit();
	pTaskbar->DeleteTab(this->m_hWnd);
	pTaskbar->Release();

	ShowWindow(SW_SHOW);
	SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}
LRESULT CNotifyWnd::ChangeOpacity(BYTE iFactor)
{
	//define function pointer
	//typedef DWORD (WINAPI *pSetLayeredWindowAttributes)(HWND, DWORD, BYTE, DWORD);
	//pSetLayeredWindowAttributes SetLayeredWindowAttributes;
	//HMODULE hDLL = LoadLibrary ("user32");
	//if (hDLL )
	//{
		//SetLayeredWindowAttributes = (pSetLayeredWindowAttributes) GetProcAddress(hDLL,"SetLayeredWindowAttributes");
		//ATLASSERT(SetLayeredWindowAttributes );//using WIN2k or onward ?
		BOOL bRes=SetLayeredWindowAttributes(m_hWnd,RGB(255,255,255), iFactor, LWA_COLORKEY | LWA_ALPHA);
		DWORD dw=GetLastError();
		//FreeLibrary(hDLL);	
	//}
	//else
	//{
	//	//not able to load library
	//	ATLASSERT(0);
	//}
	return 0;

}
LRESULT CNotifyWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DestroyWindow();
	return 1;
}
LRESULT CNotifyWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//	m_pButton->DestroyWindow();
//	delete m_pButton;
	return 1;
}
LRESULT CNotifyWnd::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bTimerActive)
	{
		KillTimer(NOTIFY_TIMER_ID);
		m_bTimerActive=FALSE;
		ChangeOpacity(m_nOpacity);
		m_nCurrentOpacity=m_nOpacity;
		//fortunately this we can get help from foe setting mouse leave event
		TRACKMOUSEEVENT stMouseEvent;
		//to check Mouse leave event
		stMouseEvent.cbSize=sizeof(stMouseEvent);
		stMouseEvent.dwFlags=TME_LEAVE;
		stMouseEvent.dwHoverTime=HOVER_DEFAULT;
		stMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent(&stMouseEvent);
		//HCURSOR hCursor=LoadCursor(NULL,IDC_HAND);
		SetClassLong(m_hWnd,GCL_HCURSOR,(LONG) g_util.hCsrHand);   // new cursor 
	}
	return 0;
}
LRESULT CNotifyWnd::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLASSERT(m_bTimerActive == FALSE);
	m_bTimerActive=SetTimer(NOTIFY_TIMER_ID ,30);
	HCURSOR hCursor=GetCursor();
	SetClassLong(m_hWnd,GCL_HCURSOR,(LONG) hCursor);   // new cursor 
	return 0;
}

