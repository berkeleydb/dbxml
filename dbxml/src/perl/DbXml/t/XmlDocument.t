
use strict;
use warnings;

use Test::More tests => 36;

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


my $mgr = new XmlManager();
ok $mgr, "Created Manager";

{
    # setContent/getContent

    my $doc ;

    $doc = $mgr->createDocument(); 

    ok ! $@, "Creating document doesn't throw an exception" ;
    ok defined $doc, "Document object created ok" ;

    my $content = '<def />';
    $doc->setContent($content);

    my $cont = $doc->getContent();
    ok $cont eq $content, "getContent returned correct content" ;

    $doc->fetchAllData();
}

my $doc ;

eval { $doc = $mgr->createDocument(); };

ok ! $@, "Creating document doesn't throw an exception" ;
ok defined $doc, "Document object created ok" ;

my $content = '<abc />';
$doc->setContent($content);

my $cont = $doc->getContent();

ok $cont eq $content, "getContent returned correct content" ;

is $doc->getName(), '', "setName/getName works ok" ; 

$doc->setName("fred");

is $doc->getName(), 'fred', "setName/getName works ok" ; 

eval
{ 
    # getMetaData/setMetaData/removeMetaData

    #my $value1 = new XmlValue(XmlValue::DECIMAL, 42) ;
    my $value1 = new XmlValue(XmlValue::STRING, "Fred") ;
    #my $value2 = new XmlValue(XmlValue::DECIMAL, 43) ;
    my $value2 = 43 ;
    my $valueb = new XmlValue(XmlValue::BOOLEAN, 1) ;

    my $uri = 'http://fred.com/';
    my $name1 = 'universe' ;
    my $name2 = 'everything' ;
    my $name3 = 'else' ;

    my %names = map {$_ => 1 } $name1, $name2, $name3 ;

    $doc->setMetaData($uri, $name1, $value1) ;
    $doc->setMetaData($uri, $name2, $value2) ;
    $doc->setMetaData($uri, $name3, $valueb) ;

    {
        my $Uri;
        my $name;
        my $value = new XmlValue() ;
        #my $value ;
        my $iter = $doc->getMetaDataIterator();
        while ($iter->next($Uri, $name, $value))
        {
            print "# $Uri $name\n" ;
		}
        $iter->reset();
        while ($iter->next($Uri, $name, $value))
        {
            next if $name eq 'name';
            my $str = $value->isNull();
            #print "# $Uri $name $value $str\n" ;

            is $Uri, $uri, "URI is $uri";
            ok $names{$name}, "Matched '$name'"
        }

        $iter->reset();
    }

    my $typed = new XmlValue() ;
    my $untyped ;

    # try to fetch an unknown bit of meta data
    ok !$doc->getMetaData($uri."x", "xx who", $typed), "getMetaData returns FALSE" ;

    ok $doc->getMetaData($uri, $name1, $typed), "getMetaData into XmlValue returns TRUE" ;
    ok $typed->isString(), "getMetaData returned a number";
    my $got = $typed->asString();
    is $got, 'Fred', "getMeta returns 'Fred'";

    ok $doc->getMetaData($uri, $name1, $untyped), "getMetaData into Perl scalar returns TRUE" ;
    is $untyped, "Fred\0", "getMeta returns 'Fred'";
    #print "AOK\n" if $untyped == 42 ;

    ok $doc->getMetaData($uri, $name2, $untyped), "getMetaData into Perl scalar returns TRUE" ;
    is $untyped, "43\0", "getMeta returns 43";


    ok $doc->getMetaData($uri, $name2, $typed), "getMetaData into XmlValue returns TRUE" ;
    $got = $typed->asString();
    is $got, 43, "getMeta returns 43";

    ok $doc->getMetaData($uri, $name3, $typed), "getMetaData into XmlValue returns TRUE" ;
    ok $typed->isBoolean(), "getMetaData returned a Boolean";
    $got = $typed->asBoolean();
    ok $got, "getMeta returns 'true'";

    ok $doc->getMetaData($uri, $name3, $untyped), "getMetaData into Perl scalar returns TRUE" ;
    is $untyped, "true\0", "getMeta returns 'true'";

    # now remove one of the bits of meta-data


    $doc->removeMetaData($uri, $name2);

    ok ! $doc->getMetaData($uri, $name2, $typed), "getMetaData returns FALSE" ;

}
or dumpException();


{ 
    # getMetaData with NULL XmlValue should not throw exception

    my $uri = 'http://fred.com/';
    my $name = 'hi' ;
    my $value1 = new XmlValue() ;

    eval { $doc->getMetaData($uri, $name, $value1) ; };
    ok !$@, "getMetaData with NULL XmlValue should not throw exception";

}

{ 

    # setMetaData with NULL XmlValue should throw exception

    my $value1 = new XmlValue() ;

    my $uri = 'http://fred.com/';
    my $name1 = 'blank' ;

    eval { $doc->setMetaData($uri, $name1, $value1); } ;
    ok $@, "setMetaData with NULL XmlValue should throw exception";

}

{ 
    # setMetaData with NODE XmlValue should throw exception

    my $doc2 = $mgr->createDocument(); 
    my $value1 = new XmlValue($doc2) ;

    my $uri = 'http://fred.com/';
    my $name1 = 'foo' ;

    eval { $doc->setMetaData($uri, $name1, $value1) } ;
    ok $@, "setMetaData with NODE XmlValue should throw exception";
}

