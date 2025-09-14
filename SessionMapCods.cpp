// SessionMapCods.cpp: implementation of the CSessionMapCods class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fttsp.h"
#include "SessionMapCods.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSessionMapCods::CSessionMapCods()
{

}

CSessionMapCods::~CSessionMapCods()
{

}
BOOL CSessionMapCods::Open(const CSessionConfiguration &aSC, int nSessionNumber)
{
	if (! CSession::Open(aSC, nSessionNumber))
		return FALSE;

	m_fMapCods.Abort();

	CString sTempPath;
    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 
	
	sTempPath = ptrApp->GetSummariesPath();
	if( !CDirectory::Create(sTempPath) )
		return FALSE;

	SetCurrentDirectory(sTempPath);
	m_sSessionFilePath = sTempPath;
	
	sTempPath.Format("%s%s%s",aSC.strProduct,aSC.strRevDesign,aSC.strPartID);
	if( !CDirectory::Create(sTempPath) )
		return FALSE;
		
	SetCurrentDirectory(sTempPath);
	m_sSessionFilePath += "\\" + sTempPath;

	CTime tCurrentTime = CTime::GetCurrentTime();
	CString Date, Time;
	Date = tCurrentTime.Format("%Y%m%d");
	Time = tCurrentTime.Format("%H%M%S");
	
	sTempPath.Format("%s-%s-%s-%s-%s-S%03d-summary.txt",
					aSC.strLotNumber,
					aSC.strPartID,
					aSC.GetFlow(), //!!20200403:LBs:Added flow in file name
					Date,
					Time,
					nSessionNumber);

	m_sSessionFilePath += "\\" + sTempPath;
	
	CFileException e;
	if (! m_fMapCods.Open(sTempPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeText, &e))
	{
		return FALSE;
	}

	WriteHeader(aSC);
	return TRUE;
}

BOOL CSessionMapCods::AddTest(const CArray<CSocketTest, CSocketTest&> &aST)
{
	if (! CSession::AddTest(aST))
		return FALSE;

	// Check if handle is still alive (prevent network issue)
	CFileStatus status;
	if (! m_fMapCods.GetStatus(status))
	{
		m_fMapCods.Close(); // Avoid access violation

		// First try to reopen in case of connection micro cut
		if (! m_fMapCods.Open(m_sSessionFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeText))
		{
			// while loop for open retry
			CString sErr;
			sErr.Format(IDS_ERROR_FILE_OPEN, m_sSessionFilePath);
			sErr += "\nCheck network connection then Retry or Cancel";
			BOOL bConnected = FALSE;
			
			// Stop handler timeout
			HWND hWnd = AfxGetMainWnd()->m_hWnd;
			CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();
			ptrApp->m_Handler.m_bNetworkConnectLoop = TRUE;
			while (IDRETRY == AfxMessageBox(sErr, MB_ICONEXCLAMATION | MB_RETRYCANCEL))
			{
				if (m_fMapCods.Open(m_sSessionFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeText))
				{
					bConnected = TRUE;
					break;
				}	
			}
			ptrApp->m_Handler.m_bNetworkConnectLoop = FALSE;

			if (! bConnected)
				return FALSE;

		}
		m_fMapCods.SeekToEnd();
	}

	CString sTemp;
	int nSktCount = aST.GetSize();

	CObject *anObject = NULL;
	int endmapcod;
	for (int nSkt=0; nSkt<nSktCount; nSkt++)
	{
		CSocketTest &aUISkt = aST[nSkt];
		if (aUISkt.m_bEnabled)
		{
			endmapcod = aUISkt.m_sMapCodes.Find("ENDMAPCOD");
			sTemp.Format("%d|%d.%02d.%02d#%02x|%s\n",
				aUISkt.m_dwSerialNumber,
				aUISkt.m_nBinHard, aUISkt.m_nBin, aUISkt.m_nBinSoft,
				nSkt, (endmapcod!=-1)?(aUISkt.m_sMapCodes.Left(endmapcod)):"");
			m_fMapCods.WriteString(sTemp);
		}
	}

	m_fMapCods.Flush();

	return TRUE;
}

BOOL CSessionMapCods::AddMapCod(const CString &sNum, const CString &sName, const CString &sUnit)
{
	if (! CSession::AddMapCod(sNum, sName, sUnit))
		return FALSE;

	CString sTemp;

	sTemp.Format("MapCod[%s]:\t\"%s\"\tUnit:\t\"%s\"", sNum, sName, sUnit);
	m_mapMapCods.SetAt(sNum, sTemp);
	return TRUE;
}

