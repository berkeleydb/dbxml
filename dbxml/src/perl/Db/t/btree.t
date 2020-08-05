#!./perl -w


use strict ;

BEGIN {
    unless(grep /blib/, @INC) {
        chdir 't' if -d 't';
        @INC = '../lib' if -d '../lib';
    }
}

use Sleepycat::Db 'simple'; 
use t::util ;

use Test::More tests => 35;


my $Dfile = "dbhash.tmp";
my $Dfile2 = "dbhash2.tmp";
my $Dfile3 = "dbhash3.tmp";
unlink $Dfile;

umask(0) ;


# Check for invalid parameters
{
    # Check for invalid parameters
    my $db ;
}

# Now check the interface to Btree

eval
{
    my $lex = new LexFile $Dfile ;

    ok my $db = new Db ;

    $db->open(undef, $Dfile, "", Db::DB_BTREE, Db::DB_CREATE );


    # Add a k/v pair
    my $value ;
    my $status ;

    ok $db->put("some key", "some value") == 0  ;
    ok $db->get("some key", $value) == 0 ;
    ok $value eq "some value" ;

    ok $db->put("key", "value") == 0  ;
    ok $db->get("key", $value) == 0 ;
    ok $value eq "value" ;

    ok $db->del("some key") == 0 ;
    ok $db->get("some key", $value) == Db::DB_NOTFOUND ;

    ok $db->sync() == 0 ;

    # Check NOOVERWRITE will make put fail when attempting to overwrite
    # an existing record.

    ok $db->put( 'key', 'x', Db::DB_NOOVERWRITE) == Db::DB_KEYEXIST ;


    # check that the value of the key  has not been changed by the
    # previous test
    ok $db->get("key", $value) == 0 ;
    ok $value eq "value" ;

    # test Db::DB_GET_BOTH
    my ($k, $v) = ("key", "value") ;
    ok $db->get($k, $v, Db::DB_GET_BOTH) == 0 ;

    ($k, $v) = ("key", "fred") ;
    ok $db->get($k, $v, Db::DB_GET_BOTH) == Db::DB_NOTFOUND ;

    ($k, $v) = ("another", "value") ;
    ok $db->get($k, $v, Db::DB_GET_BOTH) == Db::DB_NOTFOUND ;


};

check_exception();


eval
{
    diag "Check simple env works with a hash.";
    my $lex = new LexFile $Dfile ;

    my $home = "./fred" ;
    ok my $lexD = new LexDir($home) ;

    my $env = new DbEnv;
    
    ok $env->open($home, Db::DB_CREATE|Db::DB_INIT_MPOOL, 0666) == 0;

    ok my $db = new Db($env) ;

    ok $db->open($Dfile, undef, Db::DB_BTREE, Db::DB_CREATE ) == 0;


    # Add a k/v pair
    my $value ;
    ok $db->put("some key", "some value") == 0 ;
    ok $db->get("some key", $value) == 0 ;
    ok $value eq "some value" ;
    undef $db ;
    undef $env ;
};
check_exception();


 
eval
{
    diag "cursors";

    my $lex = new LexFile $Dfile ;
    my %hash ;
    my ($k, $v) ;

    ok my $db = new Db ;

    ok $db->open($Dfile, "xx", Db::DB_BTREE, Db::DB_CREATE ) == 0;

    # create some data
    my %data =  (
		"red"	=> 2,
		"green"	=> "house",
		"blue"	=> "sea",
		) ;

    my $ret = 0 ;
    while (($k, $v) = each %data) {
        $ret += $db->put($k, $v) ;
    }
    ok $ret == 0 ;

    # create the cursor
    ok my $cursor = $db->cursor() ;

    $k = $v = "" ;
    my %copy = %data ;
    my $extras = 0 ;
    # sequence forwards
    while ($cursor->get($k, $v, Db::DB_NEXT) == 0) {
        if ( $copy{$k} eq $v ) 
            { delete $copy{$k} }
	else
	    { ++ $extras }
    }
    ok keys %copy == 0 ;
    ok $extras == 0 ;

    # sequence backwards
    %copy = %data ;
    $extras = 0 ;
    my $status ;
    for ( $status = $cursor->get($k, $v, Db::DB_LAST) ;
	  $status == 0 ;
    	  $status = $cursor->get($k, $v, Db::DB_PREV)) {
        if ( $copy{$k} eq $v ) 
            { delete $copy{$k} }
	else
	    { ++ $extras }
    }
    ok $status == Db::DB_NOTFOUND ;
    ok keys %copy == 0 ;
    ok $extras == 0 ;

    ($k, $v) = ("green", "house") ;
    ok $cursor->get($k, $v, Db::DB_GET_BOTH) == 0 ;

    ($k, $v) = ("green", "door") ;
    ok $cursor->get($k, $v, Db::DB_GET_BOTH) == Db::DB_NOTFOUND ;

    ($k, $v) = ("black", "house") ;
    ok $cursor->get($k, $v, Db::DB_GET_BOTH) == Db::DB_NOTFOUND ;

};
check_exception();

