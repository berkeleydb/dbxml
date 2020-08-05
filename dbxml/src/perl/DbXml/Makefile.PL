use 5.006001;
use ExtUtils::MakeMaker;
use lib '..';

use Config;

require 'util';

if (@ARGV && $ARGV[0] =~ /names/)
{
    my %seen ;
    my @names = map { "*${_}::AUTOLOAD = \\&DbXml::AUTOLOAD;\n" }
                grep { ! /^DbXml$/ }
                grep { ! $seen{$_} ++ }
                map { s#::.*##; $_ }
                grep { /::/ }
                map { $_->{'name'} } 
                getNames() ;
    print @names ;            
    print "\n";

       @names = map { "sub $_ ; \n" }
                map { $_ = "DbXml::$_" unless /::/ ; $_  } 
                map { $_->{'name'} } 
                getNames() ;
    print @names ;            

    # get XmlValue types;
       @names = map { "  XmlValue::$_\t=> '$_',\n" }
                map { /.*::(.*);/; $1 } 
                grep { /DbXml::XmlValue::Type/ } 
                map { $_->{'pre'} } 
                getNames() ;

    print "\n\npackage XmlValue;\n\n";
    print "\n\nour %TypeNameMapping = (\n @names);\n\n";
    print "\n\npackage DbXml;\n\n";
    exit;
}

my %parsed = parseConfig("..");

my $libs = join " ", 
           map { $a = $parsed{$_}; $a = "-l$a" unless $a =~ /^-l/; $a } 
	   qw( DBXML_LIB_NAME BDB_LIB_NAME XERCES_LIB_NAME XQILLA_LIB_NAME) ;
 
my $extra_lib = '';
if ($parsed{EXTRA_LIBS})
{
    $extra_lib = $parsed{EXTRA_LIBS} ;
}
elsif ($^O eq 'solaris' && $Config{'cc'} eq 'cc')
{
    $extra_lib = ' -lCstd -lCrun -lm' ;
}

my $defines = $parsed{EXTRA_DEFINE} || '';
my $ccflags = $parsed{EXTRA_CCFLAGS} || '';


WriteMakefile(
    'NAME'		=> 'Sleepycat::DbXml',
    'VERSION_FROM'	=> 'DbXml.pm', 
    ($] >= 5.005 ?    
      (#ABSTRACT_FROM => 'DbXml.pm', # retrieve abstract from module
       AUTHOR     => 'Paul Marquess <pmqs@cpan.org>') : ()),
    'LIBS'		=> ["$parsed{LIBS} $libs $extra_lib "], 
    'DEFINE'		=> $defines, 
    'INC'		=> $parsed{INCS}, 
    'XSOPT'		=> '-noprototypes -C++ -typemap ../typemap',
    ($^O eq 'MSWin32'
     ?(
       'OPTIMIZE' => '-nologo',
       'CCFLAGS' => $parsed{EXTRA_CCFLAGS},
       'LIBC' => $parsed{EXTRA_LIBS},
       ):
     (
      'CCFLAGS'		=> $ccflags,
      )
     ),
    'macro'		=> { 'CC' => $parsed{CC}, 
    #'LD'	=> '$(CC)' 
                   } ,
);

if  (eval {require ExtUtils::Constant; 1}) {
  # If you edit these definitions to change the constants used by this module,
  # you will need to use the generated const-c.inc and const-xs.inc
  # files to replace their "fallback" counterparts before distributing your
  # changes.
  my @names = getNames();
	  
  ExtUtils::Constant::WriteConstants(
                                     NAME         => 'DbXml',
                                     NAMES        => \@names,
                                     DEFAULT_TYPE => 'UV',
                                     C_FILE       => 'const-c.inc',
                                     XS_FILE      => 'const-xs.inc',
                                  );

}
else {
  use File::Copy;
  use File::Spec;
  foreach my $file ('const-c.inc', 'const-xs.inc') {
    my $fallback = File::Spec->catfile('fallback', $file);
    copy ($fallback, $file) or die "Can't copy $fallback to $file: $!";
  }
}

