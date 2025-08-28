//
// Created by MacBook on 08.01.2025.
//
#ifndef TEST_STRING_UTILS_H
#define TEST_STRING_UTILS_H

#include "../../src/headers/assert_that.h"
#include "../../src/headers/unit_tests.h"
#include "../../src/string_utils.h"


TEST( replace )
{
    char buffer[ 128 ] = "This,is,a,string.";

    UNIT_TEST( string_replace( buffer, ",", " " ) == RV_SUCCESS );
    UNIT_TEST( strcmp( buffer, "This is a string." ) == 0 );

    UNIT_TEST( string_replace( buffer, NULL, "str" ) == RV_EXCEPTION );
    UNIT_TEST( strcmp( buffer, "This is a string." ) == 0 );

    UNIT_TEST( string_replace( buffer, "str", NULL ) == RV_EXCEPTION );
    UNIT_TEST( string_replace( NULL, "", "" ) == RV_EXCEPTION );

    UNIT_TEST( string_replace( buffer, " ", "long replacement" ) == RV_EXCEPTION );
    UNIT_TEST( strcmp( buffer, "This is a string." ) == 0 );

    UNIT_TEST( string_replace( buffer, "long", "sh" ) == RV_EXCEPTION );
    UNIT_TEST( strcmp( buffer, "This is a string." ) == 0 );

    UNIT_TEST( string_replace( buffer, "", "" ) == RV_SUCCESS );
    UNIT_TEST( strcmp( buffer, "This is a string." ) == 0 );

    UNIT_TEST( string_replace( buffer, "", "" ) == RV_SUCCESS );
}
END_TEST


Private bool test_one_replace( string_t orig, string_t old, string_t new,
                               string_t expected )
{
    str_t got = string_replaced( orig, old, new );
    bool rv   = strcmp( got, expected ) == 0;
    free( got );

    return rv;
}

TEST( replaced )
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


Private bool test_one_escape( string_t old, string_t new )
{
    str_t esc = string_escaped( old );
    bool rv   = strcmp( esc, new ) == 0;
    free( esc );
    return rv;
}

TEST( escape )
{
    UNIT_TEST( test_one_escape( "", "" ) );
    UNIT_TEST( test_one_escape( "Hopspop", "Hopspop" ) );
    UNIT_TEST( test_one_escape( "Hopspop\n", "Hopspop\\n" ) );
    UNIT_TEST( test_one_escape( "Hopspop\nKokot\n", "Hopspop\\nKokot\\n" ) );
    UNIT_TEST( test_one_escape( "\"Hopspop\"\n", "\\\"Hopspop\\\"\\n" ) );
    UNIT_TEST( test_one_escape( "Hopspop\\nKokot\n", "Hopspop\\\\nKokot\\n" ) );
    UNIT_TEST( test_one_escape( "Hopspop\0Kokot\n", "Hopspop" ) );
    UNIT_TEST( test_one_escape( "\n\r\v\t\\\"", "\\n\\r\\v\\t\\\\\\\"" ) );
}
END_TEST


Private bool test_one_unescape( string_t old, string_t new )
{
    str_t esc = string_unescaped( old );
    bool rv   = strcmp( esc, new ) == 0;
    free( esc );
    return rv;
}

TEST( unescape )
{
    UNIT_TEST( test_one_unescape( "", "" ) );
    UNIT_TEST( test_one_unescape( "Hopspop", "Hopspop" ) );
    UNIT_TEST( test_one_unescape( "Hopspop\\n", "Hopspop\n" ) );
    UNIT_TEST( test_one_unescape( "Hopspop\n", "Hopspop\n" ) );
    UNIT_TEST( test_one_unescape( "\\n\\r\\v\\t\\\\\\\"", "\n\r\v\t\\\"" ) );
    UNIT_TEST( test_one_unescape( "Hops\0pop", "Hops" ) );
}
END_TEST


