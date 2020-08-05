AUTOMAKE_OPTIONS = foreign dist-zip

##################################################
# Libraries, executables and JAR files we build
##################################################

UTIL_PROGS = dbxml dbxml_dump dbxml_load dbxml_load_container

EXAMPLES = exampleLoadContainer addIndex \
	buildDB deleteDocument deleteIndex \
	simpleAdd \
	simpleContainer simpleContainerInEnv \
	simpleQuery queryForDocumentValue \
	queryForMetaData queryWithContext \
	queryWithDocumentNames replaceIndex \
	retrieveDB updateDocument \
	helloWorld helloWorldTxn query queryTxn \
	xercesDomTranslator \
	externalFunction \
	compression \
	debug

TEST_PROGS=	dbxml_test_driver dbxml_test_databases dbxml_test_query_processor_api dbxml_test_input_streams dbxml_xqts_runner dbxml_test_ha

XMLUNIT_TESTS= dbxml_unit_runner

libj_jarfile=	dbxml.jar
libj_exjarfile=	dbxmlexamples.jar
libj_perfjarfile= dbxmlperf.jar
libj_testjarfile= dbxmltest.jar

bin_PROGRAMS = $(UTIL_PROGS)
noinst_PROGRAMS = $(EXAMPLES)
EXTRA_PROGRAMS = dbxml_benchmark dbxml_perf dbxml_disk

if BUILD_TEST
noinst_PROGRAMS += $(TEST_PROGS) $(XMLUNIT_TESTS)
endif

lib_LTLIBRARIES = libdbxml.la

if BUILD_TCL
lib_LTLIBRARIES += libdbxml_tcl.la
endif

if BUILD_JAVA
lib_LTLIBRARIES += libdbxml_java.la

jardir = $(libdir)
jar_DATA = $(libj_jarfile)

noinst_DATA = $(libj_exjarfile)

if BUILD_TEST
noinst_DATA += $(libj_testjarfile)
endif
endif

##################################################
# Global flags
##################################################

CPPFLAGS = -I$(top_srcdir)/include/ -I$(top_srcdir)/src/dbxml -I$(top_srcdir)/src/common \
  $(berkeleydb_include) $(xqilla_include) $(xerces_include) $(zlib_include)
LDFLAGS = -L$(berkeleydb_lib) -R$(berkeleydb_lib) -L$(xqilla_lib) -R$(xqilla_lib) \
  -L$(xerces_lib) -R$(xerces_lib) $(zlib_lib) -l$(berkeleydb_library) -lxqilla -lxerces-c $(zlib_library)
LDADD = libdbxml.la

##################################################
# C++ source files
##################################################

