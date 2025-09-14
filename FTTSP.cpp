/******************************************************************************/
/*                                                                            */
/*  Layer           : Main CLASS	                                          */
/*                                                                            */
/*  CLASS	        : CFTTSPApp			                                      */
/*  Description     : Main class that launch and execute the program	      */
/*                    FTTSP program                                           */
/*                                                                            */
/*  File            : FTTSP.cpp				                                  */
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
 * - Modification of the error returns of GetServerName
 * - Add specific error message for GetPcName function
 * - Robustness: add error mngt into GetHandlerDllPath
 * - HANDLER_TIMEOUT,PRGM_LOADING_TIMEOUT & DEFAULT_TEST_TIMEOUT defined in .h
 * - Deletion of 'GOTO StartMainWindow' use
 * - In general, output magic number and constant string to put them in the 
 *   header file
 * - Add commentaries  
 *
 *    !!20160412:LBo:Renamed "Alcatraz" to "Phoenix" $LABS-2543
 *
 ******************************************************************************/


/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/
#include "stdafx.h"
#include <io.h>
#include <errno.h>

/******************************************************************************/
/* INCLUSION OF OWNER HEADERS                                                 */
/******************************************************************************/
#include "FTTSP.h"

/******************************************************************************/
/* INCLUSION OF OTHER MODULES HEADERS                                         */
/******************************************************************************/
#include "FTTSPDlg.h"
#include "LoadProgram.h"
#include "FTTSPMainWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/
CString theLastErrorDesc;


/******************************************************************************/
/* PUBLIC FUNCTIONS OUTSIDE THE CLASS                                         */
/******************************************************************************/

/******************************************************************************
 * FUNCTION NAME : SetLastErrorDescription
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Copy a string error message into the local variable theLastErrorDesc
 ------------------------------------------------------------------------------
 * INPUT : theError
    Comment : error message 
    Range   : nor NULL
    Unit    : const CString&
 ------------------------------------------------------------------------------
 * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : 
 *****************************************************************************/
void SetLastErrorDescription(const CString &theError)
{
	TRACE(theError);
	theLastErrorDesc = theError;
}

/******************************************************************************
 * FUNCTION NAME : GetLastErrorDescription
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	return the last string error message from local variable theLastErrorDesc
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : last string error message
    Range   : not NULL
    Unit    : CString
 *****************************************************************************/
CString GetLastErrorDescription(void)
{
	return theLastErrorDesc;
}

