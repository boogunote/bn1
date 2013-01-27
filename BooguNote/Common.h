#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <gdiplus.h>
#pragma comment (lib,"gdiplus.lib")
using namespace Gdiplus;

class XMLElement;

std::vector<std::basic_string<TCHAR>> Split (const std::basic_string<TCHAR> &inString,
                                  const std::basic_string<TCHAR> &separator);

CString NameFromVKey(UINT nVK);

CString GetHumanReadableAccel(ACCEL& accel);

CString GetHumanReadableGlobalAccel(ACCEL& accel);

int StringToBinary(TCHAR* inputStr, int nInputStrLen, BYTE* outputData);

int BinaryToString(BYTE* inputData, int nInputDataLen, TCHAR* outputStr);

int BinaryToStringA(BYTE* inputData, int nInputDataLen, char* outputStr);

int PointsToLogical(int nPointSize);

HFONT EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR *szFace);

void SetLaunchStrategy(bool bStartup);

void UpdateMenuKeys(HMENU hMenu);

CString FindCommand(WORD cmdid);

HRESULT CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszArg, LPCTSTR lpszPathLink, LPCTSTR lpszDesc);

int GetXmlIcon(XMLElement* xe);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

HWND GlobalCopy();

void CutOrCopyFiles(LPCTSTR lpBuffer,UINT uBufLen,BOOL bCopy);

void CreateEmptyFile(LPCTSTR szFileName);

class W
{
public:
	char* we;
	W(const wchar_t* x)
	{
		//int y = wcslen(x);
		//int wy = y* + 100;
		//we = new char[wy];
		//memset(we,0,wy);

		//WideCharToMultiByte(CP_UTF8,0,x,-1,we,wy,0,0);

		DWORD dwBytesWritten = WideCharToMultiByte(CP_UTF8, 0, x, -1, 0, 0, NULL, NULL);
		we = new char[dwBytesWritten+10];
		int lBytesWritten = WideCharToMultiByte(CP_UTF8, 0, x, -1, we, dwBytesWritten, NULL, NULL);
		we[lBytesWritten] = '\0';
	}

	~W()
	{
		delete[] we;
	}

	operator char* ()
	{
		return we;
	}
};

class CEditImpl : public CWindowImpl<CEditImpl, CEdit>
{
    BEGIN_MSG_MAP(CEditImpl)
        MSG_WM_CONTEXTMENU(OnContextMenu)
    END_MSG_MAP()

    void OnContextMenu ( HWND hwndCtrl, CPoint ptClick )
    {
        MessageBox(_T("Edit control handled WM_CONTEXTMENU"));
    }
};

class CUtil
{
public:
	HCURSOR		hCsrSizeAll;
	HCURSOR		hCsrArrow;
	HCURSOR		hCsrHand;
	HCURSOR		hCrsCross;
public:
	CUtil()
	{
		hCsrSizeAll = LoadCursor(NULL, IDC_SIZEALL);
		hCsrArrow = LoadCursor(NULL, IDC_ARROW);
		hCsrHand = LoadCursor(NULL, IDC_HAND);
		hCrsCross = LoadCursor(NULL, IDC_CROSS);
	}
};


#define _1K_BUF	1024
#define _2K_BUF	2048
#define _1M_BUF	1024*1024

typedef struct  
{
	UINT uMsgId;
	COLORREF TextColor;
	COLORREF BkgColor;
} COLOR_PAIR;