libdbxml_la_LDFLAGS = -release $(DBXML_VERSION_MAJOR).$(DBXML_VERSION_MINOR) -no-undefined
libdbxml_la_SOURCES = \
src/dbxml/dataItem/DbXmlDocAvailable.cpp \
src/dbxml/dataItem/DbXmlFactoryImpl.hpp \
src/dbxml/dataItem/DbXmlASTNode.hpp \
src/dbxml/dataItem/DbXmlContains.cpp \
src/dbxml/dataItem/DbXmlContains.hpp \
src/dbxml/dataItem/DbXmlPredicate.hpp \
src/dbxml/dataItem/DbXmlPredicate.cpp \
src/dbxml/dataItem/DbXmlSequenceBuilder.cpp \
src/dbxml/dataItem/DbXmlSequenceBuilder.hpp \
src/dbxml/dataItem/DbXmlDocAvailable.hpp \
src/dbxml/dataItem/Join.cpp \
src/dbxml/dataItem/DbXmlFactoryImpl.cpp \
src/dbxml/dataItem/Join.hpp \
src/dbxml/dataItem/DbXmlFunction.cpp \
src/dbxml/dataItem/DbXmlFunction.hpp \
src/dbxml/dataItem/DbXmlUpdateFactory.cpp \
src/dbxml/dataItem/DbXmlNodeImpl.cpp \
src/dbxml/dataItem/DbXmlNodeTest.cpp \
src/dbxml/dataItem/DbXmlNodeTest.hpp \
src/dbxml/dataItem/DbXmlUri.cpp \
src/dbxml/dataItem/DbXmlURIResolver.cpp \
src/dbxml/dataItem/DbXmlURIResolver.hpp \
src/dbxml/dataItem/DbXmlUri.hpp \
src/dbxml/dataItem/LookupIndexFunction.cpp \
src/dbxml/dataItem/LookupIndexFunction.hpp \
src/dbxml/dataItem/DbXmlUpdateFactory.hpp \
src/dbxml/dataItem/MetaDataFunction.cpp \
src/dbxml/dataItem/MetaDataFunction.hpp \
src/dbxml/dataItem/DbXmlNodeImpl.hpp \
src/dbxml/dataItem/DbXmlDocumentCacheImpl.hpp \
src/dbxml/dataItem/NodeHandleFunction.hpp \
src/dbxml/dataItem/NodeHandleFunction.cpp \
src/dbxml/dataItem/DbXmlDocumentCacheImpl.cpp \
src/dbxml/dataItem/DbXmlConfiguration.cpp \
src/dbxml/dataItem/DbXmlConfiguration.hpp \
src/dbxml/dataItem/DbXmlUserData.hpp \
src/dbxml/dataItem/DbXmlPrintAST.cpp \
src/dbxml/dataItem/DbXmlPrintAST.hpp \
src/dbxml/nodeStore/EventReaderToWriter.cpp \
src/dbxml/nodeStore/EventReader.cpp \
src/dbxml/nodeStore/EventReader.hpp \
src/dbxml/nodeStore/NsConstants.hpp \
src/dbxml/nodeStore/EventReaderToWriter.hpp \
src/dbxml/nodeStore/EventWriter.hpp \
src/dbxml/nodeStore/NsDocumentDatabase.cpp \
src/dbxml/nodeStore/NsDocument.cpp \
src/dbxml/nodeStore/NsDocument.hpp \
src/dbxml/nodeStore/NsDocumentDatabase.hpp \
src/dbxml/nodeStore/NsDom.cpp \
src/dbxml/nodeStore/NsDom.hpp \
src/dbxml/nodeStore/NsUpdate.cpp \
src/dbxml/nodeStore/NsEvent.cpp \
src/dbxml/nodeStore/NsEvent.hpp \
src/dbxml/nodeStore/NsEventReader.cpp \
src/dbxml/nodeStore/NsEventReader.hpp \
src/dbxml/nodeStore/NsEventWriter.cpp \
src/dbxml/nodeStore/NsEventWriter.hpp \
src/dbxml/nodeStore/NsFormat.cpp \
src/dbxml/nodeStore/NsFormat.hpp \
src/dbxml/nodeStore/NsHandlerBase.cpp \
src/dbxml/nodeStore/NsHandlerBase.hpp \
src/dbxml/nodeStore/NsNid.cpp \
src/dbxml/nodeStore/NsNid.hpp \
src/dbxml/nodeStore/NsNode.cpp \
src/dbxml/nodeStore/NsNode.hpp \
src/dbxml/nodeStore/NsSAX2Reader.cpp \
src/dbxml/nodeStore/NsSAX2Reader.hpp \
src/dbxml/nodeStore/NsUpdate.hpp \
src/dbxml/nodeStore/NsTypes.hpp \
src/dbxml/nodeStore/NsUpgrade.cpp \
src/dbxml/nodeStore/NsUpgrade.hpp \
src/dbxml/nodeStore/NsUtil.cpp \
src/dbxml/nodeStore/NsUtil.hpp \
src/dbxml/nodeStore/NsWriter.cpp \
src/dbxml/nodeStore/NsWriter.hpp \
src/dbxml/nodeStore/NsRawNode.cpp \
src/dbxml/nodeStore/NsDoc.cpp \
src/dbxml/nodeStore/NsXercesIndexer.cpp \
src/dbxml/nodeStore/NsXercesIndexer.hpp \
src/dbxml/nodeStore/NsRawNode.hpp \
src/dbxml/nodeStore/NsXercesTranscoder.hpp \
src/dbxml/nodeStore/PullEventInputStream.cpp \
src/dbxml/nodeStore/PullEventInputStream.hpp \
src/dbxml/nodeStore/NsDoc.hpp \
src/dbxml/nodeStore/NsXercesTranscoder.cpp \
src/dbxml/nodeStore/NsImpliedSchemaFilter.hpp \
src/dbxml/nodeStore/NsImpliedSchemaFilter.cpp \
src/dbxml/nodeStore/NsReindexer.cpp \
src/dbxml/nodeStore/NsReindexer.hpp \
src/dbxml/optimizer/ASTReplaceOptimizer.cpp \
src/dbxml/optimizer/ASTReplaceOptimizer.hpp \
src/dbxml/optimizer/QueryPlanGenerator.hpp \
src/dbxml/optimizer/NodeVisitingOptimizer.hpp \
src/dbxml/optimizer/QueryPlanGenerator.cpp \
src/dbxml/optimizer/QueryPlanOptimizer.cpp \
src/dbxml/optimizer/QueryPlanOptimizer.hpp \
src/dbxml/optimizer/ImpliedSchemaGenerator.hpp \
src/dbxml/optimizer/ImpliedSchemaGenerator.cpp \
src/dbxml/optimizer/DbXmlStaticTyper.hpp \
src/dbxml/optimizer/DbXmlStaticTyper.cpp \
src/dbxml/IndexEntry.cpp \
src/dbxml/QueryContext.cpp \
src/dbxml/Key.cpp \
src/dbxml/query/UnionQP.hpp \
src/dbxml/query/DescendantJoinQP.hpp \
src/dbxml/query/NodeIterator.hpp \
src/dbxml/query/NodeIterator.cpp \
src/dbxml/query/UnionQP.cpp \
src/dbxml/query/IntersectQP.hpp \
src/dbxml/query/IntersectQP.cpp \
src/dbxml/query/ParentOfChildJoinQP.hpp \
src/dbxml/query/DescendantJoinQP.cpp \
src/dbxml/query/EmptyQP.hpp \
src/dbxml/query/EmptyQP.cpp \
src/dbxml/query/StructuralJoinQP.hpp \
src/dbxml/query/StructuralJoinQP.cpp \
src/dbxml/query/ChildJoinQP.cpp \
src/dbxml/query/ChildJoinQP.hpp \
src/dbxml/query/NodeInfo.hpp \
src/dbxml/query/NodeInfo.cpp \
src/dbxml/query/AttributeJoinQP.hpp \
src/dbxml/query/AttributeOrChildJoinQP.hpp \
src/dbxml/query/AttributeOrChildJoinQP.cpp \
src/dbxml/query/AttributeJoinQP.cpp \
src/dbxml/query/AncestorJoinQP.hpp \
src/dbxml/query/ParentOfAttributeJoinQP.hpp \
src/dbxml/query/ParentOfAttributeJoinQP.cpp \
src/dbxml/query/AncestorJoinQP.cpp \
src/dbxml/query/ImpliedSchemaNode.cpp \
src/dbxml/query/ParentOfChildJoinQP.cpp \
src/dbxml/query/ParentJoinQP.hpp \
src/dbxml/query/ParentJoinQP.cpp \
src/dbxml/query/QueryPlan.hpp \
src/dbxml/query/QueryPlan.cpp \
src/dbxml/query/ImpliedSchemaNode.hpp \
src/dbxml/query/QueryExecutionContext.cpp \
src/dbxml/query/QueryExecutionContext.hpp \
src/dbxml/query/QueryPlanHolder.cpp \
src/dbxml/query/QueryPlanHolder.hpp \
src/dbxml/query/SequentialScanQP.hpp \
src/dbxml/query/SequentialScanQP.cpp \
src/dbxml/query/ValueFilterQP.hpp \
src/dbxml/query/ValueFilterQP.cpp \
src/dbxml/query/ASTToQueryPlan.hpp \
src/dbxml/query/ASTToQueryPlan.cpp \
src/dbxml/query/QueryPlanToAST.hpp \
src/dbxml/query/QueryPlanToAST.cpp \
src/dbxml/query/FilterQP.hpp \
src/dbxml/query/FilterQP.cpp \
src/dbxml/query/PredicateFilterQP.hpp \
src/dbxml/query/PredicateFilterQP.cpp \
src/dbxml/query/DecisionPointQP.hpp \
src/dbxml/query/DecisionPointQP.cpp \
src/dbxml/query/ExceptQP.cpp \
src/dbxml/query/ExceptQP.hpp \
src/dbxml/query/ContextNodeQP.hpp \
src/dbxml/query/ContextNodeQP.cpp \
src/dbxml/query/CollectionQP.hpp \
src/dbxml/query/CollectionQP.cpp \
src/dbxml/query/DocQP.hpp \
src/dbxml/query/DocQP.cpp \
src/dbxml/query/StepQP.hpp \
src/dbxml/query/StepQP.cpp \
src/dbxml/query/LevelFilterQP.cpp \
src/dbxml/query/BufferQP.hpp \
src/dbxml/query/VariableQP.hpp \
src/dbxml/query/VariableQP.cpp \
src/dbxml/query/BufferQP.cpp \
src/dbxml/query/LevelFilterQP.hpp \
src/dbxml/query/NodePredicateFilterQP.hpp \
src/dbxml/query/NodePredicateFilterQP.cpp \
src/dbxml/AtomicNodeValue.cpp \
src/dbxml/AtomicNodeValue.hpp \
src/dbxml/BaseInputStream.cpp \
src/dbxml/BaseInputStream.hpp \
src/dbxml/Buffer.cpp \
src/dbxml/Buffer.hpp \
src/dbxml/BulkPut.cpp \
src/dbxml/BulkPut.hpp \
src/dbxml/ConfigurationDatabase.cpp \
src/dbxml/ConfigurationDatabase.hpp \
src/dbxml/Container.cpp \
src/dbxml/Container.hpp \
src/dbxml/Cursor.cpp \
src/dbxml/KeyGenerator.cpp \
src/dbxml/Cursor.hpp \
src/dbxml/Value.cpp \
src/dbxml/DatabaseNodeValue.cpp \
src/dbxml/DatabaseNodeValue.hpp \
src/dbxml/DbWrapper.cpp \
src/dbxml/DbWrapper.hpp \
src/dbxml/DbXmlXerces.hpp \
src/dbxml/DbXmlInternal.cpp \
src/dbxml/DbXmlInternal.hpp \
src/dbxml/DictionaryDatabase.cpp \
src/dbxml/DictionaryDatabase.hpp \
src/dbxml/DocID.cpp \
src/dbxml/DocID.hpp \
src/dbxml/Document.cpp \
src/dbxml/Document.hpp \
src/dbxml/DocumentDatabase.cpp \
src/dbxml/DocumentDatabase.hpp \
src/dbxml/Globals.cpp \
src/dbxml/Globals.hpp \
src/dbxml/HighResTimer.cpp \
src/dbxml/HighResTimer.hpp \
src/dbxml/KeyGenerator.hpp \
src/dbxml/IndexEntry.hpp \
src/dbxml/IndexLookup.cpp \
src/dbxml/IndexLookup.hpp \
src/dbxml/IndexSpecification.cpp \
src/dbxml/IndexSpecification.hpp \
src/dbxml/Indexer.cpp \
src/dbxml/Indexer.hpp \
src/dbxml/Key.hpp \
src/dbxml/KeyStatistics.cpp \
src/dbxml/KeyStash.cpp \
src/dbxml/KeyStash.hpp \
src/dbxml/LocalFileInputStream.cpp \
src/dbxml/KeyStatistics.hpp \
src/dbxml/Manager.cpp \
src/dbxml/LocalFileInputStream.hpp \
src/dbxml/Log.hpp \
src/dbxml/MetaDatum.cpp \
src/dbxml/Manager.hpp \
src/dbxml/NameID.cpp \
src/dbxml/Name.cpp \
src/dbxml/MemBufInputStream.cpp \
src/dbxml/MemBufInputStream.hpp \
src/dbxml/MetaDatum.hpp \
src/dbxml/Results.cpp \
src/dbxml/ResultsEventWriter.cpp \
src/dbxml/ResultsEventWriter.hpp \
src/dbxml/Name.hpp \
src/dbxml/NamespaceMap.hpp \
src/dbxml/NameID.hpp \
src/dbxml/OperationContext.hpp \
src/dbxml/StructuralStatsDatabase.hpp \
src/dbxml/QueryContext.hpp \
src/dbxml/Value.hpp \
src/dbxml/UTF8.cpp \
src/dbxml/QueryExpression.cpp \
src/dbxml/QueryExpression.hpp \
src/dbxml/ReferenceCounted.cpp \
src/dbxml/ReferenceCounted.hpp \
src/dbxml/ReferenceMinder.cpp \
src/dbxml/ReferenceMinder.hpp \
src/dbxml/Results.hpp \
src/dbxml/LogDbXml.cpp \
src/dbxml/ScopedDbt.hpp \
src/dbxml/ScopedPtr.hpp \
src/dbxml/SharedPtr.hpp \
src/dbxml/Statistics.cpp \
src/dbxml/Statistics.hpp \
src/dbxml/StdInInputStream.cpp \
src/dbxml/StdInInputStream.hpp \
src/dbxml/Syntax.cpp \
src/dbxml/Syntax.hpp \
src/dbxml/SyntaxDatabase.cpp \
src/dbxml/SyntaxDatabase.hpp \
src/dbxml/SyntaxManager.cpp \
src/dbxml/SyntaxManager.hpp \
src/dbxml/CacheDatabaseMinder.cpp \
src/dbxml/Transaction.cpp \
src/dbxml/Transaction.hpp \
src/dbxml/TypeConversions.hpp \
src/dbxml/URLInputStream.cpp \
src/dbxml/URLInputStream.hpp \
src/dbxml/UTF8.hpp \
src/dbxml/XmlEventReaderToWriter.cpp \
src/dbxml/UpdateContext.cpp \
src/dbxml/UpdateContext.hpp \
src/dbxml/VariableBindings.cpp \
src/dbxml/VariableBindings.hpp \
src/dbxml/XmlCompression.cpp \
src/dbxml/XmlContainer.cpp \
src/dbxml/XmlContainerConfig.cpp \
src/dbxml/XmlData.cpp \
src/dbxml/XmlDocument.cpp \
src/dbxml/XmlEventReader.cpp \
src/dbxml/XmlIndexSpecification.cpp \
src/dbxml/XmlEventWriter.cpp \
src/dbxml/XmlException.cpp \
src/dbxml/XmlIndexLookup.cpp \
src/dbxml/XmlInputStreamWrapper.hpp \
src/dbxml/XmlInputStream.cpp \
src/dbxml/XmlMetaDataIterator.cpp \
src/dbxml/XmlManager.cpp \
src/dbxml/XmlQueryContext.cpp \
src/dbxml/XmlNamespace.cpp \
src/dbxml/DictionaryCache.cpp \
src/dbxml/XmlQueryExpression.cpp \
src/dbxml/XmlResolver.cpp \
src/dbxml/XmlResults.cpp \
src/dbxml/XmlStatistics.cpp \
src/dbxml/XmlTransaction.cpp \
src/dbxml/XmlUpdateContext.cpp \
src/dbxml/XmlValue.cpp \
src/dbxml/db_rdbt.c \
src/dbxml/db_rdbt.h \
src/dbxml/db_utils.h \
src/dbxml/tokenizer.hpp \
src/dbxml/DictionaryCache.hpp \
src/dbxml/CacheDatabase.cpp \
src/dbxml/CacheDatabase.hpp \
src/dbxml/MutexLock.hpp \
src/dbxml/CacheDatabaseMinder.hpp \
src/dbxml/Counters.cpp \
src/dbxml/Counters.hpp \
src/dbxml/MutexLock.cpp \
src/dbxml/StructuralStatsDatabase.cpp \
src/dbxml/ContainerBase.cpp \
src/dbxml/ContainerBase.hpp \
src/dbxml/RawNodeValue.cpp \
src/dbxml/RawNodeValue.hpp \
src/dbxml/ContainerConfig.cpp \
src/dbxml/ContainerConfig.hpp \
src/dbxml/debug/DbXmlDebugHookDecorator.hpp \
src/dbxml/debug/DbXmlDebugHookDecorator.cpp \
src/dbxml/debug/QPDebugHook.hpp \
src/dbxml/debug/QPDebugHook.cpp \
src/dbxml/debug/DbXmlASTDebugHook.hpp \
src/dbxml/debug/DbXmlASTDebugHook.cpp \
src/dbxml/debug/DbXmlTupleDebugHook.hpp \
src/dbxml/debug/DbXmlTupleDebugHook.cpp

