
use strict;
use warnings;

use Test::More tests => 13 ;
BEGIN { use_ok('Sleepycat::DbXml') };
BEGIN { use_ok('File::Path') };

my $mgr = new XmlManager;

sub listIndexes
{
    my $ix = shift ;

    $ix->reset() ;
    my $uri;
    my $name;
    my $index;
    while ($ix->next($uri, $name, $index))
    {
        print "URI [$uri] NAME [$name] INDEX [$index]\n" ;
    }
}

{
    my $ix = new XmlIndexSpecification();

    ok $ix, "created IndexSpecification";

    my $default = 'node-element-equality-string';
    my $replace = 'node-element-presence-none';

    $ix->addDefaultIndex($default);
    is $ix->getDefaultIndex(), $default;
    $ix->replaceDefaultIndex($replace);
    is $ix->getDefaultIndex(), $replace;
    $ix->deleteDefaultIndex($replace);
    is $ix->getDefaultIndex(), '';

    my $uri = 'http://hello';
    my $name = 'element1';
    my $index = $default;
    my $getindex;

    ok ! $ix->find($uri, $name, $getindex), "find not ok";
    $ix->addIndex($uri, $name, $index);

    ok $ix->find($uri, $name, $getindex), "find ok";
    is $getindex, $index, "index strategy ok";

    $ix->replaceIndex($uri, $name, $replace);
    ok $ix->find($uri, $name, $getindex), "find ok";
    is $getindex, $replace, "index strategy ok";

    $ix->deleteIndex($uri, $name, $replace);
    ok ! $ix->find($uri, $name, $getindex), "find not ok";
}

{
    # getValueType

    my $got = XmlIndexSpecification::getValueType("node-element-equality-decimal");
    is $got, XmlValue::DECIMAL;


}
