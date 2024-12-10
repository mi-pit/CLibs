cmake_minimum_required(VERSION 3.24)
project(CLibs C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(CLibs
        dictionary.c
        dictionary.h
        dynarr.c
        dynarr.h
        dynstring.c
        dynstring.h
        errors.c
        errors.h
        extra_types.h
        foreach.h
        Tests/main.c
        misc.c
        misc.h
        pointer_utils.c
        pointer_utils.h
        print_functions.c
        print_functions.h
        sets.c
        sets.h
        string_utils.c
        string_utils.h
        swexpr.h swexpr.c
        )

add_executable(TestSwex
        dynarr.c
        errors.c
        Tests/main.c
        pointer_utils.c
        swexpr.c
        )
