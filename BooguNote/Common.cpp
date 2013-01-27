#include "stdafx.h"
#include "Common.h"
#include <tchar.h>
#include <malloc.h>
#include "BooguNoteConfig.h"
#include "resource.h"
#include "legacy_xml.h"

extern CBooguNoteConfig g_config;
extern TCHAR strExePath[];
//extern int g_nMainFrameState;

std::vector<std::basic_string<TCHAR>> Split (const std::basic_string<TCHAR> &inString,
                                  const std::basic_string<TCHAR> &separator)
{
   std::vector<std::basic_string<TCHAR>> returnVector;
   std::basic_string<TCHAR>::size_type start = 0;
   std::basic_string<TCHAR>::size_type end = 0;

   while ((end=inString.find (separator, start)) != std::string::npos)
   {
      returnVector.push_back (inString.substr (start, end-start));
      start = end+separator.size();
   }

   returnVector.push_back (inString.substr (start));

   return returnVector;

} 

CString NameFromVKey(UINT nVK)
{
	CString str;
	if (0x00 == nVK)
	{
		str = _T("None");
	}
	else if (VK_SNAPSHOT == nVK)
	{
		str = _T("Prt Scr");
	}
	else
	{
		UINT nScanCode = MapVirtualKeyEx(nVK, 0, GetKeyboardLayout(0));
		switch(nVK) {
			// Keys which are "extended" (except for Return which is Numeric Enter as extended)
			case VK_INSERT:
			case VK_DELETE:
			case VK_HOME:
			case VK_END:
			case VK_NEXT:  // Page down
			case VK_PRIOR: // Page up
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
				nScanCode |= 0x100; // Add extended bit
		}	

		// GetKeyNameText() expects the scan code to be on the same format as WM_KEYDOWN
		// Hence the left shift
		LPTSTR prb = str.GetBuffer(80);
		BOOL bResult = GetKeyNameText(nScanCode << 16, prb, 79);

		// these key names are capitalized and look a bit daft
		int len = lstrlen(prb);
		if(len > 1) {
			LPTSTR p2 = CharNext(prb);
			CharLowerBuff(p2, len - (p2 - prb) );
		}

		str.ReleaseBuffer();
		ATLASSERT(str.GetLength());
	}
	return str; // internationalization ready, sweet!
}

CString GetHumanReadableAccel(ACCEL& accel)
{
	CString strShortCut;
	if (accel.fVirt & FCONTROL)
		strShortCut = _T("Ctrl");
	if (accel.fVirt & FALT )
	{
		if (!strShortCut.IsEmpty())
			strShortCut += _T(" + ");
		strShortCut += _T("Alt");
	}
	if (accel.fVirt & FSHIFT )
	{
		if (!strShortCut.IsEmpty())
			strShortCut += _T(" + ");
		strShortCut += _T("Shift");
	}
	if (!strShortCut.IsEmpty())
		strShortCut += _T(" + ");
	strShortCut += NameFromVKey(accel.key);
	return strShortCut;
}

CString GetHumanReadableGlobalAccel(ACCEL& accel)
{
	CString strShortCut;
	if (accel.fVirt & MOD_CONTROL)
		strShortCut = _T("Ctrl");
	if (accel.fVirt & MOD_ALT )
	{
		if (!strShortCut.IsEmpty())
			strShortCut += _T(" + ");
		strShortCut += _T("Alt");
	}
	if (accel.fVirt & MOD_SHIFT )
	{
		if (!strShortCut.IsEmpty())
			strShortCut += _T(" + ");
		strShortCut += _T("Shift");
	}
	if (accel.fVirt & MOD_WIN )
	{
		if (!strShortCut.IsEmpty())
			strShortCut += _T(" + ");
		strShortCut += _T("Win");
	}
	if (!strShortCut.IsEmpty())
		strShortCut += _T(" + ");
	strShortCut += NameFromVKey(accel.key);
	return strShortCut;
}


#define BYTE unsigned char 

