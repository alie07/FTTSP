// RTM.cpp: implementation of the CRTM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fttsp.h"
#include "RTM.h"
#include "atlbase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRTM::CRTM()
{
	oReg = CRegKey();
	index = 0;
	check = 0;
}

CRTM::~CRTM()
{
}

void CRTM::update_reg(DWORD mask_skts, DWORD* bins)
{
	char key[100];

	set_bin_per_site(mask_skts, bins);

	if (check == 0) check = 1; else check = 0;
	for (int ss=0; ss < MAX_ACTIVE_SITE; ss++)
	{
		if (RTM_active_site[ss] == 0)
			bin[ss] = 0;

		if (bin[ss] != 0)
			index++;

		sprintf(key, "SITE%i", ss+1);
		oReg.SetValue(bin[ss], key);
	}

	oReg.SetValue(check, CHECK_FIELD);
	oReg.SetValue(index, INDEX_FIELD);
}

BOOL CRTM::init_reg()
{
	char key[100];
	DWORD dwValue;

	m_bRegAccess = FALSE;

	if (ERROR_SUCCESS != oReg.Create(HKEY_CURRENT_USER, RTM_KEY))
	{
		// Error
		AfxMessageBox("Cannot access to Windows Registry! Test will continue without RTM update.", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	if (oReg.QueryValue(dwValue, INDEX_FIELD) != ERROR_SUCCESS)
	{
		TRACE("RTM Init Error: %s\n", GetLastError());
		return FALSE;
	}

	if (index == 0)
	{
		check = 0;
		for (int ss=0; ss<MAX_ACTIVE_SITE; ss++)
		{
			bin[ss] = 0;
			RTM_active_site[ss] = 0;
		}

		oReg.SetValue(check, CHECK_FIELD);

		// Reset SITE field
		for (ss=0; ss<MAX_ACTIVE_SITE; ss++)
		{
			sprintf(key, "SITE%i", ss+1);
			oReg.SetValue((DWORD)0, key);
		}

		oReg.SetValue(index, INDEX_FIELD);
	}

	m_bRegAccess = TRUE;
	return m_bRegAccess;
}

void CRTM::set_active_site(DWORD activeMask)
{
	for (int i=0; i<MAX_ACTIVE_SITE; i++)
		RTM_active_site[i] = (activeMask >> i) && 0x01;

}

void CRTM::set_bin_per_site(DWORD mask_skts, DWORD* bins)
{
	int s32Loop=0;

	while(mask_skts)
	{
		/* Check if the socket was in test */
		if ( (mask_skts&1)==1)
			bin[s32Loop] = bins[s32Loop];
		else
			bin[s32Loop] = 0;

		s32Loop++;			// Next bin
		mask_skts>>=1;		// Next site 
	}
}
