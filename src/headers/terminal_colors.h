/*
 * Functions and macros for working with colors on UNIX terminals.
 *
 *
 * Created by MacBook on 14.01.2025.
 */

#ifndef CLIBS_TERMINAL_COLORS_H
#define CLIBS_TERMINAL_COLORS_H

#include "attributes.h" /* PrintfLike(), LibraryDefined */

#include <stdarg.h>
#include <stdio.h>


/** Format string for printing colors */
#define PRINT_COLOR "%s"

/**
 * When supplied with the correct ANSI color code,
 * this macro creates the escape code for the corresponding color.
 *
 * For example:
 * @code
 * #define COLOR_DEFAULT    COLOR_CREATOR( "0" )
 * #define FOREGROUND_BLACK COLOR_CREATOR( GROUND_FORE COLORCODE_BLACK )
 * @endcode
 */
#define COLOR_CREATOR( NUM ) "\033[" NUM "m"


#define COLOR_DEFAULT COLOR_CREATOR( "0" )


#define GROUND_FORE "3"
#define GROUND_BACK "4"


#define COLORCODE_BLACK   "0"
#define COLORCODE_RED     "1"
#define COLORCODE_GREEN   "2"
#define COLORCODE_YELLOW  "3"
#define COLORCODE_BLUE    "4"
#define COLORCODE_MAGENTA "5"
#define COLORCODE_CYAN    "6"
#define COLORCODE_WHITE   "7"

#define FOREGROUND_BLACK   COLOR_CREATOR( GROUND_FORE COLORCODE_BLACK )
#define FOREGROUND_RED     COLOR_CREATOR( GROUND_FORE COLORCODE_RED )
#define FOREGROUND_GREEN   COLOR_CREATOR( GROUND_FORE COLORCODE_GREEN )
#define FOREGROUND_YELLOW  COLOR_CREATOR( GROUND_FORE COLORCODE_YELLOW )
#define FOREGROUND_BLUE    COLOR_CREATOR( GROUND_FORE COLORCODE_BLUE )
#define FOREGROUND_MAGENTA COLOR_CREATOR( GROUND_FORE COLORCODE_MAGENTA )
#define FOREGROUND_CYAN    COLOR_CREATOR( GROUND_FORE COLORCODE_CYAN )
#define FOREGROUND_WHITE   COLOR_CREATOR( GROUND_FORE COLORCODE_WHITE )

#define BACKGROUND_BLACK   COLOR_CREATOR( GROUND_BACK COLORCODE_BLACK )
#define BACKGROUND_RED     COLOR_CREATOR( GROUND_BACK COLORCODE_RED )
#define BACKGROUND_GREEN   COLOR_CREATOR( GROUND_BACK COLORCODE_GREEN )
#define BACKGROUND_YELLOW  COLOR_CREATOR( GROUND_BACK COLORCODE_YELLOW )
#define BACKGROUND_BLUE    COLOR_CREATOR( GROUND_BACK COLORCODE_BLUE )
#define BACKGROUND_MAGENTA COLOR_CREATOR( GROUND_BACK COLORCODE_MAGENTA )
#define BACKGROUND_CYAN    COLOR_CREATOR( GROUND_BACK COLORCODE_CYAN )
#define BACKGROUND_WHITE   COLOR_CREATOR( GROUND_BACK COLORCODE_WHITE )


#define FOREGROUND_GRAY COLOR_CREATOR( GROUND_FORE "8;5;238" )
#define BACKGROUND_GRAY COLOR_CREATOR( GROUND_BACK "8;5;238" )


/**
 * Sets the terminal color to `Color`.
 *
 * The string must be a valid color string (something like `FOREGROUND_WHITE`),
 * otherwise this function is just a worse `fprintf`.
 * (it only exists as an abstraction for `fprintf`)
 */
LibraryDefined inline bool SetTerminalColor( FILE *stream, const char *Color )
{
    return fprintf( stream, "%s", Color ) > 0;
}

/// @see `PrintInColor`
LibraryDefined inline int VPrintInColor( FILE *file,
                                         const char *Color,
                                         const char *format,
                                         va_list vaList )
{
    if ( !SetTerminalColor( file, Color ) )
        return -1;

    const int rv = vfprintf( file, format, vaList );

    if ( !SetTerminalColor( file, COLOR_DEFAULT ) )
        return -1;

    return rv;
}

/**
 * Prints a formatted message (like `fprintf`) in the supplied color.
 *
 * Since there exists no reliable way to retrieve the current terminal color,
 * this function resets it to the default.
 */
LibraryDefined PrintfLike( 3, 4 ) inline int PrintInColor( FILE *file,
                                                           const char *Color,
                                                           const char *format,
                                                           ... )
{
    va_list va;
    va_start( va, format );
    const int rv = VPrintInColor( file, Color, format, va );
    va_end( va );
    return rv;
}


#endif //CLIBS_TERMINAL_COLORS_H
