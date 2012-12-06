// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0602
#define _RICHEDIT_VER	0x0200

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlcom.h>
#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>
#include <atlscrl.h>
#include <atlctrlx.h>
//#include <atlctl.h>
//#include <atlctrlw.h>
#include <atlcrack.h>
#include <atlddx.h>


#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define FRAME_DEFAULT_WIDTH	256

#define FILE_ICON_WIDTH	20

#define TEXT_HANDLE_NULL 0
#define TEXT_HANDLE_EXPAND 2
#define TEXT_HANDLE_CLOSED 1

#define TEXT_BLOCK_NARROW 0
#define TEXT_BLOCK_SHRINK 1
#define TEXT_BLOCK_WIDE 2

#define TEXT_ICON_BEGAIN	-1
#define TEXT_ICON_PLAINTEXT -1
#define TEXT_ICON_TODO		0
#define TEXT_ICON_DONE		1
#define TEXT_ICON_CROSS		2
#define TEXT_ICON_STAR		3
#define TEXT_ICON_QUESTION	4
#define TEXT_ICON_WARNING	5
#define TEXT_ICON_IDEA		6
#define TEXT_ICON_END		6

#define FRAME_STATE_NORMAL	0
#define FRAME_STATE_DOCK	1
#define FRAME_STATE_DOCK_AUTOHIDE	2

#define DOCK_RIGHT	0
#define	DOCK_TOP	1
#define DOCK_LEFT	2
#define DOCK_BOTTOM	3

#define WM_USER_REQUESTRESIZE WM_USER+1024
#define WM_USER_DEL_CLIPPED WM_USER+1025
#define WM_USER_ICONNOTIFY WM_USER+1026
#define WM_USER_FILERENAME WM_USER+1027
#define WM_USER_SEARCHDLG_CLOSED		WM_USER+1028
#define WM_USER_MAINFRM_REFRESH		WM_USER+1029
#define WM_USER_MAINFRM_OPENFILE		WM_USER+1030
#define WM_USER_SLIDE_IN		WM_USER+1031
#define WM_USER_SLIDE_OUT		WM_USER+1032
#define WM_USER_GET_FILE_DIR		WM_USER+1033
#define WM_USER_REFRESH_FILE_TREE_VIEW	WM_USER+1034
#define WM_USER_SHARE				WM_USER+1035

#define BOOGUNOTE_HOTKEY_DOCK			2352
#define BOOGUNOTE_HOTKEY_HIDE			2353
#define BOOGUNOTE_HOTKEY_GLOBAL_PASET	2354
#define BOOGUNOTE_HOTKEY_FOCUSING		2355
#define BOOGUNOTE_HOTKEY_DOCK_AUTOHIDE	2356
#define BOOGUNOTE_HOTKEY_GLOBAL_PASET_TO_FILE	2357
#define BOOGUNOTE_HOTKEY_GLOBAL_SCREENSHOT	2358

#define BOOGUNOTE_VERSION	_T("0.3.3.7 zh")
#define BOO_FILE_VERSION	7
#define XML_HEADER			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
//#define XML_HEADER			"<?xml version=\"1.0\"?>"
#define BN_API_GetSelectedBlocks WM_USER+3000

#define GUIDEFILENAME	_T("½Ì³Ì.boo")

#define NOTIFY_TIMER_ID		1

extern CString g_szCommandLineFile;