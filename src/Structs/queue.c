//
// Created by Michal Pitner on 08.06.2025.
//

#include "queue.h"


struct fifo_queue {
    struct queue_node *head;
    struct queue_node *tail;
};
