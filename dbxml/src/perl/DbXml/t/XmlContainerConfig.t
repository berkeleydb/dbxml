
use strict;
use warnings;

use Test::More tests => 28 ;
BEGIN { use_ok('Sleepycat::DbXml', 'simple') };
BEGIN { use_ok('File::Path') };

sub dumpException
{
    if (my $e = catch std::exception)
    {
        warn "caught exception ";
        warn $e->what() . "\n";
        exit( -1 );
    }
    elsif ($@)
    {
        warn "failed\n";
        warn $@;
        exit( -1 );
    }    
}

my $mgr ;
eval { $mgr = new XmlManager(); };

ok ! $@, "Creating manager doesn't throw an exception" ;
ok defined $mgr, "Manager object created ok" ;

{
    my $config = new XmlContainerConfig();

    $config->setContainerType(XmlContainer::NodeContainer);
    is $config->getContainerType, XmlContainer::NodeContainer;

    $config->setMode(0777);
    is $config->getMode(), 0777;

    $config->setPageSize(1024);
    is $config->getPageSize(), 1024;

    $config->setSequenceIncrement(2);
    is $config->getSequenceIncrement(), 2;

    $config->setIndexNodes(XmlContainerConfig::On);
    is $config->getIndexNodes(), XmlContainerConfig::On;

    $config->setStatistics(XmlContainerConfig::On);
    is $config->getStatistics(), XmlContainerConfig::On;

    $config->setChecksum(1);
    ok $config->getChecksum();

    $config->setEncrypted(1);
    ok $config->getEncrypted();

    $config->setAllowValidation(1);
    ok $config->getAllowValidation();

    $config->setTransactional(1);
    ok $config->getTransactional();

    $config->setAllowCreate(1);
    ok $config->getAllowCreate();

    $config->setExclusiveCreate(1);
    ok $config->getExclusiveCreate();

    $config->setNoMMap(1);
    ok $config->getNoMMap();

    $config->setReadOnly(1);
    ok $config->getReadOnly();

    $config->setMultiversion(1);
    ok $config->getMultiversion();

    $config->setReadUncommitted(1);
    ok $config->getReadUncommitted();

    $config->setThreaded(1);
    ok $config->getThreaded();

    $config->setTransactionNotDurable(1);
    ok $config->getTransactionNotDurable();

}    


my $tmp_dir = "tmp" ;

if (-d $tmp_dir)
{
    rmtree($tmp_dir);
}

mkdir $tmp_dir, 0777 ;

END {  rmtree($tmp_dir); }

    my $config2 = new XmlContainerConfig();
    $config2->setContainerType(XmlContainer::WholedocContainer);
    $config2->setAllowCreate(1);
    $mgr->setDefaultContainerConfig($config2);
  
    my $container_name = "$tmp_dir/test.dbxml" ;
    #will fail unless AllowCreate is set to true
    my $cont = $mgr->openContainer($container_name);
    ok defined $cont, "Container object okay" ;
    is $cont->getContainerType(), XmlContainer::WholedocContainer;
    my $config3 = $cont->getContainerConfig();
    is $config3->getContainerType(), XmlContainer::WholedocContainer;

    my $container_name2 = "$tmp_dir/test2.dbxml";
    my $cont2 = $mgr->openContainer($container_name2, $config2);
    ok defined $cont2, "Container object okay" ;
    is $cont2->getContainerType(), XmlContainer::WholedocContainer;
    my $config4 = $cont->getContainerConfig();
    is $config4->getContainerType(), XmlContainer::WholedocContainer;

