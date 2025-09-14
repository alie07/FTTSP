// FTTSPDlg.cpp : implementation file
// !!20160412:LBo:Renamed "Alcatraz" to "Phoenix" $LABS-2543
//

#include "stdafx.h"
#include "FTTSP.h"
#include "FTTSPDlg.h"
#include "AboutDlg.h"
#include "DialogPassword.h"
#include "UI.h"
#include "TX.h"
#include "Phoenix.h"
#include "SetupGateway.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ENGI_OPT_FRAME_H 40

static LPCSTR strSetupFilter = "_setup.txt";

/////////////////////////////////////////////////////////////////////////////
// CFTTSPDlg dialog

CFTTSPDlg::CFTTSPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFTTSPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFTTSPDlg)
	m_szMode = _T("");
	m_szTesterType = _T("");
	m_bEngiOptCheck = FALSE;
	m_EngiTestPrgm = _T("");
	m_bRTM = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MEMO);
	m_pToolTip = NULL;
}

CFTTSPDlg::~CFTTSPDlg()
{
	ClearConfigurations();
	delete m_pToolTip;
}

void CFTTSPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFTTSPDlg)
	DDX_Control(pDX, IDC_RTM, m_cRTM);
	DDX_Control(pDX, IDC_ENGI_FRAME, m_EngiFrame);
	DDX_Control(pDX, IDC_MODE, m_Mode);
	DDX_Control(pDX, IDCANCEL, m_ButtonCancel);
	DDX_Control(pDX, IDC_SUB_LOT_NUMBER, m_EditSubLotNumber);
	DDX_Control(pDX, IDC_OP_NAME, m_szOperatorName);
	DDX_Control(pDX, IDC_LB_NUMBER, m_szLoadBoardNumber);
	DDX_Control(pDX, IDC_CABLE_NUMBER, m_szCableNumber);
	DDX_Control(pDX, IDC_CABLE_REF, m_ComboCable);
	DDX_Control(pDX, IDC_LB_REF, m_ComboLB);
	DDX_Control(pDX, IDC_PRODUCT_REV, m_ComboRev);
	DDX_Control(pDX, IDC_HANDLER_DLL, m_ComboHandlerDll);
	DDX_Control(pDX, IDC_SOCKETS, m_ComboSockets);
	DDX_Control(pDX, IDC_CONTROLLERS, m_ComboControllers);
	DDX_Control(pDX, IDC_DEGRADED_MODE, m_ButtonDegraded);
	DDX_Control(pDX, IDC_HANDLER_ID, m_szHandlerId);
	DDX_Control(pDX, IDC_LOT_NUMBER, m_EditLotNumber);
	DDX_Control(pDX, IDC_HANDLER, m_ComboHandler);
	DDX_Control(pDX, IDC_FLOW, m_ComboFlow);
	DDX_Control(pDX, IDC_PART_ID, m_ComboPartID);
	DDX_Control(pDX, IDOK, m_ButtonStartProbe);
	DDX_Control(pDX, IDC_PRODUCT, m_ComboProduct);
	DDX_Text(pDX, IDC_MODE, m_szMode);
	DDX_Text(pDX, IDC_TESTER_TYPE, m_szTesterType);
	DDX_Check(pDX, IDC_CHECK_ENGI_OPT, m_bEngiOptCheck);
	DDX_Text(pDX, IDC_TPRGM_EDIT, m_EngiTestPrgm);
	DDX_Check(pDX, IDC_RTM, m_bRTM);
	//}}AFX_DATA_MAP

	// Perform Fill of Current Configuration
	// if invalid, do not handle number of duts selected
	if (GetConfiguration(m_SessionConfig))
	{
		m_szHandlerId.GetWindowText(m_SessionConfig.strHandlerId);
		m_EditLotNumber.GetWindowText(m_SessionConfig.strLotNumber);
		m_EditSubLotNumber.GetWindowText(m_SessionConfig.strSubLotNumber);
		m_ComboHandlerDll.GetWindowText(m_SessionConfig.strHandlerDll);
		m_szCableNumber.GetWindowText(m_SessionConfig.strCableNumber);
		m_szLoadBoardNumber.GetWindowText(m_SessionConfig.strLoadBoardNumber);
		m_szOperatorName.GetWindowText(m_SessionConfig.strOperator);
		m_SessionConfig.strSetupRev=szSetupRev;

		// Engi options
		if (m_bEngiOptCheck && (m_EngiTestPrgm != ""))
			m_SessionConfig.strProgramPath = m_EngiTestPrgm;
		else
			m_SessionConfig.strProgramPath = m_SessionConfig.GetProgram();

		// RTM
		m_SessionConfig.bRTM = m_bRTM;
	}
}

