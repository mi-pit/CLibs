/**
 * @file to_string.h
 * @brief Generic `to_string` macro (C11+ only)
 */

// Created by Michal Pitner on 16.11.2025.


#ifndef CLIBS_TO_STRING_H
#define CLIBS_TO_STRING_H

#include "../../core/stdc_versions.h"

#if __STDC_VERSION__ >= STANDARD_C11_VERSION // requires _Generic

#include "../../core/attributes.h"

#include <string.h>

LibraryDefined PrintfLike( 1, 2 ) UseResult char *GetStringFormatted( const char *fmt,
                                                                      ... )
{
    char *string;

    va_list va;
    va_start( va, fmt );
    const int rv = vasprintf( &string, fmt, va );
    va_end( va );

    if ( rv < 0 )
        return fwarn_ret( NULL, "vasprintf" );

    return string;
}


// TODO
#define GetFormatString( EXPRESSION ) \
    _Generic( ( EXPRESSION ),         \
            char *: "%s",             \
            int: "%d",                \
            long: "%ld",              \
            unsigned: "%u",           \
            unsigned long: "%lu",     \
            void *: "%p",             \
            size_t: "%zu" )


#if defined( TO_STRING_ERROR )
#include "../headers/errors.h"

// Exit with a warning message
#define TO_STRING_DEFAULT_BRANCH( EXPRESSION ) \
    default:                                   \
        ferrx_e( char *, EXIT_FAILURE, "Unknown type for expression `" #EXPRESSION "`" )
#elif defined( TO_STRING_WARN )
// No message in string
#define TO_STRING_DEFAULT_BRANCH( EXPRESSION )                           \
    default:                                                             \
        ( fflwarnx( "unknown type for expression \"" #EXPRESSION "\"" ), \
          GetStringFormatted( "0x%llx", ( unsigned long long ) ( EXPRESSION ) ) )
#elif defined( TO_STRING_WARN_IN_STRING )
// No fwarn, warning in string
#define TO_STRING_DEFAULT_BRANCH( EXPRESSION )       \
    default:                                         \
        GetStringFormatted( "(Unknown type) 0x%llx", \
                            ( unsigned long long ) ( EXPRESSION ) )
#else
// No warning, assume pointer was passed
#define TO_STRING_DEFAULT_BRANCH( EXPRESSION ) \
    default:                                   \
        GetStringFormatted( "0x%llx", ( unsigned long long ) ( EXPRESSION ) )
#endif


// We type-cast this so that `GetStringFormatted` may keep its `PrintfLike` attribute.
#define TO_STRING_BRANCH( TYPE, FMTSTR, EXPRESSION ) \
    TYPE:                                            \
    GetStringFormatted( FMTSTR, ( TYPE ) ( ptrdiff_t ) EXPRESSION )


/// Creates a heap-allocated string representation of the object
#define to_string( EXPRESSION )                                         \
    _Generic( ( EXPRESSION ),                                           \
            bool: strdup( ( EXPRESSION ) ? "true" : "false" ),          \
                                                                        \
            TO_STRING_BRANCH( signed char, "%c", EXPRESSION ),          \
            TO_STRING_BRANCH( unsigned char, "%c", EXPRESSION ),        \
                                                                        \
            TO_STRING_BRANCH( short, "%d", EXPRESSION ),                \
            TO_STRING_BRANCH( unsigned short, "%u", EXPRESSION ),       \
                                                                        \
            TO_STRING_BRANCH( int, "%d", EXPRESSION ),                  \
            TO_STRING_BRANCH( unsigned int, "%u", EXPRESSION ),         \
                                                                        \
            TO_STRING_BRANCH( long, "%ld", EXPRESSION ),                \
            TO_STRING_BRANCH( unsigned long, "%lu", EXPRESSION ),       \
                                                                        \
            TO_STRING_BRANCH( long long, "%lli", EXPRESSION ),          \
            TO_STRING_BRANCH( unsigned long long, "%llu", EXPRESSION ), \
                                                                        \
            TO_STRING_BRANCH( float, "%f", EXPRESSION ),                \
            TO_STRING_BRANCH( double, "%lf", EXPRESSION ),              \
            TO_STRING_BRANCH( long double, "%Lf", EXPRESSION ),         \
                                                                        \
            TO_STRING_BRANCH( void *, "%p", EXPRESSION ),               \
            TO_STRING_BRANCH( const void *, "%p", EXPRESSION ),         \
                                                                        \
            TO_STRING_BRANCH( char *, "%s", EXPRESSION ),               \
            TO_STRING_BRANCH( const char *, "%s", EXPRESSION ),         \
                                                                        \
            /* Most likely an unknown pointer */                        \
            TO_STRING_DEFAULT_BRANCH( EXPRESSION ) )

#else
#error `to_string( EXPR )` requires C11 or newer (requires _Generic)
#endif

#endif //CLIBS_TO_STRING_H
