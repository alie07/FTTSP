// SessionSummary.h: interface for the CSessionSummary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONSUMMARY_H__A0DD9C4C_8503_4594_A284_E15290BFDCCA__INCLUDED_)
#define AFX_SESSIONSUMMARY_H__A0DD9C4C_8503_4594_A284_E15290BFDCCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Session.h"

#define SUM_MAJOR_VERS		((unsigned char)3)
#define SUM_MINOR_VERS		((unsigned char)4)


class CSessionSummary : public CSession  
{
public:
	CSessionSummary();
	virtual ~CSessionSummary();

	virtual BOOL Open(const CSessionConfiguration &aSC, int nSessionNumber=0);
	virtual BOOL AddTest(const CArray<CSocketTest, CSocketTest&> &aST);
	virtual BOOL AddBinDescription(const CString &sDesc);
	virtual BOOL AddProbeParam(const CString &sProbeParam);
	virtual BOOL Close(void);

protected:
	CString m_sSessionFilePath;
	virtual void WriteHeader(const CSessionConfiguration &aSC);

	CStdioFile m_fSummary;
	CString m_sProbeParam;
	CMap<LONG, LONG, CString, CString&> m_mapBinDesc;

	CMapWordToOb m_mapSoftCounts;
	CMapWordToOb m_mapHardCounts;

	int m_nSktCount;

	DWORD m_dwTotal;
private:
	static int compare (const void *arg1, const void *arg2 );
};

#endif // !defined(AFX_SESSIONSUMMARY_H__A0DD9C4C_8503_4594_A284_E15290BFDCCA__INCLUDED_)
