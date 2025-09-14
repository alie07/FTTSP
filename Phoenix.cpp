// Phoenix.cpp: implementation of the CPhoenix class.
//
// !!20160412:LBo:Rename file name and class name "Alcatraz" to "Phoenix" $LABS-2543
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winver.h"
#include "fttsp.h"
#include "Phoenix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CPhoenix, CObject);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPhoenix::CPhoenix(CString sHost, int nPort, CString sExePath, CString sProcessName)
{
	m_bStarted		= FALSE;
	m_bConnected	= FALSE;

	m_sExePath		= sExePath;
	m_sHost			= sHost;
	m_nPort			= nPort;
	m_sProcessName	= sProcessName;

	// Create events / pipe to communicate with FTTSP Main Window as UI does
	hvt_start		=OpenEvent(SYNCHRONIZE|EVENT_MODIFY_STATE,TRUE,"test_start");
	hvt_complete	=OpenEvent(SYNCHRONIZE|EVENT_MODIFY_STATE,TRUE,"test_complete");
	hvt_data_ready	=OpenEvent(SYNCHRONIZE|EVENT_MODIFY_STATE,TRUE,"data_ready");

	ResetEvent( hvt_start		);
	ResetEvent( hvt_complete	);
	ResetEvent( hvt_data_ready	);

	hpipe= CreateFile(  "\\\\.\\PIPE\\comm_test",     // pointer to name of the file
					GENERIC_READ|GENERIC_WRITE,       // access (read-write) mode
					FILE_SHARE_READ|FILE_SHARE_WRITE, // share mode
					NULL,							  // pointer to security attributes
					OPEN_EXISTING,                    // how to create
					FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING,            // file attributes
					NULL                              // handle to file with attributes to copy
					);
	
	if(hpipe==INVALID_HANDLE_VALUE)  // if pipe not valid
	{
		m_sTrace.Format(IDS_ERROR_PHOENIX_PIPE, "\n");
		SetLastErrorDescription(m_sTrace);
		m_bCreated = FALSE;
		return;
	}

	// Head param init
	for (int i=0; i<MAX_SOCKET; i++)
		m_sHeadParam.SetAtGrow(i, "");
	
	// Defaults
	m_hTimer = CreateWaitableTimer(NULL, TRUE, NULL); // Set waitable timer handle
	m_bRunThread = FALSE;
	m_bCreated = TRUE;
}

CPhoenix::~CPhoenix()
{
	if (m_bStarted)
		TerminateProcess( m_pi.hProcess, 0);

}

// Static function to get current release without started process based on GetFileVersionInfo
CString CPhoenix::GetCurrentRelease(CString sExePath)
{
	CString sVer;
	
	DWORD verHandle = NULL;
	UINT size = 0;
	LPBYTE lpBuffer = NULL;

	char *lpExePath = (LPSTR)(LPCSTR)sExePath;

	DWORD verSize = GetFileVersionInfoSize(lpExePath, &verHandle);

	if (!verSize)
	{
		TRACE(_T("Get file version info size failed!\n"));
		return "";
	}

	LPSTR verData = new char[verSize];

	if (!GetFileVersionInfo(lpExePath, verHandle, verSize, verData))
	{
		return "";
	}

	if (VerQueryValue(verData, "\\", (VOID FAR* FAR*)&lpBuffer, &size))
	{
		if (size)
		{
			VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
			if (verInfo->dwSignature == 0xfeef04bd)
			{
				sVer.Format( "v%d.%d.%d.%d",
						  HIWORD(verInfo->dwFileVersionMS) & 0xff, // Major
						  LOWORD(verInfo->dwFileVersionMS) & 0xff, // Minor
						  HIWORD(verInfo->dwFileVersionLS) & 0xff, // Build Major
						  LOWORD(verInfo->dwFileVersionLS) & 0xff  // Build Minor
						);

				delete[] verData;
				return sVer;
			}
		}
	}

	delete[] verData;
	delete lpExePath;
	delete lpBuffer;

	return "";
}

