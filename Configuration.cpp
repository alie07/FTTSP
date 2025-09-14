/******************************************************************************/
/*                                                                            */
/*  Layer           : Api			                                          */
/*                                                                            */
/*  CLASS	        : Configuration		                                      */
/*  Description     : Class which contains all information concerning the     */
/*					  the program test, the handler and the product to test	  */
/*																			  */
/*                                                                            */
/*  File            : Configuration.cpp			                              */
/*                                                                            */
/*  Scope           : Public                                                  */
/*                                                                            */
/*  Target          :                                                         */
/*                                                                            */
/*  Coding language : C++                                                     */
/*                                                                            */
/* COPYRIGHT 2011 VAULT-IC France                                             */
/* all rights reserved                                                        */
/*                                                                            */
/******************************************************************************/

/***************************  REVISION HISTORY  *******************************
 * 
 *    Rev 1.0:   06 Sept 2011 CDESBOUCHAGES
 *
 * Recovery of existing code: 
 * - Add commentaries
 * - Move constants into FTTSP.h file.
 *
 ******************************************************************************/

/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/
#include "stdafx.h"

/******************************************************************************/
/* INCLUSION OF OTHER MODULES HEADERS                                         */
/******************************************************************************/
#include "FTTSP.h"

/******************************************************************************/
/* INCLUSION OF OWNER HEADERS                                                 */
/******************************************************************************/
#include "Configuration.h"

/******************************************************************************/
/* CLASS IMPLEMENTATION			                                              */
/******************************************************************************/
// Configuration.cpp: implementation of the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CConfiguration, CObject);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfiguration::CConfiguration()
{

}

CConfiguration::CConfiguration(const CConfiguration &aConfig):
	strProduct(aConfig.strProduct),
	strRevDesign(aConfig.strRevDesign),
	strPartID(aConfig.strPartID),
	strProdCode(aConfig.strProdCode),
	strFlow(aConfig.strFlow),
	dTemp(aConfig.dTemp),
	dHoldYield(aConfig.dHoldYield),
	strLoadBoard(aConfig.strLoadBoard),
	strLoadBoardNumber(aConfig.strLoadBoardNumber),
	strCable(aConfig.strCable),
	strCableNumber(aConfig.strCableNumber),
	strOperator(aConfig.strOperator),
	strHandler(aConfig.strHandler),
	strHandlerId(aConfig.strHandler),
	nControllers(aConfig.nControllers),
	strTesterType(aConfig.strTesterType),
	strSoftVersion(aConfig.strSoftVersion),
	strProgram(aConfig.strProgram),
	strProgramPath(aConfig.strProgramPath),
	strSetupRev(aConfig.strSetupRev)
{
	int nIndex;
	waSkts.RemoveAll();
	waSkts.SetSize(aConfig.waSkts.GetSize());
	for (nIndex=0; nIndex < aConfig.waSkts.GetSize(); nIndex++)
		waSkts[nIndex] = aConfig.waSkts[nIndex];
	iTestMask=aConfig.iTestMask;
}

CConfiguration& CConfiguration::operator =(const CConfiguration &aConfig)
{
	if (this == &aConfig)
		return *this;

	strProduct			= aConfig.strProduct;
	strRevDesign		= aConfig.strRevDesign;
	strPartID			= aConfig.strPartID;
	strProdCode			= aConfig.strProdCode;
	strFlow				= aConfig.strFlow;
	dTemp				= aConfig.dTemp;
	dHoldYield			= aConfig.dHoldYield;
	strLoadBoardNumber	= aConfig.strLoadBoardNumber;
	strCableNumber		= aConfig.strCableNumber;
	strLoadBoard		= aConfig.strLoadBoard;
	strCable			= aConfig.strCable;
	strHandler			= aConfig.strHandler;
	strHandlerId		= aConfig.strHandlerId;
	nControllers		= aConfig.nControllers;
	strTesterType		= aConfig.strTesterType;
	strSoftVersion		= aConfig.strSoftVersion;
	strProgram			= aConfig.strProgram;
	strProgramPath		= aConfig.strProgramPath;
	strOperator			= aConfig.strOperator;
	strSetupRev			= aConfig.strSetupRev;
	int nIndex;
	waSkts.RemoveAll();
	waSkts.SetSize(aConfig.waSkts.GetSize());
	for (nIndex=0; nIndex < aConfig.waSkts.GetSize(); nIndex++)
		waSkts[nIndex] = aConfig.waSkts[nIndex];
	iTestMask=aConfig.iTestMask;

	return *this;
}

