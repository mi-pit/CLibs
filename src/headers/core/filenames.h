/**
 * @file filenames.h
 * @brief
 * Utility functions and macros for working with file names.
 *
 * Everything *should* be platform-independent
 * (as long as you work a posix-compliant system)
 */

// Created by MacBook on 21.01.2025.


#ifndef CLIBS_FILENAMES_H
#define CLIBS_FILENAMES_H

#include "attributes.h"

#include <stdbool.h> /* bool */
#include <string.h>  /* strrchr */

#ifndef __FILE_NAME__
#define __FILE_NAME__ \
    ( strrchr( __FILE__, '/' ) ? strrchr( __FILE__, '/' ) + 1 : __FILE__ )
#endif //__FILE_NAME__


#ifndef PATH_MAX
#if defined( __APPLE__ ) // Get PATH_MAX
#include <sys/syslimits.h>
#elif defined( __linux__ )
#include <linux/limits.h>
#else
/// Maximum length of a file name
#define PATH_MAX 4096
#endif
#endif //ndef PATH_MAX

/**
 * Static buffer for saving the program name.
 *
 * If all efforts to set it to the actual name, it defaults to `"current-program"`
 */
LibraryDefined char ProgName[ PATH_MAX + 1 ] = "current-program";

/**
 * This function doesn't allocate any memory on the heap.
 * Its return value is a pointer to a part of the `full_path`.
 * <p>
 * Example:
 * @code
 * get_file_name( "/home/user/Hovno" )  // => "Hovno"
 * get_file_name( "Hovno" )             // => "Hovno"
 * get_file_name( "/a" )                // => "a"
 * get_file_name( "./a" )               // => "a"
 * get_file_name( "Directory/Subdir/" ) // => "Subdir/"
 * @endcode
 * </p>
 *
 * @param full_path Full path to file
 * @return pointer to the last part of ‹full_path›
 */
LibraryDefined const char *get_file_name( const char *const full_path )
{
    const char *last_slash = strrchr( full_path, '/' );
    if ( last_slash == NULL )
        return full_path;

    if ( *( last_slash + 1 ) != '\0' )
        return last_slash + 1;

    /* path ends with '/' -> return the name before if present */
    for ( const char *ptr = last_slash - 1; ptr >= full_path; --ptr )
        if ( *ptr == '/' )
            return ptr + 1;

    return full_path;
}

/**
 * Fetches the name of the program currently being run.
 *
 * @return Pointer to a static buffer
 */
LibraryDefined inline const char *get_prog_name( void )
{
    return ProgName;
}

/**
 * Sets the static `ProgName` buffer to the current program's name if possible.
 *
 * @return `false` if not successful
 */
BeforeMain LibraryDefined bool set_prog_name( void );


#if defined( __APPLE__ ) /* not sure if it actually works on iPhones :D */
#include <err.h>         /* warn */
#include <mach-o/dyld.h> /* _NSGetExecutablePath() */
#include <stdlib.h>      /* malloc */

BeforeMain LibraryDefined bool set_prog_name( void )
{
    char path[ PATH_MAX + 1 ];
    uint32_t size = sizeof path;
    if ( _NSGetExecutablePath( path, &size ) != 0 )
    {
        char *const alloced = malloc( size );
        if ( alloced == NULL || _NSGetExecutablePath( path, &size ) != 0 )
        {
            warn( "not able to initialize prog name" ); // use std warn to avoid circular import
            return false;
        }
        const char *name = get_file_name( path );
        strncpy( ProgName, name, size );
        free( alloced );
        return true;
    }
    const char *name = get_file_name( path );
    strncpy( ProgName, name, PATH_MAX );
    return true;
}

#elif defined( __linux__ )
#include <unistd.h> /* readlink */

BeforeMain LibraryDefined bool set_prog_name( void )
{
    char path[ PATH_MAX ] = { 0 };

    if ( readlink( "/proc/self/exe", path, PATH_MAX ) == -1 )
        return true;

    strncpy( ProgName, get_file_name( path ), PATH_MAX );
    return false;
}

#endif // set_prog_name()


#endif //CLIBS_FILENAMES_H