BEGIN_MESSAGE_MAP(CFTTSPDlg, CDialog)
	//{{AFX_MSG_MAP(CFTTSPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_PRODUCT, OnSelchangeProduct)
	ON_CBN_SELCHANGE(IDC_PART_ID, OnSelchangePartId)
	ON_CBN_SELCHANGE(IDC_FLOW, OnSelchangeFlow)
	ON_CBN_SELCHANGE(IDC_HANDLER, OnSelchangeHandler)
	ON_EN_CHANGE(IDC_LOT_NUMBER, OnChangeLotNumber)
	ON_EN_CHANGE(IDC_HANDLER_ID, OnChangeHandlerId)
	ON_BN_CLICKED(IDC_DEGRADED_MODE, OnDegradedMode)
	ON_CBN_SELCHANGE(IDC_HANDLER_DLL, OnSelchangeHandlerDll)
	ON_CBN_SELCHANGE(IDC_PRODUCT_REV, OnSelchangeProductRev)
	ON_EN_CHANGE(IDC_LB_NUMBER, OnChangeLbNumber)
	ON_EN_CHANGE(IDC_CABLE_NUMBER, OnChangeCableNumber)
	ON_EN_CHANGE(IDC_OP_NAME, OnChangeOpName)
	ON_EN_CHANGE(IDC_SUB_LOT_NUMBER, OnChangeSubLotNumber)
	ON_BN_CLICKED(IDC_CHECK_SITE_1, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_ENGI_OPT, OnCheckEngiOpt)
	ON_BN_CLICKED(IDC_BROWSE_TPGRM, OnBrowseTpgrm)
	ON_BN_CLICKED(IDC_CHECK_SITE_2, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_SITE_3, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_SITE_4, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_SITE_5, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_SITE_6, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_SITE_7, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_SITE_8, OnCheckSite)
	ON_BN_CLICKED(IDC_CHECK_SITES_ALL, OnCheckAllSites)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CFTTSPDlg::PreTranslateMessage(MSG *pMsg)
{
	if (NULL != m_pToolTip)
		m_pToolTip->RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CFTTSPDlg message handlers
BOOL CFTTSPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 

	/* Modify the caption to add the version number */
	SetWindowText(ptrApp->SetWindowText());
	
	// Caption init for the setup revision
	CButton *pSFT_Rev=(CButton*)GetDlgItem(IDC_STATIC_SFT_REV);
	pSFT_Rev->SetWindowText("");

	GetPrivateProfileString("PHOENIX", "path", PHOENIX_PATH, m_sPhoenixPath.GetBuffer(100), 100, ptrApp->m_szConfigFilePath);
	m_sPhoenixPath.ReleaseBuffer();

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
	
	// Initialize once for all the list of products
	// if return value is negative, an error occured and program should be stopped
	int errCode;

	errCode =FillProducts();
	errCode +=FillHandlerDll();
//		(FillProducts()   >= 0) && 
//		(FillHandlerDll() >= 0);
	if (errCode < 0)
	{
		EndDialog(errCode);
		return FALSE;
	}

	// Init dialog with all Combo disabled
	FillProducts(); 
	UpdateControls();

	/* Check if FTTSP is working through a subcontractor gateway */
	//CSetupGateway SubGateway(ptrApp->GetConfigPath());
	CSetupGateway * pSubGateway = new CSetupGateway(ptrApp->GetConfigPath());
	ptrApp->m_pSubGateway=pSubGateway;
	CString szTmpMode("Normal mode");
	if ( pSubGateway->bGatewayFound()==TRUE )
	{
		if (CheckGatewayData(pSubGateway)==FALSE)
		{
			ClearConfigurations();
			AfxMessageBox("Normal mode recovered!",MB_ICONEXCLAMATION );

			/* Update the status bar */
			m_EditLotNumber.SetWindowText("");
			m_EditSubLotNumber.SetWindowText("");
		}
		else
		{
			szTmpMode.Format("Mode gateway : Subcontractor %s , gateway => %s",pSubGateway->szGetSubcontractor(),pSubGateway->szGetGatewayFile());
		}
	}
	m_szMode=szTmpMode;
	m_szTesterType="";

	ShowHideEngiOptions(FALSE);

	if (ptrApp->m_sRTM == "YES")
		m_bRTM = TRUE;

	m_pToolTip = new CToolTipCtrl;
	m_pToolTip->Create(this);
	//m_pToolTip->AddTool(this, "FTTSP Application");
	m_pToolTip->AddTool(&m_cRTM, "Write results to windows registry");

	UpdateData(FALSE);

#ifdef _TEST // Autofill dialog
	TRACE("TEST MODE\n");
	m_ComboProduct.SelectString(0, "59U16");
	OnSelchangeProduct();
	m_ComboHandlerDll.SelectString(0, "ManualHandler.dll");
	m_szHandlerId.SetWindowText("ST999");
	m_EditLotNumber.SetWindowText("LOT");
	m_EditSubLotNumber.SetWindowText("SUB");
	m_szLoadBoardNumber.SetWindowText("1");
	m_szCableNumber.SetWindowText("1");
	m_szOperatorName.SetWindowText("LBS");
	UpdateData(FALSE);
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFTTSPDlg::ResetSocketCheckBox(void)
{
	int aiSocketUsed[8]={	IDC_CHECK_SITE_1,IDC_CHECK_SITE_2,IDC_CHECK_SITE_3,IDC_CHECK_SITE_4, 
							IDC_CHECK_SITE_5,IDC_CHECK_SITE_6,IDC_CHECK_SITE_7,IDC_CHECK_SITE_8};

	int iNbSockets = 8;
	CButton *pCheck;
	for (int iSocketUsed=0;iSocketUsed<iNbSockets;iSocketUsed++)
	{
		pCheck=(CButton*)GetDlgItem(aiSocketUsed[iSocketUsed]);
		pCheck->SetCheck(FALSE);
	}

}

int CFTTSPDlg::GetSiteUsed(void)
{
	int aiSocketUsed[8]={	IDC_CHECK_SITE_1,IDC_CHECK_SITE_2,IDC_CHECK_SITE_3,IDC_CHECK_SITE_4, 
							IDC_CHECK_SITE_5,IDC_CHECK_SITE_6,IDC_CHECK_SITE_7,IDC_CHECK_SITE_8};

	// Retrieve name of handler
	CString aString;
	CObject *pObject;
	m_ComboHandler.GetWindowText(aString);
	m_MapHandler.Lookup(aString, pObject);

	ASSERT(pObject->IsKindOf(RUNTIME_CLASS(CObList)));
	ASSERT(((CObList*)pObject)->GetCount() == 1);
	pObject = ((CObList*)pObject)->GetHead();
	ASSERT(pObject->IsKindOf(RUNTIME_CLASS(CConfiguration)));
	CConfiguration aConfig = *((CConfiguration *) pObject);

	int iNbSockets=aConfig.waSkts.GetSize();
	int iTestMask=0;
	if (iNbSockets>0 && iNbSockets<=8)
	{
		CButton * pCheck;
		for (int iSocketUsed=0;iSocketUsed<iNbSockets;iSocketUsed++)
		{
			pCheck=(CButton*)GetDlgItem(aiSocketUsed[iSocketUsed]);
			if ( pCheck->GetCheck()==TRUE )
			{
				iTestMask|=(1<<iSocketUsed);
			}
		}
		return iTestMask;
	}
	else if (iNbSockets>8)
	{
		return ( (1<<iNbSockets)-1 );
	}
	else
	{
		return 0;
	}
}

BOOL CFTTSPDlg::CheckGatewayData(CSetupGateway * pSubGateway)
{
	/* Error message preparation */
	CString szMess("");
	CString szParam("");

	/* A gateway mechanism is requested. Retrieve all data from gateway */
	int awRet[6]={0};
	int nIndex=0;
	awRet[nIndex++]=m_ComboProduct.SelectString(-1,pSubGateway->szGetProductName() );
	OnSelchangeProduct();
	awRet[nIndex++]=m_ComboRev.SelectString( -1,pSubGateway->szGetProductRev() );
	OnSelchangeProductRev();
	awRet[nIndex++]=m_ComboPartID.SelectString( -1,pSubGateway->szGetPartID() );
	OnSelchangePartId();
	awRet[nIndex++]=m_ComboFlow.SelectString( -1,pSubGateway->szGetFlow() );
	OnSelchangeFlow();
	m_EditLotNumber.SetWindowText(pSubGateway->szGetLotNumber() );
	awRet[nIndex++]=(pSubGateway->szGetLotNumber().GetLength()>3) ? 0 :-1 ;
	m_EditSubLotNumber.SetWindowText(pSubGateway->szGetSubLotNumber() );

	/* Check if error was found into the 4 combobox and lot number edit box */
	for ( nIndex=0; nIndex<5; nIndex++ )
	{
		if (awRet[nIndex]<0)
		{
			/* Error was found! Reset all combo and discard the gateway mecanism */
			szMess="Gateway mode is requested!\nBut at least one of the parameters used into gateway\n is not correct!\n";
			switch (nIndex)
			{
				/* Product number is not known */
				case 0:
				{
					szParam.Format("The '%s' product can't be tested because \nthere is no '%s_setup.txt' file from the FTTSP environment.",
						pSubGateway->szGetProductName(),
						pSubGateway->szGetProductName()); 
				}
				break;

				/* Design revision is not known */
				case 1:
				{
					szParam.Format("The design revision '%s' is not managed by the\n'%s_setup.txt' file from the FTTSP environment.",
						pSubGateway->szGetProductRev(),
						pSubGateway->szGetProductName()); 
				}
				break;

				/* Part ID is not known */
				case 2:
				{
					szParam.Format("The part ID '%s' is not managed by the\n'%s_setup.txt' file from the FTTSP environment.",
						pSubGateway->szGetPartID(),
						pSubGateway->szGetProductName()); 
				}
				break;

				/* The flow is not known */
				case 3:
				{
					szParam.Format("The flow '%s' is not managed by the\n'%s_setup.txt' file from the FTTSP environment.",
						pSubGateway->szGetFlow(),
						pSubGateway->szGetProductName()); 
				}
				break;

				/* The lot number format is not standard */
				case 4:
				{
					szParam.Format("The lot number '%s' is not standard!",pSubGateway->szGetLotNumber()); 
				}
				break;

				default:
				{

				}
				break;
			}
			/* Exit the loop on first error */
			szMess+=szParam;
			AfxMessageBox(szMess,MB_ICONEXCLAMATION );
			return FALSE;
		}
	}
	return TRUE;
}


void CFTTSPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFTTSPDlg::OnPaint() 
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
HCURSOR CFTTSPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFTTSPDlg::UpdateControls()
{
	EnableCombo(m_ComboProduct);
	EnableCombo(m_ComboRev);
	EnableCombo(m_ComboPartID );
	EnableCombo(m_ComboFlow   );
	EnableCombo(m_ComboHandler);
	EnableCombo(m_ComboHandlerDll);
	EnableCombo(m_ComboLB);
	EnableCombo(m_ComboCable);

	// Enables combos if sufficient data in them and in Degraded mode
	EnableCombo(m_ComboControllers, m_ButtonDegraded.GetCheck());
	EnableCombo(m_ComboSockets    , m_ButtonDegraded.GetCheck());

	m_ButtonStartProbe.EnableWindow(CheckForStart());
}

void CFTTSPDlg::ClearConfigurations()
{
	// Delete Maps
	EmptyMap(m_MapRev);
	EmptyMap(m_MapPartID);
	EmptyMap(m_MapFlow);
	EmptyMap(m_MapHandler);
	EmptyMap(m_MapLoadBoardRef);
	EmptyMap(m_MapCableRef);

	// Delete Skt Mapping
	POSITION pos = m_ConfigurationList.GetHeadPosition();
	while (pos)
		delete m_ConfigurationList.GetNext(pos);
	m_ConfigurationList.RemoveAll();
}

int CFTTSPDlg::FillComboWithFilenames(CComboBox &aCB, LPCSTR aPath, LPCSTR aFilter, BOOL bRemoveFilter)
{
	// Clear Filelist
	aCB.ResetContent();

	CFileFind finder;
	BOOL bWorking;
	CString strFilename;

	strFilename = CString(aPath);
	strFilename.TrimRight('\\');
	strFilename += CString("\\*") + aFilter;
	
	bWorking = finder.FindFile(strFilename);

	// List all the files in the directory.
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strFilename = finder.GetFileName();
		if (bRemoveFilter)
			strFilename = strFilename.Left(strFilename.GetLength() - strlen(aFilter));
		aCB.AddString(strFilename);
	}

	return aCB.GetCount();
}

