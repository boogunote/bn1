#pragma once

#include "BooguOrgDir.h"

#define WM_USER_CHANG_DIR	WM_USER+2000
#define WM_USER_DIR_UP		WM_USER+2001

class CEditDir : public CWindowImpl<CEditDir, CEdit>
{
public:
	CString m_strDir;
public:
	BEGIN_MSG_MAP(CEditDir)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ((WORD)wParam == VK_RETURN)
		{
			::PostMessage(GetParent(), WM_USER_CHANG_DIR, 0, 0);			
		}
		
		bHandled = FALSE;
		return 0;
	}
};


class CBnFresh : public CWindowImpl<CBnFresh, CButton>
{
public:
	BEGIN_MSG_MAP(CBnFresh)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostMessage(GetParent(), WM_USER_CHANG_DIR, 0, 0);			

		bHandled = FALSE;
		return 0;
	}
};

class CBnUp : public CWindowImpl<CBnFresh, CButton>
{
public:
	BEGIN_MSG_MAP(CBnFresh)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostMessage(GetParent(), WM_USER_DIR_UP, 0, 0);			

		bHandled = FALSE;
		return 0;
	}
};

class CFileTreeView : public CWindowImpl<CFileTreeView>
{
public:
	CEditDir m_editDir;
	CBnFresh m_bnFresh;
	CBnUp m_bnUp;
	CBooguOrgDir m_Org;
public:
	BEGIN_MSG_MAP(CFileTreeView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_USER_CHANG_DIR, OnUserChangeDir)
		MESSAGE_HANDLER(WM_USER_DIR_UP, OnUserDirUp)
		//MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		//MESSAGE_HANDLER(WM_TIMER, OnTimer)
		//MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		//MESSAGE_HANDLER(WM_TASK_COMPLETED, OnTaskCompleted)
		if(WM_NOTIFY == uMsg && m_editDir.m_hWnd != ((NMHDR*)lParam)->hwndFrom)
		{ 
			bHandled = TRUE; 
			lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); 
			if(bHandled) 
				return TRUE; 
		}
	END_MSG_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnUserChangeDir(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnUserDirUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void DoPaint( CDCHandle dc, RECT rcPaint);

private:
	void RecordMemberVariable();
};