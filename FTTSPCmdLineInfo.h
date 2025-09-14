// FTTSPCmdLineInfo.h: interface for the CFTTSPCmdLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTTSPCMDLINEINFO_H__E8504379_BD4C_4B81_822B_32BAAD6483B9__INCLUDED_)
#define AFX_FTTSPCMDLINEINFO_H__E8504379_BD4C_4B81_822B_32BAAD6483B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFTTSPCmdLineInfo : public CCommandLineInfo  
{

private:
	BOOL m_bEngiEnv;
	BOOL m_bValidArg;
	CString m_sSetupPath;

public:
	CFTTSPCmdLineInfo();
	virtual ~CFTTSPCmdLineInfo();

	BOOL bEngiEnv() { return m_bEngiEnv; };
	CString GetSetupPath() { return m_sSetupPath; };

	virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast);
};

#endif // !defined(AFX_FTTSPCMDLINEINFO_H__E8504379_BD4C_4B81_822B_32BAAD6483B9__INCLUDED_)