int CFTTSPDlg::FillProducts()
{
    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 

	if (FillComboWithFilenames(m_ComboProduct, ptrApp->GetSetupPath(), strSetupFilter, TRUE) <= 0)
	{
		CString ErrorMessage;
		AfxFormatString1(ErrorMessage, IDS_ERROR_NO_SETUP, ptrApp->GetSetupPath());
		AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION | MB_OK);
		return -2;
	}
	else if (m_ComboProduct.GetCount() == 1)
	{
		m_ComboProduct.SetCurSel(0);
		OnSelchangeProduct();
		if (m_iRefCKS==0xFFFFFFFF)
		{
			return -1;
		}
	}

	return 0;
}

int CFTTSPDlg::FillHandlerDll()
{
    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 
	if (FillComboWithFilenames(m_ComboHandlerDll, ptrApp->GetHandlerDllPath(), ".dll", FALSE) <= 0)
	{
		CString ErrorMessage;
		AfxFormatString1(ErrorMessage, IDS_ERROR_NO_DLL, ptrApp->GetHandlerDllPath());
		AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION | MB_OK);
		return -2;
	}

	return 0;
}

int CFTTSPDlg::FillPartID()
{
	CObject *pObject;
	const CObList *pObList = NULL;
	CString Rev;

	// Clear map before filling it
	EmptyMap(m_MapPartID);

	// Is a Rev selected ?
	if (m_ComboRev.GetCurSel() != CB_ERR)
	{
		m_ComboRev.GetWindowText(Rev);

		if (m_MapRev.Lookup(Rev, pObject)) // pObject contains returned value from the lookup element
		{
			ASSERT(pObject->IsKindOf( RUNTIME_CLASS(CObList) ) );
			pObList = (const CObList *)pObject;
			FillMap(*pObList, m_MapPartID, CConfiguration::GetPartID);
		}
	}

	FillCombo(m_ComboPartID, m_MapPartID);
	return 0;
}

