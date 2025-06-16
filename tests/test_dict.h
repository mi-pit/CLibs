//
// Created by Michal Pitner on 15.06.2025.
//

#include "../src/headers/pointer_utils.h"
#include "../src/headers/unit_tests.h"
#include "../src/structs/dictionary.h"

TEST( dict_init )
{
    Dictionary *dict = dict_init();
    UNIT_TEST( dict != NULL );
    dict_destroy( dict );
}
END_TEST
