//
// Created by MacBook on 03.01.2025.
//

#include "../assert_that.h"
#include "../dynstring.h"
#include "../run_test.h"
#include "../string_utils.h"


bool test_one_appendn( string_t init, string_t app, size_t len, string_t expected )
{
    DynamicString dynstr = dynstr_init_as( init );
    assert_that( dynstr != NULL, "dynstr alloc" );
    assert_that( dynstr_appendn( dynstr, app, len ) == RV_SUCCESS, "append rv" );

    bool rv = strcmp( dynstr_data( dynstr ), expected ) == 0;

    dynstr_destroy( dynstr );

    return rv;
}

TEST( appendn )
{
    UNIT_TEST( test_one_appendn( "Hops", "pop", 3, "Hopspop" ) );
    UNIT_TEST( test_one_appendn( "Hops", "pop", 1, "Hopsp" ) );
    UNIT_TEST( test_one_appendn( "", *( &"Hopspop" ) + 1, 3, "ops" ) );
}
END_TEST

TEST( all_fail )
{
    UNIT_TEST( false );
    UNIT_TEST( fopen( "nonexistant", "r" ) != NULL );
    UNIT_TEST( fopen( "another nonexistant", "r" ) != NULL );
}
END_TEST

TEST( test_example )
{
    UNIT_TEST( true );
    UNIT_TEST( true );
    UNIT_TEST( false );
    UNIT_TEST( true );
}
END_TEST

int main( void )
{
    RUN( appendn );
    RUN( all_fail );

    RUN( test_example );
}