int CFTTSPDlg::FillProductRev()
{
	FillMap(m_ConfigurationList, m_MapRev, CConfiguration::GetRevDesign);

	FillCombo(m_ComboRev, m_MapRev);
	return 0;
}

int CFTTSPDlg::FillFlow()
{
	CObject *pObject;
	const CObList *pObList = NULL;
	CString PartID;

	// Clear map before filling it
	EmptyMap(m_MapFlow);

	// Is a Part ID selected ?
	if (m_ComboPartID.GetCurSel() != CB_ERR)
	{
		m_ComboPartID.GetWindowText(PartID);

		if (m_MapPartID.Lookup(PartID, pObject))
		{
			ASSERT(pObject->IsKindOf( RUNTIME_CLASS(CObList) ) );
			pObList = (const CObList *)pObject;
			FillMap(*pObList, m_MapFlow, CConfiguration::GetFlow);
		}
	}

	FillCombo(m_ComboFlow, m_MapFlow);
	return 0;
}

int CFTTSPDlg::FillHandler()
{
	CObject *pObject;
	const CObList *pObList;
	CString Flow;

	EmptyMap(m_MapHandler);
	EmptyMap(m_MapTesterType);

	if (m_ComboFlow.GetCurSel() != CB_ERR)
	{
		m_ComboFlow.GetWindowText(Flow);

		if (m_MapFlow.Lookup(Flow, pObject))
		{
			ASSERT(pObject->IsKindOf( RUNTIME_CLASS(CObList) ) );
			pObList = (const CObList *)pObject;
			FillMap(*pObList, m_MapHandler, CConfiguration::GetHandler);
			FillMap(*pObList, m_MapTesterType, CConfiguration::GetTesterType);
		}
	}

	FillCombo(m_ComboHandler, m_MapHandler);

	if (m_MapTesterType.GetCount() == 1)
	{
		POSITION pos = m_MapTesterType.GetStartPosition();
		m_MapTesterType.GetNextAssoc(pos, m_szTesterType, pObject);
	}
	else
		m_szTesterType = "";

	UpdateData(FALSE);

	return 0;
}

int CFTTSPDlg::FillLoadBoard()
{
	FillMap(m_ConfigurationList, m_MapLoadBoardRef, CConfiguration::GetLoadBoard);
	
	FillCombo(m_ComboLB, m_MapLoadBoardRef);
	return 0;
}

int CFTTSPDlg::FillCable()
{
	FillMap(m_ConfigurationList, m_MapCableRef, CConfiguration::GetCable);
	
	FillCombo(m_ComboCable, m_MapCableRef);
	return 0;
}


int CFTTSPDlg::FillControllersAndSockets()
{
	CObject *pObject;
	const CObList *pObList;
	CString Handler;

	m_ComboControllers.ResetContent();
	m_ComboSockets.ResetContent();
	
	int aiSocketUsed[8]={	IDC_CHECK_SITE_1,IDC_CHECK_SITE_2,IDC_CHECK_SITE_3,IDC_CHECK_SITE_4, 
							IDC_CHECK_SITE_5,IDC_CHECK_SITE_6,IDC_CHECK_SITE_7,IDC_CHECK_SITE_8};

	CButton * pCheck;
	for (int iSocketUsed=0;iSocketUsed<8;iSocketUsed++)
	{
		pCheck=(CButton*)GetDlgItem(aiSocketUsed[iSocketUsed]);
		ASSERT_VALID(pCheck);
		pCheck->EnableWindow(FALSE);
		pCheck->ShowWindow(0);
	}

	CButton * pCheckAll;
	pCheckAll=(CButton*)GetDlgItem(IDC_CHECK_SITES_ALL);
	ASSERT_VALID(pCheckAll);
	pCheckAll->EnableWindow(FALSE);
	pCheckAll->ShowWindow(0);


	CStatic * pFrame=(CStatic*)GetDlgItem(IDC_STATIC_SITE_USED);
	//pFrame->ShowWindow(0);

	if (m_ComboHandler.GetCurSel() != CB_ERR)
	{
		m_ComboHandler.GetWindowText(Handler);

		if (m_MapHandler.Lookup(Handler, pObject))
		{
			ASSERT(pObject->IsKindOf( RUNTIME_CLASS(CObList) ) );
			pObList = (const CObList *)pObject;

			// Expects only one Object.
			// If more, indicate the error and remove list
			if (pObList->GetCount() == 1)
			{
				CConfiguration* pConfig;

				pObject = pObList->GetHead();
				ASSERT(pObject->IsKindOf( RUNTIME_CLASS(CConfiguration) ) );
				pConfig = (CConfiguration*) pObject;

				int i, nLastAdded = CB_ERR;
				CString aTmpString;

				// Controllers Combo Box
				for (i=1; i<= pConfig->GetControllers(); i++) {
					aTmpString.Format("%d", i);
					nLastAdded = m_ComboControllers.AddString(aTmpString);
				}
				m_ComboControllers.SetCurSel(nLastAdded);

				nLastAdded = CB_ERR;
				// Sockets Combo Box
				for (i=1; i<= pConfig->GetSockets().GetSize(); i++) 
				{
					aTmpString.Format("%d", i);
					nLastAdded = m_ComboSockets.AddString(aTmpString);
					if (pConfig->GetSockets().GetSize()<=8)	
					{
						pCheck=(CButton*)GetDlgItem(aiSocketUsed[i-1]);
						ASSERT_VALID(pCheck);
						pCheck->EnableWindow(TRUE);
						//!!08042021:LBs:Added check button by default + "All" button
						pCheck->SetCheck(TRUE);
						pCheck->ShowWindow(TRUE);
						pFrame->ShowWindow(TRUE);
						pCheckAll->ShowWindow(TRUE);
						pCheckAll->EnableWindow(TRUE);
						pCheckAll->SetCheck(TRUE);
					}
				}

				m_ComboSockets.SetCurSel(nLastAdded);
			}
			else
			{
				CString ErrorMessage;
				ErrorMessage.Format(IDS_ERROR_NUMBER_CONFIGURATIONS, pObList->GetCount());
				AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION | MB_OK);


				// Configuration List and Handler List are in the same order
				// So the position can be deleted safely without return to
				// Head of list.
				POSITION posCL, posCL2Delete, posHL;

				posCL = m_ConfigurationList.GetHeadPosition();
				posHL = pObList->GetHeadPosition();

				while(posHL)
				{
					posCL2Delete = posCL;
					if (m_ConfigurationList.GetNext(posCL) == pObList->GetAt(posHL))
					{
						// Configuration has to be deleted because it has been constructed 
						// during Configuration file parse.
						delete m_ConfigurationList.GetAt(posCL2Delete);
						m_ConfigurationList.RemoveAt(posCL2Delete);
						pObList->GetNext(posHL);
					}
				}

				// Remove all selection in Part ID and call for selection change
				// This method is used to avoid big call stack
				FillPartID();

				// Code below will act as the following line
				// OnSelchangePartId();
				// But the execution is delayed until current message is processed
				// this prevents big call stack
				PostMessage(
					WM_COMMAND,
					MAKEWPARAM(IDC_PART_ID, CBN_SELCHANGE),
					(LPARAM) m_ComboPartID.GetSafeHwnd()
					);

				return -1;
			}
		}
	}

	if (m_ButtonDegraded.GetCheck()==1)
	{
		if (m_ComboControllers.GetCount() > 1)
			m_ComboControllers.EnableWindow(TRUE);
		else
			m_ComboControllers.EnableWindow(FALSE);

		if (m_ComboSockets.GetCount() > 1)
			m_ComboSockets.EnableWindow(TRUE);
		else
			m_ComboSockets.EnableWindow(FALSE);
	}
	else
	{
		m_ComboControllers.EnableWindow(FALSE);
		m_ComboSockets.EnableWindow(FALSE);
	}
	return 0;
}

