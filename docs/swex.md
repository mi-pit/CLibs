# `swex`: Structured, Nested `switch` Replacement

`swex` (SWitch EXpression EXtended) is a macro-based system that enables
deeply nested, typed, and resolvable switch-style logic in C,
allowing more control flow than plain-C switch/case.

---

## Functionality

* Immediate and string matching via `swex_case_imm`, `swex_case_ptr` and `swex_case_str`
* Optional default branch
* No fallthrough
* Single-point value resolution with `resolve()`

---

## Example

```c++
void swex_example( const char *choice )
{
    swex_init_str( choice ) as_new( int, rv )
    {
        swex_case_str( "Custom" ) {
            resolve( int, 100 );
        }
        swex_case_str( "Other" ) {
            resolve( int, -100 );
        }
        swex_default() {
            exit( 1 );
        }
    }
    swex_finish();
    
    printf( "%d\n", rv );
}
```

---

## Macro Overview

- `swex_init_val(type, val)` – Starts a switch block with a typed value
- `swex_init_str(val)` – Starts a switch block on a string
- `as(var)`/`as_new(type, var)` – Declares or defines the result variable
- `swex_case_imm(type, val)` – Matches an immediate value (e.g., integer)
- `swex_case_str(str)` – Matches a string literal
- `swex_default()` – Default case fallback
- `resolve(type, val)` – Resolves the switch to a value
- `swex_finish()` – Closes a switch block (must be present after every block)

---
