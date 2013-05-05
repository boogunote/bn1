#include "stdafx.h"
#include "resource.h"
#include "BooguNoteConfig.h"
#include "BooguNoteText.h"
#include "Common.h"

extern CAppModule _Module;
TCHAR strExePath[MAX_PATH+10];
bool firstLaunch = false;

CBooguNoteConfig::CBooguNoteConfig()
{
		ResetConfig();
		GetModuleFileName(_Module.m_hInst, strExePath, MAX_PATH);
		strConfigFileName = strExePath;
		strConfigFileName.Delete(strConfigFileName.ReverseFind(_T('\\'))+1, strConfigFileName.GetLength());
		strConfigFileName+=_T("\\BooguNote.ini");

		HANDLE hFile = CreateFileW(strConfigFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ERROR_FILE_NOT_FOUND == GetLastError())
		{
			CloseHandle(hFile);
			//CreateGuideFile();
			WriteConfigToIni();
			firstLaunch = true;
		}
		else
		{
			CloseHandle(hFile);
			ReadConfigFromIni();
		}
		hFontNormal = CreateFont(PointsToLogical(fontSize), 
					  0, 0, 0, 
					  0,
					  0,0,0,DEFAULT_CHARSET,0,0,
					  DEFAULT_QUALITY,
					  0,
					  fontName);

		//hFontItalic = CreateFont(PointsToLogical(fontSize), 
		//			  0, 0, 0, 
		//			  0,
		//			  TRUE,0,0,DEFAULT_CHARSET,0,0,
		//			  DEFAULT_QUALITY,
		//			  0,
		//			  fontName);

		hTabFontBold = CreateFont(PointsToLogical(tabFontSize), 
					  0, 0, 0, 
					  FW_BOLD,
					  0,0,0,DEFAULT_CHARSET,0,0,
					  DEFAULT_QUALITY,
					  0,
					  tabFontName);
		hCaptionFont = CreateFont(PointsToLogical(8), 
					  0, 0, 0, 
					  FW_BOLD,
					  0,0,0,DEFAULT_CHARSET,0,0,
					  DEFAULT_QUALITY,
					  0,
					  captionFontName);
}
void CBooguNoteConfig::ResetConfig()
{
	fontSize = 9;
	tabFontSize = 9;
	_tcsncpy(fontName,_T("ËÎÌו"),LF_FACESIZE);
	_tcsncpy(tabFontName, _T("Arial"),LF_FACESIZE);
	_tcsncpy(captionFontName, _T("Tahoma"),LF_FACESIZE);
	wideWidth = 10000;
	narrowWidth = 200;
	hostBorder = 0;
	ptStart.x = 2;
	ptStart.y = 2;
	nHeadLength = 50;
	nIconWidth = 16;
	nCopyIndent = 4;
	bAutoAdjustFocusedTextBlockPos = false;
	bEnterNewBlock = true;
	bAutoWidth = true;
	nHandleWidth = 8;
	nBlockMargin = 5;
	nScrollbarMargin = 1;
	nScrollbarWidth = 18;
	bAutoScrollBarLength = false;
	bEnableHScrollBar = true;
	nHScrollbarLength = 100;
	nVScrollbarLength = 100;
	nTabBorder = 5;
	eachSideWidthOfFoundString = 10;
	bWideTextBlock = false;
	bShowTextBlockRightBorder = false;
	bCopyTextWithSymbol = true;
	nDockWidth = 250;
	bGlobalCopyWithDocUrl = true;
	nBlockStateAfterGlobalPaste = 0;
	bShowCharCountInShrinkTB = true;
	bLaunchAtStartup = false;
	nSaveAllElapse = 5000;
	bSaveToTxtFile = false;

	clrTabButton =RGB(55,93,129);
	clrTabCaptionText = RGB(55,93,129);

	clrSelectedTextBlockBackground = RGB(201,226,252);
	clrTabBackground = RGB(201,226,252);
	clrTabBorder = RGB(201,226,252);

	clrBullet = RGB(86,146,212);
	clrTextBlockBorder = RGB(86,146,212);
	clrScrollBar = RGB(86,146,212);
	nScrollBarTransparentRatio = 30;
	clrScrollBarBorder = RGB(255,255,255);
	clrThumbnail = RGB(86,146,212);
	clrThumbnailBorder = RGB(255,255,255);
	clrTextBlockRightBorder = RGB(86,146,212);
	clrFrame = RGB(86,146,212);
	clrTabButtonShade = RGB(86,146,212);
	clrFrameOuterBorder = RGB(128,128,128);
	clrFrameInnerBorder = RGB(255,255,255);

	clrCaptionText = RGB(255,255,255);
	clrCaptionButton = RGB(255,255,255);

	clrFocusedTextBlockBackground = RGB(255,255,255);
	clrUnSelectedTextBlockBackground = RGB(255,255,255);
	clrViewBackground = RGB(255,255,255);

	nSmallThumbnailHeight = 50;
	nBigThumbnailHeight = 200;

	_stprintf(szRootStorageDir, _T("%s"), _T(""));
	_stprintf(szCaptionString, _T("%s"), _T("BooguNote"));

	bBooOpenInSameInstance = true;
	bShowAllBranchLines = false;

	nCodePage = 0;

	bUseWorkingDirAsRootStrorageDir = true;

	if (bUseWorkingDirAsRootStrorageDir)
	{
		TCHAR _strExePath[MAX_PATH+10];
		GetModuleFileName(_Module.m_hInst, _strExePath, MAX_PATH);
		CString strPath = _strExePath;
		strPath.Delete(strPath.ReverseFind(_T('\\')), strPath.GetLength());
		_tcsncpy(szRootStorageDir,strPath.GetBuffer(MAX_PATH), MAX_PATH);
	}

	//bDirectPaste = false;
	nPasteType = 0;
	szPasteDir[0] = _T('\0');
	nSaveType = 0;

	bEnterWideTextBlock = false;
	bEnterURLDetect     = false;
	bEnterSHRINK_SELF   = true;

	FrameState = FRAME_STATE_NORMAL;
	DockSide = DOCK_RIGHT;
	nPosX = 0;
	nPosY = 0;
	nWidth = FRAME_DEFAULT_WIDTH;
	nHeight = 20;

	bTopmost = false;

	_stprintf(szFileManDir, _T("%s"), _T(""));
	//CRect rc;
	//::GetWindowRect(GetDesktopWindow(), &rc);

	//CRect rect(0, 0, 300, 500);
	//rect.OffsetRect((rc.Width() - rect.Width()) / 2, (rc.Height() - rect.Height()) / 2);
	nFileManTop = 100;
	nFileManLeft = 200;
	nFileManWidth = 250;
	nFileManHeight = 300;

	ResetGlobalAccel();
	ResetMainFrmAccel();
	ResetViewAccel();
}

