/******************************************************************************/
/*                                                                            */
/*  Layer           : Api			                                          */
/*                                                                            */
/*  CLASS	        : CAboutDlg			                                      */
/*  Description     : Dialog box 'About..." definition					      */
/*																			  */
/*                                                                            */
/*  File            : AboutDlg.h				                              */
/*                                                                            */
/*  Scope           : Public                                                  */
/*                                                                            */
/*  Target          :                                                         */
/*                                                                            */
/*  Coding language : C++                                                     */
/*                                                                            */
/* COPYRIGHT 2011 VAULT-IC France                                             */
/* all rights reserved                                                        */
/*                                                                            */
/******************************************************************************/

/***************************  REVISION HISTORY  *******************************
 * 
 *    Rev 1.0:   06 Sept 2011 CDESBOUCHAGES
 *
 * Recovery of existing code: 
 ******************************************************************************/
#if !defined(AFX_ABOUTDLG_H__5DD51795_5889_44B9_9FBC_C5D8CB5DF940__INCLUDED_)
#define AFX_ABOUTDLG_H__5DD51795_5889_44B9_9FBC_C5D8CB5DF940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// AboutDlg dialog

/******************************************************************************/
/* CLASS DEFINITION															  */
/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	static UINT Run(LPVOID p);
	void Run(void);

// Implementation
protected:
	CWinThread* m_pThread;

	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnCancelMode();
	afx_msg void OnGetDllVersion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDLG_H__5DD51795_5889_44B9_9FBC_C5D8CB5DF940__INCLUDED_)