__END__
 
eval
{
    # override default compare
    my $lex = new LexFile $Dfile, $Dfile2, $Dfile3 ;
    my $value ;
    my (%h, %g, %k) ;
    my @Keys = qw( 0123 12 -1234 9 987654321 def  ) ; 
    ok 70, tie %h, "BerkeleyDB::Btree", -Filename => $Dfile, 
				     -Compare   => sub { $_[0] <=> $_[1] },
				     -Flags    => DB_CREATE ;

    ok 71, tie %g, 'BerkeleyDB::Btree', -Filename => $Dfile2, 
				     -Compare   => sub { $_[0] cmp $_[1] },
				     -Flags    => DB_CREATE ;

    ok 72, tie %k, 'BerkeleyDB::Btree', -Filename => $Dfile3, 
				   -Compare   => sub { length $_[0] <=> length $_[1] },
				   -Flags    => DB_CREATE ;

    my @srt_1 ;
    { local $^W = 0 ;
      @srt_1 = sort { $a <=> $b } @Keys ; 
    }
    my @srt_2 = sort { $a cmp $b } @Keys ;
    my @srt_3 = sort { length $a <=> length $b } @Keys ;

    foreach (@Keys) {
        local $^W = 0 ;
        $h{$_} = 1 ; 
        $g{$_} = 1 ;
        $k{$_} = 1 ;
    }

    sub ArrayCompare
    {
        my($a, $b) = @_ ;
    
        return 0 if @$a != @$b ;
    
        foreach (1 .. length @$a)
        {
            return 0 unless $$a[$_] eq $$b[$_] ;
        }

        1 ;
    }

    ok 73, ArrayCompare (\@srt_1, [keys %h]);
    ok 74, ArrayCompare (\@srt_2, [keys %g]);
    ok 75, ArrayCompare (\@srt_3, [keys %k]);

};
check_exception();

{
    # override default compare, with duplicates, don't sort values
    my $lex = new LexFile $Dfile, $Dfile2, $Dfile3 ;
    my $value ;
    my (%h, %g, %k) ;
    my @Keys   = qw( 0123 9 12 -1234 9 987654321 def  ) ; 
    my @Values = qw( 1    0 3   dd   x abc       0    ) ; 
    ok 76, tie %h, "BerkeleyDB::Btree", -Filename => $Dfile, 
				     -Compare   => sub { $_[0] <=> $_[1] },
				     -Property  => DB_DUP,
				     -Flags    => DB_CREATE ;

    ok 77, tie %g, 'BerkeleyDB::Btree', -Filename => $Dfile2, 
				     -Compare   => sub { $_[0] cmp $_[1] },
				     -Property  => DB_DUP,
				     -Flags    => DB_CREATE ;

    ok 78, tie %k, 'BerkeleyDB::Btree', -Filename => $Dfile3, 
				   -Compare   => sub { length $_[0] <=> length $_[1] },
				   -Property  => DB_DUP,
				   -Flags    => DB_CREATE ;

    my @srt_1 ;
    { local $^W = 0 ;
      @srt_1 = sort { $a <=> $b } @Keys ; 
    }
    my @srt_2 = sort { $a cmp $b } @Keys ;
    my @srt_3 = sort { length $a <=> length $b } @Keys ;

    foreach (@Keys) {
        local $^W = 0 ;
	my $value = shift @Values ;
        $h{$_} = $value ; 
        $g{$_} = $value ;
        $k{$_} = $value ;
    }

    sub getValues
    {
         my $hash = shift ;
	 my $db = tied %$hash ;
	 my $cursor = $db->cursor() ;
	 my @values = () ;
	 my ($k, $v) = (0,0) ;
         while ($cursor->get($k, $v, DB_NEXT) == 0) {
	     push @values, $v ;
	 }
	 return @values ;
    }

    ok 79, ArrayCompare (\@srt_1, [keys %h]);
    ok 80, ArrayCompare (\@srt_2, [keys %g]);
    ok 81, ArrayCompare (\@srt_3, [keys %k]);
    ok 82, ArrayCompare ([qw(dd 0 0 x 3 1 abc)], [getValues \%h]);
    ok 83, ArrayCompare ([qw(dd 1 0 3 x abc 0)], [getValues \%g]);
    ok 84, ArrayCompare ([qw(0 x 3 0 1 dd abc)], [getValues \%k]);

    # test DB_DUP_NEXT
    ok 85, my $cur = (tied %g)->cursor() ;
    my ($k, $v) = (9, "") ;
    ok 86, $cur->get($k, $v, DB_SET) == 0 ;
    ok 87, $k == 9 && $v == 0 ;
    ok 88, $cur->get($k, $v, DB_NEXT_DUP) == 0 ;
    ok 89, $k == 9 && $v eq "x" ;
    ok 90, $cur->get($k, $v, DB_NEXT_DUP) == DB_NOTFOUND ;
}

