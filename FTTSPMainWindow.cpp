// FTTSPMainWindow.cpp : implementation file
//

#include "stdafx.h"
#include "FTTSP.h"
#include "FTTSPMainWindow.h"
#include "AboutDlg.h"
#include "DialogPassword.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CFTTSPMainWindow, CDialog);

BEGIN_MESSAGE_MAP(CFTTSPMainWindow, CDialog)
	//{{AFX_MSG_MAP(CFTTSPMainWindow)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_STOP_HANDLER, OnStopHandler)
	ON_BN_CLICKED(IDC_ENGI_MODE, OnEngiMode)
	ON_WM_TIMER()

	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HNDREADY, OnHndReady)
	ON_MESSAGE(WM_HNDTESTCOMPLETED, OnHndTestCompleted)
	ON_MESSAGE(WM_TESTCOMPLETED, OnTestCompleted)
	ON_MESSAGE(WM_HNDFINISHED, OnHndFinished)
	ON_MESSAGE(WM_RUNPERSO, OnHndRunPerso)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFTTSPMainWindow dialog


CFTTSPMainWindow::CFTTSPMainWindow(CWnd* pParent /*=NULL*/)
	: CDialog(CFTTSPMainWindow::IDD, pParent),
	m_nSessionNumber(0),
	m_dwLastSN(0),
	m_bProbeIsRunning(FALSE)
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();
	//{{AFX_DATA_INIT(CFTTSPMainWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MEMO);
	m_pHandler = &(ptrApp->m_Handler);
}


void CFTTSPMainWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFTTSPMainWindow)
	DDX_Control(pDX, IDC_SKTS, m_ListSockets);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFTTSPMainWindow message handlers

void CFTTSPMainWindow::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

