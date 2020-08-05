

use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn <<EOM;
This program updates a document found in a DBXML container. You should
pre-load the container using loadExamplesData.[sh|cmd] before running this
example. You are only required to pass this command the path location of the
database environment that you specified when you pre-loaded the examples
data:

    -h <dbenv directory>
EOM

        exit( -1 );
}

sub doUpdateDocument ($$$$$)
{
    my $container = shift ;
    my $query = shift ;
    my $context = shift ;
    my $txn = shift ;
    my $mgr = shift ;
    
    # //////////////////////////////////////////////////////////////////////
    # ////  Updates a document stored in a DBXML container.          ///////
    # //////////////////////////////////////////////////////////////////////
    
    # /// some defensive code eliminated for clarity //
    my $fullQuery = "collection('" . $container->getName() . "')$query";    
    eval {
	print "Updating document for expression: '$query'\n";
	print "Return to continue: ";
	<>;
	
	my $results = $mgr->query($txn, $fullQuery, $context ) ;
	print "Found " . $results->size() 
	    . " documents matching the expression '$fullQuery'\n\n";
	
        # create an update context. This is optional but since we could be performing more than
        #  one update, this could offer us a potential performance benefit.
	my $updateContext = $mgr->createUpdateContext();
	
	my $theDocument = $mgr->createDocument();
	while( $results->next($theDocument) ) {
	    my $docString;
	    
            # Retrieve the document's value as a string.
	    $docString = $theDocument->getContent();
	    
	    print "Updating document: \n$docString\n" ;
	    print "Return to continue: ";
	    <>;
	    
            # Update the document. We do it as a string.
	    
            # This next function just modifies the document string
            # in a small way.
	    $docString = getNewDocument( $mgr, $theDocument, $context, $docString);
	    
	    print "Updating document...\n";
	    
            # Set the document's content to be the new document string
	    $theDocument->setContent( $docString );
	    
            # Now replace the document in the container
	    $container->updateDocument($txn, $theDocument, $updateContext);
	    print "Document updated.\n";
	}
    }; 
    # Catches XmlException
    if (my $e = catch std::exception)
    {
	warn "Document deletion failed. \n";
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
	warn "Document deletion failed. \n";
        warn $@;
        exit( -1 );
    }    
}


sub getValue ($$$$)
{
    my $mgr = shift ;
    my $document = shift ;
    my $query = shift ;
    my $context = shift ;
    
    # ///////////////////////////////////////////////////////////////////////////////
    # /////////    Return specific information from a document. ///////////////////// 
    # /////////   !!!!!! Assumes a result set of size 1 !!!!!!! /////////////////////
    # ///////////////////////////////////////////////////////////////////////////////
    
    
    #  Exception handling omitted....
    
    # Perform the query
    my $doc_expr = $mgr->prepare($query, $context);
    my $result = $doc_expr->execute(new XmlValue($document), $context);

    # We require a result set size of 1.
    die "Expected a result set size of 1"
        unless $result->size == 1 ;

    # Get the value. If we allowed the result set to be larger than size 1,
    # we would have to loop through the results, processing each as is
    # required by our application.
    my $value;
    $result->next($value);

    return $value;
        
}

# Simple little function to replace part of a string with a new value.
# All this does is add an 'A' to the end of the document's inventory
# value. So each time you run this program, the inventory value should
# get longer by one 'A'.
sub getNewDocument( $$$$)
{
    my $mgr = shift ;
    my $theDocument = shift ;
    my $context = shift ;
    my $docString = shift ;

    # get the substring that we want to replace
    my $inventory = getValue($mgr, $theDocument, 
                             '/*/inventory/inventory/text()', $context );

    if ($docString !~ s/($inventory)/${1}A/)
    {
        warn "Error: inventory string: '$inventory' not found in document string:\n";
        warn "$docString\n";
    }

    return $docString;
}

my $path2DbEnv;
my $theContainer = "namespaceExampleData.dbxml";
while (@ARGV && $ARGV[0] =~ /^-/ )
{
    my $opt = shift @ARGV ;

    if ($opt =~ s/^-h//)
      { $path2DbEnv = $opt || shift }
    else
      { usage() }
}

usage()
     if ! $path2DbEnv ;

 # Open a container in the db environment
my $env = new DbEnv(0);
$env->set_cachesize(0, 64 * 1024 * 1024, 1);
$env->open($path2DbEnv,
    Db::DB_INIT_MPOOL|Db::DB_CREATE|Db::DB_INIT_LOCK|Db::DB_INIT_LOG|Db::DB_INIT_TXN);
my $mgr = new XmlManager($env);

my $containerTxn = $mgr->createTransaction();
my $container = $mgr->openContainer($containerTxn, $theContainer);
$containerTxn->commit();

# Get a transaction
my $txn = $mgr->createTransaction();

# Create a context and declare the namespaces
my $context = $mgr->createQueryContext();
$context->setNamespace('fruits'     => 'http://groceryItem.dbxml/fruits');
$context->setNamespace('vegetables' => 'http://groceryItem.dbxml/vegetables');
$context->setNamespace('desserts'   => 'http://groceryItem.dbxml/desserts');

# update the document that describes Zapote Blanco (a fruit)
doUpdateDocument( $container, "/fruits:item/product[text() = 'Zapote Blanco']", 
                      $context, $txn, $mgr );

$txn->commit();

