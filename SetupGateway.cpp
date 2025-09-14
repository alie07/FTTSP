/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  CLASS	        : CSetupGateway		                                      */
/*  Description     : Class which contains api to interface between		      */
/*					  FTTSP and subcontractor tools							  */
/*                                                                            */
/*  File            : SetupGateway.h			                              */
/*                                                                            */
/*  Scope           : Public                                                  */
/*                                                                            */
/*  Target          :                                                         */
/*                                                                            */
/*  Coding language : C++                                                     */
/*                                                                            */
/* COPYRIGHT 2012 VAULT-IC France                                             */
/* all rights reserved                                                        */
/*                                                                            */
/******************************************************************************/

/***************************  REVISION HISTORY  *******************************
 * 
 *    Rev 1.0:   21 March 2012 CDESBOUCHAGES
 *    
 * Creation: 
 *
 ******************************************************************************/

/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/
#include "stdafx.h"
#include <io.h>

/******************************************************************************/
/* INCLUSION OF OTHER MODULES HEADERS                                         */
/******************************************************************************/
#include "fttsp.h"

/******************************************************************************/
/* INCLUSION OF OWNER MODULES HEADERS                                         */
/******************************************************************************/
#include "SetupGateway.h"

/******************************************************************************/
/* LOCAL DEFINED CONSTANTS				                                      */
/******************************************************************************/
#define NO_SUBCONTRACTOR					((int)0)
#define GATEWAY_TYPE_FILE_UTAC				((int)1)

#define MAX_GATEWAY_TYPE					((int)1)
#define MAX_LENGHT_OF_COORD					((int)4)

#define CONFIG_KEY_SUBCONTRACTOR			"SUBCONTRACTOR"
#define CONFIG_FIELD_SUB_NAME				"Name"
#define CONFIG_FIELD_TYPE_INTERFACE			"Kind_Of_Interface"
#define CONFIG_FIELD_PATH_GATEWAY_FILE		"Path_Of_File"
#define CONFIG_FIELD_NAME_GATEWAY_FILE		"Name_Of_File"
#define CONFIG_FIELD_TAG_PARAM				"_Field_In_File"
#define CONFIG_FIELD_TAG_COORD				"_Data_Coordinates"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************************************************/
/* CLASS IMPLEMENTATION														  */
/******************************************************************************/

/******************************************************************************
 * CONSTRUCTOR : CSetupGateway
 ------------------------------------------------------------------------------
 *	Take by default "config.ini" file in current directory.
 *	
 ******************************************************************************/
CSetupGateway::CSetupGateway()
{
	InitClass();
	m_szConfigFile=CONFIG_FILE;
	m_bGatewayFound=bSearchSubcontractor();
}

/******************************************************************************
 * OVERLOAD CONSTRUCTOR : CSetupGateway
 ------------------------------------------------------------------------------
 * INPUT : szConfigFile
    Comment : Path of the config.ini file which possibly contains subcontractor
	          gateway mechanism
    Range   : nor NULL 
    Unit    : CString
 ******************************************************************************/
CSetupGateway::CSetupGateway(CString szConfigFile)
{
	InitClass();
	m_szConfigFile=szConfigFile;
	m_bGatewayFound=bSearchSubcontractor();	
}

/******************************************************************************
 * DESTRUCTOR : CSetupGateway
 ******************************************************************************/
CSetupGateway::~CSetupGateway()
{
	/* Free le list of parameters */
	m_ListParams.RemoveAll();
}

/******************************************************************************
 * METHOD NAME: InitClass
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PROTECTED
	Initialization of all internal class variables 
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : 
 *****************************************************************************/
void CSetupGateway::InitClass(void)
{
	m_bGatewayFound = FALSE;			
	m_szSubcontractorName.Empty();
	m_szConfigFile.Empty();				
	m_szGatewayFile.Empty();			
	m_szProductName.Empty();			
	m_szProductRev.Empty();				
	m_szPartID.Empty();					
	m_szFlow.Empty();					
	m_szLotNum.Empty();	
	m_szSubLotNum.Empty();
}

