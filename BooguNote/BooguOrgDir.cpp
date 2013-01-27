/*
Module : WtlFileTreeCtrl.cpp
Purpose: Interface for an WTL class which provides a tree control similiar 
         to the left hand side of explorer

Copyright (c) 2003 by iLYA Solnyshkin. 
E-mail: isolnyshkin@yahoo.com 
All rights reserved.
*/

#include "stdafx.h"
#include "BooguOrgDir.h"
#include "FileFind.h"
#include "common.h"
#include "BooguNoteConfig.h"

#define MIN_SHELL_ID 1
#define MAX_SHELL_ID 30000

extern CBooguNoteConfig g_config;

CSystemImageList g_SysImageList;

int CSystemImageList::m_nRefCount = 0;

////////////////////////////// Implementation of CSystemImageList /////////////////////////////////

CSystemImageList::CSystemImageList()
{
  // We need to implement reference counting to 
  // overcome an limitation whereby you cannot
  // have two CImageLists attached to the one underlyinh
  // HIMAGELIST.
  if (m_nRefCount == 0)
  {
    // Attach to the system image list
    SHFILEINFO sfi;
    HIMAGELIST hSystemImageList = (HIMAGELIST) SHGetFileInfo( _T("C:\\"), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
    m_ImageList.Attach( hSystemImageList );
  }  
  // Increment the reference count
  m_nRefCount++;
}

CSystemImageList::~CSystemImageList()
{
  //Decrement the reference count
  m_nRefCount--;  

  if( m_nRefCount == 0 )
  {
    // Detach from the image list to prevent problems on 95/98 where
    // the system image list is shared across processes
    m_ImageList.Detach();
  }
}

CImageList* CSystemImageList::GetImageList()
{
  return &m_ImageList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CBooguOrgDir::ReadChanged(TCHAR* szMonitorDir)
{
	USES_CONVERSION;

	HANDLE hDir = CreateFile( CString(szMonitorDir), // pointer to the file name
		FILE_LIST_DIRECTORY,                // access (read/write) mode
		FILE_SHARE_READ|FILE_SHARE_DELETE,  // share mode
		NULL,                               // security descriptor
		OPEN_EXISTING,                      // how to create
		FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
		NULL                                // file with attributes to copy
		);
	FILE_NOTIFY_INFORMATION Buffer[1024];
	DWORD BytesReturned;
	ReadDirectoryChangesW(
		hDir,                                  // handle to directory
		&Buffer,                                    // read results buffer
		sizeof(Buffer),                                // length of buffer
		TRUE,                                 // monitoring option
		FILE_NOTIFY_CHANGE_SECURITY|
		FILE_NOTIFY_CHANGE_CREATION|
		FILE_NOTIFY_CHANGE_LAST_ACCESS|
		FILE_NOTIFY_CHANGE_LAST_WRITE|
		FILE_NOTIFY_CHANGE_SIZE|
		FILE_NOTIFY_CHANGE_ATTRIBUTES|
		FILE_NOTIFY_CHANGE_DIR_NAME|
		FILE_NOTIFY_CHANGE_FILE_NAME,             // filter conditions
		&BytesReturned,              // bytes returned
		NULL,                          // overlapped buffer
		NULL// completion routine
		);
	CString tempDir = CString(Buffer[0].FileName).Left(Buffer[0].FileNameLength / 2);
	CString szDir = szMonitorDir;
	PathAppend(szDir.GetBuffer(MAX_PATH), (LPCTSTR)tempDir);
	szDir.ReleaseBuffer();
	PathRemoveBackslash(szDir.GetBuffer(MAX_PATH));
	szDir.ReleaseBuffer();
	PathRemoveFileSpec(szDir.GetBuffer(MAX_PATH));
	szDir.ReleaseBuffer();
}
unsigned __stdcall CBooguOrgDir::WatchDirectory(void* p)
{
	DWORD dwWaitStatus; 
	HANDLE dwChangeHandles[3]; 
	TCHAR lpDrive[4];
	TCHAR lpFile[_MAX_FNAME];
	TCHAR lpExt[_MAX_EXT];
	TCHAR lpDir[MAX_PATH*2];
	Param * pParam = (Param*)p;
	_tcscpy_s(lpDir, MAX_PATH*2, pParam->szPath);
	delete []pParam->szPath;

	if (0 == _tcslen(lpDir))
	{
		delete p;
		return 0;
	}

	_tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

	lpDrive[2] = (TCHAR)'\\';
	lpDrive[3] = (TCHAR)'\0';

	// Watch the directory for file creation and deletion. 

	dwChangeHandles[0] = FindFirstChangeNotification( 
		lpDir,                         // directory to watch 
		TRUE,                         // do not watch subtree 
		FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes 

	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
	{
		//printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		DWORD dwError = GetLastError();
		return -1; 
	}

	// Watch the subtree for directory creation and deletion. 

	dwChangeHandles[1] = FindFirstChangeNotification( 
		lpDrive,                       // directory to watch 
		TRUE,                          // watch the subtree 
		FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes 

	if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) 
	{
		//printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		DWORD dwError = GetLastError();
		return -1; 
	}

	ResetEvent(pParam->hQuit);
	dwChangeHandles[2] = pParam->hQuit;
	delete p;


	// Make a final validation check on our handles.

	if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
	{
		//printf("\n ERROR: Unexpected NULL from FindFirstChangeNotification.\n");
		DWORD dwError = GetLastError();
		return -1; 
	}

	// Change notification is set. Now wait on both notification 
	// handles and refresh accordingly. 

	while (TRUE) 
	{ 
		// Wait for notification.

		//printf("\nWaiting for notification...\n");

		dwWaitStatus = WaitForMultipleObjects(3, dwChangeHandles, 
			FALSE, INFINITE); 

		switch (dwWaitStatus) 
		{ 
		case WAIT_OBJECT_0: 

			// A file was created, renamed, or deleted in the directory.
			// Refresh this directory and restart the notification.

			ReadChanged(lpDir);
			if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )
			{
				//printf("\n ERROR: FindNextChangeNotification function failed.\n");
				DWORD dwError = GetLastError();
				return -1; 
			}
			break; 

		case WAIT_OBJECT_0 + 1: 

			// A directory was created, renamed, or deleted.
			// Refresh the tree and restart the notification.

			ReadChanged(lpDir);
			if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE )
			{
				//printf("\n ERROR: FindNextChangeNotification function failed.\n");
				DWORD dwError = GetLastError();
				return -1; 
			}
			break; 

		case WAIT_OBJECT_0 + 2:
			return 0;
			 break;

		case WAIT_TIMEOUT:

			// A timeout occurred, this would happen if some value other 
			// than INFINITE is used in the Wait call and no changes occur.
			// In a single-threaded environment you might not want an
			// INFINITE wait.

			//printf("\nNo changes in the timeout period.\n");
			break;

		default: 
			//printf("\n ERROR: Unhandled dwWaitStatus.\n");
			DWORD dwError = GetLastError();
			return -1; 
			break;
		}
	}
}

LRESULT CBooguOrgDir::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	//SetEvent(m_hQuit);
	//CloseHandle(m_hQuit);
	bHandled = FALSE;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CBooguOrgDir::PreTranslateMessage(MSG* pMsg) 
{
	if(m_haccelerator != NULL && !m_bEdit)
	{
		if(::TranslateAccelerator(m_hWnd, m_haccelerator, pMsg))
			return TRUE;
	}
	return FALSE;
}

BOOL CBooguOrgDir::SubclassWindow( HWND hWnd )
{
	BOOL bRet = CWindowImpl<CBooguOrgDir, CTreeViewCtrl>::SubclassWindow( hWnd );
	if( bRet )
		PostMessage(WM_POPULATE_TREE);
	return bRet;
}

HWND CBooguOrgDir::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szRoot)
{
	CWindowImpl< CBooguOrgDir, CTreeViewCtrl>::Create(hWndParent, rect, WC_TREEVIEW, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS, 
		WS_EX_CLIENTEDGE);
	SetWindowText(L"File Management");
	//m_hQuit = CreateEvent(NULL,               // default security attributes
	//	TRUE,               // manual-reset event
	//	FALSE,              // initial state is nonsignaled
	//	NULL  // object name
	//	); 
	//ResetEvent(m_hQuit);
	if (NULL != szRoot)
	{
		SetRootFolder(szRoot);
	}
	PostMessage(WM_POPULATE_TREE);

	m_Accel[0].cmd = ID_POPMENU_CUT;
	m_Accel[0].fVirt = FCONTROL|FVIRTKEY;
	m_Accel[0].key = 0x58;
	m_Accel[1].cmd = ID_POPMENU_COPY;
	m_Accel[1].fVirt = FCONTROL|FVIRTKEY;
	m_Accel[1].key = 0x43;
	m_Accel[2].cmd = ID_POPMENU_PASTE;
	m_Accel[2].fVirt = FCONTROL|FVIRTKEY;
	m_Accel[2].key = 0x56;
	m_Accel[3].cmd = ID_POPMENU_DELETE;
	m_Accel[3].fVirt = FVIRTKEY;
	m_Accel[3].key = VK_DELETE ;
	//m_Accel[4].cmd = ID_POPMENU_DELETE;
	//m_Accel[4].fVirt = FCONTROL|FVIRTKEY;
	//m_Accel[4].key = VK_DELETE ;
	m_haccelerator = CreateAcceleratorTable(m_Accel, 4);

	return this->m_hWnd;
}