void CBooguNoteConfig::ResetGlobalAccel()
{
	int i=0;
	AccelGlobal[i].cmd = BOOGUNOTE_HOTKEY_DOCK;
	AccelGlobal[i].fVirt = MOD_WIN;
	AccelGlobal[i].key = 0x4F;
	++i;
	AccelGlobal[i].cmd = BOOGUNOTE_HOTKEY_HIDE;
	AccelGlobal[i].fVirt = MOD_WIN;
	AccelGlobal[i].key = 0x48;
	++i;
	AccelGlobal[i].cmd = BOOGUNOTE_HOTKEY_GLOBAL_PASET;
	AccelGlobal[i].fVirt = MOD_WIN;
	AccelGlobal[i].key = 0x56;
	++i;
	AccelGlobal[i].cmd = BOOGUNOTE_HOTKEY_FOCUSING;
	AccelGlobal[i].fVirt = MOD_WIN;
	AccelGlobal[i].key = 0x5A;
	++i;
	AccelGlobal[i].cmd = BOOGUNOTE_HOTKEY_DOCK_AUTOHIDE;
	AccelGlobal[i].fVirt = MOD_WIN|MOD_SHIFT;
	AccelGlobal[i].key = 0x4F;
	++i;
	AccelGlobal[i].cmd = BOOGUNOTE_HOTKEY_GLOBAL_PASET_TO_FILE;
	AccelGlobal[i].fVirt = MOD_WIN;
	AccelGlobal[i].key = 0x43;
	++i;
	AccelGlobal[i].cmd = BOOGUNOTE_HOTKEY_GLOBAL_SCREENSHOT;
	//AccelGlobal[i].fVirt = MOD_WIN;
	AccelGlobal[i].key = VK_SNAPSHOT;
	++i;
	nAccelGlobalCount = i;
}

void CBooguNoteConfig::ResetMainFrmAccel()
{
	int i=0;
	AccelMainFrm[i].cmd = ID_CAPTION_NEW;
	AccelMainFrm[i].fVirt = FCONTROL|FVIRTKEY;
	AccelMainFrm[i].key = 0x4E;
	++i;
	AccelMainFrm[i].cmd = ID_CAPTION_OPEN;
	AccelMainFrm[i].fVirt = FCONTROL|FVIRTKEY;
	AccelMainFrm[i].key = 0X4F;
	++i;
	AccelMainFrm[i].cmd = ID_CAPTION_PREVFILE;
	AccelMainFrm[i].fVirt = FCONTROL|FVIRTKEY;
	AccelMainFrm[i].key = VK_TAB;
	++i;
	AccelMainFrm[i].cmd = ID_CAPTION_NEXTFILE;
	AccelMainFrm[i].fVirt = FCONTROL|FSHIFT|FVIRTKEY;
	AccelMainFrm[i].key = VK_TAB;
	++i;
	AccelMainFrm[i].cmd = ID_CAPTION_SEARCH;
	AccelMainFrm[i].fVirt = FVIRTKEY;
	AccelMainFrm[i].key = VK_F3;
	++i;
	AccelMainFrm[i].cmd = ID_VIEW_FILE_LIST;
	AccelMainFrm[i].fVirt = FVIRTKEY;
	AccelMainFrm[i].key = VK_F4;
	++i;
	AccelMainFrm[i].cmd = ID_CAPTION_PREFERENCE;
	AccelMainFrm[i].fVirt = FCONTROL|FALT|FVIRTKEY;
	AccelMainFrm[i].key = 0x50;
	++i;
	AccelMainFrm[i].cmd = ID_CAPTION_CLOSE;
	AccelMainFrm[i].fVirt = FCONTROL|FVIRTKEY;
	AccelMainFrm[i].key = 0x57;
	++i;
	AccelMainFrm[i].cmd = ID_CAPTIONMENU_FILESTREEVIEW;
	AccelMainFrm[i].fVirt = FVIRTKEY;
	AccelMainFrm[i].key = VK_F5;
	++i;

	nAccelMainFrmCount = i;
}

