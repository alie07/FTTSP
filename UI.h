// UI.h: interface for the CUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UI_H__5FE5E752_47EC_4BC4_9E1E_12DD091490A0__INCLUDED_)
#define AFX_UI_H__5FE5E752_47EC_4BC4_9E1E_12DD091490A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUI : public CObject  
{
	DECLARE_DYNAMIC( CUI )
protected:
	// Declare as protected to prevent direct creation
	// Instead, call Create(const CString& type)
	CUI();
public:
	virtual BOOL Start(BOOL bEngiMode);
	static CUI* Create(const CString& type);
	virtual ~CUI();

	virtual CString GetInstallPath(void) = 0;

	static CString GetCurrentRelease(void);

	virtual BOOL DeleteOldProgram(void);
	virtual BOOL InstallProgram(const CString & aProgram);

	virtual void SetControllers(int nCtrl);
protected:
	CString m_InstalledProgramName;
	virtual CString GetUICmd(BOOL bEngiMode=FALSE);
	LONG m_nCtrls;
	BOOL m_bUIStarted;

	PROCESS_INFORMATION m_pi; // To handle UI (nextest) Application
};

#endif // !defined(AFX_UI_H__5FE5E752_47EC_4BC4_9E1E_12DD091490A0__INCLUDED_)