int CBooguOrgDir::CompareByFilenameNoCase( std::wstring element1, std::wstring element2 ) 
{
	std::transform( element1.begin(), element1.end(), element1.begin(), toupper );
	std::transform( element2.begin(), element2.end(), element2.begin(), toupper );
	return lstrcmpi( element1.c_str(), element2.c_str() );
}

std::wstring CBooguOrgDir::GetSelectedPath()
{
  return ItemToPath( GetSelectedItem() );
}

void CBooguOrgDir::SetShowFiles( BOOL bFiles ) 
{ 
  m_bShowFiles = bFiles; 
  if( ::IsWindow( m_hWnd ) )
    OnViewRefresh();
}

void CBooguOrgDir::OnViewRefresh() 
{
	SetRedraw( FALSE );

	// Get the item which is currently selected
	HTREEITEM hSelItem = GetSelectedItem();
	std::wstring sItem  = ItemToPath( hSelItem );
	BOOL bExpanded = ( GetChildItem( hSelItem ) != NULL ); 

	// Display the folder items in the tree
	if( sItem == L"" )
		DisplayDrives( TVI_ROOT, TRUE );
	else
		DisplayPath( sItem, hSelItem, TRUE );
  
	// Reselect the initially selected item
	if( sItem.size() && sItem != m_sRootFolder+L"\\")
		hSelItem = SetSelectedPath( sItem, bExpanded );

	SetRedraw( TRUE );
}

std::wstring CBooguOrgDir::ItemToPath( HTREEITEM hItem )
{
	std::wstring sPath;
  
	// Create the full string of the tree item
	HTREEITEM hParent = hItem;
	std::wstring sItem = L"";
	int nLength = 0;
	wchar_t ItemText[512];

	while( hParent )
	{
		GetItemText( hParent, ItemText, 512 );
		sItem = ItemText;
		nLength = sItem.size();
		hParent = GetParentItem( hParent );

		if( sItem[ nLength - 1 ] == L'\\' )
			sPath = sItem + sPath;
		else
		{
			if( sPath.size() )
				sPath = sItem + L'\\' + sPath;
			else
				sPath = sItem;
		}
	}

	// Add the root folder if there is one
	if( m_sRootFolder.size() )
		sPath = m_sRootFolder + L'\\' + sPath;

	return sPath;
}

int CBooguOrgDir::GetIconIndex( HTREEITEM hItem )
{
	TV_ITEM tvi;
	ZeroMemory( &tvi, sizeof(TV_ITEM) );
	tvi.mask	= TVIF_IMAGE;
	tvi.hItem	= hItem;
	if( GetItem( &tvi ) )
		return tvi.iImage;
	else
		return -1;
}

