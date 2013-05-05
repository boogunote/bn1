// BooguNote.cpp : main source file for BooguNote.exe
//

#include "stdafx.h"

#include "resource.h"

#include "BooguNoteView.h"
#include "BooguNoteConfig.h"
#include "Common.h"
#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;
CBooguNoteConfig g_config;
CUtil	g_util;
CString g_szCommandLineFile;
extern bool firstLaunch;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	g_szCommandLineFile = lpstrCmdLine;
	if (firstLaunch)
	{
		TCHAR _strExePath[MAX_PATH+10];
		GetModuleFileName(_Module.m_hInst, _strExePath, MAX_PATH);
		CString strGuideName = _strExePath;
		strGuideName.Delete(strGuideName.ReverseFind(_T('\\'))+1, strGuideName.GetLength());
		strGuideName+=GUIDEFILENAME;
		if (_T(':') == strGuideName[1] && INVALID_FILE_ATTRIBUTES != GetFileAttributes(strGuideName))
		{
			g_szCommandLineFile = strGuideName;
		}
	}
	else
	{
		g_szCommandLineFile.Delete(g_szCommandLineFile.GetLength()-1, 1);
		g_szCommandLineFile.Delete(0, 1);
	}
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx(/*0,0,0,WS_EX_TOPMOST*/) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	
	//OSVERSIONINFOEX osvi;
	//ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	//osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	//GetVersionEx((OSVERSIONINFO *) &osvi);
	//if (osvi.dwMajorVersion>=6 && osvi.dwMinorVersion>=0)
	//{
	//	typedef int (__cdecl *MYPROC)(UINT, DWORD);
	//	HINSTANCE hinstLib = LoadLibrary(TEXT("user32.dll")); 
	//	MYPROC ChangeWindowMessageFilter = (MYPROC) GetProcAddress(hinstLib, "ChangeWindowMessageFilter"); 
	//	if (NULL != ChangeWindowMessageFilter) 
 //       {
	//		#define MSGFLT_ADD 1
	//		#define MSGFLT_REMOVE 2
	//		(ChangeWindowMessageFilter)(BN_API_GetSelectedBlocks, MSGFLT_ADD);
	//	}
	//}
	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{

	 //__try
  //    {
		//  //*((int*)(0xdfe)) = 1;

	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	TCHAR _strExePath[MAX_PATH+10];
	GetModuleFileName(_Module.m_hInst, _strExePath, MAX_PATH);
	PathRemoveFileSpec(_strExePath);
	PathAppend(_strExePath, L"./bn_icons.bmp");
    HIMAGELIST hImageList = ImageList_LoadImage(NULL, _strExePath, 16, 1, RGB(0xC0,0xC0,0xC0)/*CLR_DEFAULT*/, IMAGE_BITMAP, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	if(hImageList != NULL)
	{
		g_config.imageList.Attach(hImageList);
	}
	

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	int nRet = Run(lpstrCmdLine, nCmdShow);

	GdiplusShutdown(gdiplusToken);

	_Module.Term();
	::CoUninitialize();

	return nRet;
	/* }
      __except (MSJExceptionHandler::MSJUnhandledExceptionFilter (GetExceptionInformation()))
      {
         OutputDebugString (_T("executed filter function\n"));
      }*/


	
}