if USE_COMPRESSION
libdbxml_la_SOURCES += src/dbxml/CompressionZlib.cpp \
src/dbxml/CompressionZlib.hpp
CPPFLAGS += -D_DBXML_COMPRESSION
endif

#############################################################
# $(CPPFLAGS) has been added before the Tcl and Java include
# flags to ensure that the proper db.h header is found in the
# event one exists in the same directory as tcl.h or jni.h
#
# Do not use -module on Tcl -- it causes dynamic load to fail on OS X
# TBD: look into using tclConfig.sh TCL_SHLIB_LD variable to drive this
#############################################################

libdbxml_tcl_la_CPPFLAGS = $(CPPFLAGS) $(TCL_INCLUDE_SPEC)
libdbxml_tcl_la_LDFLAGS = -release $(DBXML_VERSION_MAJOR).$(DBXML_VERSION_MINOR) $(TCL_LIBRARY_EXTENSION) $(LIBTSO_LIBS)
libdbxml_tcl_la_SOURCES = src/tcl/dbxml_tcl_wrap.cpp
libdbxml_tcl_la_LIBADD = libdbxml.la

libdbxml_java_la_CPPFLAGS = $(CPPFLAGS) $(SWIGCPPFLAGS) $(JAVA_INCLUDE_SPEC)
libdbxml_java_la_LDFLAGS = -release $(DBXML_VERSION_MAJOR).$(DBXML_VERSION_MINOR) -module $(JAVA_LIBRARY_EXTENSION) $(LIBJSO_LIBS)
libdbxml_java_la_SOURCES = src/java/dbxml_java_wrap.cpp src/java/dbxml_java_wrap.h
libdbxml_java_la_LIBADD = libdbxml.la

