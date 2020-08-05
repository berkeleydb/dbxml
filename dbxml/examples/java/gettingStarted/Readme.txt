# See the file LICENSE for redistribution information.
#
# Copyright (c) 2004,2009 Oracle.  All rights reserved.
#

                DBXML Getting Started Examples
                            Readme

This directory contains examples and sample data designed to illustrate
DBXML's basic capabilities. To run the examples you must first compile
the examples and then load the sample data using exampleLoadContainer.java
to load all the sample data contained in the xmlData directory.

PREPARING TO RUN JAVA EXAMPLES

The examples are compiled, by default, by the build system, if Java
is enabled, and put in the jar file, dbxmlexamples.jar.  On Unix systems,
this will be in the dbxml/build_unix directory.  One Windows, it will be in
the jar directory.

To run examples you need to:
1. Add libraries or dlls to appropriate search path
2. Add db.jar, dbxml.jar and dbxmlexamples.jar to CLASSPATH

WINDOWS

1.  Set PATH to include required DLLS, which are, by default,
in dbxml/bin/{debug}.  For example:

      set PATH=%PATH%;<path_to_distribution>/bin/debug

2.  Set CLASSPATH to include db.jar, dbxml.jar and dbxmlexamples.jar
    (the following should be all one line, without spaces)
    set CLASSPATH=%CLASSPATH%;<path_to_distribution>/jar/db.jar;
         <path_to_distribution>/jar/dbxml.jar;
         <path_to_distribution>/jar/dbxmlexamples.jar

UNIX

1.  Set LD_LIBRARY_PATH (or equivalent) to include required
libraries.  By default, they are in <path_to_distribution>/install/lib.
For example:
      LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path_to_install>/lib
      export LD_LIBRARY_PATH

2.  Set CLASSPATH to include db.jar, dbxml.jar and dbxmlexamples.jar
    (the following should be all one line, without spaces)
      CLASSPATH=$CLASSPATH:<path_to_install>/lib/db.jar:
                  <path_to_install>/lib/dbxml.jar:
                  <path_to_install>/dbxml/build_unix/dbxmlexamples.jar
      export CLASSPATH

LOADING EXAMPLE CONTAINERS

It is best to run the examples from the example directory,
dbxml/examples/java/gettingStarted.
For example:
    cd dbxml/examples/java/gettingStarted
    mkdir dbEnv
    java dbxml.gettingStarted.exampleLoadContainer -h dbEnv -p ../../xmlData

The "-p ../../xmlData" specifies a path to the XML example files, which
are loaded into containers in the "dbEnv" directory.

RUNNING EXAMPLES

Once loaded, most of the examples found here can be run by simply identifying
the directory where you placed your database environment. For example:

    java dbxml.gettingStarted.simpleQuery -h dbEnv

EXAMPLES DESCRIPTION

- simpleContainerInEnv.java
  Creates a database environment, opens it, and then creates and opens three
  containers in that environment. Finally, the containers and environments are
  closed. You should edit this file and provide a path to an existing
  directory where you want the database environment placed before running this
  example.

- myDbEnv.java
  Class that creates/opens a database environment. All of the following
  examples rely on this class.

- simpleAdd.java
  Creates a database environment and container, and then adds two very simple
  XML documents to the container. You identify the location where you want the
  database environment placed using the -h option. This example requires no
  other command line options.

- mdConst.java
  Simple utility class use to encapsulate some constants used by some
  of the following examples.

- exampleLoadContainer.java
  Creates/opens the specified database environment, creates/opens the
  specified container in that environment, and then loads every XML document
  identified on the command line call into the identified container. As each
  document is loaded, a document name is set. Metadata is also set on each
  document that represents the day and time that the document was loaded into
  the container.

  All of the following examples assume that exampleLoadContainer has been run.

- simpleQuery.java
  Executes several XPath queries and prints query results to the console.
  Use the -h option to identify the database environment location that you
  used when you ran loadExamplesData.

       simpleQuery -h <dbenv>

  Note that all of the following examples use the same command line option.

- queryWithContext.java
  Identical to simpleQuery, except that contexts are used to define
  namespaces and variables.

- queryForDocumentValue.java
  Identical to queryWithContext, except that specific document data is
  retrieved from each document found in the query's result set.

- queryWithDocumentNames.java
  Identical to queryWithContext, except that the query is performed using the
  document name that was set when exampleLoadContainer placed the document
  into the container.

- queryForMetaData.java
  Identical to queryWithContext, except that the timestamp that was stored as
  metadata is retrieved and displayed. This timestamp was set on the document
  when exampleLoadContainer placed the document into the container.

- updateDocument.java
  Identical to queryWithContext, except that the document retrieved as the
  result of the query is updated in a trivial way and then stored back into
  the container.

- deleteDocument.java
  Identical to queryWithContext, except that every document retrieved as the
  result of the query is deleted from the container.

- addIndex.java
  Adds indexes to a specific document node.

- replaceIndex.java
  Replaces the indexes on a specific document node with new indexes. Iterates
  through the indexes maintained for the container and shows what is being
  used. For best results, run addIndex before running this example.

- deleteIndex.java
  Deletes indexes from a specific document. Iterates through the indexes
  maintained for the container and shows what is being used. For best results,
  run addIndex before running this example.

- myDb.java
  Class that creates/opens a Berkeley db. The following two examples rely
  on this class.

- buildDB.java
  Illustrates how a DBXML container can be used with a Berkeley DB database. A
  Berkeley DB database is created in the same environment as the container and
  data corresponding to documents in the container is loaded into the
  database. The DBXML query and database put are all wrapped in a common
  transaction.

- retrieveDB.java
  Illustrates how a DBXML container can be used with a Berkeley DB database.
  Documents are retrieved from the container and then data corresponding to
  each document is retrieved from the Berkeley DB database. Again, all queries
  are wrapped in a common transaction. For best results, run buildDB before
  running this example.

- externalFunction.java
  A very simple Berkeley DB XML program that demonstrates external function API
  usage. This program demonstrates:
  1. XmlResolver used to resolve multiple external functions
  2. XmlExternalFunction implementations
  3. External function results handling
