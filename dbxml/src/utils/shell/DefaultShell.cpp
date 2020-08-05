//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DefaultShell.hpp"
#include "ContainerQueryCommand.hpp"
#include "PrepareCommand.hpp"
#include "QueryCommand.hpp"
#include "PrintCommand.hpp"
#include "CommitCommand.hpp"
#include "AbortCommand.hpp"
#include "PreLoadCommand.hpp"
#include "ContextItemQueryCommand.hpp"
#include "OpenContainerCommand.hpp"
#include "ListIndexesCommand.hpp"
#include "AddIndexCommand.hpp"
#include "DeleteIndexCommand.hpp"
#include "SetVariableCommand.hpp"
#include "SetTypedVariableCommand.hpp"
#include "SetNamespaceCommand.hpp"
#include "LazyCommand.hpp"
#include "ReturnTypeCommand.hpp"
#include "QueryPlanCommand.hpp"
#include "CreateContainerCommand.hpp"
#include "AddDocumentCommand.hpp"
#include "DeleteDocumentCommand.hpp"
#include "SetBaseURICommand.hpp"
#include "IndexLookupCommand.hpp"
#include "EdgeIndexLookupCommand.hpp"
#include "StatsLookupCommand.hpp"
#include "StartTransactionCommand.hpp"
#include "RemoveContainerCommand.hpp"
#include "GetDocumentsCommand.hpp"
#include "SetVerboseCommand.hpp"
#include "UpgradeCommand.hpp"
#include "InfoCommand.hpp"
#include "AddAliasCommand.hpp"
#include "RemoveAliasCommand.hpp"
#include "SetMetaDataCommand.hpp"
#include "GetMetaDataCommand.hpp"
#include "ReindexCommand.hpp"
#include "TimeCommand.hpp"
#include "EchoCommand.hpp"
#include "SyncCommand.hpp"
#include "SetQueryTimeoutCommand.hpp"
#include "SetIgnoreCommand.hpp"
#include "CompactCommand.hpp"
#include "ProjectionCommand.hpp"
#include "SetAutoIndexingCommand.hpp"
#include "DebugCommand.hpp"
#include "DebugOptimizationCommand.hpp"

using namespace DbXml;

DefaultShell::DefaultShell()
{
	addCommand(new StartTransactionCommand);
	addCommand(new CommitCommand);
	addCommand(new AbortCommand);
	addCommand(new PrepareCommand);
	addCommand(new QueryCommand);
	addCommand(new ContainerQueryCommand);
	addCommand(new ContextItemQueryCommand);
	addCommand(new IndexLookupCommand);
	addCommand(new EdgeIndexLookupCommand);
	addCommand(new PrintCommand);
	addCommand(new CreateContainerCommand);
	addCommand(new OpenContainerCommand);
	addCommand(new PreLoadCommand);
	addCommand(new AddDocumentCommand);
	addCommand(new DeleteDocumentCommand);
	addCommand(new ListIndexesCommand);
	addCommand(new AddIndexCommand);
	addCommand(new DeleteIndexCommand);
	addCommand(new SetVariableCommand);
	addCommand(new SetTypedVariableCommand);
	addCommand(new SetNamespaceCommand);
	addCommand(new SetBaseURICommand);
	addCommand(new LazyCommand);
	addCommand(new ReturnTypeCommand);
	addCommand(new QueryPlanCommand);
	addCommand(new StatsLookupCommand);
	addCommand(new RemoveContainerCommand);
	addCommand(new GetDocumentsCommand);
	addCommand(new SetVerboseCommand);
	addCommand(new UpgradeCommand);
	addCommand(new InfoCommand);
	addCommand(new AddAliasCommand);
	addCommand(new RemoveAliasCommand);
	addCommand(new SetMetaDataCommand);
	addCommand(new GetMetaDataCommand);
	addCommand(new ReindexCommand);
	addCommand(new TimeCommand);
	addCommand(new EchoCommand);
	addCommand(new SyncCommand);
	addCommand(new SetQueryTimeoutCommand);
	addCommand(new SetIgnoreCommand);
	addCommand(new CompactCommand);
	addCommand(new ProjectionCommand);
	addCommand(new SetAutoIndexingCommand);
	addCommand(new DebugCommand);
	addCommand(new DebugOptimizationCommand);
	sortCommands();
}
