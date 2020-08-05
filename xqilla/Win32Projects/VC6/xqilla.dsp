# Microsoft Developer Studio Project File - Name="xqilla" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xqilla - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xqilla.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xqilla.mak" CFG="xqilla - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xqilla - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xqilla - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xqilla - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../build/windows/VC6\Release"
# PROP BASE Intermediate_Dir "../../build/windows/VC6\Release\xqilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../build/windows/VC6\Release"
# PROP Intermediate_Dir "../../build/windows/VC6\Release\xqilla"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XQILLA_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../include" /I "../../src/lexer" /I "../../../xerces-c-src/include" /I "../../../xerces-c-src/src" /I "../../include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XQILLA_APIS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /libpath:"../../build/windows/VC6/Release" xerces-c_3.lib /nologo /base:"0x69000000" /dll /machine:I386 /out:"../../build/windows/VC6/Release/xqilla22.dll" /implib:"../../build/windows/VC6/Release/xqilla22.lib" /libpath:"../../../xerces-c-src/Build/Win32/VC6/Release" /OPT:REF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "xqilla - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../build/windows/VC6\Debug"
# PROP BASE Intermediate_Dir "../../build/windows/VC6\Debug\xqilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../build/windows/VC6\Debug"
# PROP Intermediate_Dir "../../build/windows/VC6\Debug\xqilla"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XQILLA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../include" /I "../../src/lexer" /I "../../../xerces-c-src/include" /I "../../../xerces-c-src/src" /I "../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XQILLA_APIS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:"../../build/windows/VC6/Debug" xerces-c_3D.lib /nologo /base:"0x69000000" /dll /debug /machine:I386 /out:"../../build/windows/VC6/Debug/xqilla22d.dll" /implib:"../../build/windows/VC6/Debug/xqilla22d.lib" /pdbtype:sept /pdb:"../../build/windows/VC6/Debug/xqilla22d.pdb" /libpath:"../../../xerces-c-src/Build/Win32/VC6/Debug"

!ENDIF 

# Begin Target

# Name "xqilla - Win32 Release"
# Name "xqilla - Win32 Debug"
# Begin Group "ast"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ast\ASTNodeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\ConstantFoldingFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\ContextTuple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\ConvertFunctionArg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\ForTuple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\LetTuple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\NumericFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\OrderByTuple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\StaticAnalysis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\StaticType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\TupleNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\WhereTuple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XPath1Compat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQAnalyzeString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQApplyTemplates.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQAtomize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQAttributeConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQCallTemplate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQCastableAs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQCastAs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQCommentConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQContextItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQCopy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQCopyOf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQDocumentConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQDocumentOrder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQDOMConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQEffectiveBooleanValue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQElementConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQFunctionCall.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQFunctionConversion.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQFunctionDeref.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQFunctionRef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQGlobalVariable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQIf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQInlineFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQInstanceOf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQLiteral.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQNamespaceBinding.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQNamespaceConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQNav.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQOperator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQOrderingChange.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQPIConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQPredicate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQQuantified.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQReturn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQSequence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQStep.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQTextConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQTreatAs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQTypeswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQValidate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ast\XQVariable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\ASTNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\ASTNodeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\ConstantFoldingFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\ContextTuple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\ConvertFunctionArg.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\ForTuple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\LetTuple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\LocationInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\NumericFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\OrderByTuple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\StaticAnalysis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\StaticType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\TupleNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\WhereTuple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XPath1Compat.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQAnalyzeString.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQApplyTemplates.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQAtomize.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQAttributeConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQCallTemplate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQCastableAs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQCastAs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQCommentConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQContextItem.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQCopy.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQCopyOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQDocumentConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQDocumentOrder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQDOMConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQEffectiveBooleanValue.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQElementConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQFunctionCall.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQFunctionConversion.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQFunctionDeref.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQFunctionRef.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQGlobalVariable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQIf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQInlineFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQInstanceOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQLiteral.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQNamespaceBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQNamespaceConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQNav.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQOperator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQOrderingChange.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQPIConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQPredicate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQQuantified.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQReturn.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQSequence.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQStep.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQTextConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQTreatAs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQTypeswitch.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQValidate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\ast\XQVariable.hpp
# End Source File
# End Group
# Begin Group "axis"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\axis\NodeTest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\axis\SelfAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\axis\NodeTest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\axis\SelfAxis.hpp
# End Source File
# End Group
# Begin Group "context"

