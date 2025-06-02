//
// Created by MacBook on 08.01.2025.
//

#include "../Dev/assert_that.h"
#include "../Dev/unit_tests.h"
#include "../misc.h"
#include "../string_utils.h"
#include "../Structs/dynstring.h"

#include <assert.h>


typedef int( PendFunction )( struct dynamic_string *, string_t );

Tester test_one_pend( string_t init, string_t app, string_t expected, PendFunction func )
{
    struct dynamic_string *dynstr = dynstr_init_as( init );
    assert_that( dynstr != NULL, "dynstr alloc" );
    assert_that( func( dynstr, app ) == RV_SUCCESS, "append rv" );

    bool rv = strcmp( dynstr_data( dynstr ), expected ) == 0;

    dynstr_destroy( dynstr );

    return rv;
}

TEST( append )
{
    UNIT_TEST( test_one_pend( "Hops", "pop", "Hopspop", dynstr_append ) );
    UNIT_TEST( test_one_pend( "", *( &"Hopspop" ) + 1, "opspop", dynstr_append ) );
    UNIT_TEST( test_one_pend( "Hops", "", "Hops", dynstr_append ) );

    UNIT_TEST( test_one_pend( "Hops", "pop", "popHops", dynstr_prepend ) );
    UNIT_TEST( test_one_pend( "Vana", "Karel", "KarelVana", dynstr_prepend ) );
    UNIT_TEST( test_one_pend( "", *( &"Hopspop" ) + 1, "opspop", dynstr_prepend ) );
    UNIT_TEST( test_one_pend( "Hops", "", "Hops", dynstr_prepend ) );
}
END_TEST


