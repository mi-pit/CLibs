//
// Created by Michal Pitner on 08.06.2025.
//

#include "queue.h"

#include "../headers/errors.h"

#include <assert.h>
#include <stdlib.h>


struct queue_node {
    struct queue_node *next;

    void *data;
};


struct fifo_queue {
    struct queue_node *head; // first in line; first out
    struct queue_node *tail; // last

    size_t el_size; // each element must be the same size
};


struct fifo_queue *queue_init( const size_t el_size )
{
    struct fifo_queue *queue = calloc( 1, sizeof( struct fifo_queue ) );
    queue->el_size           = el_size;
    return queue;
}

void queue_destroy( struct fifo_queue *queue )
{
    queue_clear( queue );
    free( queue );
}

void queue_clear( struct fifo_queue *queue )
{
    struct queue_node *node = queue->head;
    while ( node != NULL )
    {
        struct queue_node *next = node->next;
        free( node->data );
        free( node );
        node = next;
    }
    queue->head = NULL;
    queue->tail = NULL;
}


int queue_enqueue( struct fifo_queue *queue, const void *data )
{
    struct queue_node **next_node_cont =
            queue->head == NULL ? &queue->head : &queue->tail->next;

    if ( ( *next_node_cont = calloc( 1, sizeof( struct queue_node ) ) ) == NULL )
        return fwarn_ret( RV_ERROR, "node alloc failed" );

    struct queue_node *node = *next_node_cont;
    assert( node != NULL );
    queue->tail = node;

    node->data = malloc( queue->el_size );
    if ( node->data == NULL )
    {
        if ( next_node_cont == &queue->head )
            free( queue->head );
        return fwarn_ret( RV_ERROR, "node data alloc failed" );
    }

    memcpy( node->data, data, queue->el_size );
    return RV_SUCCESS;
}

int queue_dequeue( struct fifo_queue *queue, void *data_cont )
{
    if ( queue->head == NULL )
        return fwarnx_ret( RV_EXCEPTION, "queue is empty" );

    struct queue_node *first = queue->head;
    if ( data_cont != NULL )
        memcpy( data_cont, first->data, queue->el_size );
    queue->head = first->next;
    if ( queue->head == NULL )
        queue->tail = NULL;

    free( first->data );
    free( first );

    return RV_SUCCESS;
}


int queue_get( const struct fifo_queue *queue, const size_t index, void *const data_cont )
{
    if ( queue->head == NULL )
        return fwarnx_ret( RV_EXCEPTION, "queue is empty" );

    if ( index == 0 )
    {
        memcpy( data_cont, queue->head->data, queue->el_size );
        return RV_SUCCESS;
    }

    size_t actual_size_idx      = 0;
    size_t distance_from_target = index;

    const struct queue_node *node = queue->head;
    while ( ( node = node->next ) != NULL )
    {
        ++actual_size_idx;
        if ( --distance_from_target == 0 )
        {
            memcpy( data_cont, node->data, queue->el_size );
            return RV_SUCCESS;
        }
        assert( distance_from_target > 0 );
    }

    return fwarnx_ret( RV_EXCEPTION,
                       "queue index %zu out of bounds for queue of length %zu",
                       index,
                       actual_size_idx );
}

int queue_get_head( const struct fifo_queue *queue, void *data_cont )
{
    if ( queue->head == NULL )
    {
        assert( queue->tail == NULL );
        return fwarnx_ret( RV_EXCEPTION, "queue is empty" );
    }

    memcpy( data_cont, queue->head->data, queue->el_size );
    return RV_SUCCESS;
}

int queue_get_tail( const struct fifo_queue *queue, void *data_cont )
{
    if ( queue->tail == NULL )
    {
        assert( queue->head == NULL );
        return fwarnx_ret( RV_EXCEPTION, "queue is empty" );
    }

    memcpy( data_cont, queue->tail->data, queue->el_size );
    return RV_SUCCESS;
}

size_t queue_get_size( const struct fifo_queue *queue )
{
    if ( queue->head == NULL )
        return 0;

    size_t i                      = 1;
    const struct queue_node *node = queue->head;
    while ( ( node = node->next ) != NULL )
        i++;

    return i;
}

bool queue_is_empty( const struct fifo_queue *queue )
{
    const bool no_head = queue->head == NULL;
    if ( no_head )
        assert( queue->tail == NULL );

    return no_head;
}


QueueEnumeratedEntry queue_get_next( const struct fifo_queue *queue,
                                     const struct queue_node *prev,
                                     const bool get_first )
{
    if ( get_first )
        return ( QueueEnumeratedEntry ) { .item = queue->head,
                                          .data = queue->head->data };

    if ( queue->head == NULL || prev == NULL )
        return ( QueueEnumeratedEntry ) { 0 };

    const struct queue_node *node = prev->next;

    return ( QueueEnumeratedEntry ) { .item = node, .data = node->data };
}
