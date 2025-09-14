// PTUI.h: interface for the CPTUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PTUI_H__7741F0F1_65F7_483B_B2F7_4BA07FF2AF60__INCLUDED_)
#define AFX_PTUI_H__7741F0F1_65F7_483B_B2F7_4BA07FF2AF60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UI.h"

#define DEFAULT_TSTPRGM_PATH	"E:\\Production\\TstPrgm"

class CPTUI : public CUI  
{
public:
	CPTUI();
	virtual ~CPTUI();

	virtual CString GetInstallPath(void);
protected:
	virtual CString GetUICmd(BOOL bEngiMode=FALSE);
};

#endif // !defined(AFX_PTUI_H__7741F0F1_65F7_483B_B2F7_4BA07FF2AF60__INCLUDED_)
