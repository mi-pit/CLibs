cmake_minimum_required(VERSION 3.24)
project(CLibs C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(TestStringUtils
        Tests/test_string_utils.c
        string_utils.c
        dynarr.c
        errors.c
        )
