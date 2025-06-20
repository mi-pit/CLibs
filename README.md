# Library for the C language

## Standard

`C99`

`_POSIX_C_SOURCE=200809L`

---

## Includes

- `List` (dynamic array)
- `DynString` (dynamic string)
- `Set` (hash table)
- `Dictionary` (hash map)
- Error handling
- Terminal colors
- Unit testing
- Assertions with messages
- Some mathematical functions
- String utilities
- Switch "expressions"
- More...

---

## Structure

* `src/headers/` – (mostly) lightweight headers without the need for linking/…
* `src/structs/` – advanced data structures
* `src/*.[ch]` – useful algorithms

---

---

## Footnotes

### Windows support

I tried supporting non-posix, but there are just too many
caveats and weird things for me to do it.

*(like the `"%zu"` format string not working for `size_t` on a Windows compiler)*