Private bool test_one_strspl_str( string_t haystack,
                                  string_t split_tok,
                                  strsplit_mode_t mode,
                                  size_t n_split,
                                  ... )
{
    str_t *spl;
    const ssize_t strspl_rv = string_split( &spl, haystack, split_tok, mode );
    if ( strspl_rv < 0 )
        exit( ffl_stack_trace( 1 ) );

    const size_t n_got = ( size_t ) strspl_rv;

    if ( n_got != n_split )
    {
        string_split_destroy( n_got, &spl );
        return false;
    }

    bool rv = true;
    va_list vaList;
    va_start( vaList, n_split );
    for ( size_t i = 0; i < n_got && rv; ++i )
    {
        const string_t cmp = va_arg( vaList, string_t );
        rv                 = strcmp( spl[ i ], cmp ) == 0;
    }
    va_end( vaList );

    string_split_destroy( n_got, &spl );
    return rv;
}
Private bool test_one_strspl_regex( string_t haystack,
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
        ffl_stack_trace( 0 );
        exit( -1 );
    }

    const size_t n_got = ( size_t ) strspl_rv;

    bool rv = n_got == n_split;
    if ( !rv )
    {
        string_split_destroy( n_got, &spl );
        return false;
    }

    va_list vaList;
    va_start( vaList, n_split );
    for ( size_t i = 0; i < n_got && rv; ++i )
    {
        const string_t cmp = va_arg( vaList, string_t );
        rv                 = strcmp( spl[ i ], cmp ) == 0;
    }
    va_end( vaList );

    string_split_destroy( n_got, &spl );
    regfree( &reg );

    return rv;
}

#define HOVEN_IPSUM          "Hovno. Prdel. Sracka. Kokot."
#define CSV_STR              "Hovno;Prdel;Sracka;Kokot"
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

    UNIT_TEST( test_one_strspl_str( CSV_STR, ".", 0, 1, CSV_STR ) );

    UNIT_TEST( test_one_strspl_str( CSV_STR, ";", 0, 4, "Hovno", "Prdel", "Sracka",
                                    "Kokot" ) );
    UNIT_TEST( test_one_strspl_str( CSV_STR ";", ";", 0, 5, "Hovno", "Prdel", "Sracka",
                                    "Kokot", "" ) );

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

    UNIT_TEST( test_one_strspl_str( "", "", 0, 0 ) );

    UNIT_TEST( test_one_strspl_str( "Jedna, Dva, Tri , Ctyri ",
                                    ",",
                                    STRSPLIT_STRIP_RESULTS,
                                    4,
                                    "Jedna",
                                    "Dva",
                                    "Tri",
                                    "Ctyri" ) );

    UNIT_TEST( test_one_strspl_str( "\n \rJedna, Dva, Tri , \t \n\n\r \v\v Ctyri \n", ",",
                                    STRSPLIT_STRIP_RESULTS, 4, "Jedna", "Dva", "Tri",
                                    "Ctyri" ) );

    UNIT_TEST( test_one_strspl_str( " ,   ,,neco", ",",
                                    STRSPLIT_STRIP_RESULTS | STRSPLIT_EXCLUDE_EMPTY, 1,
                                    "neco" ) );

    UNIT_TEST( test_one_strspl_str( " dva tri", " ", 0, 3, "", "dva", "tri" ) );

    UNIT_TEST( test_one_strspl_str( " dva tri", " ", STRSPLIT_STRIP_RESULTS, 3, "", "dva",
                                    "tri" ) );
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

    UNIT_TEST( test_one_strspl_regex( ";;a,,b,c,;d;e", "[,;]", 0, 0, 9, "", "", "a", "",
                                      "b", "c", "", "d", "e" ) );

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


#define test_one_join( WANTED, JOINER, ... )                               \
    do                                                                     \
    {                                                                      \
        const string_t arr[] = { __VA_ARGS__ };                            \
        str_t got            = string_join( countof( arr ), arr, JOINER ); \
        UNIT_TEST( strcmp( WANTED, got ) == 0 );                           \
        free( got );                                                       \
    }                                                                      \
    while ( 0 )

