// RTM.h: interface for the CRTM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTM_H__F35EAE85_DAFD_4523_8B50_2A1566E0BE10__INCLUDED_)
#define AFX_RTM_H__F35EAE85_DAFD_4523_8B50_2A1566E0BE10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "atlbase.h"

#define MAX_ACTIVE_SITE		32
#define INDEX_FIELD			"index"
#define CHECK_FIELD			"check"
#define RTM_KEY				"Environment\\RTM\\BIN_result"

class CRTM  
{
public:
	void set_bin_per_site(DWORD mask_skts, DWORD* bins);
	void set_active_site(DWORD activeMask);
	void update_reg(DWORD mask_skts, DWORD* bins);
	BOOL init_reg();
	CRTM();
	virtual ~CRTM();

protected:
	BOOL m_bRegAccess;
	CRegKey oReg;
	int RTM_active_site[MAX_ACTIVE_SITE];
	long index;
	long check;
	int bin[MAX_ACTIVE_SITE];
	HKEY hKey;
};

#endif // !defined(AFX_RTM_H__F35EAE85_DAFD_4523_8B50_2A1566E0BE10__INCLUDED_)