CConfiguration::~CConfiguration()
{

}

CString CConfiguration::GetProduct( void ) const
{
	return strProduct;
}
CString CConfiguration::GetSetupRev( void ) const
{
	return strSetupRev;
}
CString CConfiguration::GetProdCode( void ) const
{
	return strProdCode;
}

CString CConfiguration::GetRevDesign( void ) const
{
	return strRevDesign;
}

CString CConfiguration::GetPartID( void ) const
{
	return strPartID;
}

CString CConfiguration::GetFlow( void ) const
{
	return strFlow;
}

CString CConfiguration::GetHandler( void ) const
{
	return strHandler;
}

CString CConfiguration::GetHandlerId( void ) const
{
	return strHandlerId;
}

CString CConfiguration::GetLoadBoard(void) const
{
	return strLoadBoard;
}


CString CConfiguration::GetCable(void) const
{
	return strCable;
}

CString CConfiguration::GetLoadBoardNumber(void) const
{
	return strLoadBoardNumber;
}


CString CConfiguration::GetCableNumber(void) const
{
	return strCableNumber;
}

CString CConfiguration::GetOperator(void) const
{
	return strOperator;
}

BOOL CConfiguration::Parse(LPCSTR str, char c)
{
	CString strTemp = str;
	return _Parse(strTemp, c);
}

CString CConfiguration::GetTesterType() const
{
	return strTesterType;
}

CString CConfiguration::GetSoftVersion() const
{
	return strSoftVersion;
}

LONG CConfiguration::GetControllers() const
{
	return nControllers;
}

CWordArray& CConfiguration::GetSockets()
{
	return waSkts;
}

BOOL CConfiguration::_Parse(CString &str, char c)
{
	BOOL bresult=FALSE;
	char sep[] = " \t\n";
	char *token;

	char string[4096];
	sprintf(string,"%s",str); //cast cstring to char [] due to strtok

	token = strtok(string, sep);
	int index = 0;
	
	while( token != NULL )
	{
		// Parse All line and extract data
		switch(index)
		{
			case 0	:	strProduct.Format("%s",token);		bresult = TRUE; break; 
			case 1	:	strRevDesign.Format("%s",token);	bresult = TRUE; break; 
			case 2	:	strPartID.Format("%s",token);		bresult = TRUE; break; 
			case 3	:	strProdCode.Format("%s",token);		bresult = TRUE; break; 
			case 4	:	strFlow.Format("%s",token);			bresult = TRUE; break; 
			case 5	:	dTemp = atof(token);				bresult = TRUE; break; 
			case 6	:	dHoldYield = atof(token);			bresult = TRUE; break; 
			case 7	:	strLoadBoard.Format("%s",token);	bresult = TRUE; break; 
			case 8	:	strCable.Format("%s",token);		bresult = TRUE; break; 
			case 9	:	strHandler.Format("%s",token);		bresult = TRUE; break; 
			case 10	:	nControllers = atol(token);			bresult = TRUE; break; 
			case 11	:	(CStringUtils::GetChunk((CString)token, waSkts, c))? bresult = TRUE : bresult = FALSE; break; 
			case 12	:	strTesterType.Format("%s",token);	bresult = TRUE; break; 
			case 13	:	strSoftVersion.Format("%s",token);	bresult = TRUE; break; 
			case 14	:	strProgram.Format("%s",token);		bresult = TRUE; break; 
			default :	bresult = FALSE ; break;

			//case 11 ==> Keep compatibility with BBN function !!!
			//			 used GetChunk function to calculate waSkts...
		}		
		
		index++;
		//Get next token
		token = strtok( NULL, sep );
	}		
	
	// There is at least one invalid data. Deletes everything and return FALSE.
	if(!bresult) Empty();

	return bresult;
}

