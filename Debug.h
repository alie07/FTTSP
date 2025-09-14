#if !defined(AFX_DEBUG_H__813C5289_A682_4EC4_B4FC_BBEA3AB1B497__INCLUDED_)
#define AFX_DEBUG_H__813C5289_A682_4EC4_B4FC_BBEA3AB1B497__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Debug.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebug dialog

class CDebug : public CDialog
{
// Construction
public:
	CDebug(CWnd* pParent = NULL);   // standard constructor
	void SetDebugStatus(BOOL bStatus);
	BOOL GetDebugStatus (void);
	// Dialog Data
	//{{AFX_DATA(CDebug)
	enum { IDD = IDD_DEBUG_DIALOG };
	CString	m_szDebug;
	//}}AFX_DATA

	void Print(const char *szMessage, const char *szPrefixMessage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebug)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	BOOL m_bDebugInProgress;
	// Generated message map functions
	//{{AFX_MSG(CDebug)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUG_H__813C5289_A682_4EC4_B4FC_BBEA3AB1B497__INCLUDED_)
