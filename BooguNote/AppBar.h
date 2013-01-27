//////////////////////////////////////////////////////////////////////////////////////////////
// ATL implementation for AppBar support
#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error appbar.h requires atlbase.h to be included first
#endif

#ifndef __ATLAPP_H__
	#error appbar.h requires atlapp.h to be included first
#endif

#ifndef __ATLWIN_H__
	#error appbar.h requires atlwin.h to be included first
#endif

#ifndef __ATLGDI_H__
	#error appbar.h requires atlgdi.h to be included first
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	class CAppBar
//		give derived window the ability to dock on any side of the screen
//		gain behavior similar with Windows Task bar
//
//	Author:		Yao Zhifeng
//	Contact:	yaozhifeng@hotmail.com
//
//	Usage:
//		1.use this class as a base class
//		2.use CHAIN_MSG_MAP to chain message to this class
//		3.call InitAppBar in the OnCreate or OnInitDialog function
//		4.(optional) call DockAppBar to dock the window to any side of the screen
//		5.(optional) call SetAutoHide to change the autohide behavior
//		6.(optional) call SetKeepSize to enable keeping original window size when docking on the screen
//		7.(optional) if you want to do any specific operation when docking side changed, 
//		  override OnDockingChanged function in ATL way
//
//	template argument:
//		T: derived class, must also derived from CWindowImpl directly or indirectly
//
template <class T>
class CAppBar
{
public: //constants
	enum DockingSide
	{
		APPBAR_DOCKING_NONE		= 0x00,
		APPBAR_DOCKING_LEFT		= 0x01,
		APPBAR_DOCKING_RIGHT	= 0x02,
		APPBAR_DOCKING_TOP		= 0x04,
		APPBAR_DOCKING_BOTTOM	= 0x08,
		APPBAR_DOCKING_ALL		= 0x0f

	};

protected:
	CRect	m_rcOriginal;	//window rect while not docking on any side
	CRect	m_rcStartDraging;
	CRect	m_rcLastDraging;
	CPoint	m_ptStartDraging;
	bool	m_bDraging;

	UINT	m_uCallback; //callback message for calling by system
	UINT	m_uFlags;	//docking flags
	UINT	m_uCurrentDockingSide;	
	bool	m_bRegistered; //if registered
	bool	m_bAutoHide;
	bool	m_bHiding;
	bool	m_bKeepSize;

	UINT	m_uHidingTimer;
	UINT	m_uShowingTimer;

public:
	CAppBar()
	{
		m_uCallback = ::RegisterWindowMessage(_T("AppBar Callback Message"));
		m_uFlags = APPBAR_DOCKING_NONE;
		m_uCurrentDockingSide = APPBAR_DOCKING_NONE;
		m_bRegistered = false;
		m_bDraging = false;
		m_bAutoHide = false;
		m_bHiding = false;
		m_uHidingTimer = 1;
		m_uShowingTimer = 2;
		m_bKeepSize = false;
	}
	~CAppBar()
	{
	}

	BEGIN_MSG_MAP(CAppBar)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(m_uCallback, OnAppBarNotify)
	END_MSG_MAP()

public:
	//////////////////////////////////////////////////////////////////////////
	// Initialize
	// must be called right after window be created
	void InitAppBar(UINT uFlags=APPBAR_DOCKING_ALL, bool bAutoHide=false, bool bKeepSize=false)
	{
		T* pT = static_cast<T*>(this);

		m_uFlags = uFlags;
		m_bAutoHide = bAutoHide;
		m_bKeepSize = bKeepSize;

		//record the original window rect
		pT->GetWindowRect(&m_rcOriginal);

	}

	///////////////////////////////////////////////////////////
	//change the autohide behavior
	void SetAutoHide(bool bAutoHide)
	{
		if (m_bAutoHide != bAutoHide)
		{
			UINT uCurrentSide = m_uCurrentDockingSide;
			RegisterAppBar(false);
			m_bAutoHide = bAutoHide;
			DockAppBar(uCurrentSide);
		}
	}