COMMON_SOURCES = \
src/common/Timer.cpp \
src/common/common_utils.h \
src/common/Timer.hpp \
src/common/dbxml_util_sig.c \
src/common/SigBlock.cpp

dbxml_SOURCES = \
src/utils/shell/AddAliasCommand.cpp \
src/utils/shell/AbortCommand.cpp \
src/utils/shell/AbortCommand.hpp \
src/utils/shell/AddAliasCommand.hpp \
src/utils/shell/ContainerQueryCommand.cpp \
src/utils/shell/AddDocumentCommand.cpp \
src/utils/shell/AddDocumentCommand.hpp \
src/utils/shell/AddIndexCommand.cpp \
src/utils/shell/AddIndexCommand.hpp \
src/utils/shell/CommentCommand.cpp \
src/utils/shell/CommentCommand.hpp \
src/utils/shell/CommitCommand.cpp \
src/utils/shell/CommitCommand.hpp \
src/utils/shell/PrintCommand.cpp \
src/utils/shell/ContainerQueryCommand.hpp \
src/utils/shell/ContextItemQueryCommand.cpp \
src/utils/shell/ContextItemQueryCommand.hpp \
src/utils/shell/CreateContainerCommand.cpp \
src/utils/shell/CreateContainerCommand.hpp \
src/utils/shell/DebugCommand.hpp \
src/utils/shell/DebugOptimizationCommand.hpp \
src/utils/shell/DefaultShell.cpp \
src/utils/shell/DefaultShell.hpp \
src/utils/shell/DeleteDocumentCommand.cpp \
src/utils/shell/DeleteDocumentCommand.hpp \
src/utils/shell/DeleteIndexCommand.cpp \
src/utils/shell/DeleteIndexCommand.hpp \
src/utils/shell/EchoCommand.cpp \
src/utils/shell/EchoCommand.hpp \
src/utils/shell/EdgeIndexLookupCommand.cpp \
src/utils/shell/EdgeIndexLookupCommand.hpp \
src/utils/shell/Environment.cpp \
src/utils/shell/Environment.hpp \
src/utils/shell/GetDocumentsCommand.cpp \
src/utils/shell/GetDocumentsCommand.hpp \
src/utils/shell/GetMetaDataCommand.cpp \
src/utils/shell/GetMetaDataCommand.hpp \
src/utils/shell/HelpCommand.cpp \
src/utils/shell/HelpCommand.hpp \
src/utils/shell/IncludeCommand.cpp \
src/utils/shell/IncludeCommand.hpp \
src/utils/shell/IndexLookupCommand.cpp \
src/utils/shell/IndexLookupCommand.hpp \
src/utils/shell/InfoCommand.cpp \
src/utils/shell/InfoCommand.hpp \
src/utils/shell/LazyCommand.cpp \
src/utils/shell/LazyCommand.hpp \
src/utils/shell/ListIndexesCommand.cpp \
src/utils/shell/ListIndexesCommand.hpp \
src/utils/shell/RemoveContainerCommand.cpp \
src/utils/shell/OpenContainerCommand.cpp \
src/utils/shell/OpenContainerCommand.hpp \
src/utils/shell/PreLoadCommand.cpp \
src/utils/shell/PreLoadCommand.hpp \
src/utils/shell/PrintCommand.hpp \
src/utils/shell/QueryCommand.cpp \
src/utils/shell/QueryCommand.hpp \
src/utils/shell/QueryPlanCommand.cpp \
src/utils/shell/QueryPlanCommand.hpp \
src/utils/shell/QuitCommand.cpp \
src/utils/shell/QuitCommand.hpp \
src/utils/shell/ReindexCommand.cpp \
src/utils/shell/ReindexCommand.hpp \
src/utils/shell/RemoveAliasCommand.cpp \
src/utils/shell/RemoveAliasCommand.hpp \
src/utils/shell/DebugCommand.cpp \
src/utils/shell/DebugOptimizationCommand.cpp \
src/utils/shell/RemoveContainerCommand.hpp \
src/utils/shell/ReturnTypeCommand.cpp \
src/utils/shell/ReturnTypeCommand.hpp \
src/utils/shell/SetAutoIndexingCommand.cpp \
src/utils/shell/SetAutoIndexingCommand.hpp \
src/utils/shell/SetBaseURICommand.cpp \
src/utils/shell/SetBaseURICommand.hpp \
src/utils/shell/SetMetaDataCommand.cpp \
src/utils/shell/SetMetaDataCommand.hpp \
src/utils/shell/SetNamespaceCommand.cpp \
src/utils/shell/SetNamespaceCommand.hpp \
src/utils/shell/SetQueryTimeoutCommand.cpp \
src/utils/shell/SetQueryTimeoutCommand.hpp \
src/utils/shell/SetTypedVariableCommand.cpp \
src/utils/shell/SetTypedVariableCommand.hpp \
src/utils/shell/SetVariableCommand.cpp \
src/utils/shell/SetVariableCommand.hpp \
src/utils/shell/SetVerboseCommand.cpp \
src/utils/shell/SetVerboseCommand.hpp \
src/utils/shell/Shell.cpp \
src/utils/shell/Shell.hpp \
src/utils/shell/ShellCommand.cpp \
src/utils/shell/ShellCommand.hpp \
src/utils/shell/StartTransactionCommand.cpp \
src/utils/shell/StartTransactionCommand.hpp \
src/utils/shell/StatsLookupCommand.cpp \
src/utils/shell/StatsLookupCommand.hpp \
src/utils/shell/SyncCommand.cpp \
src/utils/shell/SyncCommand.hpp \
src/utils/shell/TimeCommand.cpp \
src/utils/shell/TimeCommand.hpp \
src/utils/shell/UpgradeCommand.cpp \
src/utils/shell/UpgradeCommand.hpp \
src/utils/shell/dbxmlsh.cpp \
src/utils/shell/SetIgnoreCommand.cpp \
src/utils/shell/SetIgnoreCommand.hpp \
src/utils/shell/CompactCommand.cpp \
src/utils/shell/CompactCommand.hpp \
src/utils/shell/ProjectionCommand.hpp \
src/utils/shell/ProjectionCommand.cpp \
src/utils/shell/PrepareCommand.hpp \
src/utils/shell/PrepareCommand.cpp \
$(COMMON_SOURCES)

