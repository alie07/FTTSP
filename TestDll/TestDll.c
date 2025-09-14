/* TestDll.c : Defines the entry point for the console application. */

#include <winbase.h>

typedef int   (* /*__stdcall*/ PHNDINIT)    ( char *filename );
typedef int   (* /*__stdcall*/ PHNDPOLL)    ( void );
typedef int   (* /*__stdcall*/ PHNDBIN)     ( int *bins );
typedef char* (* /*__stdcall*/ PHNDQUERY)   ( char *query);
typedef char* (* /*__stdcall*/ PHNDSET)     ( char *command);

PHNDINIT  Hnd_Init  = NULL;
PHNDPOLL  Hnd_Poll  = NULL;
PHNDBIN   Hnd_Bin   = NULL;
PHNDQUERY Hnd_Query = NULL;
PHNDSET   Hnd_Set   = NULL;

int main(int argc, char* argv[])
{
	HINSTANCE myDll = LoadLibrary("../../ManualHandler/Debug/ManualHandler.dll");

	if (myDll) {
		Hnd_Init  = (PHNDINIT)  GetProcAddress(myDll, "Hnd_Init");
		Hnd_Poll  = (PHNDPOLL)  GetProcAddress(myDll, "Hnd_Poll");
		Hnd_Bin   = (PHNDBIN)   GetProcAddress(myDll, "Hnd_Bin");
		Hnd_Query = (PHNDQUERY) GetProcAddress(myDll, "Hnd_Query");
		Hnd_Set   = (PHNDSET)   GetProcAddress(myDll, "Hnd_Set");

		Hnd_Init("");

		FreeLibrary(myDll);
	}

	return 0;
}
