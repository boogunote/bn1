#pragma once

#include "BooguNoteConfig.h"
#include <vector>
using namespace std;


extern CBooguNoteConfig g_config;

class CTextBlockPage : public CPropertyPageImpl<CTextBlockPage>,
                            public CWinDataExchange<CTextBlockPage>
{
public:
    enum { IDD = IDD_TEXT_BLOCK };

	bool m_bDirty;
	bool bAutoWidth;
	int	 narrowWidth;
	int  wideWidth;
	int	 nHandleWidth;
	int  nHeadLength;
	bool bShowCharCountInShrinkTB;

	CTextBlockPage();
	void ShowScrollBarItem();


	BEGIN_MSG_MAP(CTextBlockPage)
        MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		COMMAND_HANDLER(IDC_CHECK_AUTO_NARROW_WIDTH, BN_CLICKED, OnBnClickedCheckAutoNarrowWidth)
		CHAIN_MSG_MAP(CPropertyPageImpl<CTextBlockPage>)
    END_MSG_MAP()

	BEGIN_DDX_MAP(CTextBlockPage)
		DDX_CHECK(IDC_CHECK_AUTO_NARROW_WIDTH, bAutoWidth)
		DDX_UINT_RANGE(IDC_EDIT_NARROW_WIDTH, narrowWidth, 1, 10000)
		DDX_UINT_RANGE(IDC_EDIT_WIDE_WIDTH, wideWidth, 1, 10000)
		DDX_UINT_RANGE(IDC_EDIT_SQUARE_BULLET_SIZE, nHandleWidth, 1, 10000)
		DDX_CHECK(IDC_CHECK_SHOW_TB_RIGHT_BORDER, g_config.bShowTextBlockRightBorder)
		DDX_CHECK(IDC_CHECK_ENABLE_WIDE_STATE, g_config.bEnterWideTextBlock)
		DDX_CHECK(IDC_CHECK_ENABLE_URLDETECT, g_config.bEnterURLDetect)
		DDX_UINT_RANGE(IDC_EDIT_SHRINK_CHARS, nHeadLength, 1, 10000)
		DDX_CHECK(IDC_CHECK_CHAR_COUNT, bShowCharCountInShrinkTB)
    END_DDX_MAP()
	
	// Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnBnClickedCheckAutoNarrowWidth(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	 
};