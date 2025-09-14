// Utils.h: interface for the CDirectory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILS_H__CDFC0A3F_CEB9_4ECE_BCA3_6C2FF1AF3040__INCLUDED_)
#define AFX_UTILS_H__CDFC0A3F_CEB9_4ECE_BCA3_6C2FF1AF3040__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectory  
{
public:
	static BOOL Create(LPCTSTR path);
};

class CStringUtils
{
public:
	static BOOL ReplaceTag(CString &aString, const CString &aTag, const CString &sReplacement, BOOL bReplaceAll = TRUE);
	static BOOL GetChunk (CString &aString, CString    &aChunk , char c);
	static BOOL GetChunk (CString &aString, DOUBLE     &aDouble, char c);
	static BOOL GetChunk (CString &aString, CWordArray &aWArray, char c);
	static BOOL GetChunk (CString &aString, LONG       &aInt   , char c);
	static BOOL GetChunk (CString &aString, BOOL       &aBool  , char c);
};

class CProcessUtils
{
public:
	static BOOL IsProcess32Running(const CString processName);
};

class CTimeUtils
{
public:
	static BOOL WaitWithMsgPump(int milliSecs, HANDLE hTimer);
	static BOOL WaitWithMsgPump(int milliSecs, BOOL volatile &flag);
	static BOOL SetTimerSecond(int seconds, HANDLE hTimer);
	static BOOL WaitSeconds(int seconds, HANDLE hTimer);
	static BOOL CTimeUtils::WaitFlag(BOOL volatile &, int);
};

#endif // !defined(AFX_UTILS_H__CDFC0A3F_CEB9_4ECE_BCA3_6C2FF1AF3040__INCLUDED_)
