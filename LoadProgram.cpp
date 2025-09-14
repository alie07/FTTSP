// LoadProgram.cpp : implementation file
//



/*  CHANGES
 * - Use of PRGM_LOADING_TIMEOUT and DEFAULT_TEST_TIMEOUT (define instead of ATTRIBUTE) 
 * !!20160412:LBo:Renamed "Alcatraz" to "Phoenix" $LABS-2543
 */ 


#include "stdafx.h"
#include "FTTSP.h"
#include "LoadProgram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CLoadProgram, CDialog);

/////////////////////////////////////////////////////////////////////////////
// CLoadProgram dialog


CLoadProgram::CLoadProgram(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadProgram::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadProgram)

	//}}AFX_DATA_INIT
	
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MEMO);
}


void CLoadProgram::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadProgram)
	DDX_Text(pDX, IDC_INFO, m_sInfo);
	DDX_Control(pDX, IDC_MESSAGE, m_EditMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLoadProgram, CDialog)
	//{{AFX_MSG_MAP(CLoadProgram)
	ON_WM_CANCELMODE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_START_PROGRAM, OnStartProgram )
	ON_MESSAGE( WM_PROGRAM_LOADED, OnProgramLoaded )
	ON_MESSAGE( WM_DLL_LOADING, OnDLL_Loading )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadProgram message handlers
