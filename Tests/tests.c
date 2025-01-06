//
// Created by MacBook on 06.01.2025.
//
#include "../assert_that.h"
#include "../dynarr.h"
#include "../dynstring.h"
#include "../foreach.h"
#include "../misc.h"
#include "../string_utils.h"
#include "../swexpr.h"
#include "../unit_tests.h"

#include <stdlib.h>

#define Tester static bool
// todo?: #define TESTER( NAME ) static bool test_one_##NAME


Tester test_one_replace( string_t orig, string_t old, string_t new, string_t expected )
{
    str_t got = string_replaced( orig, old, new );
    bool rv   = strcmp( got, expected ) == 0;
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


Tester test_one_appendn( string_t init, string_t app, size_t len, string_t expected )
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


Tester test_one_escape( string_t old, string_t new )
{
    str_t esc = string_escaped( old );
    bool rv   = strcmp( esc, new ) == 0;
    free( esc );
    return rv;
}

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


static void free_spl( const size_t count, str_t strings[ count ] )
{
    for ( size_t i = 0; i < count; ++i )
    {
        free( strings[ i ] );
    }
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
    {
        ffl_stack_trace();
        exit( 1 );
    }

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
    {
        ffl_stack_trace();
        exit( -1 );
    }

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

#define HOVEN_IPSUM "Hovno. Prdel. Sracka. Kokot."

#define CSV_STR "Hovno;Prdel;Sracka;Kokot"

#define INCONSISTENT_CSV_STR "Hovno,Prdel;Sracka,Kokot"

TEST( strspl_str )
{
    UNIT_TEST( test_one_strspl_str( HOVEN_IPSUM,
                                    ".",
                                    STRSPLIT_KEEP_DELIM_BEFORE,
                                    5,
                                    "Hovno.",
                                    " Prdel.",
                                    " Sracka.",
                                    " Kokot.",
                                    "" ) );

    UNIT_TEST( test_one_strspl_str( HOVEN_IPSUM,
                                    ".",
                                    STRSPLIT_KEEP_DELIM_AFTER,
                                    5,
                                    "Hovno",
                                    ". Prdel",
                                    ". Sracka",
                                    ". Kokot",
                                    "." ) );

    UNIT_TEST( test_one_strspl_str( HOVEN_IPSUM,
                                    ". ",
                                    STRSPLIT_KEEP_DELIM_BEFORE,
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
                                    STRSPLIT_KEEP_DELIM_AFTER,
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
                                    STRSPLIT_KEEP_DELIM_AFTER | STRSPLIT_EXCLUDE_EMPTY,
                                    6,
                                    ",",
                                    ",Ho",
                                    ",",
                                    ",ps",
                                    ",",
                                    "," ) );

    UNIT_TEST(
            test_one_strspl_str( ",,Ho,,ps,,",
                                 ",,",
                                 STRSPLIT_KEEP_DELIM_BEFORE | STRSPLIT_KEEP_DELIM_AFTER,
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
                                      STRSPLIT_KEEP_DELIM_BEFORE,
                                      3,
                                      "Hopspop\n",
                                      "Pipspop\n",
                                      "Kokot\n" ) );

    UNIT_TEST( test_one_strspl_regex( "Hopspop\nPipspop\nKokot\n",
                                      "\n",
                                      REG_NEWLINE,
                                      STRSPLIT_KEEP_DELIM_BEFORE | STRSPLIT_EXCLUDE_EMPTY,
                                      3,
                                      "Hopspop\n",
                                      "Pipspop\n",
                                      "Kokot\n" ) );

    UNIT_TEST( test_one_strspl_regex( "Hopspop\nPipspop\nKokot\n",
                                      "\n",
                                      REG_NEWLINE,
                                      STRSPLIT_KEEP_DELIM_AFTER | STRSPLIT_EXCLUDE_EMPTY,
                                      4,
                                      "Hopspop",
                                      "\nPipspop",
                                      "\nKokot",
                                      "\n" ) );

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
                                      STRSPLIT_KEEP_DELIM_BEFORE,
                                      4,
                                      "Hovno,",
                                      "Prdel;",
                                      "Sracka,",
                                      "Kokot" ) );

    UNIT_TEST( test_one_strspl_regex( INCONSISTENT_CSV_STR,
                                      "[,;]",
                                      0,
                                      STRSPLIT_KEEP_DELIM_AFTER,
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

    UNIT_TEST(
            test_one_strspl_regex( ";;a,,b,c,;d;e",
                                   "[,;]",
                                   0,
                                   STRSPLIT_KEEP_DELIM_BEFORE | STRSPLIT_KEEP_DELIM_AFTER,
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


typedef void( StringToUL )( str_t );
Tester test_one_string_to_UL( StringToUL func, string_t old, string_t new )
{
    str_t s = strdup( old );
    func( s );
    bool rv = strcmp( s, new ) == 0;
    free( s );
    return rv;
}

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

typedef str_t( StringAsUL )( string_t );
Tester test_one_string_as_UL( StringAsUL func, string_t old, string_t res )
{
    str_t got = func( old );
    bool rv   = strcmp( got, res ) == 0;
    free( got );
    return rv;
}

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


Tester test_one_reverse( string_t orig, string_t result )
{
    str_t rev = strdup( orig );
    string_reverse( rev );
    bool rv = strcmp( rev, result ) == 0;
    free( rev );

    if ( !rv )
        return rv;

    rev = string_reversed( orig );
    rv  = strcmp( rev, result ) == 0;
    free( rev );
    return rv;
}

TEST( reverse )
{
    UNIT_TEST( test_one_reverse( "Hovno", "onvoH" ) );
    UNIT_TEST( test_one_reverse( "", "" ) );
    UNIT_TEST( test_one_reverse( "A", "A" ) );

    UNIT_TEST( test_one_reverse( "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                                 "ZYXWVUTSRQPONMLKJIHGFEDCBA" ) );

    UNIT_TEST( test_one_reverse( "Karel\tVana\n", "\nanaV\tleraK" ) );
    UNIT_TEST( test_one_reverse( "\n\r\"\0\"", "\"\r\n" ) );
}
END_TEST


Tester test_one_foreach_arr( const int64_t arr[], const size_t count )
{
    foreach_arr( int64_t, num, arr, count )
    {
        if ( num != arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_ls( ConstList const numbers_ls,
                            const int64_t numbers_arr[],
                            const size_t count )
{
    assert_that( list_size( numbers_ls ) == count, "list size != array size" );

    foreach_ls( int64_t, num, numbers_ls )
    {
        if ( num != numbers_arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_uni( const int64_t numbers_arr[], const size_t count )
{
    foreach_uni( int64_t, num, numbers_arr[ foreach_index_num ], count )
    {
        if ( num != numbers_arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_str( string_t str )
{
    foreach_str( c, str )
    {
        if ( foreach_cap_c != strlen( str ) )
        {
            fflwarnx( "foreach_cap" );
            return false;
        }
        if ( c != str[ foreach_index_c ] )
        {
            fflwarnx( "c != str[ foreach_idx ]" );
            return false;
        }
    }
    return true;
}
Tester test_one_foreach_dynstr( string_t str )
{
    DynamicString dynstr = dynstr_init_as( str );
    assert_that( dynstr != NULL, "dynstr init" );
    assert_that( strlen( str ) == dynstr_len( dynstr ), "dynstr len" );

    foreach_str( c, dynstr_data( dynstr ) )
    {
        if ( foreach_cap_c != strlen( str ) )
        {
            fflwarnx( "foreach_cap" );
            return false;
        }
        if ( c != str[ foreach_index_c ] )
        {
            fflwarnx( "c != str[ foreach_idx ]" );
            return false;
        }
    }
    return true;
}

TEST( foreach )
{
    List numbers_ls = list_init_type( int64_t );
    assert_that( numbers_ls != NULL, "list init" );
    int64_t numbers_arr[] = {
        1, 2, 4, 6, 7, -1, 2323, 3195,
    };
    assert_that( list_extend( numbers_ls, numbers_arr, countof( numbers_arr ) ) ==
                         RV_SUCCESS,
                 "list extend" );

    UNIT_TEST( test_one_foreach_arr( numbers_arr, countof( numbers_arr ) ) );
    UNIT_TEST( test_one_foreach_ls( numbers_ls, numbers_arr, countof( numbers_arr ) ) );
    UNIT_TEST( test_one_foreach_uni( numbers_arr, countof( numbers_arr ) ) );

    UNIT_TEST( test_one_foreach_str( "" ) );
    UNIT_TEST( test_one_foreach_str( "HOVNO" ) );
    UNIT_TEST( test_one_foreach_str( "a\nb'\0'" ) );

    UNIT_TEST( test_one_foreach_dynstr( "" ) );
    UNIT_TEST( test_one_foreach_dynstr( "HOVNO" ) );
    UNIT_TEST( test_one_foreach_dynstr( "a\nb'\0'" ) );
}
END_TEST


static int get_swex_val( int branch_1, int branch_2, string_t branch_3 )
{
    swex_init_val( int, branch_1 ) as_new( int, rv )
    {
        swex_case_imm( int, 1 )
        {
            swex_init_val( int, branch_2 ) as( rv )
            {
                swex_case_imm( int, 1 )
                {
                    resolve( int, 10 );
                }
                swex_case_imm( int, 2 )
                {
                    swex_init_str( branch_3 ) as( rv )
                    {
                        swex_case_str( "1" )
                        {
                            resolve( int, 100 );
                        }
                        swex_default()
                        {
                            resolve( int, -100 );
                        }
                    }
                    swex_finish();
                }
                swex_default()
                {
                    resolve( int, -10 );
                }
            }
            swex_finish();
        }
        swex_case_imm( int, 2 )
        {
            resolve( int, 2 );
        }

        swex_default()
        {
            resolve( int, -1 );
        }
    }
    swex_finish();

    return rv;
}
Tester test_one_swex( int branch_1, int branch_2, string_t branch_3, int rv )
{
    return get_swex_val( branch_1, branch_2, branch_3 ) == rv;
}

TEST( swex )
{
    UNIT_TEST( test_one_swex( -1, 0, NULL, -1 ) );
    UNIT_TEST( test_one_swex( -1, 124, "hops", -1 ) );
    UNIT_TEST( test_one_swex( -100, 0, NULL, -1 ) );

    UNIT_TEST( test_one_swex( 2, 0, NULL, 2 ) );
    UNIT_TEST( test_one_swex( 2, 325, "NULL", 2 ) );

    UNIT_TEST( test_one_swex( 1, 1, NULL, 10 ) );
    UNIT_TEST( test_one_swex( 1, 1, "HOPSOPPO", 10 ) );

    UNIT_TEST( test_one_swex( 1, -11, NULL, -10 ) );

    UNIT_TEST( test_one_swex( 1, 2, "1", 100 ) );
    UNIT_TEST( test_one_swex( 1, 2, "Hovno", -100 ) );
}
END_TEST


Tester test_one_strip( string_t orig, string_t desired )
{
    str_t s = strdup( orig );
    assert_that( s != NULL, "strdup" );
    string_strip( s );
    bool rv = strcmp( s, desired ) == 0;
    free( s );
    if ( !rv )
        return false;

    s  = string_stripped( orig );
    rv = strcmp( s, desired ) == 0;
    free( s );
    return rv;
}

TEST( strip )
{
    UNIT_TEST( test_one_strip( "Hello", "Hello" ) );
    UNIT_TEST( test_one_strip( " Hello", "Hello" ) );
    UNIT_TEST( test_one_strip( "Hello ", "Hello" ) );
    UNIT_TEST( test_one_strip( " Hello ", "Hello" ) );

    UNIT_TEST( test_one_strip( " Hello \0 TOTO TU NENI", "Hello" ) );

    UNIT_TEST( test_one_strip( "\n\t\tHello \t\r \v ", "Hello" ) );
    UNIT_TEST( test_one_strip( "\n\t\tHello \t\rKokote\v ", "Hello \t\rKokote" ) );
}
END_TEST

TEST( misc )
{
    UNIT_TEST( strcmp( get_file_name( "/Users/macbook/Hovno" ), "Hovno" ) == 0 );
    UNIT_TEST( strcmp( get_file_name( "Hovno" ), "Hovno" ) == 0 );
    UNIT_TEST( strcmp( get_file_name( "/a" ), "a" ) == 0 );
    UNIT_TEST( strcmp( get_file_name( "./a" ), "a" ) == 0 );
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

    RUN( reverse );

    RUN( foreach );

    RUN( swex );

    RUN( strip );

    RUN( misc );

    FINISH_TESTING();
}
