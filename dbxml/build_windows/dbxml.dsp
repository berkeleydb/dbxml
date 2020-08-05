# Microsoft Developer Studio Project File - Name="dbxml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dbxml - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbxml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbxml.mak" CFG="dbxml - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbxml - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbxml - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbxml - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\build_windows\Release"
# PROP BASE Intermediate_Dir "Release\dbxml"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\build_windows\Release"
# PROP Intermediate_Dir "Release\dbxml"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DBXML_DLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "../include" /I "../src/dbxml"  /I "../../db-4.8.26/include" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xqilla/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "NDEBUG" /D "_USRDLL" /D "DBXML_CREATE_DLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX"DbXmlInternal.hpp" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /libpath:..\build_windows\Release xerces-c_3.lib xqilla22.lib libdb48.lib  /libpath:../../xqilla/build/windows/vc6/Release /libpath:../../db-4.8.26/build_windows/Release kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"..\build_windows\Release/libdbxml25.dll" /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/Build/Win32/VC6/release
# Begin Special Build Tool
PostBuild_Desc=Copy extra files
PostBuild_Cmds=copyFiles.bat
# End Special Build Tool
!ELSEIF  "$(CFG)" == "dbxml - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\build_windows\Debug"
# PROP BASE Intermediate_Dir "Debug\dbxml"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\build_windows\Debug"
# PROP Intermediate_Dir "Debug\dbxml"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DBXML_DLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "../include" /I "../src/dbxml"  /I "../../db-4.8.26/include" /I "../../db-4.8.26/build_windows" /I "../../db-4.8.26/dbinc" /I "../../xqilla/include" /I "../../xerces-c-src/include"  /I "../../xerces-c-src/src" /I "../../include" /D "_DEBUG" /D "_USRDLL" /D "DBXML_CREATE_DLL" /D "_DLL" /D "DBG_ENABLED" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX"DbXmlInternal.hpp" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:..\build_windows\Debug libdb48d.lib xerces-c_3d.lib xqilla22d.lib /libpath:../../xqilla/build/windows/vc6/Debug /libpath:../../db-4.8.26/build_windows/Debug kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:..\build_windows\Debug/dbxmld.pdb /debug /machine:I386 /out:"..\build_windows\Debug/libdbxml25d.dll" /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/Build/Win32/VC6/Debug
# Begin Special Build Tool
PostBuild_Desc=Copy extra files
PostBuild_Cmds=copyFiles.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "dbxml - Win32 Release"
# Name "dbxml - Win32 Debug"
# Begin Source File

SOURCE=..\build_windows\libdbxml.rc
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\DbXml.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\DbXmlFwd.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlCompression.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlContainer.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlContainerConfig.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlData.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlDebugListener.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlDocument.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlEventReader.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlEventReaderToWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlEventWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlException.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlExternalFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlIndexLookup.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlIndexSpecification.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlManager.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlMetaDataIterator.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlNamespace.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlPortability.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlQueryContext.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlQueryExpression.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlResults.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlStatistics.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlTransaction.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlUpdateContext.hpp
# End Source File
# Begin Source File

