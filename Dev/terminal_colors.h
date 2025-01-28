//
// Created by MacBook on 14.01.2025.
//

// TODO: Support more colors

#ifndef CLIBS_TERMINAL_COLORS_H
#define CLIBS_TERMINAL_COLORS_H

#include "attributes.h" /* PrintfLike(), LibraryDefined */

#include <stdarg.h>
#include <stdio.h>


#define PRINT_COLOR "%s"

/**
 * When supplied with the correct ANSI color code,
 * this macro creates the escape code for the corresponding color
 */
#define COLOR_CREATOR( NUM ) "\033[" NUM "m"


#define COLOR_DEFAULT COLOR_CREATOR( "0" )


#define GROUND_FORE "3"
#define GROUND_BACK "4"


#define COLOR_BLACK   "0"
#define COLOR_RED     "1"
#define COLOR_GREEN   "2"
#define COLOR_YELLOW  "3"
#define COLOR_BLUE    "4"
#define COLOR_MAGENTA "5"
#define COLOR_CYAN    "6"
#define COLOR_WHITE   "7"

#define FOREGROUND_BLACK   COLOR_CREATOR( GROUND_FORE COLOR_BLACK )
#define FOREGROUND_RED     COLOR_CREATOR( GROUND_FORE COLOR_RED )
#define FOREGROUND_GREEN   COLOR_CREATOR( GROUND_FORE COLOR_GREEN )
#define FOREGROUND_YELLOW  COLOR_CREATOR( GROUND_FORE COLOR_YELLOW )
#define FOREGROUND_BLUE    COLOR_CREATOR( GROUND_FORE COLOR_BLUE )
#define FOREGROUND_MAGENTA COLOR_CREATOR( GROUND_FORE COLOR_MAGENTA )
#define FOREGROUND_CYAN    COLOR_CREATOR( GROUND_FORE COLOR_CYAN )
#define FOREGROUND_WHITE   COLOR_CREATOR( GROUND_FORE COLOR_WHITE )

#define BACKGROUND_BLACK   COLOR_CREATOR( GROUND_BACK COLOR_BLACK )
#define BACKGROUND_RED     COLOR_CREATOR( GROUND_BACK COLOR_RED )
#define BACKGROUND_GREEN   COLOR_CREATOR( GROUND_BACK COLOR_GREEN )
#define BACKGROUND_YELLOW  COLOR_CREATOR( GROUND_BACK COLOR_YELLOW )
#define BACKGROUND_BLUE    COLOR_CREATOR( GROUND_BACK COLOR_BLUE )
#define BACKGROUND_MAGENTA COLOR_CREATOR( GROUND_BACK COLOR_MAGENTA )
#define BACKGROUND_CYAN    COLOR_CREATOR( GROUND_BACK COLOR_CYAN )
#define BACKGROUND_WHITE   COLOR_CREATOR( GROUND_BACK COLOR_WHITE )


LibraryDefined inline void SetTerminalColor( FILE *stream, const char *Color )
{
    fprintf( stream, "%s", Color );
}

LibraryDefined PrintfLike( 3, 4 ) inline void PrintInColor( FILE *file,
                                                            const char *Color,
                                                            const char *__restrict format,
                                                            ... )
{
    fprintf( file, PRINT_COLOR, Color );

    va_list va;
    va_start( va, format );
    vfprintf( file, format, va );
    va_end( va );

    fprintf( file, COLOR_DEFAULT );
}

LibraryDefined inline void VPrintInColor( FILE *file,
                                          const char *Color,
                                          const char *__restrict format,
                                          va_list vaList )
{
    fprintf( file, PRINT_COLOR, Color );
    vfprintf( file, format, vaList );
    fprintf( file, COLOR_DEFAULT );
}


#endif //CLIBS_TERMINAL_COLORS_H
