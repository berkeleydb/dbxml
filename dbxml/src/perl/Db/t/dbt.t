#!./perl


use warnings;
use strict ;

BEGIN {
    unless(grep /blib/, @INC) {
        chdir 't' if -d 't';
        @INC = '../lib' if -d '../lib';
    }
}

use Sleepycat::Db 'simple' ; 
use t::util ;

use Test::More tests => 10 ;


umask(0) ;


# Check for invalid parameters
{
    # Check for invalid parameters
    eval { my $db = new Dbt (1,2,3,4,5) } ;
    like $@, qr/Dbt expects 0, 1 or 2 parameters/, "too many parameters";
}


{
    # constructor with no parameters
    my $dbt0 = new Dbt ;
    ok $dbt0, "object is not null" ;
    is $dbt0->get_data(), undef, "get_data should return null";
    my $data0 = "data0" ;
    $dbt0->set_data($data0);
    is $dbt0->get_data(), $data0, "get_data should return 'data0'";


    # constructor with one parameter
    my $dbt1 = new Dbt "abc" ;
    ok $dbt1, "object is not null" ;
    is $dbt1->get_data(), "abc", "get_data should return 'abc'";
    my $data1 = "data1" ;
    $dbt1->set_data($data1);
    is $dbt1->get_data(), $data1, "get_data should return 'data1'";

    # constructor with two parameters
    my $dbt2 = new Dbt "abcdef", 4 ;
    ok $dbt2, "object is not null" ;
    is $dbt2->get_data(), "abcd", "get_data should return 'abcd'";
    my $data2 = "data2" ;
    $dbt2->set_data($data2);
    is $dbt2->get_data(), $data2, "get_data should return 'data2'";
}