UINT CPhoenix::GetSiteMax()
{
	return (UINT)MAX_SOCKET;
}

// Start Phoenix process if not yet done
BOOL CPhoenix::Start(BOOL bEngiMode)
{
	CString sTrace;
	BOOL Local;

	//!!06022017:LBo:Check if we are on localhost $LABS-2699
	Local = (strcmp(m_sHost, "127.0.0.1") == 0);

	//if Phoenix is not local: don't check it's running
	if (!Local)
	{
		TRACE(_T("Phoenix is not on local machine, we suppose it's already running...\n"));
		m_bStarted = TRUE;
	}
	else
	{
		// Check if process exists 
		if (CProcessUtils::IsProcess32Running(m_sProcessName) )
		{
			TRACE(_T("Phoenix process already running...\n"));
			m_bStarted = TRUE;
		}
		else
		{
			m_sLastError.Format(IDS_PHOENIX_NOT_STARTED);
			SetLastErrorDescription(m_sLastError);
	#ifdef _TEST
			return TRUE;
	#else
			return FALSE;
	#endif

			/*************************************************************/
			/* Following code skipped. Application loading time unnknown */
			/*************************************************************/
			
			TRACE(_T("Start Phoenix process..."));

			/* Create structure for main window */
			STARTUPINFO si;
			// Ensure memory is properly cleared
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);

			/* Compute the cmd line to open ui with the test programm according */
			/* to the wanted tester and test program							*/
			CString Cmd = m_sExePath;
			LPTSTR pCmd = Cmd.GetBuffer(Cmd.GetLength() + 1);

			/* Start the application ui with a new process */
			m_bStarted = 
				CreateProcess	(	NULL, pCmd,
								NULL,
								NULL,
								TRUE,
								CREATE_DEFAULT_ERROR_MODE,
								NULL,
								NULL,
								&si,
								&m_pi	);

			Cmd.ReleaseBuffer();

			if (! m_bStarted)
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
				
				return FALSE;
			}

			delete pCmd;

		}
	}
	return m_bStarted;
}

// Return Phoenix test program installation path
CString CPhoenix::GetInstallPath()
{
	CString sInstallPath = DEFAULT_TESTPRGM_PATH;
	return sInstallPath;
}

CString CPhoenix::GetInstalledProgram()
{
	CString sProgramPath;
	//!!20160413:LBo:Removed GetInstallPath()
	sProgramPath = m_InstalledProgramName;
	return sProgramPath;
}

// InstallProgram (same name than UI::InstallProgram)
//  Install zip file in Phoenix deposit directory
BOOL CPhoenix::InstallProgram(const CString &aProgram)
{	
	CFileStatus status;
	CFile tmpFile;

	// Check if program exists in default directory
	if (CFile::GetStatus(aProgram, status))
	{
		tmpFile.Open(aProgram, CFile::modeRead);
		m_InstalledProgramName = tmpFile.GetFileName();
		tmpFile.Close();
	}
	else
	{
		m_sLastError.Format(IDS_ERROR_FILE_NOT_FOUND, aProgram);
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}

	CString DestProgram;
	DestProgram.Format("%s\\%s", DEFAULT_TESTPRGM_PATH, m_InstalledProgramName);
	m_InstalledProgramName = DestProgram;

	// Copying the file into tempory directory
	DeleteFile(DestProgram);
	if (!CopyFile(aProgram, DestProgram, FALSE))
	{
		m_sLastError.Format(IDS_ERROR_FILE_COPY, aProgram, DestProgram);
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}

	return TRUE;	
}

void CPhoenix::OnClose(int nErrorCode)
{
	if (nErrorCode==0)
	{
		m_bConnected = FALSE;
	}

	CAsyncSocket::OnClose(nErrorCode);
}

