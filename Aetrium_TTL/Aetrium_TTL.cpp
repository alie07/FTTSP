/******************************************************************************/
/*                                                                            */
/*  Layer           : DLL			                                          */
/*                                                                            */
/*  Module          : AETRIUM_TTL	                                          */
/*  Description     : Abstraction layer between the AETRIUM handler and the   */
/*                    FTTSP program                                           */
/*                                                                            */
/*  File            : AETRIUM__TTL.cpp										  */
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
 *    Rev 1.0:   06 December 2012 CDESBOUCHAGES
 *    
 * Creation of this DLL according to: 
 *
 ******************************************************************************/

/* Verify "%NIEXTCCOMPILERSUPP%\include" is declared in the include files
 * Verify "%NIEXTCCOMPILERSUPP%\LIB32\MSVC" is declared in the lib files
 */

/******************************************************************************/
/* INCLUSION OF STANDARD HEADERS                                              */
/******************************************************************************/
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>

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

using namespace std; 


#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* Local DEFINED CONSTANTS				                                      */
/******************************************************************************/

	/* MISCELLANEOUS INFORMATION */

#define RET_ERROR					((HNDID)0)								// Error return code
#define BUFFERSIZE					((int)2048)								// Size of as8as8Buffer
#define DLL_MAJOR_VERSION			((unsigned char)1)						// Major version identification
#define DLL_MINOR_VERSION			((unsigned char)4)						// Minor version identification
#define END_CONF_TABLE_GPIB			""										// marker to generate end of table
#define TIMEOUT_5S					((double)5.0)							// Timout of 5s
	
	/* GPIB INFORMATION */

#define BDINDEX						((int)0)								// Board Index
#define PAD_OF_HANDLER				((int)4)								// Primary address of device
#define SAD_OF_HANDLER				((int)NO_SAD)							// Secondary address of device
#define TIMEOUT						((int)T10s)								// Timeout value = 10 seconds

#define EOTMODE						((int)0x01)								// zero    : EOI line will not be asserted at the end of write operation.
																			// non-zero: EOI line will be asserted at the end of write

#define EOSMODE						((int)0x00)								// 0x00 Disable the EOS mode
																			// 0x04 Terminate read when EOS is detected
																			// 0x08 Set EOI with EOS on write function
																			// 0x10 Compare all 8 bits of EOS byte rather than low 7 bits(all read and write calls).
#define MAX_GPIB_ERR_MESSAGE		((int)29)								// Number of possible GPIB error messages
#define DEV_ENABLED					((int)1)								// Device enabled on the GPIB line
#define DEV_DISABLED				((int)0)								// Device disabled on the GPIB line

#define BIN_PORT_DUT1				((TBYTE)3)								// The DUT1 has its bin data plugged on port 3	
#define EOT_PORT					((TBYTE)2)								// All EOT lines are plugged on port 2.
#define MASK_SOT_EVENT				((TBYTE)0x80)							// Mask apply to the Status byte register of the 4863 device to detect a SOT_PARALLEL signal
#define MASK_CMD_ERROR_ESR			((TBYTE)0x20)							// Mask linked to command error of ESR register
#define MASK_SOT_DETECTION			((TBYTE)0x48)							// MAsk linked to the Operation register Evt : SQR + StatusA from OER register

#define CMD_READ_IDENTIFICATION		"*IDN?"									// Sent to 4863 to read back the identity 
#define CMD_IDENTITY_4863			"4863"									// Part of message sent by 4863 in response to MESS_IDENTIFICATION
#define CMD_READ_QUEST_EVT_REG		"STAT:QUES:EVEN?"							// Command to read questionable evt register
#define CMD_READ_OPER_EVT_REG		"STAT:OPER:EVEN?"
#define CMD_WRITE_PORT				"SOURCE:DATA:PORT%d %d"					// Common command to set output states
#define CMD_READ_QUES_COND			"STAT:QUES:COND?"						// Command to read questionable input conditions

	/* AETRIUM INFORMATION */

