// Implementation of the CFileDialogFilter class.
#pragma once

#include <atlmisc.h>

// Class to support a filter list when using the WTL CFileDialog.
// Allows a filter string delimited with a pipe to be used (instead of a string
// delimited with '\0')
class CFileDialogFilter  
{
private:
	CString m_strFilter;
public:
	CFileDialogFilter()
	{
	}

	/// nID The ID of a resource string containing the filter
	CFileDialogFilter(UINT nID)
	{
		SetFilter(nID);
	}
	
	/// lpsz The filter string
	CFileDialogFilter(LPCTSTR lpsz)
	{
		SetFilter(lpsz);
	}
	
	~CFileDialogFilter()
	{
	}

	inline LPCTSTR GetFilter() const { return m_strFilter; }
	inline operator LPCTSTR() const { return m_strFilter; }

	// Set the filter string to use
	// nID - The ID of a resource string containing the filter
	void SetFilter(UINT nID)
	{
		if (m_strFilter.LoadString(nID) && !m_strFilter.IsEmpty())
			ModifyString();
	}

	// Set the filter string to use
	// lpsz - The filter string
	void SetFilter(LPCTSTR lpsz)
	{		
		m_strFilter = lpsz;
		if (!m_strFilter.IsEmpty())
			ModifyString();
	}
private:
	// Replace '|' with '\0'
	void ModifyString(void)
	{
		// Get a pointer to the string buffer
		LPTSTR psz = m_strFilter.GetBuffer(0);
		// Replace '|' with '\0'
		while ((psz = _tcschr(psz, '|')) != NULL)
			*psz++ = '\0';
	}
};
