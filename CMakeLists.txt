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
        src/item_print_functions.c
        src/math.c

        src/structs/dynarr.c
        src/structs/dynstring.c
        src/structs/sets.c
        src/structs/dictionary.c
        src/structs/queue.c
)

set(CLIB_PUBLIC_HDRS
        src/string_utils.h
        src/item_print_functions.h
        src/math.h

        src/structs/dynarr.h
        src/structs/dynstring.h
        src/structs/sets.h
        src/Structs/dictionary.h
        src/Structs/queue.h
)

add_library(clib_core STATIC ${CLIB_SOURCES})

install(TARGETS clib_core ARCHIVE DESTINATION lib)


# ————————————————————————————————————
#  3) Enable CTest and define each test executable
# ————————————————————————————————————

enable_testing()

add_executable(tests
        tests/tests.c
)
target_link_libraries(tests PRIVATE clib_core)
add_test(
        NAME all_unit_tests
        COMMAND tests
)


add_executable(test_sets
        tests/test_sets.c
)
target_link_libraries(test_sets PRIVATE clib_core)
add_test(
        NAME sets_tests
        COMMAND test_sets
)


add_executable(test_queue
        tests/test_queue.c
)
target_link_libraries(test_queue PRIVATE clib_core)
add_test(
        NAME queue_test
        COMMAND test_queue
)


# Don't add_test, because this one should fail in some way
add_executable(test_unit_tests
        tests/test_unit_tests.c
)
target_link_libraries(test_unit_tests PRIVATE clib_core)


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

