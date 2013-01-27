#ifndef _CBOOGUNOTECONFIG_H_
#define _CBOOGUNOTECONFIG_H_
#include "atlctrls.h"
//class CImageList;
class CBooguNoteConfig
{
public:
	int		fontSize;
	int		tabFontSize;
	TCHAR	fontName[LF_FACESIZE];
	TCHAR	tabFontName[LF_FACESIZE];
	TCHAR	captionFontName[LF_FACESIZE];
	int		wideWidth;
	int		narrowWidth;
	int		hostBorder;
	//COLORREF clrThemeDeep;
	//COLORREF clrThemeLight;
	//COLORREF clrThemeDark;
	COLORREF clrCaptionText;
	COLORREF clrCaptionButton;
	COLORREF clrTabButton;
	COLORREF clrTabCaptionText;
	COLORREF clrTabBorder;
	COLORREF clrBullet;
	COLORREF clrTextBlockBorder;
	COLORREF clrScrollBar;
	int		 nScrollBarTransparentRatio;
	COLORREF clrScrollBarBorder;
	COLORREF clrThumbnail;
	COLORREF clrThumbnailBorder;
	COLORREF clrTextBlockRightBorder;
	COLORREF clrFrame;
	COLORREF clrTabButtonShade;
	COLORREF clrFrameOuterBorder;
	COLORREF clrFrameInnerBorder;

	COLORREF clrSelectedTextBlockBackground;
	COLORREF clrTabBackground;

	COLORREF clrFocusedTextBlockBackground;
	COLORREF clrUnSelectedTextBlockBackground;
	COLORREF clrViewBackground;

	int		nIndent;
	CPoint	ptStart;
	HFONT	hFontNormal;
	//HFONT	hFontBold;
	//HFONT	hFontItalic;
	//HFONT	hFontBoldItalic;
	HFONT	hTabFontBold;
	HFONT	hCaptionFont;
	int		nHeadLength;
	int		nIconWidth;
	int		nCopyIndent;
	bool	bAutoAdjustFocusedTextBlockPos;
	bool	bEnterNewBlock;
	bool	bAutoWidth;
	int		nHandleWidth;
	int		nBlockMargin;
	int		nScrollbarMargin;
	int		nScrollbarWidth;
	bool	bAutoScrollBarLength;
	bool	bEnableHScrollBar;
	int		nHScrollbarLength;
	int		nVScrollbarLength;
	int		nTabBorder;
	int		eachSideWidthOfFoundString;
	bool	bWideTextBlock;
	bool	bShowTextBlockRightBorder;
	bool	bCopyTextWithSymbol;
	bool	bShowCharCountInShrinkTB;
	CImageList imageList;
	static const int nAccelListLen = 50;
	int nAccelGlobalCount;
	ACCEL AccelGlobal[nAccelListLen];
	int nAccelMainFrmCount ;
	ACCEL AccelMainFrm[nAccelListLen];
	int nAccelViewCount;
	ACCEL AccelView[nAccelListLen];
	CString strConfigFileName;
	int		nDockWidth;
	bool	bGlobalCopyWithDocUrl;
	int		nBlockStateAfterGlobalPaste;
	bool	bLaunchAtStartup;
	int		nSaveAllElapse;
	bool	bSaveToTxtFile;
	int		nSmallThumbnailHeight;
	int		nBigThumbnailHeight;
	TCHAR	szRootStorageDir[MAX_PATH];
	TCHAR	szLogFolder[MAX_PATH];
	TCHAR	szCaptionString[512];
	bool	bBooOpenInSameInstance;
	bool	bShowAllBranchLines;
	int		nCodePage;
	bool	bUseWorkingDirAsRootStrorageDir;
	//bool	bDirectPaste;
	int		nPasteType;
	TCHAR	szPasteDir[MAX_PATH*10];
	int		nSaveType;
	bool	bEnterWideTextBlock;
	bool	bEnterURLDetect;
	int		FrameState;
	int		DockSide;
	int		nPosX;
	int		nPosY;
	int		nWidth;
	int		nHeight;
	bool	bTopmost;

	TCHAR	szFileManDir[MAX_PATH];
	int		nFileManLeft;
	int		nFileManTop;
	int		nFileManHeight;
	int		nFileManWidth;
public:
	CBooguNoteConfig();
	void ResetConfig();
	void ResetMainFrmAccel();
	void ResetGlobalAccel();
	void ResetViewAccel();
	void WriteConfigToIni();
	void ReadConfigFromIni();
};

#endif // _CBOOGUNOTECONFIG_H_