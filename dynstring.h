//
// Created by MacBook on 18.10.2024.
//

#ifndef CLIBS_DYNSTRING_H
#define CLIBS_DYNSTRING_H

#include <stddef.h>

typedef struct dynamic_string *DynamicString;
typedef const struct dynamic_string *ConstDynamicString;


#define DEFAULT_DYNSTRING_CAP 256


DynamicString dynstr_init( void );
DynamicString dynstr_init_cap( size_t cap );
DynamicString dynstr_init_as( const char * );
void dynstr_destroy( DynamicString );

int dynstr_append( DynamicString, char );
int dynstr_extend( DynamicString, const char * );

char *dynstr_to_str( DynamicString );
const char *dynstr_data( DynamicString );
size_t dynstr_len( DynamicString );

#endif //CLIBS_DYNSTRING_H
