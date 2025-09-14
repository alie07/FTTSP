#if !defined(AFX_DIALOGPASSWORD_H__5323600D_AF6A_4A47_8BBC_5B4173C78FE4__INCLUDED_)
#define AFX_DIALOGPASSWORD_H__5323600D_AF6A_4A47_8BBC_5B4173C78FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogPassword.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogPassword dialog

class CDialogPassword : public CDialog
{
// Construction
public:
	CDialogPassword(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogPassword)
	enum { IDD = IDD_PASSWORD };
	CString	m_Password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPassword)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPassword)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGPASSWORD_H__5323600D_AF6A_4A47_8BBC_5B4173C78FE4__INCLUDED_)