{
    # override default compare, with duplicates, sort values
    my $lex = new LexFile $Dfile, $Dfile2;
    my $value ;
    my (%h, %g) ;
    my @Keys   = qw( 0123 9 12 -1234 9 987654321 9 def  ) ; 
    my @Values = qw( 1    11 3   dd   x abc      2 0    ) ; 
    ok 91, tie %h, "BerkeleyDB::Btree", -Filename => $Dfile, 
				     -Compare   => sub { $_[0] <=> $_[1] },
				     -DupCompare   => sub { $_[0] cmp $_[1] },
				     -Property  => DB_DUP,
				     -Flags    => DB_CREATE ;

    ok 92, tie %g, 'BerkeleyDB::Btree', -Filename => $Dfile2, 
				     -Compare   => sub { $_[0] cmp $_[1] },
				     -DupCompare   => sub { $_[0] <=> $_[1] },
				     -Property  => DB_DUP,
				     
				     
				     
				     -Flags    => DB_CREATE ;

    my @srt_1 ;
    { local $^W = 0 ;
      @srt_1 = sort { $a <=> $b } @Keys ; 
    }
    my @srt_2 = sort { $a cmp $b } @Keys ;

    foreach (@Keys) {
        local $^W = 0 ;
	my $value = shift @Values ;
        $h{$_} = $value ; 
        $g{$_} = $value ;
    }

    ok 93, ArrayCompare (\@srt_1, [keys %h]);
    ok 94, ArrayCompare (\@srt_2, [keys %g]);
    ok 95, ArrayCompare ([qw(dd 1 3 x 2 11 abc 0)], [getValues \%g]);
    ok 96, ArrayCompare ([qw(dd 0 11 2 x 3 1 abc)], [getValues \%h]);

}

{
    # get_dup etc
    my $lex = new LexFile $Dfile;
    my %hh ;

    ok 97, my $YY = tie %hh, "BerkeleyDB::Btree", -Filename => $Dfile, 
				     -DupCompare   => sub { $_[0] cmp $_[1] },
				     -Property  => DB_DUP,
				     -Flags    => DB_CREATE ;

    $hh{'Wall'} = 'Larry' ;
    $hh{'Wall'} = 'Stone' ; # Note the duplicate key
    $hh{'Wall'} = 'Brick' ; # Note the duplicate key
    $hh{'Smith'} = 'John' ;
    $hh{'mouse'} = 'mickey' ;
    
    # first work in scalar context
    ok 98, scalar $YY->get_dup('Unknown') == 0 ;
    ok 99, scalar $YY->get_dup('Smith') == 1 ;
    ok 100, scalar $YY->get_dup('Wall') == 3 ;
    
    # now in list context
    my @unknown = $YY->get_dup('Unknown') ;
    ok 101, "@unknown" eq "" ;
    
    my @smith = $YY->get_dup('Smith') ;
    ok 102, "@smith" eq "John" ;
    
    {
    my @wall = $YY->get_dup('Wall') ;
    my %wall ;
    @wall{@wall} = @wall ;
    ok 103, (@wall == 3 && $wall{'Larry'} && $wall{'Stone'} && $wall{'Brick'});
    }
    
    # hash
    my %unknown = $YY->get_dup('Unknown', 1) ;
    ok 104, keys %unknown == 0 ;
    
    my %smith = $YY->get_dup('Smith', 1) ;
    ok 105, keys %smith == 1 && $smith{'John'} ;
    
    my %wall = $YY->get_dup('Wall', 1) ;
    ok 106, keys %wall == 3 && $wall{'Larry'} == 1 && $wall{'Stone'} == 1 
    		&& $wall{'Brick'} == 1 ;
    
    undef $YY ;
    untie %hh ;

}

