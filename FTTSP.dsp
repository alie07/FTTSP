# Microsoft Developer Studio Project File - Name="FTTSP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FTTSP - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FTTSP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FTTSP.mak" CFG="FTTSP - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FTTSP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FTTSP - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "FTTSP - Win32 CmdLine Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/FTTSP", EVDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FTTSP - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "FTTSP - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_TEST" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Version.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "FTTSP - Win32 CmdLine Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FTTSP___Win32_CmdLine_Debug"
# PROP BASE Intermediate_Dir "FTTSP___Win32_CmdLine_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_TEST" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FTTSP - Win32 Release"
# Name "FTTSP - Win32 Debug"
# Name "FTTSP - Win32 CmdLine Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\FTTSP.cpp
# End Source File
# Begin Source File

SOURCE=.\FTTSP.rc
# End Source File
# Begin Source File

SOURCE=.\FTTSPCmdLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\FTTSPDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FTTSPMainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\GTUI.cpp
# End Source File
# Begin Source File

SOURCE=.\Handler.cpp
# End Source File
# Begin Source File

SOURCE=.\HDUI.cpp
# End Source File
# Begin Source File

SOURCE=.\J800TX.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadProgram.cpp
# End Source File
# Begin Source File

SOURCE=.\MapCod.cpp
# End Source File
# Begin Source File

SOURCE=.\Phoenix.cpp
# End Source File
# Begin Source File

SOURCE=.\PTUI.cpp
# End Source File
# Begin Source File

SOURCE=.\RTM.cpp
# End Source File
# Begin Source File

SOURCE=.\Session.cpp
# End Source File
# Begin Source File

SOURCE=.\SessionConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=.\SessionMapCods.cpp
# End Source File
# Begin Source File

SOURCE=.\SessionSummary.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupGateway.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketTest.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TX.cpp
# End Source File
# Begin Source File

SOURCE=.\UI.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\VTUI.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=".\decl-32.h"
# End Source File
# Begin Source File

SOURCE=.\DialogPassword.h
# End Source File
# Begin Source File

SOURCE=.\FTTSP.h
# End Source File
# Begin Source File

SOURCE=.\FTTSPCmdLineInfo.h
# End Source File
# Begin Source File

SOURCE=.\FTTSPDlg.h
# End Source File
# Begin Source File

SOURCE=.\FTTSPMainWindow.h
# End Source File
# Begin Source File

SOURCE=.\FTTSPVersion.h
# End Source File
# Begin Source File

SOURCE=.\GTUI.h
# End Source File
# Begin Source File

SOURCE=.\Handler.h
# End Source File
# Begin Source File

SOURCE=.\HandlerDll.h
# End Source File
# Begin Source File

SOURCE=.\HDUI.h
# End Source File
# Begin Source File

SOURCE=.\J800TX.h
# End Source File
# Begin Source File

SOURCE=.\LoadProgram.h
# End Source File
# Begin Source File

SOURCE=.\MapCod.h
# End Source File
# Begin Source File

SOURCE=.\Phoenix.h
# End Source File
# Begin Source File

SOURCE=.\PTUI.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RTM.h
# End Source File
# Begin Source File

SOURCE=.\Session.h
# End Source File
# Begin Source File

SOURCE=.\SessionConfiguration.h
# End Source File
# Begin Source File

SOURCE=.\SessionMapCods.h
# End Source File
# Begin Source File

SOURCE=.\SessionSummary.h
# End Source File
# Begin Source File

SOURCE=.\SetupGateway.h
# End Source File
# Begin Source File

SOURCE=.\SocketTest.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TX.h
# End Source File
# Begin Source File

SOURCE=.\UI.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# Begin Source File

SOURCE=.\VTUI.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\atmel_lo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\FTTSP.ico
# End Source File
# Begin Source File

SOURCE=.\res\FTTSP.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon5.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\inside_l.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Letter-F.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Logo_Inside.bmp
# End Source File
# Begin Source File

SOURCE=".\res\memory-32.ico"
# End Source File
# Begin Source File

SOURCE=".\res\memory-64.ico"
# End Source File
# Begin Source File

SOURCE=".\res\memory-96.ico"
# End Source File
# Begin Source File

SOURCE=.\res\My_Comp2.ico
# End Source File
# Begin Source File

SOURCE=.\res\SSQ_l.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SSQ_s.bmp
# End Source File
# Begin Source File

SOURCE=".\res\speech-balloon-green-p.ico"
# End Source File
# Begin Source File

SOURCE=".\res\speech-balloon-orange-f.ico"
# End Source File
# Begin Source File

SOURCE=.\test.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\res\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\Modifications_List.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