/*
BOOL CConfiguration::_Parse(CString &str, char c)
{
	if (// Parse All Data in String
		CStringUtils::GetChunk(str, strPartID     , c) &&
		CStringUtils::GetChunk(str, strProdCode   , c) &&
		CStringUtils::GetChunk(str, strFlow       , c) &&
		CStringUtils::GetChunk(str, dTemp         , c) &&
		CStringUtils::GetChunk(str, dHoldYield    , c) &&
		CStringUtils::GetChunk(str, strLoadBoard  , c) &&
		CStringUtils::GetChunk(str, strCable      , c) &&
		CStringUtils::GetChunk(str, strHandler    , c) &&
		CStringUtils::GetChunk(str, nControllers  , c) &&
		CStringUtils::GetChunk(str, waSkts        , c) &&
		CStringUtils::GetChunk(str, strTesterType , c) &&
		CStringUtils::GetChunk(str, strSoftVersion, c) &&
		CStringUtils::GetChunk(str, strProgram    , c)
		)
		return TRUE;

	// There is at least one invalid data. Deletes everything and return FALSE.
	Empty();
	return FALSE;
}
*/

void CConfiguration::Empty()
{
	strPartID.Empty();
	strProdCode.Empty();
	strFlow.Empty();
	dTemp=0;
	dHoldYield=0;
	strLoadBoard.Empty();
	strCable.Empty();
	strLoadBoardNumber.Empty();
	strCableNumber.Empty();
	strHandler.Empty();
	nControllers=0;
	waSkts.RemoveAll();
	strTesterType.Empty();
	strSoftVersion.Empty();
	strProgram.Empty();
	strOperator.Empty();
	strHandlerId.Empty();

}

CString CConfiguration::GetProgram() const
{
	CString sProgramPath;
    CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 

	sProgramPath = ptrApp->GetProgramPath();
	sProgramPath += "\\";
	sProgramPath += GetProduct();
	sProgramPath += "\\";
	sProgramPath += GetFlow();
	sProgramPath += "\\";

//	sProgramPath += "\\FT\\";
	sProgramPath += strProgram;
	sProgramPath += ".zip";

	return sProgramPath;
}

CString CConfiguration::GetProgramName() const
{
	return (strProgram+".zip");
}

CString CConfiguration::GetProgramPerso() const
{
	CString sProgramPath;
	CFTTSPApp* ptrApp = (CFTTSPApp*) AfxGetApp(); 

	sProgramPath = ptrApp->GetProgramPath();

	sProgramPath += "\\";
	sProgramPath += GetProduct();
	sProgramPath += "\\";
	sProgramPath += GetFlow();
	sProgramPath += "\\";

	sProgramPath += strProgram;

	return sProgramPath;
}

CString CConfiguration::ExpandTags(LPCTSTR lpSrc) const
{
	CString sSrc(lpSrc);
	CTime tCurrentTime = CTime::GetCurrentTime();
	CString strTemp;

	CStringUtils::ReplaceTag(sSrc, "%Date%", tCurrentTime.Format("%Y%m%d"));
	CStringUtils::ReplaceTag(sSrc, "%Time%", tCurrentTime.Format("%H%M%S"));
	CStringUtils::ReplaceTag(sSrc, "%Product%", GetProduct());
	CStringUtils::ReplaceTag(sSrc, "%PartID%", GetPartID());
	CStringUtils::ReplaceTag(sSrc, "%Flow%", GetFlow());
	strTemp.Format("%+d", dTemp);
	CStringUtils::ReplaceTag(sSrc, "%Temp%", strTemp);
	CStringUtils::ReplaceTag(sSrc, "%LoadBoard%", strLoadBoard);
	CStringUtils::ReplaceTag(sSrc, "%Cable%", strCable);
	CStringUtils::ReplaceTag(sSrc, "%Handler%", strHandler);
	CStringUtils::ReplaceTag(sSrc, "%TesterType%", strTesterType);
	CStringUtils::ReplaceTag(sSrc, "%SoftVersion%", strSoftVersion);
	CStringUtils::ReplaceTag(sSrc, "%Program%", strProgram);

	return sSrc;
}
