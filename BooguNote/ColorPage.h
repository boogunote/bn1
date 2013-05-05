#pragma once

#include "BooguNoteConfig.h"
using namespace std;


extern CBooguNoteConfig g_config;

class CColorPage : public CPropertyPageImpl<CColorPage>,
                            public CWinDataExchange<CColorPage>
{
public:
    enum { IDD = IDD_DIALOG_COLOR };	

    // Construction
    CColorPage();
	//void ShowScrollBarItem();

    // Maps
    BEGIN_MSG_MAP(CColorPage)
        MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnStaticColor)
		COMMAND_HANDLER(IDC_BUTTON_TAB_BUTTON, BN_CLICKED, OnBnClickedButtonTabButton)
		COMMAND_HANDLER(IDC_BUTTON_TAB_CAPTION_TEXT, BN_CLICKED, OnBnClickedButtonTabCaptionText)
		COMMAND_HANDLER(IDC_BUTTON_TAB_BACKGROUND, BN_CLICKED, OnBnClickedButtonTabBackground)
		COMMAND_HANDLER(IDC_BUTTON_TAB_SHADE, BN_CLICKED, OnBnClickedButtonTabShade)
		COMMAND_HANDLER(IDC_BUTTON_TB_BULLET, BN_CLICKED, OnBnClickedButtonTbBullet)
		COMMAND_HANDLER(IDC_BUTTON_TB_TEXTBLOCK_BORDER, BN_CLICKED, OnBnClickedButtonTbTextblockBorder)
		COMMAND_HANDLER(IDC_BUTTON_TB_SCROLLBAR, BN_CLICKED, OnBnClickedButtonTbScrollbar)
		COMMAND_HANDLER(IDC_BUTTON_SELECTED_BACKGROUND, BN_CLICKED, OnBnClickedButtonSelectedBackground)
		COMMAND_HANDLER(IDC_BUTTON_TB_RIGHTBORDER, BN_CLICKED, OnBnClickedButtonTbRightborder)
		COMMAND_HANDLER(IDC_BUTTON_THUMBNAIL, BN_CLICKED, OnBnClickedButtonThumbnail)
		COMMAND_HANDLER(IDC_BUTTON_FRAME_BKG, BN_CLICKED, OnBnClickedButtonFrameBkg)
		COMMAND_HANDLER(IDC_BUTTON_FRAME_INNER_BORDER, BN_CLICKED, OnBnClickedButtonFrameInnerBorder)
		COMMAND_HANDLER(IDC_BUTTON_FRAME_OUTER_BORDER, BN_CLICKED, OnBnClickedButtonFrameOuterBorder)
		COMMAND_HANDLER(IDC_BUTTON_FRAME_CAPTION_TITLE, BN_CLICKED, OnBnClickedButtonFrameCaptionTitle)
		COMMAND_HANDLER(IDC_BUTTON_CAPTION_BUTTON, BN_CLICKED, OnBnClickedButtonCaptionButton)
		COMMAND_HANDLER(IDC_BUTTON_TAB_BORDER, BN_CLICKED, OnBnClickedButtonTabBorder)
		COMMAND_HANDLER(IDC_BUTTON_SCROLLBAR_BORDER, BN_CLICKED, OnBnClickedButtonScrollbarBorder)
		COMMAND_HANDLER(IDC_BUTTON_THUMBNAIL_BORDER2, BN_CLICKED, OnBnClickedButtonThumbnailBorder2)
		COMMAND_HANDLER(IDC_BUTTON_TB_TEXTBLOCK_UNSELECTED, BN_CLICKED, OnBnClickedButtonTbTextblockUnselected)
		COMMAND_HANDLER(IDC_BUTTON_TB_TEXTBLOCK_FOCUSED, BN_CLICKED, OnBnClickedButtonTbTextblockFocused)
		COMMAND_HANDLER(IDC_BUTTON_VIEW_BKG, BN_CLICKED, OnBnClickedButtonViewBkg)
		CHAIN_MSG_MAP(CPropertyPageImpl<CColorPage>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CColorPage)
		DDX_UINT_RANGE(IDC_EDIT_TRANSP, g_config.nScrollBarTransparentRatio, 1, 10000)
    END_DDX_MAP()

    // Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );


    // Property page notification handlers
    //int OnApply();

    // DDX variables
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnStaticColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnBnClickedButtonTabButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTabCaptionText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTabBackground(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTabShade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTbBullet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTbTextblockBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTbScrollbar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonSelectedBackground(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTbRightborder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonThumbnail(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonFrameBkg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonFrameInnerBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonFrameOuterBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonFrameCaptionTitle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonCaptionButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTabBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonScrollbarBorder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonThumbnailBorder2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTbTextblockUnselected(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonTbTextblockFocused(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonViewBkg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
