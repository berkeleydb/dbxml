
use strict;
use warnings;

use Test::More tests => 92 ;
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

my $tmp_dir = "tmp" ;

if (-d $tmp_dir)
{
    rmtree($tmp_dir);
}

mkdir $tmp_dir, 0777 ;

END {  rmtree($tmp_dir); }

my $mgr ;
my $doc ;

eval { $mgr = new XmlManager(); };

ok ! $@, "Creating manager doesn't throw an exception" 
    or dumpException() ;
ok defined $mgr, "Manager object created ok" ;

eval { $doc = $mgr->createDocument(); };

is $@, '', "Creating document doesn't throw an exception" ;
ok defined $doc, "document object created ok" ;


my $title = '<title>Knowledge Discovery in Databases.</title>';
my $content = "<book>$title</book>";
$doc->setContent($content);



ok $doc->getContent() eq $content, "getContent returned correct content" ;
ok $doc eq $content, "getContent returned correct content" ;

$doc->setName("fred");

is $doc->getName(), 'fred', "setName/getName works ok" ; 

is $mgr->getHome(), "", "getHome returns empty string with no env";


my $container_name = "$tmp_dir/fred" ;
my $exists = $mgr->existsContainer($container_name); 
ok ! $exists, "Container file does not exist" ;
ok ! -e $container_name, "Container file does not exists" ;
my $cont = $mgr->createContainer($container_name); 
ok $cont, "createContainer returned object" ;

ok -e $container_name, "Container file exists" ;

is $cont->getName(), $container_name, "Container getName";

undef $cont;


my $new_container = "$tmp_dir/joe";
$mgr->renameContainer($container_name, $new_container); 
$cont = $mgr->openContainer($new_container); 
is $cont->getName(), $new_container, "Container getName after open";

$exists = $mgr->existsContainer($new_container); 
ok $exists, "Container exists" ;
ok !-e $container_name, "Container file exists" ;
ok -e $new_container, "Container file exists" ;

undef $cont;

$mgr->removeContainer($new_container); 
ok !-e $container_name, "Container file exists" ;
ok ! -e $new_container, "Container file exists" ;

$cont = $mgr->createContainer($container_name); 
ok $cont, "createContainer returned object" ;

$title = '<title>Knowledge Discovery in Databases.</title>';
$content = "<book>$title</book>";
$cont->putDocument('x', $content);


{
    # setDefaultContainerFlags & getDefaultContainerFlags
    my $mgr = new XmlManager ;

    is $mgr->getFlags(), 0, "getFlags is 0";
    $mgr->setImplicitTimezone(4);
    is $mgr->getImplicitTimezone(), 4, ;

    $mgr->setDefaultContainerFlags(DbXml::DBXML_INDEX_NODES) ;

    
    is $mgr->getDefaultContainerFlags(), DbXml::DBXML_INDEX_NODES,
        "getDefaultContainerFlags returns DBXML_INDEX_NODES";

    $mgr->setDefaultContainerFlags(DbXml::DBXML_INDEX_NODES|DbXml::DBXML_CHKSUM) ;

    is $mgr->getDefaultContainerFlags(), DbXml::DBXML_INDEX_NODES|DbXml::DBXML_CHKSUM,
        "getDefaultContainerFlags returns DBXML_INDEX_NODES|DBXML_CHKSUM";
}

{
    # setDefaultContainerType & getDefaultContainerType
    my $mgr = new XmlManager ;

    $mgr->setDefaultContainerType(XmlContainer::NodeContainer) ;

    is $mgr->getDefaultContainerType(), XmlContainer::NodeContainer, 
        "getDefaultContainerType returns XmlContainer::NodeContainer";

    $mgr->setDefaultContainerType(XmlContainer::WholedocContainer) ;

    is $mgr->getDefaultContainerType(), XmlContainer::WholedocContainer, 
        "getDefaultContainerType returns XmlContainer::WholedocContainer";
}

