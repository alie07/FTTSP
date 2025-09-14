// TestDll.cpp : Defines the entry point for the console application.

#include "StdAfx.h"
#include <windows.h>

#include "../HandlerDll.h"

#include "stdio.h"
#include <conio.h>

LPHNDINIT  Hnd_Init  = NULL;
LPHNDPOLL  Hnd_Poll  = NULL;
LPHNDBIN   Hnd_Bin   = NULL;
LPHNDGET   Hnd_Get   = NULL;
LPHNDSET   Hnd_Set   = NULL;
LPHNDRELEASE Hnd_Release = NULL;

int main(int argc, char* argv[])
{
	//HINSTANCE myDll = LoadLibrary("..\\Tesec_3270IH_GPIB\\Debug.dll");
    //!!20160405:LBo:Debug of UTech_RS232.dll $LABS-2526
	//HINSTANCE myDll = LoadLibrary("..\\UTech_RS232\\Debug\\UTech_RS232.dll");
	HINSTANCE myDll = LoadLibrary("..\\ManualHandler\\Debug\\ManualHandler.dll");
	BOOL result;
	int RequestedSocketsMask=0x0FF;
	int nBin1Result=0;
	int nBin2Result=1;
	DWORD bins[16]={1,1,2,1,3,1,4,1};
	if (myDll) {
		Hnd_Init  = (LPHNDINIT)  GetProcAddress(myDll, "HndInit");
		Hnd_Poll  = (LPHNDPOLL)  GetProcAddress(myDll, "HndPoll");
		Hnd_Bin   = (LPHNDBIN)   GetProcAddress(myDll, "HndBin");
		Hnd_Get   = (LPHNDGET) GetProcAddress(myDll, "HndGet");
		Hnd_Set   = (LPHNDSET)   GetProcAddress(myDll, "HndSet");
		Hnd_Release   = (LPHNDRELEASE)   GetProcAddress(myDll, "HndRelease");
		
		AllocConsole(); // Creates stdin, stdout & stderr for the library

		HNDID hndid = Hnd_Init("");
		DWORD mask;
			
		while (hndid)
		{

			result = Hnd_Poll(hndid, RequestedSocketsMask, &mask);
			printf("result = %d\n", result);
			printf ("HNDID:%d\n", hndid);
			if (mask != 0)
			{
				printf("Result = %d\n", result);
				printf("Nb Sockets: %08x\n",RequestedSocketsMask);
				printf("HndPoll mask: %08x\n", mask);
				
/*				nBin1Result+=1;
				nBin2Result+=1;
				if (nBin1Result>6)
					nBin1Result=1;
				if (nBin2Result>6)
					nBin2Result=1;

				bins[0]=nBin1Result;
				bins[1]=nBin2Result;		
*/				printf("HndBin  %d\n", Hnd_Bin(hndid, RequestedSocketsMask, bins));
			}
		}

getch();
		Hnd_Release(hndid);

		FreeConsole();
		
		FreeLibrary(myDll);
	}

	return 0;
}
