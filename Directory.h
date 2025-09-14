// Directory.h: interface for the CDirectory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTORY_H__CDFC0A3F_CEB9_4ECE_BCA3_6C2FF1AF3040__INCLUDED_)
#define AFX_DIRECTORY_H__CDFC0A3F_CEB9_4ECE_BCA3_6C2FF1AF3040__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectory  
{
public:
	//CDirectory();
	//virtual ~CDirectory();
	static BOOL Create(LPCTSTR path);
};

#endif // !defined(AFX_DIRECTORY_H__CDFC0A3F_CEB9_4ECE_BCA3_6C2FF1AF3040__INCLUDED_)
