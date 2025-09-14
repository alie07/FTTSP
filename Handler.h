// Handler.h: interface for the CHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HANDLER_H__AD9FBBA2_70C9_4E86_879E_B3C5D47BBE60__INCLUDED_)
#define AFX_HANDLER_H__AD9FBBA2_70C9_4E86_879E_B3C5D47BBE60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HandlerDll.h"
#include "RTM.h"

class CHandler : public CObject  
{
public:
	typedef enum {NORMAL_EXIT = 0, TIMEOUT_TEST	} Status;
	CHandler();
	virtual ~CHandler();
	virtual BOOL LoadDll(LPCSTR dllname);

	virtual BOOL SetOnLine(BOOL bIsOnLine = TRUE);

	virtual BOOL IsRunning(void);
	int Init( LPCSTR param );

	virtual BOOL GetDllVersion(CString &strVers);

	volatile BOOL m_bNetworkConnectLoop;

	HWND m_hWnd;
	HANDLE m_hTestStart;
	HANDLE m_hTestEnd;
	HANDLE m_hTestDone;

	BOOL m_HndIsRunning;

	DWORD m_dwTestTimeOut;

	DWORD m_dwSockets;

	DWORD * m_pBins;

	BOOL m_bRTM;

protected:
	BOOL m_bRTMInit;

	CWinThread* m_pThread;

	// Called to start the worker thread
	static UINT Run(LPVOID p);

	// Everything below this line is called inside the child thread

	HNDID		 m_HndId;
	LPHNDGETVERS HndGetVers;
	LPHNDINIT	 HndInit;
	LPHNDPOLL	 HndPoll;
	LPHNDBIN	 HndBin;
	LPHNDGET	 HndGet;
	LPHNDSET	 HndSet;
	LPHNDRELEASE	HndRelease;

	// Called to run the worker thread by the static version
	void Run(void);
	volatile BOOL m_bRunning; // Read in One thread and modified by one another
	volatile BOOL m_bIsOnLine;

	HINSTANCE genHandle;

	HANDLE m_hPipeOut;
	HANDLE m_hPipeErr;
	TBYTE m_u8MajorVersion;  
	TBYTE m_u8MinorVersion;  
	BOOL m_bVersionAvailiable;

	CRTM oRTM;
};

#endif // !defined(AFX_HANDLER_H__AD9FBBA2_70C9_4E86_879E_B3C5D47BBE60__INCLUDED_)
