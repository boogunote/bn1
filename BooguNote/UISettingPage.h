
#pragma once

#include "BooguNoteConfig.h"
#include <vector>
using namespace std;


extern CBooguNoteConfig g_config;

class CUISettingPage : public CPropertyPageImpl<CUISettingPage>,
                            public CWinDataExchange<CUISettingPage>
{
public:
    enum { IDD = IDD_UI_SETTING };
	//CListViewCtrl m_wlcShortcut;
	bool m_bDirty;
	bool bAutoScrollBarLength;
	int	 nHScrollbarLength;
	bool bEnableHScrollBar;
	int	 nVScrollbarLength;
	int  nScrollbarWidth;
	int  nBigImageHeight;
	int  nSmallImageHeight;

	

    // Construction
    CUISettingPage();
	//bool ConfictDetect(ACCEL& accel, bool bGlobalKey, int nIndex);
	void ShowScrollBarItem();

    // Maps
    BEGIN_MSG_MAP(CUISettingPage)
        MSG_WM_INITDIALOG(OnInitDialog)
		//NOTIFY_HANDLER(IDC_LIST_SHORTCUT, NM_DBLCLK, OnDClick);
		COMMAND_HANDLER(IDC_BUTTON_VIEW, BN_CLICKED, OnBnClickedButtonView)
		COMMAND_HANDLER(IDC_BUTTON_TABNAME, BN_CLICKED, OnBnClickedButtonTabname)
		COMMAND_HANDLER(IDC_BUTTON_DEEP, BN_CLICKED, OnBnClickedButtonDeep)
		COMMAND_HANDLER(IDC_BUTTON_LIGHT, BN_CLICKED, OnBnClickedButtonLight)
		COMMAND_HANDLER(IDC_BUTTON_DARK, BN_CLICKED, OnBnClickedButtonDark)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnDeepStaticColor)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		COMMAND_HANDLER(IDC_CHECK_AUTO_SCROLLBAR_LENGTH, BN_CLICKED, OnBnClickedCheckAutoScrollbarLength)
		COMMAND_HANDLER(IDC_CHECK_ENABLE_BOTTOM_SCROLLBAR, BN_CLICKED, OnBnClickedCheckEnableBottomScrollbar)
		CHAIN_MSG_MAP(CPropertyPageImpl<CUISettingPage>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CUISettingPage)
		DDX_TEXT(IDC_EDIT_VIEW_FONTNAME, g_config.fontName)
		DDX_INT(IDC_EDIT_VIEW_FONTSIZE, g_config.fontSize)
		DDX_TEXT(IDC_EDIT_TAB_FONTNAME, g_config.tabFontName)
		DDX_INT(IDC_EDIT_TAB_FONTSIZE, g_config.tabFontSize)
		DDX_CHECK(IDC_CHECK_AUTO_SCROLLBAR_LENGTH, bAutoScrollBarLength)
		DDX_UINT_RANGE(IDC_EDIT_RIGHT_SCROLLBAR_LEN, nVScrollbarLength, 1, 10000)
		DDX_CHECK(IDC_CHECK_ENABLE_BOTTOM_SCROLLBAR, bEnableHScrollBar)
		DDX_UINT_RANGE(IDC_EDIT_BOTTOM_SCROLLBAR_LEN, nHScrollbarLength, 20, 10000)
		DDX_UINT_RANGE(IDC_EDIT_SCROLLBAR_WIDTH, nScrollbarWidth, 1, 10000)
		DDX_UINT_RANGE(IDC_EDIT_BIG_IMAGE_HEIGHT, nBigImageHeight, 1, 10000)
		DDX_UINT_RANGE(IDC_EDIT_SMALL_IMAGE_HEIGHT, nSmallImageHeight, 1, 10000)
		DDX_CHECK(IDC_CHECK_SHOWALL_BRANCHLINES, g_config.bShowAllBranchLines)
		//DDX_CHECK(IDC_CHECK_AUTO_NARROW_WIDTH, bAutoWidth)
		//DDX_UINT_RANGE(IDC_EDIT_NARROW_WIDTH, narrowWidth, 1, 10000)
		//DDX_UINT_RANGE(IDC_EDIT_WIDE_WIDTH, wideWidth, 1, 10000)
		//DDX_UINT_RANGE(IDC_EDIT_SQUARE_BULLET_SIZE, nHandleWidth, 1, 10000)
		//DDX_CHECK(IDC_CHECK_SHOW_TB_RIGHT_BORDER, g_config.bShowTextBlockRightBorder)
		//DDX_UINT_RANGE(IDC_EDIT_SHRINK_CHARS, nHeadLength, 1, 10000)
    END_DDX_MAP()

    // Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	
	//LRESULT OnDClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

    // Property page notification handlers
    //int OnApply();

    // DDX variables
    //int m_nColor, m_nPicture;
	LRESULT OnBnClickedButtonView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTabname(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonDeep(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonLight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonDark(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDeepStaticColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedCheckAutoScrollbarLength(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckEnableBottomScrollbar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckAutoNarrowWidth(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
