//
// Created by MacBook on 06.01.2025.
//
// tests
#include "test_dynstr.h"
#include "test_foreach.h"
#include "test_misc_c.h"
#include "test_string_utils.h"
#include "test_swex.h"


void run_dynstr( void )
{
    RUN_TEST( init_as );
    RUN_TEST( slices );
    RUN_TEST( append );
    RUN_TEST( appendn );
    RUN_TEST( appendf );
}

void run_string_utils( void )
{
    RUN_TEST( replace );
    RUN_TEST( escape );
    RUN_TEST( strspl_str );
    RUN_TEST( strspl_regex );
    RUN_TEST( string_to_UL );
    RUN_TEST( string_as_UL );
    RUN_TEST( reverse_str );
    RUN_TEST( strip );
    RUN_TEST( strings_misc );
}

void run_foreach( void )
{
    RUN_TEST( foreach );
}

void run_swex( void )
{
    RUN_TEST( swex );
}

void run_misc_c( void )
{
    RUN_TEST( misc_c );
}

int main( void )
{
    run_misc_c();
    run_foreach();
    run_swex();
    run_string_utils();
    run_dynstr();

    FINISH_TESTING();
}