int CBooguOrgDir::GetIconIndex( const std::wstring sFilename )
{
	// Retreive the icon index for a specified file/folder
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));

	if (SHGetFileInfo(sFilename.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON ) == 0 )
		return -1;
	DestroyIcon(sfi.hIcon);
	return sfi.iIcon;
}

int CBooguOrgDir::GetSelIconIndex( const std::wstring sFilename )
{
	// Retreive the icon index for a specified file/folder
	SHFILEINFO sfi;
	if( SHGetFileInfo( sFilename.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON ) == 0 )
		return -1;
	DestroyIcon(sfi.hIcon);
	return sfi.iIcon;
}

BOOL CBooguOrgDir::HasGotSubEntries( const std::wstring sDirectory )
{
	CFileFind find;
	std::wstring sFile;
	if( sDirectory[ sDirectory.size() - 1 ] == _T('\\') )
		sFile = sDirectory + _T("*.*");
	else
		sFile = sDirectory + _T("\\*.*");
	
	BOOL bFind = find.FindFile( sFile.c_str() );  
	while( bFind )
	{
		bFind = find.FindNextFile();
		if( find.IsDirectory() && !find.IsDots() )
			return TRUE;
		else if( !find.IsDirectory() && !find.IsHidden() && m_bShowFiles )
			return TRUE;
	}
	return FALSE;
}

void CBooguOrgDir::SetRootFolder( const std::wstring sPath )
{
	//SetEvent(m_hQuit);
	int nLength = sPath.size();
	if (nLength)
	{
		if ( sPath[ nLength - 1 ] == _T('\\') )
			m_sRootFolder = sPath.substr( 0, nLength - 1 );
	}
	else
		m_sRootFolder = sPath;
	if( IsWindow( ) )
	{
		TreeView_SelectItem(m_hWnd, NULL);
		OnViewRefresh();
	}
	//Param* pParam = new Param;
	//pParam->szPath = new TCHAR[sPath.length()+1];
	//memset(pParam->szPath, 0, (sPath.length()+1)*sizeof(TCHAR));
	//_tcscpy_s(pParam->szPath, sPath.length()+1, sPath.c_str());
	//pParam->hQuit = m_hQuit;
	//HANDLE hThread;
	//unsigned threadID;
	//hThread = (HANDLE)_beginthreadex( NULL, 0, &WatchDirectory, (void*)pParam, 0, &threadID);
}


HTREEITEM CBooguOrgDir::InsertFileItem(const std::wstring sFile, const std::wstring sPath, HTREEITEM hParent )
{
	// Retreive the icon indexes for the specified file/folder
	std::wstring FullPath = sPath;
	if( FullPath[ FullPath.length() - 1 ] != L'\\')
		FullPath += L"\\";
	FullPath += sFile;

	int nIconIndex = GetIconIndex( FullPath );
	int nSelIconIndex = GetSelIconIndex( FullPath );
	if( nIconIndex == -1 || nSelIconIndex == -1 )
	{
		ATLTRACE( L"Failed in call to SHGetFileInfo for %s, GetLastError:%d\n", sPath, ::GetLastError() );
		return NULL;
	}

	//Add the actual item
	std::wstring sTemp;
	if( sFile != L"" )
		sTemp = sFile;
	else
		sTemp = sPath;
	
	TV_INSERTSTRUCT tvis;
	ZeroMemory( &tvis, sizeof(TV_INSERTSTRUCT) );
	tvis.hParent		= hParent;
	tvis.hInsertAfter	= TVI_LAST;
	tvis.item.mask		= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvis.item.pszText	= (wchar_t*)sTemp.c_str();
	tvis.item.iImage	= nIconIndex;
	tvis.item.cChildren = HasGotSubEntries( FullPath );
	tvis.item.iSelectedImage = nSelIconIndex;
	HTREEITEM hItem = InsertItem( &tvis );
	return hItem;
}

void CBooguOrgDir::DisplayDrives( HTREEITEM hParent, BOOL bUseSetRedraw )
{
	CWaitCursor c;

	// Speed up the job by turning off redraw
	if( bUseSetRedraw )
		SetRedraw( FALSE );

	// Remove any items currently in the tree
	DeleteAllItems();

	// Enumerate the drive letters and add them to the tree control
	DWORD dwDrives = GetLogicalDrives();
	DWORD dwMask = 1;
	//std::wstring sDrive = "";
	wchar_t DriveLetter[2] = { L' ', L'\0' };
	std::wstring sDrive;
	
	for(int i = 0; i < 32; i++ )
	{
		if( dwDrives & dwMask )
		{
			DriveLetter[0] = i + L'A';
			sDrive = DriveLetter;
			sDrive += L":\\";

			if( sDrive == L"A:\\" || sDrive == L"B:\\" ){}
			else 
			{
				HTREEITEM hti = InsertFileItem( L"", sDrive, hParent );

				TCHAR szMark[MAX_PATH];
				_tcscpy_s(szMark, MAX_PATH, sDrive.c_str());
				PathAppend(szMark, L"boo.mark");
				if (TRUE == PathFileExists(szMark))
				{
					DisplayPath(sDrive, hti);
					if (GetChildItem( hti))
					{
						m_bInternalSendExpand = TRUE;
						TreeView_Expand(m_hWnd, hti, TVE_EXPAND);
					}
				}
			}
		}
		dwMask <<= 1;
	}

	if( bUseSetRedraw )
		SetRedraw( TRUE );
}

