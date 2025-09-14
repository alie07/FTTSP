// Debug.cpp : implementation file
//

#include "stdafx.h"
#include "fttsp.h"
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDebug dialog


CDebug::CDebug(CWnd* pParent /*=NULL*/)
: CDialog(CDebug::IDD, pParent) 
{
	//{{AFX_DATA_INIT(CDebug)
	m_szDebug = _T("");
	//}}AFX_DATA_INIT
	m_bDebugInProgress=FALSE;

}

void CDebug::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDebug)
	DDX_Text(pDX, IDC_DEBUG, m_szDebug);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDebug, CDialog)
	//{{AFX_MSG_MAP(CDebug)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDebug::SetDebugStatus(BOOL bStatus)
{
	m_bDebugInProgress=bStatus;	
}

BOOL CDebug::GetDebugStatus (void)
{
	return m_bDebugInProgress;
}

void CDebug::Print(const char *szMessage, const char *szPrefixMessage)
{
	m_szDebug+=szPrefixMessage;
	m_szDebug+=" => ";
	m_szDebug+=szMessage;
	m_szDebug+="\r\n";
	UpdateData(FALSE);
	CEdit *pPrintText=(CEdit*)GetDlgItem(IDC_DEBUG);
	int s32Len=pPrintText->GetWindowTextLength()-1;
	pPrintText->SetSel(s32Len,s32Len,TRUE);
	pPrintText->PostMessage(WM_KEYDOWN,VK_END,0);
	pPrintText->SetFocus();
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CDebug message handlers
