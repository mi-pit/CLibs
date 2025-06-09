//
// Created by Michal Pitner on 08.06.2025.
//

#ifndef CLIBS_QUEUE_H
#define CLIBS_QUEUE_H

#include "../Dev/extra_types.h"

/// single node of a queue (linked list)
struct queue_node;

/**
 * First in, first out.
 *
 * \code
 * // Create/destroy
 * init:       O(1)
 * destroy:    O(n)
 * clear:      O(n)
 * // Modify
 * enqueue:    O(1)
 * dequeue:    O(1)
 * get:        O(n) | n = index
 * \endcode
 */
struct fifo_queue;


struct fifo_queue *queue_init( size_t el_size );
void queue_destroy( struct fifo_queue *queue );
void queue_clear( struct fifo_queue *queue ); // todo

int queue_enqueue( struct fifo_queue *queue, const void *data );
int queue_dequeue( struct fifo_queue *queue, void *data_cont );

int queue_get( const struct fifo_queue *queue, size_t index, void *data_cont );
size_t queue_get_size( const struct fifo_queue *queue );

#endif //CLIBS_QUEUE_H