{
    # in-memory file

    my $lex = new LexFile $Dfile ;
    my %hash ;
    my $fd ;
    my $value ;
    ok 107, my $db = tie %hash, 'BerkeleyDB::Btree' ;

    ok 108, $db->put("some key", "some value") == 0  ;
    ok 109, $db->get("some key", $value) == 0 ;
    ok 110, $value eq "some value" ;

}
 
{
    # partial
    # check works via API

    my $lex = new LexFile $Dfile ;
    my $value ;
    ok 111, my $db = new BerkeleyDB::Btree, -Filename => $Dfile,
                                      	       -Flags    => DB_CREATE ;

    # create some data
    my %data =  (
		"red"	=> "boat",
		"green"	=> "house",
		"blue"	=> "sea",
		) ;

    my $ret = 0 ;
    while (my ($k, $v) = each %data) {
        $ret += $db->put($k, $v) ;
    }
    ok 112, $ret == 0 ;


    # do a partial get
    my ($pon, $off, $len) = $db->partial_set(0,2) ;
    ok 113, ! $pon && $off == 0 && $len == 0 ;
    ok 114, $db->get("red", $value) == 0 && $value eq "bo" ;
    ok 115, $db->get("green", $value) == 0 && $value eq "ho" ;
    ok 116, $db->get("blue", $value) == 0 && $value eq "se" ;

    # do a partial get, off end of data
    ($pon, $off, $len) = $db->partial_set(3,2) ;
    ok 117, $pon ;
    ok 118, $off == 0 ;
    ok 119, $len == 2 ;
    ok 120, $db->get("red", $value) == 0 && $value eq "t" ;
    ok 121, $db->get("green", $value) == 0 && $value eq "se" ;
    ok 122, $db->get("blue", $value) == 0 && $value eq "" ;

    # switch of partial mode
    ($pon, $off, $len) = $db->partial_clear() ;
    ok 123, $pon ;
    ok 124, $off == 3 ;
    ok 125, $len == 2 ;
    ok 126, $db->get("red", $value) == 0 && $value eq "boat" ;
    ok 127, $db->get("green", $value) == 0 && $value eq "house" ;
    ok 128, $db->get("blue", $value) == 0 && $value eq "sea" ;

    # now partial put
    $db->partial_set(0,2) ;
    ok 129, $db->put("red", "") == 0 ;
    ok 130, $db->put("green", "AB") == 0 ;
    ok 131, $db->put("blue", "XYZ") == 0 ;
    ok 132, $db->put("new", "KLM") == 0 ;

    ($pon, $off, $len) = $db->partial_clear() ;
    ok 133, $pon ;
    ok 134, $off == 0 ;
    ok 135, $len == 2 ;
    ok 136, $db->get("red", $value) == 0 && $value eq "at" ;
    ok 137, $db->get("green", $value) == 0 && $value eq "ABuse" ;
    ok 138, $db->get("blue", $value) == 0 && $value eq "XYZa" ;
    ok 139, $db->get("new", $value) == 0 && $value eq "KLM" ;

    # now partial put
    ($pon, $off, $len) = $db->partial_set(3,2) ;
    ok 140, ! $pon ;
    ok 141, $off == 0 ;
    ok 142, $len == 0 ;
    ok 143, $db->put("red", "PPP") == 0 ;
    ok 144, $db->put("green", "Q") == 0 ;
    ok 145, $db->put("blue", "XYZ") == 0 ;
    ok 146, $db->put("new", "TU") == 0 ;

    $db->partial_clear() ;
    ok 147, $db->get("red", $value) == 0 && $value eq "at\0PPP" ;
    ok 148, $db->get("green", $value) == 0 && $value eq "ABuQ" ;
    ok 149, $db->get("blue", $value) == 0 && $value eq "XYZXYZ" ;
    ok 150, $db->get("new", $value) == 0 && $value eq "KLMTU" ;
}

