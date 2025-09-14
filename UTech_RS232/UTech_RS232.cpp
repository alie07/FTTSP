

// 
// !!20160330:LBo:UTech_RS232 v1.0 Creation $LABS-2526
// !!20160427:LBo:v1.1: Debug @UID + Disable heads number count for testing less than 16 DUT $LABS-2555
// !!20160915:LBo:v1.2: If Start of test received with no active site, answer directly to handler with all bin=0 $LABS-2605

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <string>
#include "Dll_Version.h"

/* Declare as exporting the dll */
#define HANDLERDLL_EX
#include "../HandlerDll.h"


#ifdef __cplusplus
extern "C" {
#endif

#define COM_PORT_NB             "COM3"                      // Set Serial COM port*

#define RET_ERROR				((HNDID)0)			    	// Error return code
#define RET_OK					((HNDID)1)
#define BUFFERSIZE			    100                       	// Size of buffer

#define SOT						"DS,"                       // Start of Test String beginning
#define EOT                     "XXXXXXET,"                 // End of Test String beginning
#define END_OF_FRAME		    "\n\r"                      // Last character of frame from/to handler


/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/
static char Buffer[BUFFERSIZE + 1];
static DWORD Bin_0[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static char TestHead;
static HANDLE hCom;
    
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
	*pu8Major=(unsigned char)DLL_MAJOR_VERSION;
	*pu8Minor=(unsigned char)DLL_MINOR_VERSION;
	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndInit
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	Attempt to create and initialize Com port, returns 0 in case of error
    returns 1 when no error (no handle here because we don't use GPIB)
 ------------------------------------------------------------------------------
 * INPUT : options
    Comment : Not Used
    Range   : 
    Unit    : LPCTSTR
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : simulated Handle of the device found 
    Range   : not NULL
    Unit    : HNDID
 *****************************************************************************/
HNDID DLLDIR WINAPI HndInit ( IN LPCTSTR options )
{
    char *pcCommPort = COM_PORT_NB;
    DCB dcb;
    COMMTIMEOUTS CommTimeout;
    //HANDLE hCom; //Serial Port Handle

	//Create handle, open COM
    hCom  = CreateFile(pcCommPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	
    if (hCom  == INVALID_HANDLE_VALUE) 
    {
        fprintf(stderr, "Error: Cannot connect to %s\n", pcCommPort);
		fflush(stderr);
		return RET_ERROR; //Error
    }
    else
    {
        if (!GetCommState(hCom, &dcb))
        {
            fprintf(stderr, "Error: Cannot get Serial Port configuration\n");
            fflush(stderr);
            //  Handle the error.
            return RET_ERROR;
        }

        //DCB Settings: 9600 bps, 8 data bytes, no parity and 1 stop bit.
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.Parity   = NOPARITY;
        dcb.StopBits = ONESTOPBIT;

        if(!SetCommState (hCom, &dcb))
        {
            fprintf(stderr, "Error: Cannot connect configure Serial port\n");
            fflush(stderr);
            return RET_ERROR;
        }

        // Retrieve the time-out parameters for all read and write operations on the port. 
        GetCommTimeouts (hCom, &CommTimeout);
        
        //Time max between two characters: 10ms to return as soon as received frame stops
        CommTimeout.ReadIntervalTimeout = MAXDWORD;
        //The multiplier used to calculate the total time-out period for read operations, in milliseconds.
        //For each read operation, this value is multiplied by the requested number of bytes to be read.
//        CommTimeout.ReadTotalTimeoutMultiplier = 1;
		CommTimeout.ReadTotalTimeoutMultiplier = MAXDWORD ;
        //Read total timeout period for read operations
        CommTimeout.ReadTotalTimeoutConstant =  10000;    
        //No TimeOut needed for Write
        CommTimeout.WriteTotalTimeoutMultiplier = 0;  
        CommTimeout.WriteTotalTimeoutConstant = 0;    

        // Set the time-out parameters for all read and write operations on the port. 
        if (!SetCommTimeouts (hCom, &CommTimeout))
		{
            // Could not set the time-out parameters.
            fprintf(stderr, "Error: Cannot connect configure Serial port Time-outs\n");
            fflush(stderr);
            return RET_ERROR;
		}
    }

    return RET_OK;
}

/******************************************************************************
 * FUNCTION NAME : HndPoll
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function waits a Test Start Request from the µTech handler.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : handle 
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
    Range   : TRUE => either timeout or normal exit
			  FALSE => Error ==> active_sockets[0]=0
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t mask_skts, OUT LPDWORD active_sockets )
{
//    HANDLE hCom = (HANDLE)hndid;
	int SOTPos, TestHeadPos;
	int i, HeadCnt;
    DWORD ActiveSites;
	size_t skts = mask_skts;

    DWORD BytesRead = 0, BytesReadTemp = 0;
	std::string Handler_string("");

	*active_sockets = 0;

    //Wait & Read frame from handler while we don't receive the end of frame
	do
	{
		if (! ReadFile(hCom,Buffer,BUFFERSIZE,&BytesReadTemp,NULL))
		{
			fprintf(stderr, "Error at Reading serial port.\n");
			fflush(stderr);

			return FALSE;
		}
		else
		{
			if (BytesReadTemp != 0)
			{
				//fprintf(stderr, "%d\n", BytesReadTemp);
				//fflush(stderr);
				BytesRead += BytesReadTemp;
				Handler_string += std::string(Buffer, BytesReadTemp);
			}
		}
	}while(BytesReadTemp != 0 && Handler_string.find(END_OF_FRAME) == -1 && BytesRead < BUFFERSIZE);
	//If TimeOut or last character is not end of frame: exit with no error
	if((BytesRead == 0) || (Handler_string.find(END_OF_FRAME) == -1))
	{
		return TRUE;
	}

    // XXXXXXDS,A,head1,head2,head3...
    // XXXXXXDS,A,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    // XXXXXXDS,B,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    // XXXXXXDS,C,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
//    fprintf(stderr, "\nHandler string = %s\n", Handler_string.c_str());
//	fflush(stderr);
	
	SOTPos = Handler_string.find(SOT);
	if ( SOTPos == -1)
	{
		fprintf(stderr, "DS not found in received buffer.\n");
		fflush(stderr);
		return FALSE;
	}

	//Get TestHead:
	TestHeadPos = SOTPos + sizeof(SOT) - 1;
	TestHead = Handler_string[TestHeadPos];

	ActiveSites = 0;
	HeadCnt = 0;
	for (i=TestHeadPos + 2 ; i<BytesRead; i++)
	{
		if ((Handler_string[i] == '0') || (Handler_string[i] == '1'))
		{
			//head 1 is LSB of ActiveSites Mask.
			if (Handler_string[i] == '1')
                ActiveSites |= 1<<HeadCnt;
			HeadCnt++;
		}
	}


/*	// !!20160427:LBo:v1.1: Disable heads number count for testing less than 16 DUT $LABS-????
    //TestHead 'A' = LSB, TestHead 'B' = MSB, TestHead 'C' = both 'A' + 'B'
	switch(TestHead)
	{
		case 'A':
			if (HeadCnt != 16)
            {
                fprintf(stderr, "Error Test head config A wih sites count= %d", HeadCnt);
                fflush(stderr);
                return FALSE;
            }
			break;
		case 'B':
			if (HeadCnt != 16)
            {
                fprintf(stderr, "Error Test head config B wih sites count= %d", HeadCnt);
                fflush(stderr);
                return FALSE;
            }
			else ActiveSites <<= 16; //Active sites received are MSB.
			break;
		case 'C':
			if (HeadCnt != 32)
            {
                fprintf(stderr, "Error Test Head config C with sites count= %d", HeadCnt);
                fflush(stderr);
                return FALSE;
            }
            break;
		default :
			fprintf(stderr, "Test Head \"%c\" invalid\n", TestHead);
            fflush(stderr);
            return FALSE;
	}
*/

    // Mask with 'mask_skts'
    ActiveSites &= mask_skts;

	//!!20160427:LBo: If Active sites are == 0 here (start of test request with no active site)...
	//				  ... -> answer directly to handler with all bin = 0  $LABS-2605
	if (ActiveSites == 0)
	{
		return HndBin(hndid, mask_skts, Bin_0);
	}

    *active_sockets = ActiveSites;

	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndBin
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	After the end of test, send result of all sites tested to the handler

 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : Com port handle 
    Range   : not NULL & >0
    Unit    : HNDID
 ------------------------------------------------------------------------------
 * INPUT : mask_skts
    Comment : Mask of tested site
    Range   : 
    Unit    : size_t
------------------------------------------------------------------------------
 * INPUT : bins
    Comment : Pointer to bin information per site
    Range   : not NULL
    Unit    : LPDWORD
 ------------------------------------------------------------------------------
 * OUTPUT : 
    Comment : return status of the function 
    Range   : TRUE => send serial data successfully done
			  FALSE => Error 
    Unit    : BOOL
 *****************************************************************************/
BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t mask_skts, IN LPDWORD bins ) 
{
//    HANDLE hCom = (HANDLE)hndid;
    std::string EOT_string = EOT;

	size_t skts = mask_skts;
    int i, SitesNb, BytesNbToWrite;
	DWORD WrittenBytesNb;
    
    if (hndid<=0) 
		return FALSE;
    //TestHead 'A' = LSB, TestHead 'B' = MSB, TestHead 'C' = both 'A' + 'B'
    if (TestHead == 'C')
        SitesNb = 32;
    else
        SitesNb = 16;
        
    //Concatenate string to send with Test Head parameter
    EOT_string += TestHead;
    EOT_string += ",";
    
	for(i=0; i<SitesNb; i++)
	{
		// Check if the socket was in test
		if ( (skts&1)==1 )
		{
            // If bin = 1 -> good die -> Handler bin = 0
            if(bins[i] == 1)
                EOT_string += "0,";
            else
                EOT_string += "1,";
		}
		else
		{
			EOT_string += "1,";
		}
		skts>>=1;		// Next site 
	}
	EOT_string+=END_OF_FRAME;
	BytesNbToWrite = EOT_string.length();
    EOT_string.copy(Buffer, BytesNbToWrite);
//		fprintf(stderr, "Bytes nb = %d\n", BytesNbToWrite);
//		fflush(stderr);
    
	//Send to serial port
	if (!WriteFile(hCom, Buffer, BytesNbToWrite, &WrittenBytesNb, NULL))
	{
		fprintf(stderr, "Error at writing to serial port\n");
		fflush(stderr);
		return FALSE;
	}

    return TRUE;
}

BOOL DLLDIR WINAPI HndSet ( IN HNDID hndid, IN LPCTSTR command )
{
	return TRUE;
}

BOOL DLLDIR WINAPI HndGet ( IN HNDID hndid, IN size_t buffer_size, OUT LPTSTR answer )
{
	return TRUE;
}

/******************************************************************************
 * FUNCTION NAME : HndRelease
 ------------------------------------------------------------------------------
 * FUNCTION DESCRIPTION :
	This function closes serial port connection.
 ------------------------------------------------------------------------------
 * INPUT : hndid
    Comment : handle 
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
//    HANDLE hCom = (HANDLE)hndid;
	/* Check if the hndid is valid*/ 
	if ( hndid<=0 )
		return FALSE;
	PurgeComm(hCom,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	CloseHandle(hCom);
	return TRUE;
}


#ifdef __cplusplus
}
#endif