{
    # createLocalFileInputStream & setContentAsXmlInputStream
    my $mgr = new XmlManager ;
    my $filename = "xml.tmp" ;
    my $content = "<xml> hello </xml>" ;
    open F, ">$filename" || die "Cannot open $filename: $!\n" ;
    print F $content ;
    close F ;

    my $doc = $mgr->createDocument();

    my $stream = $mgr->createLocalFileInputStream($filename) ;

    ok $stream, "XmlStream created ok" ;

    $doc->setContentAsXmlInputStream($stream) ;

    is $doc->getContent(), $content, "Content ok" ;

}

{
    # setDefaultSequenceIncrement & getDefaultSequenceIncrement
    my $mgr = new XmlManager ;

    $mgr->setDefaultSequenceIncrement(55);
    is $mgr->getDefaultSequenceIncrement(), 55, "Sequence ok" ;

}

{
    # XmlContainer
    my $mgr = new XmlManager ;
    my $filename = "xml.tmp" ;
    my $content = "<xml> hello </xml>" ;
    my $content2 = "<xml> bye </xml>" ;
    my $content3 = "<xml> xyz </xml>" ;
    my $container_name = "$tmp_dir/joe";

    $mgr->setDefaultContainerType(XmlContainer::WholedocContainer) ;

    my $doc = $mgr->createDocument();
    $doc->setContent($content);
    $doc->setName('pling');
    my $cont = $mgr->createContainer($container_name); 

    $cont->addDefaultIndex('node-element-equality-string');
    my $ix = $cont->getIndexSpecification();
    $cont->replaceDefaultIndex('node-element-presence-none');
    $cont->deleteDefaultIndex('node-element-presence-none');

    $cont->sync();
    is $cont->getContainerType(), XmlContainer::WholedocContainer, "Container Type is XmlContainer::WholedocContainer" ;

    $cont->putDocument($doc) ;
    is $cont->putDocument('joey', $content2), 'joey' ;

    my $doc1 = $cont->getDocument('pling');
    is $doc1->getContent, $content;

    my $doc2 = $cont->getDocument('joey');
    is $doc2->getContent, $content2;

    $doc2->setContent($content3);
    $cont->updateDocument($doc2);

    $cont->deleteDocument('joey');
    $cont->deleteDocument($doc);

    eval { $cont->getDocument('pling'); };
    if (my $E = catch XmlException)
    {
        ok 1, "getDocument on deleted doc throws exception";
        is $E->getExceptionCode(), XmlException::DOCUMENT_NOT_FOUND, "exception is DOCUMENT_NOT_FOUND";

    }
    else
    {
        print $@;
        ok 0, "getDocument on deleted doc throws exception";
        ok 0;
    }

    eval { $cont->getDocument('joey'); };
    if (my $E = catch XmlException)
    {
        ok 1, "getDocument on deleted doc throws exception";
        is $E->getExceptionCode(), XmlException::DOCUMENT_NOT_FOUND, "exception is DOCUMENT_NOT_FOUND";

    }
    else
    {
        print $@;
        ok 0, "getDocument on deleted doc throws exception";
        ok 0;
    }

    undef $cont;

    #$mgr->removeContainer($container_name); 
}

{
    # XmlInputStream and putDocument
    my $mgr = new XmlManager ;
    my $filename = "xml1.tmp" ;
    my $content = "<xml> hello </xml>" ;
    open F, ">$filename";
    print F $content;
    close F;
    my $container_name = "$tmp_dir/joex";

    my $cont = $mgr->createContainer($container_name); 
    my $stream = $mgr->createLocalFileInputStream($filename);

    ok $stream, "Created Stream" ;
    is $cont->putDocument('joey', $stream), 'joey' ;

    my $doc1 = $cont->getDocument('joey');
    is $doc1->getContent, $content;

}

eval
{
    # bad XmlInputStream and putDocument
    my $mgr = new XmlManager ;
    my $filename = "no-such-file";
    my $content = "<xml> hello </xml>" ;
    my $container_name = "$tmp_dir/joes";

    my $cont = $mgr->createContainer($container_name); 
    my $stream = $mgr->createLocalFileInputStream($filename);

    ok $stream, "Created Stream" ;
    eval { $cont->putDocument('joey', $stream) };

    if (my $E = catch XmlException)
    {
        ok 1, "putDocument with bad XmlInputStream throws exception";
        is $E->getExceptionCode(), XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
    }
    else
    {
        print $@;
        ok 0, "putDocument with bad XmlInputStream throws exception";
        ok 0;
    }

};
if (my $e = catch std::exception)
{
    warn "failed\n";
    warn $e->what() . "\n";
    exit( -1 );
}
elsif ($@)
{
    warn "failed\n";
    warn $@;
    exit( -1 );
}    


