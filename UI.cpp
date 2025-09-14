// UI.cpp: implementation of the CUI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTTSP.h"
#include "UI.h"
#include "HDUi.h"
#include "PTUi.h"
#include "GTUi.h"
#include "VTUi.h"
#include "TX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CUI, CObject);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUI::CUI()
{
	m_bUIStarted = FALSE;
}

CUI::~CUI()
{
	if (m_bUIStarted)
		TerminateProcess( m_pi.hProcess, 0);
}

CString CUI::GetCurrentRelease()
{
	HKEY hkCR = NULL; // Handle to Current Release Registry Key
	char lpCR[1024];
	DWORD dwSize = 1024;
	DWORD dwType;

	/* Read the version of NextTest */
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\NexTest", 0, KEY_READ, &hkCR) != ERROR_SUCCESS)
	{
		CString theError;
		theError.LoadString(IDS_ERROR_REGISTRY_READ);
		SetLastErrorDescription(theError);
		return CString();
	}

	if (RegQueryValueEx(hkCR, "CurrentRelease", NULL, &dwType, (LPBYTE) lpCR, &dwSize) != ERROR_SUCCESS)
	{
		ZeroMemory(lpCR, 1024);
	}

	if ((RegCloseKey(hkCR) != ERROR_SUCCESS) || (dwType != REG_SZ))
	{
		CString theError;
		theError.LoadString(IDS_ERROR_REGISTRY_READ);
		SetLastErrorDescription(theError);
		return CString();
	}

//	TRACE("CurrentRelease size & value: %d '%s'\n", dwSize, lpCR);
	return CString(lpCR);
}

BOOL CUI::InstallProgram(const CString &aProgram)
{
	/* Delete old tempory test program on the local station path */
	if (! DeleteOldProgram())
		return FALSE;

	/* Read the new path of the tempory test program */
	CString localStationProgramPath = GetInstallPath();
	if (localStationProgramPath.IsEmpty())
		return FALSE;

	/* Create the directory */
	if(!CreateDirectory(localStationProgramPath,NULL))
	{
		CString theError;
		theError.Format(IDS_ERROR_CREATE_DIRECTORY, localStationProgramPath);// + "\\" + localPCName);
		SetLastErrorDescription(theError);
		return FALSE;
	}

	CFileStatus status;
	CString DestProgram;

	/* Check the existence and the rights of the file */
	if (CFile::GetStatus(aProgram, status))
	{
		CFile tmpFile;

		/* Check if it is possible to open the file */
		if (! tmpFile.Open(aProgram, CFile::modeRead ))
		{
			CString theError;
			theError.Format(IDS_ERROR_FILE_OPEN, aProgram);
			SetLastErrorDescription(theError);
			return FALSE;
		}

		/* Compute the executable path after copying and unzipping the test program */
		m_InstalledProgramName = tmpFile.GetFileName();
		DestProgram = localStationProgramPath + "\\" + m_InstalledProgramName;
		m_InstalledProgramName = localStationProgramPath + "\\debug\\" + m_InstalledProgramName.Left(m_InstalledProgramName.GetLength()-4);
		m_InstalledProgramName += ".exe";

		tmpFile.Close();

		/* Copying the file into tempory directory */
		if (!CopyFile(aProgram, DestProgram, FALSE))
		{
			CString theError;
			theError.Format(IDS_ERROR_FILE_COPY, aProgram, DestProgram);
			SetLastErrorDescription(theError);
			return FALSE;
		}
	}
	else
	{
		CString theError;
		theError.Format(IDS_ERROR_FILE_NOT_FOUND, aProgram);
		SetLastErrorDescription(theError);
		return FALSE;
	}

	// Unzip Program! Get the current path of the process
	CString szPgmPath;
//	TCHAR *pszTmp= szPgmPath.GetBuffer(1024);
//	GetCurrentDirectory(MAX_PATH,pszTmp);
//	szPgmPath.ReleaseBuffer();
	szPgmPath ="unzip.exe";

	// Build the arguments 	
	CString UnzipCmd = szPgmPath + " -o \"";
	
    UnzipCmd += DestProgram;
	
	UnzipCmd += "\" -d \"";
	
	UnzipCmd += localStationProgramPath + "\"";

	if (system(UnzipCmd) != 0)
	{
		CString theError;
		theError.Format(IDS_ERROR_UNZIP, DestProgram);
		SetLastErrorDescription(theError);
		return FALSE;
	}

	return TRUE;
}

BOOL CUI::DeleteOldProgram()
{
	CString RmCmd = "rmdir /S /Q ";
	CString localStationProgramPath = GetInstallPath(); // Avoid calling it several times

	// Error occured while fetching Program Path
	if (localStationProgramPath.IsEmpty())
		return FALSE;

	RmCmd += "\"" + localStationProgramPath + "\"";

	system(RmCmd);

	if (SetCurrentDirectory(localStationProgramPath))
	{
		CString theError;
		theError.Format(IDS_ERROR_DESTROY_DIRECTORY, localStationProgramPath);
		SetLastErrorDescription(theError);
		return FALSE;
	}
	return TRUE;
}

CUI* CUI::Create(const CString &type)
{
	if (type == "MAV_PT")
		return (new CPTUI);
	else if (type == "MAV_HD")
		return (new CHDUI);
	//else if (type == "MAV_VT")
	//	return (new CVTUI);
	//else if (type == "MAV_GT")
	//	return (new CGTUI);

	return NULL;
}

void CUI::SetControllers(int nCtrl)
{
	m_nCtrls = nCtrl;
}

BOOL CUI::Start(BOOL bEngiMode)
{
	/* Create structure for main window */
	STARTUPINFO si;
	// Ensure memory is properly cleared
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	/* Compute the cmd line to open ui with the test programm according */
	/* to the wanted tester and test program							*/
	CString UICmd = GetUICmd(bEngiMode);
	if(UICmd.IsEmpty())
		return(FALSE);
	LPTSTR pUICmd = UICmd.GetBuffer(UICmd.GetLength() + 1);

	/* Start the application ui with a new process */
	m_bUIStarted = 
		CreateProcess	(	NULL, pUICmd,
                        NULL,
						NULL,
						TRUE,
						CREATE_DEFAULT_ERROR_MODE,
						NULL,
						NULL,
						&si,
						&m_pi	);

	UICmd.ReleaseBuffer();

	if (! m_bUIStarted)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		CString theError = (LPSTR) lpMsgBuf;
		LocalFree( lpMsgBuf );

		SetLastErrorDescription(theError);
	}

	return m_bUIStarted;
}

CString CUI::GetUICmd(BOOL bEngiMode)
{
	return CString();
}
