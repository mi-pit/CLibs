//
// Created by MacBook on 06.01.2025.
//
#include "../assert_that.h"
#include "../dynstring.h"
#include "../misc.h"
#include "../run_test.h"
#include "../string_utils.h"

#include <stdlib.h>

#define Tester static bool

static void free_spl( const size_t count, str_t strings[ count ] )
{
    for ( size_t i = 0; i < count; ++i )
    {
        free( strings[ i ] );
    }
}

Tester test_one_replace( string_t orig, string_t old, string_t new, string_t expected )
{
    str_t got = string_replaced( orig, old, new );
    bool rv   = strcmp( got, expected ) == 0;
    free( got );

    return rv;
}
Tester test_one_appendn( string_t init, string_t app, size_t len, string_t expected )
{
    DynamicString dynstr = dynstr_init_as( init );
    assert_that( dynstr != NULL, "dynstr alloc" );
    assert_that( dynstr_appendn( dynstr, app, len ) == RV_SUCCESS, "append rv" );

    bool rv = strcmp( dynstr_data( dynstr ), expected ) == 0;

    dynstr_destroy( dynstr );

    return rv;
}
Tester test_one_escape( string_t old, string_t new )
{
    str_t esc = string_escaped( old );
    bool rv   = strcmp( esc, new ) == 0;
    free( esc );
    return rv;
}
Tester test_one_strspl_str( string_t haystack,
                            string_t split_tok,
                            strsplit_mode_t mode,
                            size_t n_split,
                            ... )
{
    str_t *spl;
    ssize_t strspl_rv = string_split( &spl, haystack, split_tok, mode );
    if ( strspl_rv < 0 )
        exit( 1 );

    size_t n_got = ( size_t ) strspl_rv;

    bool rv = n_got == n_split;
    if ( !rv )
    {
        free_spl( n_got, spl );
        return false;
    }

    va_list vaList;
    va_start( vaList, n_split );
    for ( size_t i = 0; i < n_got && rv; ++i )
    {
        string_t cmp = va_arg( vaList, string_t );
        rv           = strcmp( spl[ i ], cmp ) == 0;
    }
    va_end( vaList );

    free_spl( n_got, spl );
    return rv;
}
Tester test_one_strspl_regex( string_t haystack,
                              string_t regex_str,
                              int regex_flags,
                              strsplit_mode_t mode,
                              size_t n_split,
                              ... )
{
    regex_t reg;
    int recomp_rv = regcomp( &reg, regex_str, regex_flags );
    if ( recomp_rv == -1 )
    {
        char buff[ 128 ];
        regerror( recomp_rv, &reg, buff, 128 );
        errx( 2, "regcomp: %s", buff );
    }

    str_t *spl;
    ssize_t strspl_rv = string_split_regex( &spl, haystack, &reg, mode );
    if ( strspl_rv < 0 )
        exit( 1 );

    size_t n_got = ( size_t ) strspl_rv;

    bool rv = n_got == n_split;
    if ( !rv )
    {
        free_spl( n_got, spl );
        return false;
    }

    va_list vaList;
    va_start( vaList, n_split );
    for ( size_t i = 0; i < n_got && rv; ++i )
    {
        string_t cmp = va_arg( vaList, string_t );
        rv           = strcmp( spl[ i ], cmp ) == 0;
    }
    va_end( vaList );

    free_spl( n_got, spl );
    regfree( &reg );

    return rv;
}

typedef void( StringToUL )( str_t );
typedef str_t( StringAsUL )( string_t );

Tester test_one_string_to_UL( StringToUL func, string_t old, string_t new )
{
    str_t s = strdup( old );
    func( s );
    bool rv = strcmp( s, new ) == 0;
    free( s );
    return rv;
}
Tester test_one_string_as_UL( StringAsUL func, string_t old, string_t res )
{
    str_t got = func( old );
    bool rv   = strcmp( got, res ) == 0;
    free( got );
    return rv;
}


