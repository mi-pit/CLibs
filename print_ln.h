//
// Created by MacBook on 25.12.2024.
//

#ifndef CLIBS_PRINT_LN_H
#define CLIBS_PRINT_LN_H

#include <stdarg.h>
#include <stdio.h>

int PrintLNUniversal( FILE *__restrict, const char *__restrict format, ... )
        __printflike( 2, 3 );

#define printf_ln( ... )                 PrintLNUniversal( stdout, __VA_ARGS__ )
#define fprintf_ln( FILE_VAR_NAME, ... ) PrintLNUniversal( FILE_VAR_NAME, __VA_ARGS__ )

#endif //CLIBS_PRINT_LN_H
