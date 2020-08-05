
use strict;
use warnings;

use Test::More tests => 32 ;
BEGIN { use_ok('Sleepycat::DbXml') };
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

my $doc ;


my $mgr ;
eval { $mgr = new XmlManager(); };

ok ! $@, "Creating manager doesn't throw an exception" ;
ok defined $mgr, "Manager object created ok" ;


my $context = $mgr->createQueryContext() ;
$context->setQueryTimeoutSeconds(3);
my $timeout = $context->getQueryTimeoutSeconds();
is $timeout, 3, "timeout is 3";


{
    # Variables that are DECIMAL

    my $value1 = new XmlValue(XmlValue::DECIMAL, 42) ;
    my $value1a = new XmlValue(XmlValue::DECIMAL, 43) ;

    $context->setVariableValue("fred", $value1);
    $context->setVariableValue("joe", $value1a);

    my $value0 ;
    $context->getVariableValue("joe", $value0);
    is $value0, 43;

    my $value2 = new XmlValue();
    $context->getVariableValue("xxy", $value2);

    ok $value2->isNull(), "unknown variable returns NULL";

    $context->getVariableValue("fred", $value2);

    ok $value2->isNumber(), "isNumber";
    is $value2->asNumber(), 42;

    $context->getVariableValue("joe", $value2);
    
    ok $value2->isNumber(), "isNumber";
    is $value2->asNumber(), 43;
}

{
    # Variables that are BOOLEAN

    my $value1 = new XmlValue(XmlValue::BOOLEAN, 1) ;
    my $value1a = new XmlValue(XmlValue::BOOLEAN, 0) ;

    $context->setVariableValue("bool_fred", $value1);
    $context->setVariableValue("bool_joe", $value1a);

    my $value2 = new XmlValue();

    $context->getVariableValue("bool_fred", $value2);

    ok $value2->isBoolean(), "isBoolean";
    ok $value2->asBoolean();

    $context->getVariableValue("bool_joe", $value2);
    
    ok $value2->isBoolean(), "isBoolean";
    ok ! $value2->asBoolean();
}

{
    # Variables that are STRING

    my $value1 = new XmlValue(XmlValue::STRING, "alpha") ;
    my $value1a = 'beta' ;

    $context->setVariableValue("str_fred", $value1);
    $context->setVariableValue("str_joe", $value1a);

    my $value2 = new XmlValue();

    $context->getVariableValue("str_fred", $value2);

    ok $value2->isString(), "isString";
    is $value2->asString(), "alpha";

    $context->getVariableValue("str_joe", $value2);
    
    ok $value2->isString(), "isString";
    is $value2->asString(), "beta";
}

eval
{
    # Variables that are XmlResults

    my $results = $mgr->createResults();

    $context->setVariableValue("str_fred", $results);

    #my $value2 = new XmlValue();
    my $value2 = $mgr->createResults();

    $context->getVariableValue("str_fred", $value2);


} or dumpException();

eval
{
    # Variables that are empty

    my $value3 = new XmlValue() ;

    eval { $context->setVariableValue("var_fred3", $value3); };

    if (my $E = catch XmlException)
    {
        ok 1, "setVariableValue with null value does not throw exception";
        diag "Exception is " . $E->what();

    }
    else
    {
        print $@;
        ok 1, "setVariableValue with null does not throw exception";
    }
} or dumpException();

{
    my $prefix1 = "my_prefix1" ;
    my $uri1 = "my_uri1" ;
    my $prefix2 = "my_prefix2" ;
    my $uri2 = "my_uri2" ;

    $context->setNamespace($prefix1, $uri1);
    $context->setNamespace($prefix2, $uri2);

    is $context->getNamespace("fred"), "", "getNamespace ok";
    $context->removeNamespace("fred");

    is $context->getNamespace($prefix1), $uri1, "getNamespace ok";
    is $context->getNamespace($prefix2), $uri2, "getNamespace ok";

    $context->removeNamespace($prefix1);
    is $context->getNamespace($prefix1), "", "getNamespace ok";
    is $context->getNamespace($prefix2), $uri2, "getNamespace ok";

    $context->clearNamespaces();
    is $context->getNamespace($prefix1), "", "getNamespace ok";
    is $context->getNamespace($prefix2), "", "getNamespace ok";

    $context->setEvaluationType(XmlQueryContext::Eager);
    is $context->getEvaluationType(), XmlQueryContext::Eager;

    $context->setEvaluationType(XmlQueryContext::Lazy);
    is $context->getEvaluationType(), XmlQueryContext::Lazy;

    $context->setReturnType(XmlQueryContext::LiveValues);
    is $context->getReturnType(), XmlQueryContext::LiveValues;

    # DeadValues doesn't exist anymore
    #$context->setReturnType(XmlQueryContext::DeadValues);
    #is $context->getReturnType(), XmlQueryContext::DeadValues;

    my $uri = 'file:///fred';
    $context->setBaseURI($uri);
    is $context->getBaseURI(), $uri, "getBaseURI";

    $context->setDefaultCollection("http://abcd");
    is $context->getDefaultCollection(), "http://abcd", "getDefaultCollection";
}

