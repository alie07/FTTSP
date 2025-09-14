// SessionSummary.cpp: implementation of the CSessionSummary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fttsp.h"
#include "SessionSummary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSessionSummary::CSessionSummary()
{

}

CSessionSummary::~CSessionSummary()
{

}

BOOL CSessionSummary::Open(const CSessionConfiguration &aSC, int nSessionNumber)
{
	if (! CSession::Open(aSC, nSessionNumber))
		return FALSE;

	m_fSummary.Abort();

	m_sProbeParam.Empty();
	//m_mapBinDesc.RemoveAll();

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
	
	sTempPath.Format("%s-%s-%s-%s-%s-S%03d-fts.txt",
					aSC.strLotNumber,
					aSC.strPartID,
					aSC.GetFlow(), //!!20200403:LBs:Added flow in file name
					Date,
					Time,
					nSessionNumber);

	m_sSessionFilePath += "\\" + sTempPath;
	
	CFileException e;
	if (! m_fSummary.Open(sTempPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeText, &e))
	{
		return FALSE;
	}

	m_mapSoftCounts.RemoveAll();
	m_mapHardCounts.RemoveAll();
	m_nSktCount = aSC.waSkts.GetSize();
	m_dwTotal = 0;

	WriteHeader(aSC);
	return TRUE;
}

BOOL CSessionSummary::AddTest(const CArray<CSocketTest, CSocketTest&> &aST)
{
	if (! CSession::AddTest(aST))
		return FALSE;

	CString sTemp;
	int nSktCount = aST.GetSize();

	CObject *anObject = NULL;
	for (int nSkt=0; nSkt<nSktCount; nSkt++)
	{
		CSocketTest &aUISkt = aST[nSkt];
		if (aUISkt.m_bEnabled)
		{
			m_dwTotal ++;
			if (m_mapSoftCounts.Lookup(aUISkt.m_nBin, anObject))
			{
				CDWordArray * pDWA = (CDWordArray*)anObject;
				ASSERT(pDWA->IsKindOf(RUNTIME_CLASS(CDWordArray)));
				pDWA->SetAt(nSkt, pDWA->GetAt(nSkt) + 1);
			}
			else
			{
				CDWordArray* pDWA = new CDWordArray();
				pDWA->SetSize(nSktCount);

				for (int localSkt = 0; localSkt < nSktCount; localSkt++)
					pDWA->SetAt(localSkt, 0);

				pDWA->SetAt(nSkt, 1);
				m_mapSoftCounts.SetAt(aUISkt.m_nBin, pDWA);
			}
			if (m_mapHardCounts.Lookup(aUISkt.m_nBinHard, anObject))
			{
				CDWordArray * pDWA = (CDWordArray*)anObject;
				ASSERT(pDWA->IsKindOf(RUNTIME_CLASS(CDWordArray)));
				pDWA->SetAt(nSkt, pDWA->GetAt(nSkt) + 1);
			}
			else
			{
				CDWordArray* pDWA = new CDWordArray();
				pDWA->SetSize(nSktCount);

				for (int localSkt = 0; localSkt < nSktCount; localSkt++)
					pDWA->SetAt(localSkt, 0);

				pDWA->SetAt(nSkt, 1);
				m_mapHardCounts.SetAt(aUISkt.m_nBinHard, pDWA);
			}
		}
	}

	return TRUE;
}

BOOL CSessionSummary::AddBinDescription(const CString &sDesc)
{
	if (! CSession::AddBinDescription(sDesc))
		return FALSE;

	int nStart, nEnd;

	nStart = sDesc.Find("BINDESCXFER");
	nEnd = sDesc.Find("ENDBINDESCXFER");

	if ((nStart == -1) || (nEnd == -1))
		return FALSE;

	LONG nBin;
	CString sTemp = sDesc.Mid(nStart + 11, nEnd - nStart - 12);
	CString sName;
	if (! CStringUtils::GetChunk(sTemp, nBin,','))
		return FALSE;

	if (! CStringUtils::GetChunk(sTemp, sName, ','))
		return FALSE;

	m_mapBinDesc.SetAt(nBin, sName);
	return TRUE;
}

