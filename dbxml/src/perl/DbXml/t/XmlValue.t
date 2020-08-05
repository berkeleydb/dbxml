
use strict;
use warnings;

use Test::More tests => 275 ;
BEGIN { use_ok('Sleepycat::DbXml') };

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

sub notNode ($)
{
    my $this = shift ;

    eval { $this->getNodeName() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getNodeName - exception is INVALID_VALUE";
    eval { $this->getNodeValue() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getNodeValue - exception is INVALID_VALUE";
    eval { $this->getNamespaceURI() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getNamespaceURI - exception is INVALID_VALUE";
    eval { $this->getPrefix() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getPrefix - exception is INVALID_VALUE";
    eval { $this->getLocalName() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getLocalName - exception is INVALID_VALUE";
    eval { $this->getNodeType() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getNodeType - exception is INVALID_VALUE";
    eval { $this->getParentNode() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getParentNode - exception is INVALID_VALUE";
    eval { $this->getFirstChild() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getFirstChild - exception is INVALID_VALUE";
    eval { $this->getLastChild() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getLastChild - exception is INVALID_VALUE";
    eval { $this->getPreviousSibling() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getPreviousSibling - exception is INVALID_VALUE";
    eval { $this->getNextSibling() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getNextSibling - exception is INVALID_VALUE";
    eval { $this->getAttributes() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getAttributes - exception is INVALID_VALUE";
    eval { $this->getOwnerElement() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "getOwnerElement - exception is INVALID_VALUE";

}

my $URI = 'http://www.w3.org/2001/XMLSchema';

{
    # create XmlValue without XmlManager throws exception
    eval { new XmlValue };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
}

my $manager = new XmlManager() ;
ok $manager, "created XmlManager ok";


eval
{
    # test typeless XmlValue

    my $value = new XmlValue();

    is $value->getType(), XmlValue::NONE, "getType - None" ;
    is $value->getTypeName(), '', "getTypeName - None" ;
    is $value->getTypeURI(), '', "getTypeURI - None" ;
    ok   $value->isType(XmlValue::NONE), "isType";
    ok   $value->isNull(), "isNull";
    ok ! $value->isBoolean(), "! isBoolean";
    ok ! $value->isNumber(), "! isNumber";
    ok ! $value->isString(), "! isString";
    ok ! $value->isNode(), "! isNode";
    ok ! $value->isBinary(), "! isBinary";

    notNode($value);
    
    eval { $value->asBoolean(); };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";

    eval { $value->asNumber(); } ;
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
    eval { $value->asString(); } ;
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
    
    eval { $value->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";

    eval { $value->asBinary() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";

    eval { $value->getNodeName() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";

}
or dumpException();


eval
{
    # test BOOLEAN XmlValue == true

    my $value = new XmlValue(XmlValue::BOOLEAN, 1);

    is $value->getType(), XmlValue::BOOLEAN, "getType - Boolean" ;
    is $value->getTypeName(), 'boolean', "getTypeName - None" ;
    is $value->getTypeURI(), $URI, "getTypeURI" ;
    ok   $value->isType(XmlValue::BOOLEAN), "isType";
    ok ! $value->isNull(), "! isNull";
    ok   $value->isBoolean(), "isBoolean";
    ok ! $value->isNumber(), "! isNumber";
    ok ! $value->isString(), "! isString";
    ok ! $value->isNode(), "! isNode";
    ok ! $value->isBinary(), "! isBinary";

    notNode($value);
    
    ok $value->asBoolean(), "asBoolean returned ok";
    is $value->asNumber(), 1, "asNumber returned ok";
    is $value->asString(), "true", "asString returned ok";

    eval { $value->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
}
or dumpException();

eval
{
    # test BOOLEAN XmlValue == false

    my $value = new XmlValue(XmlValue::BOOLEAN, 0);

    is $value->getType(), XmlValue::BOOLEAN, "getType - Boolean" ;
    is $value->getTypeName(), 'boolean', "getTypeName - None" ;
    is $value->getTypeURI(), $URI, "getTypeURI" ;
    ok   $value->isType(XmlValue::BOOLEAN), "isType";
    ok ! $value->isNull(), "! isNull";
    ok   $value->isBoolean(), "isBoolean";
    ok ! $value->isNumber(), "! isNumber";
    ok ! $value->isString(), "! isString";
    ok ! $value->isNode(), "! isNode";
    ok ! $value->isBinary(), "! isBinary";

    notNode($value);
    
    ok !$value->asBoolean(), "asBoolean returned ok";
    is $value->asNumber(), 0, "asNumber returned ok";
    is $value->asString(), "false", "asString returned ok";

    eval { $value->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
}
or dumpException();

eval
{
    # test DECIMAL XmlValue

    my $number =  new XmlValue(XmlValue::DECIMAL, 43.3);
    is $number->getType(), XmlValue::DECIMAL, "getType - Number" ;
    is $number->getTypeName(), 'decimal', "getTypeName - DECIMAL" ;
    is $number->getTypeURI(), $URI, "getTypeURI" ;
    ok   $number->isType(XmlValue::DECIMAL), "isType";
    ok ! $number->isNull(), "! isNull";
    ok ! $number->isBoolean(), "! isBoolean";
    ok   $number->isNumber(), "isNumber";
    ok ! $number->isString(), "! isString";
    ok ! $number->isNode(), "! isNode";
    ok ! $number->isBinary(), "! isBinary";

    notNode($number);

    ok $number->asBoolean(), "asBoolean returned ok";
    is $number->asNumber(), 43.3, "asNumber returned ok";
    is $number->asString(), "43.3", "asString returned ok";

    eval { $number->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
}
or dumpException();

eval
{
    # test DECIMAL XmlValue with typeName xs:short

    my $number =  new XmlValue($URI,"short", "43");
    is $number->getType(), XmlValue::DECIMAL, "getType - Number" ;
    is $number->getTypeName(), 'short', "getTypeName - short" ;
    is $number->getTypeURI(), $URI, "getTypeURI" ;
    ok   $number->isType(XmlValue::DECIMAL), "isType";
    ok ! $number->isNull(), "! isNull";
    ok ! $number->isBoolean(), "! isBoolean";
    ok   $number->isNumber(), "isNumber";
    ok ! $number->isString(), "! isString";
    ok ! $number->isNode(), "! isNode";
    ok ! $number->isBinary(), "! isBinary";

    notNode($number);

    ok $number->asBoolean(), "asBoolean returned ok";
    is $number->asNumber(), 43, "asNumber returned ok";
    is $number->asString(), "43", "asString returned ok";

    eval { $number->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
}
or dumpException();

eval
{
    # test BINARY XmlValue

    my $number =  new XmlValue(XmlValue::BINARY, "abcd");
    is $number->getType(), XmlValue::BINARY, "getType - Number" ;
    #is $number->getTypeName(), 'BINARY', "getTypeName - BINARY" ;
    eval { $number->getTypeURI() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
    ok   $number->isType(XmlValue::BINARY), "isType";
    ok ! $number->isNull(), "! isNull";
    ok ! $number->isBoolean(), "! isBoolean";
    ok ! $number->isNumber(), "isNumber";
    ok ! $number->isString(), "! isString";
    ok ! $number->isNode(), "! isNode";
    ok   $number->isBinary(), "  isBinary";

    notNode($number);

    is $number->asBinary(), "abcd", "asBinary returned ok";
}
or dumpException();


eval
{
    # test STRING XmlValue

    my $str = 'abc def';
    my $string =  new XmlValue(XmlValue::STRING, $str);
    is $string->getType(), XmlValue::STRING, "getType - String" ;
    is $string->getTypeName(), 'string', "getTypeName - STRING" ;
    is $string->getTypeURI(), $URI, "getTypeURI" ;
    ok   $string->isType(XmlValue::STRING), "isType";
    ok ! $string->isNull(), "! isNull";
    ok ! $string->isNull(), "! isNull";
    ok ! $string->isBoolean(), "! isBoolean";
    ok ! $string->isNumber(), "! isNumber";
    ok   $string->isString(), "isString";
    ok ! $string->isNode(), "! isNode";
    ok ! $string->isBinary(), "! isBinary";

    is $string, $str, 'overload -- stringification' ;
    ok $string eq $str, 'overload -- stringification' ;
    is length $string, length $str, 'overload -- stringification' ;
    ok !!$string, 'overload -- boolean' ;

    notNode($string);

    ok $string->asBoolean(), "asBoolean returned ok";
    is $string->asNumber(), undef, "asNumber returned undef";
    is $string->asString(), "abc def", "asString returned ok";

    eval { $string->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";

}
or dumpException();

eval
{
    # test NODE XmlValue

    my $content = '<title>Knowledge Discovery in Databases.</title>';
    my $document = $manager->createDocument(); 
    $document->setContent($content);
    my $docu =  new XmlValue($document);
    is $docu->getType(), XmlValue::NODE, "getType - Document" ;
    #is $docu->getTypeName(), 'NODE', "getTypeName - NODE" ;
    is $docu->getTypeURI(), '', "getTypeURI - None" ;
    #eval { $docu->getTypeURI() };
    #is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
    # TODO
    #is $docu->getNodeHandle(), '', "getNodeHandle " ;
    ok ! $docu->isNull(), "! isNull";
    ok ! $docu->isBoolean(), "! isBoolean";
    ok ! $docu->isNumber(), "! isNumber";
    ok ! $docu->isString(), "! isString";
    ok   $docu->isNode(), "isNode";
    ok ! $docu->isBinary(), "! isBinary";

    my $got = $docu->asDocument();
    is $got->getContent(), $content, "got content ok";

    is $docu->asBoolean(), 1 ;
    is $docu->asNumber(), "0" ;
    is $docu->asString(), $content, "asString returned ok";
}
or dumpException();

eval
{
    # test XmlValue::equals

    my $string1 =  new XmlValue(XmlValue::STRING,"abc def");
    my $string1a =  $string1 ;
    my $string2 =  new XmlValue(XmlValue::STRING,"abc def");
    ok $string1->equals($string1a), "equals";
    ok $string1->equals($string1a), "equals";
    ok $string1->equals($string2), "equals";
    ok $string1->equals($string2), "equals";
    
    my $num1 =  new XmlValue(XmlValue::DECIMAL, 3);
    my $num2 =  new XmlValue(XmlValue::DECIMAL, 3);
    ok $num1->equals($num2), "equals";
    ok $num1->equals($num2), "equals";
    
}
or dumpException();


eval
{
    # XmlValue with one parameter should be a STRING 

    my $string =  new XmlValue("abc def");
    is $string->getType(), XmlValue::STRING, "getType - String" ;
    ok ! $string->isNull(), "! isNull";
    ok ! $string->isBoolean(), "! isBoolean";
    ok ! $string->isNumber(), "! isNumber";
    ok   $string->isString(), "isString";
    ok ! $string->isNode(), "! isNode";
    ok ! $string->isBinary(), "! isBinary";

    notNode($string);
    ok $string->asBoolean(), "asBoolean returned ok";
    is $string->asNumber(), undef, "asNumber returned undef";
    is $string->asString(), "abc def", "asString returned ok";

    eval { $string->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
}
or dumpException();

eval
{
    # XmlValue copy constructor

    my $str =  "abc def";
    my $string0 =  new XmlValue($str);
    my $string =  new XmlValue($string0);
    is $string->getType(), XmlValue::STRING, "getType - String" ;
    ok ! $string->isNull(), "! isNull";
    ok ! $string->isBoolean(), "! isBoolean";
    ok ! $string->isNumber(), "! isNumber";
    ok   $string->isString(), "isString";
    ok ! $string->isNode(), "! isNode";
    ok ! $string->isBinary(), "! isBinary";

    notNode($string);
    ok $string->asBoolean(), "asBoolean returned ok";
    is $string->asNumber(), undef, "asNumber returned undef";
    is $string->asString(), "abc def", "asString returned ok";

    eval { $string->asDocument() };
    is $@->getExceptionCode, XmlException::INVALID_VALUE, "exception is INVALID_VALUE";
    ok $string->equals($string0);
}
or dumpException();

eval
{
    # give XmlValue constructor too many parameters
    eval { my $x = new XmlValue(1,2,3,4) } ;
    like $@, qr/XmlValue::new expects between 0 & 3 parameters/, 
    	"Giving XmlValue::new too many parameters is trapped";
}
or dumpException();

eval
{
    # NODE navigation

    my $namespaceURI = 'http://myns';
#    my $content = qq[ <?xml version="1.0"?>
#                      <myns:root xmlns:myns="$namespaceURI">
#                        <single fred="joe"> hello world </single>
#                      </myns:root>
#                   ];

    my $first = q[<?xml-stylesheet type='text/xsl' href='href'?>];
    my $content = <<"EOM";
$first
<root xmlns:foo='http://www.example.com/foo'> 
    <grandparent> 
        <parent single="yes"> 
            <child eldest="true">Tom</child> 
            <child>Sally</child>
        </parent>
        <aunt> 
            <cousin>Jack</cousin>
        </aunt> 
    </grandparent> 
    <grandparent> 
        <!-- no children for this grandparent --> 
    </grandparent> 
    <foo:godparent firm='mafia'>Tony</foo:godparent> 
</root> 
EOM
    
    my $document = $manager->createDocument(); 
    $document->setContent($content);
    my $docu =  new XmlValue($document);
    is $docu->getType(), XmlValue::NODE, "getType - Document" ;
    #is $docu->getTypeName(), 'NODE', "getTypeName - NODE" ;
    is $docu->getTypeURI(), '', "getTypeURI - None" ;
    ok $docu->isNode(), "isNode";

    my $got = $docu->asDocument();
    is $got->getContent(), $content, "got content ok";


    is $docu->getNodeName(), '#document', "getNodeName" ;

    my $v = $docu->getNodeValue();
    is $docu->getNodeValue(), '', "getNodeValue" ;
    is $docu->getNamespaceURI(), '', "getNamespaceURI" ;
    is $docu->getPrefix(), '', "getPrefix";
    is $docu->getLocalName(), '', "getLocalName" ;

    is $docu->getNodeType(), XmlValue::DOCUMENT_NODE, "getNodeType" ;
    #is $docu->getParentNode(), '';
    $docu->getParentNode() ; 

    my $f = $docu->getFirstChild();
    #print "# First [$f]\n";
    is "$f", $first, "getFirstChild" ;
    my $l = $docu->getLastChild();
    #print "# Last [$l]\n";
    #is $docu->getLastChild(), 'd', "getLastChild" ;
    #is $docu->getPreviousSibling(), 'd', "getPreviousSibling" ;
    #is $docu->getNextSibling(), "f", "getNextSibling" ;
    ok $docu->getAttributes(), "getAttributes" ;
    #is $docu->getOwnerElement(), "h", "getOwnerElement" ;


}
or dumpException();
