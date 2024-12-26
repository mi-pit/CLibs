//
// Created by MacBook on 24.12.2024.
//

#include "../string_utils.h"
#include "./assert_that.h"
#include "array_printf.h"

#include <err.h>
#include <stdarg.h> /* va_list */
#include <stdlib.h> /* free(), exit() */
#include <string.h> /* strcmp() */

#define LOREM_IPSUM                                                                      \
    "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Nullam rhoncus aliquam "  \
    "metus. Mauris metus. Nulla turpis magna, cursus sit amet, suscipit a, interdum "    \
    "id, felis. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis "  \
    "voluptatibus maiores alias consequatur aut perferendis doloribus asperiores "       \
    "repellat. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis "    \
    "suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Maecenas libero. "  \
    "Maecenas aliquet accumsan leo. Aliquam ante. Phasellus enim erat, vestibulum vel, " \
    "aliquam a, posuere eu, velit. Curabitur sagittis hendrerit ante. Maecenas libero. " \
    "Praesent in mauris eu tortor porttitor accumsan. Neque porro quisquam est, qui "    \
    "dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non "      \
    "numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat "     \
    "voluptatem. Etiam ligula pede, sagittis quis, interdum ultricies, scelerisque eu. " \
    "Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed "   \
    "quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt."

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

static void test_one_str( string_t haystack,
                          string_t split_tok,
                          strsplit_mode_t mode,
                          size_t n_split,
                          ... )
{
    str_t *spl;
    ssize_t rv = string_split( &spl, haystack, split_tok, mode );
    if ( rv < 0 )
        exit( 1 );

    size_t n_got = ( size_t ) rv;

    if ( PrintSplArray )
        array_printf_d( spl, n_got, string_t, "\"%s\"", "\n" );

    assert_that( n_got == n_split, "%zu == %zu", n_got, n_split );

    va_list vaList;
    va_start( vaList, n_split );
    for ( size_t i = 0; i < n_got; ++i )
    {
        string_t cmp = va_arg( vaList, string_t );
        assert_that( strcmp( spl[ i ], cmp ) == 0, "\"%s\" == \"%s\"", spl[ i ], cmp );
    }
    va_end( vaList );

    free_spl( n_got, spl );
}

static void test_one_regex( string_t haystack,
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
    ssize_t rv = string_split_regex( &spl, haystack, &reg, mode );
    if ( rv < 0 )
        exit( 1 );

    size_t n_got = ( size_t ) rv;

    if ( PrintSplArray )
        array_printf_d( spl, n_got, string_t, "\"%s\"", "\n" );

    assert_that( n_got == n_split, "%zu == %zu", n_got, n_split );

    va_list vaList;
    va_start( vaList, n_split );
    for ( size_t i = 0; i < n_got; ++i )
    {
        string_t cmp = va_arg( vaList, string_t );
        assert_that( strcmp( spl[ i ], cmp ) == 0, "\"%s\" == \"%s\"", spl[ i ], cmp );
    }
    va_end( vaList );

    free_spl( n_got, spl );
    regfree( &reg );
}


