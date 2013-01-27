/*
Module : WtlFileTreeCtrl.h
Purpose: Interface for an WTL class which provides a tree control similiar 
         to the left hand side of explorer

Copyright (c) 2003 by iLYA Solnyshkin. 
E-mail: isolnyshkin@yahoo.com 
All rights reserved.
*/
#if !defined( WTL_FILETREECTRL_H__)
#define WTL_FILETREECTRL_H__

//Class which encapsulates access to the System image list which contains
//all the icons used by the shell to represent the file system

#include <string>
#include <algorithm>
#include <vector>

#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <shlobj.h>
#include "resource.h"

const UINT WM_POPULATE_TREE = WM_APP + 1;
const UINT WM_ITEM_SELECTED = WM_APP + 2;

class CSystemImageList
{
public:
	CSystemImageList();
	~CSystemImageList();

	//Methods
	CImageList* GetImageList();

protected:
	CImageList m_ImageList;
	static int m_nRefCount;
};

class CBooguOrgDir : public CWindowImpl< CBooguOrgDir, CTreeViewCtrl>         
{
	typedef struct
	{
		TCHAR* szPath;
		HANDLE hQuit;
	} Param;
public:
	DECLARE_WND_SUPERCLASS(_T("BooguOrg_FolderTreeView"), CTreeViewCtrl::GetWndClassName())

	CBooguOrgDir::CBooguOrgDir()
	{
		m_bShowFiles		= TRUE;
		m_hSelItem			= NULL;
		m_bInternalSendExpand = FALSE;
		m_bLDragging		= FALSE;
		m_hitemDrag			= NULL;
		m_hitemDrop			= NULL;
		m_hitemCut			= NULL;
		m_haccelerator		= NULL;
		m_bEdit				= false;
	}

	virtual ~CBooguOrgDir(){};
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL SubclassWindow( HWND hWnd );
	HWND	Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szRoot = NULL);

// Public methods
	void		SetRootFolder( const std::wstring sPath );
	std::wstring GetRootFolder() const { return m_sRootFolder; };
	std::wstring	ItemToPath( HTREEITEM hItem );
	std::wstring	GetSelectedPath();
	HTREEITEM	SetSelectedPath( const std::wstring sPath, BOOL bExpanded = FALSE );
	void		SetShowFiles( BOOL bFiles );
	BOOL		GetShowFiles() const { return m_bShowFiles; };

	BEGIN_MSG_MAP(CBooguOrgDir)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClick)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_POPULATE_TREE, OnPopulateTree)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnItemExpanding )
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_BEGINDRAG, OnBeginDrag )
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ENDLABELEDIT , OnEndLabelEdit)
		DEFAULT_REFLECTION_HANDLER()
		COMMAND_ID_HANDLER(ID_POPMENU_CUT, OnPopmenuCut)
		COMMAND_ID_HANDLER(ID_POPMENU_RFR, OnPopmenuRfr)
		COMMAND_ID_HANDLER(ID_POPMENU_COPY, OnPopmenuCopy)
		COMMAND_ID_HANDLER(ID_POPMENU_PASTE, OnPopmenuPaste)
		COMMAND_ID_HANDLER(ID_POPMENU_EDIT, OnPopmenuEdit)
		COMMAND_ID_HANDLER(ID_POPMENU_DELETE, OnPopmenuDelete)
		COMMAND_ID_HANDLER(ID_POPMENU_NEW, OnPopmenuNew)
		COMMAND_ID_HANDLER(ID_POPMENU_NEWFOLDER, OnPopmenuNewfolder)
	END_MSG_MAP()
	
	LRESULT OnLButtonDblClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPopulateTree(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnItemExpanding(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnBeginDrag(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnBeginLabelEdit(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnEndLabelEdit(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	void OnViewRefresh();

	//virtual bool HitTest(int x, int y);
	virtual void DoPaint(HDC hdc);

	static unsigned __stdcall WatchDirectory(void* lpDir);
	static void ReadChanged(TCHAR* szMonitorDir);

protected:

	void      DisplayPath( const std::wstring sPath, HTREEITEM hParent, BOOL bUseSetRedraw = TRUE );
	void      DisplayDrives( HTREEITEM hParent, BOOL bUseSetRedraw = TRUE );
	int       GetIconIndex( const std::wstring sFilename );
	int       GetIconIndex( HTREEITEM hItem );
	int       GetSelIconIndex( const std::wstring sFilename );
	int       GetSelIconIndex( HTREEITEM hItem );
	BOOL      HasGotSubEntries( const std::wstring sDirectory );
	HTREEITEM InsertFileItem( const std::wstring sFile, const std::wstring sPath, HTREEITEM hParent );
	HTREEITEM FindSibling( HTREEITEM hParent, const std::wstring sItem );
	static int CompareByFilenameNoCase( std::wstring element1, std::wstring element2 );

	std::wstring      m_sRootFolder;
	BOOL             m_bShowFiles;
	HTREEITEM        m_hSelItem;
	HANDLE			m_hQuit;
	BOOL			m_bInternalSendExpand;

	CImageList	m_DragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop,m_hitemCut;
	HACCEL		m_haccelerator;
	ACCEL		m_Accel[10];
	bool		m_bEdit;
public:
	LRESULT OnPopmenuCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopmenuRfr(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopmenuCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopmenuPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopmenuEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopmenuDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopmenuNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopmenuNewfolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

#endif //WTL_FILETREECTRL_H__