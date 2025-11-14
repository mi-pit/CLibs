// Created by Michal Pitner on 14.11.2025.

#ifndef CLIBS_DEREF_H
#define CLIBS_DEREF_H

/// Dereferences a pointer as if it was a pointer to `ARRTYPE`
#define deref_as( ARRTYPE, ARRAY_VARIABLE ) ( *( ARRTYPE * ) ( ARRAY_VARIABLE ) )
/// Dereferences a pointer as if it was a pointer to `ARRTYPE` with an offset
#define deref_as_offset( ARRTYPE, ARRAY_VARIABLE, OFFSET ) \
    ( *( ( ARRTYPE * ) ( ARRAY_VARIABLE ) ) + OFFSET )

#endif //CLIBS_DEREF_H