TEST( replace )
{
    UNIT_TEST( test_one_replace( "Hopspop", "p", "-", "Ho-s-o-" ) );
    UNIT_TEST( test_one_replace( "Hops Pop", "p", "-", "Ho-s Po-" ) );
    UNIT_TEST( test_one_replace( "Hopspop", "p", "12", "Ho12s12o12" ) );
    UNIT_TEST( test_one_replace( "HoHo a PiPi", "Ho", "Pi", "PiPi a PiPi" ) );

    UNIT_TEST( test_one_replace( "HoHo a PiPi", "Ho", "-", "-- a PiPi" ) );

    UNIT_TEST( test_one_replace(
            "Karel Vana, Karel Hoptysek a Karel \"Homosexual\" Hudlecka",
            "Karel",
            "0",
            "0 Vana, 0 Hoptysek a 0 \"Homosexual\" Hudlecka" ) );

    UNIT_TEST( test_one_replace(
            "Karel Vana, Karel Hoptysek a Karel \"Homosexual\" Hudlecka",
            "Homosexual",
            "Mistr v prevlekani",
            "Karel Vana, Karel Hoptysek a Karel \"Mistr v prevlekani\" Hudlecka" ) );


    UNIT_TEST( test_one_replace( "Hopspop", "p", "", "Hoso" ) );
    UNIT_TEST( test_one_replace( "Hopspop", "Hopspop", "Hopspop", "Hopspop" ) );
    UNIT_TEST( test_one_replace( "Hopspop", "Hopspop", "Hopcpop", "Hopcpop" ) );
    UNIT_TEST( test_one_replace( "Hopspop", "P", "_", "Hopspop" ) );

    UNIT_TEST( string_replaced( "To je jedno", "", "tohle taky" ) == NULL );
}
END_TEST

TEST( appendn )
{
    UNIT_TEST( test_one_appendn( "Hops", "pop", 3, "Hopspop" ) );
    UNIT_TEST( test_one_appendn( "Hops", "pop", 1, "Hopsp" ) );
    UNIT_TEST( test_one_appendn( "", *( &"Hopspop" ) + 1, 3, "ops" ) );
}
END_TEST

TEST( escape )
{
    UNIT_TEST( test_one_escape( "Hopspop", "Hopspop" ) );
    UNIT_TEST( test_one_escape( "Hopspop\n", "Hopspop\\n" ) );
    UNIT_TEST( test_one_escape( "Hopspop\nKokot\n", "Hopspop\\nKokot\\n" ) );
    UNIT_TEST( test_one_escape( "\"Hopspop\"\n", "\\\"Hopspop\\\"\\n" ) );
    UNIT_TEST( test_one_escape( "Hopspop\\nKokot\n", "Hopspop\\\\nKokot\\n" ) );
    UNIT_TEST( test_one_escape( "Hopspop\0Kokot\n", "Hopspop" ) );
    UNIT_TEST( test_one_escape( "\n\r\v\t\\\"", "\\n\\r\\v\\t\\\\\\\"" ) );
}
END_TEST

#define HOVEN_IPSUM "Hovno. Prdel. Sracka. Kokot."

#define CSV_STR "Hovno;Prdel;Sracka;Kokot"

#define INCONSISTENT_CSV_STR "Hovno,Prdel;Sracka,Kokot"

