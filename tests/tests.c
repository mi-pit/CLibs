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
#include "modules/test_struct_conversions.h"
#include "modules/test_swex.h"
#include "modules/test_to_string.h"

#include <sys/file.h> /* open */
#include <unistd.h>   /* dup2 */


#define warn_on_error( EXPR, FUNC, ... ) \
    do                                   \
    {                                    \
        if ( EXPR == RV_ERROR )          \
        {                                \
            FUNC( __VA_ARGS__ );         \
        }                                \
    }                                    \
    while ( 0 )


void setup_testing( void )
{
    SET_UNIT_TEST_VERBOSITY( UNIT_TESTS_YAP_FAILED );

    int fd = open( "/dev/null", 0 );
    warn_on_error( fd, fflwarn, "opening `/dev/null`" );
    warn_on_error( dup2( fd, STDERR_FILENO ), fflwarn, "stderr redirection failed" );
    warn_on_error( close( fd ), PrintInColor, stdout, COLOR_WARNING,
                   "closing `/dev/null`\n" );
}


int main( void )
{
    setup_testing();

    RUNALL_STRING_UTILS();

    RUNALL_DYNSTR();
    RUNALL_LIST();
    RUNALL_DICT();
    RUNALL_SETS();
    RUNALL_QUEUE();

    RUNALL_STRUCT_CONVERSIONS();

    RUNALL_MISC();
    RUNALL_MATH();

    RUNALL_ARRAYPRINTF();
    RUNALL_FOREACH();
    RUNALL_SWEX();

    RUNALL_FILENAMES();

    FINISH_TESTING();
}
