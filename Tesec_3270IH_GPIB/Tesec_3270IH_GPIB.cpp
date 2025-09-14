/******************************************************************************/
/*                                                                            */
/*  Layer           : DLL			                                          */
/*                                                                            */
/*  Module          : TESEC_3270IH_GPIB                                       */
/*  Description     : Abstraction layer between the TESEC handler and the     */
/*                    FTTSP program                                           */
/*                                                                            */
/*  File            : TESEC_3270IH_GPIB.cpp                                   */
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

  // rev 1.5 : pgy 05 july 2013
  //			change HndBin to use test mask instead the number of dut tested.


//pgy : aucune info sur les modi 1.2-1.3-1.4 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 * 
 *    Rev 1.1:   90 Mar 2012 CDESBOUCHAGES
 *
 * - Modification of the HndInit function. The previous version couldn't connect
 *   to the handler.
 * 
 *    Rev 1.0:   24 Aug 2011 CDESBOUCHAGES
 *    
 * Creation of this DLL according to: 
 * - The specification for 3270-IH tester interface 2002.7.22
 * - The TESEC capture spy file dated 24/01/2011
 *
 ******************************************************************************/

/* Verify "%NIEXTCCOMPILERSUPP%\include" is declared in the include files
 * Verify "%NIEXTCCOMPILERSUPP%\LIB32\MSVC" is declared in the lib files
 */

/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

/******************************************************************************/
/* INCLUSION OF OTHER MODULES HEADERS                                         */
/******************************************************************************/
#include <decl-32.h>

/******************************************************************************/
/* INCLUSION OF OWNER HEADERS                                                 */
/******************************************************************************/
/* Declare as exporting the dll */
#define HANDLERDLL_EX
#include "../HandlerDll.h"


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Local MACROS AND DEFINED CONSTANTS                                         */
/******************************************************************************/

	/* MISCELLANEOUS INFORMATION */

#define RET_ERROR				((HNDID)0)								// Error return code
#define acBufferSIZE				((int)2048)								// Size of acBuffer
#define DLL_MAJOR_VERSION		((unsigned char)1)						// Major version identification
#define DLL_MINOR_VERSION		((unsigned char)7)						// Minor version identification
#define DEV_CONNECT_TIMEOUT		((int)10)								// 10 tentatives of connection
#define LISTENER_FOUND			((short)1)								// 

	/* GPIB INFORMATION */


/* ************** */
/* works per pair */
/**/  #define BDINDEX			((int)0)								// Board Index
/**/  #define BDNAME			"GPIB0"									// Board Name linked to board Index
/**/
/**/  #define PAD_OF_HANDLER	((int)9)								// Primary address of device
/**/  #define PAD_NAME			"DEV9"									// Device Name linked to the primary Index
/* ************** */
#define SAD_OF_HANDLER			((int)NO_SAD)							// Secondary address of device

#define TIMEOUT					((int)T10s)								// Timeout value = 10 seconds

#define MAX_GPIB_ERR_MESSAGE	((int)29)								// Number of possible GPIB error messages

#define DEV_ENABLED				((int)1)								// Device enabled on the GPIB line
#define DEV_DISABLED			((int)0)								// Device disabled on the GPIB line

	/* TESEC INFORMATION */

#define MAX_AVAILABLE_SOCKETS	((unsigned int)16)						// TESEC is able to manage 16 sockets
#define MAX_RESPONSE_BYTES		((unsigned int)MAX_AVAILABLE_SOCKETS/4) // Nb of byte into the 0x44 frame depending presence of test stations

