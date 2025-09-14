
/* Verify "%NIEXTCCOMPILERSUPP%\include" is declared in the include files
 * Verify "%NIEXTCCOMPILERSUPP%\LIB32\MSVC" is declared in the lib files
 */

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <decl-32.h>

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

#define DLL_MAJOR_VERSION			((unsigned char)1)						// Major version identification
#define DLL_MINOR_VERSION			((unsigned char)1)						// Minor version identification

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
    ibonl(ud, 0);
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

	/*
	 * The application issues the '*IDN?' command to the handler
	 */
	ibwrt(ud, "*IDN?", 6L);
	if (ibsta & ERR)
	{
		GPIBCleanup(ud, "Unable to send Get ID to Handler");
		return 0;
	}

	/*
	 * The application reads the identification code in the form of an
	 * ASCII string from the handler into the Buffer variable.
	 */
	ibrd(ud, Buffer, BUFFERSIZE);
	if (ibsta & ERR)
	{
		GPIBCleanup(ud, "Unable to read data from handler");
		return 0;
	}

	/*
	 * Assume that the returned string contains ASCII data. NULL
	 * terminate the string using the value in ibcntl which is the
	 * number of bytes read in.
	 */
	Buffer[ibcnt] = '\0';

	/*
	 * Remains to verify ID of the prober
	 */

	return ud;
}

BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t max_skts, OUT LPDWORD active_sockets )
{
	char SerialPollResponse = '\0';
	if (max_skts > 32)
		return FALSE;

	ibwait(hndid, RQS | TIMO);
	if (ibsta & RQS)
	{
		fprintf(stderr, "Handler asserted RQS.\n");
		ibrsp( hndid, &SerialPollResponse );
		if (! (SerialPollResponse & 0x40))
		{
			fprintf(stderr, "Handler asserted RQS but its serial response is not valid.\n");
			return FALSE;
		}
	}
	else
	{
		// Is it just a timeout issue?
		if (ibsta & TIMO)
		{
			active_sockets[0] = 0;
			return TRUE;
		}
		// Is it an error
		return FALSE;
	}

	fprintf(stderr, "Handler asserted RQS.\n");
	ibrd(hndid, Buffer, BUFFERSIZE);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Unable to retrieve Request from Handler");
		return FALSE;
	}
	Buffer[ibcnt] = '\0';

	*active_sockets = 0;

	if (Buffer[0] == 'T' && (
			Buffer[1] == 'F' ||
			Buffer[1] == 'S' ||
			Buffer[1] == 'A'))
	{
		char* pChar = Buffer+2;
		while(*pChar != '\0')
		{
			if (*pChar == ',')
			{
				sscanf(++pChar, "%u", &gMask);
				break;
			}
			pChar++;
		}
		active_sockets[0] = gMask;
	}
	else
	{
		active_sockets[0] = 0;
	}

	return TRUE;
}

BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t skts, IN LPDWORD bins ) 
{
	char localBuffer[10];
	DWORD uSkt;

	if (skts > 32)
		return FALSE;


	strcpy(Buffer, "ET");
	for(int i = 0; gMask && (i < 32); i++)
	{
		uSkt = (DWORD) 1 << i;
		if (gMask & uSkt)
			gMask &= ~uSkt; // Clear bit
		if (i == 0)
			sprintf(localBuffer, "%d", bins[i]);
		else
			sprintf(localBuffer, " %d", bins[i]);
		strcat(Buffer, localBuffer);
	}
	ibwrt(hndid, Buffer, strlen(Buffer));
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Error sending binnings");
		return 0;
	}
	return 1;
}

BOOL DLLDIR WINAPI HndSet ( IN HNDID hndid, IN LPCTSTR command )
{
	return FALSE;
}

BOOL DLLDIR WINAPI HndGet ( IN HNDID hndid, IN size_t buffer_size, OUT LPTSTR answer )
{
	return FALSE;
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
