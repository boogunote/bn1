
#pragma once

#include <vector>
using namespace std;

class CShortcutPage : public CPropertyPageImpl<CShortcutPage>,
                            public CWinDataExchange<CShortcutPage>
{
public:
    enum { IDD = IDD_SHORTCUT_PAGE };
	CListViewCtrl m_wlcShortcut;
	bool m_bDirty;
	//vector<int> keyIndexList;
	//vector<ACCEL> keyList;

    // Construction
    CShortcutPage();
	bool ConfictDetect(ACCEL& accel, bool bGlobalKey, int nIndex);

    // Maps
    BEGIN_MSG_MAP(CShortcutPage)
        MSG_WM_INITDIALOG(OnInitDialog)
		NOTIFY_HANDLER(IDC_LIST_SHORTCUT, NM_DBLCLK, OnDClick);
        CHAIN_MSG_MAP(CPropertyPageImpl<CShortcutPage>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CShortcutPage)
        /*DDX_RADIO(IDC_BLUE, m_nColor)
        DDX_RADIO(IDC_ALYSON, m_nPicture)*/
    END_DDX_MAP()

    // Message handlers
    BOOL OnInitDialog ( HWND hwndFocus, LPARAM lParam );
	LRESULT OnDClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

    // Property page notification handlers
    //int OnApply();

    // DDX variables
    //int m_nColor, m_nPicture;
};
