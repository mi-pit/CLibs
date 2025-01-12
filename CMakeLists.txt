cmake_minimum_required(VERSION 3.24)
project(CLibs C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(Tests
        Tests/tests.c
        string_utils.c
        Structs/dynarr.c
        Structs/dynstring.c
        swexpr.c
        misc.c
        )

add_executable(TestNumbers
        Tests/bigint_test.c
        Structs/bigint.c
        Structs/dynstring.c
        Structs/dynarr.c
        string_utils.c
        )

add_executable(TestHashFunc
        Tests/test_hash_func.c
        misc.c
        Structs/sets.c
        item_print_functions.c
        )

add_executable(TestLeet
        Tests/test_leet.c
        Structs/dictionary.c
        Structs/dynarr.c
        item_print_functions.c
        misc.c
        )

add_executable(TestTests
        Tests/test_unit_tests.c
        misc.c
        )
