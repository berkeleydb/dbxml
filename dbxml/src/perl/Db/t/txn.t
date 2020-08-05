#!./perl -w

use strict ;

BEGIN {
    unless(grep /blib/, @INC) {
        chdir 't' if -d 't';
        @INC = '../lib' if -d '../lib';
    }
}

use Sleepycat::Db; 
use t::util ;

use Test::More tests => 28 ;

my $Dfile = "dbhash.tmp";

umask(0);

#{
#    # error cases
#
#    my $lex = new LexFile $Dfile ;
#    my %hash ;
#    my $value ;
#
#    my $home = "./fred" ;
#    ok 1, my $lexD = new LexDir($home);
#    ok 2, my $env = new BerkeleyDB::Env -Home => $home,
#				     -Flags => DB_CREATE| DB_INIT_MPOOL;
#    eval { $env->txn_begin() ; } ;
#    ok 3, $@ =~ /^BerkeleyDB Aborting: Transaction Manager not enabled at/ ;
#
#    eval { my $txn_mgr = $env->TxnMgr() ; } ;
#    ok 4, $@ =~ /^BerkeleyDB Aborting: Transaction Manager not enabled at/ ;
#    undef $env ;
#
#}


eval
{
    diag "transaction - commit works";

    my $lex = new LexFile $Dfile ;
    my %hash ;
    my $value ;

    my $home = "./fred" ;
    ok my $lexD = new LexDir($home);

    my $env = new DbEnv;
    
    ok $env->open($home, Db::DB_CREATE|Db::DB_INIT_MPOOL|
                         Db::DB_INIT_TXN|Db::DB_INIT_LOCK, 0666) == 0;

    ok my $txn = $env->txn_begin() ;

    ok my $db = new Db($env) ;

    ok $db->open($txn, $Dfile, undef, Db::DB_BTREE, Db::DB_CREATE ) == 0;

    is $txn->commit(), 0 ;
    ok $txn = $env->txn_begin() ;

    # create some data
    my %data =  (
		"red"	=> "boat",
		"green"	=> "house",
		"blue"	=> "sea",
		) ;

    my $ret = 0 ;
    while (my ($k, $v) = each %data) {
        $ret += $db->put($txn, $k, $v) ;
    }
    is $ret, 0 ;

    # should be able to see all the records

    ok my $cursor = $db->cursor($txn) ;
    my ($k, $v) = ("", "") ;
    my $count = 0 ;
    # sequence forwards
    while ($cursor->get($k, $v, Db::DB_NEXT) == 0) {
        ++ $count ;
    }
    is $count, 3 ;
    is $cursor->close(), 0 ;

    # now commit the transaction
    is $txn->commit(), 0 ;

    $count = 0 ;
    # sequence forwards
    ok $cursor = $db->cursor() ;
    while ($cursor->get($k, $v, Db::DB_NEXT) == 0) {
        ++ $count ;
    }
    is $count, 3 ;

    #my $stat = $env->txn_stat() ;
    #ok $stat->{'st_naborts'} == 0 ;

    undef $txn ;
    undef $cursor ;
    undef $db ;
    undef $env ;
    untie %hash ;
};
check_exception();


eval
{
    diag "transaction - abort works";

    my $lex = new LexFile $Dfile ;
    my %hash ;
    my $value ;

    my $home = "./fred" ;
    ok my $lexD = new LexDir($home);

    my $env = new DbEnv;
    
    ok $env->open($home, Db::DB_CREATE|Db::DB_INIT_MPOOL|
                         Db::DB_INIT_TXN|Db::DB_INIT_LOCK, 0666) == 0;

    ok my $txn = $env->txn_begin() ;

    ok my $db = new Db($env) ;

    ok $db->open($txn, $Dfile, undef, Db::DB_BTREE, Db::DB_CREATE ) == 0;


    is $txn->commit(), 0 ;

    ok $txn = $env->txn_begin() ;

    # create some data
    my %data =  (
		"red"	=> "boat",
		"green"	=> "house",
		"blue"	=> "sea",
		) ;

    my $ret = 0 ;
    while (my ($k, $v) = each %data) {
        $ret += $db->put($txn, $k, $v) ;
    }
    is $ret, 0 ;

    # should be able to see all the records

    ok my $cursor = $db->cursor($txn) ;
    my ($k, $v) = ("", "") ;
    my $count = 0 ;
    # sequence forwards
    while ($cursor->get($k, $v, Db::DB_NEXT) == 0) {
        ++ $count ;
    }
    is $count, 3 ;
    is $cursor->close(), 0 ;

    # now abort the transaction
    print "abou to call abort\n";
    is $txn->abort(), 0 ;

    # there should not be any records in the database
    $count = 0 ;
    # sequence forwards
    ok $cursor = $db->cursor() ;
    while ($cursor->get($k, $v, Db::DB_NEXT) == 0) {
        ++ $count ;
    }
    is $count, 0 ;

    #my $stat = $env->txn_stat() ;
    #ok 17, $stat->{'st_naborts'} == 1 ;

    undef $txn ;
    undef $cursor ;
    undef $db ;
    undef $env ;
    untie %hash ;
};
check_exception();