/******************************************************************************
 * FUNCTION NAME : GetPCName
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Read the name of the PC. If exists, then return the string value. If not, 
	returns an empty result updating the theLastErrorDesc local variable.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Name of the PC.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString GetPCName(void)
{
	TCHAR PC_Name[MAX_COMPUTERNAME_LENGTH*2 + 1];
	DWORD PC_Name_Len = MAX_COMPUTERNAME_LENGTH*2;

	CString strPCName;

	if (GetComputerName(PC_Name, &PC_Name_Len) == 0)
	{
		theLastErrorDesc.LoadString(IDS_ERROR_NO_PC_NAME);
		return CString();
	}

	PC_Name[MAX_COMPUTERNAME_LENGTH*2] = '\0';
	strPCName = PC_Name;
	return strPCName;
}

/////////////////////////////////////////////////////////////////////////////
// CFTTSPApp

BEGIN_MESSAGE_MAP(CFTTSPApp, CWinApp)
	//{{AFX_MSG_MAP(CFTTSPApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_MESSAGE(WM_DEBUG, OnDebugRequest)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFTTSPApp construction

CFTTSPApp::CFTTSPApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pDebugWindow=NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFTTSPApp object

CFTTSPApp theApp;


/******************************************************************************
 * FUNCTION NAME : GetServerName
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Read back the path of the work, defined into windows environment variables.
	if exists and matches, then return the string value. If not, returns an 
	empty result updating the theLastErrorDesc local variable.
	ex:"\\smart\data\PE"
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : path of the work.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString CFTTSPApp::GetServerName(void)
{
	return m_szServerNamePath;
}

/******************************************************************************
 * FUNCTION NAME : GetSetupPath
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Compute the directory where the *_setup.txt files should be. 
	If not possible,returns an empty result updating the theLastErrorDesc 
	local variable.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Setup files path.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString CFTTSPApp::GetSetupPath(void)
{
	CString strSetupPath;

	strSetupPath = GetServerName();
	// Error occured while fetching Server Name
	if (strSetupPath.IsEmpty())
		return strSetupPath;
	
	strSetupPath += TESTFLOOR_PATH;

	if (!oInfo.bEngiEnv())
		strSetupPath += FINAL_TEST_PATH;
	else 
		strSetupPath += FINAL_TEST_ENGI_PATH;

	strSetupPath += SETUP_PATH;
	return strSetupPath;
}

/******************************************************************************
 * FUNCTION NAME : GetSummariesPath
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Compute the directory where the summaries files should be. 
	If not possible,returns an empty result updating the theLastErrorDesc 
	local variable.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Summaries files path.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString CFTTSPApp::GetSummariesPath(void)
{
	CString strSummariesPath;

	strSummariesPath = GetServerName();
	// Error occured while fetching Server Name
	if (strSummariesPath.IsEmpty())
		return strSummariesPath;

	strSummariesPath += TESTFLOOR_PATH;

	if (!oInfo.bEngiEnv())
		strSummariesPath += FINAL_TEST_PATH;
	else 
		strSummariesPath += FINAL_TEST_ENGI_PATH;

	strSummariesPath += SUMMARIES_PATH;
	return strSummariesPath;
}

/******************************************************************************
 * FUNCTION NAME : GetHandlerDllPath
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Compute the directory where the handlers dll should be. 
	If not possible,returns an empty result updating the theLastErrorDesc 
	local variable.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Handler dll files path.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString CFTTSPApp::GetHandlerDllPath(void)
{
	CString strHandlerDllPath;

	strHandlerDllPath = GetServerName();
	// Error occured while fetching Server Name
	if (strHandlerDllPath.IsEmpty())
		return strHandlerDllPath;

	strHandlerDllPath += TESTFLOOR_PATH;

	if (!oInfo.bEngiEnv())
		strHandlerDllPath += FINAL_TEST_PATH;
	else 
		strHandlerDllPath += FINAL_TEST_ENGI_PATH;

	strHandlerDllPath += DLL_PATH;
	return strHandlerDllPath;
}

/******************************************************************************
 * FUNCTION NAME : GetProgramPath
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Compute the directory where the source of program to test products should be. 
	If not possible,returns an empty result updating the theLastErrorDesc 
	local variable.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Handler dll files path.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString CFTTSPApp::GetProgramPath(void)
{
	CString strProgramPath;

	strProgramPath = GetServerName();
	// Error occured while fetching Server Name
	if (strProgramPath.IsEmpty())
		return strProgramPath;
	
	strProgramPath += TESTFLOOR_PATH;

	if (!oInfo.bEngiEnv())
		strProgramPath += SRC_PROGRAM_PATH;
	else 
		strProgramPath += SRC_PROGRAM_ENGI_PATH;

	return strProgramPath;
}

/******************************************************************************
 * FUNCTION NAME : GetStationProgramPath
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Compute the tempory directory where the program to test products will be 
	unzipped and executed. If not possible,returns an empty result updating 
	the theLastErrorDesc local variable.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Program execution path.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString CFTTSPApp::GetStationProgramPath(void)
{
	CString strProgramPath;

	strProgramPath = GetServerName();
	// Error occured while fetching Server Name
	if (strProgramPath.IsEmpty())
		return strProgramPath;

	strProgramPath += TESTFLOOR_PATH;
	strProgramPath += EXEC_PROGRAM_PATH;

	return strProgramPath;
}

/******************************************************************************
 * FUNCTION NAME : GetConfigPath
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Compute the directory where the source of program to test products should be. 
	If not possible,returns an empty result updating the theLastErrorDesc 
	local variable.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Handler dll files path.
    Range   : All.
    Unit    : CString
 *****************************************************************************/
CString CFTTSPApp::GetConfigPath(void)
{
	return m_szConfigFilePath;
}

/******************************************************************************
 * FUNCTION NAME : SetWindowText
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Return title text for all FTTSP windows
 ------------------------------------------------------------------------------
 *****************************************************************************/
CString CFTTSPApp::SetWindowText(void)
{
	CString szCaption;
	if (oInfo.bEngiEnv())
		szCaption.Format("%s %s%d.%d",ENGI_MODE_MESS,VERSION_MESS,TSP_MAJOR_VERS,TSP_MINOR_VERS);
	else 
		szCaption.Format("%s%d.%d",VERSION_MESS,TSP_MAJOR_VERS,TSP_MINOR_VERS);
	return szCaption;
}

/////////////////////////////////////////////////////////////////////////////
// CFTTSPApp initialization