Tester test_one_appendn( string_t init, string_t app, size_t len, string_t expected )
{
    struct dynamic_string *dynstr = dynstr_init_as( init );
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


Tester test_one_init_as( string_t init )
{
    size_t str_len                = strlen( init );
    struct dynamic_string *dynstr = dynstr_init_as( init );
    assert_that( dynstr != NULL, "init" );
    bool rv = cmpeq( strcmp( dynstr_data( dynstr ), init ) ) &&
              dynstr_len( dynstr ) == str_len;
    dynstr_destroy( dynstr );
    return rv;
}

TEST( init_as )
{
    UNIT_TEST( test_one_init_as( "Hops" ) );
    UNIT_TEST( test_one_init_as( "pop" ) );
    UNIT_TEST( test_one_init_as( "" ) );

    UNIT_TEST( dynstr_init_as( NULL ) == NULL );
}
END_TEST


Tester test_one_slice( string_t str,
                       size_t start,
                       ssize_t end,
                       int rv_want,
                       string_t res )
{
    struct dynamic_string *s = dynstr_init_as( str );
    assert_that( s != NULL, "strdup" );
    if ( strcmp( str, dynstr_data( s ) ) != 0 )
    {
        dynstr_destroy( s );
        return fwarnx_ret( false, "init_as isn't working" );
    }

    int rv_got = dynstr_slice( s, start, end );
    if ( rv_got != RV_SUCCESS )
    {
        dynstr_destroy( s );
        return rv_got == rv_want;
    }

    bool rv = strcmp( dynstr_data( s ), res ) == 0;
    dynstr_destroy( s );
    return rv;
}
Tester test_one_slice_e( string_t str, ssize_t end, int rv_want, string_t res )
{
    struct dynamic_string *s = dynstr_init_as( str );
    assert_that( s != NULL, "strdup" );
    if ( strcmp( str, dynstr_data( s ) ) != 0 )
    {
        dynstr_destroy( s );
        return fwarnx_ret( false, "init_as isn't working" );
    }

    int rv_got = dynstr_slice_e( s, end );
    if ( rv_got != RV_SUCCESS )
    {
        dynstr_destroy( s );
        return rv_got == rv_want;
    }

    bool rv = strcmp( dynstr_data( s ), res ) == 0;
    dynstr_destroy( s );
    return rv;
}
Tester test_one_slice_s( string_t str, size_t start, int rv_want, string_t res )
{
    struct dynamic_string *s = dynstr_init_as( str );
    assert_that( s != NULL, "strdup" );
    if ( strcmp( str, dynstr_data( s ) ) != 0 )
    {
        dynstr_destroy( s );
        return fwarnx_ret( false, "init_as isn't working" );
    }

    int rv_got = dynstr_slice_s( s, start );
    if ( rv_got != RV_SUCCESS )
    {
        dynstr_destroy( s );
        return rv_got == rv_want;
    }

    bool rv = strcmp( dynstr_data( s ), res ) == 0;
    dynstr_destroy( s );
    return rv;
}

TEST( slices )
{
    UNIT_TEST( test_one_slice( "Hopspop", 0, -1, RV_SUCCESS, "Hopspop" ) );
    UNIT_TEST( test_one_slice( "Hopspop", 2, -1, RV_SUCCESS, "pspop" ) );
    UNIT_TEST( test_one_slice( "Hopspop", 7, -1, RV_SUCCESS, "" ) );
    UNIT_TEST( test_one_slice( "Hopspop", 6, -1, RV_SUCCESS, "p" ) );
    UNIT_TEST( test_one_slice( "Hello, world!", 0, 4, RV_SUCCESS, "Hell" ) );
    UNIT_TEST( test_one_slice( "Hello, world!", 1, 2, RV_SUCCESS, "e" ) );
    UNIT_TEST( test_one_slice( "Hello, world!", 0, -3, RV_SUCCESS, "Hello, worl" ) );
    UNIT_TEST( test_one_slice( "Hopspop", 0, 4, RV_SUCCESS, "Hops" ) );
    UNIT_TEST( test_one_slice( "Hopspop", 2, 4, RV_SUCCESS, "ps" ) );
    UNIT_TEST( test_one_slice( "Hopspop", 2, -3, RV_SUCCESS, "psp" ) );

    UNIT_TEST( test_one_slice( "Hopspop", 1, 0, RV_EXCEPTION, NULL ) );
    UNIT_TEST( test_one_slice( "Hopspop", 10, -1, RV_EXCEPTION, NULL ) );


    UNIT_TEST( test_one_slice_e( "Hopspop", 0, RV_SUCCESS, "" ) );
    UNIT_TEST( test_one_slice_e( "Hopspop", 3, RV_SUCCESS, "Hop" ) );
    UNIT_TEST( test_one_slice_e( "Hopspop", 7, RV_SUCCESS, "Hopspop" ) );
    UNIT_TEST( test_one_slice_e( "Hopspop", -1, RV_SUCCESS, "Hopspop" ) );
    UNIT_TEST( test_one_slice_e( "Hopspop", -4, RV_SUCCESS, "Hops" ) );
    UNIT_TEST( test_one_slice_e( "Hopspop", -8, RV_SUCCESS, "" ) );
    UNIT_TEST( test_one_slice_e( "Hopspop", -9, RV_EXCEPTION, NULL ) );
    UNIT_TEST( test_one_slice_e( "Hopspop", -10, RV_EXCEPTION, NULL ) );


    UNIT_TEST( test_one_slice_s( "Hopspop", 0, RV_SUCCESS, "Hopspop" ) );
    UNIT_TEST( test_one_slice_s( "Hopspop", 2, RV_SUCCESS, "pspop" ) );
    UNIT_TEST( test_one_slice_s( "Hopspop", 7, RV_SUCCESS, "" ) );
    UNIT_TEST( test_one_slice_s( "Hopspop", 6, RV_SUCCESS, "p" ) );
    UNIT_TEST( test_one_slice_s( "Hopspop", SIZE_MAX, RV_EXCEPTION, NULL ) );
}
END_TEST

PrintfLike( 2, 3 ) Tester test_one_vappendf( string_t init, string_t fmt, ... )
{
    va_list vaList, vaCopy;
    va_start( vaList, fmt );
    va_copy( vaCopy, vaList );
    size_t app_len = vsnprintf( NULL, 0, fmt, vaList );
    va_end( vaList );

    str_t formatted = malloc( strlen( init ) + app_len + 1 );
    assert_that( formatted != NULL, "malloc" );
    strcpy( formatted, init );

    va_copy( vaList, vaCopy );
    assert_that( vsnprintf( formatted + strlen( init ), app_len + 1, fmt, vaCopy ) >= 0,
                 "vsnprintf" );

    struct dynamic_string *dynstr = dynstr_init_as( init );
    assert_that( dynstr != NULL, "dynstr init" );

    assert_that( dynstr_vappendf( dynstr, fmt, vaList ) == RV_SUCCESS,
                 "dynstr_vappendf" );

    bool rv = strcmp( dynstr_data( dynstr ), formatted ) == 0;

    free( formatted );
    dynstr_destroy( dynstr );

    return rv;
}

#define TEST_ONE_APPENDF( INIT, DESIRED, FMT, ... )                         \
    do                                                                      \
    {                                                                       \
        struct dynamic_string *dynstr = dynstr_init_as( INIT );             \
        assert( dynstr != NULL );                                           \
        assert( dynstr_appendf( dynstr, FMT, __VA_ARGS__ ) == RV_SUCCESS ); \
        UNIT_TEST( strcmp( dynstr_data( dynstr ), ( DESIRED ) ) == 0 );     \
        dynstr_destroy( dynstr );                                           \
    }                                                                       \
    while ( 0 )


TEST( appendf )
{
    TEST_ONE_APPENDF( "Hopspop", "Hopspop hovno", " %s", "hovno" );
    TEST_ONE_APPENDF( "", "Hopspop hovno", "%s %s", "Hopspop", "hovno" );

    UNIT_TEST( test_one_vappendf( "Hopspop", " %s", "hovno" ) );
    UNIT_TEST( test_one_vappendf( "Hopspop", " Kokot %s", "hovno" ) );
    UNIT_TEST( test_one_vappendf( "Hopspop", " %zu Kokot %s", 1023uL, "hovno" ) );
    UNIT_TEST( test_one_vappendf( "", "%zu Kokot %s\n", 1023uL, "hovno" ) );
    UNIT_TEST( test_one_vappendf(
            "", "%zu\\\n\r\t\v\"\'Kokot %s\n" ESCAPED_CHARS, 1023uL, "hovno" ) );
}
END_TEST
