@echo off
::  Helper script to build Perl support for BDB XML using Windows tools
::  Assumes current directory is src/perl
::  Builds output to ../../../perl
::  By default it calls "nmake install" if first argument is
::  "noinstall" installation is not done
::

call :TryBat "c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat" && goto BATFOUND1

call :TryBat "c:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat" && goto BATFOUND2

call :TryBat "c:\Program Files\Microsoft Visual Studio.NET\Common7\Tools\vsvars32.bat" && goto BATFOUND3

call :TryBat "c:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat" && goto BATFOUND4

goto BATNOTFOUND

:BATFOUND1
echo Using Visual Studio .NET 2003
goto BATFOUND

:BATFOUND2
echo Using Visual Studio .NET
echo *********** CHECK: Make sure the binaries are built with the same system libraries that are shipped.
goto BATFOUND

:BATFOUND3
echo Using Visual Studio.NET
echo *********** CHECK: Make sure the binaries are built with the same system libraries that are shipped.
goto BATFOUND

:BATFOUND4
echo Using Visual Studio 2005
echo *********** CHECK: Make sure the binaries are built with the same system libraries that are shipped.
goto BATFOUND

:BATFOUND

:: Use the correct line, depending on whether you are building
:: release or debug
:: perl Makefile.PL -config config.win32d >> winbld.out
:: perl Makefile.PL prefix=../../../perl -config config.win32 >> winbld.out
perl Makefile.PL -config config.win32 >> winbld.out
nmake >> winbld.out
nmake ppd >> winbld.out
if /I [%1] NEQ [noinstall] (
    nmake install >> winbld.out
)

goto END


:ERROR
echo *********** ERROR: during build.bat *************
echo *********** ERROR: during build.bat *************  >> winbld.err
exit 1
goto END

:BATNOTFOUND
echo *********** ERROR: VC Config batch file not found *************
echo *********** ERROR: VC Config batch file not found *************  >> winbld.err
exit 3
goto END

:: TryBat(BATPATH)
:: If the BATPATH exists, use it and return 0,
:: otherwise, return 1.

:TryBat
:: Filename = %1
if not exist %1 exit /b 1
call %1
exit /b 0
goto :EOF

:END
