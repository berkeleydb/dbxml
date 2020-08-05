

use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn  <<EOM;
This program retrieves DBXML documents using their document names. You should
pre-load the container using loadExamplesData.[pl|sh|cmd] before running this
example.  You are only required to pass this command the path location of the
database environment that you specified when you pre-loaded the examples
data:

    -h <dbenv directory>
EOM
        exit( -1 );
}

sub doContextQuery($$$$)
{
    my $mgr     = shift ;
    my $cname   = shift ;
    my $query   = shift ;
    my $context = shift ;

    # //////////////////////////////////////////////////////////////////////
    # ////  Performs a simple query (with context) against the       ///////
    # ////  provided container.                                      ///////
    # //////////////////////////////////////////////////////////////////////
	
    # /// some defensive code eliminated for clarity //
	
    #  Perform the query. Result type is by default Result Document
    my $fullQuery = "collection('$cname')$query";
    eval {
        print "Exercising query '$fullQuery'\n" ;
        print "Return to continue: ";
        <>;
        print "\n";
	
        my $results = $mgr->query($fullQuery, $context);
        my $value = new XmlValue ;
        while($results->next($value)) {
            #  Get the document's name and print it to the console
            my $theDocument = $value->asDocument();
            print "Document name: " . $theDocument->getName() . "\n";
            print "$value\n" ;
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
    print "opening $path2DbEnv\n";
    $env->open($path2DbEnv,
                Db::DB_INIT_MPOOL|Db::DB_CREATE|Db::DB_INIT_LOCK|Db::DB_INIT_LOG|Db::DB_INIT_TXN);
    my $theMgr = new XmlManager($env);

    my $containerTxn = $theMgr->createTransaction();
    my $container = $theMgr->openContainer($containerTxn, $theContainer);
    $containerTxn->commit();


    # create a context and declare the namespaces
    my $context = $theMgr->createQueryContext();
    $context->setNamespace("fruits", "http://groceryItem.dbxml/fruits");
    $context->setNamespace("vegetables", "http://groceryItem.dbxml/vegetables");
    $context->setNamespace("desserts", "http://groceryItem.dbxml/desserts");

    # Query for documents by their document names.
    my @queryList = qw(
            /*[dbxml:metadata('dbxml:name')='ZuluNut.xml']
            /*[dbxml:metadata('dbxml:name')='TrifleOrange.xml']
            /*[dbxml:metadata('dbxml:name')='TriCountyProduce.xml']
        );

    foreach my $query (@queryList)
     { doContextQuery($theMgr, $container->getName, $query, $context) }
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