{
    # partial
    # check works via tied hash 

    my $lex = new LexFile $Dfile ;
    my %hash ;
    my $value ;
    ok 151, my $db = tie %hash, 'BerkeleyDB::Btree', -Filename => $Dfile,
                                      	       -Flags    => DB_CREATE ;

    # create some data
    my %data =  (
		"red"	=> "boat",
		"green"	=> "house",
		"blue"	=> "sea",
		) ;

    while (my ($k, $v) = each %data) {
	$hash{$k} = $v ;
    }


    # do a partial get
    $db->partial_set(0,2) ;
    ok 152, $hash{"red"} eq "bo" ;
    ok 153, $hash{"green"} eq "ho" ;
    ok 154, $hash{"blue"}  eq "se" ;

    # do a partial get, off end of data
    $db->partial_set(3,2) ;
    ok 155, $hash{"red"} eq "t" ;
    ok 156, $hash{"green"} eq "se" ;
    ok 157, $hash{"blue"} eq "" ;

    # switch of partial mode
    $db->partial_clear() ;
    ok 158, $hash{"red"} eq "boat" ;
    ok 159, $hash{"green"} eq "house" ;
    ok 160, $hash{"blue"} eq "sea" ;

    # now partial put
    $db->partial_set(0,2) ;
    ok 161, $hash{"red"} = "" ;
    ok 162, $hash{"green"} = "AB" ;
    ok 163, $hash{"blue"} = "XYZ" ;
    ok 164, $hash{"new"} = "KLM" ;

    $db->partial_clear() ;
    ok 165, $hash{"red"} eq "at" ;
    ok 166, $hash{"green"} eq "ABuse" ;
    ok 167, $hash{"blue"} eq "XYZa" ;
    ok 168, $hash{"new"} eq "KLM" ;

    # now partial put
    $db->partial_set(3,2) ;
    ok 169, $hash{"red"} = "PPP" ;
    ok 170, $hash{"green"} = "Q" ;
    ok 171, $hash{"blue"} = "XYZ" ;
    ok 172, $hash{"new"} = "TU" ;

    $db->partial_clear() ;
    ok 173, $hash{"red"} eq "at\0PPP" ;
    ok 174, $hash{"green"} eq "ABuQ" ;
    ok 175, $hash{"blue"} eq "XYZXYZ" ;
    ok 176, $hash{"new"} eq "KLMTU" ;
}

{
    # transaction

    my $lex = new LexFile $Dfile ;
    my %hash ;
    my $value ;

    my $home = "./fred" ;
    ok 177, my $lexD = new LexDir($home) ;
    ok 178, my $env = new BerkeleyDB::Env -Home => $home,
				     -Flags => DB_CREATE|DB_INIT_TXN|
					  	DB_INIT_MPOOL|DB_INIT_LOCK ;
    ok 179, my $txn = $env->txn_begin() ;
    ok 180, my $db1 = tie %hash, 'BerkeleyDB::Btree', -Filename => $Dfile,
                                      	       -Flags    =>  DB_CREATE ,
					       -Env 	 => $env,
					       -Txn	 => $txn ;

    ok 181, (my $Z = $txn->txn_commit()) == 0 ;
    ok 182, $txn = $env->txn_begin() ;
    $db1->Txn($txn);
    
    # create some data
    my %data =  (
		"red"	=> "boat",
		"green"	=> "house",
		"blue"	=> "sea",
		) ;

    my $ret = 0 ;
    while (my ($k, $v) = each %data) {
        $ret += $db1->put($k, $v) ;
    }
    ok 183, $ret == 0 ;

    # should be able to see all the records

    ok 184, my $cursor = $db1->cursor() ;
    my ($k, $v) = ("", "") ;
    my $count = 0 ;
    # sequence forwards
    while ($cursor->get($k, $v, DB_NEXT) == 0) {
        ++ $count ;
    }
    ok 185, $count == 3 ;
    undef $cursor ;

    # now abort the transaction
    #ok 151, $txn->txn_abort() == 0 ;
    ok 186, ($Z = $txn->txn_abort()) == 0 ;

    # there shouldn't be any records in the database
    $count = 0 ;
    # sequence forwards
    ok 187, $cursor = $db1->cursor() ;
    while ($cursor->get($k, $v, DB_NEXT) == 0) {
        ++ $count ;
    }
    ok 188, $count == 0 ;

    undef $txn ;
    undef $cursor ;
    undef $db1 ;
    undef $env ;
    untie %hash ;
}