dbxml_dump_SOURCES = \
src/dbxml_dump/dbxml_dump.cpp \
$(COMMON_SOURCES)

dbxml_load_SOURCES = \
src/dbxml_load/dbxml_load.cpp \
$(COMMON_SOURCES)

dbxml_load_container_SOURCES = \
src/utils/load_container/LoadContainer.cpp \
src/utils/load_container/LoadContainer.hpp \
$(COMMON_SOURCES)

dbxml_benchmark_SOURCES = test/benchmark/src/Benchmark.cpp $(COMMON_SOURCES)

dbxml_perf_SOURCES = \
test/perf/perf_test.cpp \
test/perf/perf_workload.cpp \
test/perf/perf_workload.hpp \
test/perf/perf_work.c \
test/perf/perf_work.h \
test/perf/perf_os.c \
test/perf/perf_os.h \
test/perf/PutAndQueriesWorkload.cpp \
test/perf/PutAndQueriesWorkload.hpp \
test/perf/PutWorkload.cpp \
test/perf/PutWorkload.hpp \
test/perf/DbXmlWorkload.cpp \
test/perf/DbXmlWorkload.hpp \
test/perf/QueryWorkload.cpp \
test/perf/QueryWorkload.hpp \
test/perf/testcases/PutDocumentWorkload.hpp \
test/perf/testcases/PutDocumentWorkload.cpp \
test/perf/testcases/GetDocumentWorkload.hpp \
test/perf/testcases/GetDocumentWorkload.cpp \
test/perf/testcases/DeleteDocumentWorkload.hpp \
test/perf/testcases/DeleteDocumentWorkload.cpp \
test/perf/testcases/QueryPresenceWorkload.cpp \
test/perf/testcases/QueryPresenceWorkload.hpp \
test/perf/testcases/QueryEqualityWorkload.cpp \
test/perf/testcases/QueryEqualityWorkload.hpp \
test/perf/testcases/QueryPrepareWorkload.cpp \
test/perf/testcases/QueryPrepareWorkload.hpp \
test/perf/testcases/UpdateDocumentWorkload.cpp \
test/perf/testcases/UpdateDocumentWorkload.hpp \
test/perf/testcases/EventReaderWorkload.cpp \
test/perf/testcases/EventReaderWorkload.hpp \
test/perf/testcases/EventWriterWorkload.cpp \
test/perf/testcases/EventWriterWorkload.hpp \
test/perf/testcases/IterateNodeWorkload.cpp \
test/perf/testcases/IterateNodeWorkload.hpp \
test/perf/testcases/ReplaceNodeWorkload.cpp \
test/perf/testcases/ReplaceNodeWorkload.hpp \
test/perf/testcases/InsertNodeWorkload.cpp \
test/perf/testcases/InsertNodeWorkload.hpp \
test/perf/testcases/DeleteNodeWorkload.cpp \
test/perf/testcases/DeleteNodeWorkload.hpp \
test/perf/testcases/RenameNodeWorkload.cpp \
test/perf/testcases/RenameNodeWorkload.hpp \
test/perf/testcases/AddIndexWorkload.cpp \
test/perf/testcases/AddIndexWorkload.hpp \
test/perf/testcases/ConfigIndexWorkload.cpp \
test/perf/testcases/ConfigIndexWorkload.hpp \
test/perf/testcases/IndexLookupWorkload.cpp \
test/perf/testcases/IndexLookupWorkload.hpp \
test/perf/testcases/MetaDataWorkload.cpp \
test/perf/testcases/MetaDataWorkload.hpp \
test/perf/testcases/DictionaryDatabaseWorkload.cpp \
test/perf/testcases/DictionaryDatabaseWorkload.hpp 

dbxml_disk_SOURCES = test/disk/diskUtility.cpp

