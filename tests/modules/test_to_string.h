#ifndef CLIBS_TEST_TO_STRING_H
#define CLIBS_TEST_TO_STRING_H

#include "../../src/headers/core/stdc_versions.h" /* STANDARD_C11_VERSION */
#include "../../src/headers/unit_tests.h"         /* TEST, TEST_NAME_CREATOR(UNIT_TEST) */

#if __STDC_VERSION__ < STANDARD_C11_VERSION
void RUNALL_TO_STRING( void )
{
    ( void ) 0;
}
#else

#include "../../src/headers/util/print/println.h" /* ... */
#include "../../src/headers/util/str/to_string.h" /* ... */


#define TEST_ONE_TO_STRING( TYPE, EXPRESSION, DESIRED_STRING )                      \
    do                                                                              \
    {                                                                               \
        char *string_ = to_string( ( TYPE ) ( EXPRESSION ) );                       \
        if ( !TEST_NAME_CREATOR( UNIT_TEST )(                                       \
                     "`(" #TYPE ") " #EXPRESSION "` should yield " #DESIRED_STRING, \
                     strcmp( string_, ( DESIRED_STRING ) ) == 0, __FILE_NAME__,     \
                     __LINE__, false ) )                                            \
        {                                                                           \
            ++TEST_NAME_CREATOR( failed_total );                                    \
            ( void ) println( "\t\t\\ got \"%s\"", string_ );                       \
        }                                                                           \
        ++TEST_NAME_CREATOR( ran_total );                                           \
        free( string_ );                                                            \
    }                                                                               \
    while ( 0 )


TEST( to_string )
{
    TEST_ONE_TO_STRING( bool, 1, "true" );
    TEST_ONE_TO_STRING( bool, true, "true" );
    TEST_ONE_TO_STRING( bool, false, "false" );

    TEST_ONE_TO_STRING( short, -123, "-123" );
    TEST_ONE_TO_STRING( unsigned short, 123, "123" );

    TEST_ONE_TO_STRING( signed char, 'A', "A" );
    TEST_ONE_TO_STRING( unsigned char, 'A', "A" );

    TEST_ONE_TO_STRING( int, -123, "-123" );
    TEST_ONE_TO_STRING( unsigned int, 123, "123" );

    TEST_ONE_TO_STRING( long, -123, "-123" );
    TEST_ONE_TO_STRING( unsigned long, 123, "123" );

    TEST_ONE_TO_STRING( long long, -123, "-123" );
    TEST_ONE_TO_STRING( unsigned long long, 123, "123" );

    TEST_ONE_TO_STRING( char *, "Hello!", "Hello!" );
    TEST_ONE_TO_STRING( const char *, "Hello!", "Hello!" );

#ifdef __APPLE__
    TEST_ONE_TO_STRING( char *, NULL, "(null)" );
    TEST_ONE_TO_STRING( const char *, NULL, "(null)" );
#endif

    TEST_ONE_TO_STRING( void *, NULL, "0x0" );
    TEST_ONE_TO_STRING( const void *, NULL, "0x0" );

    TEST_ONE_TO_STRING( void *, 0x123123123, "0x123123123" );
    TEST_ONE_TO_STRING( const void *, 0x123123123, "0x123123123" );

#ifndef TO_STRING_WARN_IN_STRING
    TEST_ONE_TO_STRING( int *, 0x12, "0x12" );
#else
    TEST_ONE_TO_STRING( int *, 0x12, "(Unknown type) 0x12" );
#endif

    TEST_ONE_TO_STRING( size_t, SIZE_MAX, "18446744073709551615" );
    TEST_ONE_TO_STRING( ptrdiff_t, PTRDIFF_MIN, "-9223372036854775808" );
    TEST_ONE_TO_STRING( uint64_t, 15895108, "15895108" );
    TEST_ONE_TO_STRING( int16_t, ( int8_t ) 0, "0" );

    char *s = GetStringFormatted( "%zu", sizeof( int ) );
    // check PrintfLike
    free( s );
}
END_TEST


void RUNALL_TO_STRING( void )
{
    RUN_TEST( to_string );
}

#endif

#endif //CLIBS_TEST_TO_STRING_H