TEST( strspl_str )
{
    UNIT_TEST( test_one_strspl_str( HOVEN_IPSUM,
                                    ".",
                                    STRSPLIT_KEEP_DELIM_PRE,
                                    5,
                                    "Hovno.",
                                    " Prdel.",
                                    " Sracka.",
                                    " Kokot.",
                                    "" ) );

    UNIT_TEST( test_one_strspl_str( HOVEN_IPSUM,
                                    ".",
                                    STRSPLIT_KEEP_DELIM_POST,
                                    5,
                                    "Hovno",
                                    ". Prdel",
                                    ". Sracka",
                                    ". Kokot",
                                    "." ) );

    UNIT_TEST( test_one_strspl_str( HOVEN_IPSUM,
                                    ". ",
                                    STRSPLIT_KEEP_DELIM_PRE,
                                    4,
                                    "Hovno. ",
                                    "Prdel. ",
                                    "Sracka. ",
                                    "Kokot." ) );

    UNIT_TEST( test_one_strspl_str( CSV_STR, ".", 0xFF, 1, CSV_STR ) );

    UNIT_TEST( test_one_strspl_str(
            CSV_STR, ";", 0, 4, "Hovno", "Prdel", "Sracka", "Kokot" ) );
    UNIT_TEST( test_one_strspl_str(
            CSV_STR ";", ";", 0, 5, "Hovno", "Prdel", "Sracka", "Kokot", "" ) );

    UNIT_TEST( test_one_strspl_str( CSV_STR ";",
                                    ";",
                                    STRSPLIT_EXCLUDE_EMPTY,
                                    4,
                                    "Hovno",
                                    "Prdel",
                                    "Sracka",
                                    "Kokot" ) );

    UNIT_TEST( test_one_strspl_str( ",,,,,,,", ",", STRSPLIT_EXCLUDE_EMPTY, 0 ) );

    /* "", ",", ",Ho", ",", ",ps", ",", "," */
    UNIT_TEST( test_one_strspl_str( ",,Ho,,ps,,",
                                    ",",
                                    STRSPLIT_KEEP_DELIM_POST,
                                    7,
                                    "",
                                    ",",
                                    ",Ho",
                                    ",",
                                    ",ps",
                                    ",",
                                    "," ) );

    /* same as before, without the "" */
    UNIT_TEST( test_one_strspl_str( ",,Ho,,ps,,",
                                    ",",
                                    STRSPLIT_KEEP_DELIM_POST | STRSPLIT_EXCLUDE_EMPTY,
                                    6,
                                    ",",
                                    ",Ho",
                                    ",",
                                    ",ps",
                                    ",",
                                    "," ) );

    UNIT_TEST( test_one_strspl_str( ",,Ho,,ps,,",
                                    ",,",
                                    STRSPLIT_KEEP_DELIM_PRE | STRSPLIT_KEEP_DELIM_POST,
                                    4,
                                    ",,",
                                    ",,Ho,,",
                                    ",,ps,,",
                                    ",," ) );

    UNIT_TEST( test_one_strspl_str( "ABCDEF", "", 0, 6, "A", "B", "C", "D", "E", "F" ) );
}
END_TEST