#define MAX_AVAILABLE_SOCKETS		((unsigned int)4)						// AETRIUM is able to manage 4 sockets
#define MAX_NB_BIN					((DWORD)0x08)							// Sort possible 1 to 8. 
	
/******************************************************************************/
/* Local MACROS							                                      */
/******************************************************************************/

/* returns NumBit in bitfield */
#define SET_BIT(NumBit) (1<<(NumBit))								

/* Save message into stderr interface */
#define PRINT_STDERR(Message) do{ fprintf(stderr, Message);\
								  fflush(stderr); }while(0)

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/
	
/* To read and write on GPIB */
static char as8Buffer[BUFFERSIZE + 1];

/* Error returns by GPIB interface */
static const char kas8ErrorMnemonic[MAX_GPIB_ERR_MESSAGE][5] = {   
														"EDVR", "ECIC", "ENOL", "EADR", "EARG",
														"ESAC", "EABO", "ENEB", "EDMA", "",
														"EOIP", "ECAP", "EFSO", "",     "EBUS",
														"ESTB", "ESRQ", "",     "",      "",
														"ETAB", "ELCK", "EARM", "EHDL",  "",
														"",     "EWIP", "ERST", "EPWR" };

/* Table of ICS 4863 behavior configuration */
static const char *kas8MessConfGPIB[] = {
									"*CLS",							// Clear the Status Structure : Do Not Change the Place !!!
									"CONF:INPUT (@1)",				// Byte 1 in input	(SOTx)
								    "CONF:INPUT:POLARITY 0",		// Byte 1 active on '0'	
								    "CONF:INPUT:HANDSHAKE OFF",		// No handshake on input
									"CONF:OUTPUT (@2:6)",			// Bytes 2 to 6 in output (EOT & Binning)
									"CONF:OUTPUT:POLARITY 0",		// Outputs active on '0'	
								    "SOURCE:DATA:PORT2:POL #h00", 	// Set all bits low TRUE Port2 (EOTx)
								    "SOURCE:DATA:PORT3:POL #h00", 	// Set all bits low TRUE Port3 (BIN SITE 1)
								    "SOURCE:DATA:PORT4:POL #h00", 	// Set all bits low TRUE Port4 (BIN SITE 2)
								    "SOURCE:DATA:PORT5:POL #h00", 	// Set all bits low TRUE Port5 (BIN SITE 3)
								    "SOURCE:DATA:PORT6:POL #h00", 	// Set all bits low TRUE Port6 (BIN SITE 4)	
									"CONF:OUTPUT:HANDSHAKE OFF",	// No handshake on output
									"FORMAT:TALK HEX",				// Format to talk is in Hex	
								    "FORMAT:LISTEN HEX",			// Format to listen is in HEX
								    "STAT:QUES:NTR 15",				// Enable the Questionable event reg. on byte1 bit [0..3] (negative transistions)
								    "STAT:QUES:ENAB 15",			// Enable the Questionable event on 4 LSB bits (SOTx)
								    "SOURCE:DATA:PORT3 0",			// BIN for DUT1 deactivated	
								    "SOURCE:DATA:PORT4 0",			// BIN for DUT2 deactivated
								    "SOURCE:DATA:PORT5 0",			// BIN for DUT3 deactivated
								    "SOURCE:DATA:PORT6 0",			// BIN for DUT3 deactivated
								    "SOURCE:DATA:PORT2 0", 			// EOT not active on each DUT line	
									"*ESE 0",
									"STAT:OPER:ENAB 0",
									"*SRE 8",						// Enable the servic request on questionable event register.
									"*CLS",							// Clear the Status Structure : Do Not Change the Place !!!
									END_CONF_TABLE_GPIB				// END OF TABLE! DO NOT CHANGE THE PLACE !!!
									};

/* Mask related to the socket available to test */
static DWORD dwSiteOnTestMask = 0;