void CFTTSPDlg::FillMap(const CObList &ConfigList, CMapStringToOb &MapToFill, CString (CConfiguration::*WhatToLookAt) ( void ) const)
{
	// Valid Configurations have been retrieved.
	// Fill Map with accurate data.
	CObject *pObject;
	CObList *pObList;
	CString WhatIsSearched;
	CConfiguration *pConfig;

	// Begin by clearing Map Contents
	EmptyMap(MapToFill);
	
	POSITION pos = ConfigList.GetHeadPosition();
	while (pos)
	{
		pObject = ConfigList.GetNext(pos);

		ASSERT(pObject->IsKindOf( RUNTIME_CLASS(CConfiguration) ));
		pConfig = (CConfiguration *) pObject;

		WhatIsSearched = (pConfig->*WhatToLookAt)(); // Get configuration element string
		if (MapToFill.Lookup(WhatIsSearched, pObject))
		{
			// At least already one CConfiguration here	
			ASSERT(pObject->IsKindOf( RUNTIME_CLASS( CObList ) ) );
			pObList = (CObList *)pObject;
			pObList->AddTail(pConfig); // Add tail to pointer already stored in MapToFill[WhatIsSearched]
		}
		else
		{
			// First Configuration
			pObList = new CObList();
			pObList->AddTail(pConfig);
			MapToFill[WhatIsSearched] = pObList; // Store pointer in Map
		}
	}

}

void CFTTSPDlg::FillCombo(CComboBox &aCB, CMapStringToOb &aMap)
{
	CObject *pObject;

	aCB.ResetContent();

	CString aString;
	POSITION pos = aMap.GetStartPosition();
	while (pos != NULL)
	{
		aMap.GetNextAssoc(pos, aString, pObject);
		aCB.AddString(aString);
	}

	if (aCB.GetCount() == 1)
		aCB.SetCurSel(0);
}