{
    # DB_DUP

    my $lex = new LexFile $Dfile ;
    my %hash ;
    ok 189, my $db = tie %hash, 'BerkeleyDB::Btree', -Filename => $Dfile,
				      -Property  => DB_DUP,
                                      -Flags    => DB_CREATE ;

    $hash{'Wall'} = 'Larry' ;
    $hash{'Wall'} = 'Stone' ;
    $hash{'Smith'} = 'John' ;
    $hash{'Wall'} = 'Brick' ;
    $hash{'Wall'} = 'Brick' ;
    $hash{'mouse'} = 'mickey' ;

    ok 190, keys %hash == 6 ;

    # create a cursor
    ok 191, my $cursor = $db->cursor() ;

    my $key = "Wall" ;
    my $value ;
    ok 192, $cursor->get($key, $value, DB_SET) == 0 ;
    ok 193, $key eq "Wall" && $value eq "Larry" ;
    ok 194, $cursor->get($key, $value, DB_NEXT) == 0 ;
    ok 195, $key eq "Wall" && $value eq "Stone" ;
    ok 196, $cursor->get($key, $value, DB_NEXT) == 0 ;
    ok 197, $key eq "Wall" && $value eq "Brick" ;
    ok 198, $cursor->get($key, $value, DB_NEXT) == 0 ;
    ok 199, $key eq "Wall" && $value eq "Brick" ;

    #my $ref = $db->stat() ; 
    #ok 200, ($ref->{bt_flags} | DB_DUP) == DB_DUP ;
#print "bt_flags " . $ref->{bt_flags} . " DB_DUP " . DB_DUP ."\n";

    undef $db ;
    undef $cursor ;
    untie %hash ;

}

{
    # stat

    my $lex = new LexFile $Dfile ;
    my $recs = ($BerkeleyDB::version >= 3.1 ? "bt_ndata" : "bt_nrecs") ;
    my %hash ;
    my ($k, $v) ;
    ok 200, my $db = new BerkeleyDB::Btree -Filename => $Dfile, 
				     -Flags    => DB_CREATE,
				 	-Minkey	=>3 ,
					-Pagesize	=> 2 **12 
					;

    my $ref = $db->stat() ; 
    ok 201, $ref->{$recs} == 0;
    ok 202, $ref->{'bt_minkey'} == 3;
    ok 203, $ref->{'bt_pagesize'} == 2 ** 12;

    # create some data
    my %data =  (
		"red"	=> 2,
		"green"	=> "house",
		"blue"	=> "sea",
		) ;

    my $ret = 0 ;
    while (($k, $v) = each %data) {
        $ret += $db->put($k, $v) ;
    }
    ok 204, $ret == 0 ;

    $ref = $db->stat() ; 
    ok 205, $ref->{$recs} == 3;
}