eval
{
    # setIndexSpecification -- opened container without docs is ok

    my $mgr = new XmlManager ;
    my $filename = "xml.tmp" ;
    my $content = "<dummy> hello </dummy>" ;
    my $content2 = "<dummy> bye </dummy>" ;
    my $content3 = "<dummy> xyz </dummy>" ;
    my $container_name = "$tmp_dir/joe1";

    {
        my $cont = $mgr->createContainer($container_name); 

        is $cont->getIndexNodes(), 1, "getIndexNodes";
        my $indexSpec = new XmlIndexSpecification();
        my $Uri = "http://xyz.com/";
        my $Name = "dummy";
        my $Index = "node-attribute-presence-none";
        $indexSpec->addIndex($Uri, $Name, $Index);

        
        my $uri ;
        my $name ;
        my $index ;

        eval { $cont->setIndexSpecification($indexSpec) ; } ;

        if (my $E = catch XmlException)
        {
            ok 0, "setIndexSpecification on opened container is ok";
        warn "# exception says: " . $E->what() ;

        }
        else
        {
            ok 1, "setIndexSpecification on opened container is ok";
        }

        my $ix = $cont->getIndexSpecification(); 

        ok $ix->next($uri, $name, $index), 
            "next returns true";

        is $index, $Index, "next index 1 is '$Index'";
        is $name, $Name, "next name is '$Name'";
        is $uri, $Uri, "next uri is '$Uri'";


        my $stats = $cont->lookupStatistics($Uri, $Name, $Index);
        my $stats1 = $cont->lookupStatistics($Uri, $Name, "parent", "parent", $Index);
        my $k = $stats->getNumberOfIndexedKeys();
        my $u = $stats->getNumberOfUniqueKeys();
        ok defined($k), "getNumberOfIndexedKeys";
        ok defined($u), "getNumberOfUniqueKeys";
    }


    my $context = $mgr->createUpdateContext() ;
    my $config = new XmlContainerConfig();
    $config->setStatistics(XmlContainerConfig::Off);
    $mgr->reindexContainer($container_name);
    $mgr->reindexContainer($container_name, $context);
    $mgr->reindexContainer($container_name, $context, $config);

};
if (my $e = catch std::exception)
{
    warn "failed\n";
    warn $e->what() . "\n";
    exit( -1 );
}
elsif ($@)
{
    warn "failed\n";
    warn $@;
    exit( -1 );
}    





eval
{
    # dump and load 

    undef $cont;
    $mgr->dumpContainer($container_name, "$tmp_dir/cont");

    ok -e "$tmp_dir/cont", "dumped content exists";
    my $new_container_name = "$tmp_dir/new";

    $mgr->loadContainer($new_container_name, "$tmp_dir/cont");

    my $c1 = $mgr->openContainer($new_container_name);

    my $doc = $c1->getDocument('x') ;
    my $cont = $doc->getContent();
    is $cont, $content, "getContent ok" ;
    unlink "$tmp_dir/cont";
};
if (my $e = catch std::exception)
{
    warn "failed\n";
    warn $e->what() . "\n";
    exit( -1 );
}
elsif ($@)
{
    warn "failed\n";
    warn $@;
    exit( -1 );
}    


