//
// Created by MacBook on 01.11.2024.
//

#ifndef CLIBS_POINTER_UTILS_H
#define CLIBS_POINTER_UTILS_H

#define free_n( ptr ) free_and_null( ( void ** ) &( ptr ) )

void free_all( int n, ... );
void free_and_null( void ** );


#define deref_as( type, var )                ( *( ( type * ) ( var ) ) )
#define deref_as_offset( type, var, offset ) ( *( ( ( type * ) ( var ) ) + offset ) )


#endif //CLIBS_POINTER_UTILS_H