void CFTTSPDlg::OnSelchangeProduct() 
{
	if (m_ComboProduct.GetCurSel() == CB_ERR)
		return;

    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 
	CString aProduct;

	// Selection has changed so delete all available configurations and
	// reload from file
	ClearConfigurations();
	ResetSocketCheckBox();

	/* Read the Product selected */
	m_ComboProduct.GetWindowText(aProduct);

	CString aSetupFilename;

	aSetupFilename = ptrApp->GetSetupPath() + "\\" + aProduct +strSetupFilter;

	CString aConfigString;
	CConfiguration aConfig;

	BOOL bIsOK = TRUE;

	/* Try to open the setup file linked to the selected product */
	CStdioFile SetupFile;
	if (! SetupFile.Open(aSetupFilename, CFile::modeRead | CFile::shareDenyNone | CFile::typeText))
	{
		CString ErrorMsg;
		AfxFormatString1(ErrorMsg, IDS_ERROR_CANNOT_OPEN_SETUP, aSetupFilename);
		AfxMessageBox(ErrorMsg, MB_ICONEXCLAMATION | MB_OK);
		if (m_ComboProduct.DeleteString(m_ComboProduct.GetCurSel()) == 0)
		{
			// There are Configuration files no more, Close Program
			EndDialog(-4);
		}
		else
		{
			UpdateControls();
			// Code below will act as the following line
			// OnSelchangeProduct();
			// But the execution is delayed until current message is processed
			// this prevents big call stack
			PostMessage(
				WM_COMMAND,
				MAKEWPARAM(IDC_PRODUCT, CBN_SELCHANGE),
				(LPARAM) m_ComboProduct.GetSafeHwnd()
				);
		}
		return;
	}

	
	int nCurrentLine = 0;
	szSetupRev="";
	m_iRefCKS=0xFFFF;
	while (SetupFile.ReadString(aConfigString))
	{
		nCurrentLine ++;

		aConfigString.TrimLeft();

		// Check if REV of setup file 
		if ( !(aConfigString.Find("//SFT_Rev:",0)<0) )
		{
			CString szTmp=aConfigString.Mid(aConfigString.ReverseFind(':')+1);
			szTmp.TrimLeft();
			szTmp.TrimRight();
			szSetupRev=szTmp;
			//sscanf(szTmp,"%d",&m_iRevNum);
		}
		// Check if CRC of the estup file
		else if ( !(aConfigString.Find("//#SFT_CRC:",0)<0) )
		{
			CString szTmp=aConfigString.Mid(aConfigString.ReverseFind(':')+1);
			szTmp.TrimLeft();
			szTmp.TrimRight();
			sscanf(szTmp,"0x%08x",&m_iRefCKS);
			m_iRefCKS&=0xFFFFFFFF;
		}
		else if (aConfigString.Left(2) == "//" || aConfigString.Left(1) == "#"  || aConfigString.GetLength() == 0)
			continue;
		
		if (! aConfig.Parse(aConfigString, '\t'))
		{
			CString ErrorMessage;
			ErrorMessage.Format(IDS_ERROR_SETUP, (LPCTSTR) aSetupFilename, nCurrentLine);
			AfxMessageBox(ErrorMessage, MB_ICONEXCLAMATION | MB_OK);
			if (m_ComboProduct.DeleteString(m_ComboProduct.GetCurSel()) == 0)
			{
				// There are Configuration files no more, Close Program (delayed with the message pump)
				EndDialog(-4);
			}
			else
			{
				UpdateControls();
				// Code below will act as the following line
				// OnSelchangeProduct();
				// But the execution is delayed until current message is processed
				// this prevents big call stack
				PostMessage(
					WM_COMMAND,
					MAKEWPARAM(IDC_PRODUCT, CBN_SELCHANGE),
					(LPARAM) m_ComboProduct.GetSafeHwnd()
					);
			}
			bIsOK = FALSE;
			break;
		}
		// Configuration is valid so add it to the list
		m_ConfigurationList.AddTail(new CConfiguration(aConfig));
	}
	
	// Calculation of the CRC of the setup file
	if (bIsOK)
	{
		SetupFile.SeekToBegin();
		__int64 ChkSum=0;
		unsigned int Line=0;
		while (SetupFile.ReadString(aConfigString))
		{
			aConfigString.TrimLeft();
			aConfigString.TrimRight();
			
			if(aConfigString.Find("//#")==0) // line with // and # at the beginning are not use for chksum calculation
				continue;
			
			if(aConfigString.GetLength()!=0)
			{
				aConfigString.MakeUpper();
				int i=0;
				while(i<aConfigString.GetLength())
				{
					int TmpByte=aConfigString.GetAt(i);
					int BitShift=i%5 + Line%3;
					while(BitShift!=0)		//with this loop, data are modify according the position in the file
					{					
						if(i%2==0)
						{
							if(TmpByte&0x80==0x80)
							{
								TmpByte<<=1;
								TmpByte|=0x01;
							}
							else
								TmpByte<<=1;
						}
						else
						{
							if(TmpByte&0x01==0x01)
							{
								TmpByte>>=1;
								TmpByte|=0x80;
							}
							else
								TmpByte>>=1;
						}
						BitShift--;
					}
					ChkSum+=TmpByte;
					ChkSum&=0xFFFFFFFF;
					i++;
				}
			}
			Line++;
		}
		ChkSum=(~ChkSum)&0xFFFFFFFF;
		if (ChkSum!=m_iRefCKS && (!ptrApp->oInfo.bEngiEnv() + (ptrApp->oInfo.bEngiEnv() && (m_iRefCKS != CKS_BYPASS_VALUE))))
		{
			MessageBox("The setup file is corrupted!","Error:",MB_ICONERROR);
			m_iRefCKS=0xFFFFFFFF;
			m_ComboProduct.DeleteString(m_ComboProduct.GetCurSel());
			bIsOK=FALSE;
		}
	}

	SetupFile.Close();

	// if one config is not valid, delete everything
	if (! bIsOK)
	{
		ClearConfigurations();
		UpdateControls();
		return;
	}

	CButton *pSFT_Rev=(CButton*)GetDlgItem(IDC_STATIC_SFT_REV);
	pSFT_Rev->SetWindowText("Setup rev "+szSetupRev);

	// Keep only the Configuration for Current Release and diplay an Error
	// message if no configuration valid
	if (FilterForCurrentRelease() < 1)
	{
		AfxMessageBox(IDS_ERROR_CURRENT_RELEASE, MB_ICONEXCLAMATION | MB_OK);
		if (m_ComboProduct.DeleteString(m_ComboProduct.GetCurSel()) == 0)
		{
			// There are Configuration files no more, Close Program
			EndDialog(-5);
		}
		ClearConfigurations();
	}

	m_szTesterType = "";
	FillProductRev();
	FillPartID();
	FillFlow();
	FillHandler();
	FillLoadBoard();
	FillCable();
	FillControllersAndSockets();

	UpdateControls();
	UpdateData(FALSE);
}

void CFTTSPDlg::OnSelchangeProductRev() 
{
	FillPartID();
	FillFlow();
	FillHandler();
	FillLoadBoard();
	FillCable();
	FillControllersAndSockets();

	UpdateControls();
}

void CFTTSPDlg::OnSelchangePartId() 
{
	int z = m_ComboPartID.GetCurSel();
	FillFlow();
	FillHandler();
	FillLoadBoard();
	FillCable();
	FillControllersAndSockets();

	UpdateControls();
}

void CFTTSPDlg::OnSelchangeFlow() 
{
	FillHandler();
	FillLoadBoard();
	FillCable();
	FillControllersAndSockets();

	UpdateControls();
}

void CFTTSPDlg::OnSelchangeHandler() 
{
	FillLoadBoard();
	FillCable();
	FillControllersAndSockets();

	UpdateControls();
}

void CFTTSPDlg::EmptyMap(CMapStringToOb &aMap)
{
	CString anUnusedKey;
	CObject *pObject;

	POSITION pos = aMap.GetStartPosition();
	while (pos)
	{
		aMap.GetNextAssoc(pos, anUnusedKey, pObject);
		delete pObject;
	}
	aMap.RemoveAll();
}

