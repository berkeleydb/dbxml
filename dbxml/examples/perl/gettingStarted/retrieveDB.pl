use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn  <<EOM;
This program retrieves data from a Berkeley DB data based on information
retrieved from an XML document. The document is first retrieved from an
XML container. Both the database and the container are stored in the same
database environment, and all the necessary queries are performed under the
protection of a transaction.

Use the 'retrieveDB' sample program to retrieve the stored data.

Before running this program, run loadExamplesData.[sh|cmd] to pre-populate
the DBXML container with the appropriate data. Then, run buildDB to populate
the database with the appropriate data.

When you run this program, identify the directory where you told loadExamplesData
to place the sample data:

    -h <dbenv directory> << std::endl;
EOM
    exit( -1 );
}


my $theDB = "testBerkeleyDB";
my $theContainer = "namespaceExampleData.dbxml";
while (@ARGV && $ARGV[0] =~ /^-/ )
{
    my $opt = shift @ARGV ;

    if ($opt =~ s/-h//)
      { $path2DbEnv = $opt || shift }
    else
      { usage() }
}

usage()
  if ! $path2DbEnv ;


eval {
    # open a container in the db environment
    my $env = new DbEnv(0);
    $env->set_cachesize(0, 64 * 1024, 1);
    $env->open($path2DbEnv,
                Db::DB_INIT_MPOOL|Db::DB_CREATE|Db::DB_INIT_LOCK|Db::DB_INIT_LOG|Db::DB_INIT_TXN);
    my $theMgr = new XmlManager($env);

    # Open a database in the environment
    my $myDb openedDatabase($theDB, $mgr );

    my $containerTxn = $theMgr->createTransaction();
    my $container = $theMgr->openContainer($containerTxn, $theContainer);
    $containerTxn->commit();

    try
    {
      //Get a transaction
      XmlTransaction txn = mgr.createTransaction();

            XmlQueryContext resultsContext = mgr.createQueryContext();

            //Find all the vendor documents.
	    std::string fullQuery = "collection('" + container.getName() + "')/vendor";
            XmlResults results( mgr.query(txn, fullQuery, resultsContext) );

            resultsContext.setReturnType( XmlQueryContext::DeadValues );

            //Loop through the result set, retrieving the data we are interested in for each document.
            XmlValue value;
            while( results.next(value) )
            {
                //Query the retrieved document for the salesrep's name
                XmlQueryExpression doc_expr = mgr.prepare("salesrep/name/text()", resultsContext);
                XmlResults docResult = doc_expr.execute(value, resultsContext);

                //The result set from the document query should be of size 1. If not, the
                //document schema has changed and the query must be modified..
                assert( docResult.size() == 1 ); 

                //Pull the value out of the document query result set.
                XmlValue docValue;
                docResult.next(docValue);
                std::string theSalesRepKey = docValue.asString();

                //Reconstruct the key used to store the information relevant to this document
                Dbt theKey( (void *)theSalesRepKey.c_str(), theSalesRepKey.length() + 1 );

                //This next structure is used to hold the data that is
                // retrieved from the db.
                Dbt theData;

                //Get the data and report the results
                openedDatabase.getDatabase().get(txn.getDbTxn(), &theKey, &theData, 0 );
                std::cout << "For key: '" << (char *)theKey.get_data() << "', retrieved:" << std::endl;
                std::cout << "\t" << (char *)theData.get_data() << std::endl;
            }

            txn.commit();
    }
    catch(DbException &e)
    {
        std::cerr << "Error writing to database: "<< std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Aborting transaction and exiting."<< std::endl;

        exit( -1 );
    }
    //Catches XmlException
    catch(std::exception &e)
    {
        std::cerr << "Error writing to database: "<< std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Aborting transaction and exiting."<< std::endl;

        exit( -1 );
    }

    return 0;
}