# PROP Default_Filter ""
# Begin Group "impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\context\impl\CodepointCollation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\CollationImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\ItemFactoryImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\VarHashEntryImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\VarStoreImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\VarTypeStoreImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\VarTypeStoreImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\XQContextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\context\impl\XQDynamicContextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\CodepointCollation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\CollationImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\ItemFactoryImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\VarHashEntryImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\VariableStoreTemplate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\VarStoreImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\XQContextImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\impl\XQDynamicContextImpl.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\context\UpdateFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\Collation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\ContextHelpers.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\DynamicContext.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\ExternalFunctionResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\ItemFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\MessageListener.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\ModuleResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\ModuleResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\RegexGroupStore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\Scope.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\StaticContext.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\UpdateFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\URIResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\VarHashEntry.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\VariableStore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\context\VariableTypeStore.hpp
# End Source File
# End Group
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\debug\ASTDebugHook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\debug\DebugHookDecorator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\debug\InputParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\debug\InteractiveDebugger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\debug\StackFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\debug\TupleDebugHook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\debug\ASTDebugHook.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\debug\DebugHookDecorator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\debug\DebugListener.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\debug\InputParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\debug\InteractiveDebugger.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\debug\StackFrame.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\debug\TupleDebugHook.hpp
# End Source File
# End Group
# Begin Group "dom-api"