void CBooguOrgDir::DisplayPath( const std::wstring sPath, HTREEITEM hParent, BOOL bUseSetRedraw )
{
	CWaitCursor c;

	// Speed up the job by turning off redraw
	if( bUseSetRedraw )
		SetRedraw( FALSE );

	// Remove all the items currently under hParent
	HTREEITEM hChild = GetChildItem( hParent );
	while( hChild )
	{
		DeleteItem( hChild );
		hChild = GetChildItem( hParent );
	}

	// Find all the directories and files underneath sPath
	std::vector< std::wstring > DirectoryPaths;
	std::vector< std::wstring > FilePaths;
	
	CFileFind find;
	std::wstring sFile;
	std::wstring sNewPath;
	if( sPath[ sPath.size()-1] == _T('\\') )
		sFile = sPath + _T("*.*");
	else
		sFile = sPath + _T("\\*.*");
  
	BOOL bFind = find.FindFile( sFile.c_str() );  
	while( bFind )
	{
		bFind = find.FindNextFile();
		if( find.IsDirectory() )
		{
			if( !find.IsDots() && !find.IsHidden())
			{
				sNewPath = find.GetFilePath();
				DirectoryPaths.push_back( sNewPath );
			}
		}
		else 
		{
			if( !find.IsHidden() && m_bShowFiles &&  L"boo.mark" != find.GetFileName())
			{
				sNewPath = find.GetFilePath();
				FilePaths.push_back( sNewPath );
			}  
		}
	}


    wchar_t path_buffer[_MAX_PATH];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];

	// Now add all the directories to the tree control
	for( int i = 0; i < DirectoryPaths.size(); i++ )
	{
		_tsplitpath( DirectoryPaths[i].c_str(), NULL, NULL, fname, ext );
		_tmakepath( path_buffer, NULL, NULL, fname, ext);
		HTREEITEM hti = InsertFileItem( path_buffer, sPath, hParent );

		TCHAR szPath[MAX_PATH];
		_tcscpy_s(szPath, MAX_PATH, sPath.c_str());
		PathAppend(szPath, fname);
		PathAddBackslash(szPath);

		TCHAR szMark[MAX_PATH];
		_tcscpy_s(szMark, MAX_PATH, szPath);
		PathAppend(szMark, L"boo.mark");
		if (TRUE == PathFileExists(szMark))
		{
			DisplayPath(szPath, hti);
			if (GetChildItem( hti))
			{
				m_bInternalSendExpand = TRUE;
				TreeView_Expand(m_hWnd, hti, TVE_EXPAND);
			}
		}
		
	}

	// And the files to the tree control (if required)
	for(int i = 0; i < FilePaths.size(); i++ )
	{
		_tsplitpath( FilePaths[i].c_str(), NULL, NULL, fname, ext);
		_tmakepath(path_buffer, NULL, NULL, fname, ext);
		InsertFileItem(path_buffer, sPath, hParent);
	}
	// Turn back on the redraw flag
	if( bUseSetRedraw )
		SetRedraw( TRUE );
}

HTREEITEM CBooguOrgDir::FindSibling( HTREEITEM hParent, const std::wstring sItem )
{
	HTREEITEM hChild = GetChildItem( hParent );
	wchar_t sFound[ _MAX_PATH ];
	while( hChild )
	{
		GetItemText(hChild, sFound, _MAX_PATH );
		if( CompareByFilenameNoCase( sItem, sFound ) == 0 )
			return hChild;
		hChild = GetNextItem( hChild, TVGN_NEXT );
	}
	return NULL;
}

HTREEITEM CBooguOrgDir::SetSelectedPath( const std::wstring sPath, BOOL bExpanded )
{
	std::wstring sSearch = sPath;
	
	int nSearchLength = sSearch.size();
	if( nSearchLength == 0 )
	{
		ATLTRACE( _T("Cannot select a empty path\n") );
		return NULL;
	}

	// Remove trailing "\" from the path
	if( nSearchLength > 3 && sSearch[ nSearchLength - 1 ] == _T('\\'))
    sSearch = sSearch.substr( 0, nSearchLength - 1 );
  
	// Remove initial part of path if the root folder is setup
	int nRootLength = m_sRootFolder.size();
	if( nRootLength )
	{
		if( sSearch.find( m_sRootFolder ) != 0 )
		{
			ATLTRACE( _T("Could not select the path %s as the root has been configued as %s\n"), sPath, m_sRootFolder );
			return NULL;
		}
		sSearch = sSearch.substr( sSearch.size() - 1 - nRootLength );
	}

	if( sSearch == L"" )
		return NULL;
	
	SetRedraw( FALSE );

	HTREEITEM hItemFound = TVI_ROOT;
	int nFound = sSearch.find( L'\\' );
	BOOL bDriveMatch = TRUE;
	if( m_sRootFolder != L"" )
		bDriveMatch = FALSE;
	
	std::wstring sMatch = L"";
	while( nFound != -1 )
	{
		if( bDriveMatch )
		{
			sMatch = sSearch.substr( 0, nFound + 1 );
			bDriveMatch = FALSE;
		}
		else
			sMatch = sSearch.substr( 0, nFound );
		hItemFound = FindSibling( hItemFound, sMatch );
		if ( hItemFound == NULL )
			break;
		else
			Expand( hItemFound, TVE_EXPAND );

		sSearch = sSearch.substr( nFound + 1 );
		nFound = sSearch.find( _T('\\') );
	}

	// The last item 
	if( hItemFound )
	{
		if( sSearch.size() )
			hItemFound = FindSibling( hItemFound, sSearch );
		SelectItem( hItemFound );

		if( bExpanded )
		Expand( hItemFound, TVE_EXPAND );
	}

	SetRedraw( TRUE );

	return hItemFound;
}

LRESULT CBooguOrgDir::OnPopulateTree(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// attach the image list to the tree control
	SetImageList( *(g_SysImageList.GetImageList()), TVSIL_NORMAL );
	// Force a refresh
	OnViewRefresh();
	return 0L;
}

