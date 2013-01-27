#ifndef _CBOOGUNOTEWIDGET_H_
#define _CBOOGUNOTEWIDGET_H_

class CBooguNoteWidget
{
public:
	HWND m_hHostWindow;
	BOOL fFocus;
public:
	CBooguNoteWidget()
	{
		m_hHostWindow = NULL;
		fFocus=FALSE;
	}

	~CBooguNoteWidget(){}

	virtual BOOL HitTest(int x, int y) = 0;

	virtual void DoPaint(HDC hdc) = 0;
};

//class CTxtWinHost;
//
//class CBooguNoteEdit: public CBooguNoteWidget
//{
//private:
//	CBooguNoteEdit(){}
//public:
//	CTxtWinHost* ptwh;
//public:
//	CBooguNoteEdit(HWND hHostWindow, int _left, int _top, int _right, int _bottom);
//	~CBooguNoteEdit();
//	void DoPaint(WPARAM wParam, LPARAM lParam);
//	void SyncClientRect();
//};

#endif // _CBOOGUNOTEWIDGET_H_