helloWorld_SOURCES = examples/cxx/basic/helloWorld.cpp
helloWorldTxn_SOURCES = examples/cxx/basic/helloWorldTxn.cpp
query_SOURCES = examples/cxx/basic/query.cpp
queryTxn_SOURCES = examples/cxx/basic/queryTxn.cpp
deleteDocument_SOURCES = examples/cxx/gettingStarted/deleteDocument.cpp
addIndex_SOURCES = examples/cxx/gettingStarted/addIndex.cpp
buildDB_SOURCES = examples/cxx/gettingStarted/buildDB.cpp examples/cxx/gettingStarted/myDb.hpp
exampleLoadContainer_SOURCES = examples/cxx/gettingStarted/exampleLoadContainer.cpp
deleteIndex_SOURCES = examples/cxx/gettingStarted/deleteIndex.cpp
queryForDocumentValue_SOURCES = examples/cxx/gettingStarted/queryForDocumentValue.cpp
queryWithDocumentNames_SOURCES = examples/cxx/gettingStarted/queryWithDocumentNames.cpp
queryForMetaData_SOURCES = examples/cxx/gettingStarted/queryForMetaData.cpp
queryWithContext_SOURCES = examples/cxx/gettingStarted/queryWithContext.cpp
simpleContainer_SOURCES = examples/cxx/gettingStarted/simpleContainer.cpp
replaceIndex_SOURCES = examples/cxx/gettingStarted/replaceIndex.cpp
retrieveDB_SOURCES = examples/cxx/gettingStarted/retrieveDB.cpp examples/cxx/gettingStarted/myDb.hpp
simpleAdd_SOURCES = examples/cxx/gettingStarted/simpleAdd.cpp
simpleContainerInEnv_SOURCES = examples/cxx/gettingStarted/simpleContainerInEnv.cpp
simpleQuery_SOURCES = examples/cxx/gettingStarted/simpleQuery.cpp
updateDocument_SOURCES = examples/cxx/gettingStarted/updateDocument.cpp
xercesDomTranslator_SOURCES = examples/cxx/xerces/xercesDomTranslator.cpp examples/cxx/xerces/xercesDomTranslator.hpp
externalFunction_SOURCES = examples/cxx/misc/externalFunction.cpp
compression_SOURCES = examples/cxx/misc/compression.cpp
debug_SOURCES = examples/cxx/misc/debug.cpp

UNITTEST_UTILS_SOURCES = \
test/cpp/util/TestLogging.cpp \
test/cpp/util/TestLogging.hpp \
test/cpp/util/TestResolver.cpp \
test/cpp/util/TestResolver.hpp \
test/cpp/util/Transcoding.cpp \
test/cpp/util/Transcoding.hpp

dbxml_test_driver_SOURCES = \
test/cpp/unitTests/UnitTest.cpp \
test/cpp/unitTests/QueryPlanTest.cpp \
test/cpp/unitTests/QueryPlanTest.hpp \
test/cpp/unitTests/ResolverUnitTest.cpp \
test/cpp/unitTests/ResolverUnitTest.hpp \
test/cpp/unitTests/TestEnvironment.cpp \
test/cpp/unitTests/TestEnvironment.hpp \
test/cpp/unitTests/UnitTest.hpp \
test/cpp/unitTests/XQueryTest.cpp \
test/cpp/unitTests/XQueryTest.hpp \
test/cpp/unitTests/unit_test_driver.cpp \
$(UNITTEST_UTILS_SOURCES)

XmlUnitFramework = \
test/cpp/xmlUnit/XmlUnit.cpp \
test/cpp/xmlUnit/XmlUnit.hpp

dbxml_unit_runner_SOURCES =	\
	test/cpp/xmlUnit/XmlUnitRunner.cpp \
	$(XmlUnitFramework)

dbxml_unit_runner_CXXFLAGS=-I../test/cpp/xmlUnit

dbxml_test_databases_SOURCES = test/cpp/databaseManagement/databaseManagement.cpp $(UNITTEST_UTILS_SOURCES)
dbxml_test_query_processor_api_SOURCES = test/cpp/queryProcessorAPI/queryProcessorAPI.cpp $(UNITTEST_UTILS_SOURCES)
dbxml_test_input_streams_SOURCES = test/cpp/inputStreams/inputStreams.cpp $(UNITTEST_UTILS_SOURCES)
dbxml_xqts_runner_SOURCES = test/cpp/xqts/xqts_runner.cpp $(UNITTEST_UTILS_SOURCES)
dbxml_test_ha_SOURCES = \
test/cpp/ha/ha_test.cpp \
test/cpp/ha/NormalTestCase.cpp \
test/cpp/ha/NormalTestCase.hpp \
test/cpp/ha/RepNode.cpp \
test/cpp/ha/RepNode.hpp \
test/cpp/ha/SimpleTestCase.cpp \
test/cpp/ha/SimpleTestCase.hpp


##################################################
# Installed C++ header files
##################################################

include_dbxmldir = $(includedir)/dbxml
include_dbxml_HEADERS = \
include/dbxml/XmlCompression.hpp \
include/dbxml/XmlContainerConfig.hpp \
include/dbxml/XmlContainer.hpp \
include/dbxml/DbXml.hpp \
include/dbxml/DbXmlFwd.hpp \
include/dbxml/XmlDocument.hpp \
include/dbxml/XmlData.hpp \
include/dbxml/XmlEventReaderToWriter.hpp \
include/dbxml/XmlEventReader.hpp \
include/dbxml/XmlIndexSpecification.hpp \
include/dbxml/XmlEventWriter.hpp \
include/dbxml/XmlException.hpp \
include/dbxml/XmlIndexLookup.hpp \
include/dbxml/XmlMetaDataIterator.hpp \
include/dbxml/XmlInputStream.hpp \
include/dbxml/XmlManager.hpp \
include/dbxml/XmlPortability.hpp \
include/dbxml/XmlNamespace.hpp \
include/dbxml/XmlQueryExpression.hpp \
include/dbxml/XmlQueryContext.hpp \
include/dbxml/XmlStatistics.hpp \
include/dbxml/XmlResolver.hpp \
include/dbxml/XmlExternalFunction.hpp \
include/dbxml/XmlDebugListener.hpp \
include/dbxml/XmlResults.hpp \
include/dbxml/XmlTransaction.hpp \
include/dbxml/XmlUpdateContext.hpp \
include/dbxml/XmlValue.hpp

##################################################
# Extra files for the distribution
##################################################

EXTRA_DIST = \
docs/

##################################################
# Java sources
##################################################

