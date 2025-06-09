//
// Created by Michal Pitner on 08.06.2025.
//

#include "queue.h"

#include "../Dev/errors.h"

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

    free( first );

    return RV_SUCCESS;
}


int queue_get( const struct fifo_queue *queue, size_t index, void *const data_cont )
{
    if ( queue->head == NULL )
        return fwarnx_ret( RV_EXCEPTION, "queue is empty" );

    if ( index == 0 )
    {
        memcpy( data_cont, queue->head->data, queue->el_size );
        return RV_SUCCESS;
    }

    const struct queue_node *node = queue->head;
    while ( ( node = node->next ) != NULL )
    {
        if ( --index == 0 )
        {
            memcpy( data_cont, node->data, queue->el_size );
            return RV_SUCCESS;
        }
    }

    return fwarnx_ret( RV_EXCEPTION, "queue index out of bounds" );
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
