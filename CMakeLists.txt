cmake_minimum_required(VERSION 3.24)
project(CLibs C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(TestSwex
        dynarr.c
        errors.c
        Tests/main.c
        pointer_utils.c
        swexpr.c
        )

add_executable(TestStringUtils
        Tests/main.c
        Tests/test_string_utils.c
        string_utils.c
        dynarr.c
        errors.c
        )