SOURCE=..\include\dbxml\XmlValue.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\AtomicNodeValue.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\AtomicNodeValue.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\BaseInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\BaseInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Buffer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\BulkPut.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\BulkPut.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\CacheDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\CacheDatabase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\CacheDatabaseMinder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\CacheDatabaseMinder.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\CompressionZlib.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\CompressionZlib.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ConfigurationDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ConfigurationDatabase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Container.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Container.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ContainerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ContainerBase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ContainerConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ContainerConfig.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Counters.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Counters.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Cursor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Cursor.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DatabaseNodeValue.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DatabaseNodeValue.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlConfiguration.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlContains.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlContains.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlDocAvailable.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlDocAvailable.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlDocumentCacheImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlDocumentCacheImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlFactoryImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlFactoryImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlNodeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlNodeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlNodeTest.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlNodeTest.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlPredicate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlPredicate.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlPrintAST.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlPrintAST.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlSequenceBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlSequenceBuilder.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlUpdateFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlUpdateFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlUri.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlUri.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlURIResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlURIResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\Join.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\Join.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\LookupIndexFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\LookupIndexFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\MetaDataFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\MetaDataFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\NodeHandleFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\NodeHandleFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\db_rdbt.c
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\db_rdbt.h
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\db_utils.h
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DbWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DbWrapper.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DbXmlInternal.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DbXmlInternal.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\DbXmlASTDebugHook.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\DbXmlASTDebugHook.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\DbXmlDebugHookDecorator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\DbXmlDebugHookDecorator.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\DbXmlTupleDebugHook.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\DbXmlTupleDebugHook.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\QPDebugHook.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\debug\QPDebugHook.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DictionaryCache.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DictionaryCache.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DictionaryDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DictionaryDatabase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DocID.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DocID.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Document.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Document.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DocumentDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DocumentDatabase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Globals.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Globals.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\HighResTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\HighResTimer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexEntry.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Indexer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Indexer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexLookup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexLookup.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexSpecification.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexSpecification.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Key.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Key.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyGenerator.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyStash.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyStash.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyStatistics.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyStatistics.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\LocalFileInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\LocalFileInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Log.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\LogDbXml.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Manager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Manager.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MemBufInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MemBufInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MetaDatum.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MetaDatum.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MutexLock.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MutexLock.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Name.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Name.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\NameID.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\NameID.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\NamespaceMap.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\EventReader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\EventReader.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\EventReaderToWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\EventReaderToWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\EventWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsConstants.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDoc.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDocument.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDocumentDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDocumentDatabase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDom.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDom.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEvent.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEventReader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEventReader.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEventWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEventWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsFormat.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsFormat.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsHandlerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsHandlerBase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsImpliedSchemaFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsImpliedSchemaFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsNid.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsNid.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsNode.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsRawNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsRawNode.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsReindexer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsReindexer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsSAX2Reader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsSAX2Reader.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsTypes.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsUpdate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsUpdate.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsUpgrade.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsUpgrade.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsUtil.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsXercesIndexer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsXercesIndexer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsXercesTranscoder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsXercesTranscoder.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\PullEventInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\PullEventInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\OperationContext.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\ASTReplaceOptimizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\ASTReplaceOptimizer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\DbXmlStaticTyper.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\DbXmlStaticTyper.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\ImpliedSchemaGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\ImpliedSchemaGenerator.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\NodeVisitingOptimizer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\QueryPlanGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\QueryPlanGenerator.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\QueryPlanOptimizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\QueryPlanOptimizer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\AncestorJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\AncestorJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ASTToQueryPlan.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ASTToQueryPlan.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\AttributeJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\AttributeJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\AttributeOrChildJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\AttributeOrChildJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\BufferQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\BufferQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ChildJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ChildJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\CollectionQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\CollectionQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ContextNodeQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ContextNodeQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\DecisionPointQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\DecisionPointQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\DescendantJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\DescendantJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\DocQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\DocQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\EmptyQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\EmptyQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ExceptQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ExceptQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\FilterQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\FilterQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ImpliedSchemaNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ImpliedSchemaNode.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\IntersectQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\IntersectQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\LevelFilterQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\LevelFilterQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\NodeInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\NodeInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\NodeIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\NodeIterator.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\NodePredicateFilterQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\NodePredicateFilterQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ParentJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ParentJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ParentOfAttributeJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ParentOfAttributeJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ParentOfChildJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ParentOfChildJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\PredicateFilterQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\PredicateFilterQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryExecutionContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryExecutionContext.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryPlan.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryPlan.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryPlanHolder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryPlanHolder.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryPlanToAST.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\QueryPlanToAST.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\SequentialScanQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\SequentialScanQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\StepQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\StepQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\StructuralJoinQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\StructuralJoinQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\UnionQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\UnionQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ValueFilterQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\ValueFilterQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\VariableQP.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\query\VariableQP.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryContext.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryExpression.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\RawNodeValue.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\RawNodeValue.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ReferenceCounted.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ReferenceCounted.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ReferenceMinder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ReferenceMinder.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Results.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Results.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ResultsEventWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ResultsEventWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ScopedDbt.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ScopedPtr.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\SharedPtr.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Statistics.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Statistics.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\StdInInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\StdInInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\StructuralStatsDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\StructuralStatsDatabase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Syntax.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Syntax.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\SyntaxDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\SyntaxDatabase.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\SyntaxManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\SyntaxManager.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\tokenizer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Transaction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Transaction.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\TypeConversions.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\UpdateContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\UpdateContext.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\URLInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\URLInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\UTF8.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\UTF8.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Value.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Value.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\VariableBindings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\VariableBindings.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlCompression.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlContainerConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlData.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlEventReader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlEventReaderToWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlEventWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlException.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlIndexLookup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlIndexSpecification.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlInputStreamWrapper.hpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlMetaDataIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlNamespace.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlQueryContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlQueryExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlResults.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlStatistics.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlTransaction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlUpdateContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlValue.cpp
# End Source File
# End Target
# End Project

