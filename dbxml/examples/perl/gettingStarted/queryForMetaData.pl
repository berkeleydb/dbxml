
use strict;
use warnings;
use Sleepycat::DbXml 'simple';

sub usage()
{
    warn   <<EOM;
This program retrieves DBXML documents and then retrieves the day and
time the document was stored in the container. The date/time information
was set as metadata information when the document was loaded. See the
exampleLoadContainer.cpp example for how this metadata is set.

Before running this example, use loadExamplesData.[sh|cmd] to pre-load
the appropriate container with documents and metadata.

You are only required to pass this command the path location of the
database environment that you specified when you pre-loaded the
examples data:

    -h <dbenv directory>
EOM

        exit( -1 );
}

sub showTimeStamp
{
    my $mgr = shift ;
    my $container = shift;
    my $query = shift; 
    my $context  = shift;

    # //////////////////////////////////////////////////////////////////////
    # ////  Performs a simple query (with context) against the ///////
    # ////  provided container. The timestamp metadata attribute     ///////
    # ////  value is then displayed.                                 ///////
    # //////////////////////////////////////////////////////////////////////

    # /// some defensive code eliminated for clarity //

    # Used for metadata query
    my $mdURI = "http://dbxmlExamples/timestamp";
    my $mdPrefix = "time";
    my $mdName = "timeStamp";


    #  Perform the query. Result type is by default Result Document
    my $fullQuery = "collection('" . $container->getName() . "')$query";    
    eval {
        print "Exercising query '$fullQuery'\n" ;
        print "Return to continue: ";
        <>;

        my $results = $mgr->query($fullQuery, $context );
        my $value = new XmlValue ;
        while( $results->next($value) )
        {
            my $theDocument = $value->asDocument();

            # Get this document's name
            my $docName= $theDocument->getName() ;

            # Get the timestamp on the document (stored as metadata)
            #  and print it to the console.
            my $metaValue = new XmlValue() ;
            $theDocument->getMetaData( $mdURI, $mdName, $metaValue );
            print "Document '$docName' stored on $metaValue\n";
            #print "Document '$docName' stored on " . $metaValue->asString() . "\n";
            #my $metaValue ;
            #$theDocument->getMetaData( $mdURI, $mdName, $metaValue );
            #print "Document '$docName' stored on $metaValue\n";
        }

        print $results->size() . " objects returned for expression '$fullQuery'\n";

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

# Find all the vegetables
showTimeStamp( $mgr, $container, "/vegetables:item", $context);

