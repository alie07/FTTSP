// Directory.cpp: implementation of the CDirectory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fttsp.h"
#include "Directory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CDirectory::CDirectory()
//{
//
//}

//CDirectory::~CDirectory()
//{
//
//}


// Try to create Path indicated.
BOOL CDirectory::Create(LPCTSTR path)
{
	TCHAR pOldPath[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH+1, pOldPath);

	CString sPath = path;
	CString sTempDir;
	if (sPath.Left(2) == "\\\\")
	{
		int nServerNamePos = sPath.Find('\\', 2);
		if (nServerNamePos == -1)
			return FALSE;
		if (sPath.GetLength() <= nServerNamePos)
			return FALSE;
		int nSharePos = sPath.Find('\\', nServerNamePos + 1);
		if (nSharePos == -1)
			nSharePos = sPath.GetLength();

		sTempDir = sPath.Left(nSharePos);
	}

	sPath.TrimRight('\\');
	sPath+='\\';

	int nPos = sPath.Find('\\', sTempDir.GetLength());
	while (nPos != -1)
	{
		sTempDir = sPath.Left(nPos);
		if (! SetCurrentDirectory(sTempDir))
		{
			while (nPos != -1)
			{
				sTempDir = sPath.Left(nPos);
				if (! CreateDirectory(sTempDir, NULL))
					nPos = -1;
				nPos = sPath.Find('\\', sTempDir.GetLength() +1);
			}
		}
		else
		{
			nPos = sPath.Find('\\', sTempDir.GetLength() +1);
		}
	}

	sPath.TrimRight('\\');

	SetCurrentDirectory(pOldPath);
	return (sTempDir==sPath);
}