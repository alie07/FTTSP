
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

#define DLL_MAJOR_VERSION			((unsigned char)1)						// Major version identification
#define DLL_MINOR_VERSION			((unsigned char)1)						// Minor version identification


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

static DWORD gMask = 0;

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
	int AnsLength=100,Hnd;
	char c_Cmd[30], reply[100];
	
	Hnd	= ibfind("dev4"); //Primary Address of GPIB box
	if(Hnd == -1)
		return NULL; //Error
	
	ibclr (Hnd); 

	//Check communication with GPIB box
	sprintf(c_Cmd,"*IDN?");
	ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
	Sleep(10);
	strset(reply,'\0');
	ibrd(Hnd,reply,AnsLength);
	if(strlen(reply) == 0)
		return NULL;

	sprintf(c_Cmd,"*CLS");	//clear value of the questionable event register
	ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
	Sleep(10);

	if(Hnd != -1)	
	{
		//GPIB BOX
		//INPUT CONFIGURATION
		sprintf(c_Cmd,"CONF:INP (@1)");		//Byte 1 as in INPUT
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);
		
		sprintf(c_Cmd,"CONF:INP:POL 0");	//Logical true level = 0 for input
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);

		sprintf(c_Cmd,"CONF:INP:HAND OFF");	//HANDSHAKE OFF
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);
		
		sprintf(c_Cmd,"STAT:QUES:NTR 1");	//NEGATIVE TRANSITION ON CHANNEL 1
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);

		//OUTPUT CONFIGURATION
		sprintf(c_Cmd,"CONF:OUT (@2:3)");	//Byte 2 & 3 as in output
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);

		sprintf(c_Cmd,"CONF:OUT:HAND OFF");	//HANDSHAKE OFF
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);
		
		sprintf(c_Cmd,"SOURCE:DATA:PORT2 255");
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);

		sprintf(c_Cmd,"SOURCE:DATA:PORT3 255");
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);

		sprintf(c_Cmd,"*CLS");	//clear value of the questionable event register
		ibwrt(Hnd,c_Cmd,strlen(c_Cmd));
		Sleep(10);
	}

	return (HNDID)Hnd; //cast mandatory here due to function prototype
}

BOOL DLLDIR WINAPI HndPoll ( IN HNDID hndid, IN size_t max_skts, OUT LPDWORD active_sockets )
{
	char c_Cmd[30], reply[100];
	int AnsLength=100;

	active_sockets[0] = 0;

	if(max_skts > 32)
		return FALSE;

	sprintf(c_Cmd,"*CLS");	//clear value of the questionable event register
	ibwrt(hndid,c_Cmd,strlen(c_Cmd));

	do
	{
		sprintf(c_Cmd,"STAT:QUES:EVENT?"); //query the questionable event register
		ibwrt(hndid,c_Cmd,strlen(c_Cmd));
		strset(reply,'\0');
		ibrd(hndid,reply,AnsLength);

	}while( !(atoi(reply) & 0x01) );	//Low level on channel 1 : START TEST DETECTED 

	sprintf(c_Cmd,"*CLS"); //Ensure the value of the questionable event register is cleared
	ibwrt(hndid,c_Cmd,strlen(c_Cmd));

	active_sockets[0] = 1; //only 1 site active

	return TRUE;
}

BOOL DLLDIR WINAPI HndBin ( IN HNDID hndid, IN size_t skts, IN LPDWORD bins ) 
{
	char c_Cmd[30];
	
//	Port3 : Bit[0]		=> EOT
//			Bit[1-7]	=> NOT USED
//	Port2 : Bit[0-7]	=> bit[0] = bin 1; bit[1] = bin 2...bit[7] = bin 8

	if(bins[0] == 0) //Unknown BIN
		return FALSE;

	// All signals are active low PORT2 = 0xFF (255)
	//1st => send bin on port2
	sprintf(c_Cmd,"SOURCE:DATA:PORT2 %d", 255 - (1<<(bins[0]-1)) );
	ibwrt(hndid,c_Cmd,strlen(c_Cmd));
	Sleep(10);
	
	//2nd => send EOT on port3 & bin remain on port2
	sprintf(c_Cmd,"SOURCE:DATA:PORT3 0");
	ibwrt(hndid,c_Cmd,strlen(c_Cmd));
	Sleep(10);
	
	//3rd => Remove EOT on port3 & bin remain port2
	sprintf(c_Cmd,"SOURCE:DATA:PORT3 1");
	ibwrt(hndid,c_Cmd,strlen(c_Cmd));
	Sleep(10);

	//4th => Remove bin remain on port2
	sprintf(c_Cmd,"SOURCE:DATA:PORT2 255");
	ibwrt(hndid,c_Cmd,strlen(c_Cmd));
	Sleep(10);
	
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

BOOL DLLDIR WINAPI HndRelease ( IN HNDID hndid )
{
	CloseHandle( (HANDLE)hndid ); //close handles returned by calls to the CreateFile function
	return TRUE;
}


#ifdef __cplusplus
}
#endif
