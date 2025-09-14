// Session.cpp: implementation of the CSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fttsp.h"
#include "Session.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSession::CSession()
{

}

CSession::~CSession()
{

}

BOOL CSession::Open(const CSessionConfiguration &aSC, int nSessionNumber)
{
	return TRUE;
}

BOOL CSession::AddTest(const CArray<CSocketTest, CSocketTest&> &aST)
{
	return TRUE;
}

BOOL CSession::AddMapCod(const CString &sNum, const CString &sName, const CString &sUnit)
{
	return TRUE;
}

BOOL CSession::AddBinDescription(const CString &sDesc)
{
	return TRUE;
}

BOOL CSession::AddProbeParam(const CString &sProbeParam)
{
	return TRUE;
}

BOOL CSession::Close()
{
	return TRUE;
}
