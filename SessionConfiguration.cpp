// SessionConfiguration.cpp: implementation of the CSessionConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTTSP.h"
#include "SessionConfiguration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CSessionConfiguration, CConfiguration);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSessionConfiguration::CSessionConfiguration()
{

}

CSessionConfiguration::~CSessionConfiguration()
{

}

BOOL CSessionConfiguration::_Parse(CString& str, char c)
{
	if (CConfiguration::_Parse(str, c) &&
		// Parse All Data in String
		CStringUtils::GetChunk(str, strLotNumber  , c) &&
		CStringUtils::GetChunk(str, strHandlerDll , c) &&
		CStringUtils::GetChunk(str, bEngiMode     , c) &&
		CStringUtils::GetChunk(str, strEngiModePassword , c)
		)
		return TRUE;

	Empty();
	return FALSE;
}

void CSessionConfiguration::Empty(void)
{
	CConfiguration::Empty();
	
	strLotNumber.Empty();
	strSubLotNumber.Empty();
	strHandlerDll.Empty();
	strEngiModePassword.Empty();
}

CString CSessionConfiguration::ExpandTags(LPCTSTR lpSrc) const
{
	CString sSrc = CConfiguration::ExpandTags(lpSrc);
	CStringUtils::ReplaceTag(sSrc, "%LotNumber%", strLotNumber);

	return sSrc;
}