BOOL CFTTSPMainWindow::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();
	
	/* Modify the caption to add the version number */
	SetWindowText(ptrApp->SetWindowText());

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize the Program Name
	CEdit* pEdit;
	CString temp, szTmp;
	
	pEdit = (CEdit*)GetDlgItem(IDC_PRODUCT);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strProduct);

	pEdit = (CEdit*)GetDlgItem(IDC_PRODUCT_REV);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strRevDesign);

	pEdit = (CEdit*)GetDlgItem(IDC_PART_ID);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strPartID);

	pEdit = (CEdit*)GetDlgItem(IDC_FLOW);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strFlow);

	pEdit = (CEdit*)GetDlgItem(IDC_TEMPERATURE);
	ASSERT_VALID(pEdit);
	temp.Format("%.0fC", ptrApp->m_SessionConfig.dTemp);
	pEdit->SetWindowText(temp);

	pEdit = (CEdit*)GetDlgItem(IDC_TESTER);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(GetPCName());

	pEdit = (CEdit*)GetDlgItem(IDC_HANDLER);
	ASSERT_VALID(pEdit);
	szTmp = ptrApp->m_SessionConfig.strHandler;
	szTmp += " - " + ptrApp->m_SessionConfig.strHandlerId;
	pEdit->SetWindowText(szTmp);

	pEdit = (CEdit*)GetDlgItem(IDC_OP_NAME);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strOperator);

	pEdit = (CEdit*)GetDlgItem(IDC_LOADBOARD);
	ASSERT_VALID(pEdit);
	szTmp=ptrApp->m_SessionConfig.strLoadBoard;
	szTmp+=" - " + ptrApp->m_SessionConfig.strLoadBoardNumber;
	pEdit->SetWindowText(szTmp);

	pEdit = (CEdit*)GetDlgItem(IDC_CABLE);
	ASSERT_VALID(pEdit);
	szTmp=ptrApp->m_SessionConfig.strCable;
	szTmp+=" - " + ptrApp->m_SessionConfig.strCableNumber;
	pEdit->SetWindowText(szTmp);

	pEdit = (CEdit*)GetDlgItem(IDC_LOT_NUMBER);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strLotNumber);

	pEdit = (CEdit*)GetDlgItem(IDC_SUB_LOT);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strSubLotNumber);
	pEdit->ShowWindow(TRUE);

	pEdit = (CEdit*)GetDlgItem(IDC_SUB_LOT_NUM);
	ASSERT_VALID(pEdit);

	pEdit = (CEdit*)GetDlgItem(IDC_PROGRAMNAME);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(ptrApp->m_SessionConfig.strProgram);

	// Create, initialize, and hook up image lists
	m_pImageListSmall = new CImageList();
	ASSERT(m_pImageListSmall != NULL); // serious allocation failure checking
	m_pImageListSmall->Create(16, 16, TRUE, 4, 4);
	m_pImageListSmall->Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_pImageListSmall->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	m_pImageListSmall->Add(AfxGetApp()->LoadIcon(IDI_ICON_UNSUSED));
	m_ListSockets.SetImageList(m_pImageListSmall, LVSIL_SMALL);

	m_pImageListBig = new CImageList();
	ASSERT(m_pImageListBig != NULL); // serious allocation failure checking
	m_pImageListBig->Create(32, 32, TRUE, 4, 4);
	m_pImageListSmall->Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_pImageListSmall->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	m_pImageListSmall->Add(AfxGetApp()->LoadIcon(IDI_ICON_UNSUSED));
	m_ListSockets.SetImageList(m_pImageListBig, LVSIL_NORMAL);

	m_ListSockets.InsertColumn(LS_COLSKT	, "Skt"		, LVCFMT_LEFT , 100);
	m_ListSockets.InsertColumn(LS_COLYIELD	, "Yield"	, LVCFMT_RIGHT,  50);
	m_ListSockets.InsertColumn(LS_COLTOTAL	, "Total"	, LVCFMT_RIGHT,  50);
	m_ListSockets.InsertColumn(LS_COLPASS	, "Pass"	, LVCFMT_RIGHT,  50);
	m_ListSockets.InsertColumn(LS_COLFAIL	, "Fail"	, LVCFMT_RIGHT,  50);
	m_ListSockets.InsertColumn(LS_COLBIN	, "Bin"		, LVCFMT_RIGHT,  60);
	m_ListSockets.InsertColumn(LS_COLHARDBIN, "Hard Bin", LVCFMT_RIGHT,  80);
	CSessionConfiguration* pSC = &(ptrApp->m_SessionConfig);

	CWordArray* pwSkts = &(pSC->waSkts);
	int nSktCount = pwSkts->GetSize();
	m_aSocketTests.SetSize(nSktCount);
	CString sSkt2Dut;
	int nSkt = 0;
	for (nSkt = 0; nSkt < nSktCount; ++nSkt)
	{
		sSkt2Dut.Format("%d => %d", nSkt, pwSkts->GetAt(nSkt));
		m_ListSockets.InsertItem(nSkt, sSkt2Dut, -1);
		m_ListSockets.SetItemText(nSkt, LS_COLYIELD		, "");
		m_ListSockets.SetItemText(nSkt, LS_COLTOTAL		, "0");
		m_ListSockets.SetItemText(nSkt, LS_COLPASS		, "0");
		m_ListSockets.SetItemText(nSkt, LS_COLFAIL		, "0");
		m_ListSockets.SetItemText(nSkt, LS_COLBIN		, "0");
		m_ListSockets.SetItemText(nSkt, LS_COLHARDBIN	, "0");
		m_aSocketTests.ElementAt(nSkt).Empty();
	}

	//!!08072021:LBs:Add last line for global yield
	sSkt2Dut.Format("Global");
	m_ListSockets.InsertItem(nSkt, sSkt2Dut, -1);
	m_ListSockets.SetItemText(nSkt, LS_COLYIELD		, "");
	m_ListSockets.SetItemText(nSkt, LS_COLTOTAL		, "0");
	m_ListSockets.SetItemText(nSkt, LS_COLPASS		, "0");
	m_ListSockets.SetItemText(nSkt, LS_COLFAIL		, "0");
	m_ListSockets.SetItemText(nSkt, LS_COLBIN		, "");
	m_ListSockets.SetItemText(nSkt, LS_COLHARDBIN	, "");


	CButton * pButton = (CButton*) GetDlgItem(IDC_ENGI_MODE);
	ASSERT_VALID(pButton);
	pSC->bEngiMode=FALSE;
	pButton->SetCheck(pSC->bEngiMode);

	pButton->EnableWindow(ptrApp->m_EngiModeAllowed);

	// Blink message : Please start FTTSP before handler! 
	SetTimer(ID_CLOCK_TIMER2,250,NULL); //Start timer to chect handler timeout 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFTTSPMainWindow::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFTTSPMainWindow::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CFTTSPMainWindow::OnDestroy() 
{
	delete m_pImageListSmall;
	delete m_pImageListBig;

	CDialog::OnDestroy();	
}


void CFTTSPMainWindow::OnOK() 
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();