#define DEVICE_IN_TEST_POSITION	((char)0x40)							// A device is in test position in any test station  
#define TEST_START_ACK_MESSAGE	"D"										// 0x44  Message to aknowledge the Test Start Request
#define HEADER_DUT_DATA			'D'										// 0x44 : Response from Tesec on DUT information 
#define HEADER_FRAME_BIN		"S@"									// 0x53 0x40 : Sort signal + Nb site
#define END_FRAME_BIN			"Q\r\n"										// 0x51 : Tests are finished.
#define TEST_RESULT_PATTERN		((char)0x40)							// used for the sorting 
#define MASK_POSSIBLE_SORT		((char)0x3F)							// Sort possible 1 to 32. (bit mask)
#define NB_MAX_BIN				((DWORD)32)								// Nb bin present onto TESEC
typedef DWORD		HNDID;

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/
	
/* To read and write on GPIB */
char acBuffer[acBufferSIZE + 1];

/* Error returns by GPIB interface */
char acErrorMnemonic[MAX_GPIB_ERR_MESSAGE][5] = { "EDVR", "ECIC", "ENOL", "EADR", "EARG",
												"ESAC", "EABO", "ENEB", "EDMA", "",
												"EOIP", "ECAP", "EFSO", "",     "EBUS",
												"ESTB", "ESRQ", "",     "",      "",
												"ETAB", "ELCK", "EARM", "EHDL",  "",
												"",     "EWIP", "ERST", "EPWR" };
/* Mask related to the socket available to test */
static DWORD gdwSiteOnTestMask = 0;


/******************************************************************************/
/* DLL Entry point	                                                          */
/******************************************************************************/
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : GPIBCleanup
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	After each GPIB function execution, the application checks
	whether it has successfully executed. If an NI-488.2 call
	fails, the GPIB driver sets the corresponding bit in the
	global status variable. Then GPIBCleanup prints an error
	message, takes the board offline and exits.
 ------------------------------------------------------------------------------
 * INPUT : ud
    Comment : GPIB board address 
    Range   : nor NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : pachErrorMsg
    Comment : error message to write 
    Range   : not NULL
    Unit    : const char *
 ------------------------------------------------------------------------------
 * OUTPUT : void
    Comment : 
    Range   : 
    Unit    : 
 *****************************************************************************/
void GPIBCleanup(HNDID hndDeviceId, const char * pachErrorMsg)
{
	fprintf(stderr, "Error : %s\nibsta = 0x%x iberr = %d (%s)\n",
           pachErrorMsg, ibsta, iberr, acErrorMnemonic[iberr]);
    fprintf(stderr, "Cleanup: Taking board offline\n");
	fflush(stderr);
}

/******************************************************************************
 * FUNCTION NAME : HndGetVers
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Give to application the DLL version 
 ------------------------------------------------------------------------------
 * INPUT : pucMajor
    Comment : pointer to variable application to read the major version
    Range   : 0 to 255
    Unit    : unsigned char
 ------------------------------------------------------------------------------
 * INPUT : pucMinor
    Comment : pointer to variable application to read the minor version
    Range   : 0 to 255
    Unit    : unsigned char
 ------------------------------------------------------------------------------
 * OUTPUT : Boolean
    Comment : -
    Range   : TRUE
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndGetVers (OUT PTBYTE pucMajor, OUT PTBYTE pucMinor)
{
	*pucMajor=DLL_MAJOR_VERSION;
	*pucMinor=DLL_MINOR_VERSION;
	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndInit
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Attempt to connect to the TESEC handler on GPIB. If it is 
	not possible, HndInit returns 0. Else it returns the  
	device handle found
 ------------------------------------------------------------------------------
 * INPUT : options
    Comment : Not Used
    Range   : 
    Unit    : LPCTSTR
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : Handle of the device found 
    Range   : not NULL
    Unit    : HNDID
 *****************************************************************************/
HNDID DLLDIR WINAPI HndInit ( IN LPCTSTR options )
{
HNDID hndDeviceId;	// TESEC handle
short sListener=0;

	/* Attempt to connect to TESEC */
	for (int nLoop=0; (nLoop<DEV_CONNECT_TIMEOUT) && (sListener==0); nLoop++)
	{
		/* Check the presence of device on the bus */
		ibln(ibfind(BDNAME),PAD_OF_HANDLER,NO_SAD,&sListener);
		/* Opens the device and returns the handle associated */
		hndDeviceId=ibfind(PAD_NAME);
	}
	
	if (sListener!=LISTENER_FOUND)
	{
		/* No board or device found! */
		return RET_ERROR;
	}
	ibclr(hndDeviceId);
	return hndDeviceId;
}