BOOL CSessionSummary::AddProbeParam(const CString &sProbeParam)
{
	if (! CSession::AddProbeParam(sProbeParam))
		return FALSE;

	int nStart, nEnd;

	nStart = sProbeParam.Find("PROBEPARAM");
	nEnd = sProbeParam.Find("ENDPROBEPARAM");

	if ((nStart == -1) || (nEnd == -1))
		return FALSE;

	m_sProbeParam = sProbeParam.Mid(nStart + 10, nEnd - nStart - 11);
	m_sProbeParam.TrimLeft();
	m_sProbeParam.TrimRight();
	return TRUE;
}

int CSessionSummary::compare (const void *arg1, const void *arg2 )
{
	// Argument should be WORDs
	return (int) *((WORD *) arg1) - *((WORD*) arg2);
}

BOOL CSessionSummary::Close()
{
	if (! CSession::Close())
		return FALSE;

	CFileStatus fStatus;
	if (! m_fSummary.GetStatus(fStatus))
	{
		// Try to reopen in case of connection lost then reestablished
		m_fSummary.Close(); // Avoid violation access for re-open
		if (! m_fSummary.Open(m_sSessionFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeText))
		{
			AfxMessageBox("Session summary file cannot be opened, data will be lost!", MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
		m_fSummary.SeekToEnd();
	}

	CString sTemp;
	CTime t = CTime::GetCurrentTime();
	sTemp.Format("%20s: %s\n", "STOP DATE/TIME", t.Format("%Y-%m-%d %H:%M:%S")); m_fSummary.WriteString(sTemp);

	// Hard Bin dump
	{
		sTemp = CString('.', 8+9*(m_nSktCount + 1)+7); m_fSummary.WriteString(sTemp);
		m_fSummary.WriteString("\n       SUMMARY OF HARDWARE BINS\n");
		m_fSummary.WriteString("HardBin ");
		for (int nSkt=0; nSkt<m_nSktCount; nSkt++)
		{
			sTemp.Format(" Site %02d ", nSkt);
			m_fSummary.WriteString(sTemp);
		}
		m_fSummary.WriteString("  TOTALS Yield\n");

		sTemp = CString('-', 7);
		for (nSkt=0; nSkt<=m_nSktCount; nSkt ++)
			sTemp += " " + CString('-', 8);
		sTemp += "--------\n";
		m_fSummary.WriteString(sTemp);

		CDWordArray dwaTotal, dwaPass, dwaFail;

		int nCount = m_mapHardCounts.GetCount();
		if (nCount > 0)
		{
			POSITION pos = m_mapHardCounts.GetStartPosition();
			WORD * saOrderedHardCounts = new WORD [nCount];
			WORD rKey;
			CObject *pObject;
			int index = 0;
			while (pos)
			{
				m_mapHardCounts.GetNextAssoc(pos, rKey, pObject);
				saOrderedHardCounts[index++] = rKey;
			}
			qsort((void*) saOrderedHardCounts, (size_t)nCount, sizeof (WORD), CSessionSummary::compare);


			dwaTotal.SetSize(m_nSktCount+1);
			dwaPass.SetSize(m_nSktCount+1);
			dwaFail.SetSize(m_nSktCount+1);
			for (index = 0; index < nCount; index++)
			{
				DWORD dwLineTotal = 0;
				CDWordArray *pDWA;
				rKey = saOrderedHardCounts[index];
				m_mapHardCounts.Lookup(rKey, pObject);
				pDWA = (CDWordArray*) pObject;
				ASSERT(pDWA->IsKindOf(RUNTIME_CLASS(CDWordArray)));
				ASSERT(pDWA->GetSize() == m_nSktCount);

				sTemp.Format(" %5d  ", rKey); m_fSummary.WriteString(sTemp);
				for (int nSkt=0; nSkt<m_nSktCount; nSkt++)
				{
					dwLineTotal += pDWA->GetAt(nSkt);
					dwaTotal[nSkt] += pDWA->GetAt(nSkt);
					// This is the count of pass bin
					if (rKey == 1)
						dwaPass[nSkt] += pDWA->GetAt(nSkt);
					else
						dwaFail[nSkt] += pDWA->GetAt(nSkt);

					sTemp.Format(" %7d ", pDWA->GetAt(nSkt)); m_fSummary.WriteString(sTemp);
				}

				sTemp.Format(" [%6d %5.1f]\n", dwLineTotal, 100.0*dwLineTotal/m_dwTotal); m_fSummary.WriteString(sTemp);
				dwaTotal[nSkt] += dwLineTotal;
				if (rKey == 1)
					dwaPass[nSkt] += dwLineTotal;
				else
					dwaFail[nSkt] += dwLineTotal;
				delete pObject;
			}
			delete [] saOrderedHardCounts;
		}

		sTemp = CString(' ', 7);
		for (nSkt=0; nSkt<=m_nSktCount; nSkt ++)
			sTemp += " " + CString('-', 8);
		sTemp += "--------\n";
		m_fSummary.WriteString(sTemp);
		m_fSummary.WriteString("TESTED  ");
		if (nCount > 0)
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", dwaTotal[nSkt]);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5.1f]]", dwaTotal[nSkt], 100.0 * dwaTotal[nSkt] / m_dwTotal); m_fSummary.WriteString(sTemp);
		}
		else
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", 0);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5s]]", 0, "NaN"); m_fSummary.WriteString(sTemp);
		}

		m_fSummary.WriteString("\n");
		m_fSummary.WriteString("PASSED  ");
		if (nCount > 0)
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", dwaPass[nSkt]);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5.1f]]", dwaPass[nSkt], 100.0 * dwaPass[nSkt] / m_dwTotal); m_fSummary.WriteString(sTemp);
		}
		else
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", 0);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5s]]", 0, "NaN"); m_fSummary.WriteString(sTemp);
		}

		m_fSummary.WriteString("\n");
		m_fSummary.WriteString("FAILED  ");
		if (nCount > 0)
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", dwaFail[nSkt]);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5.1f]]", dwaFail[nSkt], 100.0 * dwaFail[nSkt] / m_dwTotal); m_fSummary.WriteString(sTemp);
		}
		else
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", 0);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5s]]", 0, "NaN"); m_fSummary.WriteString(sTemp);
		}

		m_fSummary.WriteString("\n");
		// All Counts have been deleted, finish the cleanup
		m_mapHardCounts.RemoveAll();
	}

	// Soft Bin dump
	{
		sTemp = CString('.', 8+9*(m_nSktCount + 1) + 27); m_fSummary.WriteString(sTemp);
		m_fSummary.WriteString("\n       SUMMARY OF SOFTWARE BINS\n");
		m_fSummary.WriteString("SoftBin ");
		for (int nSkt=0; nSkt<m_nSktCount; nSkt++)
		{
			sTemp.Format(" Site %02d ", nSkt);
			m_fSummary.WriteString(sTemp);
		}
		m_fSummary.WriteString("  TOTALS Yield  BinName\n");

		sTemp = CString('-', 7);
		for (nSkt=0; nSkt<=m_nSktCount; nSkt ++)
			sTemp += " " + CString('-', 8);
		sTemp += "--------\n";
		m_fSummary.WriteString(sTemp);

		CDWordArray dwaTotal;

		int nCount = m_mapSoftCounts.GetCount();
		if (nCount > 0)
		{
			POSITION pos = m_mapSoftCounts.GetStartPosition();
			WORD * saOrderedSoftCounts = new WORD [nCount];
			WORD rKey;
			CObject *pObject;
			int index = 0;
			while (pos)
			{
				m_mapSoftCounts.GetNextAssoc(pos, rKey, pObject);
				saOrderedSoftCounts[index++] = rKey;
			}
			qsort((void*) saOrderedSoftCounts, (size_t)nCount, sizeof (WORD), CSessionSummary::compare);


			dwaTotal.SetSize(m_nSktCount+1);
			for (index = 0; index < nCount; index++)
			{
				DWORD dwLineTotal = 0;
				CDWordArray *pDWA;
				rKey = saOrderedSoftCounts[index];
				m_mapSoftCounts.Lookup(rKey, pObject);
				pDWA = (CDWordArray*) pObject;
				ASSERT(pDWA->IsKindOf(RUNTIME_CLASS(CDWordArray)));
				ASSERT(pDWA->GetSize() == m_nSktCount);

				sTemp.Format(" %5d  ", rKey); m_fSummary.WriteString(sTemp);
				for (int nSkt=0; nSkt<m_nSktCount; nSkt++)
				{
					dwLineTotal += pDWA->GetAt(nSkt);
					dwaTotal[nSkt] += pDWA->GetAt(nSkt);
					sTemp.Format(" %7d ", pDWA->GetAt(nSkt)); m_fSummary.WriteString(sTemp);
				}

				CString sBinName;
				if (m_mapBinDesc.Lookup(rKey, sBinName))
				{
					sBinName = "(" + sBinName + ")";
				}
				sTemp.Format(" [%6d %5.1f] %s\n", dwLineTotal, (double) 100.0 * dwLineTotal / m_dwTotal, sBinName); m_fSummary.WriteString(sTemp);
				dwaTotal[nSkt] += dwLineTotal;
				delete pObject;
			}
			delete [] saOrderedSoftCounts;
		}


		sTemp = CString(' ', 7);
		for (nSkt=0; nSkt<=m_nSktCount; nSkt ++)
			sTemp += " " + CString('-', 8);
		sTemp += "--------\n";
		m_fSummary.WriteString(sTemp);
		m_fSummary.WriteString("TESTED  ");
		if (nCount > 0)
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", dwaTotal[nSkt]);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5.1f]]", dwaTotal[nSkt], (double)100.0 * dwaTotal[nSkt]/m_dwTotal); m_fSummary.WriteString(sTemp);
		}
		else
		{
			for (nSkt=0; nSkt<m_nSktCount; nSkt++)
			{
				sTemp.Format(" %7d ", 0);
				m_fSummary.WriteString(sTemp);
			}
			sTemp.Format("[[%6d %5s]]", 0,"NaN"); m_fSummary.WriteString(sTemp);
		}

		m_fSummary.WriteString("\n");
		// All Counts have been deleted, finish the cleanup
		m_mapSoftCounts.RemoveAll();
	}

	m_fSummary.Close();
	return TRUE;
}

