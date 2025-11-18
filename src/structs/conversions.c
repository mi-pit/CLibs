//
// Created by Michal Pitner on 14.11.2025.
//

#include "conversions.h"

#define FOREACH_SET
#include "../headers/util/ctrlflow.h"


List *list_from_set( const Set *set )
{
    size_t elsize;
    foreach_set ( e, set )
    {
        // assign first value
        elsize = e.item->size;
        break;
    }

    foreach_set ( entry, set )
    {
        const size_t curr_elsize = entry.item->size;
        if ( curr_elsize != elsize )
            return fwarnx_ret( NULL,
                               "sets contents aren't all the same size (entry %" PRIi64
                               "=%zu)",
                               entry.index,
                               curr_elsize );
    }

    List *ls = list_init_cap_size( elsize, set_size( set ) );
    if ( ls == NULL )
        return f_stack_trace( NULL );

    foreach_set ( entry, set )
    {
        assert_that( !entry.item->removed,
                     "foreach_set should not be yielding removed entries" );

        const void *datap = entry.item->data;
        assert_that( datap != NULL, "foreach_set should not be yielding NULL entries" );

        if ( list_append( ls, datap ) != RV_SUCCESS )
        {
            list_destroy( ls );
            return f_stack_trace( NULL );
        }
    }

    return ls;
}


List *list_from_queue( const Queue *q )
{
    List *ls = list_init_cap_size( queue_get_el_size( q ), queue_get_size( q ) );
    if ( ls == NULL )
        return f_stack_trace( NULL );

    foreach_que( entry, q )
    {
        const void *datap = queue_node_get_data( entry );
        on_fail ( list_append( ls, datap ) )
            return f_stack_trace( NULL );
    }

    return ls;
}
