// ManualHandler.h : main header file for the MANUALHANDLER DLL
//

#if !defined(AFX_MANUALHANDLER_H__DAA1B567_4E8F_422C_B4F3_66CA8BA43419__INCLUDED_)
#define AFX_MANUALHANDLER_H__DAA1B567_4E8F_422C_B4F3_66CA8BA43419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "PollMaskDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CManualHandlerApp
// See ManualHandler.cpp for the implementation of this class
//

class CManualHandlerApp : public CWinApp
{
public:
	DWORD dwMask;
	BOOL bLoop_active;
	DWORD dwRun_nb;
	CManualHandlerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManualHandlerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CManualHandlerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUALHANDLER_H__DAA1B567_4E8F_422C_B4F3_66CA8BA43419__INCLUDED_)