LRESULT CBooguOrgDir::OnLButtonDblClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::wstring path = GetSelectedPath();
	::SendMessage( GetParent(), WM_ITEM_SELECTED, (WPARAM)path.c_str(), 0 );
	if (PathIsDirectory(path.c_str()))
	{
		HTREEITEM hti = TreeView_GetSelection(m_hWnd);
		TreeView_Expand(m_hWnd, hti, TVE_TOGGLE);
	}
	else
	{
		if (0 == wcscmp(L".boo", PathFindExtension(path.c_str())))
		{
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
				ShellExecute(NULL, _T("open"), path.c_str(), 0, 0, SW_SHOWNORMAL);
			}
			else
			{
				::SendMessage(GetParent().GetParent(), WM_USER_MAINFRM_OPENFILE, (WPARAM)path.c_str(), 0);
			}
		}
		else
		{
			ShellExecute(NULL, _T("open"), path.c_str(), 0, 0, SW_SHOWNORMAL);
		}
	}
	
	return 0;	
}

LRESULT CBooguOrgDir::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	GetCursorPos(&pt);
	TVHITTESTINFO tvhi = {0};
	tvhi.pt = pt;
	ScreenToClient(&(tvhi.pt));
	HTREEITEM hTI = TreeView_HitTest(m_hWnd, &tvhi);
	TreeView_SelectItem(m_hWnd,hTI);

	CMenu menu;
	menu.LoadMenu(IDR_MENU_ORG);
	CMenuHandle menuPopup = menu.GetSubMenu(0);

	std::wstring path = GetSelectedPath();
	PTSTR test = PathFindExtension(path.c_str());
	if (PathIsDirectory(path.c_str()))
	{
		CMenuItemInfo mi;
		mi.fMask = MIIM_STRING|MIIM_ID|MIIM_STATE;
		mi.fState = MFS_ENABLED;

		mi.wID = ID_POPMENU_USEASROOT;
		mi.dwTypeData = _T("设为根路径");
		mi.cch = lstrlen(mi.dwTypeData);
		menuPopup.InsertMenuItemW(1, TRUE, &mi);

		mi.wID = ID_POPMENU_OPEN;
		mi.dwTypeData = _T("打开");
		mi.cch = lstrlen(mi.dwTypeData);
		menuPopup.InsertMenuItemW(1, TRUE, &mi);
	}
	else
	{
		if (0 == wcscmp(L".boo", PathFindExtension(path.c_str())))
		{
			CMenuItemInfo mi;
			mi.fMask = MIIM_STRING|MIIM_ID|MIIM_STATE;
			mi.fState = MFS_ENABLED;

			mi.wID = ID_POPMENU_OPEN;
			mi.dwTypeData = _T("在新窗口中打开\tCtrl+双击");
			mi.cch = lstrlen(mi.dwTypeData);
			menuPopup.InsertMenuItemW(0, TRUE, &mi);

			mi.wID = ID_POPMENU_OPENIN;
			mi.dwTypeData = _T("打开\t双击");
			mi.cch = lstrlen(mi.dwTypeData);
			menuPopup.InsertMenuItemW(0, TRUE, &mi);
		}
	}
	

	//ScreenToClient(&pt);
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON,
		(int)pt.x, (int)pt.y, m_hWnd);
	menuPopup.DestroyMenu();
	menu.DestroyMenu();
	
	bHandled = TRUE;
	return 0;	
}

LRESULT CBooguOrgDir::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(GetParent(), uMsg, wParam, lParam);
	bHandled = FALSE;
	return 0;
}

LRESULT CBooguOrgDir::OnItemExpanding(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pnmh;
	if( pNMTreeView->action == TVE_EXPAND )
	{
		// Add the new items to the tree if it does not have any child items
		// already
		//if( !GetChildItem( pNMTreeView->itemNew.hItem ) )
		//{
			std::wstring sPath = ItemToPath(pNMTreeView->itemNew.hItem);
			DisplayPath( sPath, pNMTreeView->itemNew.hItem );
			if (!m_bInternalSendExpand)
			{
				CString tmpFileName = sPath.c_str();
				PathAppend(tmpFileName.GetBuffer(MAX_PATH), L"boo.mark");
				HANDLE hFile = CreateFileW(tmpFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
				CloseHandle(hFile);
			}
			else
			{
				m_bInternalSendExpand = FALSE;
			}
		//}
	}
	else if (TVE_COLLAPSE == pNMTreeView->action)
	{
		if (!m_bInternalSendExpand)
		{
			std::wstring sPath = ItemToPath(pNMTreeView->itemNew.hItem);
			CString tmpFileName = sPath.c_str();
			PathAppend(tmpFileName.GetBuffer(MAX_PATH), L"boo.mark");
			DeleteFile((LPCTSTR)tmpFileName);
		}
		else
		{
			m_bInternalSendExpand = FALSE;
		}
	}
	return 0;
}

LRESULT CBooguOrgDir::OnBeginDrag(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pnmh;

	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;

	m_DragImage = CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control

	m_bLDragging = TRUE;
	m_DragImage.BeginDrag(0, CPoint(-15,-15));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_DragImage.DragEnter(NULL, pt);
	SetCapture();
	return 0;
}

LRESULT CBooguOrgDir::OnBeginLabelEdit(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	/*hEdit=*/TreeView_GetEditControl(m_hWnd);
	m_bEdit = true;
	return 0;
}

LRESULT CBooguOrgDir::OnEndLabelEdit(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	bHandled = TRUE;
	m_bEdit = false;
	NMTVDISPINFO* p = (NMTVDISPINFO*)pnmh;
	if (NULL == p->item.pszText)
	{
		return FALSE;
	}
	else
	{
		CString strTemp = p->item.pszText;
		if (-1 != strTemp.FindOneOf(L"\"#%&*:<>?\\\/{|}~"))
		{
			MessageBox(L"Name cannot include '\"#%&*:<>?\\\/{|}~'", L"Warning", MB_OK|MB_ICONWARNING);
			return FALSE;
		}
		else
		{
			TCHAR* szFrom = new TCHAR[1024*1024];
			TCHAR* szTo = new TCHAR[1024*1024];
			std::wstring strDir = GetSelectedPath();
			_stprintf(szFrom, _T("%s\0\0"), strDir.c_str());
			_stprintf(szTo, _T("%s\0\0"),  strDir.c_str());
			PathRemoveBackslash(szTo);
			PathRemoveFileSpec(szTo);
			PathAppend(szTo, p->item.pszText);
			SHFILEOPSTRUCT shfo;
			shfo.hwnd = m_hWnd;
			shfo.lpszProgressTitle = szFrom;
			shfo.wFunc = FO_RENAME;
			shfo.fFlags = 0;//FOF_MULTIDESTFILES;
			shfo.pTo = szTo;
			shfo.pFrom = szFrom;

			int iRC = SHFileOperation(&shfo);
			delete [] szFrom;
			delete [] szTo;

			if (0 != iRC)
			{
				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
	}
	
	return TRUE;
}


LRESULT CBooguOrgDir::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	HTREEITEM	hitem;
	UINT		flags;

	if (m_bLDragging)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam); 
		pt.y = GET_Y_LPARAM(lParam); 
		POINT _pt = pt;
		ClientToScreen( &_pt );
		CImageList::DragMove(_pt);
		if ((hitem = HitTest(pt, &flags)) != NULL)
		{
			CImageList::DragShowNolock(FALSE);
			SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			CImageList::DragShowNolock(TRUE);
		}
	}
	return 0;
}