/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/
BOOL fCheckStdEvtStatus ( IN HNDID hndid, TBYTE u8Mask  );
BOOL HndSetGet (IN HNDID hndid,IN LPCTSTR szCmd);

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
 * INPUT : ps8ErrorMsg
    Comment : error message to write 
    Range   : not NULL
    Unit    : const char *
 ------------------------------------------------------------------------------
 * OUTPUT : void
    Comment : 
    Range   : 
    Unit    : 
 *****************************************************************************/
void GPIBCleanup(HNDID hndDeviceId, const char * ps8ErrorMsg)
{
	fprintf(stderr, "Error : %s\nibsta = 0x%x iberr = %d (%s)\n",
           ps8ErrorMsg, ibsta, iberr, kas8ErrorMnemonic[iberr]);
    
	PRINT_STDERR("Cleanup: Taking board offline\n");
    /* Disable the board */
	//ibonl(hndDeviceId, DEV_DISABLED);

}

/******************************************************************************
 * FUNCTION NAME : HndGetVers
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Give to application the DLL version 
 ------------------------------------------------------------------------------
 * INPUT : pu8Major
    Comment : pointer to variable application to read the major version
    Range   : 0 to 255
    Unit    : unsigned char
 ------------------------------------------------------------------------------
 * INPUT : pu8Minor
    Comment : pointer to variable application to read the minor version
    Range   : 0 to 255
    Unit    : unsigned char
 ------------------------------------------------------------------------------
 * OUTPUT : Boolean
    Comment : -
    Range   : TRUE
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndGetVers (OUT PTBYTE pu8Major, OUT PTBYTE pu8Minor)
{
	*pu8Major=DLL_MAJOR_VERSION;
	*pu8Minor=DLL_MINOR_VERSION;
	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndInit
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Attempt to connect to the AETRIUM handler on GPIB. If it is 
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
HNDID hndDeviceId;		// AETRIUM handle
char * ps8Token=NULL;	
int s32Loop=0;

	/* Attenpt to connect to ICS 4863 device */
	hndDeviceId = ibdev (BDINDEX, PAD_OF_HANDLER, SAD_OF_HANDLER, TIMEOUT, EOTMODE, EOSMODE);
	if (ibsta & ERR)
	{
		fprintf(stderr, "Unable to open device\nibsta = 0x%x iberr = %d\n", ibsta, iberr);
		fflush(stderr);
		return RET_ERROR;
	}
	/* Connection etablished with a device! Check if it is a ICS 4863 device */
    /* Clear the device. */
	HndSet(hndDeviceId, kas8MessConfGPIB[0]);
	ibclr(hndDeviceId);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndDeviceId, "Unable to clear device");
		return RET_ERROR;
	}
	/* Read identity */
	if (! (HndSet(hndDeviceId, CMD_READ_IDENTIFICATION) && HndGet(hndDeviceId, BUFFERSIZE, as8Buffer)) )
	{
		PRINT_STDERR("Unable to identify the device.\n");
		return RET_ERROR;
	}
	/* Check ID of the device; It should be */
	/* "ICS Electronics,4863, S/N 510123, Rev XX.XX Ver AA.BB.CC" */
	ps8Token = strtok(as8Buffer, ",");
	if (ps8Token != NULL)
	{
		ps8Token = strtok(NULL, ",");
		if (strstr(ps8Token, CMD_IDENTITY_4863) == NULL)
		{
			PRINT_STDERR("Device found is not an 4863 box.\n");
			return RET_ERROR;
		}
	}
	else
	{
		PRINT_STDERR("Device found is not an 4863 box.\n");
		return RET_ERROR;
	}
    /* Clear the device. */
	ibclr(hndDeviceId);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndDeviceId, "Unable to clear device");
		return RET_ERROR;
	}
	/* Set the configuration of the ICS 4863 */
	for(s32Loop=0; kas8MessConfGPIB[s32Loop]!=END_CONF_TABLE_GPIB; s32Loop++)
	{
		
		if ( (! HndSet(hndDeviceId,kas8MessConfGPIB[s32Loop])) ||			// send the cmd
			    fCheckStdEvtStatus(hndDeviceId,MASK_CMD_ERROR_ESR) )		// check if ERR led is ON
		{
			fprintf(stderr, "Error on sending config. message n%d.\n",s32Loop+1);
			fflush(stderr);
			return RET_ERROR;
		}
	}
	HndSet(hndDeviceId, kas8MessConfGPIB[0]);
	ibclr(hndDeviceId);

	return hndDeviceId;
}

