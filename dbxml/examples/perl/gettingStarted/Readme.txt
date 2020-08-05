#
# Copyright (c) 2004,2009 Oracle.  All rights reserved.

                DBXML Getting Started Examples (Perl)
                            Readme

This directory contains examples and sample data designed to illustrate
BDB XML's basic capabilities. Generally, to run the examples you must first
load the sample data using loadExamplesData.pl.
This script uses the exampleLoadContainer.pl example program to
load all the sample data contained in the dbxml/examples/xmlData directory
into the containers expected by the rest of the examples.

PREPARING TO RUN PERL EXAMPLES

Before running the loadExamplesData script, or any of the commands,
you must make sure that your environment is correct.  On Windows, this
means setting PATH to include the location of the BDB XML library
and the third-party libraries.  For example:
      PATH=%PATH%;<path_to_distribution>/bin

On Unix, you might use (in sh):
      LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path_to_install>/lib
      export LD_LIBRARY_PATH

LOADING EXAMPLES CONTAINERS

The examples can be run from other locations, but it
is best to run them from the directory in which this
Readme file resides (dbxml/examples/perl/gettingStarted).

Windows:

1.  Change directory to dbxml/examples/perl/gettingStarted
    cd dbxml/examples/perl/gettingStarted
2.  Create a directory for the new container environment
    mkdir dbEnv  (use any name you want)
3.  Run the script
    perl loadExamplesData.pl dbEnv ..\..\xmlData

    The "dbEnv" argument is the directory you created.  The
    second argument is the path to the example XML files, 
    which are in ../../xmlData.

Unix:

1.  Change directory to dbxml/examples/perl/gettingStarted
    cd dbxml/examples/perl/gettingStarted
2.  Create a directory for the new container environment
    mkdir dbEnv  (use any name you want)
3.  Run the script
    perl loadExamplesData.pl dbEnv ../../xmlData

At this point, the sample data is loaded into a database
environment stored in the directory you specified above.
This data is used by most of the other gettingStarted examples.

RUNNING EXAMPLES

Once loaded, most of the examples can be run by simply identifying
the directory where you placed your database environment.
For example:

    (from dbxml/examples/perl/gettingStarted)
    perl simpleQuery.pl -h dbEnv

EXAMPLES DESCRIPTION

- simpleAdd.pl
  Creates a database environment and container, and then adds two very simple
  XML documents to the container. You identify the location where you want the
  database environment placed using the -h option. This example requires no
  other command line options.

- exampleLoadContainer.pl
  Creates/opens the specified database environment, creates/opens the
  specified container in that environment, and then loads every XML document
  identified on the command line call into the identified container. As each
  document is loaded, a document name is set. Metadata is also set on each
  document that represents the day and time that the document was loaded into
  the container.

  All of the following examples assume that exampleLoadContainer has been run
  via loadExamplesData.pl

    perl exampleLoadContainer.pl -h <dbenv> -c <containername> file1.xml file2.xml ...

- simpleQuery.pl
  Executes several XPath queries and prints query results to the console.
  Use the -h option to identify the database environment location that you
  used when you ran loadExamplesData.

       perl simpleQuery.pl -h <dbenv>

  Note that all of the following examples use the same command line option.

- queryWithContext.pl
  Identical to simpleQuery, except that contexts are used to define
  namespaces and variables.

- queryForDocumentValue.pl
  Identical to queryWithContext, except that specific document data is
  retrieved from each document found in the query's result set.

- queryWithDocumentNames.pl
  Identical to queryWithContext, except that the query is performed using the
  document name that was set when exampleLoadContainer placed the document
  into the container.

- queryForMetaData.pl
  Identical to queryWithContext, except that the timestamp that was stored as
  metadata is retrieved and displayed. This timestamp was set on the document
  when exampleLoadContainer placed the document into the container.

- updateDocument.pl
  Identical to queryWithContext, except that the document retrieved as the
  result of the query is updated in a trivial way and then stored back into
  the container.

- deleteDocument.pl
  Identical to queryWithContext, except that every document retrieved as the
  result of the query is deleted from the container.

- retrieveDB.pl
  Illustrates how a DBXML container can be used with a Berkeley DB database.
  Documents are retrieved from the container and then data corresponding to
  each document is retrieved from the Berkeley DB database. Again, all queries
  are wrapped in a common transaction. For best results, run buildDB before
  running this example.
