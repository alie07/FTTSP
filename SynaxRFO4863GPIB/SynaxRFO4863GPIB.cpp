
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

#define BDINDEX					1     // Board Index
#define PAD_OF_HANDLER			4     // Primary address of device
#define SAD_OF_HANDLER			NO_SAD// Secondary address of device
#define TIMEOUT					T10s  // Timeout value = 10 seconds
#define EOTMODE					1     // Enable the END message
#define EOSMODE					0     // Disable the EOS mode

#define DLL_MAJOR_VERSION		((unsigned char)1)						// Major version identification
#define DLL_MINOR_VERSION		((unsigned char)1)						// Minor version identification

#define BUFFERSIZE			1024	// Size of buffer
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
	char* cmd;
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
	 * The application issues the '*IDN?' command to the GPIB 4863 box
	 */
	if (! HndSet(ud, "*IDN?"))
	{
		fprintf(stderr, "Unable to send Get ID to Handler.\n");
		fflush(stderr);
		return 0;
	}

	/*
	 * The application reads the identification code in the form of an
	 * ASCII string from the handler into the Buffer variable.
	 */
	if (! HndGet(ud, BUFFERSIZE, Buffer))
	{
		fprintf(stderr, "Unable to read data from handler.\n");
		fflush(stderr);
		return 0;
	}

	/*
	 * Remains to verify ID of the prober
	 * Box should return a string comma separated like:
	 * ICS Electronics,4863, S/N 510123, Rev 00.00 Ver 98.05.18
	 */
	char * token = strtok(Buffer, ",");
	if (token == NULL)
		return FALSE;

	token = strtok(NULL, ",");
	if (strstr(token, "4863") == NULL)
	{
		fprintf(stderr, "Device found is not an 4863 box.\n");
		fflush(stderr);
		return 0;
	}

	/*
	 * Configuration of the 4863 box
	 */
	if (
		HndSet(ud, cmd = "CONF:INPUT (@1)") &&
		HndSet(ud, cmd = "CONF:INPUT:POLARITY 0") &&
		HndSet(ud, cmd = "CONF:INPUT:HANDSHAKE OFF") &&
		HndSet(ud, cmd = "CONF:OUTPUT (@2:4)") &&
		HndSet(ud, cmd = "CONF:OUTPUT:POLARITY 0") &&
		HndSet(ud, cmd = "CONF:OUTPUT:HANDSHAKE OFF") &&
		HndSet(ud, cmd = "FORMAT:TALK HEX") &&
		HndSet(ud, cmd = "FORMAT:LISTEN HEX") &&
		HndSet(ud, cmd = "STATUS:QUES:NTR 3") &&
		HndSet(ud, cmd = "STATUS:QUES:ENAB 3") &&
		HndSet(ud, cmd = "*SRE 8") &&
		HndSet(ud, cmd = "*CLS"))
	{
		return ud;
	}

	fprintf(stderr, "Error while sending command: '%s'\n", cmd);
	fflush(stderr);
	return 0;
}

static void update_gMask( IN HNDID hndid )
{
	if (HndSet(hndid, "STAT:QUES?") &&
		HndGet(hndid, BUFFERSIZE, Buffer))
	{
		sscanf(Buffer, "%d", &gMask);
		gMask &= 0x3;
	} else {
		gMask = 0;
	}
}

BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t max_skts, OUT LPDWORD active_sockets )
{
	char spr;

	if (max_skts > 2)
		return FALSE;

	ibwait(hndid, RQS | TIMO);
	if (! (ibsta & RQS))
	{
		// Is it just a timeout issue?
		if (ibsta & TIMO)
		{
			update_gMask(hndid);
			active_sockets[0] = gMask;
			return TRUE;
		}
		else
		{
			// Is it an error
			return FALSE;
		}
	}
	else
	{
		fprintf(stderr, "Handler asserted RQS.\n");
		fflush(stderr);
	}

	// Clears spr byte
	ibrsp(hndid, &spr);
	if (ibsta & ERR)
	{
		GPIBCleanup(hndid, "Error During Serial Poll");
		return FALSE;
	}

	if (spr & 0x80)
	{// Operation Register summary set
		if (HndSet(hndid, "*OSR?") &&
			HndGet(hndid, BUFFERSIZE, Buffer))
		{
			fprintf(stderr, "Operation Register request received: %s", Buffer);
			fflush(stderr);
		}
	}

	if (spr & 0x20)
	{// Standard event status register summary set
		if (HndSet(hndid, "*ESR?") &&
			HndGet(hndid, BUFFERSIZE, Buffer))
		{
			fprintf(stderr, "Standard Event request received: %s", Buffer);
			fflush(stderr);
		}
	}

	if (spr & 0x10)
	{// Output Queue
		if (HndGet(hndid, BUFFERSIZE, Buffer))
		{
			fprintf(stderr, "Output Queue not Empty: %s", Buffer);
			fflush(stderr);
		}
	}

	if (spr & 0x08)
	{
		update_gMask(hndid);
		active_sockets[0] = gMask;
		return TRUE;
	}

	return FALSE;
}

BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t skts, IN LPDWORD bins ) 
{
	BOOL result = TRUE;
	BOOL skten[2];
	DWORD sktbinmask[2];

	skten[0] = gMask & 0x1;
	skten[1] = gMask & 0x2;

	if (skts > 2)
		return FALSE;

	// Is global mask empty
	if (! (gMask & 0x3))
		return TRUE;

	if (
		(skten[0] && (bins[0] < 1 || bins[0] > 8)) || 
		(skten[1] && (bins[1] < 1 || bins[1] > 8))
		)
	{
		fprintf(stderr, "Invalid bin received.\n");
		fflush(stderr);
		return FALSE;
	}

	sktbinmask[1] = skten[1]?(1 << (bins[1]-1)):0;
	sktbinmask[0] = skten[0]?(1 << (bins[0]-1)):0;

	sprintf(Buffer, "SOURCE:DATA %02x%02x%02x", 0, sktbinmask[0], sktbinmask[1]);
	result &= HndSet(hndid, Buffer);
	Sleep(10);

	sprintf(Buffer, "SOURCE:DATA %02x%02x%02x", (gMask & 0x3), sktbinmask[0], sktbinmask[1]);
	result &= HndSet(hndid, Buffer);
	Sleep(10);

	sprintf(Buffer, "SOURCE:DATA %02x%02x%02x", 0, sktbinmask[0], sktbinmask[1]);
	result &= HndSet(hndid, Buffer);
	Sleep(10);

	sprintf(Buffer, "SOURCE:DATA %02x%02x%02x", 0, 0, 0);
	result &= HndSet(hndid, Buffer);
	Sleep(10);

	/* Clears invalid Test starts */
	result &= HndSet(hndid, "*CLS");
	result &= HndSet(hndid, "STAT:QUES?");
	result &= HndGet(hndid, BUFFERSIZE, Buffer);


	return result;
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
