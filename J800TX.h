// J800TX.h: interface for the CJ800TX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_J800TX_H__B5FF76C3_42A1_4C4A_8D69_DC11BE626EEB__INCLUDED_)
#define AFX_J800TX_H__B5FF76C3_42A1_4C4A_8D69_DC11BE626EEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TX.h"

#define DEFAULT_TSTPRGM_PATH_J800	"D:\\Production\\TstPrgm"

class CJ800TX : public CTX
{
public:
	CJ800TX();
	virtual ~CJ800TX();

	virtual CString GetInstallPath(void);
protected:
	virtual CString GetTXCmd(BOOL bEngiMode=FALSE);
	CString TestXentralRelease();
	//bool RetrieveCtrlAddress(CStringArray *Ip_Site);

};

#endif // !defined(AFX_J800TX_H__B5FF76C3_42A1_4C4A_8D69_DC11BE626EEB__INCLUDED_)