BOOL CLoadProgram::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog. The framework does this
	// automatically when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); 	// Set big icon
	SetIcon(m_hIcon, FALSE);	// Set small icon

	/* Force painting of the Window or else it will wait until Idle before show up */
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp();
	CString aMessage;

	aMessage.Format(
		"Please,\r\n => Use loadBoard: '%s - %s'\r\n"
		" => Use cable: '%s - %s'\r\n"
		" => Set Temperature to %0.0fC\r\n",
		ptrApp->m_SessionConfig.strLoadBoard,
		ptrApp->m_SessionConfig.strLoadBoardNumber,
		ptrApp->m_SessionConfig.strCable,
		ptrApp->m_SessionConfig.strCableNumber,
		ptrApp->m_SessionConfig.dTemp);

	m_EditMessage.SetWindowText(aMessage);

	// 1/ Load tester program
	if (ptrApp->m_SessionConfig.strTesterType != "PC")
	{
		SendMessage(WM_DLL_LOADING);	// Load Handler DLL and Init it
		if (!m_bDllLoaded)
			return FALSE;
		SendMessage(WM_START_PROGRAM);	// Load UI with test program
		if (m_bStartProgram)
			PostMessage(WM_PROGRAM_LOADED);
	}
	
	// 2/ Application mode for personalization
	else if (ptrApp->m_SessionConfig.strTesterType == "PC") // pplication mode for personalization with PC
	{
		SendMessage(WM_DLL_LOADING);	//Load Handler DLL and Init it
		if (!m_bDllLoaded)
			return FALSE;
		PostMessage(WM_PROGRAM_LOADED);
	}
	// Default
	else
	{
		AfxMessageBox(IDS_ERROR_TESTER_INSTANCIATION, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

afx_msg BOOL CLoadProgram::OnDLL_Loading(WPARAM wParam, LPARAM lParam)
{
	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp();
	CString sInfo;

	m_bDllLoaded = FALSE;

	//PostMessage(WM_PAINT);
	UpdateWindow();
	// Loading Dll
	AppendInfo(IDS_LOADING_DLL, ptrApp->m_SessionConfig.strHandlerDll);
	//BringWindowToTop();
	UpdateWindow();
	if (! ptrApp->m_Handler.LoadDll(ptrApp->m_SessionConfig.strHandlerDll))
	{
		sInfo = GetLastErrorDescription();
		AppendInfo(IDS_FAIL);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);

	/* Trace the DLL version */
	CString strVersion;
	ptrApp->m_Handler.GetDllVersion(strVersion);
	AppendInfo(IDS_DLL_VERSION,strVersion);
	// Initialize the handler
	AppendInfo(IDS_HND_INIT);
	//BringWindowToTop();
	UpdateWindow();
	/* Modified the 19/11/2012 by CDE */
	//ptrApp->m_Handler.m_dwSockets = ptrApp->m_SessionConfig.waSkts.GetSize();
	ptrApp->m_Handler.m_dwSockets = ptrApp->m_SessionConfig.iTestMask;

	ptrApp->m_Handler.m_bRTM = ptrApp->m_SessionConfig.bRTM;  // UTAC RTM
	
	// Declaration of HndInit is not correct, should accept const char * i/o char *
	if (! ptrApp->m_Handler.Init(NULL))
	{
		AppendInfo(IDS_FAIL);
		sInfo = GetLastErrorDescription();
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);
	m_bDllLoaded = TRUE;
	return TRUE;
}

// Message receiver
afx_msg BOOL CLoadProgram::OnStartProgram(WPARAM wParam, LPARAM lParam)
{
	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp();

	// Switch accroding to tester type
	CString sTesterType = ptrApp->m_SessionConfig.GetTesterType();

	if (sTesterType.Left(3) == "MAV") // NEXTEXT
	{
		if (!this->StartUI())
			m_bStartProgram = FALSE;
	}
	else if (sTesterType.Left(2) == "TX") // TestXentral
	{
		if (!this->StartTX())
			m_bStartProgram = FALSE;
	}
	else if (sTesterType == "PHOENIX") // PHOENIX
	{
		if (!this->StartPhoenix())
			m_bStartProgram = FALSE;
	}
	else // UNKNOWN!
	{
		m_sInfo.Format("Tester %s not implemented!\n", sTesterType);
		SetLastErrorDescription(m_sInfo);
		AppendInfo(m_sInfo);
		AfxMessageBox(m_sInfo, MB_ICONEXCLAMATION);
	}

	return 0;
}

// Start UI procedure
BOOL CLoadProgram::StartUI()
{
	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp();

	/* Install Program */
	CString aSourceProgram = ptrApp->m_SessionConfig.strProgramPath;
	AppendInfo(IDS_LOADING_PRGM, aSourceProgram);
	UpdateWindow();
	if (! ptrApp->m_pUI->InstallProgram(aSourceProgram))
	{
		CString sInfo;
		sInfo = GetLastErrorDescription();
		AppendInfo(IDS_FAIL);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);

	/* Start UI */
	ptrApp->m_pUI->SetControllers(ptrApp->m_SessionConfig.GetControllers());
	AppendInfo(IDS_START_UI);
	UpdateWindow();
	if (! ptrApp->m_pUI->Start(ptrApp->m_SessionConfig.bEngiMode))
	{
		CString sInfo;
		sInfo = GetLastErrorDescription();
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);

	/* synchronisation (wait until the pipe is open in the test prgm) */
	AppendInfo(IDS_WAIT_PRGM_LOAD);
	UpdateWindow();
	if(WaitForSingleObject(ptrApp->hvt_pipe_open,PRGM_LOADING_TIMEOUT)!=WAIT_OBJECT_0)
	{
		CString sInfo;
		sInfo.LoadString(IDS_ERROR_TIMEOUT_OPEN_PIPE);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	AppendInfo(IDS_PASS);

	/* synchronisation (wait until the data nbBoard,nb_site & dutpersite are ready) */
	AppendInfo(IDS_WAIT_PRGM_INFO);
	UpdateWindow();
	if(WaitForSingleObject(ptrApp->hvt_data_ready,10000)==WAIT_TIMEOUT)
	{
		CString sInfo;
		sInfo.LoadString(IDS_ERROR_COMMUNICATION);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	/* to see how many sites are active & Dut Per Site number */
	/*  Read these datas from the tester through the pipe */
	DWORD nbByteRead;
	CString DataFromTst;
	TCHAR* Buffer;

	Buffer = DataFromTst.GetBuffer(4096);
	ReadFile(	ptrApp->hpipe,			// handle of file to read
				Buffer,					// pointer to buffer that receives data
				4095,					// number of bytes to read
				&nbByteRead,			// pointer to number of bytes read
				NULL					// pointer to structure for data
				);
	Buffer[nbByteRead] = '\0'; // Ensure read data properly finished
	DataFromTst.ReleaseBuffer();
	ptrApp->DebugTrace(Buffer,"UI");
	FlushFileBuffers(ptrApp->hpipe);

	int nBoards;
	int nSite;
	int nDutPerSiteMax;
	TCHAR sFlagMapCod[20];
	int nEngiModeAllowed;
	
	/* extract values */
	sscanf(DataFromTst,"%d %d %d %20s %d",&nBoards,&nSite,&nDutPerSiteMax,sFlagMapCod,&nEngiModeAllowed);
	if (DataFromTst.Find("TIMEOUT_") != -1) // Prgm is sending timeout to use
	{
		CString Tmp = DataFromTst.Mid(DataFromTst.Find("TIMEOUT_") + 8);

		sscanf(Tmp, "%d", &(ptrApp->m_Handler.m_dwTestTimeOut));
		ptrApp->m_Handler.m_dwTestTimeOut *= 60000; // Receive in mn and convert in ms

		ptrApp->TimeOutFromPrgm = TRUE;
	}
	else
	{
		ptrApp->m_Handler.m_dwTestTimeOut  = DEFAULT_TEST_TIMEOUT;
		ptrApp->TimeOutFromPrgm = FALSE;
	}
	ptrApp->m_EngiModeAllowed = nEngiModeAllowed;

	ptrApp->m_dwSiteMax = nDutPerSiteMax * ptrApp->m_SessionConfig.GetControllers() / nSite;
	/* flush the pipe (if I don't flush it, it is full after about 20 Dice) */
	FlushFileBuffers(ptrApp->hpipe);

	SetEvent(ptrApp->hvt_complete);
	AppendInfo(IDS_PASS);
	UpdateWindow();

	if (strcmp(sFlagMapCod, "TRUE")==0)
	{
		//!!27052020:LBs:Increased character limitation (254->1024 for Name, 4->256 for units)
		char MapCodNum[5];
		char MapCodName[1025];
		char MapCodUnit[257];

		AppendInfo(IDS_FETCHING_MAPCODES);
		UpdateWindow();

		Buffer = DataFromTst.GetBuffer(PIPE_BUFFER_SIZE);
		
		BOOL bEndOfRead=FALSE;
		/* Force painting of the Window or else it will wait until Idle before show up */
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		do 
		{
			/* Read MapCodDefinition  */
			/* synchronization (wait until the data are ready) */
			if (WaitForSingleObject(ptrApp->hvt_data_ready, PIPE_TIMEOUT) == WAIT_TIMEOUT)
			{
				DataFromTst.ReleaseBuffer();
				CString sInfo;
				sInfo.LoadString(IDS_ERROR_COMMUNICATION);
				AppendInfo(sInfo);
				AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
				return FALSE;
			}

			ReadFile(	ptrApp->hpipe,			// handle of file to read
						Buffer,					// pointer to buffer that receives data
						4095,					// number of bytes to read
						&nbByteRead,			// pointer to number of bytes read
						NULL					// pointer to structure for data
						);
			Buffer[nbByteRead] = '\0'; // Ensure read data properly finished
			FlushFileBuffers(ptrApp->hpipe);
			SetEvent(ptrApp->hvt_complete);
			ptrApp->DebugTrace(Buffer,"UI");

			if (strstr(Buffer, "EndMapCod") != NULL) // End of mapcode fetch
			{
				// Modification the 22/11/2012 by CDE
				// Force production mode
				DWORD nbBytesWritten;
				CString DataToSend;

				DataToSend="ENGIMODE0";

				WriteFile(	ptrApp->hpipe,				// Handle file to write
							DataToSend,					// Pointer to data to write
							DataToSend.GetLength()+1,	// Number of bytes to Write
							&nbBytesWritten,			// pNumber of bytes written
							NULL);						// pOverlapped (not used)

				ptrApp->DebugTrace(DataToSend,"FTTSP");

				// Tells UI there are some data
				SetEvent(ptrApp->hvt_start);
				// End modification
				bEndOfRead=TRUE;
			}
			else
			{
				/* extract values */
				//!!26052020:LBs:Increased character limitation (254->1024 for Name, 4->256 for units)
				sscanf(Buffer, "%4s %1024s %256s ", MapCodNum, MapCodName, MapCodUnit);
				AppendInfo(IDS_MAPCODE_FETCHED, MapCodNum, MapCodName, MapCodUnit);
				UpdateWindow();
				ptrApp->m_SessionSummary.AddMapCod(MapCodNum, MapCodName, MapCodUnit);
				ptrApp->m_SessionMapCods.AddMapCod(MapCodNum, MapCodName, MapCodUnit);
				bEndOfRead=FALSE;
			}
		}while(bEndOfRead==FALSE); // Loops indefinitely until EndMapCod flag detected

		DataFromTst.ReleaseBuffer();
	}

	AppendInfo(IDS_PRGM_LOADED);
	UpdateWindow();
	return TRUE;
}


//!!14032023:LBs:Added Start TX procedure copied from startTX
BOOL CLoadProgram::StartTX()
{
	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp();

	/* Install Program */
	CString aSourceProgram = ptrApp->m_SessionConfig.strProgramPath;
	AppendInfo(IDS_LOADING_PRGM, aSourceProgram);
	UpdateWindow();
	if (! ptrApp->m_pTX->InstallProgram(aSourceProgram))
	{
		CString sInfo;
		sInfo = GetLastErrorDescription();
		AppendInfo(IDS_FAIL);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	AppendInfo(IDS_PASS);

	/* Start TX */
	ptrApp->m_pTX->SetControllers(ptrApp->m_SessionConfig.GetControllers());
	AppendInfo(IDS_START_TX);
	UpdateWindow();
	if (! ptrApp->m_pTX->Start(ptrApp->m_SessionConfig.bEngiMode))
	{
		CString sInfo;
		sInfo = GetLastErrorDescription();
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);

	/* synchronisation (wait until the pipe is open in the test prgm) */
	AppendInfo(IDS_WAIT_PRGM_LOAD_TX);
	UpdateWindow();
	if(WaitForSingleObject(ptrApp->hvt_pipe_open,PRGM_LOADING_TIMEOUT)!=WAIT_OBJECT_0)
	{
		CString sInfo;
		sInfo.LoadString(IDS_ERROR_TIMEOUT_OPEN_PIPE);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	AppendInfo(IDS_PASS);

	/* synchronisation (wait until the data nbBoard,nb_site & dutpersite are ready) */
	AppendInfo(IDS_WAIT_PRGM_INFO);
	UpdateWindow();
	if(WaitForSingleObject(ptrApp->hvt_data_ready,10000)==WAIT_TIMEOUT)
	{
		CString sInfo;
		sInfo.LoadString(IDS_ERROR_COMMUNICATION);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	/* to see how many sites are active & Dut Per Site number */
	/*  Read these datas from the tester through the pipe */
	DWORD nbByteRead;
	CString DataFromTst;
	TCHAR* Buffer;

	Buffer = DataFromTst.GetBuffer(4096);
	ReadFile(	ptrApp->hpipe,			// handle of file to read
				Buffer,					// pointer to buffer that receives data
				4095,					// number of bytes to read
				&nbByteRead,			// pointer to number of bytes read
				NULL					// pointer to structure for data
				);
	Buffer[nbByteRead] = '\0'; // Ensure read data properly finished
	DataFromTst.ReleaseBuffer();
	ptrApp->DebugTrace(Buffer,"TX");
	FlushFileBuffers(ptrApp->hpipe);

	int nBoards;
	int nSite;
	int nDutPerSiteMax;
	TCHAR sFlagMapCod[20];
	int nEngiModeAllowed;
	
	/* extract values */
	sscanf(DataFromTst,"%d %d %d %20s %d",&nBoards,&nSite,&nDutPerSiteMax,sFlagMapCod,&nEngiModeAllowed);
	if (DataFromTst.Find("TIMEOUT_") != -1) // Prgm is sending timeout to use
	{
		CString Tmp = DataFromTst.Mid(DataFromTst.Find("TIMEOUT_") + 8);

		sscanf(Tmp, "%d", &(ptrApp->m_Handler.m_dwTestTimeOut));
		ptrApp->m_Handler.m_dwTestTimeOut *= 60000; // Receive in mn and convert in ms

		ptrApp->TimeOutFromPrgm = TRUE;
	}
	else
	{
		ptrApp->m_Handler.m_dwTestTimeOut  = DEFAULT_TEST_TIMEOUT;
		ptrApp->TimeOutFromPrgm = FALSE;
	}
	ptrApp->m_EngiModeAllowed = nEngiModeAllowed;

	ptrApp->m_dwSiteMax = nDutPerSiteMax * ptrApp->m_SessionConfig.GetControllers() / nSite;
	/* flush the pipe (if I don't flush it, it is full after about 20 Dice) */
	FlushFileBuffers(ptrApp->hpipe);

	SetEvent(ptrApp->hvt_complete);
	AppendInfo(IDS_PASS);
	UpdateWindow();

	if (strcmp(sFlagMapCod, "TRUE")==0)
	{
		//!!27052020:LBs:Increased character limitation (254->1024 for Name, 4->256 for units)
		char MapCodNum[5];
		char MapCodName[1025];
		char MapCodUnit[257];

		AppendInfo(IDS_FETCHING_MAPCODES);
		UpdateWindow();

		Buffer = DataFromTst.GetBuffer(PIPE_BUFFER_SIZE);
		
		BOOL bEndOfRead=FALSE;
		/* Force painting of the Window or else it will wait until Idle before show up */
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		do 
		{
			/* Read MapCodDefinition  */
			/* synchronization (wait until the data are ready) */
			if (WaitForSingleObject(ptrApp->hvt_data_ready, PIPE_TIMEOUT) == WAIT_TIMEOUT)
			{
				DataFromTst.ReleaseBuffer();
				CString sInfo;
				sInfo.LoadString(IDS_ERROR_COMMUNICATION);
				AppendInfo(sInfo);
				AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
				return FALSE;
			}

			ReadFile(	ptrApp->hpipe,			// handle of file to read
						Buffer,					// pointer to buffer that receives data
						4095,					// number of bytes to read
						&nbByteRead,			// pointer to number of bytes read
						NULL					// pointer to structure for data
						);
			Buffer[nbByteRead] = '\0'; // Ensure read data properly finished
			FlushFileBuffers(ptrApp->hpipe);
			SetEvent(ptrApp->hvt_complete);
			ptrApp->DebugTrace(Buffer,"TX");

			if (strstr(Buffer, "EndMapCod") != NULL) // End of mapcode fetch
			{
				// Force production mode
				DWORD nbBytesWritten;
				CString DataToSend;

				DataToSend="ENGIMODE0";

				WriteFile(	ptrApp->hpipe,				// Handle file to write
							DataToSend,					// Pointer to data to write
							DataToSend.GetLength()+1,	// Number of bytes to Write
							&nbBytesWritten,			// pNumber of bytes written
							NULL);						// pOverlapped (not used)

				ptrApp->DebugTrace(DataToSend,"FTTSP");

				// Tells TX there are some data
				SetEvent(ptrApp->hvt_start);
				// End modification
				bEndOfRead=TRUE;
			}
			else
			{
				/* extract values */
				//!!26052020:LBs:Increased character limitation (254->1024 for Name, 4->256 for units)
				sscanf(Buffer, "%4s %1024s %256s ", MapCodNum, MapCodName, MapCodUnit);
				AppendInfo(IDS_MAPCODE_FETCHED, MapCodNum, MapCodName, MapCodUnit);
				UpdateWindow();
				ptrApp->m_SessionSummary.AddMapCod(MapCodNum, MapCodName, MapCodUnit);
				ptrApp->m_SessionMapCods.AddMapCod(MapCodNum, MapCodName, MapCodUnit);
				bEndOfRead=FALSE;
			}
		}while(bEndOfRead==FALSE); // Loops indefinitely until EndMapCod flag detected

		DataFromTst.ReleaseBuffer();
	}

	AppendInfo(IDS_PRGM_LOADED);
	UpdateWindow();
	return TRUE;
}

// Start Phoenix procedure
BOOL CLoadProgram::StartPhoenix()
{
	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp();
	CString sInfo;

	// Fix variables
	ptrApp->m_EngiModeAllowed = FALSE; // Fix engi mode feature to false, not available on Phoenix.
	ptrApp->m_Handler.m_dwTestTimeOut = DEFAULT_TEST_TIMEOUT;
	ptrApp->m_dwSiteMax = ptrApp->m_pPhoenix->GetSiteMax();

	/* Install Program */
	CString aSourceProgram = ptrApp->m_SessionConfig.strProgramPath;
	AppendInfo(IDS_INSTALL_PRGM, aSourceProgram);
	UpdateWindow();
	if (!ptrApp->m_pPhoenix->InstallProgram(aSourceProgram))
	{
		sInfo = GetLastErrorDescription();
		AppendInfo(IDS_FAIL);
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);

	//  1/ Check if application started
	AppendInfo(IDS_START_PHOENIX);
	UpdateWindow();
	if (!ptrApp->m_pPhoenix->Start(ptrApp->m_SessionConfig.bEngiMode))
	{
		sInfo = GetLastErrorDescription();
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);

	//  2/ Set Connection
	AppendInfo(IDS_PHOENIX_CONNECT);
	UpdateWindow();
	if (!ptrApp->m_pPhoenix->Connection())
	{
		sInfo = GetLastErrorDescription();
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);
	
	//  3/ GetID
	AppendInfo(IDS_PHOENIX_ID);
	UpdateWindow();
	CString sId;
	if (!ptrApp->m_pPhoenix->GetID(sId))
	{
		sInfo = GetLastErrorDescription();
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(sId);
	AppendInfo("...");
	AppendInfo(IDS_PASS);
	
	//  4/ Load test program
	AppendInfo(IDS_LOADING_PRGM, ptrApp->m_pPhoenix->GetInstalledProgram());
	UpdateWindow();
	if (!ptrApp->m_pPhoenix->LoadProgram(aSourceProgram))
	{
		sInfo = GetLastErrorDescription();
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}
	AppendInfo(IDS_PASS);

	//  5/ Get params (mapcod)
	AppendInfo(IDS_FETCHING_MAPCODES);
	UpdateWindow();
	CStringArray ParamsMap;
	if (!ptrApp->m_pPhoenix->GetParams(ParamsMap))
	{
		sInfo = GetLastErrorDescription();
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	// Process ParamsMap
	CString MapCodNum, MapCodName, MapCodUnit, sValue;
	int keyCount = ParamsMap.GetSize();
	for (int i=0; i<keyCount; i++)
	{
		sValue = ParamsMap.GetAt(i);
		AfxExtractSubString(MapCodName, (LPCTSTR)(LPCSTR)sValue, 0, ':');
		AfxExtractSubString(MapCodUnit, (LPCTSTR)(LPCSTR)sValue, 1, ':');
		//!!20160413:LBo:Added MapCodNum index number $LABS-2544
		MapCodNum.Format("%d", i+1);

		TRACE("Num: %d, Name: %s, Unit: %s\n", i, MapCodName, MapCodUnit);
		
		AppendInfo(IDS_MAPCODE_FETCHED, MapCodNum, MapCodName, MapCodUnit);
		UpdateWindow();
		ptrApp->m_SessionSummary.AddMapCod(MapCodNum, MapCodName, MapCodUnit);
		ptrApp->m_SessionMapCods.AddMapCod(MapCodNum, MapCodName, MapCodUnit);
	}
	
	// Activate Thread Loop
	if (!ptrApp->m_pPhoenix->SetRunThread())
	{
		sInfo.Format("Can't start Phoenix thread!\n");
		AppendInfo(sInfo);
		AfxMessageBox(sInfo, MB_ICONEXCLAMATION | MB_OK, 0);
		return FALSE;
	}

	AppendInfo("All done, click OK to continue...\r\n");
	UpdateWindow();
	
	return TRUE;
}

afx_msg BOOL CLoadProgram::OnProgramLoaded(WPARAM wParam, LPARAM lParam)
{
	// Program has finished Loading
	// Activate OK Button
	CButton* pButtonOK;
	pButtonOK = (CButton*) this->GetDlgItem(IDOK);
	pButtonOK->EnableWindow(TRUE);

	//BringWindowToTop();
	return TRUE;
}

void CLoadProgram::AppendInfo(LPCSTR anInfo, ...)
{
	va_list marker;
	va_start(marker, anInfo);
	AppendInfoV(anInfo, marker);
	va_end(marker);
}

void CLoadProgram::AppendInfo(UINT idString, ...)
{
	CString sAnInfoFormat;

	va_list marker;
	va_start(marker, idString);
	sAnInfoFormat.LoadString(idString);
	AppendInfoV(sAnInfoFormat, marker);
	va_end(marker);
}

void CLoadProgram::AppendInfoV(LPCSTR anInfo, va_list arg)
{
	CString sInfo;
	int len;

	sInfo.FormatV(anInfo, arg);

	m_sInfo += sInfo;
	UpdateData(FALSE);
	CEdit* info = (CEdit*)GetDlgItem(IDC_INFO);
	info->SetFocus();
	len = m_sInfo.GetLength();
	info->SetSel(len, len);
}

void CLoadProgram::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CLoadProgram::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

// Phoenix timer
void CLoadProgram::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == m_nConnectionTimer) // Phoenix connection time out
	{
		KillTimer(nIDEvent);

	}
	

	if (nIDEvent == m_nTestPrgmLoadTimer) // Test program loading time out
	{
		KillTimer(nIDEvent);


	}
	
	CDialog::OnTimer(nIDEvent);
}

