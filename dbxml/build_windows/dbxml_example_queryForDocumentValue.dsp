# Microsoft Developer Studio Project File - Name="dbxml_example_queryForDocumentValue" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dbxml_example_queryForDocumentValue - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_example_queryForDocumentValue.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_example_queryForDocumentValue.mak" CFG="dbxml_example_queryForDocumentValue - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbxml_example_queryForDocumentValue - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dbxml_example_queryForDocumentValue - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbxml_example_queryForDocumentValue - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\build_windows\Release"
# PROP BASE Intermediate_Dir "Release\dbxml_example_queryForDocumentValue"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\build_windows\Release"
# PROP Intermediate_Dir "Release\dbxml_example_queryForDocumentValue"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "../include" /I "../src/dbxml" /I "../../xqilla/include" /I "../../db-4.8.26/include" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /libpath:..\build_windows\Release /libpath:Release libdbxml25.lib libdb48.lib xerces-c_3.lib xqilla22.lib /libpath:../../xqilla/build/windows/vc6/Release /libpath:../../db-4.8.26/build_windows/Release /nologo /subsystem:console /machine:I386 /nodefaultlib:"libcmt" /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/build/win32/vc6/release
# Begin Special Build Tool
PostBuild_Desc=Install files
PostBuild_Cmds=installProj.bat dbxml_example_queryForDocumentValue prog ..\build_windows\Release release
# End Special Build Tool
!ELSEIF  "$(CFG)" == "dbxml_example_queryForDocumentValue - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\build_windows\Debug"
# PROP BASE Intermediate_Dir "Debug\dbxml_example_queryForDocumentValue"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\build_windows\Debug"
# PROP Intermediate_Dir "Debug\dbxml_example_queryForDocumentValue"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "../include" /I "../src/dbxml" /I "../../xqilla/include" /I "../../db-4.8.26/include" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xerces-c-src/include"  /I "../../xerces-c-src/src" /I "../../include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:..\build_windows\Debug /libpath:Debug libdbxml25d.lib libdb48d.lib xerces-c_3d.lib xqilla22d.lib /libpath:../../xqilla/build/windows/vc6/Debug /libpath:../../db-4.8.26/build_windows/Debug /nologo /subsystem:console /pdb:./Debug/dbxml_example_queryForDocumentValue.pdb /debug /machine:I386 /nodefaultlib:"libcmtd" /fixed:no /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/build/win32/vc6/debug

# Begin Special Build Tool
PostBuild_Desc=Install files
PostBuild_Cmds=installProj.bat dbxml_example_queryForDocumentValue prog ..\build_windows\Debug debug
# End Special Build Tool
!ENDIF 

# Begin Target

# Name "dbxml_example_queryForDocumentValue - Win32 Release"
# Name "dbxml_example_queryForDocumentValue - Win32 Debug"
# Begin Source File

SOURCE=..\examples\cxx\gettingStarted\queryForDocumentValue.cpp
# End Source File
# End Target
# End Project
