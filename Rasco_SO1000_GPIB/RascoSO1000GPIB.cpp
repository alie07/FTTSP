
/* Verify "%NIEXTCCOMPILERSUPP%\include" is declared in the include files
 * Verify "%NIEXTCCOMPILERSUPP%\LIB32\MSVC" is declared in the lib files
 */

// Rasco_SO1000_GPIB 1.0 : 13/11/2023 (LBs)
//				- Creation




#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <decl-32.h>

#include "dll_version.h"

/* Declare as exporting the dll */
#define HANDLERDLL_EX
#include "../HandlerDll.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
 
#ifdef __cplusplus
extern "C" {
#endif

#define BDINDEX					0     // Board Index
#define PAD_OF_HANDLER			1     // Primary address of device
#define SAD_OF_HANDLER			NO_SAD// Secondary address of device
#define TIMEOUT					T10s  // Timeout value = 10 seconds
#define EOTMODE					1     // Enable the END message
#define EOSMODE					0     // Disable the EOS mode

#define MAX_RETRIES				1	  // Number of times to try resend bins if ECHONG received

#define BUFFERSIZE			2048	// Size of buffer
char Buffer[BUFFERSIZE + 1];

static DWORD gMask = 0;

char ErrorMnemonic[29][5] = { "EDVR", "ECIC", "ENOL", "EADR", "EARG",
                              "ESAC", "EABO", "ENEB", "EDMA", "",
                              "EOIP", "ECAP", "EFSO", "",     "EBUS",
                              "ESTB", "ESRQ", "",     "",      "",
                              "ETAB", "ELCK", "EARM", "EHDL",  "",
                              "",     "EWIP", "ERST", "EPWR" };

/*
 * After each GPIB call, the application checks whether the call
 * succeeded. If an NI-488.2 call fails, the GPIB driver sets the
 * corresponding bit in the global status variable. If the call
 * failed, this procedure prints an error message, takes the board
 * offline and exits.
 */
void GPIBCleanup(int ud, const char * ErrorMsg)
{
    fprintf(stderr, "Error : %s\nibsta = 0x%x iberr = %d (%s)\n",
           ErrorMsg, ibsta, iberr, ErrorMnemonic[iberr]);
    fprintf(stderr, "Cleanup: Taking board offline\n");
	fflush(stderr);
	//Clear error instead of closing connection to avoid full restart of FTTSP in case of communication error
    //ibonl(ud, 0);
	ibclr(ud);
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

HNDID DLLDIR WINAPI HndInit ( IN LPCTSTR options )
{
	HNDID ud;
    /* Return -1 if device not found else return handle
	 * ibdev(board, pad, sad, tmo, eot, eos);
	 * eot zero    : EOI line will not be asserted at the end of write operation.
	 *     non-zero: EOI line will be asserted at the end of write
	 * eos low 8bit: EOS character
	 *     bit A   : 00000100 Terminate read when EOS is detected
	 *     bit B   : 00001000 Set EOI with EOS on write function
	 *     bit C   : 00010000 Compare all 8 bits of EOS byte rather than low 7 bits(all read and write calls).
	 */
	ud = ibdev (BDINDEX, PAD_OF_HANDLER, SAD_OF_HANDLER, TIMEOUT, EOTMODE, EOSMODE);
	if (ibsta & ERR)
	{
		fprintf(stderr, "Unable to open device\nibsta = 0x%x iberr = %d\n", ibsta, iberr);
		fflush(stderr);
		return 0;
	}

	/*
	 * Clear the internal or device functions of the device. If the
	 * error bit ERR is set in ibsta, call GPIBCleanup with an error message.
	 */
	ibclr(ud);
	if (ibsta & ERR)
	{
		GPIBCleanup(ud, "Unable to clear device");
		return 0;
	}
	return ud;
}

BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t mask_skts, OUT LPDWORD active_sockets )
{
	char spr;

	active_sockets[0]=0;

	ibwait(hndid, RQS | TIMO);
	if (! (ibsta & RQS))
	{
		// Is it just a timeout issue?
		if (ibsta & TIMO)
		{
			active_sockets[0] = 0;
			return TRUE;
		}

		// Is it an error
		ibclr(hndid);
		return FALSE;
	}

	//!!Commented out fprintf
//	fprintf(stderr, "Handler asserted RQS.\n");
//	fflush(stderr);
	
	ibrsp(hndid, &spr);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Error During Serial Poll");
		return FALSE;
	}

	// 0x41 indicates that some sockets are ready, anything else, unknown Request so set to 0
	if (spr != 0x41)
	{
		active_sockets[0] = 0;
		return TRUE;
	}


	ibwrt(hndid, "FULLSITES?\r\n", 12);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Error sending FULLSITES?");
		return FALSE;
	}

	ibrd(hndid, Buffer, BUFFERSIZE);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Cannot read FULLSITES");
		return FALSE;
	}
	Buffer[ibcnt] = '\0'; // Terminate string

	if (sscanf(Buffer, "Fullsites %8x", &gMask) != 1)
	{
		fprintf(stderr, "Invalid response from Handler, received '%s' i/o 'Fullsites xxxxxxxx'.\n", Buffer);
		fflush(stderr);
		return FALSE;
	}
	
	gMask&=mask_skts;
	active_sockets[0] = gMask;

	return TRUE;
}

BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t mask_skts, IN LPDWORD bins ) 
{
	char binBuffer[] = "xxxxxxxx,xxxxxxxx,xxxxxxxx,xxxxxxxx";
	char *pbinChar = binBuffer;
	int tries = 0;
	DWORD curbin;

	for (int i=31; i>=0; i--)
	{
		if ( (mask_skts&(1<<i))==0 )
		{
			curbin = 0;
		}
		else
		{
			curbin = bins[i];
		/*	if ( curbin==0 )
			{
				fprintf(stderr, "Bin 0 received for an activated Dut.\n");
				fflush(stderr);
				return 0;
			}
		*/	if (curbin > 0xF)
			{
				fprintf(stderr, "Warning: Received Bin out of bounds, used 0xF instead\n");
				fflush(stderr);
				curbin = 0xF;
			}
		}
		while (*pbinChar != 'x')
			pbinChar++;
		sprintf(Buffer, "%d", curbin);
		*pbinChar++ = Buffer[0];

	}
	while (tries++ < MAX_RETRIES)
	{
		strcpy(Buffer, "BINON:");
		strcat(Buffer, binBuffer);
		strcat(Buffer, ";\r\n"); //!!08122023:LBs:Added termination character ";"
		ibwrt(hndid, Buffer, strlen(Buffer));
		if (ibsta & ERR)
		{
			GPIBCleanup(hndid, "Error sending binnings");
			return FALSE;
		}

		ibrd(hndid, Buffer, BUFFERSIZE);
		if (ibsta & ERR)
		{
			GPIBCleanup(hndid, "Cannot Read Echo");
			return FALSE;
		}
		if (
			(strncmp(Buffer, "ECHO:", 5) == 0) &&
			(strncmp(Buffer + 5, binBuffer, strlen(binBuffer)) == 0)
			)
		{
			ibwrt(hndid, "ECHOOK\r\n", 8);
			if (ibsta & ERR)
			{
				GPIBCleanup(hndid, "Error sending ECHOOK");
				return FALSE;
			}
			return TRUE;
		}
		ibwrt(hndid, "ECHONG\r\n", 8);
		if (ibsta & ERR)
		{
			GPIBCleanup(hndid, "Error sending ECHONG");
			return FALSE;
		}
	}

/*
	// Clear the device
	strcpy(Buffer, "SDC\r\n");
	ibwrt(hndid, Buffer, strlen(Buffer));
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Cannot Send SDC.\n");
		return FALSE;
	}
*/
	return FALSE;
}

BOOL DLLDIR WINAPI HndSet ( IN HNDID hndid, IN LPCTSTR command )
{
	ibwrt(hndid, (PVOID) command, strlen(command));
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Cannot Send Command");
		return FALSE;
	}
	fprintf(stderr,"HndSet: %04x '%s'\n", ibsta, command);
	fflush(stderr);

	return TRUE;
}

BOOL DLLDIR WINAPI HndGet ( IN HNDID hndid, IN size_t buffer_size, OUT LPTSTR answer )
{
	ibrd(hndid, answer, buffer_size);
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

	fprintf(stderr,"HndGet: %04x '%s'\n", ibsta, answer);
	fflush(stderr);

	return TRUE;
}

BOOL DLLDIR WINAPI HndRelease ( IN HNDID hndid )
{
	// Take the requested device offline
	ibonl( hndid, 0);
	return TRUE;
}


#ifdef __cplusplus
}
#endif