/******************************************************************************
 * FUNCTION NAME : HndPoll
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function waits a Test Start Request from the AETRIUM 
	handler. If it is asserted, then it returns the mask of
	available socket ready to test.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : AETRIUM handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : max_skts
    Comment : Number of site to managed
    Range   : 0 to MAX_AVAILABLE_SOCKETS
    Unit    : size_t
------------------------------------------------------------------------------
 * INPUT : active_sockets
    Comment : pointer to the mask of active sites 
    Range   : NOT NULL
    Unit    : LPDWORD
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => either timout on SQR or normal exit
			  FALSE => Error ==> active_sockets[0]=0
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t mask_skts, OUT LPDWORD active_sockets )
{
TBYTE u8SerialPollResponse=0;
DWORD dwMaskMaxSockets=0;
DWORD dwSOTConditions=0;
DWORD dwMemoryCondition=0;

double f64Timeout=0.0;
clock_t time;
BOOL bRet=FALSE;

	/* Init */
	*active_sockets=(DWORD)0;
	memset(as8Buffer,'\0',BUFFERSIZE);

	/* Check if the hndid is valid and max_skts is in accordance with the AETRIUM range */ 
	if ( hndid<=0 )
		return FALSE;

	/* Wait for SRQ activation (Test Start) */
	ibwait(hndid, RQS | TIMO);
	if ( ! (ibsta & RQS) )
	{
		/* Timout occurs? */
		if (ibsta & TIMO)
		{
			PRINT_STDERR("Timout on waiting RQS.\n");
			/* Clear the device. */
			return TRUE;	// YES
		}
		else
		{
			PRINT_STDERR("Error on waiting RQS.\n");
			return FALSE;	// NO! ERROR 
		}
	}
	
	PRINT_STDERR("Handler asserted RQS.\n");
	
	/* Check the serial poll response byte from ICS 4863 and then SOT of handler*/
	ibrsp(hndid, (char*)&u8SerialPollResponse);
	if (ibsta & ERR)
	{
		/* Error */
		GPIBCleanup(hndid, "Error During Serial Poll");
		return FALSE;
	}
	/* Check if SOT has been detected */
	if ( (u8SerialPollResponse&MASK_SOT_DETECTION) != MASK_SOT_DETECTION )
	{
		/* no device in test position ! */
		GPIBCleanup(hndid, "No valid SOT detected");
		return FALSE;
	}
    /* Clear the device. */
	ibclr(hndid);
	/* MASK generation of wanted sockets */
	dwSiteOnTestMask=mask_skts;
	/* MASK generation of MAXIMUM socket managed by the handler */
	dwMaskMaxSockets=SET_BIT(MAX_AVAILABLE_SOCKETS)-1;
	dwSiteOnTestMask&=dwMaskMaxSockets;
	/* Timout management */
	time=clock();
	do{
		/* Read the questionable event register */		
		if ( !HndSetGet(hndid,CMD_READ_QUEST_EVT_REG) )
		{
			PRINT_STDERR("Error on reading conditions.\n");
			/* Clear the device */
			HndSet(hndid, kas8MessConfGPIB[0]);
			ibclr(hndid);
			return FALSE;
		}
		/* Convert ASCII to dec */
		sscanf(as8Buffer, "%d",&dwSOTConditions);
		/* Compute the memory of the SOT conditions */
		dwMemoryCondition|=dwSOTConditions;
		dwMemoryCondition&=dwSiteOnTestMask;
		/* Check if all wanted SOT occured (SYNCHRONOUS TEST) */
		if (dwSiteOnTestMask==dwMemoryCondition)
		{
			*active_sockets = dwMemoryCondition;
			bRet=TRUE;
			break;
		}
		Sleep(100);
		/* Check if timout occur */
		f64Timeout=(double)(clock()-time);
		f64Timeout/=(double)CLOCKS_PER_SEC;
	}while(f64Timeout< TIMEOUT_5S); 

	/* Init the GPIB and filter to discard bounces */
	do{
		/* Clear the device */
		HndSet(hndid, kas8MessConfGPIB[0]);
		HndSetGet(hndid,CMD_READ_QUEST_EVT_REG);
		HndSetGet(hndid,CMD_READ_OPER_EVT_REG);
		ibrsp(hndid, (char*)&u8SerialPollResponse);
		u8SerialPollResponse&=MASK_SOT_DETECTION;
	}while (u8SerialPollResponse!=0);
	
	ibclr(hndid);
	return bRet;
}

