// Session.h: interface for the CSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSION_H__435C958A_97AB_481A_B45D_19A986F9FAED__INCLUDED_)
#define AFX_SESSION_H__435C958A_97AB_481A_B45D_19A986F9FAED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "SessionConfiguration.h"
#include "SocketTest.h"

class CSession: public CObject  
{
public:
	CSession();
	virtual ~CSession();

	virtual BOOL Open(const CSessionConfiguration &aSC, int nSessionNumber=0);
	virtual BOOL AddTest(const CArray<CSocketTest, CSocketTest&> &aST);
	virtual BOOL AddMapCod(const CString &sNum, const CString & sName, const CString &sUnit);
	virtual BOOL AddBinDescription(const CString &sDesc);
	virtual BOOL AddProbeParam(const CString &sProbeParam);
	virtual BOOL Close(void);
};

#endif // !defined(AFX_SESSION_H__435C958A_97AB_481A_B45D_19A986F9FAED__INCLUDED_)
