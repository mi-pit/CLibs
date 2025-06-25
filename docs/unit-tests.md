# `unit_tests.h`

Header-only unit testing utility.

Provides a lightweight, color-coded test runner for C projects.
Supports test grouping, verbosity control, and automatic summary output on program exit.

Requires no external dependencies.

---

## API Overview

### Functions / function-like macros

* `TEST(name)`/`END_TEST` – Define a test block.
* `UNIT_TEST(condition)` – Run an individual test. Must be in a test block.
* `RUN_TEST(name)` – Invoke a test block.
* `FINISH_TESTING()` – Print summary and exit.
* `GET/SET_UNIT_TEST_VERBOSITY` – Toggle display of passing tests (`false` => silent). Failed unit-tests still print.
  Useful for loops.

### Optional definitions

* `UNIT_TESTS_SILENT` – If this macro is defined, **all** `UNIT_TEST` calls are silenced completely
* `LINE_PREF_WIDTH` – Preferred width of a line – dictates the number of dots in a test case (some constraints apply)
* `COLOR_FAIL` – Color of a failed test – `FOREGROUND_RED` by default
* `COLOR_SUCC` – Color of a successful test – `FOREGROUND_GREEN` by default
* `COLOR_NOTE` – Color of the summary note – `FOREGROUND_CYAN` by default
* `COLOR_TEST_TAG` – Color of the `[TEST]` and `[UNIT_TEST]` tags – `FOREGROUND_YELLOW` by default

---

## Example

```c++
TEST(test_example)
{
    UNIT_TEST(1 == 1);
    UNIT_TEST(1 == 2);
    UNIT_TEST(2 == 1);
    UNIT_TEST(2 == 2);
}
END_TEST

int main( void )
{
    RUN_TEST(test_example);
    FINISH_TESTING();
}
```

Output: (+ colors if defined)

```
[TEST] test_example
    [UNIT TEST] 1 == 1 ................... SUCCESS
    [UNIT TEST] 1 == 2 ................... FAILURE
    [UNIT TEST] 2 == 1 ................... FAILURE
    [UNIT TEST] 2 == 2 ................... SUCCESS
[TEST] test_example: ran 4 tests, 2 successful, 2 failed

[SUMMARY] total unit tests ran: 4, total unit tests failed: 2
[SUMMARY] total tests ran: 1, total tests failed: 1
```

---

## Requirements

* `src/headers` only

---