/******************************************************************************
 * FUNCTION NAME : HndPoll
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function waits a Test Start Request from the TESEC 
	handler. If it is asserted, then it returns the mask of
	available socket ready to test.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : Tesec handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : uiMax_uiSkts
    Comment : Number of site to managed
    Range   : 0 to MAX_AVAILABLE_SOCKETS
    Unit    : size_t
------------------------------------------------------------------------------
 * INPUT : pdwActive_Sockets
    Comment : pointer to the mask of active sites 
    Range   : NOT NULL
    Unit    : LPDWORD
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => either timout on SQR or normal exit
			  FALSE => Error ==> pdwActive_Sockets[0]=0
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t uiMask_uiSkts, OUT LPDWORD pdwActive_Sockets )
{
char cSerialPollResponse=0;
int iStrLengh=0;
unsigned int uiNbResponses=0;
unsigned int uiNbShiftLeft=0;
char *pcIndex=NULL;

	/* Init */
	pdwActive_Sockets[0]=0;
	gdwSiteOnTestMask=0;
	memset(acBuffer,'\0',acBufferSIZE);

	/* Check the serial poll response byte from TESEC */
	ibrsp(hndid, &cSerialPollResponse);
	if (ibsta & ERR)
	{
		/* Error */
		return FALSE;
	}

	/* Check if a device is in test position in any test station */
	if (cSerialPollResponse != DEVICE_IN_TEST_POSITION)
	{
		/* no device in test position ! */
		return FALSE;
	}

	/* Acknowledge Test Start  */
	ibwrt(hndid,"D\r\n",3);

	/* Read DUT informations */
	if (! (HndGet(hndid,acBufferSIZE,acBuffer)) ) 
		return FALSE;	// Error on receiving data to Tesec

	/* Check if DUT pattern is present */
	if (acBuffer[0]!=HEADER_DUT_DATA)
	{
		/* no device in test position ! */
		return FALSE;
	}

	/* Search the EOI pattern to determine */
    /* hom many byte have been received    */
	uiNbResponses=(unsigned int)( strlen(acBuffer) ); // Index of EOI
	uiNbResponses-=2;								  // substract 1 because of first byte in acBuffer ('D' or 0x44)
	/* Check if a valid message is received */
	if ( (uiNbResponses<=0) || (uiNbResponses>MAX_RESPONSE_BYTES) )
	{
		return FALSE;
	}
	
	/* Calculate the pdwActive_Sockets mask */
	int nIndex=1;
	uiNbResponses-=1;
	if (uiNbResponses>1)
	{
		for (int i=uiNbResponses; i>0; i--)
		{	
			int nNbShift= i<<2;
			int nData=acBuffer[nIndex++]&0x0F;
			gdwSiteOnTestMask|=(DWORD)(nData<<nNbShift);
		}
	}
	gdwSiteOnTestMask|=(DWORD)(acBuffer[nIndex]&0x0F);
	/* Match the pdwActive_Sockets mask according to the asked mask */
//	pdwActive_Sockets[0] = gdwSiteOnTestMask & ((1 << uiMax_uiSkts) - 1);
	pdwActive_Sockets[0] = gdwSiteOnTestMask & uiMask_uiSkts;
	return TRUE;

}

