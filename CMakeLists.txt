cmake_minimum_required(VERSION 3.24)
project(CLibs C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(TestStringStrip
        Tests/test_strsplit.c
        string_utils.c
        dynarr.c
        errors.c
        )

add_executable(TestStringEscape
        Tests/test_string_escape.c
        string_utils.c
        dynarr.c
        errors.c
        )

add_executable(TestUpperLower
        Tests/test_upper_lower.c
        string_utils.c
        dynarr.c
        errors.c
        )

add_executable(TestHashFunc
        Tests/test_hash_func.c
        misc.c
        sets.c
        errors.c
        item_print_functions.c
        )

add_executable(TestLeet
        Tests/test_leet.c
        dictionary.c
        dynarr.c
        item_print_functions.c
        errors.c
        pointer_utils.c
        misc.c
        )
