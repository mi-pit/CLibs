//
// Created by MacBook on 30.12.2024.
//

#include "../src/headers/assert_that.h" /* assert_that(), include errors.h */
#include "../src/headers/misc.h"        /* hash_func() */
#include "../src/structs/set.h"         /* Set */

#include <inttypes.h>
#include <stdlib.h>


#define try_one( TYPE, VALUE )                                         \
    do                                                                 \
    {                                                                  \
        TYPE Variable = VALUE;                                         \
        uint64_t Hash = hash_func( &Variable, sizeof( TYPE ) );        \
        printf( "`%s`\t:= %s -> %" PRIu64 "\n", #TYPE, #VALUE, Hash ); \
    }                                                                  \
    while ( 0 )

int main( void )
{
    try_one( int64_t, 0 );
    try_one( int32_t, 0 );
    try_one( int16_t, 0 );
    try_one( int8_t, 0 );

    try_one( size_t, SIZE_MAX );

    try_one( char *, NULL );

    char *p;
    try_one( void *, p = malloc( 1 ) );
    printf( "%td\n", ( ptrdiff_t ) p );

    char buffer[ 128 ] = { 0 };
    uint64_t Hash      = hash_func( buffer, sizeof buffer );
    printf( "`%s`\t:= %s -> %" PRIu64 "\n", "char[128]", "{0}", Hash );

    for ( int i = 0; i < 128; ++i )
        buffer[ i ] = ( char ) ( rand() % 0xFF ); // NOLINT(cert-msc50-cpp)
    Hash = hash_func( buffer, 128 );
    printf( "`%s`\t:= %s -> %" PRIu64 "\n", "char[128]", "rand() % 0xFF", Hash );

    Set *set = set_init();
    if ( set == NULL )
        err( -1, "set_init" );

    return_on_error( set_insert( set, &buffer, sizeof( char * ) ) );
    return_on_error( set_insert( set, p, sizeof( char ) ) );
    return_on_error( set_insert( set, &p, sizeof( char * ) ) );

    assert_that( set_search( set, &buffer, sizeof( char * ) ), "&buffer" );
    assert_that( set_search( set, p, sizeof( char ) ), "p" );
    assert_that( set_search( set, &p, sizeof( char * ) ), "&p" );

    set_destroy( set );
}
