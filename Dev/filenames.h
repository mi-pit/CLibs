//
// Created by MacBook on 21.01.2025.
//

#ifndef CLIBS_GET_PROG_NAME_H
#define CLIBS_GET_PROG_NAME_H

#include "attributes.h"

#include <string.h>

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
#define PATH_MAX 1024
#endif // Get PATH_MAX
#endif //ndef PATH_MAX

LibraryDefined char ProgName[ PATH_MAX + 1 ] = "current-program";

/**
 * This function doesn't allocate any memory on the heap.
 * Its return value is a pointer to a part of the full_path.
 * @param full_path Full path to file
 * @return pointer to the last part of ‹full_path›
 * @example
 * @code
 * get_file_name( "/home/user/Hovno" )  // => "Hovno"
 * get_file_name( "Hovno" )             // => "Hovno"
 * get_file_name( "/a" )                // => "a"
 * get_file_name( "./a" )               // => "a"
 * get_file_name( "Directory/Subdir/" ) // => "Subdir/"
 * @endcode
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

LibraryDefined const char *get_prog_name( void )
{
    return ProgName;
}

#if defined( __APPLE__ ) /* not sure if it actually works on iPhones :D */
#include <mach-o/dyld.h> /* _NSGetExecutablePath() */

BeforeMain LibraryDefined bool set_prog_name( void )
{
    char path[ PATH_MAX ];
    uint32_t size = PATH_MAX;
    if ( _NSGetExecutablePath( path, &size ) == 0 )
    {
        const char *name = get_file_name( path );
        strncpy( ProgName, name, PATH_MAX );
        return true;
    }

    return false;
}
#elif ( defined( __APPLE__ ) || defined( __FreeBSD__ ) ) && !defined( _POSIX_C_SOURCE )
#include <stdlib.h>
#define get_prog_name() getprogname()
#elif defined( __linux__ )
#include <unistd.h> /* readlink */

BeforeMain LibraryDefined bool set_prog_name( void )
{
    char path[ PATH_MAX ] = { 0 };

    if ( readlink( "/proc/self/exe", path, PATH_MAX ) == RV_ERROR )
    {
        warn( "%s: readlink", __func__ );
        return 1;
    }

    strncpy( prog_name, get_file_name( path ), PATH_MAX );
    return 0;
}

#elif defined( __FILE_NAME__ ) /* this kinda sucks, but hey, what are you gonna do? */
#define get_prog_name() __FILE_NAME__
#endif // get_prog_name()


#endif //CLIBS_GET_PROG_NAME_H