void CBooguNoteConfig::ResetViewAccel()
{
	int i=0;
	AccelView[i].cmd = ID_EDIT_COPY;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x43;

	++i;

	AccelView[i].cmd = ID_EDIT_CUT;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x58;

	++i;

	AccelView[i].cmd = ID_EDIT_PASTE;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x56;

	++i;

	AccelView[i].cmd = ID_EDIT_REDO;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x59;

	++i;

	AccelView[i].cmd = ID_EDIT_UNDO;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x5A;

	++i;

	AccelView[i].cmd = ID_VIEW_TIME_STAMP;
	AccelView[i].fVirt = FVIRTKEY;
	AccelView[i].key = VK_F12;

	++i;

	AccelView[i].cmd = ID_FILE_SAVE;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x53;

	++i;

	AccelView[i].cmd = ID_VIEW_SAVEAS;
	AccelView[i].fVirt = FCONTROL|FSHIFT|FVIRTKEY;
	AccelView[i].key = 0x53;

	++i;

	AccelView[i].cmd = ID_VIEW_BOLD;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x42;

	++i;

	AccelView[i].cmd = ID_VIEW_CLEAN;
	AccelView[i].fVirt = FVIRTKEY;
	AccelView[i].key = VK_ESCAPE;

	++i;

	AccelView[i].cmd = ID_VIEW_CREATE_CHILD;
	AccelView[i].fVirt = FSHIFT|FVIRTKEY;
	AccelView[i].key = VK_RETURN;

	++i;

	AccelView[i].cmd = ID_VIEW_CREATE_NEXT;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_RETURN;

	++i;

	AccelView[i].cmd = ID_VIEW_CREATE_PREV;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = VK_RETURN;

	++i;

	AccelView[i].cmd = ID_VIEW_CREATE_PARENT;
	AccelView[i].fVirt = FCONTROL|FSHIFT|FVIRTKEY;
	AccelView[i].key = VK_RETURN;

	++i;

	AccelView[i].cmd = ID_VIEW_DELETE;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_DELETE;

	++i;

	AccelView[i].cmd = ID_VIEW_HOME;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_HOME;

	++i;

	AccelView[i].cmd = ID_VIEW_END;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_END;

	++i;

	AccelView[i].cmd = ID_VIEW_LEFT;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = VK_HOME;

	++i;

	AccelView[i].cmd = ID_VIEW_MOVE_UP;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_UP;

	++i;

	AccelView[i].cmd = ID_VIEW_MOVE_DOWN;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_DOWN;

	++i;

	AccelView[i].cmd = ID_VIEW_MOVE_LEFT;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_LEFT;

	++i;

	AccelView[i].cmd = ID_VIEW_MOVE_RIGHT;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_RIGHT;

	++i;

	AccelView[i].cmd = ID_VIEW_NEWLINE;
	AccelView[i].fVirt = FSHIFT|FVIRTKEY;
	AccelView[i].key = VK_RETURN;

	++i;

	AccelView[i].cmd = ID_VIEW_PAGE_UP;
	AccelView[i].fVirt = FVIRTKEY;
	AccelView[i].key = VK_PRIOR;
	
	++i;

	AccelView[i].cmd = ID_VIEW_PAGE_DOWN;
	AccelView[i].fVirt = FVIRTKEY;
	AccelView[i].key = VK_NEXT;

	++i;

	AccelView[i].cmd = ID_VIEW_PAGE_LEFT;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_PRIOR;

	++i;

	AccelView[i].cmd = ID_VIEW_PAGE_RIGHT;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_NEXT;

	++i;

	AccelView[i].cmd = ID_VIEW_SELECT_ALL;
	AccelView[i].fVirt = FCONTROL|FALT|FVIRTKEY;
	AccelView[i].key = 0x41;

	++i;

	AccelView[i].cmd = ID_VIEW_SWITCH;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = 0x55;

	++i;

	AccelView[i].cmd = ID_VIEW_ICON_PREV;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_OEM_MINUS;

	++i;

	AccelView[i].cmd = ID_VIEW_ICON_NEXT;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_OEM_PLUS;

	++i;

	AccelView[i].cmd = ID_VIEW_SPLIT_TEXTBLOCK;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_OEM_5;

	++i;

	AccelView[i].cmd = ID_VIEW_OPEN_CLOSE_SUB_BLOCK;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = 0x5A;

	++i;

	AccelView[i].cmd = ID_POPUPMENU_FIND;
	AccelView[i].fVirt = FCONTROL|FVIRTKEY;
	AccelView[i].key = VK_F3;

	++i;

	AccelView[i].cmd = ID_POPUPMENU_BRANCH;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = 0x42; //"B"

	++i;

	AccelView[i].cmd = ID_POPUPMENU_UNBRANCH;
	AccelView[i].fVirt = FALT|FSHIFT|FVIRTKEY;
	AccelView[i].key = 0x42; //"B"

	++i;

	AccelView[i].cmd = ID_POPUPMENU_EXPANDALL;
	AccelView[i].fVirt = FALT|FSHIFT|FVIRTKEY;
	AccelView[i].key = 0x5A;//"Z"

	++i;

	AccelView[i].cmd = ID_OPERATION_JUMPTOPARENTNODE;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = VK_LEFT;

	++i;

	AccelView[i].cmd = ID_POPUPMENU_TEXTCOLOR;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = 0x54;//"T"

	++i;

	AccelView[i].cmd = ID_POPUPMENU_BKGRDCOLOR;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = 0x47;//"G"

	++i;

	AccelView[i].cmd = ID_POPUPMENU_EXTRACT;
	AccelView[i].fVirt = FALT|FVIRTKEY;
	AccelView[i].key = 0x53;//"S"

	++i;

	nAccelViewCount = i;

}