LRESULT CBooguOrgDir::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(m_hWnd);
		CImageList::EndDrag();
		ReleaseCapture();

		//delete m_pDragImage;

		// Remove drop target highlighting
		SelectDropTarget(NULL);

		if( m_hitemDrag == m_hitemDrop )
			return -1;

		// If Drag item is an ancestor of Drop item then return
		HTREEITEM htiParent = m_hitemDrop;
		while( (htiParent = GetParentItem( htiParent )) != NULL )
		{
			if( htiParent == m_hitemDrag ) return -1;
		}

		Expand( m_hitemDrop, TVE_EXPAND ) ;

		//HTREEITEM htiNew = CopyBranch( m_hitemDrag, m_hitemDrop, TVI_LAST );
		std::wstring strFromPath = ItemToPath(m_hitemDrag);
		std::wstring strToPath = ItemToPath(m_hitemDrop);
		

		TCHAR* szFrom = new TCHAR[1024*1024];
		//if (PathIsDirectory(strFromPath.c_str()))
		//{	
		//	_stprintf(szFrom, _T("%s\\*\0\0"), strFromPath.c_str());
		//}
		//else
		//{
		//	_stprintf(szFrom, _T("%s*\0\0"), strFromPath.c_str());
		//}
		_stprintf(szFrom, _T("%s\0\0"), strFromPath.c_str());
		//_stprintf(szFrom+strFromPath.length()+1, _T("%s\\*\0\0"), strFromPath.c_str());
		TCHAR* szTo = new TCHAR[1024*1024];
		CString tempFolderPath = strToPath.c_str();
		if (FALSE == PathIsDirectory(strToPath.c_str()))
		{
			PathRemoveBackslash(tempFolderPath.GetBuffer(MAX_PATH));
			tempFolderPath.ReleaseBuffer();
			PathRemoveFileSpec(tempFolderPath.GetBuffer(MAX_PATH));
			tempFolderPath.ReleaseBuffer();
			m_hitemDrop = GetParentItem(m_hitemDrop);
			//tempFolderPath = ItemToPath(htiParent).c_str();
		}
		PathRemoveBackslash(tempFolderPath.GetBuffer(MAX_PATH));
		tempFolderPath.ReleaseBuffer();
		if (L"" == tempFolderPath)
		{
			if (L"" == GetRootFolder())
			{
				return 0;
			}
			else
			{
				tempFolderPath = GetRootFolder().c_str();
			}
		}
		/*if (tempFolderPath.Right(1) == _T('\\'))
		{
			tempFolderPath.TrimRight(_T('\\'));
		}*/
		_stprintf(szTo, _T("%s\0\0"), tempFolderPath.GetBuffer(_MAX_PATH + 100));
		tempFolderPath.ReleaseBuffer();

		SHFILEOPSTRUCT shfo;
		shfo.hwnd = m_hWnd;
		shfo.lpszProgressTitle = szFrom;
		shfo.wFunc = FO_MOVE;
		shfo.fFlags = 0;//FOF_MULTIDESTFILES;
		shfo.pTo = szTo;
		shfo.pFrom = szFrom;

		int iRC = SHFileOperation(&shfo);


		if (!shfo.fAnyOperationsAborted)
		{
			DeleteItem(m_hitemDrag);
			htiParent = GetParentItem(m_hitemDrop);
			if (NULL != htiParent)
			{
				std::wstring sPath = ItemToPath(htiParent);
				DisplayPath( sPath, htiParent, TRUE );
			}
			else
			{
				if (L"" == this->GetRootFolder())
				{
					DisplayDrives(TVI_ROOT, m_bShowFiles);
				}
				else
				{
					DisplayPath(this->GetRootFolder().c_str(), TVI_ROOT);
				}
			}
			tempFolderPath = szFrom;
			PathRemoveBackslash(tempFolderPath.GetBuffer(MAX_PATH));
			tempFolderPath.ReleaseBuffer();
			tempFolderPath.Delete(0, tempFolderPath.ReverseFind(L'\\')+1);
			PathAddBackslash(szTo);
			tempFolderPath = szTo + tempFolderPath;
			SetSelectedPath((LPCTSTR)tempFolderPath);
			//SelectItem( m_hitemDrop );
		}

		delete [] szFrom;
		delete [] szTo;
		
		//;
	}
	return 0;
}

LRESULT CBooguOrgDir::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	//CRect rcWindow;
	//GetWindowRect(&rcWindow);
	//GetParent().ScreenToClient(&rcWindow);
	//rcWindow.InflateRect(20,20);
	//HDC hDC = ::GetDC(GetParent());
	//CDCHandle dc(hDC);
	//
	//CPen penScrollBarBorder;
	//penScrollBarBorder.CreatePen(PS_SOLID, 1, RGB(86,146,212));
	//dc.SelectPen(penScrollBarBorder);
	//dc.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));

	//dc.RoundRect(rcWindow, CPoint(20,20));

	//::ReleaseDC(GetParent(), hDC);

	bHandled = FALSE;
	return 0;
}