//	int ret = SetTimer(ID_CLOCK_TIMER,10000,NULL);

	KillTimer(ID_CLOCK_TIMER2);
	CStatic * pStatic = (CStatic*)GetDlgItem(IDC_TSP_INFO);
	ASSERT_VALID(pStatic);
	pStatic->ShowWindow(FALSE);
 	
	// Create log files
	if (!ptrApp->m_SessionSummary.Open(ptrApp->m_SessionConfig, m_nSessionNumber))
	{
		AfxMessageBox("Cannot create Session Summary", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	
	if (! ptrApp->m_SessionMapCods.Open(ptrApp->m_SessionConfig, m_nSessionNumber))
	{
		AfxMessageBox("Cannot create MapCods dump", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP_HANDLER)->EnableWindow(TRUE);
	GetDlgItem(IDC_ENGI_MODE)->EnableWindow(FALSE);

	m_bProbeIsRunning = TRUE;

	m_pHandler->m_hWnd = AfxGetMainWnd()->m_hWnd;
	m_pHandler->m_hTestStart = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pHandler->m_hTestEnd  = ptrApp->hvt_complete;
	m_pHandler->m_hTestDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pHandler->m_pBins = new DWORD[ptrApp->m_SessionConfig.waSkts.GetSize()];

	CString sTesterType = ptrApp->m_SessionConfig.GetTesterType();
	if (m_nSessionNumber++ == 0 && (sTesterType.Left(3) == "MAV" ||  sTesterType.Left(2) == "TX"))
	{
		OnFirstSession();
	}
	m_tSessionTime = CTime::GetCurrentTime();
	// Start the handler
	m_pHandler->SetOnLine();
}

void CFTTSPMainWindow::OnFirstSession()
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();

	DWORD nbBytesWritten;

	CString DataToSend;
	CString CAPConfig, OneDut;

	CAPConfig.Format("N%d", ptrApp->m_dwSiteMax);
	for(DWORD dut=0; dut < ptrApp->m_dwSiteMax; dut++)
	{
		OneDut.Format("X%dY%d", (int) dut/256, dut % 256);
		CAPConfig += OneDut;
	}

	CString strEE=ptrApp->m_SessionConfig.strProdCode.Right(2);
	CString strFlash=ptrApp->m_SessionConfig.strProdCode.Right(4);
	strFlash=strFlash.Left(2);
	CString strRom=ptrApp->m_SessionConfig.strProdCode.Left(2-(6-ptrApp->m_SessionConfig.strProdCode.GetLength()));

	CString strSubLotNum= ptrApp->m_SessionConfig.strSubLotNumber;
	if ( !strSubLotNum.IsEmpty() )
	{
		strSubLotNum.Insert(0,"SUB_LOT_NUM_");
	}

	DataToSend.Format("%s %s %s %s %s %s %s %s %s PART_ID_%s %s",
		GetPCName(),										// Station name
		ptrApp->m_SessionConfig.GetProduct(),				// Product
		ptrApp->m_SessionConfig.GetRevDesign(),				// Revision
		strEE,												// EE
		ptrApp->m_SessionConfig.strFlow,					// Probe
		ptrApp->m_SessionConfig.strLotNumber,				// Lot
		CAPConfig,											// CAP Config
		strRom,												// Rom
		strFlash,											// Flash
		ptrApp->m_SessionConfig.GetPartID(),				// Part ID
		strSubLotNum);										// Sub-contractor lot number

	DataToSend += " PROBEPARAM_Y"; // FTTSP Handles Probe parameters
	DataToSend += " TSTPROGPARAM_Y"; // Add in Configuration a field for probe params
	DataToSend += " BINDESC_Y";	// This TSP handles Bin descriptions
	// FabName ?
	// TabNumber??

	WriteFile(	ptrApp->hpipe,				// Handle file to write
				DataToSend,					// Pointer to data to write
				DataToSend.GetLength()+1,	// Number of bytes to Write
				&nbBytesWritten,			// pNumber of bytes written
				NULL);						// pOverlapped (not used)

	// Tells UI there are some data
	ptrApp->DebugTrace(DataToSend,"FTTSP");

	SetEvent(ptrApp->hvt_start);

	// Wait for UI to reply
	if(ptrApp->m_SessionConfig.GetTesterType() != "PC")
		WaitForSingleObject(ptrApp->hvt_complete, INFINITE);
}

void CFTTSPMainWindow::OnStopHandler() 
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();

	// Set event at true to indicate to DLL "stop handler" button was pushed
	SetEvent(ptrApp->hnd_stop_handler);
	
	GetDlgItem(IDC_STOP_HANDLER)->EnableWindow(FALSE);
	m_pHandler->SetOnLine(FALSE);

	KillTimer(ID_CLOCK_TIMER);

}