/******************************************************************************
 * FUNCTION NAME : HndSetGet
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function will write a command and read the response with the GPIB 
	device. The response is placed into global variable as8Buffer
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : AETRIUM handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : szCmd
    Comment : string command
    Range   : Not NULL
    Unit    : LPCTSTR
 *****************************************************************************/
BOOL HndSetGet (IN HNDID hndid,IN LPCTSTR szCmd)
{
BOOL bRet=FALSE;
	
	bRet=HndSet(hndid,szCmd);
	bRet&=HndGet(hndid,BUFFERSIZE, as8Buffer);
	return bRet;
}

/******************************************************************************
 * FUNCTION NAME : HndBin
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	After the end of test, all sites tested have to be sorted
	into the wanted bin. 
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : AETRIUM handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : skts
    Comment : Number of tested site
    Range   : 0 to MAX_AVAILABLE_SOCKETS
    Unit    : size_t
------------------------------------------------------------------------------
 * INPUT : bins
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
BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t skts, IN LPDWORD bins ) 
{

DWORD dwBin=0;
TBYTE u8Bin=0;
char *ps8Buffer = &as8Buffer[1];			// Point onto the nb of test result 

	/* Check if the hndid is valid and if the asked sockets are into the range */ 
	if ( (hndid<=0) )
		return FALSE;

	/* Init */
	memset(as8Buffer,'\0',BUFFERSIZE);

	/* Send bin for each DUT */
	size_t mask_skts=skts;
	int s32Loop=0;
	while(mask_skts)
	{
		/* Check if the socket was in test */
		if ( (mask_skts&(size_t)1)==(size_t)1)
		{
		
			/* Check if bin result is into the range */
			dwBin=bins[s32Loop];
			if ( !(dwBin>0) && (dwBin<=MAX_NB_BIN) )
			{
				GPIBCleanup(hndid, "Bin outside the range.");
				return FALSE;
			}
			/* Send the bining data for DUTn */
			u8Bin= SET_BIT(dwBin-1);
			sprintf(as8Buffer,CMD_WRITE_PORT,(TBYTE)(BIN_PORT_DUT1+s32Loop),u8Bin); 
			HndSet(hndid,as8Buffer);
			if ( (ibsta & ERR) || fCheckStdEvtStatus(hndid,MASK_CMD_ERROR_ESR) )
			{
				GPIBCleanup(hndid, "Error on bining.");
				return FALSE;
			}
		}
		s32Loop++;			// Next bin
		mask_skts>>=1;		// Next site 
	}
	/* Generation of the EOT	 */
	/* -----+			  +----	 */
	/*      |			  |		 */ 
	/*      +-------------+		 */
	/*       <-- 50 ms -->		 */

	/* Set the EOT lines  */
	sprintf(as8Buffer,CMD_WRITE_PORT,EOT_PORT,(TBYTE)(dwSiteOnTestMask));
	if (! HndSet(hndid,as8Buffer) )
	{
		PRINT_STDERR("Error on unseting EOT line to AETRIUM.\n");
		return FALSE;	
	}
	
	Sleep(5);
	HndSetGet(hndid,CMD_READ_QUEST_EVT_REG); // Read(Reset) questionable event register again to clear bounces effect
	/* Wait 50 ms */
	Sleep(50);

	/* Unset the EOT lines  */
	sprintf(as8Buffer,CMD_WRITE_PORT,EOT_PORT,(TBYTE)0);
	if (! HndSet(hndid,as8Buffer) )
	{
		PRINT_STDERR("Error on unseting EOT line to AETRIUM.\n");
		return FALSE;	
	}

	/* Unset all BINs */
	for (s32Loop=0; s32Loop<MAX_AVAILABLE_SOCKETS; s32Loop++)
	{
		sprintf(as8Buffer,CMD_WRITE_PORT,(TBYTE)(BIN_PORT_DUT1+s32Loop),0); 
		HndSet(hndid,as8Buffer);
		if ( (ibsta & ERR) || fCheckStdEvtStatus(hndid,MASK_CMD_ERROR_ESR) )
		{
			GPIBCleanup(hndid, "Error on bining.");
			return FALSE;
		}
	}
	/* End of test */
	Sleep(10);
	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndSet
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function sends a command to the AETRIUM.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : AETRIUM handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : command
    Comment : command to send on the GPIB line bus
    Range   : not NULL
    Unit    : LPCTSTR
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => command successfully sent
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndSet ( IN HNDID hndid, IN LPCTSTR command )
{
	/* Check if the hndid is valid and if the command is really present */ 
	if ( (hndid<=0) || (command==NULL) )
		return FALSE;

	ibwrt(hndid, (PVOID) command, strlen(command));
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Cannot Send Command");
		return FALSE;
	}
	fprintf(stderr,"HndSet: %04x '%s'\n", ibsta, command);
	fflush(stderr);
	Sleep(20);
	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndGet
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function read information from AETRIUM.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : AETRIUM handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : as8as8Buffer_size
    Comment : size of the as8as8Buffer used. 
    Range   : >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : answer
    Comment : as8as8Buffer pointer used to write data
    Range   : not NULL
    Unit    : LPCTSTR
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => Data successfully read
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndGet ( IN HNDID hndid, IN size_t s32BufferSize, OUT LPTSTR answer )
{
	/* Check if the hndid and as8as8Buffer_size are valid*/ 
	if ( (hndid<=0) || (s32BufferSize<=0) )
		return FALSE;

	ibrd(hndid, answer, s32BufferSize);
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
	answer[ibcnt] = '\0';

	fprintf(stderr,"HndGet: %04x: %s\n", ibsta, answer);
	fflush(stderr);

	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : fCheckStdEvtStatus
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function check if the data pointed by the u8Mask is present into 
	the std event register status of the 4863 device.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : AETRIUM handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : u8Mask
    Comment : Mask linked to std event register of 4863.
    Range   : 0x00 to 0xFF
    Unit    : TBYTE
 ------------------------------------------------------------------------------
 * BOOL : 
    Comment : return status of the function 
    Range   : TRUE => cmd successful
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL fCheckStdEvtStatus ( IN HNDID hndid, TBYTE u8Mask  )
{
TBYTE u8StdEvtStatus=0;

	/* Read the standard event status */
	HndSet(hndid, "*ESR?");
	Sleep(100);
	HndGet(hndid, BUFFERSIZE, as8Buffer);
	sscanf(as8Buffer,"%d",&u8StdEvtStatus);
	/* Mask reg with wanted information */
	if (u8StdEvtStatus & u8Mask)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/******************************************************************************
 * FUNCTION NAME : HndGet
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function discard the AETRIUM connection.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : AETRIUM handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => command successfully
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndRelease ( IN HNDID hndid )
{
	/* Check if the hndid is valid*/ 
	if ( hndid<=0 )
		return FALSE;

	/* Take the requested device offline */
	ibonl( hndid, 0);
	if (ibsta & ERR)
	{
		PRINT_STDERR("Cannot close the AETRIUM Communication.\n");
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

