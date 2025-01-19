//
// Created by MacBook on 14.01.2025.
//

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
#define COLOR_CREATOR( NUM ) "\033[" #NUM "m"

#define COLOR_DEFAULT COLOR_CREATOR( 0 )

#define COLOR_BLACK   COLOR_CREATOR( 30 )
#define COLOR_RED     COLOR_CREATOR( 31 )
#define COLOR_GREEN   COLOR_CREATOR( 32 )
#define COLOR_YELLOW  COLOR_CREATOR( 33 )
#define COLOR_BLUE    COLOR_CREATOR( 34 )
#define COLOR_MAGENTA COLOR_CREATOR( 35 )
#define COLOR_CYAN    COLOR_CREATOR( 36 )
#define COLOR_WHITE   COLOR_CREATOR( 37 )


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