LRESULT CFTTSPMainWindow::OnHndReady(WPARAM, LPARAM lParam)
{
	DWORD dwSitesReady = (DWORD) lParam;
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();
	// Should indicate to UI to perform its tests. For now, just tells worker thread to continue
	// Should also signal if it is the last test.
//	TRACE("Worker Thread signal '%08x' sites are ready\n", dwSitesReady);

	m_tTestTime = CTime::GetCurrentTime();

	DWORD nbBytesWritten;

	CString DataToSend;

	if (m_dwLastSN == 0)
	{
		// Simulate a Wafer Start
		DataToSend = "STARTWAFER 1";

		WriteFile(	ptrApp->hpipe,				// Handle file to write
					DataToSend,					// Pointer to data to write
					DataToSend.GetLength()+1,	// Number of bytes to Write
					&nbBytesWritten,			// pNumber of bytes written
					NULL);						// pOverlapped (not used)

		ptrApp->DebugTrace(DataToSend,"FTTSP");

		// Tells UI there are some data
		SetEvent(ptrApp->hvt_start);

		// Wait for UI to reply
		WaitForSingleObject(ptrApp->hvt_complete, INFINITE);
	}

	WORD dut;
	__int64 mask1 = 0, mask2 = 0;
	for (int skt=0; skt< ptrApp->m_SessionConfig.waSkts.GetSize(); skt++)
	{
		m_aSocketTests.ElementAt(skt).Empty();
		if ((dwSitesReady & ((DWORD)1 << skt)) != 0)
		{
			m_aSocketTests.ElementAt(skt).m_bEnabled = TRUE;
			dut = ptrApp->m_SessionConfig.waSkts.GetAt(skt);
			if (dut < 64)
				mask1 |= (__int64) 1 << dut;
			else if (dut < 128)
				mask2 |= (__int64) 1 << (dut - 64);
			m_aSocketTests.ElementAt(skt).m_dwSerialNumber = m_dwLastSN++;
		}
	}

	if (ptrApp->m_dwSiteMax <= 64)
		DataToSend.Format("%I64dTX0Y0", mask1);
	else if (ptrApp->m_dwSiteMax <= 128)
		DataToSend.Format("%I64d,%I64dTX0Y0", mask2, mask1);

	WriteFile(	ptrApp->hpipe,				// Handle file to write
				DataToSend,					// Pointer to data to write
				DataToSend.GetLength()+1,	// Number of bytes to Write
				&nbBytesWritten,			// pNumber of bytes written
				NULL);						// pOverlapped (not used)

	ptrApp->DebugTrace(DataToSend,"FTTSP");
	
	// Tells UI there are some data
	SetEvent(ptrApp->hvt_start);

	// Signal Worker thread to really start test
	SetEvent(m_pHandler->m_hTestStart);
	
	return 0;
}

