#
# Copyright (c) 2004,2009 Oracle.  All rights reserved.

                DBXML Getting Started Examples
                            Readme

This directory contains examples and sample data designed to illustrate
BDB XML's basic capabilities. Generally, to run the examples you must first
load the sample data using loadExamplesData[.sh|.cmd] (depending on your
platform).  This script uses the exampleLoadContainer example program to
load all the sample data contained in the dbxml/examples/xmlData directory
into the containers expected by the rest of the examples.

PREPARING TO RUN C++ EXAMPLES

The examples are compiled by the default build of BDB XML.  On
Unix, they are built into the directory:
   dbxml/build_unix

On Windows, they are in:
   bin -- release build
or
   bin/debug -- debug build

On Windows, the example names are prefixed with "dbxml_example_,"
for example, dbxml_example_simpleQuery.

Before running the loadExamplesData script, or any of the commands,
you must make sure that your environment is correct.  On Windows, this
means setting PATH to include the location of the BDB XML library
and the third-party libraries.  For example:
      PATH=%PATH%;<path_to_distribution>/bin/debug

On Unix, you might use (in sh):
      LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path_to_install>/lib
      export LD_LIBRARY_PATH

LOADING EXAMPLES CONTAINERS

The examples can be run from other locations, but it
is best to run them from the directory in which this
Readme file resides (dbxml/examples/cxx/gettingStarted).

Windows:

1.  Change directory to dbxml/examples/cxx/gettingStarted
    cd dbxml/examples/cxx/gettingStarted
2.  Create a directory for the new container environment
    mkdir dbEnv  (use any name you want)
3.  Run the script
    loadExamplesData.cmd dbEnv .

    The "dbEnv" argument is the directory you created.  The
    "." argument is the current directory.  It is used to find
    the example XML files, which are in ../../xmlData.

Unix:

1.  Change directory to dbxml/examples/cxx/gettingStarted
    cd dbxml/examples/cxx/gettingStarted
2.  Create a directory for the new container environment
    mkdir dbEnv  (use any name you want)
3.  Run the script
    ./loadExamplesData.sh dbEnv ../../../build_unix

    The "dbEnv" argument is the directory you created.  The
    "../../../build_unix" argument is the path to the example
    executable files.  This command must be run from the
    gettingStarted directory, since it looks in "../../xmlData"
    to find the example XML files.

At this point, the sample data is loaded into a database
environment stored in the directory you specified above.
This data is used by most of the other gettingStarted examples.


RUNNING EXAMPLES

Once loaded, most of the examples can be run by simply identifying
the directory where you placed your database environment.
For example:

On Unix:
    (from dbxml/examples/cxx/gettingStarted)
    ../../../build_unix/simpleQuery -h dbEnvironment

On Windows:
    (assumes example executables are in your PATH)
    dbxml_example_simpleQuery -h dbEnvironment

On Windows, the example names are prefixed with "dbxml_example_,"
for example, dbxml_example_simpleQuery.  Windows examples are built
into the <path_to_distribution>/bin{/debug} directory
by default, where Release or Debug is the configuration that has
been built.

EXAMPLES DESCRIPTION

- simpleContainer.cpp

  Creates a container, closes it, and then quits. No database environment is
  used. You should edit this file and identify a real path to where you want
  your container placed before running this example.

- simpleContainerInEnv.cpp
  Creates a database environment, opens it, and then creates and opens three
  containers in that environment. Finally, the containers and environments are
  closed. You should edit this file and provide a path to an existing
  directory where you want the database environment placed before running this
  example.

- simpleAdd.cpp
  Creates a database environment and container, and then adds two very simple
  XML documents to the container. You identify the location where you want the
  database environment placed using the -h option. This example requires no
  other command line options.

- exampleLoadContainer.cpp
  Creates/opens the specified database environment, creates/opens the
  specified container in that environment, and then loads every XML document
  identified on the command line call into the identified container. As each
  document is loaded, a document name is set. Metadata is also set on each
  document that represents the day and time that the document was loaded into
  the container.

  All of the following examples assume that exampleLoadContainer has been run
  via loadExamplesData[.sh|.bat].

    exampleLoadContainer -h <dbenv> -c <containername> file1.xml file2.xml ...

- simpleQuery.cpp
  Executes several XPath queries and prints query results to the console.
  Use the -h option to identify the database environment location that you
  used when you ran loadExamplesData.

       simpleQuery -h <dbenv>

  Note that all of the following examples use the same command line option.

- queryWithContext.cpp
  Identical to simpleQuery, except that contexts are used to define
  namespaces and variables.

- queryForDocumentValue.cpp
  Identical to queryWithContext, except that specific document data is
  retrieved from each document found in the query's result set.

- queryWithDocumentNames.cpp
  Identical to queryWithContext, except that the query is performed using the
  document name that was set when exampleLoadContainer placed the document
  into the container.

- queryForMetaData.cpp
  Identical to queryWithContext, except that the timestamp that was stored as
  metadata is retrieved and displayed. This timestamp was set on the document
  when exampleLoadContainer placed the document into the container.

- updateDocument.cpp
  Identical to queryWithContext, except that the document retrieved as the
  result of the query is updated in a trivial way and then stored back into
  the container.

- deleteDocument.cpp
  Identical to queryWithContext, except that every document retrieved as the
  result of the query is deleted from the container.

- addIndex.cpp
  Adds indexes to a specific document node.

- replaceIndex.cpp
  Replaces the indexes on a specific document node with new indexes. Iterates
  through the indexes maintained for the container and shows what is being
  used. For best results, run addIndex before running this example.

- deleteIndex.cpp
  Deletes indexes from a specific document. Iterates through the indexes
  maintained for the container and shows what is being used. For best results,
  run addIndex before running this example.

- buildDB.cpp
  Illustrates how a DBXML container can be used with a Berkeley DB database. A
  Berkeley DB database is created in the same environment as the container and
  data corresponding to documents in the container is loaded into the
  database. The DBXML query and database put are all wrapped in a common
  transaction.

- retrieveDB.cpp
  Illustrates how a DBXML container can be used with a Berkeley DB database.
  Documents are retrieved from the container and then data corresponding to
  each document is retrieved from the Berkeley DB database. Again, all queries
  are wrapped in a common transaction. For best results, run buildDB before
  running this example.
