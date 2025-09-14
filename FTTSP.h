/******************************************************************************/
/*                                                                            */
/*  Layer           : main header	                                          */
/*                                                                            */
/*  CLASS	        : CFTTSPApp			                                      */
/*  Description     : Main header file for the FTTSP application		      */
/*																			  */
/*                                                                            */
/*  File            : FTTSP.h				                                  */
/*                                                                            */
/*  Scope           : Public                                                  */
/*                                                                            */
/*  Target          :                                                         */
/*                                                                            */
/*  Coding language : C++                                                     */
/*                                                                            */
/* COPYRIGHT 2016 SSQ France                                               */
/* all rights reserved                                                        */
/*                                                                            */
/******************************************************************************/

/***************************  REVISION HISTORY  *******************************
 * 
 *    Rev 1.0:   06 Sept 2011 CDESBOUCHAGES
 *    
 * Recovery of existing code: 
 * - Add version constants 
 * - Add constants from .cpp code
 * - HANDLER_TIMEOUT,PRGM_LOADING_TIMEOUT & DEFAULT_TEST_TIMEOUT defined instead of attributes
 * - Add commentaries  
 *
 *    !!20160412:LBo:Renamed "Alcatraz" to "Phoenix" $LABS-2543
 *
 ******************************************************************************/

#if !defined(AFX_FTTSP_H__FE7CFC12_E6B6_4368_9CC1_0BE839011E38__INCLUDED_)
#define AFX_FTTSP_H__FE7CFC12_E6B6_4368_9CC1_0BE839011E38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/******************************************************************************/
/* INCLUSION OF OTHER MODULES HEADERS                                         */
/******************************************************************************/
#include "resource.h"				// main symbols
#include "SessionConfiguration.h"	// Added by ClassView
#include "UI.h"						// Added by ClassView
#include "Handler.h"				// Added by ClassView
#include "Phoenix.h"				// Added June 2014 //!!20160412:LBo:Renamed to "Phoenix"
#include "TX.h"
#include "SessionSummary.h"
#include "SessionMapCods.h"
#include "Utils.h"
#include "Debug.h"
#include "SetupGateway.h"
#include "FTTSPCmdLineInfo.h"
#include "FTTSPVersion.h"

/******************************************************************************/
/* PUBLIC DEFINED CONSTANTS				                                      */
/******************************************************************************/

#define VERSION_MESS				"Final Test TSP v"
//!!20160419:LBo:TSP versions constants moved to FTTSPVersion.h $LABS-2538
#define ENGI_MODE_MESS				"*** ENGINEERING ENVIRONMENT SETUP ***"

#define TESTFLOOR_PATH				"\\TestFloor"
#define FINAL_TEST_PATH				"\\Final_Test"
#define FINAL_TEST_ENGI_PATH		"\\Final_Test_Engi"

#define SRC_PROGRAM_PATH			"\\Program"
#define SRC_PROGRAM_ENGI_PATH		"\\Program_Engi"
#define SUMMARIES_PATH				"\\Summaries"
#define SETUP_PATH					"\\Setup"
#define DLL_PATH					"\\Handlers"
#define EXEC_PROGRAM_PATH			"\\Station\\CurrentPrgm"


#define CONFIG_FILE					"Config.ini"
#define ENGIMODE_PASSWORD			"dur"
#define CKS_BYPASS_VALUE			0x12345678

#define WM_START_PROGRAM			(WM_USER + 1)
#define WM_PROGRAM_LOADED			(WM_USER + 2)

#define WM_HNDREADY					(WM_USER + 3)
#define WM_HNDTESTCOMPLETED			(WM_USER + 4)
#define WM_HNDFINISHED				(WM_USER + 5)

#define WM_TESTCOMPLETED			(WM_USER + 6)

#define WM_DLL_LOADING				(WM_USER + 7)

#define WM_RUNPERSO					(WM_USER + 8)

#define WM_DEBUG					(WM_USER + 9)

#define HANDLER_TIMEOUT				((DWORD)300000)					// unit is ms ==> 5mn
#define TESTER_TIMEOUT				((DWORD)300000)
#define PIPE_TIMEOUT				((DWORD)10000)
#define PRGM_LOADING_TIMEOUT		((DWORD)240000)
#define DEFAULT_TEST_TIMEOUT		((DWORD)240000)					// 4mn
#define PIPE_BUFFER_SIZE			((DWORD)4096)

#define PHOENIX_HOST				"127.0.0.1"
#define PHOENIX_PORT				"333"
#define PHOENIX_PATH				"C:\\Program Files\\testeur\\Phoenix.exe"

//#define _DEBUG_HANDLER_DLL			TRUE

/******************************************************************************/
/* PUBLIC FUNCTIONS						                                      */
/******************************************************************************/

void SetLastErrorDescription(const CString &theError);
CString GetLastErrorDescription(void);
CString GetPCName(void);

 
/******************************************************************************/
/* CLASS DEFINITION															  */
/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CFTTSPApp:
// See FTTSP.cpp for the implementation of this class
//

class CFTTSPApp : public CWinApp
{
public:
	DWORD m_dwSiteMax;
	CFTTSPApp();
	CString GetServerName(void);
	CString GetSetupPath(void);
	CString GetHandlerDllPath(void);
	CString GetSummariesPath(void);
	CString GetProgramPath(void);
	CString GetStationProgramPath(void);
	CString GetConfigPath(void);

	CString SetWindowText(void);

	CFTTSPCmdLineInfo oInfo;

	CSessionConfiguration m_SessionConfig;

	HANDLE hpipe;
	HANDLE hvt_pipe_open;
	HANDLE hvt_start;
	HANDLE hvt_complete;
	HANDLE hvt_data_ready;
	HANDLE hnd_stop_handler;
	HWND m_hWnd;

	CSetupGateway *m_pSubGateway;

	ULONG TstPrgmTimeOut;
	BOOL TimeOutFromPrgm;
	BOOL m_EngiModeAllowed;

	CString m_szServerNamePath;
	CString m_szConfigFilePath;
	CUI* m_pUI;
	CTX* m_pTX;
	CPhoenix* m_pPhoenix;
	CHandler m_Handler;
	CDebug *m_pDebugWindow;
	LRESULT OnDebugRequest(WPARAM, LPARAM lParam);
	CString m_sRTM;

	void DebugTrace (const char *szMessage, const char *szPrefixMessage);
	void OpenTrace (void);
	void KillTrace (void);

	CSessionSummary m_SessionSummary;
	CSessionMapCods m_SessionMapCods;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFTTSPApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFTTSPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTTSP_H__FE7CFC12_E6B6_4368_9CC1_0BE839011E38__INCLUDED_)
