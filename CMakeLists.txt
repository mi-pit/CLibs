cmake_minimum_required(VERSION 3.24)
project(CLibs C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(TestStringStrip
        Tests/test_strsplit.c
        string_utils.c
        dynarr.c
        )

add_executable(TestStringEscape
        Tests/test_string_escape.c
        string_utils.c
        dynarr.c
        )

add_executable(TestUpperLower
        Tests/test_upper_lower.c
        string_utils.c
        dynarr.c
        )
