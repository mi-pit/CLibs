# Dynamic Array – `List`

A flexible dynamic array ("list") implementation in C.

It can store elements of arbitrary (fixed) size,
provides dynamic resizing, and supports basic operations such as
adding, removing, sorting, and more.

---

## Features

- Stores elements of **any type** (built-in or custom)
- Automatically resizes on insertions
- Memory-safe and type-agnostic

---

## Initialization

Use one of the `list_init_*` functions:

### By Type

```c++
List *list_init_type( int );    // Element type: int
List *list_init_type( char * ); // Element type: char *
```

### By Element Size

```c++
List *list_init_size( size_t el_size ); // e.g., sizeof(struct Foo)
```

---

## Default Configuration

```c++
List::size     = 0
List::capacity = LIST_DEF_SIZE  // Default: 64
List::el_size  = sizeof(element)
List::items    = calloc(capacity, el_size)
```

---

## Core API

All method names start with `list_` followed by method name

### Adding elements

Methods `append` & `insert` accept a pointer to the data.

This means the usage is

```c++
List *num_ls = list_init_type(int);
int num = 123; // type must be the same as in the init function
list_append(num_ls, &num); // note the `&'
```

### Removing elements

Methods `pop`, `remove` and `remove_fast` all allow
the caller to specify if and where the removed element's data is stored.

```c++
List *num_ls = list_init_type(int);
// add some elements
int cont;
list_pop(num_ls, &cont); // the last number in num_ls gets stored in cont
```

### Printing

Prototype:

```c++
list_printf( LIST, ITEM_TYPE, FORMAT )
```

where

- `LIST` is a valid `List *`,
- `ITEM_TYPE` is a C keyword/typedef/…,
- `FORMAT` is a printf-like format string (e.g. `"%i"` for `int`)

Also allows the caller to choose the beginning, separator, and end strings (see `_d` & `_sde` variants).
By default, these are `[`, `, ` and `]` respectively

See `array_printf` for more info

Note that all `list_printf` variants are macros, not functions.

---

### Other methods

* `access` – similar to array indexing, allows modification
* `fetch` – returns a value from the list, doesn't allow modification
* `copy` – creates a copy of another list
* `destroy` – frees all memory owned by the list
* `clear` – resets the list to the defaults
* `append` – appends one (1) item to the end of the list
* `extend` – appends any number of items from an array
* `extend_list` – appends all items from another list
* `insert`
  * inserts one item at a specified index
  * all following items are moved forward
* `is_empty`
* `size` – returns the number of items in the list
* `pop` – removes the last item
* `remove` – removes an item from the specified index 
* `remove_fast`
* `reverse`
* `sort` – uses `<stdlib.h>`'s `qsort` to sort its elements
* `cmp_size` – compares lists based on number of items
* `cmp_elsize` – compares lists based on element size

and probably more

## Notes

- All elements must be **fixed size** and homogeneous. All methods assume this and not following this rule is UB.
- Functions perform bounds checks and return either pointers or `int` success status. (see `headers/errors.h`)
- Functions have their own – more detailed – docs in the header file

---

## Example

```c++
int main(void)
{
    List *nums = list_init_type(int);

    int n = 123;
    list_append(nums, &n);

    list_printf(nums, int, "%d");

    list_destroy(nums);
}
```

---
