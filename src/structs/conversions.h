/**
 * @file conversions.h
 * @brief Conversions between data structs from this lib.
 */

// Created by Michal Pitner on 14.11.2025.


#ifndef CLIBS_CONVERSIONS_H
#define CLIBS_CONVERSIONS_H

#include "dynarr.h"
#include "queue.h"
#include "set.h"

Constructor List *list_from_set( const Set * );

Constructor List *list_from_queue( const Queue * );

#endif //CLIBS_CONVERSIONS_H
