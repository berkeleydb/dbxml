
use strict;
use warnings;
use Sleepycat::DbXml 'simple';

sub usage()
{
    warn <<EOM ;
This program performs various XPath queries against a DBXML container. 
You should pre-load the container using loadExamplesData.[sh|cmd] before
running this example. You are only required to pass this command the
path location of the database environment that you specified when you
pre-loaded the examples data:

    -h <dbenv directory>
EOM

    exit( -1 );
}

sub doQuery($$$)
{
    my $theMgr = shift ;
    my $container = shift ;
    my $query = shift ;

    #///////////////////////////////////////////////////////////////////////
    #/////  Performs a simple query (no context) against the   ///////
    #/////  provided container.                                      ///////
    #///////////////////////////////////////////////////////////////////////

    #//// some defensive code eliminated for clarity //

    #  Perform the query. Result type is by default Result Document
    my $fullQuery = "collection('" . $container->getName() . "')$query";    
    eval {
        print "Exercising query '$fullQuery '\n" ;
        print "Return to continue: ";
        <> ;

        my $results = $theMgr->query($fullQuery);
        my $value ;
        while( $results->next($value) ) {
            print "$value\n";
        }

        print $results->size() . " objects returned for expression '" .
              "$fullQuery'\n" ;
	};

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
my $theContainer = "simpleExampleData.dbxml";
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


    # perform the queries

    # find all the Vendor documents in the database
    doQuery( $theMgr, $container, '/vendor' );

    # find all the vendors that are wholesale shops
    doQuery( $theMgr, $container, '/vendor[@type="wholesale"]');

    # find the product document for "Lemon Grass"
    doQuery( $theMgr, $container, '/product/item[.="Lemon Grass"]');

    # find all the products where the price is less than or equal to 0.11
    doQuery( $theMgr, $container, '/product/inventory[price<=0.11]');

    # find all the vegetables where the price is less than or equal to 0.11
    doQuery( $theMgr, $container, '/product[inventory/price<=0.11 and category="vegetables"]');

    exit 0;
} ;

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
