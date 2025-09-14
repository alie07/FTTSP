/******************************************************************************/
/*                                                                            */
/*  Layer           : Api			                                          */
/*                                                                            */
/*  CLASS	        : CAboutDlg			                                      */
/*  Description     : Dialog box 'About..." implementation				      */
/*																			  */
/*                                                                            */
/*  File            : AouboutDlg.cpp			                              */
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
 * - Add commentaries
 * - Add the version mngt into the contructor 
 ******************************************************************************/

/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/
#include "stdafx.h"

/******************************************************************************/
/* INCLUSION OF OTHER MODULES HEADERS                                         */
/******************************************************************************/
#include "FTTSP.h"

/******************************************************************************/
/* INCLUSION OF OWNER HEADERS                                                 */
/******************************************************************************/
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IsShiftDown()( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCtrlDown()( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsAltDown()( (GetKeyState(VK_LMENU) & (1 << (sizeof(SHORT)*8-1))) != 0   )

/******************************************************************************/
/* CLASS IMPLEMENTATION			                                              */
/******************************************************************************/
// AboutDlg.cpp : implementation file
//
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
	
	/* Update the version textbox */
	m_strVersion.Format("%s%d.%d",VERSION_MESS,TSP_MAJOR_VERS,TSP_MINOR_VERS);
}


UINT CAboutDlg::Run(LPVOID p)
{
	CAboutDlg * me = (CAboutDlg *)p;
	me->Run();
	return 0;
}

void CAboutDlg::Run()
{
	/* List and display all DLL Handler versions */
    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 
	CListCtrl *pDLL_Sum=(CListCtrl*)GetDlgItem(IDC_LIST_DLL);
	
	CFileFind Finder;
	BOOL bWorking;
	CString szFilename;
	CString szDllPath;
	CString szVersion;
	CString szDLL_Info;
	CString szInfoToDisplay;
	int nTmp=0;

	szDllPath = ptrApp->GetHandlerDllPath();
	szDllPath +="\\";
	szFilename = szDllPath;
	szFilename += CString("*.dll");
	
	bWorking = Finder.FindFile(szFilename);

	/* List all the files in the directory.*/
	nTmp=0;
	while (bWorking)
	{
		CHandler HandlerDll;
		bWorking = Finder.FindNextFile();
		szFilename = Finder.GetFileName();
		szDLL_Info.Format("%s \t\t=> ",szFilename);
		pDLL_Sum->InsertItem(nTmp,szFilename,-1);
		pDLL_Sum->SetItemText(nTmp,1,"Wait...");
		pDLL_Sum->EnsureVisible(nTmp,FALSE);
		HandlerDll.LoadDll(szFilename);
		if ( HandlerDll.GetDllVersion(szVersion)==FALSE )
		{
			szVersion="Unknown!";
		}
		pDLL_Sum->SetItemText(nTmp,1,szVersion);
		nTmp++;
		HandlerDll.~CHandler();
	}	
	return;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_ABOUT_VERS, m_strVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_DLL_VERSION, OnGetDllVersion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAboutDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if (IsCtrlDown() && IsShiftDown() && pMsg->wParam==VK_RETURN)//VK_Jaune c'est une touche de mon Appareil cible
		{
			 pMsg->wParam = 0;//La touche ne sera plus geree.
			 CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 
			 ::PostMessage(ptrApp->m_hWnd, WM_DEBUG, 0, 0);
			 return true;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CAboutDlg::OnGetDllVersion() 
{
	/* Hide the button */
	CButton *pButton=(CButton*)GetDlgItem(IDC_DLL_VERSION);
	pButton->EnableWindow(FALSE);
	pButton->ShowWindow(FALSE);
	/* Show the edit box of DLL version summary */
	CListCtrl *pDLL_Sum=(CListCtrl*)GetDlgItem(IDC_LIST_DLL);
	RECT Dimmension;
	pDLL_Sum->GetWindowRect(&Dimmension);
	int nLenghtFirstCol=int ((Dimmension.right-Dimmension.left)/3);
	pDLL_Sum->InsertColumn(0,"DLL",LVCFMT_LEFT,(nLenghtFirstCol*2)-10);
	pDLL_Sum->InsertColumn(1,"VERSION",LVCFMT_LEFT,nLenghtFirstCol-10);
	pDLL_Sum->EnableWindow(TRUE);
	pDLL_Sum->ShowWindow(TRUE);
	/* Start the proccess od reading version */
	m_pThread = AfxBeginThread(Run,this);
}