int _httoi(const TCHAR *value)
{
  struct CHexMap
  {
    TCHAR chr;
    int value;
  };
  const int HexMapL = 16;
  CHexMap HexMap[HexMapL] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}
  };
  TCHAR *mstr = _tcsupr(_tcsdup(value));
  TCHAR *s = mstr;
  int result = 0;
  if (*s == '0' && *(s + 1) == 'X') s += 2;
  bool firsttime = true;
  while (*s != '\0')
  {
    bool found = false;
    for (int i = 0; i < HexMapL; i++)
    {
      if (*s == HexMap[i].chr)
      {
        if (!firsttime) result <<= 4;
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    s++;
    firsttime = false;
  }
  free(mstr);
  return result;
}

void ctoh(TCHAR *output, BYTE& value)
{
	_stprintf(output, _T("%+02x"), value);
}

int StringToBinary(TCHAR* inputStr, int nInputStrLen, BYTE* outputData)
{
	for(int i=0; i<nInputStrLen/2; ++i)
	{
		TCHAR temp[4];
		temp[0] = inputStr[2*i];
		temp[1] = inputStr[2*i+1];
		temp[2] = _T('\0');
		int tempInt = _httoi(temp);
		outputData[i] = (BYTE)tempInt;
	}
	return nInputStrLen/2;
}

int BinaryToString(BYTE* inputData, int nInputDataLen, TCHAR* outputStr)
{
	for (int i=0; i<nInputDataLen; ++i)
	{
		_stprintf(&(outputStr[2*i]), _T("%+02x"), inputData[i]);
	}
	return 0;
}

int BinaryToStringA(BYTE* inputData, int nInputDataLen, char* outputStr)
{
	for (int i=0; i<nInputDataLen; ++i)
	{
		sprintf(&(outputStr[2*i]), "%+02x", inputData[i]);
	}
	return 0;
}

int PointsToLogical(int nPointSize)
{
	HDC hdc      = GetDC(0);
	int nLogSize = -MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(0, hdc);

	return nLogSize;
}

HFONT EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR *szFace)
{
	return CreateFont(PointsToLogical(nPointSize), 
					  0, 0, 0, 
					  fBold ? FW_BOLD : 0,
					  0,0,0,DEFAULT_CHARSET,0,0,
					  dwQuality,
					  0,
					  szFace);
}

void SetLaunchStrategy(bool bStartup/*, int nState*/)
{
	TCHAR szStartupPath[_MAX_PATH+1];
	SHGetFolderPath(NULL, CSIDL_STARTUP, NULL, 0, szStartupPath);
	lstrcat(szStartupPath, _T("\\BooguNote.lnk"));
	if(bStartup)
	{
		//TCHAR strExePath [MAX_PATH+10];
		//GetModuleFileName(_Module.m_hInst, strExePath, MAX_PATH);
		TCHAR strBuf[MAX_PATH+100];
		_stprintf(strBuf, _T("\"%s\""), strExePath);
		//if (0 == nState)
		//{
			CreateLink(strBuf, NULL, szStartupPath, NULL) ;
		/*}
		else if (1 == nState)
		{
			CreateLink(strBuf, _T("-dock"), szStartupPath, NULL) ;
		}
		else if (2 == nState)
		{
			CreateLink(strBuf, _T("-autohide"), szStartupPath, NULL) ;
		}*/
		//else if (3 == nState)
		//{
		//	CreateLink(strBuf, _T("-hiden"), szStartupPath, NULL) ;
		//}
		//g_nMainFrameState = nState;
	}
	else
	{
		HANDLE hFile = CreateFileW(szStartupPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ERROR_FILE_NOT_FOUND != GetLastError())
		{
			CloseHandle(hFile);
			DeleteFile(szStartupPath);
		}
		
	}
	/*HKEY hSoftware;
	LONG lr = RegOpenKey(HKEY_CURRENT_USER, _T("Software"), &hSoftware);
	HKEY hMicrosoft;
	lr = RegOpenKey(hSoftware, _T("Microsoft"), &hMicrosoft);
	HKEY hWindows;
	lr = RegOpenKey(hMicrosoft, _T("Windows"), &hWindows);
	HKEY hCurrentVersion;
	lr = RegOpenKey(hWindows, _T("CurrentVersion"), &hCurrentVersion);
	HKEY hRun;
	lr = RegOpenKey(hCurrentVersion, _T("Run"), &hRun);
	if(g_config.bLaunchAtStartup)
	{
		TCHAR strExePath [MAX_PATH+10];
		GetModuleFileName(_Module.m_hInst, strExePath, MAX_PATH);
		TCHAR strBuf[MAX_PATH+100];
		_stprintf(strBuf, _T("\"%s\" -dock"), strExePath);
		RegSetValueExW(hRun, _T("BooguNote"), NULL,REG_SZ, (BYTE*)strBuf, (lstrlen(strBuf) + 1)*sizeof(TCHAR));
	}
	else
	{
		RegDeleteValue(hRun, _T("BooguNote"));
	}
	RegCloseKey(hRun);
	RegCloseKey(hCurrentVersion);
	RegCloseKey(hWindows);
	RegCloseKey(hMicrosoft);
	RegCloseKey(hSoftware);*/
}

