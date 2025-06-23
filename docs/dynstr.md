# Dynamic String

also commonly referred to as "dynstr" / "dynstring"

## Brief

A string structure, capable of growing/shrinking in size.
All methods return error codes, and print
[`errors.h`](../src/headers/errors.h)
messages in case of an error/exception.

## Methods

For an exhaustive and detailed list, see the [header file](../src/structs/dynstring.h)

### (De)Initialization

Use `dynstr_init*` for initialization and `dynstr_destroy` for finalization

### Append / prepend

- `n` -- pends **at most** `n` chars
- `f` -- pends a formatted string (as in `man 3 printf`)
- `v*f` -- accepts a `va_list` instead of `...` varargs

The structure doesn't allow direct modification of the data.
All access must be mediated through the provided methods.

For modifying outside of these methods use `data_copy` and `set`,
since these methods preserve the [invariants](#invariants)

## Invariants

* `dynstr::len` is equal to number of characters in the string
* `dynstr::data` is a pointer to valid heap memory of length `cap`
* `dynstr::data[len]` is equal to `'\0'`
* `dynstr::cap` is always more than `dynstr::len` + 1