TEST( join )
{
    test_one_join( "Hopspop!", "", "Hops", "pop", "!" );
    test_one_join( "Hops pop !", " ", "Hops", "pop", "!" );
    test_one_join( "Hops.pop.!", ".", "Hops", "pop", "!" );
    test_one_join( "Hops\npop\n!", "\n", "Hops", "pop", "!" );
    test_one_join( "HopsHopspopHops!", "Hops", "Hops", "pop", "!" );

    test_one_join( "Hops", "\n", "Hops" );
    test_one_join( "Hops", "", "Hops" );

    {
        const str_t arr[ 1 ] = { strdup( "hops" ) };
        const str_t got      = string_join( 0, ( strjoin_strarr_t ) arr, "\n" );
        UNIT_TEST( strcmp( got, "" ) == 0 );
        free( got );

        UNIT_TEST( string_join( 1, ( strjoin_strarr_t ) arr, NULL ) == NULL );
        free( arr[ 0 ] );
    }

    UNIT_TEST( string_join( 0, NULL, "\n" ) == NULL );
}
END_TEST


typedef void( StringToUL )( str_t );
Private bool test_one_string_to_UL( StringToUL func, string_t old, string_t new )
{
    const str_t s = strdup( old );
    func( s );
    const bool rv = strcmp( s, new ) == 0;
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
Private bool test_one_string_as_UL( StringAsUL func, string_t old, string_t res )
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


Private bool test_one_reverse_str( string_t orig, string_t result )
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

TEST( reverse_str )
{
    UNIT_TEST( test_one_reverse_str( "Hovno", "onvoH" ) );
    UNIT_TEST( test_one_reverse_str( "", "" ) );
    UNIT_TEST( test_one_reverse_str( "A", "A" ) );

    UNIT_TEST( test_one_reverse_str( "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                                     "ZYXWVUTSRQPONMLKJIHGFEDCBA" ) );

    UNIT_TEST( test_one_reverse_str( "Karel\tVana\n", "\nanaV\tleraK" ) );
    UNIT_TEST( test_one_reverse_str( "\n\r\"\0\"", "\"\r\n" ) );
}
END_TEST


Private bool test_one_strip( string_t orig, string_t desired )
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


Private bool test_one_collapse_backspace( const string_t orig, const string_t result )
{
    const str_t s = strdup( orig );
    string_collapse_backspaces( s );
    const bool rv = strcmp( s, result ) == 0;
    free( s );
    return rv;
}

TEST( collapse_backspaces )
{
    UNIT_TEST( test_one_collapse_backspace( "Hello,\b World!", "Hello World!" ) );
    UNIT_TEST( test_one_collapse_backspace( "Hello, World!\b\b\b\b\b\b", "Hello, " ) );

    UNIT_TEST( test_one_collapse_backspace( "\bHello!", "Hello!" ) );
    UNIT_TEST( test_one_collapse_backspace( "\b\b", "" ) );

    UNIT_TEST( test_one_collapse_backspace( "\n\t\b\b", "" ) );
    UNIT_TEST( test_one_collapse_backspace( "A\bH\bO\bJ\b\b", "" ) );

    UNIT_TEST( test_one_collapse_backspace( "Hello!", "Hello!" ) );
    UNIT_TEST( test_one_collapse_backspace( "", "" ) );

    UNIT_TEST( test_one_collapse_backspace( "\bA", "A" ) );
}
END_TEST

LibraryDefined void RUNALL_STRING_UTILS( void )
{
    RUN_TEST( replace );
    RUN_TEST( replaced );
    RUN_TEST( escape );
    RUN_TEST( unescape );
    RUN_TEST( strspl_str );
    RUN_TEST( strspl_regex );
    RUN_TEST( string_to_UL );
    RUN_TEST( string_as_UL );
    RUN_TEST( reverse_str );
    RUN_TEST( strip );
    RUN_TEST( join );
    RUN_TEST( collapse_backspaces );
}

#endif
