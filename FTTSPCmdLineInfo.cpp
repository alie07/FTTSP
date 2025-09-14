// FTTSPCommandLineInfo.cpp: implementation of the CFTTSPCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTTSPCmdLineInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFTTSPCmdLineInfo::CFTTSPCmdLineInfo()
{
	/* Default values */
	m_bValidArg = TRUE;
	m_bEngiEnv = FALSE;
	m_sSetupPath = "";
}

CFTTSPCmdLineInfo::~CFTTSPCmdLineInfo()
{

}

/////////////////////////////////////////////////////////////////////
// ParseParam called x times for each switch/arguments
/////////////////////////////////////////////////////////////////////
void CFTTSPCmdLineInfo::ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
{
	CString csParam(pszParam);

	if (0 == strcmp(pszParam, "engi"))
	{ // Engi switch => /engi
		m_bEngiEnv = TRUE;
	} else if ("setup:" == csParam.Left(6))
	{ // Setup path => /setup:<path>
		m_sSetupPath = csParam.Mid(6, csParam.GetLength() - 5);
	} else {
		m_bValidArg = FALSE;
	}

}
