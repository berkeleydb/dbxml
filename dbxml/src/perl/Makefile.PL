use 5.006001;
use ExtUtils::MakeMaker;
use File::Copy;
use Cwd;
#use lib '.';

require "util" ;

parseConfig('.');

WriteMakefile(
    'NAME'		=> 'Sleepycat::DbXml',
    'VERSION_FROM'	=> 'DbXml/DbXml.pm', 
    ($] >= 5.005 ?    
      (ABSTRACT => 'Berkeley DB XML Perl Interface',
       AUTHOR     => 'Paul Marquess <pmqs@cpan.org>') : ()),
    MAN3PODS 		=> {},

);

foreach my $dir ( qw( Db DbXml ) )
{
    mkdir "$dir/Test" ;
    copy "Test/More.pm", "$dir/Test/More.pm" ;
    copy "Test/Builder.pm", "$dir/Test/Builder.pm" ;
}
