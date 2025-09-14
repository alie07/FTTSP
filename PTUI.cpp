// PTUI.cpp: implementation of the CPTUI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTTSP.h"
#include "PTUI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPTUI::CPTUI()
{

}

CPTUI::~CPTUI()
{

}

CString CPTUI::GetInstallPath()
{
//    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 
//	CString ProgDirectory = ptrApp->GetStationProgramPath();
	
//	if (ProgDirectory.IsEmpty())
//		return CString();

//	CString localPCName = GetPCName();
//	if (localPCName.IsEmpty())
//		return CString();

//	return ProgDirectory + "\\" + localPCName;

	//!!20032023:LBs:Install path now return fixed path for PT because Hang-up and errors due to network issues at UTAC
	return DEFAULT_TSTPRGM_PATH;

}

CString CPTUI::GetUICmd(BOOL bEngiMode)
{
	CString theCmd;
	
	theCmd = "ui /NOLOGO /C=clear /C=localhost";
	//theCmd = "ui /C=clear /C=localhost";

	if (m_nCtrls > 1)
	{
		theCmd += " /C=";
		theCmd += "PT-058";
	}

	theCmd += " /HT=40000 /MT=40000";
	if (bEngiMode)
		theCmd += " /E";

	theCmd += " \"" + m_InstalledProgramName + "\"";

	return theCmd;
}