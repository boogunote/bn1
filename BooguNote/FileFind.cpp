/*
Module : FileFind.h
Purpose: Implementation of FileFind class for WTL which performs local file searches

Copyright (c) 2003 by iLYA Solnyshkin. 
E-mail: isolnyshkin@yahoo.com 
All rights reserved.
*/


#include "stdafx.h"
#include "FileFind.h"

////////////////////////////////////////////////////////////////////////////
// CFileFind implementation

CFileFind::CFileFind()
{
	m_pFoundInfo = NULL;
	m_pNextInfo = NULL;
	m_hContext = NULL;
	m_chDirSeparator = '\\';
}

CFileFind::~CFileFind()
{
	Close();
}

void CFileFind::Close()
{
	if (m_pFoundInfo != NULL)
	{
		delete m_pFoundInfo;
		m_pFoundInfo = NULL;
	}

	if (m_pNextInfo != NULL)
	{
		delete m_pNextInfo;
		m_pNextInfo = NULL;
	}

	if (m_hContext != NULL && m_hContext != INVALID_HANDLE_VALUE)
	{
		CloseContext();
		m_hContext = NULL;
	}
}

void CFileFind::CloseContext()
{
	::FindClose(m_hContext);
	return;
}

BOOL CFileFind::FindFile(LPCTSTR pstrName /* = NULL */, DWORD dwUnused /* = 0 */)
{
	Close();
	m_pNextInfo = new WIN32_FIND_DATA;
	m_bGotLast = FALSE;

	if (pstrName == NULL)
		pstrName = _T("*.*");
	lstrcpy(((WIN32_FIND_DATA*) m_pNextInfo)->cFileName, pstrName);

	m_hContext = ::FindFirstFile(pstrName, (WIN32_FIND_DATA*) m_pNextInfo);

	if (m_hContext == INVALID_HANDLE_VALUE)
	{
		DWORD dwTemp = ::GetLastError();
		Close();
		::SetLastError(dwTemp);
		return FALSE;
	}

	wchar_t pstrRoot[ _MAX_PATH ];
	LPCWSTR pstr = _wfullpath((LPWSTR)pstrRoot, pstrName, _MAX_PATH);

	// passed name isn't a valid path but was found by the API
	ATLASSERT(pstr != NULL);
	if (pstr == NULL)
	{
		m_strRoot = L"";
		Close();
		::SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}
	else
	{
		// find the last forward or backward whack
		LPTSTR pstrBack  = _tcsrchr(pstrRoot, L'\\');
		LPTSTR pstrFront = _tcsrchr(pstrRoot, L'/');

		if (pstrFront != NULL || pstrBack != NULL)
		{
			if (pstrFront == NULL)
				pstrFront = (wchar_t*)pstrRoot;
			if (pstrBack == NULL)
				pstrBack = (wchar_t*)pstrRoot;

			// from the start to the last whack is the root

			if (pstrFront >= pstrBack)
				*pstrFront = L'\0';
			else
				*pstrBack = L'\0';
		}
		m_strRoot = L"";
	}

	return TRUE;
}

BOOL CFileFind::MatchesMask(DWORD dwMask) const
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL)
		return (!!(((LPWIN32_FIND_DATA) m_pFoundInfo)->dwFileAttributes & dwMask));
	else
		return FALSE;
}

BOOL CFileFind::GetLastAccessTime(FILETIME* pTimeStamp) const
{
	if( m_hContext == NULL)
		return FALSE;
	if( pTimeStamp == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastAccessTime;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFileFind::GetLastWriteTime(FILETIME* pTimeStamp) const
{
	if( m_hContext == NULL)
		return FALSE;
	if( pTimeStamp == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastWriteTime;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFileFind::GetCreationTime(FILETIME* pTimeStamp) const
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftCreationTime;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFileFind::IsDots() const
{
	if( m_hContext == NULL)
		return FALSE;

	// return TRUE if the file name is "." or ".." and
	// the file is a directory

	BOOL bResult = FALSE;
	if (m_pFoundInfo != NULL && IsDirectory())
	{
		LPWIN32_FIND_DATA pFindData = (LPWIN32_FIND_DATA) m_pFoundInfo;
		if (pFindData->cFileName[0] == '.')
		{
			if (pFindData->cFileName[1] == '\0' ||
				(pFindData->cFileName[1] == '.' &&
				 pFindData->cFileName[2] == '\0'))
			{
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL CFileFind::FindNextFile()
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo == NULL)
		m_pFoundInfo = new WIN32_FIND_DATA;

	void* pTemp = m_pFoundInfo;
	m_pFoundInfo = m_pNextInfo;
	m_pNextInfo = pTemp;

	return ::FindNextFile(m_hContext, (LPWIN32_FIND_DATA) m_pNextInfo);
}

std::wstring CFileFind::GetFileURL() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring strResult = L"file://";
	strResult += GetFilePath();
	return strResult;
}

std::wstring CFileFind::GetRoot() const
{
	if( m_hContext == NULL)
		return FALSE;

	return m_strRoot;
}

std::wstring CFileFind::GetFilePath() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring strResult = m_strRoot;
	if (strResult.size()>1 && (strResult[strResult.size()-1] != '\\' &&
		strResult[strResult.size()-1] != '/'))
		strResult += m_chDirSeparator;
	strResult += GetFileName();
	return strResult;
}

std::wstring CFileFind::GetFileTitle() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring strFullName = GetFileName();
	wchar_t strResult[_MAX_PATH];

	_tsplitpath( strFullName.c_str(), NULL, NULL, strResult, NULL);
	return strResult;
}

std::wstring CFileFind::GetFileName() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring ret;

	if (m_pFoundInfo != NULL)
		ret = ((LPWIN32_FIND_DATA) m_pFoundInfo)->cFileName;
	return ret;
}

DWORD CFileFind::GetLength() const
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL)
		return ((LPWIN32_FIND_DATA) m_pFoundInfo)->nFileSizeLow;
	else
		return 0;
}
