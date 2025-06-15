//
// Created by Michal Pitner on 08.06.2025.
//

#ifndef CLIBS_QUEUE_H
#define CLIBS_QUEUE_H

#include "../headers/extra_types.h"

/**
 * First in, first out.
 *
 * \code
 * // Create/destroy
 * init        O(1)
 * destroy     O(n)
 * clear       O(n)
 * // Modify
 * enqueue     O(1)
 * dequeue     O(1)
 * // getters
 * get         O(n) | n = index
 * get_head    O(1)
 * get_tail    O(1)
 * get_size    O(n)
 * is_empty    O(1)
 * \endcode
 */
struct fifo_queue;


struct fifo_queue *queue_init( size_t el_size );
void queue_destroy( struct fifo_queue *queue );
void queue_clear( struct fifo_queue *queue );

int queue_enqueue( struct fifo_queue *queue, const void *data );
int queue_dequeue( struct fifo_queue *queue, void *data_cont );

int queue_get( const struct fifo_queue *queue, size_t index, void *data_cont );
int queue_get_head( const struct fifo_queue *queue, void *data_cont );
int queue_get_tail( const struct fifo_queue *queue, void *data_cont );

size_t queue_get_size( const struct fifo_queue *queue );
bool queue_is_empty( const struct fifo_queue *queue );

typedef struct {
    const struct queue_node *item;
    void *const data;
} QueueEnumeratedEntry;

/**
 * Iterator over queue
 *
 * @return QueueEnumeratedEntry
 */
QueueEnumeratedEntry queue_get_next( const struct fifo_queue *,
                                     const struct queue_node *prev,
                                     bool get_first );


#endif //CLIBS_QUEUE_H