//bool CBooguOrgDir::HitTest(int x, int y)
//{
//	return false;
//}

void CBooguOrgDir::DoPaint(HDC hdc)
{
}
LRESULT CBooguOrgDir::OnPopmenuCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HTREEITEM hti = TreeView_GetSelection(m_hWnd);
	m_hitemCut = hti;

	TreeView_SetItemState(m_hWnd, hti, TVIS_CUT, TVIS_CUT);

	std::wstring path = GetSelectedPath();
	CString strDir = path.c_str();;
	
	CutOrCopyFiles((LPCTSTR)strDir, strDir.GetLength(), FALSE);

	return 0;
}

LRESULT CBooguOrgDir::OnPopmenuRfr(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HTREEITEM hti = GetSelectedItem();
	std::wstring path = GetSelectedPath();
	
	//if (!GetChildItem(hti))
	//{
		HTREEITEM hParent = GetParentItem(hti);
		if (NULL != hParent)
		{
			std::wstring sPath = ItemToPath(hParent);
			DisplayPath( sPath, hParent, TRUE );
		}
		else
		{
			TreeView_SelectItem(m_hWnd, NULL);
			OnViewRefresh();
		}
	/*}
	else
	{
		if ((TreeView_GetItemState(m_hWnd, hti, TVIF_STATE) & TVIS_EXPANDED))
		{
			std::wstring sPath = ItemToPath(hti);
			DisplayPath( sPath, hti, TRUE );
		}
	}*/


	return 0;
}

LRESULT CBooguOrgDir::OnPopmenuCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//HTREEITEM hti = TreeView_GetSelection(m_hWnd);

	//TreeView_SetItemState(m_hWnd, hti, TVIS_CUT, TVIS_CUT);

	std::wstring path = GetSelectedPath();
	CString strDir = path.c_str();;

	CutOrCopyFiles((LPCTSTR)strDir, strDir.GetLength(), TRUE);

	return 0;
}

LRESULT CBooguOrgDir::OnPopmenuPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	std::wstring path = GetSelectedPath();
	HTREEITEM htiPaste = GetSelectedItem();
	//CString strDir = path.c_str();

	//TCHAR* szFrom = new TCHAR[1024*1024];
	TCHAR* szTo = new TCHAR[1024*1024];
	_stprintf(szTo, _T("%s\0\0"), path.c_str());

	if (FALSE == PathIsDirectory(szTo))
	{
		PathRemoveBackslash(szTo);
		PathRemoveFileSpec(szTo);
		htiPaste = GetParentItem(htiPaste);
		//tempFolderPath = ItemToPath(htiParent).c_str();
	}

	UINT uDropEffect=RegisterClipboardFormat(L"Preferred DropEffect");

	if( OpenClipboard()) 
	{     //hWnd是某一句柄，也可以为NULL
		HDROP hDrop = HDROP( GetClipboardData( CF_HDROP));
		if( hDrop) 
		{
			DWORD dwEffect,*dw;
			dw=(DWORD*)(GetClipboardData( uDropEffect));
			if(dw==NULL)
				dwEffect=DROPEFFECT_COPY;
			else
				dwEffect=*dw;

			TCHAR Buf[4096];
			Buf[0] = 0;
			UINT cFiles = DragQueryFile( hDrop, (UINT) -1, NULL, 0);
			POINT Point;
			TCHAR szFile[ MAX_PATH*2];
			for( UINT count = 0; count < cFiles; count++ ) 
			{
				DragQueryFile( hDrop, count, szFile, sizeof( szFile));
				szFile[_tcslen(szFile)+1] = L'\0';
				SHFILEOPSTRUCT shfo;
				shfo.hwnd = m_hWnd;
				shfo.lpszProgressTitle = szFile;
				if(dwEffect & DROPEFFECT_MOVE)
				{
					shfo.wFunc = FO_MOVE;
				}
				else
				{
					shfo.wFunc = FO_COPY;
				}
				
				shfo.fFlags = 0;//FOF_MULTIDESTFILES;
				shfo.pTo = szTo;
				shfo.pFrom = szFile;
				if ( FO_COPY == shfo.wFunc)
				{
					if (_tcscmp(shfo.pTo, shfo.pFrom) == 0)
					{
						
					}
				}

				int iRC = SHFileOperation(&shfo);


				if (!shfo.fAnyOperationsAborted)
				{
				}
				//lstrcat(Buf,szFile);
				//lstrcat(Buf," ");
			}

			if (NULL != m_hitemCut)
			{
				DeleteItem(m_hitemCut);
				m_hitemCut = NULL;
			}
			
			
			htiPaste = GetParentItem(htiPaste);
			if (NULL != htiPaste)
			{
				std::wstring sPath = ItemToPath(htiPaste);
				DisplayPath( sPath, htiPaste, TRUE );
			}
			else
			{
				if (L"" == this->GetRootFolder())
				{
					DisplayDrives(TVI_ROOT, m_bShowFiles);
				}
				else
				{
					DisplayPath(this->GetRootFolder().c_str(), TVI_ROOT);
				}
			}

			CloseClipboard();
		}
	} 

	

	//delete[] szFrom;
	delete[] szTo;

	return 0;
}

LRESULT CBooguOrgDir::OnPopmenuEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	
	HTREEITEM htiEdit = GetSelectedItem();
	if(NULL != htiEdit)
	{
		SendMessage(TVM_EDITLABEL, 0, (LPARAM)htiEdit);
	}
	return 0;
}

