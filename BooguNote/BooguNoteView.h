// BooguNoteView.h : interface of the CBooguNoteView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"
#include "CoolContextMenu.h"
#include "BooguNoteText.h"
#include "Common.h"
#include <vector>
using namespace std;



class CDefaultSaveDirDlg : public CDialogImpl<CDefaultSaveDirDlg>,
	public CWinDataExchange<CDefaultSaveDirDlg>
{
public:
	enum { IDD = IDD_DIALOG_DEFAULT_SAVE_DIR };
	CEditImpl   m_wndText;
	CString		m_strText;
	CComboBox	m_cbFileType;
	CString		m_strExtension;

	// Construction
	CDefaultSaveDirDlg()
	{
	};

	~CDefaultSaveDirDlg()
	{
	};

	// Maps
	BEGIN_MSG_MAP(CDefaultSaveDirDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CDefaultSaveDirDlg)
		//DDX_CHECK(IDC_CHECK_DIRECT_PASTE, g_config.bDirectPaste)
		DDX_CONTROL(IDC_EDIT_CONTENT, m_wndText)
		DDX_TEXT(IDC_EDIT_CONTENT, m_strText)
	END_DDX_MAP()

	// Message handlers
	BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	LRESULT    OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT    OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

class XMLElement;

class CBooguNoteView : public CWindowImpl<CBooguNoteView>,
					   public CMessageFilter,
					   public CIdleHandler,
					   public CCoolContextMenu<CBooguNoteView>
{
public:
    HACCEL		m_haccelerator;
	CMenu		m_menuEdit;
	int			startPointX;
	int			startPointY;
	CBooguNoteText* m_pTextBlockHovering;
	bool		m_bRButtonDown;
	CPoint		m_ptDragStart;
	bool		m_bCancelRButtonUp;
	int			m_nHRange;
	int			m_nVRange;
	CRect		m_rcVScroll;
	CRect		m_rcHScroll;
	CRect		m_rcVThumb;
	CRect		m_rcHThumb;
	bool		m_VThumbDraged;
	bool		m_HThumbDraged;
	
	int			m_nOriginalOffsetX;
	int			m_nOriginalOffsetY;
public:
	CPoint		m_ptOffset;
	CBooguNoteText* m_pTextBlockFocused;
	int			m_nLastClicked;
	CString		m_fileDirectory;
	bool		m_bInitialized;
	bool		m_bCancelAutoAdjust;
	bool		m_bActive;
	bool		m_bDirty;
	vector<CBooguNoteText*> m_textBlockList;
	vector<CBooguNoteText*> undoList;
	vector<int> undoIndexList;
	int			m_nFocusedTextBlockBeforeKillFocus;
	int			m_nSingleLineHeight;
	CString		m_strDefaultSaveDir;
	CString		m_strDefaultImgExtension;

//private:
//	LRESULT CBooguNoteView::CreateTextControl(HWND hwndControl, HWND hwndParent,DWORD dwStyle,TCHAR *pszText,
//			LONG lLeft,LONG lTop,LONG lWidth,LONG lHeight,CTxtWinHost **pptec);
//protected:
//	vector<SEditControlInfo> reList;
//	SEditControlInfo aseciChildControl;
public:
	void AssociateImage(CMenuItemInfo& mii, MenuItemData * pMI);
	virtual BOOL OnIdle();
public:
	DECLARE_WND_CLASS(NULL)

	CBooguNoteView()
	{
		/*aseciChildControl.ptwh = NULL;
		aseciChildControl.fFocus = FALSE;*/
		m_menuEdit.m_hMenu = NULL;
		m_pTextBlockFocused=NULL;
		m_pTextBlockHovering=NULL;
		m_haccelerator = NULL;
		startPointX = 5;
		startPointY = 5;
		m_bRButtonDown = false;
		m_ptDragStart.x = -1;
		m_ptDragStart.y = -1;
		m_bCancelRButtonUp = false;
		m_nHRange = 0;
		m_nVRange = 0;
		m_ptOffset.x = 0;
		m_ptOffset.y = 0;
		m_VThumbDraged = false;
		m_HThumbDraged = false;
		m_bInitialized = false;
		m_bCancelAutoAdjust = false;
		m_nOriginalOffsetX = 0;
		m_nOriginalOffsetY = 0;
		m_nLastClicked = 0;
		m_bActive = false;
		m_bDirty = true;
		m_textBlockList.clear();
		undoList.clear();
		undoIndexList.clear();
		m_nFocusedTextBlockBeforeKillFocus = 0;
		m_nSingleLineHeight = -1;
		m_strDefaultSaveDir = L"";
		m_strDefaultImgExtension = L"png";
	}

	BOOL PreTranslateMessage(MSG* pMsg);

	//BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);
	#define DEFAULT_MESSAGE_HANDLER \
	{if (/*!bHandled&&*/m_pTextBlockFocused/* && (TEXT_BLOCK_SHRINK != m_pTextBlockFocused->m_nExpandState)*/) \
	{ \
		LRESULT lres = m_pTextBlockFocused->TxWindowProc(m_hWnd, uMsg, wParam, lParam); \
		bHandled = TRUE;\
	}} \

	BEGIN_MSG_MAP(CBooguNoteView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_IME_CHAR, OnImeChar)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		//MESSAGE_HANDLER(WM_SYSKEYDOWN, OnSysKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLBottonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLBottonUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLBottonDoubleClick)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRBottonDown)
		MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnRBottonDoubleClick)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRBottonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_USER_REQUESTRESIZE, OnUserRequestResize)
		MESSAGE_HANDLER(WM_USER_DEL_CLIPPED, OnUserDelClipped)
		MESSAGE_HANDLER(WM_USER_GET_FILE_DIR, OnUserGetFileDir)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnPaste)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnCut)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnCopy)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnUndo)
		COMMAND_ID_HANDLER(ID_EDIT_REDO, OnRedo)
		COMMAND_ID_HANDLER(ID_POPUPMENU_PLAINTEXT, OnPopupMenuPlainText)
		COMMAND_ID_HANDLER(ID_POPUPMENU_TODO, OnPopupMenuTodo)
		COMMAND_ID_HANDLER(ID_POPUPMENU_DONE, OnPopupMenuDone)
		COMMAND_ID_HANDLER(ID_POPUPMENU_CROSS, OnPopupMenuCross)
		COMMAND_ID_HANDLER(ID_POPUPMENU_IDEA, OnPopupMenuIdea)
		COMMAND_ID_HANDLER(ID_POPUPMENU_STAR, OnPopupMenuStar)
		COMMAND_ID_HANDLER(ID_POPUPMENU_QUESTION, OnPopupMenuQuestion)
		COMMAND_ID_HANDLER(ID_POPUPMENU_WARNING, OnPopupMenuWarning)
		//COMMAND_ID_HANDLER(ID_POPUPMENU_PASTE, OnPaste)
		//COMMAND_ID_HANDLER(ID_POPUPMENU_CUT, OnCut)
		//COMMAND_ID_HANDLER(ID_POPUPMENU_COPY, OnCopy)
		//COMMAND_ID_HANDLER(ID_POPUPMENU_CLEAN, OnClean)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnSave)
		COMMAND_ID_HANDLER(ID_VIEW_SAVEAS, OnSaveAs)
		COMMAND_ID_HANDLER(ID_VIEW_CREATE_CHILD, OnCreateChild)
		COMMAND_ID_HANDLER(ID_VIEW_CREATE_NEXT, OnCreateNext)
		COMMAND_ID_HANDLER(ID_VIEW_CREATE_PREV, OnCreatePrev)
		COMMAND_ID_HANDLER(ID_VIEW_CREATE_PARENT, OnCreateParent)
		COMMAND_ID_HANDLER(ID_VIEW_NEWLINE, OnCreateNewLine)
		COMMAND_ID_HANDLER(ID_VIEW_MOVE_LEFT, OnMoveLeft)
		COMMAND_ID_HANDLER(ID_VIEW_MOVE_RIGHT, OnMoveRight)
		COMMAND_ID_HANDLER(ID_VIEW_MOVE_UP, OnMoveUp)
		COMMAND_ID_HANDLER(ID_VIEW_MOVE_DOWN, OnMoveDown)
		COMMAND_ID_HANDLER(ID_VIEW_CLEAN, OnClean)
		COMMAND_ID_HANDLER(ID_VIEW_DELETE, OnDelete)
		COMMAND_ID_HANDLER(ID_VIEW_BOLD, OnBold)
		COMMAND_ID_HANDLER(ID_VIEW_PAGE_UP, OnPageUp)
		COMMAND_ID_HANDLER(ID_VIEW_PAGE_LEFT, OnPageLeft)
		COMMAND_ID_HANDLER(ID_VIEW_PAGE_DOWN, OnPageDown)
		COMMAND_ID_HANDLER(ID_VIEW_PAGE_RIGHT, OnPageRight)
		COMMAND_ID_HANDLER(ID_VIEW_HOME, OnHome)
		COMMAND_ID_HANDLER(ID_VIEW_END, OnEnd)
		COMMAND_ID_HANDLER(ID_VIEW_LEFT, OnLeft)
		COMMAND_ID_HANDLER(ID_VIEW_TIME_STAMP, OnTimeStamp)
		COMMAND_ID_HANDLER(ID_STATE_NARROW, OnStateNarrow)
		COMMAND_ID_HANDLER(ID_STATE_WIDE, OnStateWide)
		COMMAND_ID_HANDLER(ID_STATE_SHRINK, OnStateShrink)
		COMMAND_ID_HANDLER(ID_VIEW_SELECT_ALL, OnSelectAll)
		COMMAND_ID_HANDLER(ID_VIEW_SWITCH, OnSwitch)
		COMMAND_ID_HANDLER(ID_VIEW_ICON_PREV, OnIconPrev)
		COMMAND_ID_HANDLER(ID_VIEW_ICON_NEXT, OnIconNext)
		COMMAND_ID_HANDLER(ID_VIEW_SPLIT_TEXTBLOCK, OnSplitTextBlock)
		COMMAND_ID_HANDLER(ID_VIEW_OPEN_CLOSE_SUB_BLOCK, OnOpenCloseSubBlock)
		COMMAND_ID_HANDLER(ID_VIEW_EDIT_FILE_BLOCK, OnEditFileBlock)
		COMMAND_ID_HANDLER(ID_VIEW_LOCATE_FILE, OnLocateFile)
		COMMAND_ID_HANDLER(ID_VIEW_MOVE_FILE, OnMoveFile)
		COMMAND_ID_HANDLER(ID_POPUPMENU_FIND, OnFind)
		//COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_POPUPMENU_BRANCH, OnPopupmenuBranch)
		COMMAND_ID_HANDLER(ID_POPUPMENU_UNBRANCH, OnPopupmenuUnbranch)
		COMMAND_ID_HANDLER(ID_POPUPMENU_EXPANDALL, OnPopupmenuExpandall)
		COMMAND_ID_HANDLER(ID_OPERATION_JUMPTOPARENTNODE, OnOperationJumptoparentnode)
		COMMAND_ID_HANDLER(ID_POPUPMENU_TEXTCOLOR, OnPopupmenuTextcolor)
		COMMAND_ID_HANDLER(ID_POPUPMENU_BKGRDCOLOR, OnPopupmenuBkgrdcolor)
		COMMAND_ID_HANDLER(ID_POPUPMENU_MOVEFILES, OnPopupmenuMovefiles)
		COMMAND_ID_HANDLER(ID_POPUPMENU_COPYFILES, OnPopupmenuCopyfiles)
		COMMAND_ID_HANDLER(ID_POPUPMENU_OPEN_BOO_IN_EXPLORER, OnPopupmenuOpenBooInExplorer)
		COMMAND_ID_HANDLER(ID_POPUPMENU_DEFAULT_SAVE_DIR, OnPopupmenuDefaultSaveDir)
		COMMAND_ID_HANDLER(ID_POPUPMENU_RELATIVE_DIR, OnPopupmenuRelativeDir)
		COMMAND_ID_HANDLER(ID_POPUPMENU_ABSOLUTE_DIR, OnPopupmenuAbsoluteDir)
		COMMAND_ID_HANDLER(ID_POPUPMENU_EXTRACT, OnPopupmenuExtract)
		COMMAND_RANGE_HANDLER(ID_POPUPMENU_COLOR_BEGIN,ID_POPUPMENU_COLOR_END, OnTextColor);
		COMMAND_ID_HANDLER(ID_POPUPMENU_REPAIR_SHORTCUT, OnPopupmenuRepairShortcut)
			COMMAND_ID_HANDLER(ID_POPUPMENU_SHARE, OnPopupmenuShare)
			CHAIN_MSG_MAP(CCoolContextMenu<CBooguNoteView>)
		DEFAULT_MESSAGE_HANDLER
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = TRUE;
		return 1;
	}

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnImeChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//LRESULT OnSysKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLBottonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM &lParam, BOOL& /*bHandled*/);
	LRESULT OnLBottonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM &lParam, BOOL& /*bHandled*/);
	LRESULT OnLBottonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM &lParam, BOOL& /*bHandled*/);
	LRESULT OnRBottonDoubleClick(UINT uMsg, WPARAM wParam, LPARAM &lParam, BOOL& bHandled);
	LRESULT OnRBottonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRBottonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM &lParam, BOOL& /*bHandled*/);
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM &lParam, BOOL& /*bHandled*/);
	LRESULT OnInitMenuPopup(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUserRequestResize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUserGetFileDir(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUserDelClipped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnPopupMenuPlainText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupMenuTodo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupMenuDone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupMenuCross(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupMenuIdea(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupMenuStar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupMenuQuestion(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupMenuWarning(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuOpenBooInExplorer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuExtract(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClean(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateChild(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreatePrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateParent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateNewLine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMoveLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMoveRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMoveUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMoveDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBold(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPageUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPageLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPageDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPageRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTimeStamp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStateNarrow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStateWide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStateShrink(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSwitch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnIconPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnIconNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSplitTextBlock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpenCloseSubBlock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditFileBlock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLocateFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMoveFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	//////////////////////////////////////////
	CBooguNoteText* CreateTextBlock(int x, int y, int width, LPCTSTR initString);
	void DoPaint(HDC hdc);
	void DrawTranspRoundRect(HDC hDC, CRect &rc);
	void CalcScrollBar();
	void CalcLayout();
	void CopyToText();
	void CopyToBlock();
	void CleanAllSelected();
	void CleanAllClipped();
	void SerializeTextBlock(CBooguNoteText* pTextBlock, XMLElement* r, int minIncident);
	void UnserializeBuffer(BYTE* &pBuf);
	void ShrinkTextBlock(CBooguNoteText* pTextBlock);
	void SetSelectedItemIcon(DWORD Icon);
	bool OpenFile();
	CString GetFileName();
	void PopupMenu(const CPoint& pt);
	int Save();
	int SaveAs();
	int SaveBooFile(vector<CBooguNoteText*> &textBlockList,TCHAR* szFilePath, bool bUpdateFileName = false, bool bSaveFullPath = false);
	int SaveBooFileWithAttachments(TCHAR* szFilePath);
	//int SaveBooFileWithAttachments(TCHAR* szFilePath);
	int SaveMMFile(TCHAR* szFilePath, bool bUpdateFileName = false);
	bool LoadData();
	bool LoadData0006();
	bool LoadData0005();
	bool LoadOldData0003();
	bool LoadOldData0004();
	void CreateNextBlock(bool bNarrow);
	CBooguNoteText* CreateChildBlock(bool bNarrow);
	void CreatePrevBlock();
	void CreateParentBlock();
	int GetCurrFocusedBlock();
	void AutoAdjustFocusedBlockPosition();
	void CloseBlocks(int index);
	void ExpandBlocks(int index);
	void DeleteSelectedBlock();
	void MoveSelectedItemsLeft();
	void MoveSelectedItemsRight();
	void MoveSelectedItemsUp();
	void MoveSelectedItemsDown();
	void SetTextBold();
	void LoadAccel();
	void PageUp();
	void PageDown();
	void PageLeft();
	void PageRight();
	void Paste();
	void SwitchState(CBooguNoteText* pTB);
	void StateNarrow();
	void StateWide();
	void StateShrink();
	void SpliteTextBlock();
	int SaveTxtFile(TCHAR* szFilePath);
	bool LoadBooData();
	bool LoadBooData0006();
	bool LoadBooData0005();
	bool LoadMMData();
	void ProcessMMNodes(XMLElement* r, int nLevel);
	void CreateMMNodes(XMLElement* r, int nLevel, int nStartPoint);
	void PasteFiles(HDROP hDrop);
	void CreateBooNodes(vector<CBooguNoteText*> &textBlockList, XMLElement* r, int nLevel, int nStartPoint, bool bSaveFullPath, const CString& strFolder);
	void CreateBooNodesWithAttachments(XMLElement* r, int nLevel, int nStartPoint, const CString& strFolder);
	void ProcessBooNodes(XMLElement* r, int nDefaultLevel);
	void ProcessBooNodes0005(XMLElement* r, int nDefaultLevel);
	int ShowBranch(int nIndex, CDCHandle& dc);
	int SaveHtmlFile(TCHAR* szFilePath);
	int SaveHtmlFileWithAttachments(TCHAR* szFilePath);
	LRESULT FindInFiles(LPCTSTR szFileName, basic_string<TCHAR> searchDir, CBooguNoteText* pTextBlock);
	//void PopupToolTip(CBooguNoteText* pTB);
	//void DeleteToolTip();
	void ClientToDevice(POINT &pt)
	{
		pt.x += m_ptOffset.x;
		pt.y += m_ptOffset.y;
	}	 

	void DeviceToClient(POINT &pt)
	{
		pt.x -= m_ptOffset.x;
		pt.y -= m_ptOffset.y;
	}

	LRESULT OnPopupmenuBranch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuUnbranch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuExpandall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOperationJumptoparentnode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuTextcolor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuBkgrdcolor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuMovefiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuCopyfiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuDefaultSaveDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuRelativeDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuAbsoluteDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTextColor(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuRepairShortcut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupmenuShare(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