// OnConnect notification function
//  Set m_bConnected state according to nErrorCode
void CPhoenix::OnConnect(int nErrorCode)
{
	if (nErrorCode==0) // Success
	{
		TRACE("OnConnect call back. Timer signaled.\n");
		m_bConnected = TRUE;
		SetEvent(m_hTimer); // Signal timer handle
	}

	CAsyncSocket::OnConnect(nErrorCode);
}

// OnReceive notification function overriding
//  Use this callback function to process received data by calling this->Receive() function
void CPhoenix::OnReceive(int nErrorCode)
{
	if (!m_bReceived)
		m_sReceived = "";
	else //!20160413:LBo:Add space separator if m_sReceived==TRUE $LABS-2544
		m_sReceived += " ";

	if (nErrorCode==0)
	{
		LPTSTR lpBuf = new char[BYTE_RECEIVED+1];
		DWORD iReceived = this->Receive(lpBuf, BYTE_RECEIVED);

		if ((iReceived == SOCKET_ERROR) || (iReceived == 0))
		{
			DWORD iLastError = this->GetLastError();
			m_sLastError.Format("%d", iLastError);
			TRACE(_T("Receive error: %s\n"), m_sLastError);
		}
		else
		{
			CString sTemp;
			lpBuf[iReceived-2] = NULL; // Cut surplus with crlf
			sTemp.Format("%s", (char *)lpBuf);
			//!!20160413:LBo:Remove " " at beginning $LABS-2544
			m_sReceived += sTemp;
		}
		delete lpBuf;
	}

	//TRACE("Received: %s\n", m_sReceived.GetBuffer(100)); // TRACE limited to 512 characters
	//m_sReceived.ReleaseBuffer();
	m_bReceived = TRUE;
}

// OnSend notification function overriding
void CPhoenix::OnSend(int nErrorCode)
{
	if (nErrorCode==0)
	{
	
	}

	CAsyncSocket::OnReceive(nErrorCode);

}

// Return if the socket is connected
BOOL CPhoenix::IsConnected()
{
	return m_bConnected;
}

// Format Phoenix command 
//  Simple function to format command composed by command + args + \r\n
void CPhoenix::FormatCmd(LPCSTR& lpCmd, CString sCmd, CString sArg = "")
{
	sCmd += sArg + "\r\n";
	lpCmd = (LPCSTR)(LPCTSTR)sCmd;
}

// Format Phoenix command 
//  Simple function to format command composed by command + args + \r\n
void CPhoenix::FormatCmd(CString& lpCmd, CString sCmd, CString sArg = "")
{
	sCmd += sArg + "\r\n";
	lpCmd = sCmd;
}

// Try to connect to Phoenix server
//  Note that the connect is non-blocking, use OnConnect() override to perform actions
//  Return FALSE is AfxSocketInit() failed
BOOL CPhoenix::Connection()
{
	m_bConnected = FALSE;
	ResetEvent(m_hTimer);
	
	// Test already started
	if (!AfxSocketInit())
	{
		TRACE(_T("AfxSocketInit() failed!"));
		return FALSE;
	}

	m_sTrace.Format("Try to connect to Phoenix...\n");
	TRACE(m_sTrace);

	if (!this->Create())
	{
		m_sLastError.Format(IDS_ERROR_PHOENIX_CONNECT, "\n");
		AfxMessageBox(m_sLastError);
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}
		
	this->Connect(m_sHost, m_nPort);

	if (!CTimeUtils::WaitWithMsgPump(TIMEOUT_CONNECTION, m_bConnected))
	{
		m_sLastError.Format(IDS_ERROR_TIMER);
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}

	if (!m_bConnected)
	{
		m_sLastError.Format(IDS_ERROR_PHOENIX_CONNECT);
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}
	
	TRACE("Phoenix connection ok\n");
	return m_bConnected;
}

