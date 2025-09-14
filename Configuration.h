// Configuration.h: interface for the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGURATION_H__AFD66AE2_AA1F_4814_929E_C7CCFCA0FB67__INCLUDED_)
#define AFX_CONFIGURATION_H__AFD66AE2_AA1F_4814_929E_C7CCFCA0FB67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConfiguration : public CObject  
{
	DECLARE_DYNAMIC( CConfiguration )
public:
	CConfiguration();
	CConfiguration(const CConfiguration &aConfig);
	virtual ~CConfiguration();

	CConfiguration& operator=(const CConfiguration& aConfig);

	virtual BOOL Parse(LPCSTR str, char c);

	virtual void Empty(void);

	virtual CString GetProduct( void ) const;
	virtual CString GetProdCode( void ) const;
	virtual CString GetRevDesign( void ) const;
	virtual CString GetPartID( void ) const;
	virtual CString GetFlow( void ) const;
	virtual CString GetHandler( void ) const;
	virtual CString GetHandlerId( void ) const;
	virtual CString GetTesterType( void ) const;
	virtual CString GetSoftVersion( void ) const;
	virtual CString GetLoadBoard( void ) const;
	virtual CString GetCable( void ) const;
	virtual LONG    GetControllers( void ) const;
	virtual CWordArray& GetSockets( void );
	virtual CString GetProgram(void) const;
	virtual CString GetProgramPerso(void) const;
	virtual CString GetLoadBoardNumber(void) const;
	virtual CString GetCableNumber(void) const;
	virtual CString GetOperator(void) const;
	virtual CString ExpandTags(LPCTSTR lpSrc) const;
	virtual CString GetProgramName(void) const;
	virtual CString GetSetupRev( void ) const;

	CString    strProduct;
	CString    strRevDesign;
	CString    strPartID;
	CString    strProdCode;
	CString    strFlow;
	CString	   strSetupRev;
	DOUBLE     dTemp;
	DOUBLE     dHoldYield;
	CString    strLoadBoard;
	CString    strLoadBoardNumber;
	CString    strCable;
	CString    strCableNumber;
	CString    strHandler;
	//!!08042021:LBs:Added strHandlerId
	CString    strHandlerId;
	LONG       nControllers;
	CWordArray waSkts;
	int		   iTestMask;
	CString    strTesterType;
	CString    strSoftVersion;
	CString    strProgram;
	CString	   strProgramPath;
	CString	   strOperator;

protected:
	virtual BOOL _Parse(CString& str, char c);
};

#endif // !defined(AFX_CONFIGURATION_H__AFD66AE2_AA1F_4814_929E_C7CCFCA0FB67__INCLUDED_)