# PROP Default_Filter ""
# Begin Group "impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XPath2ResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XPath2ResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XPathDocumentImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XPathDocumentImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XPathNamespaceImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XPathNamespaceImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaBuilderImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaBuilderImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaDocumentImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaDocumentImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaExpressionImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaExpressionImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaNSResolverImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaXMLGrammarPoolImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\impl\XQillaXMLGrammarPoolImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\dom-api\impl\XQillaNSResolverImpl.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\dom-api\XPath2NodeSerializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\XPath2NodeSerializer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\XQillaImplementation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\dom-api\XQillaImplementation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\dom-api\XPath2Result.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\dom-api\XQillaExpression.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\dom-api\XQillaNSResolver.hpp
# End Source File
# End Group
# Begin Group "events"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\events\ContentSequenceFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\events\EventGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\events\EventSerializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\events\NoInheritFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\events\NoInheritFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\events\NSFixupFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\events\QueryPathTreeFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\events\ContentSequenceFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\events\EventGenerator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\events\EventHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\events\EventSerializer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\events\NSFixupFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\events\QueryPathTreeFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\events\SequenceBuilder.hpp
# End Source File
# End Group
# Begin Group "exceptions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\exceptions\InvalidLexicalSpaceException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\exceptions\MiscException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\exceptions\XQException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\exceptions\XQillaException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\ASTException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\ContextException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\DynamicErrorException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\FunctionException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\IllegalArgumentException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\ItemException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\NamespaceLookupException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\QueryInterruptedException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\QueryTimeoutException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\StaticErrorException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\TypeErrorException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\TypeNotFoundException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\XMLParseException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\XPath2ErrorException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\XPath2TypeCastException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\XPath2TypeMatchException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\XQException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\exceptions\XQillaException.hpp
# End Source File
# End Group
# Begin Group "fastxdm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMDocument.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMNodeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMNodeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMSequenceBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMSequenceBuilder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMUpdateFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fastxdm\FastXDMUpdateFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fastxdm\FastXDMConfiguration.hpp
# End Source File
# End Group
# Begin Group "framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\framework\BaseMemoryManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\framework\ProxyMemoryManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\framework\ReferenceCounted.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\framework\StringPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\framework\XPath2MemoryManagerImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\framework\BaseMemoryManager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\framework\ProxyMemoryManager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\framework\ReferenceCounted.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\framework\StringPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\framework\XPath2MemoryManager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\framework\XPath2MemoryManagerImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\framework\XQillaExport.hpp
# End Source File
# End Group
# Begin Group "fulltext"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\fulltext\DefaultTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\DefaultTokenStore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTAnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTContains.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTContent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTDistance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTMildnot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTOr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTOrder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTScope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTUnaryNot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\fulltext\FTWords.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\AllMatches.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\DefaultTokenizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\DefaultTokenStore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTAnd.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTContains.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTContent.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTDistance.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTMildnot.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTOption.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTOr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTOrder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTRange.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTScope.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTSelection.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTUnaryNot.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTWindow.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\FTWords.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\Match.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\TokenInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\Tokenizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\fulltext\TokenStore.hpp
# End Source File
# End Group
# Begin Group "functions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\functions\ExternalFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FuncFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FuncFactoryTemplate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionAbs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionAdjustDateTimeToTimezone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionAdjustDateToTimezone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionAdjustTimeToTimezone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionAnalyzeString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionAvg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionBaseURI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionBoolean.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCeiling.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCodepointEqual.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCodepointsToString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCollection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCompare.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionConcat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionContains.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCount.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCurrentDate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCurrentDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionCurrentTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDayFromDate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDayFromDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDaysFromDuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDeepEqual.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDefaultCollation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDistinctValues.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDocAvailable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionDocumentURI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionEmpty.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionEncodeForUri.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionEndsWith.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionError.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionEscapeHtmlUri.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionExactlyOne.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionExists.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionFalse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionFloor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionFunctionArity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionFunctionName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionHoursFromDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionHoursFromDuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionHoursFromTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionId.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionIdref.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionImplicitTimezone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionIndexOf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionInScopePrefixes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionInsertBefore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionIriToUri.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionLang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionLast.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionLocalname.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionLocalNameFromQName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionLookup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionLowerCase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMatches.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMax.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMinutesFromDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMinutesFromDuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMinutesFromTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMonthFromDate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMonthFromDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionMonthsFromDuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNamespaceUri.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNamespaceURIForPrefix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNamespaceURIFromQName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNilled.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNodeName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNormalizeSpace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNormalizeUnicode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionNumber.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionOneOrMore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionParseHTML.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionParseJSON.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionParseXML.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionPartialApply.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionPosition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionPrefixFromQName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionQName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionRegexGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionRemove.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionReplace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionResolveQName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionResolveURI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionReverse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionRoot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionRound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionRoundHalfToEven.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSecondsFromDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSecondsFromDuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSecondsFromTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSerializeJSON.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionStartsWith.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionStaticBaseURI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionStringJoin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionStringLength.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionStringToCodepoints.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSubsequence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSubstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSubstringAfter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSubstringBefore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionSum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTimezoneFromDate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTimezoneFromDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTimezoneFromTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTokenize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTranslate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionTrue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionUnordered.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionUnparsedText.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionUpperCase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionYearFromDate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionYearFromDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionYearsFromDuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\FunctionZeroOrOne.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\XQillaFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\functions\XQUserFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\ExternalFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FuncFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionAbs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionAdjustDateTimeToTimezone.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionAdjustDateToTimezone.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionAdjustTimeToTimezone.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionAnalyzeString.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionAvg.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionBaseURI.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionBoolean.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCeiling.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCodepointEqual.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCodepointsToString.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCollection.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCompare.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionConcat.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionConstructor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionContains.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCount.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCurrentDate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCurrentDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionCurrentTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionData.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDayFromDate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDayFromDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDaysFromDuration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDeepEqual.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDefaultCollation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDistinctValues.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDoc.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDocAvailable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDocument.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionDocumentURI.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionEmpty.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionEncodeForUri.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionEndsWith.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionError.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionEscapeHtmlUri.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionExactlyOne.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionExists.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionFalse.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionFloor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionFunctionArity.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionFunctionName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionHoursFromDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionHoursFromDuration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionHoursFromTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionId.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionIdref.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionImplicitTimezone.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionIndexOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionInScopePrefixes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionInsertBefore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionIriToUri.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionLang.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionLast.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionLocalname.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionLocalNameFromQName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionLookup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionLowerCase.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMatches.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMax.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMin.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMinutesFromDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMinutesFromDuration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMinutesFromTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMonthFromDate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMonthFromDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionMonthsFromDuration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNamespaceUri.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNamespaceURIForPrefix.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNamespaceURIFromQName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNilled.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNodeName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNormalizeSpace.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNormalizeUnicode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNot.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionNumber.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionOneOrMore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionParseHTML.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionParseJSON.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionParseXML.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionPartialApply.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionPosition.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionPrefixFromQName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionQName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionRegexGroup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionRemove.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionReplace.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionResolveQName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionResolveURI.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionReverse.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionRoot.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionRound.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionRoundHalfToEven.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSecondsFromDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSecondsFromDuration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSecondsFromTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSerializeJSON.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionStartsWith.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionStaticBaseURI.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionString.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionStringJoin.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionStringLength.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionStringToCodepoints.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSubsequence.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSubstring.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSubstringAfter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSubstringBefore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionSum.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTimezoneFromDate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTimezoneFromDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTimezoneFromTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTokenize.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTrace.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTranslate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionTrue.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionUnordered.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionUnparsedText.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionUpperCase.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionYearFromDate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionYearFromDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionYearsFromDuration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\FunctionZeroOrOne.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\XQillaFunction.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\functions\XQUserFunction.hpp
# End Source File
# End Group
# Begin Group "items"

