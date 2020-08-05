

use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn <<EOM;
This program performs various XPath queries against a DBXML container and then
retrieves information of interest from the retrieved document(s).  You should
pre-load the container using loadExamplesData.[sh|cmd] before running this
example.  You are only required to pass this command the path location of the
database environment that you specified when you pre-loaded the examples data:

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

sub getDetails($$$$)
{
    my $mgr = shift ;
    my $container = shift ;
    my $query = shift ;
    my $context = shift ;

    #///////////////////////////////////////////////////////////////////////
    #/////  Performs an query (in context) against the               ///////
    #/////  provided container.                                      ///////
    #///////////////////////////////////////////////////////////////////////


    #//// some defensive code eliminated for clarity //

    #  Perform the query. Result type is by default Result Document
    my $fullQuery = "collection('" .$container->getName() . "')$query";
    eval {
        print "Exercising query '$fullQuery' \n";
        print "Return to continue: ";
        <>;

        my $results = $mgr->query($fullQuery, $context );
        my $value = new XmlValue();
        print "\n\tProduct : Price : Inventory Level\n";
        while( $results->next($value) ) {
            #  Retrieve the value as a document
            my $theDocument = $value->asDocument();

            #  Obtain information of interest from the document. Note that the 
            #  wildcard in the query expression allows us to not worry about what
            #  namespace this document uses.
            my $item = getValue($mgr, $theDocument, "/*/product/text()", $context);
            my $price = getValue($mgr, $theDocument, "/*/inventory/price/text()", $context);
            my $inventory = getValue($mgr, $theDocument, "/*/inventory/inventory/text()", $context);

            print "\t$item : $price : $inventory\n";

        }
        print "\n";
        print $results->size() . " objects returned for expression '$fullQuery'\n" ;
    };
    # Catches XmlException
    if (my $e = catch std::exception)
    {
        warn "Query $fullQuery failed\n";
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
        warn "Query $fullQuery failed\n";
        warn $@;
        exit( -1 );
    }    
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

# Create a context and declare the namespaces
my $context = $mgr->createQueryContext();
$context->setNamespace('fruits'     => 'http://groceryItem.dbxml/fruits');
$context->setNamespace('vegetables' => 'http://groceryItem.dbxml/vegetables');
$context->setNamespace('desserts'   => 'http://groceryItem.dbxml/desserts');

# get details on Zulu Nuts
getDetails($mgr, $container, "/fruits:item/product[text() = 'Zulu Nut']", $context);

# get details on all fruits that start with 'A'
getDetails($mgr, $container, "/vegetables:item/product[starts-with(text(),'A')]", $context);