// Send command to get Phoenix ID (non-blocking function)
//  Check OnReceive() notification function to get results
BOOL CPhoenix::GetID(CString &sID)
{
	CString sCmd;
	FormatCmd(sCmd, CMD_SOFTVERSION, "");
	LPCSTR lpCmd = (LPCTSTR)(LPCSTR)sCmd;

	TRACE(_T("Get Phoenix ID...\n"));
	m_bReceived = FALSE;
	this->Send(lpCmd, strlen(lpCmd)); // Send TCP command

	// Wait for response
	if (CTimeUtils::WaitWithMsgPump(TIMEOUT_CONNECTION, m_bReceived))
	{
		if (m_sReceived.GetLength() > 0)
		{
			sID = m_sReceived;	
		}
		else
		{
			m_sLastError.Format(IDS_ERROR_PHOENIX_ID, "\n");
			SetLastErrorDescription(m_sLastError);
			return FALSE;
		}
	}
	else
	{
		m_sLastError.Format(IDS_ERROR_PHOENIX_ID, "\n");
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	};

	TRACE(_T("Phoenix ID = %s\n"), sID);
	return TRUE;
}

// Load test program (called flow in Phoenix lexic)
BOOL CPhoenix::LoadProgram(CString &sPrgm)
{
	if (!this->InstallProgram(sPrgm)) // Do copy from netwrok place (based on UI storage method).
		return FALSE;

	CString sCmd;
	BOOL bLoadOK;

	FormatCmd(sCmd, CMD_LOAD_PRGM, m_InstalledProgramName);
	LPCSTR lpCmd = (LPCTSTR)(LPCSTR)sCmd;

	TRACE(_T("Load Program...\n"));
	m_bReceived = FALSE;
	this->Send(lpCmd, strlen(lpCmd)); // Send TCP command

	TRACE(_T("Wait FLOWOK...\n"));
	if (!CTimeUtils::WaitWithMsgPump(PRGM_TIMEOUT, m_bReceived)) // Wait for response FLOWOK
		bLoadOK = FALSE;

	m_bReceived = FALSE; // Reset
	if (m_sReceived.Find(ANS_FLOW_OK)==-1)
		bLoadOK = FALSE;

	TRACE(_T("Wait FLOWLOADED...\n"));
	if (!CTimeUtils::WaitWithMsgPump(PRGM_TIMEOUT, m_bReceived)) // Wait for FLOWLOADED
			bLoadOK = FALSE;
	// !!20170215:LBo:"FLOWLOADED" is now checked $LABS-2706
	else if (m_sReceived.Find(ANS_FLOW_LOADED)==-1)
		bLoadOK = FALSE;
		
	if (!bLoadOK)
	{
		m_sLastError.Format(IDS_ERROR_PHOENIX_PRGM, "\n");
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}
	
	return TRUE;
}

// Get params list
BOOL CPhoenix::GetParams(CStringArray &ParamsMap)
{
	CString sCmd;
	BOOL bParamOK = TRUE;

	FormatCmd(sCmd, CMD_RETURN_PARAM, "");
	LPCSTR lpCmd = (LPCTSTR)(LPCSTR)sCmd;

	m_bReceived = FALSE;
	this->Send(lpCmd, strlen(lpCmd)); // Send TCP command

	// Wait for response
	TRACE(_T("Wait Get Params...\n"));
	if (!CTimeUtils::WaitWithMsgPump(PARAM_TIMEOUT, m_bReceived))
		bParamOK = FALSE;

	if (!m_bReceived)
		bParamOK = FALSE;

	if (!m_sReceived.GetLength())
		bParamOK = FALSE;
	
	if (!bParamOK)
	{
		m_sLastError.Format(IDS_ERROR_FETCH_MAPCODES, "\n");
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}
	
	int i = 0;
	LPCTSTR lpReceived = m_sReceived;
	CString sToken, sUnit, sName;

	while (AfxExtractSubString(sToken, lpReceived, i, _T('/')))
	{
		if (sToken.Find(_T('('), 0)>1) // Unit is specified: <mapcodname>(<unit>)/...
		{
			AfxExtractSubString(sName, (LPCTSTR)(LPCSTR)sToken, 0, _T('('));
			AfxExtractSubString(sUnit, (LPCTSTR)(LPCSTR)sToken, 1, _T('('));
			sUnit.TrimRight(_T(')'));
		}
		else // Unit is not available
		{
			sName = sToken;
			sUnit = "NA";
		}

		//!!20160413:LBo:Changed Name & Unit separator from "" to ":" $LABS-2544
		ParamsMap.SetAtGrow(i, sName + ":" + sUnit);
		i++;
	}

	return TRUE;
}

