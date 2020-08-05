use 5.006001;
use ExtUtils::MakeMaker;
use lib '..';
use Config;

require "util";

my %parsed = parseConfig('..');

foreach ($parsed{BDB_INC_PATH}, $parsed{BDB_DBINC_PATH})
{
    s/^\s*//;
    s/\s*$//;
    $incs{$_}++ if $_;
}

$incs = join " ", map {"-I$_" } keys %incs ;

my $extra_lib = '';
if ($parsed{EXTRA_LIBS})
{
    $extra_lib = $parsed{EXTRA_LIBS} ;
}
elsif ($^O eq 'solaris' && $Config{'cc'} eq 'cc')
{
    $extra_lib = ' -lCstd -lCrun' ;
}

my $defines = $parsed{EXTRA_DEFINE} || '';
my $ccflags = $parsed{EXTRA_CCFLAGS} || '';

WriteMakefile(
    'NAME'		=> 'Sleepycat::Db',
    'VERSION_FROM'	=> 'Db.pm', 
    ($] >= 5.005 ?    
      (#ABSTRACT_FROM => 'Db.pm', # retrieve abstract from module
       AUTHOR     => 'Paul Marquess <pmqs@cpan.org>') : ()),
    'LIBS'		=> ["-L$parsed{BDB_LIB_PATH} -l$parsed{BDB_LIB_NAME} $extra_lib"], 
    'DEFINE'		=> $defines, 
    'INC'		=> $incs,
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
    #'macro'		=> { 'CC' => $CC, 'LD'	=> '$(CC)' } ,
    'macro'		=> { 'CC' => $parsed{CC}, 
                    #'LD'	=> '$(CC)' 
                   } ,
);

if  (eval {require ExtUtils::Constant; 1}) {
  # If you edit these definitions to change the constants used by this module,
  # you will need to use the generated const-c.inc and const-xs.inc
  # files to replace their "fallback" counterparts before distributing your
  # changes.
  #

  my @names ;

  open F, "<Db.pm" || die "Cannot open Db.pm: $!\n" ;
  while (<F>)
  {
     last if /^__DATA__/;	  
  }

  while (<F>)
  {
        chomp;
        s/^\s*//;
        s/\s*$//;
        next if /^\s*$/ || /^#/ ;

	my $macro = 1;
	if (s/\s+(\d+\S+)\s*$//)
	{
	    $macro = [enum_Macro($val), "#endif\n"];	
	}

        elsif (s/\s+STRING\s*$//)
	{
             push @names,  
                     { name => "Db::$_",
                       type => 'PVN',
                       value => [$_, length($_)],
                       macro => $macro 
	             };
		
	}
	elsif ( s/\s+DEFINE\s*$//)
	{
	    push @names, { name => "Db::$_", 
		           macro => ["#ifdef $_\n", "#endif\n"],
                           value => "$_",
		         }
	}
	else
	{
            push @names,  
	            { name => "Db::$_", 
                      pre => "\tint tmp = $_;", 
	              value => "(IV)tmp", 
	              macro => $macro 
	            } ;
	}

  }



  ExtUtils::Constant::WriteConstants(
                                     NAME         => 'Db',
                                     NAMES        => \@names,
                                     DEFAULT_TYPE => 'IV',
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

sub enum_Macro
{
    my $str = shift ;
    my ($major, $minor, $patch) = split /\./, $str ;

    my $macro =
    "#if (DB_VERSION_MAJOR > $major) || \\\n" .
    "    (DB_VERSION_MAJOR == $major && DB_VERSION_MINOR > $minor) || \\\n" .
    "    (DB_VERSION_MAJOR == $major && DB_VERSION_MINOR == $minor && \\\n" .
    "     DB_VERSION_PATCH >= $patch)\n" ;

    return $macro;

}


