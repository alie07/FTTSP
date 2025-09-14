/* HandlerDll.h: interface for the HandlerDll class.
 *
 */

#if !defined(AFX_HANDLERDLL_H__70FACDDB_8635_4BD4_B92C_8DB88683F5DB__INCLUDED_)
#define AFX_HANDLERDLL_H__70FACDDB_8635_4BD4_B92C_8DB88683F5DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

#ifdef HANDLERDLL_EX
	#define DLLDIR	__declspec(dllexport)	/* export DLL information */
#else
	#define DLLDIR	__declspec(dllimport)	/* import DLL information */
#endif /* HANDLERDLL_EX */

typedef DWORD		HNDID;
typedef HNDID near	*PHNDID;
typedef HNDID far	*LPHNDID; 

typedef HNDID (WINAPI * LPHNDINIT)    ( IN LPCTSTR options );
typedef BOOL  (WINAPI * LPHNDPOLL)    ( IN HNDID hndid, IN size_t max_skts, OUT LPDWORD active_sockets );
typedef BOOL  (WINAPI * LPHNDBIN)     ( IN HNDID hndid, IN size_t max_skts, IN LPDWORD bins );
typedef BOOL  (WINAPI * LPHNDSET)     ( IN HNDID hndid, IN LPCTSTR command );
typedef BOOL  (WINAPI * LPHNDGET)     ( IN HNDID hndid, IN size_t buffer_size, OUT LPTSTR answer );
typedef BOOL  (WINAPI * LPHNDRELEASE) ( IN HNDID hndid );
typedef BOOL  (WINAPI * LPHNDGETVERS) ( OUT PTBYTE u8Major, OUT PTBYTE u8Minor );


#ifdef HANDLERDLL_EX /* Force declaration of prototypes */
#ifdef __cplusplus
extern "C" {
#endif

	BOOL  DLLDIR WINAPI HndGetVersion ( OUT PTBYTE u8Major, OUT PTBYTE u8Minor);
	HNDID DLLDIR WINAPI HndInit		  ( IN LPCTSTR options );
	BOOL  DLLDIR WINAPI HndPoll	      ( IN HNDID hndid, IN size_t max_skts, OUT LPDWORD active_sockets );
	BOOL  DLLDIR WINAPI HndBin	      ( IN HNDID hndid, IN size_t skts, IN LPDWORD bins );
	BOOL  DLLDIR WINAPI HndSet	      ( IN HNDID hndid, IN LPCTSTR command );
	BOOL  DLLDIR WINAPI HndGet	      ( IN HNDID hndid, IN size_t buffer_size, OUT LPTSTR answer );
	BOOL  DLLDIR WINAPI HndRelease    ( IN HNDID hndid );

#ifdef __cplusplus
}
#endif
#endif /* HANDLERDLL_EX */

#endif /* !defined(AFX_HANDLERDLL_H__70FACDDB_8635_4BD4_B92C_8DB88683F5DB__INCLUDED_) */