/******************************************************************************
 * METHOD NAME: szGetSubcontractor
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the name of subcontractor from config.ini file
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetSubcontractor(void)
{
	return m_szSubcontractorName;
}

/******************************************************************************
 * METHOD NAME: bGatewayFound
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the status on the gateway data recovery. If the config.ini file is 
	set to use FTTSP with a gateway mecanism, the this flag is set.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : If the class has recovered all data then the flag is set to TRUE.
			  Else, if no gateway mecanism is requested OR the config.ini is 
			  not well write, then the flag is set to FALSE.
    Range   : TRUE or FALSE
    Unit    : BOOL
 *****************************************************************************/
BOOL CSetupGateway::bGatewayFound(void)
{
	return m_bGatewayFound;
}
/******************************************************************************
 * METHOD NAME: szGetProductName
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the product name from gateway
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetProductName(void)
{
	return m_szProductName;
}

/******************************************************************************
 * METHOD NAME: szGetGatewayFile
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the path and name of the file used as gateway
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetGatewayFile(void)
{
	return m_szGatewayFile;
}


/******************************************************************************
 * METHOD NAME: szGetProductRev
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the product design revision from gateway
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetProductRev(void)
{
	return m_szProductRev;
}

/******************************************************************************
 * METHOD NAME: szGetPartID
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the part ID from gateway
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetPartID(void)
{
	return m_szPartID;
}

/******************************************************************************
 * METHOD NAME: szGetFlow
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the flow from gateway
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetFlow(void)
{
	return m_szFlow;
}

/******************************************************************************
 * METHOD NAME: szGetLotNumber
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the lot number from gateway INSIDE SECURE STD (ex: 1T1234)
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetLotNumber(void)
{
	return m_szLotNum;
}

/******************************************************************************
 * METHOD NAME: szGetSubLotNumber
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PUBLIC
	Return the lot number from gateway subcontractor  STD (ex:ICWPE0880.4)
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
    Range   : 
    Unit    : CString
 *****************************************************************************/
CString CSetupGateway::szGetSubLotNumber(void)
{
	return m_szSubLotNum;
}
/******************************************************************************
 * METHOD NAME: bSearchSubcontractor
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PROTECTED
	Read into the config.ini file if a subcontractor gateway is set.
	If it is:
	- read the subcontractor name
	- check the kind of gateway
	- Retrieve all data
	
	then return TRUE if all parameters are well set.Else return FALSE if no
	gateway is requested or one of the needed parameters is not good.
 ------------------------------------------------------------------------------
 * INPUT : -
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : 
	Range   : TRUE or FALSE			  
    Unit    : BOOL
 *****************************************************************************/
BOOL CSetupGateway::bSearchSubcontractor(void)
{
BOOL bRet=FALSE;
CString szTemp("");
int nTemp=0;

	/* config.ini file exists? */
	int nFileFound=_access(m_szConfigFile,00);
	if ( nFileFound!=0 )
	{
		/* config.ini is not found! */
		return FALSE;
	}
	/* Yes! check if subcontractor mecanism is requested */
	nTemp=GetPrivateProfileString(CONFIG_KEY_SUBCONTRACTOR,CONFIG_FIELD_SUB_NAME,NULL,szTemp.GetBuffer(100),100,m_szConfigFile);
	szTemp.ReleaseBuffer();
	if (nTemp<=0)
	{
		/* No subcontractor found! */
		return FALSE;
	}
	/* subcontractor name found! Save it */
	m_szSubcontractorName=szTemp;
	/* Check the type of interface */
	nTemp=GetPrivateProfileInt(CONFIG_KEY_SUBCONTRACTOR,CONFIG_FIELD_TYPE_INTERFACE,NO_SUBCONTRACTOR,m_szConfigFile);
	if ( (nTemp>NO_SUBCONTRACTOR) && (nTemp<=MAX_GATEWAY_TYPE) )
	{	
		/* Valid gateway mecanism is found! Save it. */
		m_wGatewayType=nTemp;
		/* Extract all needed information from gateway to FTTSP */
		bRet=bExtractData(nTemp);
		return bRet;
	}
	else
	{
		/* no valid gateway mechanism found! */
		return FALSE;
	}
}

