// Handler.cpp: implementation of the CHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTTSP.h"
#include "Handler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHandler::CHandler():
	genHandle(NULL),
	HndBin(NULL),
	HndInit(NULL),
	HndPoll(NULL),
	HndGet(NULL),
	HndSet(NULL),
	HndRelease(NULL),
	m_hWnd(NULL),
	m_HndId(NULL),
	m_bRunning(FALSE),
	m_bIsOnLine(FALSE),
	m_HndIsRunning(FALSE)
{
}

CHandler::~CHandler()
{
	if (m_HndId != 0)
	{
		HndRelease(m_HndId);
		m_HndId = 0;
	}
	FreeLibrary(genHandle);
}

HANDLE hPipeOutIn, hPipeOutOut;

BOOL CHandler::LoadDll(LPCSTR dllname)
{
	CString handlerdllFile;
    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 

	handlerdllFile = ptrApp->GetHandlerDllPath();
	if (handlerdllFile.IsEmpty())
		return FALSE;

	handlerdllFile += CString("\\") + dllname;

//	TRACE("Search Dll in: '%s'\n", handlerdllFile);

	if (m_HndId != 0) 
	{
		HndRelease(m_HndId);
		m_HndId = 0;
	}
	FreeLibrary(genHandle);

	//AllocConsole(); // Creates stdin, stdout & stderr for the library
	//CreatePipe(&hPipeOutOut, &hPipeOutIn, NULL, 2048);
	//SetStdHandle(STD_OUTPUT_HANDLE, hPipeOutIn);
	//Sleep(3000);

#ifdef _DEBUG_HANDLER_DLL
	AfxMessageBox("DLL DEBUG MODE");
	handlerdllFile="..\\Rasco_SO1000_TTL\\Debug\\Rasco_SO1000_TTL.dll";
#endif
	genHandle = LoadLibrary(handlerdllFile);

	//FreeConsole();

	if (genHandle == NULL)
	{
		CString theError;
		theError.Format(IDS_ERROR_DLL_OPEN, handlerdllFile);
		SetLastErrorDescription(theError);
		return FALSE;
	}

	CString n; // Last Name retrieved from Dll
	// Load function name with address mapped from gendll
	if (NULL == (HndInit	= (LPHNDINIT)		GetProcAddress(genHandle, (n="HndInit"   ))) ||
		NULL == (HndPoll	= (LPHNDPOLL)		GetProcAddress(genHandle, (n="HndPoll"   ))) ||
		NULL == (HndBin		= (LPHNDBIN)		GetProcAddress(genHandle, (n="HndBin"    ))) ||
		NULL == (HndGet		= (LPHNDGET)		GetProcAddress(genHandle, (n="HndGet"    ))) ||
		NULL == (HndSet		= (LPHNDSET)		GetProcAddress(genHandle, (n="HndSet"    ))) ||
		NULL == (HndRelease	= (LPHNDRELEASE)	GetProcAddress(genHandle, (n="HndRelease"))) 
		)
	{
		CString theError;
		theError.Format(IDS_ERROR_DLL_OPEN, handlerdllFile + "': Function '" +n);
		SetLastErrorDescription(theError);
		return FALSE;
	}

	/* Check if it is possible to get back the DLL version */
	 if ( (HndGetVers = (LPHNDGETVERS)	GetProcAddress(genHandle, (n="HndGetVers"))) !=NULL)
	 {
		/* YES */
		/* Read  and save the DLL version */
		HndGetVers(&m_u8MajorVersion,&m_u8MinorVersion);
		m_bVersionAvailiable=TRUE;
	 }
	 else
	 {
		/* NO */
		m_bVersionAvailiable=FALSE;
	 }

	return TRUE;
}

BOOL CHandler::GetDllVersion(CString &strVers)
{
	if (m_bVersionAvailiable==TRUE)
	{
		strVers.Format("%d.%d",m_u8MajorVersion ,m_u8MinorVersion);
	}
	else
	{
		strVers.Format(IDS_ERROR_VERSION_DLL);	
	}
	return m_bVersionAvailiable;	
}

BOOL CHandler::SetOnLine(BOOL bOnLine)
{
	if (bOnLine)
	{
		// Is Handler already on line?
		if (m_bIsOnLine)
			return TRUE;

		if (genHandle == NULL)
			return FALSE;

		m_bRunning = TRUE;
		m_bIsOnLine = TRUE;

		m_pThread = AfxBeginThread(Run,this);
		return (m_pThread != NULL);
	}
	else
	{
		m_bIsOnLine = FALSE;
		return TRUE;
	}
}