// Start run loop thread
BOOL CPhoenix::SetRunThread(BOOL bSet)
{
	if (bSet)
	{
		// Is running thread started?
		if (m_bRunThread)
			return TRUE;

		m_bRunThread = TRUE;

		m_pThread = AfxBeginThread(RunWorkerThread,this, THREAD_PRIORITY_LOWEST);
		return (m_pThread != NULL);
	}
	else
	{
		m_bRunThread = FALSE;
		return TRUE;
	}
}

// Worker thread implementation
UINT CPhoenix::RunWorkerThread(LPVOID p)
{
	CPhoenix * me = (CPhoenix *)p;
	me->RunLoop();
	return 0;
}

// Threaded run loop => UI with test program emulation
void CPhoenix::RunLoop()
{
	CFTTSPApp* ptrApp = (CFTTSPApp*)AfxGetApp();

	DWORD SktMask=0;
	CString sBinStream;
	CStringArray sBinArray, sParamArray;

	char dataReceived[4096];
	char dataToSend[4096];
	char firstInfo[100];
	DWORD nbByteRead, nbByteWrt;

	while(m_bRunThread)
	{
		do // Engi mode listening loop
		{
			WaitForSingleObject(hvt_start, INFINITE);
			ReadFile(hpipe,	dataReceived, 4096, &nbByteRead, NULL);
			FlushFileBuffers(hpipe);
			sscanf(dataReceived, "%s", firstInfo);
			if (strstr(firstInfo, "UNLOAD")!=NULL)
			{
				m_bRunThread = FALSE;
				return;
			}
		} while (strstr(firstInfo, "ENGIMODE")!=NULL);

		sscanf(dataReceived, "%s", firstInfo);

		// Bin and mapcod transfer
		if (strstr(firstInfo, "STARTWAFER")!=NULL)
		{
			SetEvent(hvt_complete);
			WaitForSingleObject(hvt_start, INFINITE); // Wait for mask data written
			ReadFile(hpipe,	dataReceived, 4096, &nbByteRead, NULL); // Read pipe for mask data
			FlushFileBuffers(hpipe);
		}

		// Retreive masks
		CString temp;
		temp.Format("%s", dataReceived);
		temp.Left(temp.Find(" "));

		__int64 Mask1=0; // LSB
		__int64 Mask2=0; // MSB

		if (temp.Find(",")!=-1)
		{
			Mask1 = _atoi64(temp.Left(temp.Find("T")));
			Mask2 = _atoi64(temp.Mid(temp.Find(",")+1, temp.Find("T")-temp.Find(",")));
		}
		else
			Mask1 = _atoi64(temp.Left(temp.Find("T")));

		
		// Run test
		if (!this->RunTest(Mask1))
		{
			AfxMessageBox(m_sLastError);
			m_bRunThread = FALSE;
			return;
		}

		// Fetch result
		if (!this->ParseResults(sBinArray, sParamArray))
		{
			AfxMessageBox(m_sLastError);
			m_bRunThread = FALSE;
			return;
		}

		// Send binning
		sBinStream.Empty();
		for (int dut=0; dut<MAX_SOCKET; dut++)
			sBinStream += sBinArray.GetAt(dut) + ",";

		sBinStream.TrimRight(_T(','));
		TRACE(sBinStream, "\n");

		strcpy(dataToSend, sBinStream.GetBuffer(10));
		WriteFile(hpipe, dataToSend, strlen(dataToSend), &nbByteWrt, NULL);
		SetEvent(hvt_complete);

		// Mapcod
		WaitForSingleObject(hvt_start, INFINITE);
		for (dut=0; dut<MAX_SOCKET; dut++)
		{
			CString sParamStream = sParamArray.GetAt(dut);
			if (sParamStream.GetLength() == 0)
				continue;

			CString sData;
			sData.Format("%s", sParamStream);
			strcpy(dataToSend, sData);
			WriteFile(hpipe, dataToSend, strlen(dataToSend), &nbByteWrt, NULL);

			SetEvent(hvt_data_ready);
			WaitForSingleObject(hvt_complete, INFINITE);
		}
		// End xfer
		strcpy(dataToSend, "ENDMAPXFER");
		WriteFile(hpipe, dataToSend, strlen(dataToSend), &nbByteWrt, NULL);

		SetEvent(hvt_data_ready);
		WaitForSingleObject(hvt_complete, INFINITE);

	} // while m_bRunThread
}

