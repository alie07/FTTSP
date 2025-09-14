@echo off
rem Denis P. (2014/10/07)
rem Version 1.1
rem Clean intermediate files except exe and user files

setlocal EnableDelayedExpansion

set p[0]=debug
set p[1]=release

set proj[0]=Aetrium_TTL
set proj[1]=ElectroGlass
set proj[2]=ManualHandler
set proj[3]=MT8501_GPIB
set proj[4]=QT_GPIB
set proj[5]=Rasco_SO1000_TTL
set proj[6]=SeikoEpsonGPIB
set proj[7]=SynaxRFO4863GPIB
set proj[8]=Tesec_3270IH_GPIB
set proj[9]=UTech_RS232

set ext[0]=bsc
set ext[1]=sbr
set ext[2]=obj
set ext[3]=ilk
set ext[4]=pch
set ext[5]=pdb
set ext[6]=idb

rem FTTSP debug/release in root folder
for /L %%f in (0,1,1) do (
	set dir=!p[%%f]!
	echo !dir!
	for /L %%e in (0,1,6) do (
		set file=!dir!\*.!ext[%%e]!
		echo Delete files !file!
		if exist !file! del !file!
	)
)

rem DLL debug/release in dll folder
for /L %%j in (0,1,9) do (
	for /L %%f in (0,1,1) do (
		set dir=!proj[%%j]!\!p[%%f]!
		echo !dir!
		for /L %%e in (0,1,6) do (
			set file=!dir!\*.!ext[%%e]!
			echo Delete files !file!
			if exist !file! del !file!
		)
	)
)

pause