// use custom accelerator table to change the keyboard shortcuts displayed on said hMenu
void UpdateMenuKeys(HMENU hMenu) 
{
	ATLASSERT(hMenu);

	int nItems = ::GetMenuItemCount(hMenu);
	CMenuItemInfo mi;
	mi.fMask = MIIM_ID | MIIM_SUBMENU;
	TCHAR buf[512];
	CString name;
	for(int i = 0; i < nItems; i++)
	{
		// i hope we're not going to have GPFs like in RepairMenu!
		::GetMenuItemInfo(hMenu, i, TRUE, &mi); // by position
		if(mi.hSubMenu)
			UpdateMenuKeys(mi.hSubMenu);
		else if(mi.wID != 0 ) { // separators etc excluded
			// see if there's accelerator info in text
			ATLASSERT(!(buf[0] = 0));
			::GetMenuString(hMenu, i, buf, sizeof(buf)/sizeof(buf[0]), MF_BYPOSITION);
			ATLASSERT(buf[0]);

			int len = lstrlen(buf), k = len;
			/*while(k--)
				if(_T('\t') == buf[k])
					break;
			BOOL bTab = k > 0;
			BOOL bChanged = 1;*/

			// is there any accelerator for this command nowadays?
			CString MenuString = FindCommand(mi.wID);
			MenuString = _T('\t')+MenuString;
			MenuString = buf+MenuString;
			//if(-1 == idx) {
			//	if(bTab)
			//		buf[k] = 0; // remove old one
			//	else
			//		bChanged = 0;
			//}
			//else {
			//	if(!bTab) {
			//		k = len;
			//		buf[k] = _T('\t');
			//	}
			//	k++;

			//	name = dummy.NameFromAccel(m_pAccelTable[idx]);
			//	ATLASSERT(k+name.GetLength() < sizeof(buf)/sizeof(buf[0]));
			//	lstrcpy(buf+k, name);
			//}

			//if(bChanged) {
			//	ATLASSERT(lstrlen(buf));
			::ModifyMenu(hMenu, i, MF_BYPOSITION, mi.wID, MenuString.GetBuffer(512));
			MenuString.ReleaseBuffer();
				// $TSEK no need to update item enable/icon states? (see wtl's command bar atlctrlw.h line 2630)
			//}
		}
	}
}

