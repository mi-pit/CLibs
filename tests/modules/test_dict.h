//
// Created by Michal Pitner on 15.06.2025.
//
#ifndef TEST_DICT_H
#define TEST_DICT_H

#include "../../src/headers/unit_tests.h"
#include "../../src/headers/util/ptrs/alloc.h"
#include "../../src/structs/dictionary.h"

TEST( dict_init )
{
    Dictionary *dict = dict_init();
    UNIT_TEST( dict != NULL );
    dict_destroy( dict );
}
END_TEST

LibraryDefined void RUNALL_DICT( void )
{
    RUN_TEST( dict_init );
}

#endif
