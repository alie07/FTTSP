#if !defined(AFX_POLLMASKDLG_H__EF86FE81_DB72_42CC_93A6_8F885BE7DBE7__INCLUDED_)
#define AFX_POLLMASKDLG_H__EF86FE81_DB72_42CC_93A6_8F885BE7DBE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PollMaskDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPollMaskDlg dialog

class CPollMaskDlg : public CDialog
{
// Construction
public:
	CPollMaskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPollMaskDlg)
	enum { IDD = IDD_POLL_MASK };
	CEdit	m_Run_Nb;
	CStatic	m_sMask;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPollMaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPollMaskDlg)
		virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLLMASKDLG_H__EF86FE81_DB72_42CC_93A6_8F885BE7DBE7__INCLUDED_)
