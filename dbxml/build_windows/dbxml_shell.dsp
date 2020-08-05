# Microsoft Developer Studio Project File - Name="dbxml_shell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dbxml_shell - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_shell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_shell.mak" CFG="dbxml_shell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbxml_shell - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dbxml_shell - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbxml_shell - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\build_windows\Release"
# PROP BASE Intermediate_Dir "Release\dbxml_shell"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\build_windows\Release"
# PROP Intermediate_Dir "Release\dbxml_shell"
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
PostBuild_Cmds=installProj.bat dbxml_shell prog ..\build_windows\Release release
# End Special Build Tool
!ELSEIF  "$(CFG)" == "dbxml_shell - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\build_windows\Debug"
# PROP BASE Intermediate_Dir "Debug\dbxml_shell"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\build_windows\Debug"
# PROP Intermediate_Dir "Debug\dbxml_shell"
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
# ADD LINK32 /libpath:..\build_windows\Debug /libpath:Debug libdbxml25d.lib libdb48d.lib xerces-c_3d.lib xqilla22d.lib /libpath:../../xqilla/build/windows/vc6/Debug /libpath:../../db-4.8.26/build_windows/Debug /nologo /subsystem:console /pdb:./Debug/dbxml_shell.pdb /debug /machine:I386 /nodefaultlib:"libcmtd" /fixed:no /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/build/win32/vc6/debug

# Begin Special Build Tool
PostBuild_Desc=Install files
PostBuild_Cmds=installProj.bat dbxml_shell prog ..\build_windows\Debug debug
# End Special Build Tool
!ENDIF 

# Begin Target

# Name "dbxml_shell - Win32 Release"
# Name "dbxml_shell - Win32 Debug"
# Begin Source File

SOURCE=..\src\clib\getopt.c
# End Source File
# Begin Source File

SOURCE=..\src\common\dbxml_util_sig.c
# End Source File
# Begin Source File

SOURCE=..\src\common\SigBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\Timer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AbortCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AbortCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AddAliasCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AddAliasCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AddDocumentCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AddDocumentCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AddIndexCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\AddIndexCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CommentCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CommentCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CommitCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CommitCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CompactCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CompactCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ContainerQueryCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ContainerQueryCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ContextItemQueryCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ContextItemQueryCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CreateContainerCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\CreateContainerCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\dbxmlsh.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DebugCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DebugCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DebugOptimizationCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DebugOptimizationCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DefaultShell.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DefaultShell.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DeleteDocumentCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DeleteDocumentCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DeleteIndexCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\DeleteIndexCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\EchoCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\EchoCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\EdgeIndexLookupCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\EdgeIndexLookupCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\Environment.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\Environment.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\GetDocumentsCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\GetDocumentsCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\GetMetaDataCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\GetMetaDataCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\HelpCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\HelpCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\IncludeCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\IncludeCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\IndexLookupCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\IndexLookupCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\InfoCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\InfoCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\LazyCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\LazyCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ListIndexesCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ListIndexesCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\OpenContainerCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\OpenContainerCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\PreLoadCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\PreLoadCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\PrepareCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\PrepareCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\PrintCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\PrintCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ProjectionCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ProjectionCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\QueryCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\QueryCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\QueryPlanCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\QueryPlanCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\QuitCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\QuitCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ReindexCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ReindexCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\RemoveAliasCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\RemoveAliasCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\RemoveContainerCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\RemoveContainerCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ReturnTypeCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ReturnTypeCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetAutoIndexingCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetAutoIndexingCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetBaseURICommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetBaseURICommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetIgnoreCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetIgnoreCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetMetaDataCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetMetaDataCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetNamespaceCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetNamespaceCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetQueryTimeoutCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetQueryTimeoutCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetTypedVariableCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetTypedVariableCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetVariableCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetVariableCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetVerboseCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SetVerboseCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\Shell.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\Shell.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ShellCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\ShellCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\StartTransactionCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\StartTransactionCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\StatsLookupCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\StatsLookupCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SyncCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\SyncCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\TimeCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\TimeCommand.hpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\UpgradeCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\src\utils\shell\UpgradeCommand.hpp
# End Source File
# End Target
# End Project
