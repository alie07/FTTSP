// DialogPassword.cpp : implementation file
//

#include "stdafx.h"
#include "fttsp.h"
#include "DialogPassword.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPassword dialog


CDialogPassword::CDialogPassword(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPassword::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPassword)
	m_Password = _T("");
	//}}AFX_DATA_INIT
}


void CDialogPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPassword)
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPassword, CDialog)
	//{{AFX_MSG_MAP(CDialogPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPassword message handlers

BOOL CDialogPassword::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd *pWnd = GetDlgItem(IDC_PASSWORD);
	ASSERT_VALID(pWnd);
	pWnd->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
