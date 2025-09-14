#if !defined(AFX_FTTSPMAINWINDOW_H__F6D755C6_6D36_42A3_B953_D9363EB3EB4F__INCLUDED_)
#define AFX_FTTSPMAINWINDOW_H__F6D755C6_6D36_42A3_B953_D9363EB3EB4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FTTSPMainWindow.h : header file
//

#include <afxtempl.h>
#include "SocketTest.h"

/////////////////////////////////////////////////////////////////////////////
// CFTTSPMainWindow dialog

class CHandler;

class CFTTSPMainWindow : public CDialog
{
	DECLARE_DYNAMIC( CFTTSPMainWindow )
// Construction
public:
	CFTTSPMainWindow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFTTSPMainWindow)
	enum { IDD = IDD_FTTSP_MAINWINDOW };
	CListCtrl	m_ListSockets;
	//}}AFX_DATA

	enum {
		LS_COLSKT = 0,
		LS_COLYIELD,
		LS_COLTOTAL,
		LS_COLPASS,
		LS_COLFAIL,
		LS_COLBIN,
		LS_COLHARDBIN
	};

	CImageList* m_pImageListSmall;
	CImageList* m_pImageListBig;

	BOOL m_bProbeIsRunning;

	CTime m_tSessionTime;
	CTime m_tTestTime;
	CTimeSpan m_tsLastTestDuration;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFTTSPMainWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	DWORD m_dwLastSN;

	CArray<CSocketTest, CSocketTest&> m_aSocketTests;
	virtual void UpdateListSockets(void);


// Implementation
protected:
	HICON m_hIcon;
	CFont m_font;
	int  m_nSessionNumber;
	CHandler * m_pHandler;

	virtual void OnFirstSession();

	// Generated message map functions
	//{{AFX_MSG(CFTTSPMainWindow)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnStopHandler();
	afx_msg void OnEngiMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnProductRev();
	//}}AFX_MSG
	afx_msg LRESULT OnHndReady(WPARAM, LPARAM);
	afx_msg LRESULT OnHndTestCompleted(WPARAM, LPARAM);
	afx_msg LRESULT OnTestCompleted(WPARAM, LPARAM);
	afx_msg LRESULT OnHndFinished(WPARAM, LPARAM);
	afx_msg LRESULT OnHndRunPerso(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTTSPMAINWINDOW_H__F6D755C6_6D36_42A3_B953_D9363EB3EB4F__INCLUDED_)