0 && eval
{
    # verify and load

    undef $cont;
    #$mgr->verifyContainer($container_name, "$tmp_dir/verify", Db::DB_SALVAGE|Db::DB_AGGRESSIVE);
    $mgr->verifyContainer($container_name, "$tmp_dir/verify", Db::DB_SALVAGE);
    ok -e "$tmp_dir/verify", "dumped content exists";
    my $new_container_name = "$tmp_dir/new";

    $mgr->loadContainer($new_container_name, "$tmp_dir/verify");

    my $c1 = $mgr->openContainer($new_container_name);

    my $doc = $c1->getDocument('x') ;
    my $cont = $doc->getContent();
    is $cont, $content, "getContent ok" ;
    unlink "$tmp_dir/cont";
    unlink "$tmp_dir/verify";

} ;
if (my $e = catch std::exception)
{
    warn "failed\n";
    warn $e->what() . "\n";
    exit( -1 );
}
elsif ($@)
{
    warn "failed\n";
    warn $@;
    exit( -1 );
}    

eval
{
    # compact

    undef $cont;
    my $context = $mgr->createUpdateContext();
    $mgr->compactContainer($container_name, $context);
    $mgr->truncateContainer($container_name, $context);
} 
or dumpException();

{
    # addAlias/removeAlias

    my $mgr = new XmlManager;
    my $container_name = "$tmp_dir/freed2" ;
    my $cont = $mgr->createContainer($container_name); 
    ok $cont, "createContainer returned object" ;

    ok ! $cont->removeAlias("fred"), "remove non-existant alias returns false";
    ok   $cont->addAlias("fred"), "add non-existant alias returns true";
    ok ! $cont->addAlias("fred"), "add existant alias returns false";
    ok   $cont->removeAlias("fred"), "remove existant alias returns true";
}

sub count
{
    my $mgr = shift ;
    my $cont = shift ;
    my @expect = @_ ;

    my $e = @expect ;

    my $results = $cont->getAllDocuments();
    ok $results, "created XmlResults" ;

    my $doc = $mgr->createDocument();
    my @got = ();
    while ($results->next($doc))
    {
        push @got, $doc->getName();
    }

    is scalar(@got), $e, "Found $e documents";
    is_deeply \@got, \@expect, "Container has '@expect'" ;
    is $cont->getNumDocuments(), $e, "getNumDocuments returned $e";
    
}

eval
{
    # getAllDocuments/getNumDocuments

    my $mgr = new XmlManager;
    my $container_name = "$tmp_dir/freed3" ;
    my $cont = $mgr->createContainer($container_name); 
    ok $cont, "createContainer returned object" ;

    count($mgr, $cont);

    $cont->putDocument('x', '<abc></abc>');
    count($mgr, $cont, 'x');

    $cont->putDocument('y', '<abc></abc>');
    count($mgr, $cont, 'x', 'y');

    $cont->putDocument('z', '<abc></abc>');
    count($mgr, $cont, 'x', 'y', 'z');
};
if (my $e = catch std::exception)
{
    warn "failed\n";
    warn $e->what() . "\n";
    exit( -1 );
}
elsif ($@)
{
    warn "failed\n";
    warn $@;
    exit( -1 );
}    

if(1)
{
    # query - ok

    my $mgr = new XmlManager;
    my $container_name = "$tmp_dir/freed1" ;
    my $cont = $mgr->createContainer($container_name); 
    ok $cont, "createContainer returned object" ;

    my $title = '<title>Knowledge Discovery in Databases.</title>';
    my $content = qq[<book>$title</book>];
    my $docName = 'docname';
    $cont->putDocument($docName, $content);


    my $context = $mgr->createQueryContext() ;
    my $path = qq[collection("] . $cont->getName() . qq[")/book/title];
    #diag "path is $path" ;
    
    for my $x (0, 1)
    {
        my $results;
        if ($x)
        {
            my $expression = $mgr->prepare($path, $context);
            ok $expression, "prepare ok" ;

            is $expression->getQuery(), $path, "getQuery ok";
            $results = $expression->execute($context) ;
            ok $results, 'execute returned results';
        }
        else
        {
             $results = $mgr->query($path, $context)  ;
            ok $results, 'query returned results';
        }

        my $value = new XmlValue ;
        $results->next($value); 
        ok !$value->isNull(), "results->next not null";
        ok !$value->isString(), "results->next is string";
        ok $value->isNode(), "results->next is node";
        is $value->asString(), $title, "nodelist contents ok";

        $results->next($value); 
        ok $value->isNull(), "results->next is null";
    }
    #diag "here";
}

