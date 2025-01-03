//
// Created by MacBook on 03.01.2025.
//

#include "../assert_that.h"
#include "../dynstring.h"
#include "../string_utils.h"

#define run_test( TEST )                                 \
    do                                                   \
    {                                                    \
        printf( "running ‹" #TEST "› \n" );              \
        TEST;                                            \
        printf( "successfully finished ‹" #TEST "›\n" ); \
    }                                                    \
    while ( 0 )

#define run_unit_test( TEST )                              \
    do                                                     \
    {                                                      \
        printf( "\trunning ‹" #TEST "› \n" );              \
        TEST;                                              \
        printf( "\tsuccessfully finished ‹" #TEST "›\n" ); \
    }                                                      \
    while ( 0 )


#define test_one_appendn( INIT, APP, LEN, EXPECTED )                                  \
    do                                                                                \
    {                                                                                 \
        DynamicString dynstr = dynstr_init_as( INIT );                                \
        assert_that( dynstr != NULL, "dynstr alloc" );                                \
        assert_that( dynstr_appendn( dynstr, APP, LEN ) == RV_SUCCESS, "append rv" ); \
        assert_that( strcmp( dynstr_data( dynstr ), EXPECTED ) == 0, "streq" );       \
        dynstr_destroy( dynstr );                                                     \
    }                                                                                 \
    while ( 0 )


void test_appendn( void )
{
    run_unit_test( test_one_appendn( "Hops", "pop", 3, "Hopspop" ) );
    run_unit_test( test_one_appendn( "Hops", "pop", 1, "Hopsp" ) );
    run_unit_test( test_one_appendn( "", *( &"Hopspop" ) + 1, 3, "ops" ) );
}

int main( void )
{
    run_test( test_appendn() );
}
