
use strict;
use warnings;

die <<EOM unless @ARGV == 2 ;
Use:
    loadExamplesData.pl <path to dbenv> <path to xml data>
EOM

my $path2dbEnv      = shift  || '';
my $path2data   = shift  || '';

# usage: loadExamplesData.pl <path to dbenv> <path to examples>
if ( ! -d $path2dbEnv )
{
    warn <<EOM;
Error: You must provide a path to the directory where you want to place
the database environment, and this directory must currently exist.
Use:
    loadExamplesData.pl <path to dbenv> <path to xml data>
EOM

	exit 1 ;
}

if ( ! -d $path2data)
{
    warn <<EOM;
Error: You must provide the path to the directory where the Xml test Data
is stored.
Use:  loadExamplesData.sh <path to dbenv> <path to xmlData>
path2data: $path2data
EOM
	exit 1;
}

my $loadCommand="exampleLoadContainer.pl";
print "$loadCommand\n";

die "Error: $loadCommand does not exist.\n"
    if ! -f $loadCommand ;

my @simpleData    = glob "$path2data/simpleData/*.xml" ;
my @namespaceData = glob "$path2data/nsData/*.xml" ;

my $perl = 'perl';

print "Loading container simpleExampleData\n" ;
system "$perl $loadCommand -h $path2dbEnv -c simpleExampleData.dbxml @simpleData";

print "Loading container namespaceExampleData\n";
system "$perl $loadCommand -h $path2dbEnv -c namespaceExampleData.dbxml @namespaceData";
