
use strict;
use warnings;

use Test::More tests => 44 ;
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

sub check_results
{
    my $results = shift ;
    my $count   = shift ;
    my $doc_name = shift || "";
    my $node_name = shift || "";

    ok 1, "Check Results";
    my $value = new XmlValue ;
    my $c = 0;
    while ($results->next($value))
    {
        ++ $c ;
        my $doc = $value->asDocument();
        my $name = $doc->getName();
        is $name, $doc_name, "  document '$doc_name'"
            if $doc_name;
         my $nodeName = $value->getNodeName() ;
         is $nodeName, $node_name, "  node name $node_name"
            if $node_name;
    }

    is $c, $count, "  found $count results";

}

eval
{
    # XmlIndexLookup
    # from xml010_1_6_4

    my $mgr = new XmlManager;
    my $container_name = "$tmp_dir/freed2" ;
    my $cont = $mgr->createContainer($container_name, DbXml::DBXML_INDEX_NODES); 
    my $qc = $mgr->createQueryContext();
    my $uc = $mgr->createUpdateContext();

    my $contentA = <<EOM;
        <docA> 
          <foo>hello</foo>
          <foo>charlie</foo>
          <foo>brown</foo>
          <foo>aNd</foo>
          <foo>Lucy</foo>
        </docA> 
EOM

    my $contentB = <<EOM;
        <docB xmlns:bar='http://www.example.com/schema'> 
          <bar:foo>hello</bar:foo>
          <bar:foo>charlie</bar:foo>
          <bar:foo>brown</bar:foo>
          <bar:foo>aNd</bar:foo>
          <bar:foo>Lucy</bar:foo>
        </docB> 
EOM
    
    my $contentC = <<'EOM';
        <docC> 
          <foobar>
            <baz len='6.7'>tall guy</baz>
            <baz len='75'>30 yds</baz>
            <baz len='75'>30 yds</baz>
            <baz len='5.0'>five feeet</baz>
            <baz len='0.2'>point two</baz>
            <baz len='60.2'>five feet</baz>
          </foobar>
        </docC> 
EOM

    my $contentD = <<EOM;
        <docD> 
         <dates1>
          <date>2005-08-02</date>
          <date>2003-06-12</date>
          <date>1005-12-12</date>
         </dates1>
         <dates2>
          <date>1492-05-30</date>
          <date>2000-01-01</date>
          <date>1984-12-25</date>
         </dates2>
        </docD> 
EOM

    
    $cont->addIndex('', 'foo', 'node-element-equality-string', $uc);
    $cont->addIndex('http://www.example.com/schema', 'foo', 'node-element-equality-string', $uc);
    $cont->addIndex('', 'foo', 'node-element-presence', $uc);
    $cont->addIndex('http://www.example.com/schema', 'foo', 'node-element-presence', $uc);
    $cont->addIndex('', 'len', 'edge-attribute-equality-decimal', $uc);
    $cont->addIndex('', 'len', 'edge-attribute-presence', $uc);
    $cont->addIndex('', 'date', 'edge-element-equality-date', $uc);
	
    
    $cont->putDocument('docA', $contentA, $uc);
    $cont->putDocument('docB', $contentB, $uc);
    $cont->putDocument('docC', $contentC, $uc);
    $cont->putDocument('docD', $contentD, $uc);

    {
        # node presence 

        my $lookup = $mgr->createIndexLookup($cont, "", "foo", "node-element-presence");

        my $results = $lookup->execute($qc);

        check_results($results, 5, 'docA', 'foo');


        $lookup->setNode("http://www.example.com/schema", $lookup->getNodeName());
        $results = $lookup->execute($qc);
        check_results($results, 5, 'docB', 'bar:foo');

        is $lookup->getNodeURI(), 'http://www.example.com/schema', 'getNodeURI';
        
        $lookup->setIndex('node-element-presence');
        is $lookup->getIndex(), 'node-element-presence', 'getIndex';
    }

    {
        my $v = new XmlValue(XmlValue::DATE, "2003-06-12");
        my $lookup = $mgr->createIndexLookup($cont, "", "date", "edge-element-equality-date", $v, XmlIndexLookup::EQ);

        $lookup->setParent('','dates1');
        is $lookup->getParentName(), 'dates1', 'getParentName';
        is $lookup->getParentURI(), '', 'getParentURI';
        
        my $results = $lookup->execute($qc);

        check_results($results, 1, 'docD', 'date');
    }

    {
        my $v = new XmlValue(XmlValue::DECIMAL, 40.0);
        my $lookup = $mgr->createIndexLookup($cont, "", "len", "edge-attribute-equality-decimal");
        $lookup->setParent('','baz');

        $lookup->setLowBound($v, XmlIndexLookup::LT);

        is $lookup->getLowBoundOperation(), XmlIndexLookup::LT, 'getLowBoundOperation';

        $lookup->setNode("", $lookup->getNodeName());
        
        my $results = $lookup->execute($qc);

        check_results($results, 3, 'docC', 'len');

        is $lookup->getContainer()->getName, $cont->getName(), "getContainer";
    }
}
 or dumpException();
