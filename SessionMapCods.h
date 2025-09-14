// SessionMapCods.h: interface for the CSessionMapCods class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONMAPCODS_H__D6D9D022_F2F1_4EC8_9DDF_8C92A721F784__INCLUDED_)
#define AFX_SESSIONMAPCODS_H__D6D9D022_F2F1_4EC8_9DDF_8C92A721F784__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Session.h"

class CSessionMapCods : public CSession  
{
public:
	CSessionMapCods();
	virtual ~CSessionMapCods();

	virtual BOOL Open(const CSessionConfiguration &aSC, int nSessionNumber=0);
	virtual BOOL AddTest(const CArray<CSocketTest, CSocketTest&> &aST);
	virtual BOOL AddMapCod(const CString &sNum, const CString & sName, const CString &sUnit);
	virtual BOOL Close(void);

protected:
	CString m_sSessionFilePath;
	virtual void WriteHeader(const CSessionConfiguration &aSC);

	CStdioFile m_fMapCods;
	CMapStringToString m_mapMapCods;

private:
	static int compare (const void *arg1, const void *arg2 );

	CTime m_tStart;
	CTime m_tStop;
};

#endif // !defined(AFX_SESSIONMAPCODS_H__D6D9D022_F2F1_4EC8_9DDF_8C92A721F784__INCLUDED_)
