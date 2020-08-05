
use strict;
use warnings;
use Sleepycat::DbXml 'simple';


sub usage()
{
    warn  <<EOM;
This program adds a few simple XML documents to container 'simpleExampleData.dbxml'
You are only required to pass this program the path to where you want the
database environment to be located:

    -h <dbenv directory>
EOM
        exit( -1 );
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

# open a container in the db environment
my $env = new DbEnv(0);
$env->set_cachesize(0, 64 * 1024 * 1024, 1);
$env->open($path2DbEnv,
            Db::DB_INIT_MPOOL|Db::DB_CREATE|Db::DB_INIT_LOCK|Db::DB_INIT_LOG|Db::DB_INIT_TXN);
my $theMgr = new XmlManager($env);

my $containerTxn = $theMgr->createTransaction();
my $container = $theMgr->openContainer($containerTxn, $theContainer);
$containerTxn->commit();

#  Get an XmlUpdateContext. Useful from a performance perspective.
my $updateContext = $theMgr->createUpdateContext();

# myDbEnv and myXmlContainer open with transactions. All subsequent
# writes to them must also be performed inside a transaction.

# Get a transaction
my $txn = $theMgr->createTransaction();

my $document1 = "<aDoc><title>doc1</title><color>green</color></aDoc>";
my $document2 = "<aDoc><title>doc2</title><color>yellow</color></aDoc>";


# Add the documents
my $myXMLDoc = $theMgr->createDocument();

# Set the XmlDocument to the relevant string and then put it 
#  into the container.
$myXMLDoc->setContent($document1 );
$container->putDocument($txn, $myXMLDoc, $updateContext, DbXml::DBXML_GEN_NAME);

# do it again for the second document
$myXMLDoc->setContent($document2 );
$container->putDocument($txn, $myXMLDoc, $updateContext, DbXml::DBXML_GEN_NAME);

# Normally we would use a try/catch block to trap any exceptions.
#  In the catch, we should call txn->abort() to avoid leaving the
#  database in an indeterminate state in the event of an error.
#  However, this simple example avoids error handling so as to 
#  highlite basic concepts, so that step if omitted here as well.

# Commit the writes. This causes the container write operations
#   to be saved to the container.
$txn->commit();

