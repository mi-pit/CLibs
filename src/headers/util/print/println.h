//
// Created by Michal Pitner on 14.10.2025.
//

#ifndef PRINT_H
#define PRINT_H


#ifndef LINEBREAK_STR
#define LINEBREAK_STR "\n"
#endif

#ifndef LINEBREAK_CHR
#define LINEBREAK_CHR '\n'
#endif


#include <stdio.h>


#define println( ... ) ( printf( __VA_ARGS__ ) + printf( "%c", LINEBREAK_CHR ) )


#endif //PRINT_H
