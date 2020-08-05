

use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn  <<EOM;
This program illustrates how to query for documents that require namespace
usage in the query.  You should pre-load the container using
loadExamplesData.[sh|cmd] before running this example.  You are only required
to pass this command the path location of the database environment that you
specified when you pre-loaded the examples data:

    -h <dbenv directory>
EOM
        exit( -1 );
}

sub doContextQuery($$$$)
{
    my $mgr = shift ;
	my $cname = shift ;
    my $query = shift ;
    my $context = shift ;

    # //////////////////////////////////////////////////////////////////////
    # ////  Performs a simple query (with context) against the ///////
    # ////  provided container.                                      ///////
    # //////////////////////////////////////////////////////////////////////
	
    # /// some defensive code eliminated for clarity //
	
    #  Perform the query. Result type is by default Result Document
    my $fullQuery = "collection('$cname')$query";
    eval {
	print "Exercising query '$fullQuery'\n";
	print "Return to continue: ";
	<>;
	
	my $results = $mgr->query($fullQuery, $context);
	my $value;
	while( $results->next($value) )
	{
            #  Obtain the value as a string and print it to the console
	    print "$value\n";
	}
	
	print $results->size() . " objects returned for expression '$fullQuery'\n" ;
	
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


  # Create a context and declare the namespaces
  my $context = $theMgr->createQueryContext();
  $context->setNamespace( "fruits" => "http://groceryItem.dbxml/fruits");
  $context->setNamespace( "vegetables" => "http://groceryItem.dbxml/vegetables");
  $context->setNamespace( "desserts" => "http://groceryItem.dbxml/desserts");

  # Set a variable
  $context->setVariableValue("aDessert", "Blueberry Boy Bait");

  # Perform the queries

  # Find all the Vendor documents in the database. Vendor documents do
  #  not use namespaces, so this query returns documents.
  doContextQuery( $theMgr, $container->getName(), "/vendor", $context );

  # Find the product document for "Lemon Grass". This query returns no documents
  #  because a namespace prefix is not identified for the 'item' node.
  doContextQuery( $theMgr, $container->getName(),
		  '/item/product[text()="Lemon Grass"]', $context);

  # Find the product document for "Lemon Grass" using the namespace prefix 'fruits'. This
  #  query successfully returns a document.
  doContextQuery( $theMgr, $container->getName(),
		  '/fruits:item/product[text()="Lemon Grass"]', $context);

  # Find all the vegetables
  doContextQuery( $theMgr, $container->getName(), "/vegetables:item", $context);

  # Find the  dessert called Blueberry Boy Bait
  #  Note the use of a variable
  doContextQuery( $theMgr, $container->getName(),
		  '/desserts:item/product[text()=$aDessert]', $context);

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