/******************************************************************************
 * METHOD NAME: bExtractData
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PROTECTED
	This method extract data from gateway interface selected. 
	In case of multiple type of gateway, the method consists to add the function
	adress of the method gateway type into the array afpKindOfTreatment
 ------------------------------------------------------------------------------
 * INPUT : nTypeOfGateway
    Comment : gateway type selected
    Range   : 1 to MAX_GATEWAY_TYPE
    Unit    : int
 ------------------------------------------------------------------------------
  * OUTPUT : 
    Comment : TRUE  => data extracted successfully
			  FALSE => problem on data extracion
    Unit    : BOOL
 *****************************************************************************/
BOOL CSetupGateway::bExtractData(int nTypeOfGateway)
{
BOOL bRet=FALSE;
/* Array of function pointer
 * To add a function, just increase 
 * MAX_GATEWAY_TYPE and add adress of 
 * the new fonction into the array  
*/
typedef BOOL (CSetupGateway::*fp)();		
fp afpKindOfTreatment[MAX_GATEWAY_TYPE] ={ &CSetupGateway::bExtractDataFromFile };			// Read data needed for FTTSP into a specific file parsed as UTAC 
	
	/* Execution of the wanted gateway treatment */
	bRet=(this->*afpKindOfTreatment[nTypeOfGateway-1])();
	return bRet;	
}

/******************************************************************************
 * METHOD NAME: bExtractDataFromFile
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PROTECTED
	Extract all data from specific file of subcontractor. (UTAC like)
 ------------------------------------------------------------------------------
 * INPUT : 
    Comment : 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : 
    Comment : TRUE  => data extracted successfully
			  FALSE => problem on data extracion
    Unit    : BOOL
 *****************************************************************************/
BOOL CSetupGateway::bExtractDataFromFile(void)
{
BOOL bRet=FALSE;
CString szTmp("");
int nTemp=0;
	
	/* Retrieve the name and the path of the gateway file*/
	nTemp=GetPrivateProfileString(CONFIG_KEY_SUBCONTRACTOR,CONFIG_FIELD_PATH_GATEWAY_FILE,NULL,m_szGatewayFile.GetBuffer(MAX_PATH),MAX_PATH,m_szConfigFile);
	m_szGatewayFile.ReleaseBuffer();
	if (nTemp<=0)
	{
		/* No field 'Path_Of_File' found */
		return FALSE;
	}
	nTemp=GetPrivateProfileString(CONFIG_KEY_SUBCONTRACTOR,CONFIG_FIELD_NAME_GATEWAY_FILE,NULL,szTmp.GetBuffer(MAX_PATH),MAX_PATH,m_szConfigFile);
	szTmp.ReleaseBuffer();
	if (nTemp<=0)
	{
		/* No field 'Name_Of_File' found */
		return FALSE;
	}
	/* Read all data from gateway file */
	if (m_szGatewayFile.Right(1)!='\\')
	{
		m_szGatewayFile+="\\";
	}
	m_szGatewayFile+=szTmp;
	bRet=bParseFileTypeUTAC(m_szGatewayFile);
	if (bRet==FALSE)
	{
		/* Not possible to read correctly the gateway file */
		return FALSE;
	}
	/* Extract all data for FTTSP */
	bRet =bExtractField("Product",m_szProductName);
	bRet&=bExtractField("Product_Rev",m_szProductRev);
	bRet&=bExtractField("Part_ID",m_szPartID);
	bRet&=bExtractField("Flow",m_szFlow);
	bRet&=bExtractField("Lot_Number",m_szLotNum);
	bRet&=bExtractField("Sub_Lot_Number",m_szSubLotNum);

	return bRet;
}
/******************************************************************************
 * METHOD NAME: bExtractField
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PROTECTED
 	each field retrieved are set as below:
	-XXX_Field_In_File(from config.ini) : field to parse into subcontractor file
	-XXX_Data_Coordinates(from config.ini) : x,y 
											where x is the begin of string
											where y is the lenght of the data 
											and XXX the wanted field
	ex: config.ini
	Product_Field_In_File=product_no
	Product_Data_Coordinates=0,5
	
	into lot.txt(from sucontractor file)
	product_no=58829D/05
	The returned string is "58829" from index 0 and lenght=5.

 ------------------------------------------------------------------------------
 * INPUT : szParam
    Comment : Field param to retrieve into the config.ini
    Range   : Not NULL
    Unit    : CString
 ------------------------------------------------------------------------------
  * INPUT : szClassMember by reference
    Comment : param updated from the subcontractor file.
    Range   : 
    Unit    : CString
 ------------------------------------------------------------------------------
 * OUTPUT : -
    Comment : 
			  
    Range   : TRUE  => successfull done
			  FALSE => ERROR
    Unit    : BOOL
 *****************************************************************************/