LRESULT CFTTSPMainWindow::OnTestCompleted(WPARAM, LPARAM)
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();

	CString Buffer;
	{
		DWORD nbByteRead;
		LPTSTR pBuffer = Buffer.GetBuffer(4096);

		ReadFile(	ptrApp->hpipe,			// handle of file to read
					pBuffer,				// pointer to buffer that receives data
					4095,					// number of bytes to read
					&nbByteRead,			// pointer to number of bytes read
					NULL					// pointer to structure for data
					);
		pBuffer[nbByteRead] = '\0'; // Ensure read data properly finished
		FlushFileBuffers(ptrApp->hpipe);
		Buffer.ReleaseBuffer();
		ptrApp->DebugTrace(pBuffer,"UI");
	}

	DWORD* pDutBins		= new DWORD [ptrApp->m_dwSiteMax];
	DWORD* pDutBinsHard	= new DWORD [ptrApp->m_dwSiteMax];
	DWORD* pDutBinsSoft	= new DWORD [ptrApp->m_dwSiteMax];
	CStringArray saMapCodes;
	saMapCodes.SetSize(ptrApp->m_dwSiteMax);
	DWORD* pBin = pDutBins;
	DWORD* pBinHard = pDutBinsHard;
	DWORD* pBinSoft = pDutBinsSoft;

	int pos;
	for(DWORD dut = 0; dut < ptrApp->m_dwSiteMax; dut++)
	{
		if (sscanf(Buffer, "%d|%d.%d", pBinHard++, pBin++, pBinSoft++) != 3)
		{// New format not handled, only Bin received, set the others to '0'
			pDutBins[dut] = pDutBinsHard[dut];
			pDutBinsHard[dut] = 0;
			pDutBinsSoft[dut] = 0;
		}
		pos = Buffer.FindOneOf(", \t");
		if (pos == -1)
			Buffer.Empty();
		else
			Buffer = Buffer.Mid(pos + 1);
	}
	// Fills whole buffer if returned data too short
	for( ;dut < ptrApp->m_dwSiteMax; dut++)
	{
		*(pBin++) = 0;
		*(pBinHard++) = 0;
		*(pBinSoft++) = 0;
	}


	pBin = m_pHandler->m_pBins;

	// Tells UI that TSP is waiting for some MapCod data
	SetEvent(ptrApp->hvt_start);
	do
	{
		WaitForSingleObject(ptrApp->hvt_data_ready, INFINITE);

		DWORD nbByteRead;
		LPTSTR pBuffer = Buffer.GetBuffer(4096);

		ReadFile(	ptrApp->hpipe,			// handle of file to read
					pBuffer,				// pointer to buffer that receives data
					4095,					// number of bytes to read
					&nbByteRead,			// pointer to number of bytes read
					NULL					// pointer to structure for data
					);
		pBuffer[nbByteRead] = '\0'; // Ensure read data properly finished
		FlushFileBuffers(ptrApp->hpipe);
		Buffer.ReleaseBuffer();
		ptrApp->DebugTrace(pBuffer,"UI");
		//TRACE("UI sends back: '%s'\n", Buffer);

		// Is it a MapCode or something else
		if (sscanf(Buffer, "%d ", &dut) != 0)
		{
			// Copy String
			saMapCodes[dut] = Buffer.Mid(Buffer.FindOneOf(" \t\n"));
			saMapCodes[dut].TrimLeft();
		}
		else if (Buffer.Find("BINDESC") != -1)
		{
			ptrApp->m_SessionSummary.AddBinDescription(Buffer);
			ptrApp->m_SessionMapCods.AddBinDescription(Buffer);
		}
		else if (Buffer.Find("PROBEPARAM") != -1)
		{
			ptrApp->m_SessionSummary.AddProbeParam(Buffer);
			ptrApp->m_SessionMapCods.AddProbeParam(Buffer);
		}
		SetEvent(ptrApp->hvt_complete);
	}
	while (Buffer.Find("ENDMAPXFER") == -1);

	BOOL bTesterError = FALSE;
	int skt_count = ptrApp->m_SessionConfig.waSkts.GetSize();
	for(int skt = 0; skt < skt_count ; skt++)
	{
		dut = ptrApp->m_SessionConfig.waSkts.GetAt(skt);
		CSocketTest &aSocketTest = m_aSocketTests.ElementAt(skt);

		if (aSocketTest.m_bEnabled)
		{
					aSocketTest.m_nBin = pDutBins[dut];
		pBin[skt] = aSocketTest.m_nBinHard = pDutBinsHard[dut];
					aSocketTest.m_nBinSoft = pDutBinsSoft[dut];
					aSocketTest.m_sMapCodes = saMapCodes[dut];
			if (pBin[skt] == 0)
				bTesterError = TRUE;
		}
		else
		{
					aSocketTest.m_nBin = 0;
		pBin[skt] = aSocketTest.m_nBinHard = 0;
					aSocketTest.m_nBinSoft = 0;
					aSocketTest.m_sMapCodes.Empty();
		}
	}

	if (bTesterError)
	{
		OnStopHandler();
		AfxMessageBox(IDS_ERROR_INVALIDBIN, MB_ICONEXCLAMATION | MB_OK);
	}
	ptrApp->m_SessionSummary.AddTest(m_aSocketTests);
	if (!ptrApp->m_SessionMapCods.AddTest(m_aSocketTests))
	{
		OnStopHandler();
		AfxMessageBox("Session MapCods cannnot be written\nHandler is stopped, please restart test when connection is well established.", MB_ICONEXCLAMATION | MB_OK);
	}

	delete [] pDutBins;
	delete [] pDutBinsHard;
	delete [] pDutBinsSoft;

	UpdateListSockets();

	SetEvent(m_pHandler->m_hTestDone);
	return 0;
}

