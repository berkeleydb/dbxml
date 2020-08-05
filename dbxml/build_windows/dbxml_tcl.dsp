# Microsoft Developer Studio Project File - Name="dbxml_tcl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dbxml_tcl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_tcl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_tcl.mak" CFG="dbxml_tcl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbxml_tcl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbxml_tcl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbxml_tcl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\build_windows\Release"
# PROP BASE Intermediate_Dir "Release\dbxml_tcl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\build_windows\Release"
# PROP Intermediate_Dir "Release\dbxml_tcl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "../include" /I "../../db-4.8.26" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "DBXML_CREATE_DLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /libpath:..\build_windows\Release libdb_tcl48.lib libdb48.lib /libpath:c:/tcl/lib /libpath:../../db-4.8.26/build_windows/Release tcl84.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /machine:I386 /out:"..\build_windows\Release/libdbxml_tcl25@.dll"

# Begin Special Build Tool
PostBuild_Desc=Install files
PostBuild_Cmds=installProj.bat libdbxml_tcl25 dll ..\build_windows\Release release
# End Special Build Tool
!ELSEIF  "$(CFG)" == "dbxml_tcl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\build_windows\Debug"
# PROP BASE Intermediate_Dir "Debug\dbxml_tcl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\build_windows\Debug"
# PROP Intermediate_Dir "Debug\dbxml_tcl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "../include" /I "../../db-4.8.26" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "DBXML_CREATE_DLL" /D "CONFIG_TEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX"config.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:..\build_windows\Debug libdb48d.lib libdb_tcl48d.lib /libpath:../../db-4.8.26/build_windows/Debug /libpath:c:/tcl/lib tcl84g.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:..\build_windows\Debug/dbxml_tcl.pdb /debug /machine:I386 /out:"..\build_windows\Debug/libdbxml_tcl25@d.dll"

# Begin Special Build Tool
PostBuild_Desc=Install files
PostBuild_Cmds=installProj.bat libdbxml_tcl25 dll ..\build_windows\Debug debug
# End Special Build Tool
!ENDIF 

# Begin Target

# Name "dbxml_tcl - Win32 Release"
# Name "dbxml_tcl - Win32 Debug"
# Begin Source File

SOURCE=..\src\tcl\dbxml_tcl_wrap.cpp
# End Source File
# End Target
# End Project