	///////////////////////////////////////////////////////////
	//change the keepsize behavior
	void SetKeepSize(bool bKeepSize)
	{
		if (m_bKeepSize != bKeepSize)
		{
			UINT uCurrentSide = m_uCurrentDockingSide;
			RegisterAppBar(false);
			m_bKeepSize = bKeepSize;
			DockAppBar(uCurrentSide);
		}
	}

	//////////////////////////////////////////////////////////
	// dock window to one side of the screen
	void DockAppBar(UINT uFlag)
	{
		T* pT = static_cast<T*>(this);

		if (uFlag == APPBAR_DOCKING_NONE)
		{
			//Undock the window
			RegisterAppBar(false);
			m_uCurrentDockingSide = uFlag;
			pT->MoveWindow(&m_rcOriginal);
		}
		else
		{
			//docking flag must match the initial flags
			ATLASSERT((uFlag & m_uFlags)!=0);

			if (!m_bRegistered)
				RegisterAppBar(true);

			//record the original position before becoming an appbar
			if (m_uCurrentDockingSide == APPBAR_DOCKING_NONE)
				pT->GetWindowRect(&m_rcOriginal);

			//autohide processing
			APPBARDATA	abd;
			abd.cbSize	= sizeof(abd);
			abd.hWnd	= pT->m_hWnd;
			abd.uEdge	= GetEdge(uFlag);
			abd.lParam	= m_bAutoHide ? TRUE:FALSE;

			::SHAppBarMessage(ABM_SETAUTOHIDEBAR, &abd);

			//set area and move window
			RECT	rc = SetAppBarPos(uFlag);
			m_uCurrentDockingSide = uFlag;

			//for autohide&keepsize setting
			//reajust the window position
			if (m_bAutoHide && m_bKeepSize)
				rc = QueryDockingRect(uFlag);
			pT->MoveWindow(&rc);

			pT->OnDockingChanged(uFlag);

			//start the hiding timer to hide the autohide appbar
			if (m_bAutoHide)
			{
				m_uHidingTimer = pT->SetTimer(m_uHidingTimer, 500);
			}
		}
	}

	//////////////Message Handlers/////////////////////////////////////////////////////

	LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);

		WINDOWPOS *pWindowPos = (WINDOWPOS*)lParam;
	
		//default processing
		::DefWindowProc(pT->m_hWnd, uMsg, wParam, lParam);

		if (m_uCurrentDockingSide==APPBAR_DOCKING_NONE)
		{
			//update original rect
			pT->GetWindowRect(&m_rcOriginal);
		}
		else
		{
			if (!m_bHiding)
			{
				//if window being sized when docked, update appbar area
				if (m_uCurrentDockingSide==APPBAR_DOCKING_LEFT || m_uCurrentDockingSide==APPBAR_DOCKING_RIGHT)
				{
					m_rcOriginal.right = m_rcOriginal.left + pWindowPos->cx;
					SetAppBarPos(m_uCurrentDockingSide);
				}
				else if (m_uCurrentDockingSide==APPBAR_DOCKING_TOP || m_uCurrentDockingSide==APPBAR_DOCKING_BOTTOM)
				{
					m_rcOriginal.bottom = m_rcOriginal.top + pWindowPos->cy;
					SetAppBarPos(m_uCurrentDockingSide);
				}
			}
		}

		if (m_bRegistered && m_uCurrentDockingSide!=APPBAR_DOCKING_NONE)
		{
			APPBARDATA	abd;
			memset(&abd, 0, sizeof(APPBARDATA));
			abd.cbSize = sizeof(abd);
			abd.hWnd = pT->m_hWnd;

			::SHAppBarMessage(ABM_WINDOWPOSCHANGED, &abd);
		}

		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// revoke appbar when destroy window
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RegisterAppBar(false);
		bHandled = FALSE;
		//return 0;
		return 1;
	}

	//record start position for draging operation
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//enable derived class to handle this message
		bHandled = FALSE;

		T* pT = static_cast<T*>(this);

		pT->SetCapture();
		::GetCursorPos(&m_ptStartDraging);

		////first rect
		//if (m_uCurrentDockingSide==APPBAR_DOCKING_NONE)
		//{
			pT->GetWindowRect(&m_rcStartDraging);
		//}
		//else
		//{
		//	m_rcStartDraging.SetRect(m_ptStartDraging, m_ptStartDraging+m_rcOriginal.Size());
		//	m_rcStartDraging.OffsetRect(-m_rcOriginal.Width()/2, -m_rcOriginal.Height()/2);
		//}

		return 0;
	}

	//determin if draging, draw a draging rectangle to show target rect
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//enable derived class to handle this message
		bHandled = FALSE;

		T* pT = static_cast<T*>(this);
		CPoint	ptCurrent;
		::GetCursorPos(&ptCurrent);
		CPoint ptOffset = ptCurrent - m_ptStartDraging;

		enum
		{
			DRAG_DISTANCE = 2
		};

		//only if the mouse has been dragged for a small distance,
		//we can determine this is a draging operation
		if (::GetCapture()==pT->m_hWnd && !m_bDraging)
		{
			if (abs(ptOffset.x)>DRAG_DISTANCE || abs(ptOffset.y)>DRAG_DISTANCE)
			{
				m_bDraging = true;
				//draw the first frame recorded in LButtonDown handler
				DrawFrame(m_rcStartDraging);
				m_rcLastDraging = m_rcStartDraging;
			}
		}

		if (m_bDraging)
		{
			//cancel last frame rect
			DrawFrame(m_rcLastDraging);

			//calculate new frame rect
			m_rcLastDraging = m_rcStartDraging;
			m_rcLastDraging.OffsetRect(ptOffset);

			UINT uFlag = GetProposedDockingFlag(ptCurrent);
			if (uFlag != APPBAR_DOCKING_NONE)
			{
				//m_rcLastDraging = GetProposedDockingRect(uFlag);
				m_rcLastDraging = QueryDockingRect(uFlag);
			}

			//draw new frame rect
			DrawFrame(m_rcLastDraging);
		}

		return 0;
	}

	//if draging, move window to new position or dock to correct side
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//enable derived class to handle this message
		bHandled = FALSE;

		T* pT = static_cast<T*>(this);

		::ReleaseCapture();
		if (m_bDraging)
		{
			m_bDraging = false;

			DrawFrame(m_rcLastDraging);

			CPoint	ptCurrent;
			::GetCursorPos(&ptCurrent);
			UINT uFlag = GetProposedDockingFlag(ptCurrent);
			DockAppBar(uFlag);
			if (uFlag == APPBAR_DOCKING_NONE)
			{
				pT->MoveWindow(m_rcLastDraging);
			}
			
		}
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// take over default hit test behavior
	// prevent unnormal moving and sizing operation
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

		//take care of the auto unhide stuff
		if (m_bAutoHide && m_bHiding)
		{
			m_uShowingTimer = pT->SetTimer(m_uShowingTimer, 50);
		}

		LRESULT hit = ::DefWindowProc(pT->m_hWnd, uMsg, wParam, lParam);

		if (m_bAutoHide)
		{
			switch (hit)
			{
			case HTCAPTION:
				hit = HTCLIENT;
				break;
			case HTLEFT:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_LEFT)
					hit = HTCLIENT;
				break;
			case HTRIGHT:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_RIGHT)
					hit = HTCLIENT;
				break;
			case HTTOP:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_TOP)
					hit = HTCLIENT;
				break;
			case HTBOTTOM:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_BOTTOM)
					hit = HTCLIENT;
				break;
			case HTTOPLEFT:
			case HTTOPRIGHT:
			case HTBOTTOMLEFT:
			case HTBOTTOMRIGHT:
				if (m_uCurrentDockingSide!=APPBAR_DOCKING_NONE)
					hit = HTCLIENT;
				break;
			}
		}
		else
		{
			switch (hit)
			{
			case HTCAPTION:
				hit = HTCLIENT;
				break;
			case HTLEFT:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_LEFT ||
					m_uCurrentDockingSide==APPBAR_DOCKING_TOP ||
					m_uCurrentDockingSide==APPBAR_DOCKING_BOTTOM)
					hit = HTCLIENT;
				break;
			case HTRIGHT:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_RIGHT ||
					m_uCurrentDockingSide==APPBAR_DOCKING_TOP ||
					m_uCurrentDockingSide==APPBAR_DOCKING_BOTTOM)
					hit = HTCLIENT;
				break;
			case HTTOP:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_LEFT ||
					m_uCurrentDockingSide==APPBAR_DOCKING_TOP ||
					m_uCurrentDockingSide==APPBAR_DOCKING_RIGHT)
					hit = HTCLIENT;
				break;
			case HTBOTTOM:
				if (m_uCurrentDockingSide==APPBAR_DOCKING_LEFT ||
					m_uCurrentDockingSide==APPBAR_DOCKING_BOTTOM ||
					m_uCurrentDockingSide==APPBAR_DOCKING_RIGHT)
					hit = HTCLIENT;
				break;
			case HTTOPLEFT:
			case HTTOPRIGHT:
			case HTBOTTOMLEFT:
			case HTBOTTOMRIGHT:
				if (m_uCurrentDockingSide!=APPBAR_DOCKING_NONE)
					hit = HTCLIENT;
				break;
			}
		}

		return hit;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// take over default hit test behavior
	// prevent unnormal moving and sizing operation
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//enable derived class to handle this message
		bHandled = FALSE;

		T* pT = static_cast<T*>(this);
		APPBARDATA abd;
		
		// Always send the activate message to the system
		abd.cbSize = sizeof(APPBARDATA);
		abd.hWnd = pT->m_hWnd;
		abd.lParam = 0;                
		SHAppBarMessage(ABM_ACTIVATE, &abd);

		UINT state = LOWORD(wParam);
		// Now determine if we're getting or losing activation
		switch (state)
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				// If we're gaining activation, make sure we're visible
				if (m_uCurrentDockingSide!=APPBAR_DOCKING_NONE && m_bHiding && m_bAutoHide)
				{
					DoShowAppBar();
					//start the hiding timer after showing
					m_uHidingTimer = pT->SetTimer(m_uHidingTimer, 500);
				}
				break;

			case WA_INACTIVE:
				break;
		}
		return 0;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//enable derived class to handle this message
		bHandled = FALSE;

		T* pT = static_cast<T*>(this);
		if (wParam==m_uHidingTimer)
		{
			if (m_uCurrentDockingSide==APPBAR_DOCKING_NONE || !m_bAutoHide)
			{
				pT->KillTimer(wParam);
			}
			else
			{
				// Get the mouse position, the window position, and active 
				// window
				CPoint	pt;
				CRect	rcWnd;
				::GetCursorPos(&pt);
				pT->GetWindowRect(&rcWnd);

				CRect	rcFileTV(0,0,0,0);
				if (pT->m_FileTV.IsWindow())
				{
					pT->m_FileTV.GetWindowRect(&rcFileTV);
				}
	
				CWindow wndActive = ::GetForegroundWindow();

				// If the mouse is outside of our window, or we are not active,
				// or at least one window is active, or we are not the parent
				// of an active window, the hide the appbar window.
				if ((!rcWnd.PtInRect(pt)) && (!rcFileTV.PtInRect(pt))  && (wndActive.m_hWnd != pT->m_hWnd) && 
					(wndActive.m_hWnd != NULL) && wndActive.GetWindow(GW_OWNER)!=pT->m_hWnd)
				{
					pT->KillTimer(wParam);
					DoHideAppBar();
				}
			}
		}
		else if (wParam==m_uShowingTimer)
		{
			pT->KillTimer(wParam);
			if (m_bAutoHide && m_bHiding && m_uCurrentDockingSide!=APPBAR_DOCKING_NONE)
			{
				CPoint	pt;
				CRect	rcWnd;
				::GetCursorPos(&pt);
				pT->GetWindowRect(&rcWnd);

				//check if the cursor is still in the window rect
				if (rcWnd.PtInRect(pt))
				{
					DoShowAppBar();
					//start the hiding timer after showing
					m_uHidingTimer = pT->SetTimer(m_uHidingTimer, 500);
				}
			}
		}

		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	//handle system callback
	LRESULT OnAppBarNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		APPBARDATA abd; 
		UINT uState; 

		abd.cbSize = sizeof(abd); 
		abd.hWnd = pT->m_hWnd; 

		switch (wParam)
		{
		case ABN_FULLSCREENAPP:
			//to be implemented
			//...
			break;
		case ABN_POSCHANGED:
			//adjust window position and docking area
			if (m_uCurrentDockingSide!=APPBAR_DOCKING_NONE)
			{
				CRect	rc = SetAppBarPos(m_uCurrentDockingSide);
				pT->MoveWindow(&rc);
			}
			break;
		case ABN_STATECHANGE:
            // Check to see if the taskbar's always-on-top state has 
            // changed and, if it has, change the appbar's state 
            // accordingly. 
            uState = SHAppBarMessage(ABM_GETSTATE, &abd); 

            pT->SetWindowPos( 
                (ABS_ALWAYSONTOP & uState) ? HWND_TOPMOST : HWND_NOTOPMOST, 
                0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); 
			break;
		case ABN_WINDOWARRANGE:
			//exclude the appbar from window arrangement
			if (lParam)//begin
			{
				pT->ShowWindow(SW_HIDE);
			}
			else //completed
			{
				pT->ShowWindow(SW_SHOW);
			}
			break;
		}
		return 0;
	}
	////////End Message Handlers//////////////////////////////////////////////////////


	////////////For override in derived class///////////////////////////////
	//
	///////////////////////////////////////////////////////////////////////
	// Be Called after docking side changed
	void OnDockingChanged(UINT uDockingSide)
	{
	}

	/////////////////////////////////////////////////////////////////
	// get width and height while docking window
	SIZE GetDockSize()
	{
		SIZE	szDock;
		szDock.cx = m_rcOriginal.Width();
		szDock.cy = m_rcOriginal.Height();

		return szDock;
	}

	////////////End for override in derived class///////////////////////////


