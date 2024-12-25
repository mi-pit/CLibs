//
// Created by MacBook on 24.12.2024.
//

#include "../string_utils.h"

#include <assert.h> /* assert */
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

static void free_spl( const size_t count, str_t strings[ count ] )
{
    for ( size_t i = 0; i < count; ++i )
    {
        free( strings[ i ] );
    }
}

static void test_one( string_t haystack,
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
    assert( n_got == n_split );

    va_list vaList;
    va_start( vaList, n_split );
    for ( size_t i = 0; i < n_got; ++i )
        assert( strcmp( spl[ i ], va_arg( vaList, string_t ) ) == 0 );
    va_end( vaList );

    free_spl( n_got, spl );
}


int main( void )
{
    test_one( HOVEN_IPSUM,
              ".",
              STRSPLIT_KEEP_DELIM_PRE,
              5,
              "Hovno.",
              " Prdel.",
              " Sracka.",
              " Kokot.",
              "" );

    test_one( HOVEN_IPSUM,
              ".",
              STRSPLIT_KEEP_DELIM_POST,
              5,
              "Hovno",
              ". Prdel",
              ". Sracka",
              ". Kokot",
              "." );

    test_one( HOVEN_IPSUM,
              ". ",
              STRSPLIT_KEEP_DELIM_PRE,
              4,
              "Hovno. ",
              "Prdel. ",
              "Sracka. ",
              "Kokot." );

    test_one( CSV_STR, ".", 0xFF, 1, CSV_STR );

    test_one( CSV_STR, ";", 0, 4, "Hovno", "Prdel", "Sracka", "Kokot" );
    test_one( CSV_STR ";", ";", 0, 5, "Hovno", "Prdel", "Sracka", "Kokot", "" );

    test_one( CSV_STR ";",
              ";",
              STRSPLIT_EXCLUDE_EMPTY,
              4,
              "Hovno",
              "Prdel",
              "Sracka",
              "Kokot" );

    test_one( ",,,,,,,", ",", STRSPLIT_EXCLUDE_EMPTY, 0 );

    /* "", ",", ",Ho", ",", ",ps", ",", "," */
    test_one( ",,Ho,,ps,,",
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
    test_one( ",,Ho,,ps,,",
              ",",
              STRSPLIT_KEEP_DELIM_POST | STRSPLIT_EXCLUDE_EMPTY,
              6,
              ",",
              ",Ho",
              ",",
              ",ps",
              ",",
              "," );

    test_one( ",,Ho,,ps,,",
              ",,",
              STRSPLIT_KEEP_DELIM_PRE | STRSPLIT_KEEP_DELIM_POST,
              4,
              ",,",
              ",,Ho,,",
              ",,ps,,",
              ",," );

    return EXIT_SUCCESS;
}