LRESULT CFTTSPMainWindow::OnHndTestCompleted(WPARAM, LPARAM)
{
//	TRACE("Worker Thread signals Binning done\n");

	CTimeSpan tsDuration;
	m_tsLastTestDuration = CTime::GetCurrentTime() - m_tTestTime;
	m_tTestTime = CTime::GetCurrentTime();
	tsDuration = CTime::GetCurrentTime() - m_tSessionTime;
	GetDlgItem(IDC_LASTTESTDURATION)->SetWindowText(m_tsLastTestDuration.Format("%M:%S"));
	GetDlgItem(IDC_SESSIONDURATION)->SetWindowText(tsDuration.Format("%D %H:%M:%S"));
	return 0;
}

LRESULT CFTTSPMainWindow::OnHndFinished(WPARAM, LPARAM lParam)
{
	CHandler::Status HandlerStatus = (CHandler::Status) lParam;
	
	// Is it a normal stop request or a failure?
	if (HandlerStatus != CHandler::NORMAL_EXIT)
	{
		AfxMessageBox(IDS_ERROR_UI, MB_ICONEXCLAMATION | MB_OK);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		SetTimer(ID_CLOCK_TIMER2,250,NULL);
	}

	m_pHandler->m_hWnd = NULL;
	CloseHandle(m_pHandler->m_hTestStart);
	CloseHandle(m_pHandler->m_hTestDone);
	m_pHandler->m_hTestStart =NULL;
	m_pHandler->m_hTestEnd  = NULL;
	m_pHandler->m_hTestDone = NULL;
	delete [] m_pHandler->m_pBins;
	m_pHandler->m_pBins = NULL;

	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp();

	GetDlgItem(IDC_STOP_HANDLER)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	if (ptrApp->m_EngiModeAllowed)
		GetDlgItem(IDC_ENGI_MODE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_ENGI_MODE)->EnableWindow(FALSE);
	m_bProbeIsRunning = FALSE;
	
	ptrApp->m_SessionSummary.Close();
	if(ptrApp->m_SessionConfig.strTesterType != "PC")
		ptrApp->m_SessionMapCods.Close();
	
	return 0;
}

void CFTTSPMainWindow::OnCancel() 
{
	if (m_bProbeIsRunning)
	{
		AfxMessageBox("Cannot Exit while Handler is still running");
	}
	else if (IDYES == AfxMessageBox("Are you sure you want to quit?", MB_YESNO))
	{
		if (m_pHandler->m_pBins)
			delete [] m_pHandler->m_pBins;
		CDialog::OnCancel();
	}
}


void CFTTSPMainWindow::OnEngiMode() 
{
	// Handler should not be on line to change its state
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();
	ASSERT(ptrApp->m_EngiModeAllowed);

	if (ptrApp->m_SessionConfig.strEngiModePassword != ENGIMODE_PASSWORD)
	{
		CDialogPassword dlg;
		dlg.m_Password = ptrApp->m_SessionConfig.strEngiModePassword;
 		if (dlg.DoModal() == IDOK)
			ptrApp->m_SessionConfig.strEngiModePassword = dlg.m_Password;
	}

	CButton * pButton = (CButton*)GetDlgItem(IDC_ENGI_MODE);
	ASSERT_VALID(pButton);

	if (ptrApp->m_SessionConfig.strEngiModePassword != ENGIMODE_PASSWORD)
		pButton->SetCheck(0);
	else // Toggles Button State
		pButton->SetCheck((pButton->GetCheck() == 0)?1:0);

	DWORD nbBytesWritten;
	CString DataToSend;

	DataToSend.Format("ENGIMODE%d", pButton->GetCheck());

	WriteFile(	ptrApp->hpipe,				// Handle file to write
				DataToSend,					// Pointer to data to write
				DataToSend.GetLength()+1,	// Number of bytes to Write
				&nbBytesWritten,			// pNumber of bytes written
				NULL);						// pOverlapped (not used)

	ptrApp->DebugTrace(DataToSend,"FTTSP");

	// Tells UI there are some data
	SetEvent(ptrApp->hvt_start);
}