void CBooguNoteConfig::WriteConfigToIni()
{
	HANDLE hFile = CreateFileW(strConfigFileName, GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	CloseHandle(hFile);
	TCHAR buf[64*1024];
	WritePrivateProfileString(_T("Font"),_T("ViewFontName"), fontName,strConfigFileName);
	_itot(fontSize, buf, 10);
	WritePrivateProfileString(_T("Font"),_T("ViewFontSize"), buf,strConfigFileName);
	WritePrivateProfileString(_T("Font"),_T("TabFontName"), tabFontName,strConfigFileName);
	_itot(tabFontSize, buf, 10);
	WritePrivateProfileString(_T("Font"),_T("TabFontSize"), buf,strConfigFileName);

	BinaryToString((BYTE*)&bAutoScrollBarLength, sizeof(bAutoScrollBarLength), buf);
	WritePrivateProfileString(_T("Scrollbar"),_T("AutoScrollBarLength"),buf,strConfigFileName);
	BinaryToString((BYTE*)&bEnableHScrollBar, sizeof(bEnableHScrollBar), buf);
	WritePrivateProfileString(_T("Scrollbar"),_T("EnableHScrollBar"),buf,strConfigFileName);
	_itot(nVScrollbarLength, buf, 10);
	WritePrivateProfileString(_T("Scrollbar"),_T("VScrollbarLength"),buf,strConfigFileName);
	_itot(nHScrollbarLength, buf, 10);
	WritePrivateProfileString(_T("Scrollbar"),_T("HScrollbarLength"),buf,strConfigFileName);
	_itot(nScrollbarWidth, buf, 10);
	WritePrivateProfileString(_T("Scrollbar"),_T("ScrollbarWidth"),buf,strConfigFileName);

	_itot(nBigThumbnailHeight, buf, 10);
	WritePrivateProfileString(_T("Image"),_T("BigThumbnailHeight"),buf,strConfigFileName);
	_itot(nSmallThumbnailHeight, buf, 10);
	WritePrivateProfileString(_T("Image"),_T("SmallThumbnailHeight"),buf,strConfigFileName);

	//BinaryToString((BYTE*)&, sizeof(COLORREF), buf);
	//WritePrivateProfileString(_T("Color"),_T("Light"),buf,strConfigFileName);
	//BinaryToString((BYTE*)&clrThemeDeep, sizeof(COLORREF), buf);
	//WritePrivateProfileString(_T("Color"),_T("Deep"),buf,strConfigFileName);
	//BinaryToString((BYTE*)&clrThemeDark, sizeof(COLORREF), buf);
	//WritePrivateProfileString(_T("Color"),_T("Dark"),buf,strConfigFileName);

	BinaryToString((BYTE*)&clrCaptionText, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrCaptionText"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrCaptionButton, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrCaptionButton"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrTabButton, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrTabButton"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrTabCaptionText, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrTabCaptionText"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrTabBorder, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrTabBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrBullet, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrBullet"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrTextBlockBorder, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrTextBlockBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrScrollBar, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrScrollBar"),buf,strConfigFileName);
	_itot(nScrollBarTransparentRatio, buf, 10);
	WritePrivateProfileString(_T("Color"),_T("nScrollBarTransparentRatio"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrScrollBarBorder, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrScrollBarBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrThumbnail, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrThumbnail"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrThumbnailBorder, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrThumbnailBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrTextBlockRightBorder, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrTextBlockRightBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrFrame, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrFrame"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrTabButtonShade, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrTabButtonShade"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrFrameOuterBorder, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrFrameOuterBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrFrameInnerBorder, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrFrameInnerBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrSelectedTextBlockBackground, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrSelectedTextBlockBackground"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrTabBackground, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrTabBackground"),buf,strConfigFileName);

	BinaryToString((BYTE*)&clrFocusedTextBlockBackground, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrFocusedTextBlockBackground"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrUnSelectedTextBlockBackground, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrUnSelectedTextBlockBackground"),buf,strConfigFileName);
	BinaryToString((BYTE*)&clrViewBackground, sizeof(COLORREF), buf);
	WritePrivateProfileString(_T("Color"),_T("clrViewBackground"),buf,strConfigFileName);

	BinaryToString((BYTE*)&bAutoWidth, sizeof(bAutoWidth), buf);
	WritePrivateProfileString(_T("TextBlock"),_T("AutoWidth"),buf,strConfigFileName);
	BinaryToString((BYTE*)&bShowTextBlockRightBorder, sizeof(bShowTextBlockRightBorder), buf);
	WritePrivateProfileString(_T("TextBlock"),_T("ShowTextBlockRightBorder"),buf,strConfigFileName);
	BinaryToString((BYTE*)&bEnterWideTextBlock, sizeof(bEnterWideTextBlock), buf);
	WritePrivateProfileString(_T("TextBlock"),_T("EnterWideTextBlock"),buf,strConfigFileName);

	BinaryToString((BYTE*)&bEnterURLDetect, sizeof(bEnterURLDetect), buf);
	WritePrivateProfileString(_T("TextBlock"),_T("EnterURLDetectBlock"),buf,strConfigFileName);

	BinaryToString((BYTE*)&bEnterSHRINK_SELF, sizeof(bEnterSHRINK_SELF), buf);
	WritePrivateProfileString(_T("TextBlock"),_T("EnterSHRINK_SELF"),buf,strConfigFileName);

	_itot(narrowWidth, buf, 10);
	WritePrivateProfileString(_T("TextBlock"),_T("NarrowWidth"),buf,strConfigFileName);
	_itot(wideWidth, buf, 10);
	WritePrivateProfileString(_T("TextBlock"),_T("WideWidth"),buf,strConfigFileName);
	_itot(nHandleWidth, buf, 10);
	WritePrivateProfileString(_T("TextBlock"),_T("HandleWidth"),buf,strConfigFileName);
	_itot(nHeadLength, buf, 10);
	WritePrivateProfileString(_T("TextBlock"),_T("HeadLength"),buf,strConfigFileName);
	_itot(bShowCharCountInShrinkTB, buf, 10);
	WritePrivateProfileString(_T("TextBlock"),_T("ShowCharCountInShrinkTB"),buf,strConfigFileName);

	_itot(eachSideWidthOfFoundString, buf, 10);
	WritePrivateProfileString(_T("Search"),_T("EachSideWidthOfFoundString"),buf,strConfigFileName);
	BinaryToString((BYTE*)&bCopyTextWithSymbol, sizeof(bCopyTextWithSymbol), buf);
	WritePrivateProfileString(_T("CopyPast"),_T("CopyTextWithSymbol"),buf,strConfigFileName);
	_itot(nCopyIndent, buf, 10);
	WritePrivateProfileString(_T("CopyPast"),_T("CopyIndent"),buf,strConfigFileName);
	BinaryToString((BYTE*)&bGlobalCopyWithDocUrl, sizeof(bGlobalCopyWithDocUrl), buf);
	WritePrivateProfileString(_T("CopyPast"),_T("GlobalCopyWithDocUrl"),buf,strConfigFileName);
	_itot(nBlockStateAfterGlobalPaste, buf, 10);
	WritePrivateProfileString(_T("CopyPast"),_T("BlockStateAfterGlobalPaste"),buf,strConfigFileName);


	int nAccelGlobalListSize = nAccelGlobalCount*sizeof(ACCEL);
	BinaryToString((BYTE*)AccelGlobal, nAccelGlobalListSize, buf);
	WritePrivateProfileString(_T("Shortcut"),_T("Global"),buf,strConfigFileName);
	int nAccelMainFrmListSize = nAccelMainFrmCount*sizeof(ACCEL);
	BinaryToString((BYTE*)AccelMainFrm, nAccelMainFrmListSize, buf);
	WritePrivateProfileString(_T("Shortcut"),_T("MainFrm"),buf,strConfigFileName);
	int nAccelViewListSize = nAccelViewCount*sizeof(ACCEL);
	BinaryToString((BYTE*)AccelView, nAccelViewListSize, buf);
	WritePrivateProfileString(_T("Shortcut"),_T("View"),buf,strConfigFileName);

	_itot(nDockWidth, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("DockedWidth"),buf,strConfigFileName);
	BinaryToString((BYTE*)&bLaunchAtStartup, sizeof(bLaunchAtStartup), buf);
	WritePrivateProfileString(_T("MainFrame"),_T("LaunchAtStartup"),buf,strConfigFileName);
	_itot(nSaveAllElapse, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("SaveAllElapse"),buf,strConfigFileName);
	WritePrivateProfileString(_T("MainFrame"),_T("Version"),_T("0.3.3.1"),strConfigFileName);
	BinaryToString((BYTE*)&bSaveToTxtFile, sizeof(bSaveToTxtFile), buf);
	WritePrivateProfileString(_T("MainFrame"),_T("SaveToTxtFile"),buf,strConfigFileName);
	//WritePrivateProfileString(_T("MainFrame"),_T("RootStorageDir"),szRootStorageDir,strConfigFileName);
	WritePrivateProfileString(_T("MainFrame"),_T("LogFolder"),szLogFolder,strConfigFileName);
	WritePrivateProfileString(_T("MainFrame"),_T("CaptionString"),szCaptionString,strConfigFileName);
	BinaryToString((BYTE*)&bBooOpenInSameInstance, sizeof(bBooOpenInSameInstance), buf);
	WritePrivateProfileString(_T("MainFrame"),_T("BooOpenInSameInstance"),buf,strConfigFileName);
	BinaryToString((BYTE*)&bShowAllBranchLines, sizeof(bShowAllBranchLines), buf);
	WritePrivateProfileString(_T("MainFrame"),_T("ShowAllBranchLines"),buf,strConfigFileName);
	_itot(nCodePage, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("CodePage"),buf,strConfigFileName);
	//BinaryToString((BYTE*)&bDirectPaste, sizeof(bDirectPaste), buf);
	//WritePrivateProfileString(_T("MainFrame"),_T("DirectPaste"),buf,strConfigFileName);
	WritePrivateProfileString(_T("MainFrame"),_T("PasteDir"),szPasteDir,strConfigFileName);
	_itot(nSaveType, buf, 10);
	WritePrivateProfileString(_T("CopyPast"),_T("SaveType"),buf,strConfigFileName);
	_itot(FrameState, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("FrameState"),buf,strConfigFileName);
	_itot(DockSide, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("DockSide"),buf,strConfigFileName);
	_itot(nPosX, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("PosX"),buf,strConfigFileName);
	_itot(nPosY, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("PosY"),buf,strConfigFileName);
	_itot(nWidth, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("Width"),buf,strConfigFileName);
	_itot(nHeight, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("Height"),buf,strConfigFileName);
	WritePrivateProfileString(_T("MainFrame"),_T("FileManDir"),szFileManDir,strConfigFileName);
	_itot(nFileManLeft, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("FileManLeft"),buf,strConfigFileName);
	_itot(nFileManTop, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("FileManTop"),buf,strConfigFileName);
	_itot(nFileManWidth, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("FileManWidth"),buf,strConfigFileName);
	_itot(nFileManHeight, buf, 10);
	WritePrivateProfileString(_T("MainFrame"),_T("FileManHeight"),buf,strConfigFileName);
}

void CBooguNoteConfig::ReadConfigFromIni()
{
	int nAccelListSize = nAccelListLen*sizeof(ACCEL);
	//TCHAR* buf = new TCHAR[nAccelListSize*2+10];

	TCHAR buf[64*1024];

	GetPrivateProfileString(_T("Shortcut"),_T("Global"),_T(""),buf, nAccelGlobalCount*sizeof(ACCEL)*2+10,strConfigFileName);
	StringToBinary(buf, lstrlen(buf), (BYTE*)AccelGlobal);
	GetPrivateProfileString(_T("Shortcut"),_T("MainFrm"),_T(""),buf, nAccelMainFrmCount*sizeof(ACCEL)*2+10,strConfigFileName);
	StringToBinary(buf, lstrlen(buf), (BYTE*)AccelMainFrm);
	GetPrivateProfileString(_T("Shortcut"),_T("View"),_T(""),buf, nAccelViewCount*sizeof(ACCEL)*2+10,strConfigFileName);
	StringToBinary(buf, lstrlen(buf), (BYTE*)AccelView);

	GetPrivateProfileString(_T("Font"),_T("ViewFontName"), _T(""),fontName, LF_FACESIZE,strConfigFileName);
	GetPrivateProfileString(_T("Font"),_T("ViewFontSize"), _T(""),buf, 100, strConfigFileName);
	fontSize = _tstoi(buf);
	GetPrivateProfileString(_T("Font"),_T("TabFontName"), _T(""),tabFontName, LF_FACESIZE,strConfigFileName);
	GetPrivateProfileString(_T("Font"),_T("TabFontSize"), _T(""),buf, 100, strConfigFileName);
	tabFontSize = _tstoi(buf);

	GetPrivateProfileString(_T("Scrollbar"),_T("AutoScrollBarLength"),_T(""),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bAutoScrollBarLength);
	GetPrivateProfileString(_T("Scrollbar"),_T("EnableHScrollBar"),_T(""),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bEnableHScrollBar);
	GetPrivateProfileString(_T("Scrollbar"),_T("VScrollbarLength"), _T(""),buf, 100, strConfigFileName);
	nVScrollbarLength = _tstoi(buf);
	GetPrivateProfileString(_T("Scrollbar"),_T("HScrollbarLength"), _T(""),buf, 100, strConfigFileName);
	nHScrollbarLength = _tstoi(buf);
	GetPrivateProfileString(_T("Scrollbar"),_T("ScrollbarWidth"), _T(""),buf, 100, strConfigFileName);
	nScrollbarWidth = _tstoi(buf);

	GetPrivateProfileString(_T("Image"),_T("BigThumbnailHeight"), _T("200"),buf, 100, strConfigFileName);
	nBigThumbnailHeight = _tstoi(buf);
	GetPrivateProfileString(_T("Image"),_T("SmallThumbnailHeight"), _T("50"),buf, 100, strConfigFileName);
	nSmallThumbnailHeight = _tstoi(buf);

	//GetPrivateProfileString(_T("Color"),_T("Light"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	//StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrThemeLight);
	//GetPrivateProfileString(_T("Color"),_T("Deep"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	//StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrThemeDeep);
	//GetPrivateProfileString(_T("Color"),_T("Dark"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	//StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrThemeDark);

	GetPrivateProfileString(_T("Color"),_T("clrCaptionText"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrCaptionText);
	GetPrivateProfileString(_T("Color"),_T("clrCaptionButton"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrCaptionButton);
	GetPrivateProfileString(_T("Color"),_T("clrTabButton"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrTabButton);
	GetPrivateProfileString(_T("Color"),_T("clrTabCaptionText"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrTabCaptionText);
	GetPrivateProfileString(_T("Color"),_T("clrTabBorder"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrTabBorder);
	GetPrivateProfileString(_T("Color"),_T("clrBullet"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrBullet);
	GetPrivateProfileString(_T("Color"),_T("clrTextBlockBorder"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrTextBlockBorder);
	GetPrivateProfileString(_T("Color"),_T("clrScrollBar"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrScrollBar);
	GetPrivateProfileString(_T("Color"),_T("nScrollBarTransparentRatio"), _T(""),buf, 100, strConfigFileName);
	nScrollBarTransparentRatio = _tstoi(buf);
	GetPrivateProfileString(_T("Color"),_T("clrScrollBarBorder"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrScrollBarBorder);
	GetPrivateProfileString(_T("Color"),_T("clrThumbnail"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrThumbnail);
	GetPrivateProfileString(_T("Color"),_T("clrThumbnailBorder"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrThumbnailBorder);
	GetPrivateProfileString(_T("Color"),_T("clrTextBlockRightBorder"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrTextBlockRightBorder);
	GetPrivateProfileString(_T("Color"),_T("clrFrame"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrFrame);
	GetPrivateProfileString(_T("Color"),_T("clrTabButtonShade"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrTabButtonShade);
	GetPrivateProfileString(_T("Color"),_T("clrFrameOuterBorder"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrFrameOuterBorder);
	GetPrivateProfileString(_T("Color"),_T("clrFrameInnerBorder"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrFrameInnerBorder);

	GetPrivateProfileString(_T("Color"),_T("clrSelectedTextBlockBackground"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrSelectedTextBlockBackground);
	GetPrivateProfileString(_T("Color"),_T("clrTabBackground"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrTabBackground);

	GetPrivateProfileString(_T("Color"),_T("clrFocusedTextBlockBackground"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrFocusedTextBlockBackground);
	GetPrivateProfileString(_T("Color"),_T("clrUnSelectedTextBlockBackground"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrUnSelectedTextBlockBackground);
	GetPrivateProfileString(_T("Color"),_T("clrViewBackground"),_T(""),buf, sizeof(COLORREF)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(COLORREF)*2+1, (BYTE*)&clrViewBackground);

	GetPrivateProfileString(_T("TextBlock"),_T("AutoWidth"),_T(""),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bAutoWidth);
	GetPrivateProfileString(_T("TextBlock"),_T("ShowTextBlockRightBorder"),_T(""),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bShowTextBlockRightBorder);
	GetPrivateProfileString(_T("TextBlock"),_T("EnterWideTextBlock"),_T("01"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bEnterWideTextBlock);
	
	GetPrivateProfileString(_T("TextBlock"),_T("EnterURLDetectBlock"),_T("00"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bEnterURLDetect);	
	
	GetPrivateProfileString(_T("TextBlock"),_T("EnterSHRINK_SELF"),_T("01"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bEnterSHRINK_SELF);	
	
	GetPrivateProfileString(_T("TextBlock"),_T("NarrowWidth"), _T(""),buf, 100, strConfigFileName);
	narrowWidth = _tstoi(buf);
	GetPrivateProfileString(_T("TextBlock"),_T("WideWidth"), _T(""),buf, 100, strConfigFileName);
	wideWidth = _tstoi(buf);
	GetPrivateProfileString(_T("TextBlock"),_T("HandleWidth"), _T(""),buf, 100, strConfigFileName);
	nHandleWidth = _tstoi(buf);
	GetPrivateProfileString(_T("TextBlock"),_T("HeadLength"), _T("30"),buf, 100, strConfigFileName);
	nHeadLength = _tstoi(buf);
	GetPrivateProfileString(_T("TextBlock"),_T("ShowCharCountInShrinkTB"),_T(""),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bShowCharCountInShrinkTB);

	GetPrivateProfileString(_T("Search"),_T("EachSideWidthOfFoundString"), _T(""),buf, 100, strConfigFileName);
	eachSideWidthOfFoundString = _tstoi(buf);
	GetPrivateProfileString(_T("CopyPast"),_T("CopyTextWithSymbol"),_T(""),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bCopyTextWithSymbol);
	GetPrivateProfileString(_T("CopyPast"),_T("CopyIndent"), _T(""),buf, 100, strConfigFileName);
	nCopyIndent = _tstoi(buf);
	GetPrivateProfileString(_T("CopyPast"),_T("GlobalCopyWithDocUrl"),_T("00"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bGlobalCopyWithDocUrl);
	GetPrivateProfileString(_T("CopyPast"),_T("BlockStateAfterGlobalPaste"), _T("0"),buf, 2, strConfigFileName);
	nBlockStateAfterGlobalPaste = _tstoi(buf);

	//WritePrivateProfileSection(_T("RestoreFiles"), _T(""), strConfigFileName);
	//WritePrivateProfileString(_T("RestoreFiles"),_T("0"),GUIDEFILENAME,strConfigFileName);
	//WritePrivateProfileString(_T("RestoreFiles"),_T("ActiveView"),_T("0"),strConfigFileName);

	GetPrivateProfileString(_T("MainFrame"),_T("DockedWidth"), _T("250"),buf, 100, strConfigFileName);
	nDockWidth = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("LaunchAtStartup"),_T("00"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bLaunchAtStartup);
	GetPrivateProfileString(_T("MainFrame"),_T("SaveAllElapse"), _T("5000"),buf, 100, strConfigFileName);
	nSaveAllElapse = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("SaveToTxtFile"), _T("00"),buf, 100, strConfigFileName);
	bSaveToTxtFile = _tstoi(buf);

	GetPrivateProfileString(_T("MainFrame"),_T("UseWorkingDirAsRootStrorageDir"),_T("01"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bUseWorkingDirAsRootStrorageDir);
	if (bUseWorkingDirAsRootStrorageDir)
	{
		TCHAR _strExePath[MAX_PATH+10];
		GetModuleFileName(_Module.m_hInst, _strExePath, MAX_PATH);
		CString strPath = _strExePath;
		strPath.Delete(strPath.ReverseFind(_T('\\')), strPath.GetLength());
		_tcsncpy(szRootStorageDir,strPath.GetBuffer(MAX_PATH), MAX_PATH);
	}
	else
	{
		GetPrivateProfileString(_T("MainFrame"),_T("RootStorageDir"), _T(""), szRootStorageDir, MAX_PATH, strConfigFileName);
	}

	//GetPrivateProfileString(_T("MainFrame"),_T("DirectPaste"),_T("00"),buf, sizeof(bool)*2+1,strConfigFileName);
	//StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bDirectPaste);
	GetPrivateProfileString(_T("MainFrame"),_T("PasteDir"), szRootStorageDir, szPasteDir, MAX_PATH, strConfigFileName);

	GetPrivateProfileString(_T("MainFrame"),_T("LogFolder"), _T(""), szLogFolder, MAX_PATH, strConfigFileName);
	GetPrivateProfileString(_T("MainFrame"),_T("CaptionString"), _T("BooguNote"), szCaptionString, 512, strConfigFileName);
	GetPrivateProfileString(_T("MainFrame"),_T("BooOpenInSameInstance"),_T("01"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bBooOpenInSameInstance);
	GetPrivateProfileString(_T("MainFrame"),_T("ShowAllBranchLines"),_T("00"),buf, sizeof(bool)*2+1,strConfigFileName);
	StringToBinary(buf, sizeof(bool)*2+1, (BYTE*)&bShowAllBranchLines);
	GetPrivateProfileString(_T("MainFrame"),_T("CodePage"), _T("0"),buf, 100, strConfigFileName);
	nCodePage = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("SaveType"), _T("0"),buf, 100, strConfigFileName);
	nSaveType = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("FrameState"), _T("0"),buf, 100, strConfigFileName);
	FrameState = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("DockSide"), _T("0"),buf, 100, strConfigFileName);
	DockSide = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("PosX"), _T("0"),buf, 100, strConfigFileName);
	nPosX = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("PosY"), _T("0"),buf, 100, strConfigFileName);
	nPosY = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("Width"), _T("200"),buf, 100, strConfigFileName);
	nWidth = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("Height"), _T("200"),buf, 100, strConfigFileName);
	nHeight = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("FileManDir"), _T(""), szFileManDir, MAX_PATH, strConfigFileName);
	GetPrivateProfileString(_T("MainFrame"),_T("FileManLeft"), _T("150"),buf, 100, strConfigFileName);
	nFileManLeft = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("FileManTop"), _T("200"),buf, 100, strConfigFileName);
	nFileManTop = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("FileManWidth"), _T("250"),buf, 100, strConfigFileName);
	nFileManWidth = _tstoi(buf);
	GetPrivateProfileString(_T("MainFrame"),_T("FileManHeight"), _T("300"),buf, 100, strConfigFileName);
	nFileManHeight = _tstoi(buf);
}