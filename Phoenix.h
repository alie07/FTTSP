// Phoenix.h: interface for the CPhoenix class.
// !!20160412:LBo:Rename file name and class name "Alcatraz" to "Phoenix" $LABS-2543
// !!20170215:LBo:FLOWLOADED is now checked $LABS-2706
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHOENIX_H__1CBAFB16_F581_4C3E_9BCF_CB1A759AFF18__INCLUDED_)
#define AFX_PHOENIX_H__1CBAFB16_F581_4C3E_9BCF_CB1A759AFF18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************************************************************************/
/* PUBLIC DEFINED CONSTANTS				                                      */
/******************************************************************************/

// Defaults
#define DEFAULT_EXE_PATH		"C:\\Program Files\\testeur\\Phoenix.exe"
#define DEFAULT_HOST			"127.0.0.1"
#define DEFAULT_PORT			333
#define DEFAULT_PROCESS_NAME	"Phoenix.exe"
#define DEFAULT_TESTPRGM_PATH	"C:\\testeur\\flowzip"

#define BYTE_RECEIVED			10000

// Phoenix command
#define CMD_SOFTVERSION			"ID?"
#define CMD_LOAD_PRGM			"FLOW"
#define CMD_RETURN_PARAM		"RETURN?"
#define CMD_START_TEST			"RUN"

// Phoenix answer
#define ANS_FLOW_OK				"FLOWOK"
#define ANS_FLOW_LOADED			"FLOWLOADED"
#define ANS_PARAM_OK			"PARAMOK"
#define ANS_RUN_OK				"RUNOK"

// Timeout in milliseconds
#define TIMEOUT_CONNECTION		10000
#define PRGM_TIMEOUT			10000
#define PARAM_TIMEOUT			10000
#define RUN_TEST_TIMEOUT		120000 //!!07022017:LBo:increased timeout to 2 min $LABS-2699

#define MAX_SOCKET 32

/******************************************************************************/
/* CLASS DEFINITION															  */
/******************************************************************************/

class CPhoenix : public CAsyncSocket
{
	DECLARE_DYNAMIC( CPhoenix )

public:
	UINT GetSiteMax();
	CString GetInstalledProgram();
	void SetHeadParams(CStringArray& sHeadParamArr);
	HANDLE hpipe;
	HANDLE hvt_data_ready;
	HANDLE hvt_complete;
	HANDLE hvt_start;
	BOOL volatile m_bReceived;
	BOOL volatile m_bRunThread;
	static CString GetCurrentRelease(CString=DEFAULT_EXE_PATH);
	static UINT RunWorkerThread(LPVOID p);
	BOOL SetRunThread(BOOL bSet = TRUE);
	BOOL LoadProgram(CString &sPrgm);
	BOOL GetParams(CStringArray &ParamsMap);
	BOOL GetID(CString &sID);
	BOOL IsConnected();
	BOOL Connection();
	BOOL InstallProgram(const CString &aProgram);
	CString GetInstallPath();
	BOOL Start(BOOL bEngiMode);
	// Generated message map functions
	//{{AFX_MSG(CPhoenix)
		// NOTE - the ClassWizard will add and remove member functions here.
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	//}}AFX_MSG
	CPhoenix(CString sHost=DEFAULT_HOST, int nPort=DEFAULT_PORT, CString sExePath=DEFAULT_EXE_PATH, CString sProcessName=DEFAULT_PROCESS_NAME);
	virtual ~CPhoenix();

// Implementation
protected:
	CStringArray m_sHeadParam;
	CWinThread *m_pThread;
	PROCESS_INFORMATION m_pi;
	CString m_sHost;
	int m_nPort;
	CString m_sExePath;
	CString m_sProcessName;
	CString m_InstalledProgramName;
	void FormatCmd(LPCSTR& lpCmd, CString sCmd, CString sArg);
	void FormatCmd(CString& lpCmd, CString sCmd, CString sArg);
	BOOL ParseResults(CStringArray& sBinArray, CStringArray& sParamArray);
	BOOL RunTest(__int64 Mask);
	void RunLoop();

private:
	HANDLE m_hTimer;
	BOOL m_bCreated;
	BOOL m_bStarted;
	CString m_sTrace;
	CString m_sReceived;
	CString m_sLastError;
	BOOL m_bConnected;
};

#endif // !defined(AFX_PHOENIX_H__1CBAFB16_F581_4C3E_9BCF_CB1A759AFF18__INCLUDED_)
