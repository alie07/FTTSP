// HDUI.h: interface for the CHDUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDUI_H__EA650731_0899_46F4_8D88_BAB25FAF8FA3__INCLUDED_)
#define AFX_HDUI_H__EA650731_0899_46F4_8D88_BAB25FAF8FA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UI.h"

#define DEFAULT_TSTPRGM_PATH	"E:\\Production\\TstPrgm"

class CHDUI : public CUI  
{
public:
	CHDUI();
	virtual ~CHDUI();

	virtual CString GetInstallPath(void);
protected:
	virtual CString GetUICmd(BOOL bEngiMode=FALSE);
	CString NexTestRelease();
	bool RetrieveCtrlAddress(CStringArray *Ip_Site);


};

#endif // !defined(AFX_HDUI_H__EA650731_0899_46F4_8D88_BAB25FAF8FA3__INCLUDED_)