UINT CHandler::Run(LPVOID p)
{
	CHandler * me = (CHandler *)p;
	me->Run();
	return 0;
}

void CHandler::Run() 
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();

	DWORD SktMask=0;

	// TESTER IS NEXTEST
	if(ptrApp->m_SessionConfig.GetTesterType() != "PC")
	{
		while(m_bIsOnLine)
		{
			// Handler is ready ?
			SetTimer(m_hWnd,ID_CLOCK_TIMER,HANDLER_TIMEOUT,NULL); //Start timer to chect handler timeout 
			if(!HndPoll(m_HndId, m_dwSockets, &SktMask))
			{
				/* The DLL manages the asynchronism of the handler (when needed).
				 * Then it is possible to exit the HndPoll with timeout.
				 * Check if Error on GPIB OR SktMask not as expected. 
				 */
			}

			KillTimer(m_hWnd,ID_CLOCK_TIMER); //Stop handler timeout
			if(SktMask == 0) continue;

			/* RTM */
			if (m_bRTM)
				oRTM.set_active_site(SktMask);

			// Prepare site mask and send SOT
			::PostMessage(m_hWnd, WM_HNDREADY, 0, (LPARAM) SktMask);
			if(WaitForSingleObject(m_hTestStart, m_dwTestTimeOut) == WAIT_TIMEOUT)	break;
			if(WaitForSingleObject(m_hTestEnd, m_dwTestTimeOut) == WAIT_TIMEOUT)		break;

			// FORMAT BIN & MAPCOD
			::PostMessage(m_hWnd, WM_TESTCOMPLETED, 0, 0);
			BOOL bTimeout = FALSE;
			while ((WaitForSingleObject(m_hTestDone, m_dwTestTimeOut) == WAIT_TIMEOUT))
			{
				if (! m_bNetworkConnectLoop)
				{
					bTimeout = TRUE;
					break;
				}
				else
					Sleep(100);
			}
			if (bTimeout) break; // timeout reached without connection loop
			
			// SEND BIN TO HANDLER & TEST TIME TO MAIN WINDOW
			HndBin(m_HndId, m_dwSockets, m_pBins);
			::PostMessage(m_hWnd, WM_HNDTESTCOMPLETED, 0, 0);

			/* RTM */
			if (m_bRTM)
				oRTM.update_reg(m_dwSockets, m_pBins);

		}
	}
	// TESTER IS A PC
	else
	{
		while(m_bIsOnLine)
		{
			SetTimer(m_hWnd,ID_CLOCK_TIMER,HANDLER_TIMEOUT,NULL); //Start timer to check handler timeout 
			if(!HndPoll(m_HndId, m_dwSockets, &SktMask)) break;
			KillTimer(m_hWnd,ID_CLOCK_TIMER); //Stop handler timeout
			//20150408:LBo:Remove the "continue" instruction $LABS-2534
			if(SktMask != 0)
			{			
				::SendMessage(m_hWnd,WM_RUNPERSO, 0, 0);

				//SEND BIN TO HANDLER & TEST TIME TO MAIN WINDOW
				HndBin(m_HndId, m_dwSockets, m_pBins);										
				::PostMessage(m_hWnd, WM_HNDTESTCOMPLETED, 0, 0);
			}
		}
	}

	KillTimer(m_hWnd,ID_CLOCK_TIMER); // Stop handler timeout

	CHandler::Status aStatus;

	if (m_bIsOnLine)
		aStatus = TIMEOUT_TEST;
	else
		aStatus = NORMAL_EXIT;

	m_bRunning = FALSE;
	::PostMessage(m_hWnd, WM_HNDFINISHED, 0, (LPARAM) aStatus);
}

inline BOOL CHandler::IsRunning(void)
{
	return m_bRunning;
}

int CHandler::Init(LPCTSTR param)
{
	if (HndInit != NULL)
	{
		m_HndId = HndInit(param);
		if (m_HndId == 0)
		{
			CString theError;
			theError.Format(IDS_ERROR_HND_INIT);
			SetLastErrorDescription(theError);
			return 0;
		}
		else
		{
			if (m_bRTM)
				m_bRTMInit = oRTM.init_reg();

			return TRUE;
		}
	}
	return 0;
}