protected:
	////////////////////////////////////////////////////////
	//add or remove appbar from system
	void RegisterAppBar(bool bRegister)
	{
		T* pT = static_cast<T*>(this);
		APPBARDATA	abd;
		memset(&abd, 0, sizeof(APPBARDATA));
		abd.cbSize = sizeof(APPBARDATA);
		abd.hWnd	= pT->m_hWnd;

		if (&m_bRegistered && bRegister)
		{
			//register appbar to system
			abd.uCallbackMessage	= m_uCallback;

			if (!m_bAutoHide)
			{
				::SHAppBarMessage(ABM_NEW, &abd);
			}
			m_bRegistered = true;

		}
		else if (m_bRegistered && !bRegister)
		{
			//remove appbar from system
			::SHAppBarMessage(ABM_REMOVE, &abd);
			m_bRegistered = false;
		}
	}

	///////////////////////////////////////////////////////////////////////
	// set or adjust the appbar
	// return the target rect for appbar
	RECT SetAppBarPos(UINT uFlag)
	{
		T* pT = static_cast<T*>(this);

		APPBARDATA	abd;
		memset(&abd, 0, sizeof(APPBARDATA));
		abd.cbSize	= sizeof(APPBARDATA);
		abd.hWnd	= pT->m_hWnd;

		abd.uEdge	= GetEdge(uFlag);
		abd.rc		= GetProposedDockingRect(uFlag);

		::SHAppBarMessage(ABM_QUERYPOS, &abd);

		//the proposed rect might be changed, we need to ajust the width or height
		AdjustDockingRect(abd.rc, uFlag);

		//set docking pos
		::SHAppBarMessage(ABM_SETPOS, &abd);

		return abd.rc;
	}

	///////////////////////////////////////////////////////////////////////
	// calculat proposed docking rect
	RECT GetProposedDockingRect(UINT uFlag)
	{
		T* pT = static_cast<T*>(this);

		//get screen rect
		RECT	rc;
		::GetWindowRect(::GetDesktopWindow(), &rc);

		//adjust the rect according to docking flag
		SIZE	sz = pT->GetDockSize();
		switch (uFlag)
		{
		case APPBAR_DOCKING_LEFT:
			rc.right = rc.left + sz.cx;
			break;
		case APPBAR_DOCKING_RIGHT:
			rc.left = rc.right - sz.cx;
			break;
		case APPBAR_DOCKING_TOP:
			rc.bottom = rc.top + sz.cy;
			break;
		case APPBAR_DOCKING_BOTTOM:
			rc.top = rc.bottom - sz.cy;
			break;
		default:
			ATLASSERT("Invalid argument");
			break;
		}

		return rc;
	}

	/////////////////////////////////////////////////////////////////////
	// query docking rect
	// for visual effect while moving window with mouse
	RECT QueryDockingRect(UINT uFlag)
	{
		T* pT = static_cast<T*>(this);

		APPBARDATA	abd;
		memset(&abd, 0, sizeof(APPBARDATA));
		abd.cbSize	= sizeof(APPBARDATA);
		abd.hWnd	= pT->m_hWnd;

		abd.uEdge	= GetEdge(uFlag);
		abd.rc		= GetProposedDockingRect(uFlag);

		::SHAppBarMessage(ABM_QUERYPOS, &abd);

		//the proposed rect might be changed, we need to ajust the width or height
		AdjustDockingRect(abd.rc, uFlag);

		//ajust rect for autosize&keepsize settings
		if (m_bAutoHide && m_bKeepSize)
		{
			//CPoint	pt;
			//::GetCursorPos(&pt);
			if (m_rcLastDraging.IsRectEmpty())
			{
				m_rcLastDraging = m_rcOriginal;
			}
			SIZE	sz = pT->GetDockSize();
			switch (uFlag)
			{
			case APPBAR_DOCKING_LEFT:
			case APPBAR_DOCKING_RIGHT:
				abd.rc.top = m_rcLastDraging.top;
				//abd.rc.top = pt.y - sz.cy/2;
				abd.rc.bottom = abd.rc.top + sz.cy;
				break;
			case APPBAR_DOCKING_TOP:
			case APPBAR_DOCKING_BOTTOM:
				abd.rc.left = m_rcLastDraging.left;
				//abd.rc.left = pt.x - sz.cx/2;
				abd.rc.right = abd.rc.left + sz.cx;
				break;
			}
		}

		return abd.rc;
	}

	/////////////////////////////////////////////////////////////
	// after calling ABM_QUERYPOS, the acquired rect need to be adjusted
	void AdjustDockingRect(RECT &rc, UINT uFlag)
	{
		T* pT = static_cast<T*>(this);
		SIZE	sz = pT->GetDockSize();
		switch (uFlag)
		{
		case APPBAR_DOCKING_LEFT:
			rc.right = rc.left + sz.cx;
			break;
		case APPBAR_DOCKING_RIGHT:
			rc.left = rc.right - sz.cx;
			break;
		case APPBAR_DOCKING_TOP:
			rc.bottom = rc.top + sz.cy;
			break;
		case APPBAR_DOCKING_BOTTOM:
			rc.top = rc.bottom - sz.cy;
			break;
		default:
			ATLTRACE("Invalid argument");
			ATLASSERT(FALSE);
			break;
		}
	}

	////////////////////////////////////////////////////////////////
	// get proposed target docking side while moving window
	UINT GetProposedDockingFlag(POINT ptCursor)
	{
		UINT	uFlag = APPBAR_DOCKING_NONE;
		RECT	rcScreen;
		::GetWindowRect(::GetDesktopWindow(), &rcScreen);
		if (ptCursor.x - rcScreen.left<10)
			uFlag = APPBAR_DOCKING_LEFT;
		else if (ptCursor.y - rcScreen.top<10)
			uFlag = APPBAR_DOCKING_TOP;
		else if (rcScreen.right - ptCursor.x<10)
			uFlag = APPBAR_DOCKING_RIGHT;
		else if (rcScreen.bottom - ptCursor.y<10)
			uFlag = APPBAR_DOCKING_BOTTOM;

		//Check if the edge is allowed by initial flags
		if ((uFlag & m_uFlags) > 0)
			return uFlag;
		else
			return APPBAR_DOCKING_NONE;
	}

	//////////////////////////////////////////////////////////////
	// get uEdge value given docking flag
	UINT GetEdge(UINT uFlag)
	{
		UINT uEdge ;
		switch (uFlag)
		{
		case APPBAR_DOCKING_LEFT:
			uEdge = ABE_LEFT;
			break;
		case APPBAR_DOCKING_RIGHT:
			uEdge = ABE_RIGHT;
			break;
		case APPBAR_DOCKING_TOP:
			uEdge = ABE_TOP;
			break;
		case APPBAR_DOCKING_BOTTOM:
			uEdge = ABE_BOTTOM;
			break;
		default:
			uEdge = ABE_BOTTOM;
			ATLTRACE("Invalid argument");
			ATLASSERT(FALSE);
			break;
		}

		return uEdge;
	}

	//////////////////////////////////////////////////////////////////////
	// draw temp frame for target rect
	void DrawFrame(CRect rc)
	{
		enum
		{
			FRAME_BORDER_SIZE = 3
		};

		CWindowDC	dc(::GetDesktopWindow());
		CBrush brush = CDCHandle::GetHalftoneBrush();
		if(brush.m_hBrush != NULL)
		{
			HBRUSH hBrushOld = dc.SelectBrush(brush);
			dc.PatBlt(rc.left, rc.top, rc.Width(), FRAME_BORDER_SIZE, PATINVERT);
			dc.PatBlt(rc.left, rc.bottom-FRAME_BORDER_SIZE, rc.Width(), FRAME_BORDER_SIZE, PATINVERT);
			dc.PatBlt(rc.left, rc.top+FRAME_BORDER_SIZE, FRAME_BORDER_SIZE, rc.Height()-FRAME_BORDER_SIZE*2, PATINVERT);
			dc.PatBlt(rc.right-FRAME_BORDER_SIZE, rc.top+FRAME_BORDER_SIZE, FRAME_BORDER_SIZE, rc.Height()-FRAME_BORDER_SIZE*2, PATINVERT);

			dc.SelectBrush(hBrushOld);
		}
	}

	void DoHideAppBar()
	{
		if (!m_bHiding)
		{
			m_bHiding = true;
			SlideWindow(false);
		}
	}

	void DoShowAppBar()
	{
		if (m_bHiding)
		{
			SlideWindow(true);
			m_bHiding = false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	//	Slides the AppBar off or from the edge of the screen when the AppBar
	//	has the AutoHide state set.
	void SlideWindow(bool bShow)
	{
		enum SlideTick
		{
			HIDE_TICKS = 80,
			SHOW_TICKS = 40,
			HIDING_BAR_SIZE = 3
		};

		T* pT = static_cast<T*>(this);

		CRect	rcWnd;
		pT->GetWindowRect(&rcWnd);
		int	dx = 0;
		int dy = 0;
		switch (GetEdge(m_uCurrentDockingSide))
		{
			case ABE_TOP:
				dy = HIDING_BAR_SIZE - rcWnd.Height();
				if (bShow)
					dy = - dy;
				break;
			case ABE_BOTTOM:
				dy = rcWnd.Height()-HIDING_BAR_SIZE;
				if (bShow)
					dy = -dy;
				break;
			case ABE_LEFT:
				dx = HIDING_BAR_SIZE-rcWnd.Width();
				if (bShow)
					dx = -dx;
				break;
			case ABE_RIGHT:
				dx = rcWnd.Width()-HIDING_BAR_SIZE;
				if (bShow)
					dx = -dx;
				break;
			default:
				return;
		}

		//Do move
		HANDLE hThreadMe = ::GetCurrentThread();
		int priority = ::GetThreadPriority(hThreadMe);
		::SetThreadPriority(hThreadMe, THREAD_PRIORITY_HIGHEST);

		int x, y, dt, t, t0;
		dt = bShow? SHOW_TICKS : HIDE_TICKS;

		t0 = GetTickCount();
		while ((t = GetTickCount()) < t0 + dt)
		{
			x = rcWnd.left + dx * (t - t0) / dt;
			y = rcWnd.top + dy * (t - t0) / dt;

			pT->SetWindowPos( NULL, x, y, 0, 0,
						SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			if (bShow)
				pT->UpdateWindow();
			else
				::UpdateWindow(GetDesktopWindow());
		}

		SetThreadPriority(hThreadMe, priority);

		//Ajust position to the right place
		pT->SetWindowPos(NULL, rcWnd.left+dx, rcWnd.top+dy,
					0, 0,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_NOSIZE);
		if (bShow)
		{
			SendMessage(pT->m_hWnd, WM_USER_SLIDE_OUT, 0, 0);
		}
		else
		{
			SendMessage(pT->m_hWnd, WM_USER_SLIDE_IN, 0, 0);
		}

	}


};
