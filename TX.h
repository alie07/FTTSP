// TX.h: interface for the TX class. to drive TestXentral for Javelin
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TX_H__FDE82F63_7571_4834_85D3_2EA662EAF912__INCLUDED_)
#define AFX_TX_H__FDE82F63_7571_4834_85D3_2EA662EAF912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTX : public CObject
{
	DECLARE_DYNAMIC( CTX )
protected:
	// Declare as protected to prevent direct creation
	// Instead, call Create(const CString& type)
	CTX();

public:
	virtual BOOL Start(BOOL bEngiMode);
	static CTX* Create(const CString& type);
	virtual ~CTX();

	virtual CString GetInstallPath(void) = 0;

	static CString GetCurrentRelease();

	virtual BOOL DeleteOldProgram(void);
	virtual BOOL InstallProgram(const CString & aProgram);

	virtual void SetControllers(int nCtrl);
protected:
	CString m_InstalledProgramName;
	virtual CString GetTXCmd(BOOL bEngiMode=FALSE);
	LONG m_nCtrls;
	BOOL m_bTXStarted;

	CString m_TXPath;
	CString m_tx_release;
	CString TestXentralRelease();

	PROCESS_INFORMATION m_pi; // To handle TestXentral Application
};

#endif // !defined(AFX_TX_H__FDE82F63_7571_4834_85D3_2EA662EAF912__INCLUDED_)
