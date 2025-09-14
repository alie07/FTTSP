// ManualHandler.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "ManualHandler.h"

// Declare as exporting the dll
#define HANDLERDLL_EX
#include "../HandlerDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DLL_MAJOR_VERSION			((unsigned char)2)						// Major version identification
#define DLL_MINOR_VERSION			((unsigned char)1)						// Minor version identification


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CManualHandlerApp

BEGIN_MESSAGE_MAP(CManualHandlerApp, CWinApp)
	//{{AFX_MSG_MAP(CManualHandlerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManualHandlerApp construction

CManualHandlerApp::CManualHandlerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	srand((unsigned) time(NULL));
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CManualHandlerApp object

CManualHandlerApp manualHanderApp;

CString sMask;

#ifdef __cplusplus
extern "C" {
#endif

HNDID DLLDIR WINAPI HndInit ( IN LPCTSTR options )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CManualHandlerApp* dllApp = (CManualHandlerApp*)AfxGetApp();
	dllApp->dwRun_nb = 1;
	dllApp->bLoop_active = FALSE;

	AfxMessageBox("HndInit");
	return 1;
}

/******************************************************************************
 * FUNCTION NAME : HndGetVers
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Give to application the DLL version 
 ------------------------------------------------------------------------------
 * INPUT : pu8Major
    Comment : pointer to variable application to read the major version
    Range   : 0 to 255
    Unit    : unsigned char
 ------------------------------------------------------------------------------
 * INPUT : pu8Minor
    Comment : pointer to variable application to read the minor version
    Range   : 0 to 255
    Unit    : unsigned char
 ------------------------------------------------------------------------------
 * OUTPUT : Boolean
    Comment : -
    Range   : TRUE
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndGetVers (OUT PTBYTE pu8Major, OUT PTBYTE pu8Minor)
{
	*pu8Major=DLL_MAJOR_VERSION;
	*pu8Minor=DLL_MINOR_VERSION;
	return TRUE;
}

BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t mask_skts, OUT LPDWORD active_sockets )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//	DWORD mask=(rand()%16);
//	DWORD mask=0xFFFF;

//	CString str;
//	str.Format("Hnd_Poll 0x%0X", mask_skts);
//	AfxMessageBox(str);

	CManualHandlerApp* dllApp = (CManualHandlerApp*)AfxGetApp();
	dllApp->dwMask = (DWORD)mask_skts;

	HINSTANCE hMainResource;
	hMainResource = AfxGetResourceHandle();
	AfxSetResourceHandle(manualHanderApp.m_hInstance);

	if(!dllApp->bLoop_active)
	{
		CPollMaskDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			mask_skts = (unsigned int)dllApp->dwMask; // Update mask with ready socket
			//dllApp->dwRun_nb = dlg.m;
		}
	}
	else
	{
		dllApp->dwRun_nb--;
		if(dllApp->dwRun_nb == 1)
			dllApp->bLoop_active = FALSE;
	}

	AfxSetResourceHandle(hMainResource);

	*active_sockets = mask_skts;
	return 1;
}

BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t skts, IN LPDWORD bins )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CManualHandlerApp* dllApp = (CManualHandlerApp*)AfxGetApp();

	Sleep(500);
	CString str("");
	CString str2("");
/*	for (size_t i=0; i<skts; i++)
	{
		str2.Format("Socket %02d: bin %d  \n",i,bins[i]);
		str+=str2;
	}
*/
	size_t mask_skts=skts;
	int i=0;
	while (mask_skts)
	{
		if ( (mask_skts&0x0001)==0x0001 )
		{
			str2.Format("Socket %02d: bin %d  \n",i,bins[i]);
			str+=str2;
		}
		i++;
		mask_skts>>=1;
	}


	//str.Format("Hnd_Bin %d %d %d %d", bins[0], bins[1], bins[2], bins[3]);
	if(!dllApp->bLoop_active)
		AfxMessageBox(str);
	return 1;
}

BOOL DLLDIR WINAPI HndSet ( IN HNDID hndid, IN LPCTSTR command )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AfxMessageBox("Hnd_Set");
	return 0;
}

BOOL DLLDIR WINAPI HndGet ( IN HNDID hndid, IN size_t buffer_size, OUT LPTSTR answer )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AfxMessageBox("HndGet");
	return TRUE;
}

BOOL DLLDIR WINAPI HndRelease ( IN HNDID hndid )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AfxMessageBox("HndRelease");
	return TRUE;
}

#ifdef __cplusplus
}
#endif
