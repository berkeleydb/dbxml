
use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn  <<EOM;
This program deletes a DBXML document from a container. You should pre-load the
container using loadExamplesData.[sh|cmd] before running this example. You are
only required to pass this this program the path location of the database
environment that you specified when you pre-loaded the examples data:

    -h <dbenv directory>
EOM
        exit( -1 );
}

sub getValue($$$$)
{
    my $mgr         = shift ;
    my $document    = shift ;
    my $XPath       = shift ;
    my $context     = shift ;

    #///////////////////////////////////////////////////////////////////////
    #//////////    Return specific information from a document. ////////////
    #//////////   !!!!!! Assumes a result set of size 1 !!!!!!! ////////////
    #///////////////////////////////////////////////////////////////////////


    #  Exception handling omitted....

    # Perform the query
    my $doc_expr = $mgr->prepare($XPath, $context);
    my $result = $doc_expr->execute(new XmlValue($document), $context);

    # We require a result set size of 1.
    die "Result set != 1" unless $result->size() == 1 ;

    # Get the value. If we allowed the result set to be larger than size 1,
    # we would have to loop through the results, processing each as is
    # required by our application.
    my $value;
    $result->next($value);

    return $value;
        
}


sub doDeleteDocument($$$$$$)
{
    my $mgr         = shift;
    my $container   = shift;
    my $query       = shift;
    my $context     = shift;
    my $txn         = shift;
    my $uc          = shift;

    # //////////////////////////////////////////////////////////////////////
    # ////  Deletes a document from a DBXML container.               ///////
    # //////////////////////////////////////////////////////////////////////
	
	
    # /// some defensive code eliminated for clarity //
    
    eval {
	print "Deleting documents for expression: '$query'\n";
	print "Return to continue: ";
	<> ; 
        print "\n";
	
	my $results = $mgr->query($txn, $query, $context);
	
	print "Found " . $results->size() 
	    . " documents matching the expression '$query'\n" ;
	
	my $value = new XmlValue;
	print "\n";
	while( $results->next( $value ) ) {
            # Retrieve the value as a document
	    my $theDocument = $value->asDocument();
	    
            # Obtain information of interest from the document. Note that the 
            # wildcard in the query expression allows us to not worry about what
            # namespace this document uses.
	    my $item = getValue($mgr, $theDocument, "/*/product/text()", $context);
	    print "Deleting document: $item\n";
	    
	    $container->deleteDocument($txn, $theDocument, $uc);
	    
	    print "Deleted document: $item\n";
	    
	}
    };
    # Catches XmlException
    if (my $e = catch std::exception)
    {
        warn "Document deletion failed.\n";
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
        warn "Document deletion failed.\n";
        warn $@;
        exit( -1 );
    }
    
}


sub confirmDelete ($$$)
{
    my $mgr     = shift;
    my $query   = shift;
    my $context = shift;

    #  Exception handling omitted....

    print "Confirming the delete.\n";
    print "The query: '$query' should get result set size 0.\n";
    my $resultsConfirm = $mgr->query($query, $context);
    if ( ! $resultsConfirm->size() ) {
        print "No documents found matching query query: '$query'\n"; 
        print "Deletion confirmed.\n";
    } else {
        print "Found documents matching '$query'! " .
        $resultsConfirm->size() . " documents found.\n";
        print "Document deletion failed.\n";
    }
    print "\n";
}

my $path2DbEnv;
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
    $env->set_cachesize(0, 64 * 1024 * 1024, 1);
    $env->open($path2DbEnv,
                Db::DB_INIT_MPOOL|Db::DB_CREATE|Db::DB_INIT_LOCK|Db::DB_INIT_LOG|Db::DB_INIT_TXN);
    my $theMgr = new XmlManager($env);

    my $containerTxn = $theMgr->createTransaction();
    my $container = $theMgr->openContainer($containerTxn, $theContainer);
    $containerTxn->commit();

  
    # myDbEnv and myXmlContainer open with transactions. All subsequent
    # writes to them must also be performed inside a transaction.

    # Get a transaction
    my $txn = $theMgr->createTransaction();

    # create a context and declare the namespaces
    my $context = $theMgr->createQueryContext();
    $context->setNamespace("fruits", "http://groceryItem.dbxml/fruits");
    $context->setNamespace("vegetables", "http://groceryItem.dbxml/vegetables");
    $context->setNamespace("desserts", "http://groceryItem.dbxml/desserts");

    # delete the document that describes Mabolo (a fruit)
    my $query = "/fruits:item[product = 'Mabolo']";
    my $uc = $theMgr->createUpdateContext();
    my $fullQuery = "collection('" . $container->getName() . "')$query";	
    doDeleteDocument($theMgr, $container, $fullQuery, $context, $txn, $uc);

    # Commit the delete
    $txn->commit();

    # The document should now no longer exist in the container. Just for fun,
    #   confirm the delete.
    confirmDelete ($theMgr, $fullQuery, $context);

    return 0;
};
if (my $e = catch std::exception)
{
    warn "Query failed\n";
    warn $e->what() . "\n";
    exit( -1 );
}
elsif ($@)
{
    warn "Query failed\n";
    warn $@;
    exit( -1 );
}

exit 0 ;