/******************************************************************************
 * FUNCTION NAME : HndBin
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	After the end of test, all sites tested have to be sorted
	into the wanted bin. 
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : Tesec handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : uiSkts
    Comment : Number of tested site
    Range   : 0 to MAX_AVAILABLE_SOCKETS
    Unit    : size_t
------------------------------------------------------------------------------
 * INPUT : pdwBins
    Comment : Pointer to bin information per site
    Range   : not NULL
    Unit    : LPDWORD
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => sorting successfully done
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t TestMask, IN LPDWORD pdwBins ) 
{

DWORD Bin;
char *pcBuffer = &acBuffer[1];			// Point onto the nb of test result 

	/* Init */
	memset(acBuffer,'\0',acBufferSIZE);
	
	/* Prepare the Sort frame */
	strcpy(acBuffer,HEADER_FRAME_BIN);	// 0x53, 0x40 

	//count nb DUT active in the TestMask
	int nbActiveDUT=0;
	for (int i=0; i<32; i++)
	{
		if ( (TestMask&(1<<i))!=0 )
			nbActiveDUT++;
	}

	*pcBuffer++|=nbActiveDUT;			// 0x40 | NbActive DUT
	
	for ( i=31; i>=0; i--)
	{
		if ( (TestMask&(1<<i))!=0 )	
		{
			Bin = pdwBins[i];

			//only bin 1 to 6 are allowed
			if (Bin>6 || Bin==0)
				Bin = 6;

			//add the result in the GPIB cmd
			*pcBuffer=TEST_RESULT_PATTERN;
			*pcBuffer|=((char)(Bin))&MASK_POSSIBLE_SORT; 			
			pcBuffer++;
		}
	}
	
	/* Prepare the end of frame */
	strcpy(pcBuffer,END_FRAME_BIN);	//0x51  	

	/* Transmit to Tesec */
	ibwrt(hndid,acBuffer,(int)((pcBuffer+3)-acBuffer));

	/* End of test */
	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndSet
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function sends a pszCommand to the Tesec.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : Tesec handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : pszCommand
    Comment : pszCommand to send on the GPIB line bus
    Range   : not NULL
    Unit    : LPCTSTR
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => pszCommand successfully sent
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndSet ( IN HNDID hndid, IN LPCTSTR pszCommand )
{

	int i=strlen(pszCommand);
	char tab[256];
	strcpy(tab,pszCommand);		
	ibwrt(hndid, /*(PVOID)*/ tab, 1);
	if (ibsta & ERR)
	{
		//GPIBCleanup(hndid, "Cannot Send pszCommand");
		return FALSE;
	}
	fprintf(stderr,"HndSet: %04x '%s'\n", ibsta, pszCommand);
	fflush(stderr);

	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndGet
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function read information from Tesec.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : Tesec handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : uiBuffer_size
    Comment : size of the acBuffer used. 
    Range   : >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : pszAnswer
    Comment : acBuffer pointer used to write data
    Range   : not NULL
    Unit    : LPCTSTR
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => Data successfully read
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndGet ( IN HNDID hndid, IN size_t uiBuffer_size, OUT LPTSTR pszAnswer )
{
	/* Check if the hndid and uiBuffer_size are valid*/ 
//	if ( (hndid<=0) || (uiBuffer_size<=0) )
//		return FALSE;

	ibrd(hndid, pszAnswer, uiBuffer_size);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Cannot Get data");
		return FALSE;
	}
	/*
	 * Assume that the returned string contains ASCII data. NULL
	 * terminate the string using the value in ibcntl which is the
	 * number of bytes read in.
	 */
	pszAnswer[ibcnt] = '\0';

	fprintf(stderr,"HndGet: %04x '%s'\n", ibsta, pszAnswer);
	fflush(stderr);

	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndGet
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function discard the Tesec connection.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : Tesec handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => pszCommand successfully
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndRelease ( IN HNDID hndid )
{

	/* Take the requested device offline */
	ibonl( hndid, 0);
	if (ibsta & ERR)
	{
		fprintf(stderr,"Cannot close the Tesec Communication.\n");
		fflush(stderr);
		return FALSE;	
	}
	else
	{
		return TRUE;
	}
}


#ifdef __cplusplus
}
#endif