static void test_str( void )
{
    test_one_str( HOVEN_IPSUM,
                  ".",
                  STRSPLIT_KEEP_DELIM_PRE,
                  5,
                  "Hovno.",
                  " Prdel.",
                  " Sracka.",
                  " Kokot.",
                  "" );

    test_one_str( HOVEN_IPSUM,
                  ".",
                  STRSPLIT_KEEP_DELIM_POST,
                  5,
                  "Hovno",
                  ". Prdel",
                  ". Sracka",
                  ". Kokot",
                  "." );

    test_one_str( HOVEN_IPSUM,
                  ". ",
                  STRSPLIT_KEEP_DELIM_PRE,
                  4,
                  "Hovno. ",
                  "Prdel. ",
                  "Sracka. ",
                  "Kokot." );

    test_one_str( CSV_STR, ".", 0xFF, 1, CSV_STR );

    test_one_str( CSV_STR, ";", 0, 4, "Hovno", "Prdel", "Sracka", "Kokot" );
    test_one_str( CSV_STR ";", ";", 0, 5, "Hovno", "Prdel", "Sracka", "Kokot", "" );

    test_one_str( CSV_STR ";",
                  ";",
                  STRSPLIT_EXCLUDE_EMPTY,
                  4,
                  "Hovno",
                  "Prdel",
                  "Sracka",
                  "Kokot" );

    test_one_str( ",,,,,,,", ",", STRSPLIT_EXCLUDE_EMPTY, 0 );

    /* "", ",", ",Ho", ",", ",ps", ",", "," */
    test_one_str( ",,Ho,,ps,,",
                  ",",
                  STRSPLIT_KEEP_DELIM_POST,
                  7,
                  "",
                  ",",
                  ",Ho",
                  ",",
                  ",ps",
                  ",",
                  "," );

    /* same as before, without the "" */
    test_one_str( ",,Ho,,ps,,",
                  ",",
                  STRSPLIT_KEEP_DELIM_POST | STRSPLIT_EXCLUDE_EMPTY,
                  6,
                  ",",
                  ",Ho",
                  ",",
                  ",ps",
                  ",",
                  "," );

    test_one_str( ",,Ho,,ps,,",
                  ",,",
                  STRSPLIT_KEEP_DELIM_PRE | STRSPLIT_KEEP_DELIM_POST,
                  4,
                  ",,",
                  ",,Ho,,",
                  ",,ps,,",
                  ",," );

    test_one_str( "ABCDEF", "", 0, 6, "A", "B", "C", "D", "E", "F" );
}

static void test_regex( void )
{
    test_one_regex( "a;b;c;d", ";", 0, 0, 4, "a", "b", "c", "d" );
    test_one_regex( "a;b;c;d", "0", 0, 0, 1, "a;b;c;d" );

    test_one_regex( "Hopspop\nPipspop\nKokot\n",
                    "pop\n",
                    REG_NEWLINE,
                    STRSPLIT_KEEP_DELIM_PRE,
                    3,
                    "Hopspop\n",
                    "Pipspop\n",
                    "Kokot\n" );

    test_one_regex( HOVEN_IPSUM,
                    "\\.",
                    0,
                    STRSPLIT_EXCLUDE_EMPTY,
                    4,
                    "Hovno",
                    " Prdel",
                    " Sracka",
                    " Kokot" );

    test_one_regex(
            INCONSISTENT_CSV_STR, "[,;]", 0, 0, 4, "Hovno", "Prdel", "Sracka", "Kokot" );

    test_one_regex( INCONSISTENT_CSV_STR,
                    "[,;]",
                    0,
                    STRSPLIT_KEEP_DELIM_PRE,
                    4,
                    "Hovno,",
                    "Prdel;",
                    "Sracka,",
                    "Kokot" );

    test_one_regex( INCONSISTENT_CSV_STR,
                    "[,;]",
                    0,
                    STRSPLIT_KEEP_DELIM_POST,
                    4,
                    "Hovno",
                    ",Prdel",
                    ";Sracka",
                    ",Kokot" );

    test_one_regex(
            ";;a,,b,c,;d;e", "[,;]", 0, 0, 9, "", "", "a", "", "b", "c", "", "d", "e" );

    test_one_regex( ";;a,,b,c,;d;e",
                    "[,;]",
                    0,
                    STRSPLIT_EXCLUDE_EMPTY,
                    5,
                    "a",
                    "b",
                    "c",
                    "d",
                    "e" );

    test_one_regex( ";;a,,b,c,;d;e",
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
                    ";e" );

    test_one_regex( ",,,,,,,", ",", 0, STRSPLIT_EXCLUDE_EMPTY, 0 );
}

#define RUN_TEST( FUNC_CALL )                      \
    do                                             \
    {                                              \
        printf( "running  \"" #FUNC_CALL "\"\n" ); \
        ( FUNC_CALL );                             \
        printf( "finished \"" #FUNC_CALL "\"\n" ); \
    }                                              \
    while ( 0 )

int main( void )
{
    RUN_TEST( test_str() );
    RUN_TEST( test_regex() );
}
