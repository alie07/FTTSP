#if !defined(AFX_LOADPROGRAM_H__3B63E80A_AB36_4567_800E_0785D3B71402__INCLUDED_)
#define AFX_LOADPROGRAM_H__3B63E80A_AB36_4567_800E_0785D3B71402__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadProgram.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadProgram dialog

class CLoadProgram : public CDialog
{
	DECLARE_DYNAMIC( CLoadProgram )
// Construction
public:
	CLoadProgram(CWnd* pParent = NULL);   // standard constructor

	virtual void AppendInfo(LPCSTR anInfo, ...);
	virtual void AppendInfo(UINT idString, ...);

// Dialog Data
	//{{AFX_DATA(CLoadProgram)
	enum { IDD = IDD_LOADPROGRAM_DIALOG };
	CString	m_sInfo;
	CEdit	m_EditMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadProgram)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	virtual void AppendInfoV(LPCSTR anInfo, va_list arg);
	// Generated message map functions
	//{{AFX_MSG(CLoadProgram)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg BOOL OnStartProgram(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnProgramLoaded(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDLL_Loading(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bDllLoaded;
	BOOL m_bStartProgram;
	BOOL StartUI();
	BOOL StartTX();
	BOOL StartPhoenix();
	UINT m_nGetIDTimer;
	UINT m_nConnectionTimer;
	UINT m_nTestPrgmLoadTimer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADPROGRAM_H__3B63E80A_AB36_4567_800E_0785D3B71402__INCLUDED_)
