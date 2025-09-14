/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*  CLASS	        : CSetupGateway		                                      */
/*  Description     : Class which contains api to interface between		      */
/*					  UTAC Autoloader and FTTSP								  */
/*                                                                            */
/*  File            : GatewayUTAC.h				                              */
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
 *    Rev 1.0:   19 Sept 2011 CDESBOUCHAGES
 *    
 * Creation: 
 *
 ******************************************************************************/

#if !defined(AFX_SETUPGATEWAY_H__F61C6502_F5DB_45E7_A9D3_AB4FFABD5D96__INCLUDED_)
#define AFX_SETUPGATEWAY_H__F61C6502_F5DB_45E7_A9D3_AB4FFABD5D96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/

/******************************************************************************/
/* INCLUSION OF OTHER MODULES HEADERS                                         */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTIONS						                                      */
/******************************************************************************/

/******************************************************************************/
/* CLASS DEFINITION															  */
/******************************************************************************/

class CSetupGateway  
{

/* Constructor/Destructor */
public:

	CSetupGateway();					
	CSetupGateway(CString szConfigFile);
	virtual ~CSetupGateway();

/* Interface of methods */
	BOOL bGatewayFound(void);						// Getter to return if a mechanism is found to interface beetwen subcontractor and FTTSP
	int nGetGatewayType(void);						// Getter to return the type of gateway interface.
	CString szGetSubcontractor(void);						// Getter to return the name of the subcontractor.
	CString szGetProductName(void);					// Getter to return the product name from the gateway
	CString szGetProductRev(void);					// Getter to return the product revision from the gateway
	CString szGetPartID(void);						// Getter to return the part ID from the gateway
	CString szGetFlow(void);						// Getter to return the flow from the gateway
	CString szGetLotNumber(void);					// Getter to return the lot number from the gateway INSIDE SECURE STD
	CString szGetSubLotNumber(void);				// Getter to return the lot number from the gateway subcontractor STD
	CString szGetGatewayFile(void);					// Getter to return the path and name of file used as gateway

protected:
	void InitClass(void);							// Initialization of all internal variables
	BOOL bSearchSubcontractor(void);				// Read the config.ini to check if gateway mecanism is requested
	BOOL bExtractData(int);							// Generic method to extract data from kind of gateway mecanism
	/* Extract data from gateway file */
	BOOL bExtractDataFromFile(void);	 			// Extract data from file (UTAC like)
	BOOL bParseFileTypeUTAC(CString szFileGateway);	// PArse all parameters present into gateway file.
	BOOL bExtractField(CString szParam,CString& szClassMember); // Retrieve the data 'szParam' into the list 'm_ListParams' created from the gateway file
	

/* Interface of attributs */
protected:
	BOOL m_bGatewayFound	;						// Gateway found or not!
	int	m_wGatewayType;								// Type of gateway used to interface with FTTSP
													// => 1: specific file is used to read back product information. (UTAC like)

	CString m_szSubcontractorName;					// If gateway found, then contains the name of the subcontractor 	
	CString m_szConfigFile;							// path and name of the config.ini file
	CString m_szGatewayFile;						// path and name of file from UTAC ("lot.txt")
	CString m_szProductName;						// Product name extracted from "lot.txt"
	CString m_szProductRev;							// Design revision extracted from "lot.txt"
	CString m_szPartID;								// Part ID extracted from "lot.txt"
	CString m_szFlow;								// Flow extracted from "lot.txt"
	CString m_szLotNum;								// Lot number "Inside Secure STD" from "lot.txt"
	CString	m_szSubLotNum;							// Lot Number "UTAC" from lot.txt

	CMapStringToString m_ListParams;				// List created for parsing gateway file parameters 

};

#endif // !defined(AFX_SETUPGATEWAY_H__F61C6502_F5DB_45E7_A9D3_AB4FFABD5D96__INCLUDED_)