BOOL CSetupGateway::bExtractField(CString szParam,CString& szClassMember)
{
CString szTmp("");
CString szKey("");
CString szValue("");
CString szCoord("");
CString szToken("");
int nTemp=0;
	
	/* Read into config.ini the param name */
	if ( szParam.IsEmpty() )
	{
		return FALSE;
	}
	szTmp=szParam;
	szTmp+=CONFIG_FIELD_TAG_PARAM;
	nTemp=GetPrivateProfileString(CONFIG_KEY_SUBCONTRACTOR,szTmp,NULL,szKey.GetBuffer(MAX_PATH),MAX_PATH,m_szConfigFile);
	szKey.ReleaseBuffer();
	if (nTemp<=0)
	{
		/* Parameter not found into config.ini */
		return FALSE;
	}
	/* Found the parameter into the list */
	if ( !m_ListParams.Lookup(szKey,szValue) || szValue.IsEmpty() )
	{
		return FALSE;
	}
	/* Retrieve the value depending to the coordinates into config.ini */
	szTmp=szParam;
	szTmp+=CONFIG_FIELD_TAG_COORD;
	nTemp=GetPrivateProfileString(CONFIG_KEY_SUBCONTRACTOR,szTmp,NULL,szCoord.GetBuffer(MAX_PATH),MAX_PATH,m_szConfigFile);
	szCoord.ReleaseBuffer();
	if (nTemp<=0)
	{
		/* Coordinates not found into config.ini */
		return FALSE;
	}
	/* Read the begin index */
	szToken=strtok(szCoord.GetBuffer(MAX_LENGHT_OF_COORD),",\n");
	szCoord.ReleaseBuffer();
	if ( szToken.IsEmpty() )
	{
		/* Index coordinates is not valid! */
		return FALSE;
	}
	/* Decimal conversion */
	nTemp=(int)strtol(szToken,NULL,10);
	/* read the data lenght */
	szToken=strtok(NULL,",\n");
	if ( szToken.IsEmpty() )
	{
		/* Lenght coordinates is not valid! */
		return FALSE;
	}
	/* Retrieve the needed data */
	szClassMember=szValue.Mid(nTemp,(int)strtol(szToken,NULL,10));
	return TRUE;
}


/******************************************************************************
 * METHOD NAME: bParseFileTypeUTAC
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION : PROTECTED
	This method open the a file which is containing all parameters
	and create a list with all these parameters. Then it will be possible 
	to retrieve easily all usefull data to create the Gateway from 
	subcontractors and FTTSP.
 ------------------------------------------------------------------------------
 * INPUT : szFileGateway
    Comment : path and name of file 
    Range   : 
    Unit    : 
 ------------------------------------------------------------------------------
  * OUTPUT : -
    Comment : return value will serve to know if the list of parameters is 
			  correctly created.
    Range   : TRUE  => successfull done
			  FALSE => ERROR
    Unit    : BOOL
 *****************************************************************************/
BOOL CSetupGateway::bParseFileTypeUTAC(CString szFileGateway)
{
	/* Try to open the file from UTAC Autoloader */
	CStdioFile GatewayFile;
	
	if (! GatewayFile.Open(szFileGateway, CFile::modeRead | CFile::shareDenyNone | CFile::typeText))
	{
		/* No file is found! */
		return FALSE;
	}
	
	/* Read the content of the gateway file */
	/* and create a map with all parameters */

	CString strLine;
	CString strParam;
	CString strValue;

	int s32PosToken=-1;
	while (GatewayFile.ReadString(strLine))
	{
		/* Find the '=' caracter */
		s32PosToken=strLine.Find("=");
		if (s32PosToken>0)
		{
			/* '=' found! Fill the list parameters */
			strParam=strLine.Left(s32PosToken);
			strValue=strLine.Mid(s32PosToken+1);
			m_ListParams.SetAt(strParam,strValue);
		}
	}
	GatewayFile.Close();

	return TRUE;
}