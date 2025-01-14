//
// Created by MacBook on 03.01.2025.
//
#include "../assert_that.h"
#include "../pointer_utils.h"
#include "../Structs/dictionary.h"
#include "../Structs/dynarr.h"

// after dynarr
#include "../foreach.h"


typedef int64_t Number;
typedef size_t count_t;

#define TypeNumber 2

#if TypeNumber == 1
#define Type Number
#include <inttypes.h>
define_print_func( Number, "%" PRIi64 );

#elif TypeNumber == 2
typedef void *pointer_t;
define_print_func( pointer_t, "%p" );
#define Type pointer_t

#endif //TypeNumber


Type majority_element( struct dynamic_array *nums )
{
    Dict freq = dict_init();

    foreach_ls( Type, num, nums )
    {
        const struct key_value_pair *kvp = dict_get( freq, &num, sizeof num );
        count_t count = ( kvp == NULL ? 0 : deref_as( count_t, kvp->val ) ) + 1;
        dict_insert( freq, &num, sizeof num, &count, sizeof count );
    }

    dict_printn_as( freq, print_function( Type ), print_size_t );

    foreach_ls( Type, num, nums )
    {
        const struct key_value_pair *kvp = dict_get( freq, &num, sizeof num );
        Type key                         = deref_as( Type, kvp->key );
        count_t value                    = deref_as( count_t, kvp->val );

        if ( value >= list_size( nums ) / 2 )
        {
            list_destroy( nums );
            dict_destroy( freq );
            return key;
        }
    }

    list_destroy( nums );
    dict_destroy( freq );

    exit( fflwarnx_ret( 1, "couldn't find majority element" ) );
}


#define test_one_majelem( TYPE, DESIRED, LS_LEN, ... )         \
    do                                                         \
    {                                                          \
        struct dynamic_array *values = list_init_type( TYPE ); \
        TYPE values_array[ LS_LEN ]  = { __VA_ARGS__ };        \
        list_extend( values, values_array, LS_LEN );           \
        TYPE found = majority_element( values );               \
        assert_that( found == DESIRED,                         \
                     "found %lli expected %lli",               \
                     ( long long ) found,                      \
                     ( long long ) DESIRED );                  \
    }                                                          \
    while ( 0 )


int main( void )
{
#if TypeNumber == 1
    test_one_majelem( Type, 1, 3, 1, 2, 1 );

    test_one_majelem( Type, 1, 6, 3, 2, 2, 1, 1, 1 );

    test_one_majelem( Type,
                      /* should get */ 2,
                      /* len */ 10,
                      2,
                      4,
                      2,
                      4,
                      2,
                      2,
                      2,
                      1,
                      5,
                      7 );

    test_one_majelem( Type,
                      /* should get */ 2,
                      /* len */ 16,
                      100,
                      2,
                      2,
                      2,
                      500,
                      10000,
                      2,
                      2,
                      2,
                      2,
                      1000000,
                      60,
                      600,
                      2,
                      2,
                      1001000 );
#elif TypeNumber == 2
    test_one_majelem( void *,
                      NULL,
                      4,
                      NULL,
                      list_init_type( int ),
                      list_init_type( size_t ),
                      list_init_size( -1 ) );
#endif

    return 0;
}
