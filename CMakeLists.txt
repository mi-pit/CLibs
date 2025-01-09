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
        pointer_utils.c
        misc.c
        )

add_executable(TestTests
        Tests/test_unit_tests.c
        misc.c
        )