void CFTTSPMainWindow::UpdateListSockets()
{
	int nSktCount = m_aSocketTests.GetSize();

	LVITEM anItem;
	/* Important!!! Must be added because LVITEM is a Windows structure !!! */
	/* In RELEASE configuration, the structure is not initialized			*/
	/* Added due to display problems on the m_ListSockets control.			*/
	ZeroMemory(&anItem, sizeof(anItem));

	CString temp;

	//!!08072021:LBs:Add counter for global yield, pass & fail
	int nSkt=0;
	int global_tested = 0, global_pass = 0, global_fail = 0;

	for (nSkt=0; nSkt < nSktCount; nSkt++)
	{
		CSocketTest& aSocketTest = m_aSocketTests.ElementAt(nSkt);

		anItem.iItem = nSkt;
		anItem.mask = LVIF_IMAGE;
		m_ListSockets.GetItem(&anItem);

		if (aSocketTest.m_bEnabled)
		{
			/* Update UI with bin and hard bin */
			temp.Format("%d", aSocketTest.m_nBin);
			m_ListSockets.SetItemText(nSkt, LS_COLBIN, temp);
			temp.Format("%d", aSocketTest.m_nBinHard);
			m_ListSockets.SetItemText(nSkt, LS_COLHARDBIN, temp);
			
			temp = m_ListSockets.GetItemText(nSkt,LS_COLTOTAL); // Retrieve number of dice tested
			temp.Format("%d", atoi(temp) + 1);
			m_ListSockets.SetItemText(nSkt, LS_COLTOTAL, temp);// Inc

			if (1 <= aSocketTest.m_nBin && aSocketTest.m_nBin <= 6)
			{
				anItem.iImage = 0; // Set Pass Icon
				temp = m_ListSockets.GetItemText(nSkt,LS_COLPASS); // Retrieve number of dice Pass
				temp.Format("%d", atoi(temp) + 1);
				m_ListSockets.SetItemText(nSkt, LS_COLPASS, temp);// Inc
			}
			else
			{
				anItem.iImage = 1; // Set Fail Icon
				temp = m_ListSockets.GetItemText(nSkt,LS_COLFAIL); // Retrieve number of dice Fail
				temp.Format("%d", atoi(temp) + 1);
				m_ListSockets.SetItemText(nSkt, LS_COLFAIL, temp);// Inc
			}

			temp.Format("%.2f", (double) ( 100 *
				atof(m_ListSockets.GetItemText(nSkt, LS_COLPASS)) / 
				atof(m_ListSockets.GetItemText(nSkt, LS_COLTOTAL))
				));
			m_ListSockets.SetItemText(nSkt, LS_COLYIELD, temp);
		}
		else
		{
			anItem.iImage = 2; // Socket unused
		}

		temp = m_ListSockets.GetItemText(nSkt,LS_COLTOTAL); // Retrieve number of dice tested to add to global
		global_tested += atoi(temp);
		temp = m_ListSockets.GetItemText(nSkt,LS_COLPASS); // Retrieve number of dice Pass to add to global
		global_pass += atoi(temp);
		temp = m_ListSockets.GetItemText(nSkt,LS_COLFAIL); // Retrieve number of dice Fail to add to global
		global_fail += atoi(temp);

		m_ListSockets.SetItem(&anItem);

	}
	//!!08072021:LBs:Add last line for global yield

	anItem.iItem = nSkt;
	anItem.mask = LVIF_IMAGE;
	m_ListSockets.GetItem(&anItem);

	//Update Global total of tested dies
	temp.Format("%d", global_tested);
	m_ListSockets.SetItemText(nSkt, LS_COLTOTAL, temp);// Inc

	temp.Format("%d", global_pass);
	m_ListSockets.SetItemText(nSkt, LS_COLPASS, temp);// Inc

	temp.Format("%d", global_fail);
	m_ListSockets.SetItemText(nSkt, LS_COLFAIL, temp);// Inc

	temp.Format("%.2f", (double) ( 100 * (double)global_pass/(double)global_tested));
	m_ListSockets.SetItemText(nSkt, LS_COLYIELD, temp);
}


