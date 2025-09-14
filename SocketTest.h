// SocketTest.h: interface for the CSocketTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETTEST_H__D0D622EE_93BA_4293_BAF7_3DAB6241A34C__INCLUDED_)
#define AFX_SOCKETTEST_H__D0D622EE_93BA_4293_BAF7_3DAB6241A34C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSocketTest  
{
public:
	CSocketTest();
	virtual ~CSocketTest();

	virtual void Empty(void);

	BOOL m_bEnabled;
	int m_nBin;
	int m_nBinHard;
	int m_nBinSoft;
	DWORD m_dwSerialNumber;
	CString m_sMapCodes;
};

#endif // !defined(AFX_SOCKETTEST_H__D0D622EE_93BA_4293_BAF7_3DAB6241A34C__INCLUDED_)
