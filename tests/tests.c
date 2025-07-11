//
// Created by MacBook on 06.01.2025.
//

#include "modules/test_array_sprintf.h"
#include "modules/test_dict.h"
#include "modules/test_dynstr.h"
#include "modules/test_filenames.h"
#include "modules/test_foreach.h"
#include "modules/test_list.h"
#include "modules/test_math.h"
#include "modules/test_misc_c.h"
#include "modules/test_queue.h"
#include "modules/test_sets.h"
#include "modules/test_string_utils.h"
#include "modules/test_swex.h"


int main( void )
{
    SET_UNIT_TEST_VERBOSITY( false );
    RUNALL_STRING_UTILS();

    RUNALL_DYNSTR();
    RUNALL_LIST();
    RUNALL_DICT();
    RUNALL_SETS();
    RUNALL_QUEUE();

    RUNALL_MISC();
    RUNALL_MATH();

    RUNALL_ARRAYPRINTF();
    RUNALL_FOREACH();
    RUNALL_SWEX();

    RUNALL_FILENAMES();

    FINISH_TESTING();
}
