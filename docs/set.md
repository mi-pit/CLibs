# Set

Hash-table based unordered set

A **set** stores a collection of **unique** elements and provides approximately constant-time insertion, lookup, and
removal. Internally it uses an open-addressing hash table with linear probing and automatic resizing.

---

### Table of Contents

1. [Data Structure](#data-structure)
2. [Public API](#public-api)
3. [Usage Example](#usage-example)
4. [Performance & Complexity](#performance--complexity)
5. [Notes & Limitations](#notes)

---

## Data Structure

Key characteristics:

- Unique keys only
- Unordered collection
- Generic

Data hashing is done by `hash_func` in `headers/misc.h`

---

## Public API

Include the header and then use the following functions/macros:

```c++
#include "set.h"
```

### Function / Macro Description

- `init` – Initialize a heap‐allocated set.
- `destroy` – Free all memory ***owned*** by the set.
- `insert` – Insert key into the set. (see [return values](#return-values))
- `remove` – Remove key from the set.
- `search` – Predicate which returns true if the element is in the set.
- `size` – Number of elements currently stored.
- `print` – Prints each item.

### Print

Each item has its own `PrintFunction` as an attribute
set at the time of insertion (see `insert_f`).
This may be overridden by `print_as`.

The type of the print function must be the equivalent
to the type of the element data.

### Return values

Methods returning `int`, unless specified otherwise,
may return `RV_EXCEPTION` or `RV_ERROR` (defined in `headers/errors.h`).
Each method has its own rules for returning these.

#### Insert/Remove

The `insert` and `remove` methods have their own return values.

Apart from the normal `int` rv's,
they return values from `enum SetInsertRV` and `enum SetRemoveRV` respectively

---

## Usage Example

```c++
Set *set = set_init();
if ( set == NULL )
    /* handle error */

int number = 123;
assert( set_insert( set, &number, sizeof number ) == SETINSERT_INSERTED );
/* no items are present => must have been inserted (unless RV_ERROR is returned) */

assert( set_insert( set, &number, sizeof number ) == SETINSERT_WAS_IN );
/* item was already present */

set_destroy( set ); // end of lifetime
```

---

## Performance & Complexity

Insert, search, remove:

* O(1) on average
* O(n) worst-case

---

## Notes

* Not thread-safe; concurrent access must be synchronized externally.
* Does not store values – only keys. For ‹key -> value› maps, use the `dictionary` module.

---
