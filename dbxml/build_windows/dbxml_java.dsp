# Microsoft Developer Studio Project File - Name="dbxml_java" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dbxml_java - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_java.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_java.mak" CFG="dbxml_java - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbxml_java - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbxml_java - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbxml_java - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\build_windows\Release"
# PROP BASE Intermediate_Dir "Release\dbxml_java"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\build_windows\Release"
# PROP Intermediate_Dir "Release\dbxml_java"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "../include" /I "../../db-4.8.26" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xqilla/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "DBXML_CREATE_DLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /libpath:..\build_windows\Release xerces-c_3.lib xqilla22.lib /libpath:../../xqilla/build/windows/vc6/Release /libpath:../../db-4.8.26/build_windows/Release libdb48.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /machine:I386 /out:"..\build_windows\Release/libdbxml_java25.dll" /implib:"..\build_windows\Release/libdbxml_java25.lib"  /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/build/win32/vc6/release
# Begin Custom Build
ProjDir=.
InputPath=..\build_windows\Release/libdbxml_java25.dll
SOURCE="$(InputPath)"

"force_compilation.txt" : $(SOURCE) "$(INTDIR)"
	mkdir ".\Release\classes\dummy" ".\..\build_windows\dummy"
	echo compiling Berkeley DB XML classes
	javac -O -d "Release/classes" -classpath "Release/classes;..\build_windows/db.jar;Release/classes;../../db-4.8.26/build_windows/Release/db.jar" ../src/java/com/sleepycat/dbxml/*.java ../src/java/com/sleepycat/db/*.java
	echo creating jar files
	cd "./Release/classes"
	jar cf ../../..\build_windows/dbxml.jar com/sleepycat/dbxml/*.class com/sleepycat/db/*.class
	cd "../.."
	echo compiling Berkeley DB XML examples
	javac -O -d "Release/classes" -classpath "Release/classes;..\build_windows/dbxml.jar;..\build_windows/db.jar;../../db-4.8.26/build_windows/Release/db.jar;Release/classes" ../examples/java/gettingStarted/*.java ../examples/java/basic/*.java
	echo creating example jar file
	cd "./Release/classes"
	jar cf ../../..\build_windows/dbxmlexamples.jar dbxml/gettingStarted/*.class dbxml/basic/*.class
	cd "../.."
	rmdir ".\Release\classes\dummy" ".\..\build_windows\dummy"
	echo Copying Jar files
	copy *.jar ..\..\jar
	echo Java build finished
# End Custom Build

# Begin Special Build Tool
PostBuild_Desc=Install files
PostBuild_Cmds=installProj.bat dbxml_java dll ..\build_windows\Release release
xcopy *.jar ..\..\jar /F/Y"
# End Special Build Tool
!ELSEIF  "$(CFG)" == "dbxml_java - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\build_windows\Debug"
# PROP BASE Intermediate_Dir "Debug\dbxml_java"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\build_windows\Debug"
# PROP Intermediate_Dir "Debug\dbxml_java"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "../include" /I "../../db-4.8.26" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xqilla/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "DBXML_CREATE_DLL" /D "CONFIG_TEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX"config.h" /FD /c
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
# ADD LINK32 /libpath:..\build_windows\Debug xerces-c_3d.lib xqilla22d.lib /libpath:../../xqilla/build/windows/vc6/Debug /libpath:../../db-4.8.26/build_windows/Debug libdb48d.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:..\build_windows\Debug/dbxml_java.pdb /debug /machine:I386 /out:"..\build_windows\Debug/libdbxml_java25d.dll" /implib:"..\build_windows\Debug/libdbxml_java25d.lib" /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/build/win32/vc6/debug
# Begin Custom Build
ProjDir=.
InputPath=..\build_windows\Debug/libdbxml_java25d.dll
SOURCE="$(InputPath)"

"force_compilation.txt" : $(SOURCE) "$(INTDIR)"
	mkdir ".\Debug\classes\dummy" ".\..\build_windows\dummy"
	echo compiling Berkeley DB XML classes
	javac -O -d "Debug/classes" -classpath "Debug/classes;..\build_windows/db.jar;Debug/classes;../../db-4.8.26/build_windows/Debug/db.jar" ../src/java/com/sleepycat/dbxml/*.java ../src/java/com/sleepycat/db/*.java
	cd "./Debug/classes"
	echo creating jar files
	jar cf ../../..\build_windows/dbxml.jar com/sleepycat/dbxml/*.class com/sleepycat/db/*.class
	cd "../.."
	echo compiling Berkeley DB XML examples
	javac -O -d "Debug/classes" -classpath "Debug/classes;..\build_windows/dbxml.jar;..\build_windows/db.jar;Debug/classes;../../db-4.8.26/build_windows/Debug/db.jar" ../examples/java/gettingStarted/*.java ../examples/java/basic/*.java
	cd "Debug/classes"
	echo creating example jar file
	jar cf ../../..\build_windows/dbxmlexamples.jar dbxml/gettingStarted/*.class dbxml/basic/*.class
	cd "../.."
	rmdir ".\Debug\classes\dummy" ".\..\build_windows\dummy"
	echo Copying Jar files
	copy *.jar ..\..\jar
	echo Java build finished
# End Custom Build

# Begin Special Build Tool
PostBuild_Desc=Install files
PostBuild_Cmds="installProj.bat dbxml_java dll ..\build_windows\Debug release
xcopy *.jar ..\..\jar /F/Y"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "dbxml_java - Win32 Release"
# Name "dbxml_java - Win32 Debug"
# Begin Source File

SOURCE=..\src\java\dbxml_java_wrap.cpp
# End Source File
# End Target
# End Project