JAVA_CLASSTOP=	./classes
JAVA_SRCDIR=	$(top_srcdir)/src/java
JAVA_DBXMLREL=	com/sleepycat/dbxml
JAVA_DBREL=	com/sleepycat/db
JAVA_EX_GSG=	dbxml/gettingStarted
JAVA_EX_BASIC=	basic
JAVA_EX_EVENT=	event
JAVA_EX_MISC=	misc
JAVA_EX_ADVANCED=
JAVA_EXREL=	$(JAVA_EX_GSG) $(JAVA_EX_BASIC) $(JAVA_EX_EVENT) $(JAVA_EX_MISC) $(JAVA_EX_ADVANCED) 
JAVA_PERFREL=	com/sleepycat/dbxml/perftest
JAVA_DBXMLDIR=	$(JAVA_SRCDIR)/$(JAVA_DBXMLREL)
JAVA_DBDIR=	$(JAVA_SRCDIR)/$(JAVA_DBREL)
JAVA_EX_GSG_DIR= $(srcdir)/examples/java/gettingStarted
JAVA_EX_BASIC_DIR= $(srcdir)/examples/java/basic
JAVA_EX_EVENT_DIR= $(srcdir)/examples/java/event
JAVA_EX_MISC_DIR= $(srcdir)/examples/java/misc
JAVA_PERFDIR=	$(srcdir)/src/dbxml_perf
JAVA_TESTROOT=	$(srcdir)/test/java
JAVA_TESTREL=	dbxmltest
JAVA_TESTDIR=	$(JAVA_TESTROOT)
JAVAC_TESTFLAGS= 

CLASSPATH=	$(JAVA_CLASSTOP):$(berkeleydb_jar):$(JUNIT_JAR)

JAVA_DBXMLSRCS=\
	$(JAVA_DBXMLDIR)/XmlCompression.java \
	$(JAVA_DBXMLDIR)/XmlConstants.java \
	$(JAVA_DBXMLDIR)/XmlQueryExpression.java \
	$(JAVA_DBXMLDIR)/XmlContainer.java \
	$(JAVA_DBXMLDIR)/XmlData.java \
	$(JAVA_DBXMLDIR)/XmlDocument.java \
	$(JAVA_DBXMLDIR)/XmlException.java \
	$(JAVA_DBXMLDIR)/XmlEventReader.java \
	$(JAVA_DBXMLDIR)/XmlEventReaderToWriter.java \
	$(JAVA_DBXMLDIR)/XmlEventWriter.java \
	$(JAVA_DBXMLDIR)/XmlIndexDeclaration.java \
	$(JAVA_DBXMLDIR)/XmlIndexSpecification.java \
	$(JAVA_DBXMLDIR)/XmlQueryContext.java \
	$(JAVA_DBXMLDIR)/XmlResults.java \
	$(JAVA_DBXMLDIR)/XmlUpdateContext.java \
	$(JAVA_DBXMLDIR)/XmlValue.java \
	$(JAVA_DBXMLDIR)/XmlManager.java \
	$(JAVA_DBXMLDIR)/XmlTransaction.java \
	$(JAVA_DBXMLDIR)/XmlInputStream.java \
	$(JAVA_DBXMLDIR)/XmlIndexLookup.java \
	$(JAVA_DBXMLDIR)/XmlStatistics.java \
	$(JAVA_DBXMLDIR)/XmlMetaDataIterator.java \
	$(JAVA_DBXMLDIR)/XmlMetaData.java \
	$(JAVA_DBXMLDIR)/XmlResolver.java \
	$(JAVA_DBXMLDIR)/XmlManagerConfig.java \
	$(JAVA_DBXMLDIR)/XmlContainerConfig.java \
	$(JAVA_DBXMLDIR)/XmlDocumentConfig.java \
	$(JAVA_DBXMLDIR)/XmlExternalFunctionBase.java \
	$(JAVA_DBXMLDIR)/XmlExternalFunction.java \
	$(JAVA_DBXMLDIR)/XmlArguments.java \
	$(JAVA_DBXMLDIR)/XmlDebugListener.java \
	$(JAVA_DBXMLDIR)/XmlStackFrame.java \
	$(JAVA_DBXMLDIR)/dbxml_java.java \
	$(JAVA_DBXMLDIR)/dbxml_javaConstants.java \
	$(JAVA_DBXMLDIR)/dbxml_javaJNI.java \
	$(JAVA_DBXMLDIR)/NativeXmlInputStream.java \
	$(JAVA_DBXMLDIR)/XmlInputStreamWrap.java \
	$(JAVA_DBXMLDIR)/HelperFunctions.java \
	$(JAVA_DBDIR)/XmlHelper.java

JAVA_EXSRCS= \
	$(JAVA_EX_GSG_DIR)/addIndex.java \
	$(JAVA_EX_GSG_DIR)/buildDB.java \
	$(JAVA_EX_GSG_DIR)/deleteDocument.java \
	$(JAVA_EX_GSG_DIR)/deleteIndex.java \
	$(JAVA_EX_GSG_DIR)/exampleLoadContainer.java \
	$(JAVA_EX_GSG_DIR)/mdConst.java \
	$(JAVA_EX_GSG_DIR)/myDb.java \
	$(JAVA_EX_GSG_DIR)/myDbEnv.java \
	$(JAVA_EX_GSG_DIR)/queryForDocumentValue.java \
	$(JAVA_EX_GSG_DIR)/queryForMetaData.java \
	$(JAVA_EX_GSG_DIR)/queryWithContext.java \
	$(JAVA_EX_GSG_DIR)/queryWithDocumentNames.java \
	$(JAVA_EX_GSG_DIR)/replaceIndex.java \
	$(JAVA_EX_GSG_DIR)/retrieveDB.java \
	$(JAVA_EX_GSG_DIR)/simpleAdd.java \
	$(JAVA_EX_GSG_DIR)/simpleContainerInEnv.java \
	$(JAVA_EX_GSG_DIR)/simpleQuery.java \
	$(JAVA_EX_GSG_DIR)/updateDocument.java \
	$(JAVA_EX_BASIC_DIR)/HelloWorld.java \
	$(JAVA_EX_BASIC_DIR)/HelloWorldTxn.java \
	$(JAVA_EX_BASIC_DIR)/Query.java \
	$(JAVA_EX_BASIC_DIR)/QueryTxn.java \
	$(JAVA_EX_EVENT_DIR)/EventReaderToWriter.java \
	$(JAVA_EX_EVENT_DIR)/UseEventReaderToWriter.java \
	$(JAVA_EX_MISC_DIR)/Compression.java \
	$(JAVA_EX_MISC_DIR)/ExternalFunction.java \
	$(JAVA_EX_MISC_DIR)/Debug.java \
	$(JAVA_EX_MISC_DIR)/Profiler.java

JAVA_PERFSRCS=\
	$(JAVA_PERFDIR)/PerfTest.java