// Start test using socket mask
BOOL CPhoenix::RunTest(__int64 Mask)
{
	
	CString sCmd, sArg, sSkt, sCat;
	int sktCnt = 0;

	// Make command using mask
	for (int i=0; i<MAX_SOCKET; i++)
	{
		if ((int)(Mask >> i) & (int)0x0001)
		{
			sktCnt++;
			// !!20150408:LBo:Use "%02d" to avoid adding "0" before "10" $LABS-2533
			sSkt.Format("%02d", i+1);

			sCat += "/" + sSkt;
			if (m_sHeadParam[i].GetLength() > 0)
				sCat += ":" + m_sHeadParam[i];
		}
	}
	
	// !!20160408:LBo:Use "%02d" to avoid adding "0" before 10 $LABS-2533
	sArg.Format("%02d%s", sktCnt, sCat);

	TRACE(_T("RUN%s\n"), sArg);
	FormatCmd(sCmd, CMD_START_TEST, sArg);
	LPCTSTR lpCmd = (LPCTSTR)(LPCSTR)sCmd;

	m_bReceived = FALSE;
	this->Send(lpCmd, strlen(lpCmd));

	// Acknowledge feed 
	//while (!m_bReceived) {Sleep(100);}; // Ack receive
	if (!CTimeUtils::WaitFlag(m_bReceived, 10000))
	{
		return FALSE;
	}
	CString sAck = m_sReceived; // Copy ack response
	m_bReceived = FALSE; // Reset receive flag

	// Process ack
	if (sAck.Find(_T(ANS_RUN_OK)) == -1)
	{
		m_sLastError.Format("Error executing test. Response from tester not correct: %s!\n", m_sReceived);
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}
	
	//while (!m_bReceived) {Sleep(100);}; // Wait for test result
	//!!20160420:LBo:Set timeout to 30s with RUN_TEST_TIMEOUT constant $LABS-2546
	if (!CTimeUtils::WaitFlag(m_bReceived, RUN_TEST_TIMEOUT))
	{
		return FALSE;
	}
	m_bReceived = FALSE; // Reset quickly

	return TRUE;
}