LRESULT CFTTSPMainWindow::OnHndRunPerso(WPARAM, LPARAM)
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();

	CString aSourceProgram = ptrApp->m_SessionConfig.GetProgramPerso();

	CString CurrentDirectory = aSourceProgram;

	//Remove *.bat from path
	CurrentDirectory = CurrentDirectory.Left(CurrentDirectory.ReverseFind('\\'));
	SetCurrentDirectory(CurrentDirectory);

	//Script from appli creates bin.txt => results of their program
	CFile BinFile;
	TRY
	{
		BinFile.Remove("bin.txt");
	}
	CATCH( CFileException, e )
	{
		//here bin.txt does not exist
	}
	END_CATCH

	DWORD* pBinHard=NULL;

	DWORD res = system(aSourceProgram); //load perso and create bin.txt

	char pbuf[10];
	/* --- Added by CDE 25/01/12: To ensure that the .bat program created the bin file --- */
	if ( !BinFile.Open("bin.txt",CFile::modeRead) )
	{
		/* It seems that the bin file is not created correctly. */
		OnStopHandler();
		AfxMessageBox(IDS_ERROR_APPLI, MB_ICONEXCLAMATION | MB_OK);
		SetEvent(m_pHandler->m_hTestDone);
		return 0;
	}
	TRY
	{
		BinFile.Read(pbuf,10);
	}
	CATCH( CFileException , e )
	{
		/* It seems that the bin file is not created correctly. */
		OnStopHandler();
		AfxMessageBox(IDS_ERROR_APPLI, MB_ICONEXCLAMATION | MB_OK);
		SetEvent(m_pHandler->m_hTestDone);
		return 0;
	}
	END_CATCH

	DWORD tmp = atol(pbuf);
	pBinHard = &tmp;

	BinFile.Close();

	//Ensure bin.txt is deleted
	TRY
	{
		BinFile.Remove("bin.txt");
	}
	CATCH( CFileException, e )
	{
		//here bin.txt does not exist
	}
	END_CATCH

	
	BOOL bTesterError = FALSE;
	int skt_count = ptrApp->m_SessionConfig.waSkts.GetSize();
	DWORD dut;

	for(int skt = 0; skt < skt_count ; skt++)
	{
		dut = ptrApp->m_SessionConfig.waSkts.GetAt(skt);
		CSocketTest &aSocketTest = m_aSocketTests.ElementAt(skt);

		aSocketTest.m_bEnabled = TRUE; //just to update the main window with icon pass fail bin ....
		aSocketTest.m_sMapCodes.Empty();
		aSocketTest.m_nBinSoft = 0;
		
		aSocketTest.m_nBin = pBinHard[dut];
		aSocketTest.m_nBinHard = pBinHard[dut];

		if(aSocketTest.m_nBinHard == 0) //system return ==> ERROR
			bTesterError = TRUE;
	}

	if (bTesterError)
	{
		OnStopHandler();
		AfxMessageBox(IDS_ERROR_INVALIDBIN, MB_ICONEXCLAMATION | MB_OK);
	}
	ptrApp->m_SessionSummary.AddTest(m_aSocketTests);
//	ptrApp->m_SessionMapCods.AddTest(m_aSocketTests);

	UpdateListSockets();

	SetEvent(m_pHandler->m_hTestDone);
	return 0;
}	


void CFTTSPMainWindow::OnTimer(UINT nIDEvent) 
{
	static int iLoop=1;

	switch (nIDEvent)
	{
		case ID_CLOCK_TIMER:
		{
			// TODO: Add your message handler code here and/or call default
			CString Message;
			Message.Format("! Handler Timeout ! => No device to test\nPlease put device in input and start again");
			AfxMessageBox(Message, MB_ICONEXCLAMATION | MB_OK);
			
			CFTTSPMainWindow::OnStopHandler();
		}
		break;

		case ID_CLOCK_TIMER2:
		{
			CStatic *pStatic;
			BOOL bFlash;
			LOGFONT lf;
			CFont *pFont;

			pStatic = (CStatic*)GetDlgItem(IDC_TSP_INFO);
			ASSERT_VALID(pStatic);
			
			pFont=pStatic->GetFont();
			pFont->GetLogFont(&lf);
			lf.lfWeight=FW_BOLD;
			m_font.DeleteObject();				// destruction GDI de l'ancien objet eventuellement
			m_font.CreateFontIndirect(&lf);		// creation de la fonte d'apres la LOGFONT
			pStatic->SetFont(&m_font,TRUE);
			
			pStatic->SetWindowText("Please, press the button before starting the handler! ==>");				
			
			if (iLoop==1)
			{
				bFlash=TRUE;
				SetTimer(ID_CLOCK_TIMER2,250,NULL); 
			}
			else if (iLoop>=5)
			{
				bFlash=TRUE;
				iLoop=0;
				SetTimer(ID_CLOCK_TIMER2,1500,NULL); 
			}
			else
			{
				bFlash=pStatic->IsWindowVisible()^TRUE;
			}

			pStatic->ShowWindow(bFlash);
			iLoop++;
		}
		break;

		default:
		{
			// Wrong ID: Kill it! 
			KillTimer(nIDEvent);
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

