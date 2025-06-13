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
        src/string_utils.c
        src/misc.c
        src/swexpr.c

        src/Structs/dynarr.c
        src/Structs/dynstring.c
        src/Structs/sets.c

        src/item_print_functions.c
)

set(CLIB_PUBLIC_HDRS
        src/string_utils.h
        src/misc.h
        src/swexpr.h

        src/Structs/dynarr.h
        src/Structs/dynstring.h
        src/Structs/sets.h

        src/item_print_functions.h
)

add_library(clib_core STATIC ${CLIB_SOURCES})

install(TARGETS clib_core ARCHIVE DESTINATION lib)


# ————————————————————————————————————
#  3) Enable CTest and define each test executable
# ————————————————————————————————————

enable_testing()

add_executable(Tests
        Tests/tests.c
)
target_link_libraries(Tests PRIVATE clib_core)

# Register it with CTest so that “ctest” will pick it up automatically:
add_test(
        NAME all_unit_tests
        COMMAND Tests
)


add_executable(TestSets
        Tests/test_sets.c
)
target_link_libraries(TestSets PRIVATE clib_core)

add_test(
        NAME sets_tests
        COMMAND TestSets
)


# Don't add_test, because this one should fail in some way
add_executable(TestUnitTests
        Tests/test_unit_tests.c
)
target_link_libraries(TestUnitTests PRIVATE clib_core)


# Add more tests here…


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