// Parse data sent by Phoenix
//  sBinStream is single string stream containing all duts: <dut1_hb>|<dut1_bin>.<dut1_sb>,<dut2_hb>|<dut2_bin>.<dut2_sb>,...
//  sParamArray is an array of params for each dut: sParamArray[<dut1>] = <dut1> <param1>|<param2>|...|ENDMAPCOD
BOOL CPhoenix::ParseResults(CStringArray& sBinArray, CStringArray& sParamArray)
{
	CString sToken, sBin, sSoftBin, sHardBin;
	int sktCnt, sktNum;
	int i = 0, MapCodCounter, ParamArrayLen;
	TCHAR MapCodIdxChar;

	// Initialize array
	for (i=0; i<=MAX_SOCKET; i++)
	{
		sParamArray.SetAtGrow(i, "");
		sBinArray.SetAtGrow(i, "0|0.00");
	}
	
	if (m_sReceived.Find("RUN") == -1)
	{
		m_sLastError.Format(_T("Can't parse results %s\n"), m_sReceived);
		SetLastErrorDescription(m_sLastError);
		return FALSE;
	}

	// Parse Phoenix results string

	if (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)m_sReceived, 0, _T('/')))
	{
		sToken = sToken.Right(2);
		sktCnt = atoi(sToken.GetBuffer(2));
	}
	else
		return FALSE;

	i = 1;
	while (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)m_sReceived, i, _T('/'))) // Split by "/", dut loop
	{
		int j = 0;
		CString sDutResult = sToken;
		CString sSubBin = sToken;
		
		if (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)sDutResult, 0, _T(':'))) // socket index !!! 1-based !!!
		{
			sBin = sSoftBin = "";
			sktNum = atoi(sToken.GetBuffer(100));
			sParamArray[sktNum-1].Format("%d ", sktNum-1);
		}
		else
			continue;
		
		if (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)sDutResult, 1, _T(':'))) // bin
			sBin = sToken;
		else
			continue;

		if (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)sDutResult, 2, _T(':'))) // softbin
			sSoftBin = sToken;
		else
			continue;

		if (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)sDutResult, 3, _T(':'))) // mapcod
			//!!20150414:LBo:Insert MapCod index '1' before string $LABS-2544
			sParamArray[sktNum-1] += '1'+ sToken;
		else
			continue;

		//20170215:LBo:Added HardBin at 4th token $LABS-2703
		if (AfxExtractSubString(sToken, (LPCTSTR)(LPCSTR)sDutResult, 4, _T(':'))) // mapcod
			sHardBin = sToken;
		else //Legacy mode: if no hard bin specified at 4th token, then hard bin = bin
			sHardBin = sBin;

		sBinArray.SetAt(sktNum-1, sHardBin + "|" + sBin + "." + sSoftBin);
		//!!20150414:LBo:Add MapCod index at separator $LABS-2544
		MapCodCounter = 1;
		ParamArrayLen = sParamArray[sktNum-1].GetLength();
		for(j=0; j<ParamArrayLen; j++)
		{
			if (sParamArray[sktNum-1].GetAt(j) == _T(';'))
			{
				MapCodCounter++;
				sParamArray[sktNum-1].SetAt(j, _T('|'));
				if (MapCodCounter <10)
				{
					MapCodIdxChar = '0' + MapCodCounter;
				}
				else 
				{
					if((MapCodCounter > 9) && (MapCodCounter < 65))
					{
						MapCodIdxChar = 'A' + MapCodCounter - 10;
					}
				}
				sParamArray[sktNum-1].Insert(j+1, MapCodIdxChar);
				ParamArrayLen++;
			}
		}
		sParamArray[sktNum-1] += "|ENDMAPCOD";
		TRACE("DUT: %d (1-BASED), BIN = %s\n", sktNum, sBinArray[sktNum-1]);
		TRACE("DUT: %d (1-BASED), MAPCOD = %s\n", sktNum, sParamArray[sktNum-1]);

		i++;
	}

	sBin.Empty();
	sToken.Empty();
	sSoftBin.Empty();

	return TRUE;
}

// Fill head param array
void CPhoenix::SetHeadParams(CStringArray& sHeadParamArr)
{
	int max = sHeadParamArr.GetSize();
	
	if (max > MAX_SOCKET)
		max = MAX_SOCKET;

	for (int i=0; i<max; i++)
		m_sHeadParam[i] = sHeadParamArr.GetAt(i);
}