BOOL CFTTSPDlg::CheckForStart()
{
	// For debug
	int a = m_ComboProduct.GetCurSel();
	int b = m_ComboPartID.GetCurSel();
	int c = m_ComboFlow.GetCurSel();
	int d = m_ComboHandler.GetCurSel();
	int e = m_ComboHandlerDll.GetCurSel();
	int f = m_ComboControllers.GetCurSel();
	int g = m_ComboSockets.GetCurSel();	
	int h = m_ComboLB.GetCurSel();	
	int i = m_ComboCable.GetCurSel();

	BOOL IsValid = FALSE;
	if ((m_ComboProduct    .GetCurSel() != CB_ERR) &&
		(m_ComboRev		   .GetCurSel() != CB_ERR) &&
		(m_ComboPartID     .GetCurSel() != CB_ERR) &&
		(m_ComboFlow       .GetCurSel() != CB_ERR) &&
		(m_ComboHandler    .GetCurSel() != CB_ERR) &&
		(m_ComboHandlerDll .GetCurSel() != CB_ERR) &&
		(m_ComboControllers.GetCurSel() != CB_ERR) &&
		(m_ComboLB		   .GetCurSel() != CB_ERR) &&
		(m_ComboCable	   .GetCurSel() != CB_ERR) &&
		(m_ComboSockets    .GetCurSel() != CB_ERR) 
		)
	{
	
		// All Combos are valid, now check for the lot number
		CString LotNumber;
		m_EditLotNumber.GetWindowText(LotNumber);

		// Remove all spaces at begin and end
		LotNumber.TrimLeft();
		LotNumber.TrimRight();

		// Spaces are not allowed
		if (LotNumber.FindOneOf(" \t\r\n\f") == -1)
		{
			// A Lot number is 5 alphanum characters minimum && cable number, load board number and operator name are set
			if ((m_szCableNumber		  .GetWindowTextLength()>0) &&
				(m_szLoadBoardNumber	  .GetWindowTextLength()>0) &&
				(m_szOperatorName		  .GetWindowTextLength()>0) && 
				(m_szHandlerId			  .GetWindowTextLength()>0) && 
				(LotNumber                .GetLength() >= 3)
				)
			{
				// Check if sites under test are chosen
				if (GetSiteUsed()>0)
				{
					IsValid = TRUE;
				}
			}
		}
	}

	return IsValid;
}

void CFTTSPDlg::OnSelchangeHandlerDll() 
{
	UpdateControls();
}

//!!08042021:LBs:Added handler id entry
void CFTTSPDlg::OnChangeHandlerId() 
{
	UpdateControls();
}

void CFTTSPDlg::OnChangeLotNumber() 
{
	UpdateControls();
}

int CFTTSPDlg::FilterForCurrentRelease()
{
	//13032023:LBs:Added Get release of TX
	CString aCR, aUICR, aPhoenixCR, aTXCR;

	aUICR = CUI::GetCurrentRelease();

	aTXCR = CTX::GetCurrentRelease();

	//06022017:LBo:Set default version when path is empty (remote Phoenix) $LABS-2699
	if (m_sPhoenixPath.IsEmpty())
	{
		aPhoenixCR = _T("v1.0.0.0");
	}
	else
	{
		aPhoenixCR = CPhoenix::GetCurrentRelease(m_sPhoenixPath);
	}
/*
	if (aUICR.IsEmpty())
	{
		AfxMessageBox(GetLastErrorDescription(), MB_ICONEXCLAMATION | MB_OK);
		ClearConfigurations();
		return -1;
	}
*/
	// Pos1 to walk through list
	// Pos2 to delete if not valid
	POSITION pos1, pos2;
	CObject *pObject;
	CConfiguration *pConfig;

	CString aTesterType, aSoftVersion;
	BOOL bIsValid;
	int nValidConfig = 0;

	for (pos1 = m_ConfigurationList.GetHeadPosition(); (pos2 = pos1) != NULL; )
	{
		pObject = m_ConfigurationList.GetNext(pos1);
		ASSERT(pObject->IsKindOf(RUNTIME_CLASS(CConfiguration)));
		pConfig = (CConfiguration *)pObject;

		// Assume that Current Config is valid until declared it is not.
		bIsValid = TRUE;

		// Verify TesterType is known
		aTesterType = pConfig->GetTesterType();
		aTesterType.MakeUpper();
		if ((aTesterType.Left(3) != "MAV") && 
			(aTesterType.Left(2) != "TX") &&
			(aTesterType != "PHOENIX") &&
			(aTesterType != "PC")
			)
			bIsValid = FALSE;

		// Verify if Soft Version is known && if Tester is different at PC
		aSoftVersion = pConfig->GetSoftVersion();
		aSoftVersion.MakeLower();

		if (aTesterType.Left(2) == "TX")
			aCR = aTXCR;
		else if (aTesterType == "PHOENIX")
			aCR = aPhoenixCR;
		else if (aTesterType.Left(3) == "MAV")
			aCR = aUICR;
		else // PC
			bIsValid = FALSE;
		
		aCR.MakeLower();
		if (bIsValid && aCR.Find(aSoftVersion) >= 0)
		{
			nValidConfig++;
		}
		else
		{
			// Remove from list
			m_ConfigurationList.RemoveAt(pos2);
			delete pConfig;
		}
	}

	return nValidConfig;
}

BOOL CFTTSPDlg::GetConfiguration(CConfiguration& aConfig)
{
	CString aString;
	CObject* pObject;

	if (! CheckForStart())
		return FALSE;

	// Retrieve name of handler
	m_ComboHandler.GetWindowText(aString);
	m_MapHandler.Lookup(aString, pObject);

	ASSERT(pObject->IsKindOf(RUNTIME_CLASS(CObList)));
	ASSERT(((CObList*)pObject)->GetCount() == 1);
	pObject = ((CObList*)pObject)->GetHead();
	ASSERT(pObject->IsKindOf(RUNTIME_CLASS(CConfiguration)));
	aConfig = *((CConfiguration *) pObject);

	// Truncate waSkts to selected number of sockets
	m_ComboSockets.GetWindowText(aString);
	aConfig.waSkts.SetSize(atoi(aString));

	// Modified the 19/11/2012 by CDE to allow possibility to check on HMI which socket is in test
	int iNbSockets=aConfig.waSkts.GetSize();
	aConfig.iTestMask=GetSiteUsed();
	// End modification 

	// Set number of controllers to selected count
	m_ComboControllers.GetWindowText(aString);
	aConfig.nControllers = atoi(aString);

	return TRUE;
}

