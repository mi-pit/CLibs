cmake_minimum_required(VERSION 3.24)
project(CLibs VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# ————————————————————————————————————
#  1) Set sanitizer flags and “build type” as options
# ————————————————————————————————————

# Option to enable Address+Undefined sanitizer (ON by default for Debug builds).
option(USE_SANITIZERS "Compile & link with -fsanitize=address,undefined" OFF)
if (USE_SANITIZERS)
    message(STATUS "→ Building with Address+Undefined sanitizers enabled")
    add_compile_options(-fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)
endif ()

# Allow the user to pass -DCMAKE_BUILD_TYPE=Debug/Release from the command line.
# By default, set it to “Release” if nothing was specified:
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif ()

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_compile_definitions(_POSIX_C_SOURCE=200809L)


# ————————————————————————————————————
#  2) Gather all “core” sources into a static library
# ————————————————————————————————————

set(CLIB_SOURCES
        string_utils.c
        misc.c
        swexpr.c

        Structs/dynarr.c
        Structs/dynstring.c
        Structs/sets.c

        item_print_functions.c
)

set(CLIB_PUBLIC_HDRS
        string_utils.h
        misc.h
        swexpr.h

        Structs/dynarr.h
        Structs/dynstring.h
        Structs/sets.h

        item_print_functions.h
)

add_library(clib_core STATIC ${CLIB_SOURCES})

target_include_directories(clib_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Structs
        ${CMAKE_CURRENT_SOURCE_DIR}/Dev      # if Test code (or library itself) needs those “Dev/*.h”
)

install(TARGETS clib_core ARCHIVE DESTINATION lib)


# ————————————————————————————————————
#  3) Enable CTest and define each test executable
# ————————————————————————————————————

enable_testing()

#
#  3a) “Tests” executable (tests.c + everything it needs)
#
add_executable(Tests
        Tests/tests.c
)
target_link_libraries(Tests PRIVATE clib_core)
target_include_directories(Tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Dev
        ${CMAKE_CURRENT_SOURCE_DIR}/Tests
)

# Register it with CTest so that “ctest” will pick it up automatically:
# (If your Tests/tests.c dispatcher accepts command‐line flags, you can add multiple add_test() lines here.)
add_test(
        NAME all_unit_tests
        COMMAND Tests
)


#
#  3b) “TestSets” executable (only test_sets.c + the minimal pieces)
#
add_executable(TestSets
        Tests/test_sets.c
)
target_link_libraries(TestSets PRIVATE clib_core)
target_include_directories(TestSets PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Dev
        ${CMAKE_CURRENT_SOURCE_DIR}/Structs
        ${CMAKE_CURRENT_SOURCE_DIR}/Tests
)

add_test(
        NAME sets_tests
        COMMAND TestSets
)


#
#  3c) Add more tests here…
#  e.g. add a “TestDynStr” if you want to split them up:
#    add_executable(TestDynStr   Tests/test_dynstr.c)
#    target_link_libraries(TestDynStr PRIVATE clib_core)
#    target_include_directories(TestDynStr PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}
#        ${CMAKE_CURRENT_SOURCE_DIR}/Dev
#        ${CMAKE_CURRENT_SOURCE_DIR}/Structs
#        ${CMAKE_CURRENT_SOURCE_DIR}/Tests
#    )
#    add_test(NAME dynstr_tests COMMAND TestDynStr)
#


# ————————————————————————————————————
#  4) Installation rules for headers & library
# ————————————————————————————————————

install(TARGETS clib_core
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
)
install(FILES ${CLIB_PUBLIC_HDRS}
        DESTINATION include
)