void CSessionSummary::WriteHeader(const CSessionConfiguration &aSC)
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();
	CString sTemp;
	
	CString strDllVers;
	ptrApp->m_Handler.GetDllVersion(strDllVers);
	CString strDllName;
	strDllName =ptrApp->m_SessionConfig.strHandlerDll;
	//Calculate the number of socket used 
	int iNbSockets=0;
	int iTmp=aSC.iTestMask;
	while (iTmp&(int)1)
	{
		iNbSockets++;
		iTmp>>=1;
	}

//!!08042021:LBs:Added handler id line

	sTemp.Format("%20s: %d.%d\n"		,"SUMMARY VERSION"		,SUM_MAJOR_VERS,SUM_MINOR_VERS)						;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: FTTSP V%d.%d\n"	,"TSP VERSION"			,TSP_MAJOR_VERS,TSP_MINOR_VERS)						;m_fSummary.WriteString(sTemp);	
	sTemp.Format("%20s: Rev %s\n"		,"SETUP REVISION"		,aSC.GetSetupRev())									;m_fSummary.WriteString(sTemp);	
	sTemp.Format("%20s: %s\n"			,"SUBCONTRACTOR"		,ptrApp->m_pSubGateway->szGetSubcontractor())		;m_fSummary.WriteString(sTemp);	
	sTemp.Format("%20s: %s\n"			,"TEST PROGRAM"			,aSC.GetProgramName())								;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"FLOW"					,aSC.GetFlow())										;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"LOT #"				,aSC.strLotNumber)									;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"SUBCONTRACTOR LOT #"	,aSC.strSubLotNumber)								;m_fSummary.WriteString(sTemp);	
	sTemp.Format("%20s: %s\n"			,"PRODUCT"				,aSC.GetProduct())									;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"REV"					,aSC.GetRevDesign())								;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"PART ID #"			,aSC.GetPartID())									;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"PRODUCT CODE"			,aSC.GetProdCode())									;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %d sites\n"		,"SOCKET(S) #"			,iNbSockets)										;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %+.0fC\n"		,"TEMPERATURE"			,aSC.dTemp)											;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"TESTER"				,::GetPCName())										;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"HANDLER"				,aSC.GetHandler())									;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"HANDLER ID"			,aSC.GetHandlerId())								;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"HANDLER DLL NAME"		,strDllName)										;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"HANDLER DLL VERSION"	,strDllVers)										;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s - %s\n"		,"LOADBOARD"			,aSC.GetLoadBoard()
																,aSC.GetLoadBoardNumber())							;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s - %s\n"		,"CABLE"				,aSC.GetCable()
																,aSC.GetCableNumber())								;m_fSummary.WriteString(sTemp);
	sTemp.Format("%20s: %s\n"			,"OPERATOR" 			,aSC.GetOperator())									;m_fSummary.WriteString(sTemp);

	CTime t = CTime::GetCurrentTime();
	sTemp.Format("%20s: %s\n"			,"START DATE/TIME"		,t.Format("%Y-%m-%d %H:%M:%S"))						; m_fSummary.WriteString(sTemp);

	m_fSummary.Flush();
}
