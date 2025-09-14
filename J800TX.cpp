// J800TX.cpp: implementation of the CJ800TX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "J800TX.h"
#include "TX.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJ800TX::CJ800TX()
{
	char * tx_path;

	tx_path = getenv("TXPATH");
	m_TXPath = CString(tx_path);

	if(m_TXPath != "")
	{
		char *release = strrchr(m_TXPath, '\\');
		m_tx_release = CString(release+1);
	}
	else
		m_tx_release = "0";
}

CJ800TX::~CJ800TX()
{

}

//Get installation path from environment variable
CString CJ800TX::GetInstallPath(void)
{
	return DEFAULT_TSTPRGM_PATH_J800;
}

//Find the TestXentral release used for Javelin tester from the path that looks like "C:\TestXentral\m1.5.8"
CString CJ800TX::TestXentralRelease()
{
	return(m_tx_release);
}

CString CJ800TX::GetTXCmd(BOOL bEngiMode)
{
	CString theCmd;
	CString strCurRelease;

//	strCurRelease = GetCurrentRelease();
//	if (strCurRelease.IsEmpty())
//		return CString();

//	strCurRelease=TestXentralRelease(); //Find the release used
//	if(operator !=(strCurRelease,"NOT_FOUND"))
//		strCurRelease += "\\Utils\\";


//	SetCurrentDirectory(strCurRelease);
//	char IPcmd[512]="GetIpAddrs.exe > C:\\TspFile\\CtrIpAddress.txt";
//	if(system(IPcmd)!=0)  
//	{
//		CString Message;
//		Message.Format("GetIpAddrs.exe ERROR   : %s",IPcmd);
//		AfxMessageBox(Message,MB_OK);
//		return "";
//	}

	//Retrieve the IP address for all sites controller
//	CStringArray Ip_Site;
//	if(!RetrieveCtrlAddress(&Ip_Site)) return "";

//	if(m_nCtrls > Ip_Site.GetSize() )
//	{
//		CString Message;
//		Message.Format("%d Controller to load and only %d Controller inside tester !!!",m_nCtrls,Ip_Site.GetSize() );
//		AfxMessageBox(Message,MB_OK);
//		return "";
//	}

	//Initialize command line with TestXentral path
	theCmd = m_TXPath;

	theCmd += "\\Bin\\TestXentral.exe";

	//Add program to be loaded
	theCmd += " /ldpgm:" + m_InstalledProgramName;

	//Add "testjob"? /testjob:<testjob_name>

	//if (bEngiMode)
	//Add /prodn:y or / prodn:n ?

	return theCmd;
}