sub getNames
{
  my @names ;  
  push @names,  map { 
	  { name => "DbXml::$_", 
	    type => 'UV',
	    macro => 1 } 
	 } (

	# enum Flags
    'DBXML_ADOPT_DBENV',
    'DBXML_ALLOW_EXTERNAL_ACCESS',
    'DBXML_ALLOW_AUTO_OPEN',
    'DBXML_ALLOW_VALIDATION',
    'DBXML_TRANSACTIONAL',
    'DBXML_REVERSE_ORDER',
    'DBXML_INDEX_VALUES',
    'DBXML_CHKSUM',
    'DBXML_ENCRYPT',
    'DBXML_GEN_NAME',
    'DBXML_LAZY_DOCS',
    'DBXML_DOCUMENT_PROJECTION',
    'DBXML_NO_AUTO_COMMIT',
    'DBXML_INDEX_NODES',
    'DBXML_NO_INDEX_NODES',
    'DBXML_STATISTICS',
    'DBXML_NO_STATISTICS',
    
    
	# enum LogLevel
	'LEVEL_NONE',
	'LEVEL_DEBUG',
	'LEVEL_INFO',
	'LEVEL_WARNING',
	'LEVEL_ERROR',
	'LEVEL_ALL',


	# enum LogCategory
    'CATEGORY_NONE',
    'CATEGORY_INDEXER',
    'CATEGORY_QUERY',
    'CATEGORY_OPTIMIZER',
    'CATEGORY_DICTIONARY',
    'CATEGORY_CONTAINER',
    'CATEGORY_NODESTORE',
    'CATEGORY_MANAGER',
    'CATEGORY_ALL',

);

  push @names,  map { 
	  { name => $_, 
	    type => 'UV',
	    macro => 1 } 
	 } qw(

	DBXML_VERSION_MAJOR
	DBXML_VERSION_MINOR
	DBXML_VERSION_PATCH

);

  # enum XmlContainer::ContainerType
  push @names,  map { 
	  { name => "XmlContainer::$_", 
            pre => "\tDbXml::XmlContainer::ContainerType tmp = XmlContainer::$_ ;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(

	
	WholedocContainer
	NodeContainer
    );

  # enum XmlContainerConfig::ConfigState
  push @names,  map { 
	  { name => "XmlContainerConfig::$_", 
            pre => "\tDbXml::XmlContainerConfig::ConfigState tmp = XmlContainerConfig::$_ ;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(

	
	On
	Off
        UseDefault
    );

  # enum XmlException::ExceptionCode
  push @names,  map { 
	  { name => "XmlException::$_", 
            pre => "\tDbXml::XmlException::ExceptionCode tmp = XmlException::$_ ;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(

	
	INTERNAL_ERROR
	CONTAINER_OPEN
	CONTAINER_CLOSED
	NULL_POINTER
	INDEXER_PARSER_ERROR
	DATABASE_ERROR
	QUERY_PARSER_ERROR
	QUERY_EVALUATION_ERROR
	LAZY_EVALUATION
	DOCUMENT_NOT_FOUND
	CONTAINER_EXISTS
	UNKNOWN_INDEX
	INVALID_VALUE
	VERSION_MISMATCH
	EVENT_ERROR
	CONTAINER_NOT_FOUND
	TRANSACTION_ERROR
	UNIQUE_ERROR
	NO_MEMORY_ERROR
	);
	

  # enum XmlIndexSpecification::Type
  push @names,  map { 
	  { name => "XmlIndexSpecification::$_", 
            pre => "\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::$_;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(
	
        UNIQUE_OFF 
        UNIQUE_ON 

        PATH_NONE 
        PATH_NODE 
        PATH_EDGE 

        NODE_NONE 
        NODE_ELEMENT 
        NODE_ATTRIBUTE 
        NODE_METADATA 

        KEY_NONE 
        KEY_PRESENCE 
        KEY_EQUALITY 
        KEY_SUBSTRING 

	);

  # enum XmlQueryContext::ReturnType
  push @names,  map { 
	  { name => "XmlQueryContext::$_", 
            pre => "\tDbXml::XmlQueryContext::ReturnType tmp = XmlQueryContext::$_;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(
	
	LiveValues
	);

  # enum XmlQueryContext::EvaluationType
  push @names,  map { 
	  { name => "XmlQueryContext::$_", 
            pre => "\tDbXml::XmlQueryContext::EvaluationType tmp = XmlQueryContext::$_;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(
	
	Eager
	Lazy
	);


  # enum XmlValue::Type
  push @names,  map { 
	  { name => "XmlValue::$_", 
            pre => "\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::$_;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(
	
        NONE
        NODE
        ANY_SIMPLE_TYPE    

        ANY_URI            
        BASE_64_BINARY     
        BOOLEAN            
        DATE               
        DATE_TIME          
        DAY_TIME_DURATION  
        DECIMAL            
        DOUBLE             
        DURATION           
        FLOAT              
        G_DAY              
        G_MONTH            
        G_MONTH_DAY        
        G_YEAR             
        G_YEAR_MONTH       
        HEX_BINARY         
        NOTATION           
        QNAME              
        STRING             
        TIME               
        YEAR_MONTH_DURATION
        UNTYPED_ATOMIC     
        BINARY
            

	);

  # enum XmlValue::NodeType
  push @names,  map { 
	  { name => "XmlValue::$_", 
            pre => "\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::$_;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(
             ELEMENT_NODE 
             ATTRIBUTE_NODE 
             TEXT_NODE 
             CDATA_SECTION_NODE 
             ENTITY_REFERENCE_NODE 
             ENTITY_NODE 
             PROCESSING_INSTRUCTION_NODE 
             COMMENT_NODE 
             DOCUMENT_NODE 
             DOCUMENT_TYPE_NODE 
             DOCUMENT_FRAGMENT_NODE 
             NOTATION_NODE 

	);

  # enum XmlIndexLookup::Operation
  push @names,  map { 
	  { name => "XmlIndexLookup::$_", 
            pre => "\tDbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::$_;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(
	
                NONE
                EQ
                GT
                GTE
                LT
                LTE
	 
	);

  # enum XmlEventType
  push @names,  map { 
	  { name => "XmlEventType::$_", 
            pre => "\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::$_;", 
	    value => "(UV)tmp", 
	    type => 'UV',
	    macro => 1 } 
	 } qw(
	 
            StartElement
            EndElement
            Characters
            CDATA
            Comment
            Whitespace
            StartDocument
            EndDocument
            StartEntityReference
            EndEntityReference
            ProcessingInstruction
            DTD
	);

    return @names ;
}
