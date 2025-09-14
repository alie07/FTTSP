
#if !defined(__HANDLER_PROTOTYPES_INCLUDED_)
#define __HANDLER_PROTOTYPES_INCLUDED_

#if defined (_HANDLER_DLL)
#define DLL_DIR	__declspec(dllexport)
#else
#define DLL_DIR	__declspec(dllimport)
#endif

DLL_DIR int   APIENTRY Hnd_Init  (char* filename);
DLL_DIR int   APIENTRY Hnd_Poll  (void );
DLL_DIR int   APIENTRY Hnd_Bin   (int* bins);
DLL_DIR char* APIENTRY Hnd_Query (char* query);
DLL_DIR int   APIENTRY Hnd_Set   (char* request);

#endif