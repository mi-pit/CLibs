//
// Created by MacBook on 24.12.2024.
//

#include "../assert_that.h"
#include "../string_utils.h"
#include "array_printf.h"
#include "run_test.h"

#include <err.h>
#include <stdarg.h> /* va_list */
#include <stdlib.h> /* free(), exit() */
#include <string.h> /* strcmp() */

#define HOVEN_IPSUM "Hovno. Prdel. Sracka. Kokot."

#define CSV_STR "Hovno;Prdel;Sracka;Kokot"

#define INCONSISTENT_CSV_STR "Hovno,Prdel;Sracka,Kokot"

static void free_spl( const size_t count, str_t strings[ count ] )
{
    for ( size_t i = 0; i < count; ++i )
    {
        free( strings[ i ] );
    }
}

bool PrintSplArray = false;

static bool test_one_str( string_t haystack,
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

    if ( PrintSplArray )
        array_printf_d( spl, n_got, string_t, "\"%s\"", "\n" );

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

static bool test_one_regex( string_t haystack,
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

    if ( PrintSplArray )
        array_printf_d( spl, n_got, string_t, "\"%s\"", "\n" );

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


TEST( str )
{
    UNIT_TEST( test_one_str( HOVEN_IPSUM,
                             ".",
                             STRSPLIT_KEEP_DELIM_PRE,
                             5,
                             "Hovno.",
                             " Prdel.",
                             " Sracka.",
                             " Kokot.",
                             "" ) );

    UNIT_TEST( test_one_str( HOVEN_IPSUM,
                             ".",
                             STRSPLIT_KEEP_DELIM_POST,
                             5,
                             "Hovno",
                             ". Prdel",
                             ". Sracka",
                             ". Kokot",
                             "." ) );

    UNIT_TEST( test_one_str( HOVEN_IPSUM,
                             ". ",
                             STRSPLIT_KEEP_DELIM_PRE,
                             4,
                             "Hovno. ",
                             "Prdel. ",
                             "Sracka. ",
                             "Kokot." ) );

    UNIT_TEST( test_one_str( CSV_STR, ".", 0xFF, 1, CSV_STR ) );

    UNIT_TEST( test_one_str( CSV_STR, ";", 0, 4, "Hovno", "Prdel", "Sracka", "Kokot" ) );
    UNIT_TEST( test_one_str(
            CSV_STR ";", ";", 0, 5, "Hovno", "Prdel", "Sracka", "Kokot", "" ) );

    UNIT_TEST( test_one_str( CSV_STR ";",
                             ";",
                             STRSPLIT_EXCLUDE_EMPTY,
                             4,
                             "Hovno",
                             "Prdel",
                             "Sracka",
                             "Kokot" ) );

    UNIT_TEST( test_one_str( ",,,,,,,", ",", STRSPLIT_EXCLUDE_EMPTY, 0 ) );

    /* "", ",", ",Ho", ",", ",ps", ",", "," */
    UNIT_TEST( test_one_str( ",,Ho,,ps,,",
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
    UNIT_TEST( test_one_str( ",,Ho,,ps,,",
                             ",",
                             STRSPLIT_KEEP_DELIM_POST | STRSPLIT_EXCLUDE_EMPTY,
                             6,
                             ",",
                             ",Ho",
                             ",",
                             ",ps",
                             ",",
                             "," ) );

    UNIT_TEST( test_one_str( ",,Ho,,ps,,",
                             ",,",
                             STRSPLIT_KEEP_DELIM_PRE | STRSPLIT_KEEP_DELIM_POST,
                             4,
                             ",,",
                             ",,Ho,,",
                             ",,ps,,",
                             ",," ) );

    UNIT_TEST( test_one_str( "ABCDEF", "", 0, 6, "A", "B", "C", "D", "E", "F" ) );
}
END_TEST

TEST( regex )
{
    UNIT_TEST( test_one_regex( "a;b;c;d", ";", 0, 0, 4, "a", "b", "c", "d" ) );
    UNIT_TEST( test_one_regex( "a;b;c;d", "0", 0, 0, 1, "a;b;c;d" ) );

    UNIT_TEST( test_one_regex( "Hopspop\nPipspop\nKokot\n",
                               "pop\n",
                               REG_NEWLINE,
                               STRSPLIT_KEEP_DELIM_PRE,
                               3,
                               "Hopspop\n",
                               "Pipspop\n",
                               "Kokot\n" ) );

    UNIT_TEST( test_one_regex( HOVEN_IPSUM,
                               "\\.",
                               0,
                               STRSPLIT_EXCLUDE_EMPTY,
                               4,
                               "Hovno",
                               " Prdel",
                               " Sracka",
                               " Kokot" ) );

    UNIT_TEST( test_one_regex( INCONSISTENT_CSV_STR,
                               "[,;]",
                               0,
                               0,
                               4,
                               "Hovno",
                               "Prdel",
                               "Sracka",
                               "Kokot" ) );

    UNIT_TEST( test_one_regex( INCONSISTENT_CSV_STR,
                               "[,;]",
                               0,
                               STRSPLIT_KEEP_DELIM_PRE,
                               4,
                               "Hovno,",
                               "Prdel;",
                               "Sracka,",
                               "Kokot" ) );

    UNIT_TEST( test_one_regex( INCONSISTENT_CSV_STR,
                               "[,;]",
                               0,
                               STRSPLIT_KEEP_DELIM_POST,
                               4,
                               "Hovno",
                               ",Prdel",
                               ";Sracka",
                               ",Kokot" ) );

    UNIT_TEST( test_one_regex(
            ";;a,,b,c,;d;e", "[,;]", 0, 0, 9, "", "", "a", "", "b", "c", "", "d", "e" ) );

    UNIT_TEST( test_one_regex( ";;a,,b,c,;d;e",
                               "[,;]",
                               0,
                               STRSPLIT_EXCLUDE_EMPTY,
                               5,
                               "a",
                               "b",
                               "c",
                               "d",
                               "e" ) );

    UNIT_TEST( test_one_regex( ";;a,,b,c,;d;e",
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

    UNIT_TEST( test_one_regex( ",,,,,,,", ",", 0, STRSPLIT_EXCLUDE_EMPTY, 0 ) );
}
END_TEST

int main( void )
{
    RUN( str );
    RUN( regex );
}