{
   # sub-class test

   package Another ;

   use strict ;

   open(FILE, ">SubDB.pm") or die "Cannot open SubDB.pm: $!\n" ;
   print FILE <<'EOM' ;

   package SubDB ;

   use strict ;
   use vars qw( @ISA @EXPORT) ;

   require Exporter ;
   use BerkeleyDB;
   @ISA=qw(BerkeleyDB::Btree);
   @EXPORT = @BerkeleyDB::EXPORT ;

   sub put { 
	my $self = shift ;
        my $key = shift ;
        my $value = shift ;
        $self->SUPER::put($key, $value * 3) ;
   }

   sub get { 
	my $self = shift ;
        $self->SUPER::get($_[0], $_[1]) ;
	$_[1] -= 2 ;
   }

   sub A_new_method
   {
	my $self = shift ;
        my $key = shift ;
        my $value = $self->FETCH($key) ;
	return "[[$value]]" ;
   }

   1 ;
EOM

    close FILE ;

    BEGIN { push @INC, '.'; }    
    eval 'use SubDB ; ';
    main::ok 206, $@ eq "" ;
    my %h ;
    my $X ;
    eval '
	$X = tie(%h, "SubDB", -Filename => "dbbtree.tmp", 
			-Flags => DB_CREATE,
			-Mode => 0640 );
	' ;

    main::ok 207, $@ eq "" && $X ;

    my $ret = eval '$h{"fred"} = 3 ; return $h{"fred"} ' ;
    main::ok 208, $@ eq "" ;
    main::ok 209, $ret == 7 ;

    my $value = 0;
    $ret = eval '$X->put("joe", 4) ; $X->get("joe", $value) ; return $value' ;
    main::ok 210, $@ eq "" ;
    main::ok 211, $ret == 10 ;

    $ret = eval ' DB_NEXT eq main::DB_NEXT ' ;
    main::ok 212, $@ eq ""  ;
    main::ok 213, $ret == 1 ;

    $ret = eval '$X->A_new_method("joe") ' ;
    main::ok 214, $@ eq "" ;
    main::ok 215, $ret eq "[[10]]" ;

    undef $X;
    untie %h;
    unlink "SubDB.pm", "dbbtree.tmp" ;

}

{
    # DB_RECNUM, DB_SET_RECNO & DB_GET_RECNO

    my $lex = new LexFile $Dfile ;
    my %hash ;
    my ($k, $v) = ("", "");
    ok 216, my $db = new BerkeleyDB::Btree 
				-Filename  => $Dfile, 
			     	-Flags     => DB_CREATE,
			     	-Property  => DB_RECNUM ;


    # create some data
    my @data =  (
		"A zero",
		"B one",
		"C two",
		"D three",
		"E four"
		) ;

    my $ix = 0 ;
    my $ret = 0 ;
    foreach (@data) {
        $ret += $db->put($_, $ix) ;
	++ $ix ;
    }
    ok 217, $ret == 0 ;

    # get & DB_SET_RECNO
    $k = 1 ;
    ok 218, $db->get($k, $v, DB_SET_RECNO) == 0;
    ok 219, $k eq "B one" && $v == 1 ;

    $k = 3 ;
    ok 220, $db->get($k, $v, DB_SET_RECNO) == 0;
    ok 221, $k eq "D three" && $v == 3 ;

    $k = 4 ;
    ok 222, $db->get($k, $v, DB_SET_RECNO) == 0;
    ok 223, $k eq "E four" && $v == 4 ;

    $k = 0 ;
    ok 224, $db->get($k, $v, DB_SET_RECNO) == 0;
    ok 225, $k eq "A zero" && $v == 0 ;

    # cursor & DB_SET_RECNO

    # create the cursor
    ok 226, my $cursor = $db->cursor() ;

    $k = 2 ;
    ok 227, $db->get($k, $v, DB_SET_RECNO) == 0;
    ok 228, $k eq "C two" && $v == 2 ;

    $k = 0 ;
    ok 229, $cursor->get($k, $v, DB_SET_RECNO) == 0;
    ok 230, $k eq "A zero" && $v == 0 ;

    $k = 3 ;
    ok 231, $db->get($k, $v, DB_SET_RECNO) == 0;
    ok 232, $k eq "D three" && $v == 3 ;

    # cursor & DB_GET_RECNO
    ok 233, $cursor->get($k, $v, DB_FIRST) == 0 ;
    ok 234, $k eq "A zero" && $v == 0 ;
    ok 235, $cursor->get($k, $v, DB_GET_RECNO) == 0;
    ok 236, $v == 0 ;

    ok 237, $cursor->get($k, $v, DB_NEXT) == 0 ;
    ok 238, $k eq "B one" && $v == 1 ;
    ok 239, $cursor->get($k, $v, DB_GET_RECNO) == 0;
    ok 240, $v == 1 ;

    ok 241, $cursor->get($k, $v, DB_LAST) == 0 ;
    ok 242, $k eq "E four" && $v == 4 ;
    ok 243, $cursor->get($k, $v, DB_GET_RECNO) == 0;
    ok 244, $v == 4 ;

}