CString FindCommand(WORD cmdid)
{
	if (ID_CAPTION_DOCK == cmdid)
		cmdid = BOOGUNOTE_HOTKEY_DOCK;
	if (ID_CAPTION_AUTOHIDE == cmdid)
		cmdid = BOOGUNOTE_HOTKEY_DOCK_AUTOHIDE;
	if (ID_CAPTION_HIDEN == cmdid)
		cmdid = BOOGUNOTE_HOTKEY_HIDE;
	if (ID_STATE_NARROW == cmdid)
		cmdid = ID_VIEW_SWITCH;
	if (ID_STATE_WIDE == cmdid)
		cmdid = ID_VIEW_SWITCH;
	if (ID_STATE_SHRINK == cmdid)
		cmdid = ID_VIEW_SWITCH;
	for (int i=0; i<g_config.nAccelGlobalCount; ++i)
	{
		if (g_config.AccelGlobal[i].cmd == cmdid)
		{
			return GetHumanReadableGlobalAccel(g_config.AccelGlobal[i]);
		}
	}
	for (int i=0; i<g_config.nAccelMainFrmCount; ++i)
	{
		if (g_config.AccelMainFrm[i].cmd == cmdid)
		{
			return GetHumanReadableAccel(g_config.AccelMainFrm[i]);
		}
	}
	for (int i=0; i<g_config.nAccelViewCount; ++i)
	{
		if (g_config.AccelView[i].cmd == cmdid)
		{
			return GetHumanReadableAccel(g_config.AccelView[i]);
		}
	}
	return CString(_T(""));
}

// CreateLink - uses the Shell's IShellLink and IPersistFile interfaces 
//              to create and store a shortcut to the specified object. 
//
// Returns the result of calling the member functions of the interfaces. 
//
// Parameters:
// lpszPathObj  - address of a buffer containing the path of the object. 
// lpszPathLink - address of a buffer containing the path where the 
//                Shell link is to be stored. 
// lpszDesc     - address of a buffer containing the description of the 
//                Shell link. 

HRESULT CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszArg, LPCTSTR lpszPathLink, LPCTSTR lpszDesc) 
{ 
    HRESULT hres; 
    IShellLink* psl; 
 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
 
        // Set the path to the shortcut target and add the description. 
        psl->SetPath(lpszPathObj); 
        psl->SetDescription(lpszDesc); 
		psl->SetArguments(lpszArg);
 
        // Query IShellLink for the IPersistFile interface for saving the 
        // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
 
        if (SUCCEEDED(hres)) 
        { 
            //WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            //MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 
			
            // Add code here to check return value from MultiByteWideChar 
            // for success.
 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(lpszPathLink, TRUE); 
            ppf->Release(); 
        } 
        psl->Release(); 
    } 
    return hres; 
}