BOOL CSessionMapCods::Close()
{
	CString sTemp;
	m_tStop = CTime::GetCurrentTime();

	CTimeSpan t = m_tStop - m_tStart;

	if (! CSession::Close())
		return FALSE;

	CFileStatus fStatus;
	if (! m_fMapCods.GetStatus(fStatus))
		return FALSE;

	if (! fStatus.m_size)
		return FALSE;

	sTemp.Format("TstTime:\t%s\n", t.Format("%D %H:%M:%S")); m_fMapCods.WriteString(sTemp);
	m_fMapCods.Close();
	return TRUE;
}

int CSessionMapCods::compare (const void *arg1, const void *arg2 )
{
	// Argument should be LPCTSTR representing MapCode number
	int num1, num2;
	sscanf(*(LPCSTR*) arg1, "%d", &num1);
	sscanf(*(LPCSTR*) arg2, "%d", &num2);

	return num1 - num2;
}

void CSessionMapCods::WriteHeader(const CSessionConfiguration &aSC)
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();
	
	CString sTemp;
	
	CString strDllVers;
	ptrApp->m_Handler.GetDllVersion(strDllVers);
	CString strDllName;
	strDllName =ptrApp->m_SessionConfig.strHandlerDll;

	m_tStart = CTime::GetCurrentTime();

	//Calculate the number of socket used 
	int iNbSockets=0;
	int iTmp=aSC.iTestMask;
	while (iTmp&(int)1)
	{
		iNbSockets++;
		iTmp>>=1;
	}

//!!08042021:LBs:Added handler id line
	sTemp.Format("%s\t\t: %d.%d\n"			,"SUMMARY VERSION"	,SUM_MAJOR_VERS,SUM_MINOR_VERS);				m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %d.%d\n"			,"TSP VERSION"		,TSP_MAJOR_VERS,TSP_MINOR_VERS);				m_fMapCods.WriteString(sTemp);	
	sTemp.Format("%s\t\t: %s\n"				,"SETUP REVISION"	,aSC.GetSetupRev());							m_fMapCods.WriteString(sTemp);	
	sTemp.Format("%s\t\t: %s\n"				,"SUBCONTRACTOR"	,ptrApp->m_pSubGateway->szGetSubcontractor());	m_fMapCods.WriteString(sTemp);	
	sTemp.Format("%s\t\t\t: %s\n"			,"PROGNAM"			,aSC.GetProgramName());							m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"PRBTYPE"			,aSC.GetFlow());								m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"LOT"				,aSC.strLotNumber);								m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t: %s\n"				,"SUBCONTRACTOR LOT",aSC.strSubLotNumber);							m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"PRODID"			,aSC.GetProduct()+aSC.GetRevDesign());			m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"PARTID"			,aSC.GetPartID());								m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"PRODCOD"			,aSC.GetProdCode());							m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %d\n"				,"NBRSITES"			,iNbSockets);									m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %+.0fC\n"		,"TEMPERATURE"		,aSC.dTemp);									m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %s\n"				,"TESTERNAM"		,::GetPCName());								m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"HANDLER"			,aSC.GetHandler());								m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %s\n"				,"HANDLER ID"		,aSC.GetHandlerId());							m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t: %s\n"				,"HANDLER DLL NAME"	,strDllName);									m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t: %s\n"				,"HANDLER DLL VERSION",strDllVers);									m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"LBNAM "			,aSC.GetLoadBoard());							m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t\t: %s\n"			,"LBNO "			,aSC.GetLoadBoardNumber());						m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %s\n"				,"CABLENAM "		,aSC.GetCable());								m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %s\n"				,"CABLENO "			,aSC.GetCableNumber());							m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %s\n"				,"OPERATOR "		,aSC.GetOperator());							m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %s\n"				,"SITE SEPARATOR"	,"#");											m_fMapCods.WriteString(sTemp);
	sTemp.Format("%s\t\t: %s\n"				,"START DATE"		,m_tStart.Format("%Y-%m-%d %H:%M:%S"));			m_fMapCods.WriteString(sTemp);
	

	POSITION pos = m_mapMapCods.GetStartPosition();

	CString sKey;
	CString sValue;

	LPCTSTR * saOrderedMapCodes;

	int nCount = m_mapMapCods.GetCount();
	if (nCount > 0)
	{
		saOrderedMapCodes = new LPCTSTR [nCount];

		int index = 0;
		while (pos)
		{
			m_mapMapCods.GetNextAssoc(pos, sKey, sValue);
			saOrderedMapCodes[index++] = (LPCTSTR) sKey;
		}
		qsort((void*) saOrderedMapCodes, (size_t)nCount, sizeof (LPCTSTR), CSessionMapCods::compare);

		for(index=0; index<nCount; index++)
		{
			sKey = saOrderedMapCodes[index];
			m_mapMapCods.Lookup(sKey, sValue);
			sTemp.Format("%s\n", sValue);m_fMapCods.WriteString(sTemp);
		}
		delete [] saOrderedMapCodes;
	}
}
