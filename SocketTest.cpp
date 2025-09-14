// SocketTest.cpp: implementation of the CSocketTest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fttsp.h"
#include "SocketTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketTest::CSocketTest()
{

}

CSocketTest::~CSocketTest()
{

}

void CSocketTest::Empty()
{
	m_bEnabled = FALSE;
	m_nBin = 0;
	m_nBinHard = 0;
	m_nBinSoft = 0;
	m_dwSerialNumber = 0L;
	m_sMapCodes.Empty();
}
