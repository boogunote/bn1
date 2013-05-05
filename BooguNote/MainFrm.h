// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "AppBar.h"
#include "FileTreeView.h"
#include "ShareWnd.h"

class BooguNoteView;
class CSearchDlg;
class CNotifyWnd;

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler, public CAppBar<CMainFrame>
{
	HACCEL		m_haccelerator;
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CMenu		m_menuCaption;
	//CBooguNoteView m_view;
	CBooguNoteView* m_pCurrView;
	//CCommandBarCtrl m_CmdBar;
	vector<CBooguNoteView*> m_viewList;
	int m_nCaptionHeight;
	int m_nTopOfView;
	//int m_nShownView;
	int m_nHeightOfTabPanel;
	bool m_bShrinkTabList;
	//bool m_bDocked;
	int	 m_nFrameState;
	bool m_bAutoHide;
	int m_nTimerID;
	//bool	m_bEnableNcActive;
	//CToolTipCtrl ttTrayIcon;
	CNotifyWnd* m_pwndNotify;
	CFileTreeView m_FileTV;
	CShareWnd m_wndShare;
	vector<CBooguNoteView*> m_ActiveFileChain;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	void UpdateLayout(BOOL bResizeBars = TRUE);
	int GetActiveView();
	CRect GetTabRectByOrderNumber(int nOrder);
	CRect GetCloseButtonRect();
	CRect GetMaximizeButtonRect();
	CRect GetMinimizeButtonRect();
	CRect GetMenuButtonRect();
	static bool NameCompare(CBooguNoteView* e1, CBooguNoteView* e2);
	static bool Compare(basic_string<TCHAR> e1, basic_string<TCHAR> e2);
	void DrawTabHead(int index, CString str);
	void ActivePage(int index);
	void Dock();
	void UnDock();
	void SetTrayIcon(bool bSet);
	CPoint GetCurrCurPos();
	void PopFileList();
	int OpenFileByName(TCHAR* szFileName);
	//LRESULT UpgradeBooFile(basic_string<TCHAR> &searchDir, const bool &bSearchSubDir);
	bool CloseAll(bool record);
	void LoadAccel();
	void SaveAll();
	void Save();
	void SaveAs();
	void WriteRegistry();
	void WindowShown(bool bShown);
	//void OpenMMFile(TCHAR* szFileName);
	CBooguNoteView * CreateNewView();
	void SaveCopiedToFile(TCHAR* szTitle);
	//void ProcessMMNodes(XMLElement* r, CBooguNoteView* pView, int nLevel);
	LRESULT ShowNotification(TCHAR* szNofity);
	int WriteBooguNoteIni();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActive)
		//MESSAGE_HANDLER(WM_ACTIVATE, OnActive)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		////MESSAGE_HANDLER(WM_ENABLE, OnEnable)
		MESSAGE_HANDLER(WM_GETTEXT, OnGetText)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		//MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		//MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
		MESSAGE_HANDLER(WM_USER_FILERENAME, OnFileRename)
		MESSAGE_HANDLER(WM_USER_ICONNOTIFY, OnIconNotify)
		MESSAGE_HANDLER(WM_USER_MAINFRM_REFRESH, OnMainFrmRefresh)
		MESSAGE_HANDLER(WM_USER_MAINFRM_OPENFILE, OnMainFrmOpenFile)
		MESSAGE_HANDLER(WM_USER_SLIDE_OUT, OnUserSlideOut)
		MESSAGE_HANDLER(WM_USER_SLIDE_IN, OnUserSlideIn)
		MESSAGE_HANDLER(WM_USER_SHARE, OnUserShare)
		MESSAGE_HANDLER(WM_USER_REFRESH_FILE_TREE_VIEW, OnUserRefreshFileTreeView)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		COMMAND_ID_HANDLER(ID_CAPTION_DOCK, OnDock)
		COMMAND_ID_HANDLER(ID_CAPTION_AUTOHIDE, OnAutoHide)
		COMMAND_ID_HANDLER(ID_CAPTION_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_CAPTION_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_CAPTION_SEARCH, OnSearch)
		COMMAND_ID_HANDLER(ID_CAPTION_SAVEALL, OnSaveAll)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnSave)
		COMMAND_ID_HANDLER(ID_POPUPMENU_DEFAULT_SAVE_DIR, OnCaptureSetting)
		COMMAND_ID_HANDLER(ID_VIEW_SAVEAS, OnSaveAs)
		COMMAND_ID_HANDLER(ID_CAPTION_CLOSEALL, OnCloseAll)
		COMMAND_ID_HANDLER(ID_CAPTION_OPENSTRORAGEROOT, OnOpenStorageRoot)
		COMMAND_ID_HANDLER(ID_CAPTION_CLOSE, OnCaptionClose)
		COMMAND_ID_HANDLER(ID_CAPTION_PREVFILE, OnPrevFile)
		COMMAND_ID_HANDLER(ID_CAPTION_NEXTFILE, OnNextFile)
		//COMMAND_ID_HANDLER(ID_CAPTION_CONVERTFILE_FROM021TO030, OnConvertFileFrom021To030)
		COMMAND_ID_HANDLER(ID_CAPTION_PREFERENCE, OnPreference)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		//COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		//COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_VIEW_FILE_LIST, OnPopFileList)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_RANGE_HANDLER(ID_FILELIST_BASE,ID_FILELIST_BASE+10000, OnFileList);
		if(FRAME_STATE_DOCK == m_nFrameState || FRAME_STATE_DOCK_AUTOHIDE == m_nFrameState)
		{
			CHAIN_MSG_MAP(CAppBar<CMainFrame>)
		}
		//if(!m_viewList.empty())
		//	m_viewList[GetActiveView()]->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
		COMMAND_ID_HANDLER(ID_CAPTION_NORMAL_WINDOW, OnCaptionNormalWindow)
			COMMAND_ID_HANDLER(ID_CAPTION_HIDEN, OnCaptionHiden)
			COMMAND_ID_HANDLER(ID_CAPTIONMENU_TOPMOST, OnCaptionmenuTopMost)
			COMMAND_ID_HANDLER(ID_CAPTIONMENU_FILESTREEVIEW, OnCaptionmenuFilesTreeview)
			CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>);
	END_MSG_MAP()

	CMainFrame();

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnFileRename(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = TRUE;
		return 0;
	}
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		//Invalidate();
		SendMessage(WM_NCPAINT,0,0);
		int nCurrActiveView = GetActiveView();
		if (nCurrActiveView>=0)
		{
			m_viewList[nCurrActiveView]->SendMessage(WM_KILLFOCUS);
		}
		bHandled = TRUE;
		return -1;
	}
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		SendMessage(WM_NCPAINT,0,0);
		bHandled = FALSE;
		return -1;
	}

	LRESULT OnEnable(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		SendMessage(WM_NCPAINT,0,0);
		bHandled = TRUE;
		return -1;
	}

	LRESULT OnGetText(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		*((TCHAR*)lParam) = _T('\0');
		SendMessage(WM_NCPAINT,0,0);
		bHandled = TRUE;
		return -1;
	}
	
	//LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	//{
	//	SendMessage(WM_NCPAINT,0,0);
	//	bHandled = FALSE;
	//	return 0;
	//}
	LRESULT OnNcActive(UINT /*uMsg*/, WPARAM &wParam, LPARAM &lParam, BOOL& bHandled);
	//LRESULT OnActive(UINT /*uMsg*/, WPARAM &wParam, LPARAM &lParam, BOOL& bHandled);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAutoHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//LRESULT OnConvertFileFrom021To030(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPreference(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSaveAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCaptureSetting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCaptionClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpenStorageRoot(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPrevFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNextFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopFileList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMainFrmRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnMainFrmOpenFile(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnUserChildActive(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnUserSlideOut(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnUserSlideIn(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnUserRefreshFileTreeView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnUserShare(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnIconNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCaptionNormalWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCaptionHiden(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCaptionmenuTopMost(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCaptionmenuFilesTreeview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