JAVA_TESTSRCS=\
	$(JAVA_TESTDIR)/TestConfig.java \
	$(JAVA_TESTDIR)/miscellaneous/AutoOpenTest.java \
	$(JAVA_TESTDIR)/miscellaneous/AutoIndexTest.java \
	$(JAVA_TESTDIR)/miscellaneous/NodeHandleTest.java \
	$(JAVA_TESTDIR)/miscellaneous/UpdateNodeWithPrefixTest.java \
	$(JAVA_TESTDIR)/XmlManager/ManagerTest.java \
	$(JAVA_TESTDIR)/XmlManagerConfig/ManagerConfigTest.java \
	$(JAVA_TESTDIR)/XmlValue/ValueTest.java \
	$(JAVA_TESTDIR)/XmlResults/ResultsTest.java \
	$(JAVA_TESTDIR)/XmlDocument/DocumentTest.java \
	$(JAVA_TESTDIR)/XmlMetaData/MetaDataTest.java \
	$(JAVA_TESTDIR)/XmlMetaDataIterator/MetaDataIteratorTest.java \
	$(JAVA_TESTDIR)/XmlResolver/ResolverTest.java \
	$(JAVA_TESTDIR)/XmlResolver/TestResolver.java \
	$(JAVA_TESTDIR)/XmlEventReader/EventReaderTest.java \
	$(JAVA_TESTDIR)/XmlEventReaderToWriter/EventReaderToWriterTest.java \
	$(JAVA_TESTDIR)/XmlEventWriter/EventWriterTest.java \
	$(JAVA_TESTDIR)/XmlIndexSpecification/IndexSpecificationTest.java \
	$(JAVA_TESTDIR)/XmlInputStream/InputStreamTest.java \
	$(JAVA_TESTDIR)/XmlInputStream/InputStreamTest2.java \
	$(JAVA_TESTDIR)/XmlInputStream/TestInputStream.java \
	$(JAVA_TESTDIR)/XmlStatistics/StatisticsTest.java \
	$(JAVA_TESTDIR)/XmlIndexLookup/IndexLookupTest.java \
	$(JAVA_TESTDIR)/XmlException/ExceptionTest.java \
	$(JAVA_TESTDIR)/XmlQueryExpression/QueryExpressionTest.java \
	$(JAVA_TESTDIR)/XmlQueryContext/QueryContextTest.java \
	$(JAVA_TESTDIR)/XmlTransaction/TransactionTest.java \
	$(JAVA_TESTDIR)/XmlContainer/ContainerTest.java \
	$(JAVA_TESTDIR)/XmlContainerConfig/ContainerConfigTest.java \
	$(JAVA_TESTDIR)/XmlCompression/CompressionTest.java \
	$(JAVA_TESTDIR)/XmlCompression/TestCompression.java \
	$(JAVA_TESTDIR)/XmlData/DataTest.java \
	$(JAVA_TESTDIR)/XmlTestRunner.java 

##################################################
# Java compilation rules
##################################################

$(JAVA_CLASSTOP):
	test -d $(JAVA_CLASSTOP) || $(mkdir_p) $(JAVA_CLASSTOP)

$(libj_jarfile): $(JAVA_DBXMLSRCS) $(JAVA_CLASSTOP)
	$(JAVAC) -classpath $(CLASSPATH) -d $(JAVA_CLASSTOP) $(JAVACFLAGS) $(JAVA_DBXMLSRCS)
	cd $(JAVA_CLASSTOP) && $(JAR) cf ../$@ $(JAVA_DBXMLREL) $(JAVA_DBREL)

$(libj_exjarfile): $(libj_jarfile) $(JAVA_EXSRCS) $(JAVA_CLASSTOP)
	$(JAVAC) -classpath $(CLASSPATH) -d $(JAVA_CLASSTOP) $(JAVACFLAGS) $(JAVA_EXSRCS)
	cd $(JAVA_CLASSTOP) && $(JAR) cf ../$@ $(JAVA_EXREL)

$(libj_perfjarfile): $(libj_jarfile) $(JAVA_PERFSRCS) $(JAVA_CLASSTOP)
	$(JAVAC) -classpath $(CLASSPATH) -d $(JAVA_CLASSTOP) $(JAVACFLAGS) $(JAVA_PERFSRCS)
	cd $(JAVA_CLASSTOP) && $(JAR) cf ../$@ $(JAVA_PERFREL)

$(libj_testjarfile): $(libj_jarfile) $(JAVA_TESTSRCS) $(JAVA_CLASSTOP)
	$(JAVAC) -classpath $(CLASSPATH) -d $(JAVA_CLASSTOP) $(JAVACFLAGS) $(JAVAC_TESTFLAGS) $(JAVA_TESTSRCS)
	cd $(JAVA_CLASSTOP) && $(JAR) cf ../$@ $(JAVA_TESTREL)


##################################################
# Installed documentation
##################################################

DOCLIST=\
	api_c api_cxx api_tcl collections gsg gsg_db_rep gsg_txn gsg_xml_txn \
	gsg_xml images index.html license java ref ref_xml intro_xml utility

install_docs:
	test -z "$(pkgdatadir)" || $(MKDIR_P) "$(DESTDIR)$(pkgdatadir)"
	cd "$(DESTDIR)$(pkgdatadir)" && rm -rf $(DOCLIST)
	cd $(top_srcdir)/docs && (cp -pr $(DOCLIST) "$(DESTDIR)$(pkgdatadir)" || echo "Documentation cannot be found - skipping install")

uninstall_docs:
	cd "$(DESTDIR)$(pkgdatadir)" && (rm -rf $(DOCLIST) || echo "Documentation cannot be found - skipping uninstall")

#install-data-local: install_docs
#uninstall-local: uninstall_docs

##################################################
# Misc rules
##################################################

# Rule for rebuilding dist/version.m4 - the version information for configure.ac
$(top_srcdir)/dist/version.m4: $(top_srcdir)/dist/version.m4.template $(top_srcdir)/dist/lib_paths.sed
	sed -f $(top_srcdir)/dist/lib_paths.sed  < $(top_srcdir)/dist/version.m4.template > $(top_srcdir)/dist/version.m4

# Rule for rebuilding dist/lib_paths.sed from lib_paths.sed.template
$(top_srcdir)/dist/lib_paths.sed: $(top_srcdir)/dist/lib_paths.sed.template $(top_srcdir)/dist/RELEASE $(top_srcdir)/dist/s_paths
	cd $(top_srcdir)/dist && ./s_paths

util_progs: $(UTIL_PROGS)

examples: $(EXAMPLES)

test_progs: libdbxml_tcl.la dbxml_dump dbxml_load $(TEST_PROGS) $(XMLUNIT_TESTS)