LRESULT CBooguOrgDir::OnPopmenuDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HTREEITEM htiDel = GetSelectedItem();
	if (NULL != htiDel)
	{
		TCHAR* szFrom = new TCHAR[1024*1024];
		//TCHAR* szTo = new TCHAR[1024*1024];
		std::wstring strDir = GetSelectedPath();
		_stprintf(szFrom, _T("%s\0\0"), strDir.c_str());
		//_stprintf(szTo, _T("%s\0\0"),  strDir.c_str());
		//PathRemoveBackslash(szTo);
		//PathRemoveFileSpec(szTo);
		//PathAppend(szTo, p->item.pszText);
		SHFILEOPSTRUCT shfo;
		shfo.hwnd = m_hWnd;
		shfo.lpszProgressTitle = szFrom;
		shfo.wFunc = FO_DELETE;
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			shfo.fFlags = 0;
		}
		else
		{
			shfo.fFlags = FOF_ALLOWUNDO;
		}
		shfo.pTo = NULL;
		shfo.pFrom = szFrom;

		int iRC = SHFileOperation(&shfo);
		delete [] szFrom;
		if (0 == iRC && !shfo.fAnyOperationsAborted)
		{
			DeleteItem(htiDel);
		}
		//delete [] szTo;
	}

	return 0;
}

LRESULT CBooguOrgDir::OnPopmenuNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	std::wstring path = GetSelectedPath();
	HTREEITEM htiNew = GetSelectedItem();

	TCHAR* szNew = new TCHAR[1024*1024];
	_stprintf(szNew, _T("%s\0"), path.c_str());

	if (FALSE == PathIsDirectory(szNew))
	{
		PathRemoveBackslash(szNew);
		PathRemoveFileSpec(szNew);
		htiNew = GetParentItem(htiNew);
	}
	PathAddBackslash(szNew);
	CString strFullPath = szNew;
	CString strFileName;
	int i=0;
	do 
	{
		strFileName.Format(L"Notes%d.boo", i);
		strFullPath.Format(L"%s%s", szNew, strFileName);
		i++;
	} while (TRUE == PathFileExists(strFullPath));
	
	CFileFind find;
	std::wstring sFile = szNew;
	sFile += _T("\\*.*");
	int nCount = 0;

	BOOL bFind = find.FindFile( sFile.c_str() );  
	while( bFind )
	{
		bFind = find.FindNextFile();
		if( find.IsDirectory() )
		{
			if( !find.IsDots() && !find.IsHidden())
			{
				nCount++;
			}
		}
		else 
		{
			if( !find.IsHidden() && m_bShowFiles &&  L"boo.mark" != find.GetFileName())
			{
				nCount++;
			}  
		}
	}

	//BOOL bPathEmpty = PathIsDirectoryEmpty(szNew);
	CreateEmptyFile(LPCTSTR(strFullPath));
	HTREEITEM htiAdd = InsertFileItem(LPCTSTR(strFileName), szNew, htiNew);
	HTREEITEM hParent = GetParentItem(htiNew);
	if (nCount<=0)
	{
		if (NULL != hParent)
		{
			std::wstring sPath = ItemToPath(hParent);
			DisplayPath( sPath, hParent, TRUE );
		}
		else
		{
			TreeView_SelectItem(m_hWnd, NULL);
			OnViewRefresh();
		}
		SetSelectedPath(szNew);
		htiNew = GetSelectedItem();
		Expand(htiNew, TVE_EXPAND);
		SetSelectedPath(LPCTSTR(strFullPath));
		htiAdd = GetSelectedItem();
		
	}
	else
	{
		Expand(htiNew, TVE_EXPAND);
		SelectItem(htiAdd);
		SendMessage(TVM_EDITLABEL, 0, (LPARAM)htiAdd);
	}
	
	//SendMessage(TVM_EDITLABEL, 0, (LPARAM)htiAdd);

	return 0;
}

LRESULT CBooguOrgDir::OnPopmenuNewfolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	std::wstring path = GetSelectedPath();
	HTREEITEM htiNew = GetSelectedItem();

	TCHAR* szNew = new TCHAR[1024*1024];
	_stprintf(szNew, _T("%s\0"), path.c_str());

	if (FALSE == PathIsDirectory(szNew))
	{
		PathRemoveBackslash(szNew);
		PathRemoveFileSpec(szNew);
		htiNew = GetParentItem(htiNew);
	}
	PathAddBackslash(szNew);
	CString strFullPath = szNew;
	CString strFileName;
	int i=0;
	do 
	{
		strFileName.Format(L"New Folder%d", i);
		strFullPath.Format(L"%s%s\\", szNew, strFileName);
		i++;
	} while (TRUE == PathFileExists(strFullPath));

	CFileFind find;
	std::wstring sFile = szNew;
	sFile += _T("\\*.*");
	int nCount = 0;

	BOOL bFind = find.FindFile( sFile.c_str() );  
	while( bFind )
	{
		bFind = find.FindNextFile();
		if( find.IsDirectory() )
		{
			if( !find.IsDots() && !find.IsHidden())
			{
				nCount++;
			}
		}
		else 
		{
			if( !find.IsHidden() && m_bShowFiles &&  L"boo.mark" != find.GetFileName())
			{
				nCount++;
			}  
		}
	}

	//BOOL bPathEmpty = PathIsDirectoryEmpty(szNew);
	CreateDirectory(LPCTSTR(strFullPath), NULL);
	Expand(htiNew, TVE_EXPAND);
	HTREEITEM htiAdd = InsertFileItem(LPCTSTR(strFileName), szNew, htiNew);
	HTREEITEM hParent = GetParentItem(htiNew);
	if (nCount<=0)
	{
		if (NULL != hParent)
		{
			std::wstring sPath = ItemToPath(hParent);
			DisplayPath( sPath, hParent, TRUE );
		}
		else
		{
			TreeView_SelectItem(m_hWnd, NULL);
			OnViewRefresh();
		}
		SetSelectedPath(szNew);
		htiNew = GetSelectedItem();
		Expand(htiNew, TVE_EXPAND);
		SetSelectedPath(LPCTSTR(strFullPath));
		htiAdd = GetSelectedItem();

	}
	else
	{
		Expand(htiNew, TVE_EXPAND);
		SelectItem(htiAdd);

		SendMessage(TVM_EDITLABEL, 0, (LPARAM)htiAdd);
	}
	//SendMessage(TVM_EDITLABEL, 0, (LPARAM)htiAdd);

	return 0;
}
