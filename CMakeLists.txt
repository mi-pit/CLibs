cmake_minimum_required(VERSION 3.24)
project(CLibs VERSION 2.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# ------------------------------------
#  1) Set compiler options/definitions
# ------------------------------------

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_compile_definitions(
        _POSIX_C_SOURCE=200809L
        LINE_PREF_WIDTH=120
)


# ------------------------------------
#  2) Gather all “core” sources into a static library
# ------------------------------------

set(CLIB_SOURCES
        src/util/string_utils.c
        src/util/item_print_functions.c
        src/util/math.c

        src/structs/dynarr.c
        src/structs/dynstring.c
        src/structs/set.c
        src/structs/dictionary.c
        src/structs/queue.c

        src/structs/conversions.c
)

set(CLIB_PUBLIC_HDRS
        src/util/string_utils.h
        src/util/item_print_functions.h
        src/util/math.h

        src/structs/dynarr.h
        src/structs/dynstring.h
        src/structs/set.h
        src/structs/dictionary.h
        src/structs/queue.h
)

add_library(clib_core STATIC ${CLIB_SOURCES})

install(TARGETS clib_core ARCHIVE DESTINATION lib)


# ------------------------------------
#  Enable CTest and define each test executable
# ------------------------------------

# ================ With ASAN ================
enable_testing()

add_executable(tests_sanitizers
        tests/tests.c
)
target_compile_definitions(tests_sanitizers PRIVATE CLIBS_TESTS_MAIN)
target_link_options(tests_sanitizers PRIVATE -fsanitize=address -fsanitize=undefined)
target_link_libraries(tests_sanitizers PRIVATE clib_core)
add_test(
        NAME all_unit_tests
        COMMAND tests_sanitizers
)


# ================ Without ASAN ================

add_executable(tests
        tests/tests.c
)
target_link_libraries(tests PRIVATE clib_core)

add_executable(c23
        tests/tests.c
)
target_link_libraries(c23 PRIVATE clib_core)
target_compile_options(c23 PRIVATE --std=c23)

add_executable(test_unit_tests
        tests/test_unit_tests.c
)
target_link_libraries(test_unit_tests PRIVATE clib_core)

add_executable(test_hash
        tests/test_hash_func.c
)
target_link_libraries(test_hash PRIVATE clib_core)

add_executable(test_leet
        tests/test_leet.c
)
target_link_libraries(test_leet PRIVATE clib_core)

add_executable(test_prints
        tests/test_printing.c
)
target_link_libraries(test_prints PRIVATE clib_core)