int GetXmlIcon(XMLElement* xe)
{
	int nChildNum = xe->GetChildrenNum();
	for (int i=0; i<nChildNum; ++i)
	{
		XMLElement* ch = xe->GetChildren()[i];
		char n[128];
		ch->GetElementName(n);
		if (strcmp(n, "icon") ==0)
		{
			char szBuiltin[128];
			ch->FindVariableZ("BUILTIN")->GetValue(szBuiltin, 1);
			if (strcmp(szBuiltin, "flag")==0)
			{
				return TEXT_ICON_TODO;
			}
			else if (strcmp(szBuiltin, "button_ok")==0)
			{
				return TEXT_ICON_DONE;
			}
			else if (strcmp(szBuiltin, "button_cancel")==0)
			{
				return TEXT_ICON_CROSS;
			}
			else if (strcmp(szBuiltin, "bookmark")==0)
			{
				return TEXT_ICON_STAR;
			}
			else if (strcmp(szBuiltin, "help")==0)
			{
				return TEXT_ICON_QUESTION;
			}
			else if (strcmp(szBuiltin, "messagebox_warning")==0)
			{
				return TEXT_ICON_WARNING;
			}
			else if (strcmp(szBuiltin, "idea")==0)
			{
				return TEXT_ICON_IDEA;
			}
			else
			{
				return TEXT_ICON_PLAINTEXT;
			}
		}
		
	}
	return TEXT_ICON_PLAINTEXT;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

HWND GlobalCopy()
{
	HWND hCurrGlobalActiveWindow = GetForegroundWindow();
	if (hCurrGlobalActiveWindow!=NULL)
	{
		//DWORD remoteThreadId = GetWindowThreadProcessId(hCurrGlobalActiveWindow,0);
		//DWORD currentThreadId = GetCurrentThreadId();
		//AttachThreadInput(remoteThreadId, currentThreadId, true);
		//HWND  hFocusedWindow = GetFocus();
		//AttachThreadInput(remoteThreadId, currentThreadId, false);
		
		keybd_event(VK_CONTROL, MapVirtualKeyEx(VK_CONTROL, 0, GetKeyboardLayout(0)), 0, 0);
		keybd_event(0x43, MapVirtualKeyEx(0x43, 0, GetKeyboardLayout(0)), 0, 0);
		keybd_event(0x43, MapVirtualKeyEx(0x43, 0, GetKeyboardLayout(0)), KEYEVENTF_KEYUP, 0);
		keybd_event(VK_CONTROL, MapVirtualKeyEx(VK_CONTROL, 0, GetKeyboardLayout(0)), KEYEVENTF_KEYUP, 0);

		Sleep(100);
	}
	return hCurrGlobalActiveWindow;

}

void CutOrCopyFiles(LPCTSTR lpBuffer,UINT uBufLen,BOOL bCopy)
{
	UINT uDropEffect;
	DROPFILES dropFiles;
	UINT uGblLen,uDropFilesLen;
	HGLOBAL hGblFiles,hGblEffect;
	char *szData,*szFileList;

	DWORD *dwDropEffect;

	uDropEffect=RegisterClipboardFormat(L"Preferred DropEffect");
	hGblEffect=GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,sizeof(DWORD));
	dwDropEffect=(DWORD*)GlobalLock(hGblEffect);
	if(bCopy)
		*dwDropEffect=DROPEFFECT_COPY;
	else 
		*dwDropEffect=DROPEFFECT_MOVE;
	GlobalUnlock(hGblEffect);

	uDropFilesLen=sizeof(DROPFILES);
	dropFiles.pFiles =uDropFilesLen;
	dropFiles.pt.x=0;
	dropFiles.pt.y=0;
	dropFiles.fNC =FALSE;
	dropFiles.fWide =TRUE;

	uGblLen=uDropFilesLen+uBufLen*2+20;
	hGblFiles= GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE, uGblLen);
	szData=(char*)GlobalLock(hGblFiles);
	memset(szData, 0, uGblLen);
	memcpy(szData,(LPVOID)(&dropFiles),uDropFilesLen);
	szFileList=szData+uDropFilesLen;
	memcpy(szFileList, lpBuffer, uBufLen*sizeof(TCHAR));

	GlobalUnlock(hGblFiles);

	if( OpenClipboard(NULL) )
	{
		EmptyClipboard();
		SetClipboardData( CF_HDROP, hGblFiles );
		SetClipboardData(uDropEffect,hGblEffect);
		CloseClipboard();
	}
}   

TCHAR szEmptyFile[] = L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n\
					   <root version=\"6\">\n\
					   <item content=\"\" icon=\"none\" branch=\"none\" block=\"narrow\" level=\"0\" IsShown=\"true\" IsBold=\"false\" ShowBranch=\"false\" TextColor=\"00000000\" BkgrdColor=\"ffffff00\"/>\n\
					   </root>";
void CreateEmptyFile(LPCTSTR szFileName)
{
	int nGuideLen = WideCharToMultiByte(CP_UTF8, 0, szEmptyFile, -1, 0, 0, NULL, NULL);
	char* buf = new char[nGuideLen+10];
	int lBytesWritten = WideCharToMultiByte(CP_UTF8, 0, szEmptyFile, -1, buf, nGuideLen, NULL, NULL);
	buf[lBytesWritten] = '\0';

	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD le = GetLastError();
	DWORD dwBytesWritten;
	
	WriteFile(hFile, buf, lBytesWritten+1, &dwBytesWritten, NULL);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	delete []buf;
}