// PollMaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ManualHandler.h"
#include "PollMaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPollMaskDlg dialog


CPollMaskDlg::CPollMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPollMaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPollMaskDlg)
	//}}AFX_DATA_INIT
}


void CPollMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPollMaskDlg)
	DDX_Control(pDX, IDC_RUN_NB, m_Run_Nb);
	DDX_Control(pDX, IDC_MASK, m_sMask);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPollMaskDlg, CDialog)
	//{{AFX_MSG_MAP(CPollMaskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPollMaskDlg message handlers

BOOL CPollMaskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	CString sMask;
	CManualHandlerApp *dllApp = (CManualHandlerApp*)AfxGetApp();
	sMask.Format("0x%0X", dllApp->dwMask);
	m_sMask.SetWindowText(sMask);

	int aiSocketUsed[8] = {	IDC_CHECK0,IDC_CHECK1,IDC_CHECK2,IDC_CHECK3, 
							IDC_CHECK4,IDC_CHECK5,IDC_CHECK6,IDC_CHECK7};

	m_Run_Nb.SetWindowText("1");

	CString sToken;
	if (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)sMask, 1, _T('x')))
	{
		DWORD dwMask = dllApp->dwMask;
		for (int i=0; i<8; i++)
		{
			CButton *pSocket = (CButton*)GetDlgItem(aiSocketUsed[i]);
			pSocket->EnableWindow(FALSE);
			if ((dwMask >> i) & 0x01)
			{
				pSocket->EnableWindow();
				pSocket->SetCheck(TRUE);
			}
		}
	}

	return TRUE;
}

void CPollMaskDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	// Update global sMask variable
	int aiSocketUsed[8] = {	IDC_CHECK0,IDC_CHECK1,IDC_CHECK2,IDC_CHECK3, 
							IDC_CHECK4,IDC_CHECK5,IDC_CHECK6,IDC_CHECK7};

	DWORD dwMask = 0x00;
	for (int i=0; i<8; i++)
	{
		CButton *pSocket = (CButton*)GetDlgItem(aiSocketUsed[i]);
		if (pSocket->GetCheck())
			dwMask |= (1 << i);
	}

	CManualHandlerApp *dllApp = (CManualHandlerApp*)AfxGetApp();
	dllApp->dwMask = dwMask;

	CString s_run_nb;
	m_Run_Nb.GetWindowText(s_run_nb);
//	int run_nb = m_sRunNb.GetWindowLong();
	dllApp->dwRun_nb = atoi(s_run_nb);
	if(dllApp->dwRun_nb>1)
		dllApp->bLoop_active = TRUE;

	CDialog::OnOK();
}