TEST( strspl_regex )
{
    UNIT_TEST( test_one_strspl_regex( "a;b;c;d", ";", 0, 0, 4, "a", "b", "c", "d" ) );
    UNIT_TEST( test_one_strspl_regex( "a;b;c;d", "0", 0, 0, 1, "a;b;c;d" ) );

    UNIT_TEST( test_one_strspl_regex( "Hopspop\nPipspop\nKokot\n",
                                      "pop\n",
                                      REG_NEWLINE,
                                      STRSPLIT_KEEP_DELIM_PRE,
                                      3,
                                      "Hopspop\n",
                                      "Pipspop\n",
                                      "Kokot\n" ) );

    UNIT_TEST( test_one_strspl_regex( HOVEN_IPSUM,
                                      "\\.",
                                      0,
                                      STRSPLIT_EXCLUDE_EMPTY,
                                      4,
                                      "Hovno",
                                      " Prdel",
                                      " Sracka",
                                      " Kokot" ) );

    UNIT_TEST( test_one_strspl_regex( INCONSISTENT_CSV_STR,
                                      "[,;]",
                                      0,
                                      0,
                                      4,
                                      "Hovno",
                                      "Prdel",
                                      "Sracka",
                                      "Kokot" ) );

    UNIT_TEST( test_one_strspl_regex( INCONSISTENT_CSV_STR,
                                      "[,;]",
                                      0,
                                      STRSPLIT_KEEP_DELIM_PRE,
                                      4,
                                      "Hovno,",
                                      "Prdel;",
                                      "Sracka,",
                                      "Kokot" ) );

    UNIT_TEST( test_one_strspl_regex( INCONSISTENT_CSV_STR,
                                      "[,;]",
                                      0,
                                      STRSPLIT_KEEP_DELIM_POST,
                                      4,
                                      "Hovno",
                                      ",Prdel",
                                      ";Sracka",
                                      ",Kokot" ) );

    UNIT_TEST( test_one_strspl_regex(
            ";;a,,b,c,;d;e", "[,;]", 0, 0, 9, "", "", "a", "", "b", "c", "", "d", "e" ) );

    UNIT_TEST( test_one_strspl_regex( ";;a,,b,c,;d;e",
                                      "[,;]",
                                      0,
                                      STRSPLIT_EXCLUDE_EMPTY,
                                      5,
                                      "a",
                                      "b",
                                      "c",
                                      "d",
                                      "e" ) );

    UNIT_TEST( test_one_strspl_regex( ";;a,,b,c,;d;e",
                                      "[,;]",
                                      0,
                                      STRSPLIT_KEEP_DELIM_PRE | STRSPLIT_KEEP_DELIM_POST,
                                      9,
                                      ";",
                                      ";;",
                                      ";a,",
                                      ",,",
                                      ",b,",
                                      ",c,",
                                      ",;",
                                      ";d;",
                                      ";e" ) );

    UNIT_TEST( test_one_strspl_regex( ",,,,,,,", ",", 0, STRSPLIT_EXCLUDE_EMPTY, 0 ) );
}
END_TEST

TEST( string_to_UL )
{
    UNIT_TEST(
            test_one_string_to_UL( string_to_lower, "Hello, World!", "hello, world!" ) );
    UNIT_TEST(
            test_one_string_to_UL( string_to_lower, "hello, world!", "hello, world!" ) );
    UNIT_TEST(
            test_one_string_to_UL( string_to_lower, "HELLO, WORLD!", "hello, world!" ) );
    UNIT_TEST( test_one_string_to_UL( string_to_lower, "HELLO", "hello" ) );

    UNIT_TEST(
            test_one_string_to_UL( string_to_upper, "Hello, World!", "HELLO, WORLD!" ) );
    UNIT_TEST(
            test_one_string_to_UL( string_to_upper, "HELLO, WORLD!", "HELLO, WORLD!" ) );
    UNIT_TEST(
            test_one_string_to_UL( string_to_upper, "hello, world!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_string_to_UL( string_to_upper, "hello", "HELLO" ) );
}
END_TEST

TEST( string_as_UL )
{
    UNIT_TEST(
            test_one_string_as_UL( string_as_lower, "Hello, World!", "hello, world!" ) );
    UNIT_TEST(
            test_one_string_as_UL( string_as_lower, "hello, world!", "hello, world!" ) );
    UNIT_TEST(
            test_one_string_as_UL( string_as_lower, "HELLO, WORLD!", "hello, world!" ) );
    UNIT_TEST( test_one_string_as_UL( string_as_lower, "HELLO", "hello" ) );

    UNIT_TEST(
            test_one_string_as_UL( string_as_upper, "Hello, World!", "HELLO, WORLD!" ) );
    UNIT_TEST(
            test_one_string_as_UL( string_as_upper, "HELLO, WORLD!", "HELLO, WORLD!" ) );
    UNIT_TEST(
            test_one_string_as_UL( string_as_upper, "hello, world!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_string_as_UL( string_as_upper, "hello", "HELLO" ) );
}
END_TEST


int main( int argc, string_t argv[] )
{
    UNUSED( argc && argv );

    RUN( replace );

    RUN( appendn );

    RUN( escape );

    RUN( strspl_str );
    RUN( strspl_regex );

    RUN( string_to_UL );
    RUN( string_as_UL );

    FINISH_TESTING();
}
