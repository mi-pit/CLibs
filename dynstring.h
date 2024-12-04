//
// Created by MacBook on 18.10.2024.
//

#ifndef CLIBS_DYNSTRING_H
#define CLIBS_DYNSTRING_H

#include <stddef.h>

typedef struct dynamic_string *DynamicString;
typedef const struct dynamic_string *ConstDynamicString;


#define DEFAULT_DYNSTRING_CAP 256


DynamicString string_init( void );
DynamicString string_init_cap( size_t capacity );
DynamicString string_init_as( const char *s );
void string_destroy( DynamicString );
void string_destroy_n( DynamicString *dstr_cont );

int string_append( DynamicString, char );
int string_extend( DynamicString, const char * );

char *dynstr_to_str( DynamicString );
const char *dynstr_data( DynamicString dstr );
size_t dynstr_len( DynamicString );

#endif //CLIBS_DYNSTRING_H