BOOL CFTTSPApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	/* Parse command line info */
	ParseCommandLine(oInfo);

 	if (m_lpCmdLine[0] == _T('\0') || oInfo.bEngiEnv())
	{
		/* Check if the Config.ini file exist */

		TCHAR szConfigDir[MAX_PATH] = "";
		::GetCurrentDirectory(sizeof(szConfigDir),szConfigDir);
		
		m_szConfigFilePath=szConfigDir;
		m_szConfigFilePath.TrimRight("\\");
		m_szConfigFilePath+="\\";
		m_szConfigFilePath+=CONFIG_FILE;
		int nTmp=_access(m_szConfigFilePath,00);
		if ( nTmp!=0 )
		{
			/* No configuration file found! Exit the program */
			AfxMessageBox("Error!\nThe configuration file is not found!",MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
		/* File Config.ini exists */
		/* Read the server name path */
		TCHAR szServerNamePath[MAX_PATH]="";
		nTmp=GetPrivateProfileString("SERVER_NAME","Path",NULL,szServerNamePath,sizeof(szServerNamePath),m_szConfigFilePath);
		if (nTmp<=0)
		{
			/* SERVER_NAME variable was not found */
			AfxMessageBox("Error!\nSERVER_NAME parameter is not found!",MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
		
		m_szServerNamePath=szServerNamePath;
		m_szServerNamePath.TrimRight("\\");

		/* RTM info */
		GetPrivateProfileString("SUBCONTRACTOR", "RTM", "NO", m_sRTM.GetBuffer(100), 100, m_szConfigFilePath);

		/* Create the DEBUG window */
		OpenTrace();
		m_pDebugWindow->SetDebugStatus(FALSE);
		m_pDebugWindow->Print(GetSetupPath().GetBuffer(100), "Setup path");
		m_pDebugWindow->Print(GetHandlerDllPath().GetBuffer(100), "Handler'dll path");
		m_pDebugWindow->Print(GetProgramPath().GetBuffer(100), "Program path");
		m_pDebugWindow->Print(GetStationProgramPath().GetBuffer(100), "Station path");
		m_pDebugWindow->Print(GetSummariesPath().GetBuffer(100), "Summaries path");

		/* Display the dialog box to select the */
		/* product, handler and the lot number  */
		CFTTSPDlg dlg;			
		
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
			/* copy the configuration selected by the user */
			m_SessionConfig = dlg.m_SessionConfig;
			/* start the test program! */
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel

			// Since the dialog has been closed, return FALSE so that we exit the
			//  application, rather than start the application's message pump.
			return FALSE;
		}
	}
	else
	{
		/* Check if command line parameters are correct? */
		if (!m_SessionConfig.Parse(m_lpCmdLine, ' '))
		{
			/* No! */
			AfxMessageBox(IDS_ERROR_ABNORMAL_TERMINATION, MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
	}

	//-------------------------------------------------------
	//-------------------------------------------------------
	//  INSTALL TESTER TSP COMMUNICATION & START TEST PROGRAM
	//
	//-------------------------------------------------------
	
	//---------------------------------------------------------------------
	//create synchro flags
	hvt_pipe_open	=CreateEvent(NULL,FALSE,FALSE,"pipe_open");
	hvt_start		=CreateEvent(NULL,FALSE,FALSE,"test_start");
	hvt_complete	=CreateEvent(NULL,FALSE,FALSE,"test_complete");
	hvt_data_ready	=CreateEvent(NULL,FALSE,FALSE,"data_ready");
	hnd_stop_handler=CreateEvent(NULL,FALSE,FALSE,"stop_handler");

	if(		hvt_pipe_open		==NULL 
		||	hvt_start			==NULL
		||	hvt_complete		==NULL
		||	hvt_data_ready		==NULL
		||  hnd_stop_handler	==NULL)
	{
	   AfxMessageBox(IDS_ERROR_SYNCHRO_FLAG, MB_OK, 0);
	   return FALSE;
	}

	// Ensure event is nonsignaled
	ResetEvent(hvt_pipe_open);
	ResetEvent(hvt_start);
	ResetEvent(hvt_complete);
	ResetEvent(hvt_data_ready);
	ResetEvent(hnd_stop_handler);

	//---------------------------------------------------------------------
	// create pipe for TSP/Test_prgm communication 
	// the pipe communication is based on the NETBEUI protocol
	if((hpipe=  CreateNamedPipe("\\\\.\\PIPE\\comm_test",								// pointer to pipe name
										PIPE_ACCESS_DUPLEX,								// pipe open mode
										PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,	// pipe-specific modes
										PIPE_UNLIMITED_INSTANCES,						// maximum number of instances
										PIPE_BUFFER_SIZE,								// output buffer size, in bytes
										PIPE_BUFFER_SIZE,								// input buffer size, in bytes
										PIPE_TIMEOUT,									// time-out time, in milliseconds
										NULL)											// pointer to security attributes
										) ==INVALID_HANDLE_VALUE)
	{						
		AfxMessageBox(IDS_ERROR_CREATE_PIPE, MB_OK, 0);
		return FALSE;
	}

	CString sTesterType = m_SessionConfig.GetTesterType();
	if (	(sTesterType == "MAV_PT") ||
			(sTesterType == "MAV_VT") || 
			(sTesterType == "MAV_GT") || 
			(sTesterType == "MAV_HD")) // Load UI with test program
	{
		//--------------------------------------------------------------------
		//start test prgm
		m_pUI = CUI::Create(sTesterType);
		if (m_pUI == NULL)
		{
			AfxMessageBox(IDS_ERROR_TESTER_INSTANCIATION_MAV, MB_ICONEXCLAMATION | MB_OK, 0);
			return FALSE;
		}
	}
	else if (sTesterType == "TX_J800") // Load TestXentral with test program
	{
		//--------------------------------------------------------------------
		//start test prgm
		m_pTX = CTX::Create(sTesterType);
		if (m_pTX == NULL)
		{
			AfxMessageBox(IDS_ERROR_TESTER_INSTANCIATION_TX, MB_ICONEXCLAMATION | MB_OK, 0);
			return FALSE;
		}
	}
	else if (sTesterType == "PC")
	{
		//Need to map server_name to disk v: 
		//the command : "system()" does not run with unc path
		CString Message;
		Message.Format("these configuration require you connect :\"SERVER_NAME\" to disk \"V:\"\nClick Yes when done or No to quit");
		int res=AfxMessageBox(Message, MB_ICONEXCLAMATION | MB_YESNO, 0);
		if(res == IDNO)
			return FALSE;
	}
	//!!20160412:LBo:Add Phoenix as valid tester $LABS-2543
	else if (sTesterType == "PHOENIX")
	{
		CString sHost, sPort, sPath;
		GetPrivateProfileString("PHOENIX", "host", PHOENIX_HOST, sHost.GetBuffer(100), 100, m_szConfigFilePath);
		GetPrivateProfileString("PHOENIX", "port", PHOENIX_PORT, sPort.GetBuffer(100), 100, m_szConfigFilePath);
		GetPrivateProfileString("PHOENIX", "path", PHOENIX_PATH, sPath.GetBuffer(100), 100, m_szConfigFilePath);
		sHost.ReleaseBuffer();
		sPort.ReleaseBuffer();
		sPath.ReleaseBuffer();

		m_pPhoenix = new CPhoenix(sHost, atoi(sPort), sPath);
		if (m_pPhoenix == NULL)
		{
			AfxMessageBox(IDS_ERROR_TESTER_INSTANCIATION_PHOENIX, MB_ICONEXCLAMATION | MB_OK, 0);
			return FALSE;
		}
	}
	else
	{
		CString Message;
		Message.Format("Tester %s not known", sTesterType);
		AfxMessageBox(Message, MB_ICONSTOP | MB_OK, 0);
		return FALSE;
	}

	/* Connect to the DLL handler, Unzip the program test to the		*/
	/* tempory work,load the UI test interface and the log dialog box.	*/
	CLoadProgram dlgLP;
	if (dlgLP.DoModal() != IDOK)
		return FALSE;

	/* Create and start the Main Window: Test! */
	CFTTSPMainWindow dlgMW;

	/* Set Wnd to be the one controlling the Application */
	m_pMainWnd = &dlgMW;
	
	dlgMW.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	KillTrace();
	return FALSE;
}

void CFTTSPApp::DebugTrace (const char *szMessage, const char *szPrefixMessage)
{
	if (m_pDebugWindow->GetDebugStatus())
	{
		m_pDebugWindow->Print(szMessage,szPrefixMessage);
	}
	return;
}

void CFTTSPApp::OpenTrace()
{
	if (m_pDebugWindow!=NULL)
		return;
	/* Load debug window */
	m_pDebugWindow = new CDebug;
	m_pDebugWindow->Create(IDD_DEBUG_DIALOG);
	return;
}

void CFTTSPApp::KillTrace()
{
	if (m_pDebugWindow==NULL)
		return;
	if (m_pDebugWindow->GetDebugStatus())
	{
		m_pDebugWindow->DestroyWindow();
	}
	delete m_pDebugWindow;
	m_pDebugWindow=NULL;
}

int CFTTSPApp::ExitInstance()
{
	KillTrace();

	if (m_pUI)
	{
		delete m_pUI;
		m_pUI = NULL;
	}
	if (m_pPhoenix)
	{
		delete m_pPhoenix;
		m_pPhoenix = NULL;
	}
	if (m_pSubGateway)
	{
		delete m_pSubGateway;
		m_pSubGateway = NULL;
	}
	return 0;
}

LRESULT CFTTSPApp::OnDebugRequest(WPARAM, LPARAM lParam)
{
	if (m_pDebugWindow->GetDebugStatus())
	{
		m_pDebugWindow->ShowWindow(SW_HIDE);
	}
	else
	{
		m_pDebugWindow->ShowWindow(SW_SHOWNORMAL);
	}
	m_pDebugWindow->SetDebugStatus(~m_pDebugWindow->GetDebugStatus());
	return 0;
}
