@echo off
::  Helper script to build BDB XML for binary release
::  Assumes current directory is <xml>/dist
::

:: Try several different locations for Visual Studio
call :TryBat "c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat" && goto batfound1

call :TryBat "c:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat" && goto batfound2

call :TryBat "c:\Program Files\Microsoft Visual Studio.NET\Common7\Tools\vsvars32.bat" && goto batfound3

:: no luck
goto batnotfound

:batfound1
echo Using Visual Studio .NET 2003
goto batfound

:batfound2
echo Using Visual Studio .NET
goto batfound

:batfound3
echo Using Visual Studio.NET
goto batfound

:batfound

:: The guts of this script
:: Build all projects in the list, winmsi/project.list.in
:: That list defines much of what's part of the binary distribution
@echo "Windows Build" > winbld.out
for /f "eol=; tokens=1,2* delims=, " %%i in (winmsi\project.list.in) do @call :dobuild %%i %%j
@goto :eof

:dobuild
@echo BEGIN BUILD /project %1 %2 >> winbld.out
:: @echo devenv /build Debug /project %1 %2
:: @devenv /build Debug /project %1 %2 >> winbld.out
@echo devenv /build Release /project %1 %2
@devenv /build Release /project %1 %2 >> winbld.out
@echo END BUILD %1  >> winbld.out
goto :eof

:: End guts of this script

:batnotfound
echo *********** ERROR: VC Config batch file not found *************
exit 3
goto end

:: TryBat(BATPATH)
:: If the BATPATH exists, use it and return 0,
:: otherwise, return 1.

:TryBat
:: Filename = %1
if not exist %1 exit /b 1
call %1
exit /b 0
goto :eof

:end
