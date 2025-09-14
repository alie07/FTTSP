// TX.cpp: implementation of the TX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTTSP.h"
#include "TX.h"
#include "J800TX.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CTX, CObject);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTX::CTX()
{
	m_bTXStarted = FALSE;
}

CTX::~CTX()
{
	if (m_bTXStarted)
		TerminateProcess( m_pi.hProcess, 0);
}

CString CTX::GetCurrentRelease()
{
	char *release = strrchr(CString(getenv("TXPATH")), '\\');

	return(CString(release+1));
}

BOOL CTX::InstallProgram(const CString &aProgram)
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

BOOL CTX::DeleteOldProgram()
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

CTX* CTX::Create(const CString &type)
{
	if (type == "TX_J800")
		return (new CJ800TX);

	return NULL;
}

void CTX::SetControllers(int nCtrl)
{
	m_nCtrls = nCtrl;
}

BOOL CTX::Start(BOOL bEngiMode)
{
	/* Create structure for main window */
	STARTUPINFO si;
	// Ensure memory is properly cleared
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	/* Compute the cmd line to open TX with the test programm according */
	/* to the wanted tester and test program							*/
	CString TXCmd = GetTXCmd(bEngiMode);

	if(TXCmd.IsEmpty())
		return(FALSE);
	LPTSTR pTXCmd = TXCmd.GetBuffer(TXCmd.GetLength() + 1);

	/* Start the application ui with a new process */
	m_bTXStarted = CreateProcess	(	NULL, pTXCmd,
                        NULL,
						NULL,
						TRUE,
						CREATE_DEFAULT_ERROR_MODE,
						NULL,
						NULL,
						&si,
						&m_pi	);

	TXCmd.ReleaseBuffer();

	if (! m_bTXStarted)
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

	return m_bTXStarted;
}

CString CTX::GetTXCmd(BOOL bEngiMode)
{
	return CString();
}