void CFTTSPDlg::OnDegradedMode() 
{
	if (m_ButtonDegraded.GetCheck() == 0)
	{
		if (m_SessionConfig.strEngiModePassword != ENGIMODE_PASSWORD)
		{
			CDialogPassword dlg;
			dlg.m_Password = m_SessionConfig.strEngiModePassword;
			if (dlg.DoModal() == IDOK)
				m_SessionConfig.strEngiModePassword = dlg.m_Password;
		}
	}
	if (m_SessionConfig.strEngiModePassword != ENGIMODE_PASSWORD)
		m_ButtonDegraded.SetCheck(0);
	else // Toggles Button State
		m_ButtonDegraded.SetCheck((m_ButtonDegraded.GetCheck() == 0)?1:0);

//	TRACE("Degraded Mode is now %s!\n", m_ButtonDegraded.GetCheck()==1?"ON":"OFF");
	FillControllersAndSockets();
}

void CFTTSPDlg::EnableCombo(CComboBox &aCB, BOOL bEnabled)
{
	switch(aCB.GetCount())
	{
	case 0:
		aCB.EnableWindow(FALSE);
		break;
	case 1:
		aCB.EnableWindow(FALSE);
		aCB.SetCurSel(0);
		break;
	default:
		aCB.EnableWindow(bEnabled);
	}
}

void CFTTSPDlg::OnChangeLbNumber() 
{
	UpdateControls();	
}

void CFTTSPDlg::OnChangeCableNumber() 
{
	UpdateControls();		
}

void CFTTSPDlg::OnChangeOpName() 
{
	UpdateControls();		
	
}

void CFTTSPDlg::OnChangeSubLotNumber() 
{
	UpdateControls();
}

void CFTTSPDlg::OnCheckSite() 
{
	int aiSocketUsed[8]={	IDC_CHECK_SITE_1,IDC_CHECK_SITE_2,IDC_CHECK_SITE_3,IDC_CHECK_SITE_4, 
							IDC_CHECK_SITE_5,IDC_CHECK_SITE_6,IDC_CHECK_SITE_7,IDC_CHECK_SITE_8};

	CButton * pCheck;
	CButton * pCheckAll;
	pCheckAll=(CButton*)GetDlgItem(IDC_CHECK_SITES_ALL);
	ASSERT_VALID(pCheckAll);
	pCheckAll->SetCheck(2);

	//update "check all in case of all off or all on
	BOOL all_true = TRUE, all_false = TRUE;
	for (int iSocketUsed=0;iSocketUsed<8;iSocketUsed++)
	{
		pCheck=(CButton*)GetDlgItem(aiSocketUsed[iSocketUsed]);
		ASSERT_VALID(pCheck);
		all_true &= pCheck->GetCheck();
		all_false &= ! pCheck->GetCheck();
	}
	if (all_true)
		pCheckAll->SetCheck(1);
	if (all_false)
		pCheckAll->SetCheck(0);

	// TODO: Add your control notification handler code here
	UpdateControls();
}

void CFTTSPDlg::OnCheckAllSites()
{
	int aiSocketUsed[8]={	IDC_CHECK_SITE_1,IDC_CHECK_SITE_2,IDC_CHECK_SITE_3,IDC_CHECK_SITE_4, 
							IDC_CHECK_SITE_5,IDC_CHECK_SITE_6,IDC_CHECK_SITE_7,IDC_CHECK_SITE_8};

	CButton * pCheck;
	CButton * pCheckAll;
	pCheckAll=(CButton*)GetDlgItem(IDC_CHECK_SITES_ALL);
	ASSERT_VALID(pCheckAll);
	if (pCheckAll->GetCheck() == 2)
		pCheckAll->SetCheck(0);
	for (int iSocketUsed=0;iSocketUsed<8;iSocketUsed++)
	{
		pCheck=(CButton*)GetDlgItem(aiSocketUsed[iSocketUsed]);
		ASSERT_VALID(pCheck);
		pCheck->SetCheck(pCheckAll->GetCheck());
	}
	
	UpdateControls();
}

void CFTTSPDlg::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}

void CFTTSPDlg::ShowHideEngiOptions(BOOL bShowHide)
{
	RECT recWin, recEngiOpt, recMode;
	int rectHeight;

	this->GetWindowRect(&recWin);
	m_EngiFrame.GetWindowRect(&recEngiOpt);
	this->ScreenToClient(&recEngiOpt);
	m_Mode.GetWindowRect(&recMode);
	this->ScreenToClient(&recMode);

	rectHeight = recEngiOpt.bottom - recEngiOpt.top + 10;

	if (bShowHide)
	{ // Show
		CDialogPassword dlg;
		dlg.m_Password = _T("");
 		if (dlg.DoModal() == IDOK)
		{
			if (dlg.m_Password != ENGIMODE_PASSWORD)
			{
				m_bEngiOptCheck = FALSE;
				UpdateData(FALSE);
				return;
			}
		}
		else
		{
			m_bEngiOptCheck = FALSE;
			UpdateData(FALSE);
			return;
		}

		m_EngiFrame.ShowWindow(TRUE);
		recWin.bottom += rectHeight;
		recMode.top += rectHeight;
		recMode.bottom += rectHeight;
	}
	else
	{ // Hide
		m_EngiFrame.ShowWindow(FALSE);
		recWin.bottom -= rectHeight;
		recMode.top -= rectHeight;
		recMode.bottom -= rectHeight;
	}

	m_Mode.MoveWindow(&recMode);
	this->MoveWindow(&recWin);
}

void CFTTSPDlg::OnCheckEngiOpt() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ShowHideEngiOptions(m_bEngiOptCheck);
}

void CFTTSPDlg::OnBrowseTpgrm() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);

	if (dlg.DoModal() == IDOK)
	{
		if (dlg.GetPathName().Right(3) == _T("zip"))
		{
			m_EngiTestPrgm = dlg.GetPathName();
			UpdateData(FALSE);
		}
		else
		{
			MessageBox(_T("Test program must be zip file."), _T("Attention"), MB_OK);
		}
	}
}