# PROP Default_Filter ""
# Begin Group "impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\items\impl\ATAnySimpleTypeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATAnySimpleTypeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATAnyURIOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATAnyURIOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATBase64BinaryOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATBase64BinaryOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATBooleanOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATDateOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATDateTimeOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATDecimalOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATDoubleOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATDurationOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATFloatOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATGDayOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATGMonthDayOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATGMonthOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATGYearMonthOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATGYearOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATHexBinaryOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATHexBinaryOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATNotationOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATNotationOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATQNameOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATStringOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATStringOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATTimeOrDerivedImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATUntypedAtomicImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\ATUntypedAtomicImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\FunctionRefImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\impl\FunctionRefImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATBooleanOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATDateOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATDateTimeOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATDecimalOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATDoubleOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATDurationOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATFloatOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATGDayOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATGMonthDayOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATGMonthOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATGYearMonthOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATGYearOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATQNameOrDerivedImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\impl\ATTimeOrDerivedImpl.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\items\AnyAtomicType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\ATDurationOrDerived.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\ATUntypedAtomic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\DatatypeFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\DatatypeFactoryTemplate.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\DatatypeLookup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\DateOrTimeType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\Item.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\Node.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\Numeric.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\items\Timezone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\AnyAtomicType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATAnySimpleType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATAnyURIOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATBase64BinaryOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATBooleanOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATDateOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATDateTimeOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATDecimalOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATDoubleOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATDurationOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATFloatOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATGDayOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATGMonthDayOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATGMonthOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATGYearMonthOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATGYearOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATHexBinaryOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATNotationOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATQNameOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATStringOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATTimeOrDerived.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\ATUntypedAtomic.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\DatatypeFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\DatatypeLookup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\DateOrTimeType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\FunctionRef.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\Item.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\Node.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\Numeric.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\items\Timezone.hpp
# End Source File
# End Group
# Begin Group "lexer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\lexer\FlexLexer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lexer\XQLexer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lexer\XQLexer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lexer\XQLexer2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lexer\XSLT2Lexer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lexer\XSLT2Lexer.hpp
# End Source File
# End Group
# Begin Group "mapm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\mapm\m_apm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\m_apm_lc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm5sin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_add.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_cpi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_div.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_exp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_fam.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_fft.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_flr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_fpf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_gcd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_lg2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_lg3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_log.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_mt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_mul.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_pow.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_rcp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_rnd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_set.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapm_sin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmasin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmasn0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmcbrt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmcnst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmfact.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmfmul.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmgues.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmhasn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmhsin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmipwr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmistr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmpwr2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmrsin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmsqrt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmstck.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmutil.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mapm\mapmutl2.c
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\mapm\m_apm.h
# End Source File
# End Group
# Begin Group "operators"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\operators\And.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\ArithmeticOperator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\ComparisonOperator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Divide.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Equals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Except.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\GeneralComp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\GreaterThan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\GreaterThanEqual.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\IntegerDivide.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Intersect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\LessThan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\LessThanEqual.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Minus.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Mod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Multiply.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\NodeComparison.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\NotEquals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Or.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\OrderComparison.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Plus.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Range.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\UnaryMinus.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\operators\Union.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\And.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\ArithmeticOperator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\ComparisonOperator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Divide.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Equals.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Except.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\GeneralComp.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\GreaterThan.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\GreaterThanEqual.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\IntegerDivide.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Intersect.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\LessThan.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\LessThanEqual.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Minus.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Mod.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Multiply.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\NodeComparison.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\NotEquals.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Or.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\OrderComparison.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Plus.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Range.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\UnaryMinus.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\operators\Union.hpp
# End Source File
# End Group
# Begin Group "optimizer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\optimizer\ASTCopier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\optimizer\ASTReleaser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\optimizer\ASTVisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\optimizer\Optimizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\optimizer\PartialEvaluator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\optimizer\QueryPathNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\optimizer\QueryPathTreeGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\optimizer\StaticTyper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\ASTCopier.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\ASTReleaser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\ASTVisitor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\Optimizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\PartialEvaluator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\QueryPathNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\QueryPathTreeGenerator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\optimizer\StaticTyper.hpp
# End Source File
# End Group
# Begin Group "parser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\parser\QName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\parser\XQParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\parser\XQParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\parser\QName.hpp
# End Source File
# End Group
# Begin Group "runtime"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\runtime\ClosureResult.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\EmptyResult.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\LazySequenceResult.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\Result.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\ResultBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\ResultBufferImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\ResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\Sequence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\SequenceResult.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\runtime\SingleResult.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\ClosureResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\EmptyResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\LazySequenceResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\Result.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\ResultBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\ResultBufferImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\ResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\Sequence.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\SequenceResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\SingleResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\runtime\TupleResult.hpp
# End Source File
# End Group
# Begin Group "schema"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\schema\AnyAtomicTypeDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\schema\DocumentCacheImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\schema\FaxppDocumentCacheImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\schema\FaxppDocumentCacheImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\schema\SchemaValidatorFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\schema\SequenceType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\schema\AnyAtomicTypeDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\schema\DocumentCache.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\schema\DocumentCacheImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\schema\SchemaValidatorFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\schema\SequenceType.hpp
# End Source File
# End Group
# Begin Group "simple-api"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\simple-api\XQilla.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\simple-api\XQQuery.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\simple-api\XQilla.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\simple-api\XQillaConfiguration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\simple-api\XQQuery.hpp
# End Source File
# End Group
# Begin Group "update"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\update\FunctionPut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\PendingUpdateList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UApplyUpdates.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UDelete.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UInsertAfter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UInsertAsFirst.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UInsertAsLast.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UInsertBefore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UInsertInto.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\URename.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UReplace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UReplaceValueOf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\update\UTransform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\FunctionPut.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\PendingUpdateList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UApplyUpdates.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UDelete.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UInsertAfter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UInsertAsFirst.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UInsertAsLast.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UInsertBefore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UInsertInto.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\URename.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UReplace.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UReplaceValueOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\update\UTransform.hpp
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\utils\ContextUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\DateUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\DateUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\NumUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\PrintAST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\UCANormalizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\UCANormalizer1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\UCANormalizer2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\UCANormalizer3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\UCANormalizer4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\UTF8Str.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\XMLChCompare.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\XPath2NSUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\XPath2Utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\XQillaPlatformUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\XStr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\ContextUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\NumUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\PrintAST.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\UCANormalizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\UTF8Str.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\XMLChCompare.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\XPath2NSUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\XPath2Utils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\XQillaPlatformUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\utils\XStr.hpp
# End Source File
# End Group
# Begin Group "xerces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\xerces\AncestorAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\AncestorAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\AncestorOrSelfAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\AncestorOrSelfAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\AttributeAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\AttributeAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\Axis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\Axis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\ChildAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\ChildAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\DescendantAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\DescendantAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\DescendantOrSelfAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\DescendantOrSelfAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\FollowingAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\FollowingAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\FollowingSiblingAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\FollowingSiblingAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\NamespaceAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\NamespaceAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\ParentAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\ParentAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\PrecedingAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\PrecedingAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\PrecedingSiblingAxis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\PrecedingSiblingAxis.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesNodeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesNodeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesSequenceBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesSequenceBuilder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesUpdateFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesUpdateFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesURIResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xerces\XercesURIResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\xerces\XercesConfiguration.hpp
# End Source File
# End Group
# Begin Group "xqc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCDynamicContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCDynamicContext.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCExpression.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCImplementation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCImplementation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCSequence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCSequence.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCStaticContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqc\XQillaXQCStaticContext.hpp
# End Source File
# End Group
# Begin Group "xqts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\xqts\TestSuiteParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqts\TestSuiteResultListener.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xqts\TestSuiteRunner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\xqts\TestSuiteParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\xqts\TestSuiteResultListener.hpp
# End Source File
# Begin Source File

SOURCE=..\..\include\xqilla\xqts\TestSuiteRunner.hpp
# End Source File
# End Group
# Begin Group "yajl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\yajl\yajl.c
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_buf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_buf.h
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_common.h
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_encode.c
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_encode.h
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_gen.c
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_gen.h
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_lex.c
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_lex.h
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_parse.h
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_parser.c
# End Source File
# Begin Source File

SOURCE=..\..\src\yajl\yajl_parser.h
# End Source File
# End Group
# End Target
# End Project
