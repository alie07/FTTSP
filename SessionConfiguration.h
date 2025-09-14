// SessionConfiguration.h: interface for the CSessionConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONCONFIGURATION_H__9F1CFB15_BD3E_4D36_AFAE_3BE18A8E9D4D__INCLUDED_)
#define AFX_SESSIONCONFIGURATION_H__9F1CFB15_BD3E_4D36_AFAE_3BE18A8E9D4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Configuration.h"

class CSessionConfiguration : public CConfiguration
{
	DECLARE_DYNAMIC( CSessionConfiguration )
public:
	CSessionConfiguration();
	virtual ~CSessionConfiguration();

	virtual void Empty(void);
	virtual CString ExpandTags(LPCTSTR lpSrc) const;

	CString strLotNumber;
	CString strSubLotNumber;
	CString strHandlerDll;
	CString strHandlerDllVersion;
	BOOL    bEngiMode;
	CString strEngiModePassword;
	BOOL	bRTM;

protected:
	virtual BOOL _Parse(CString& str, char c);
};

#endif // !defined(AFX_SESSIONCONFIGURATION_H__9F1CFB15_BD3E_4D36_AFAE_3BE18A8E9D4D__INCLUDED_)
