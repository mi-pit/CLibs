# Dynamic Array – `List`

A flexible dynamic array ("list") implementation in C.

It can store elements of arbitrary size, provides dynamic resizing, and supports basic operations such as adding,
removing, sorting, and more.

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

### `_p` Suffix

The `_p` variants also accept a **print mode**, which determines how elements are printed (see section **Printing**).

---

## Default Configuration

```
List::capacity = LIST_DEF_SIZE  // Default: 64
List::size     = 0
List::items    = calloc(capacity, el_size)
List::el_size  = sizeof(element)
```

---

## Core API

### Add & Insert

The functions `list_append` & `insert` accept a pointer to the data.

This means the usage is

```c++
List *num_ls = list_init_type(int);
int num = 123;
list_append(num_ls, &num); // note the `&'
```

#### Other methods

* `list_access` – similar to array indexing, allows modification
* `list_fetch` – returns a value from the list, doesn't allow modification
* `list_copy`
* `list_destroy`
* `list_clear`
* `list_extend`
* `list_extend_list`
* `list_insert`
* `list_is_empty`
* `list_size`
* `list_pop`
* `list_remove`
* `list_remove_fast`
* `list_reverse`
* `list_set_print_mode`
* `list_sort`
* `list_cmp_elsize`
* `list_cmp_size`

and probably more

### Printing

* #### `list_printf` Macros

```c++
list_printf( LIST, ITEM_TYPE, FORMAT )
```

see `array_printf` for more info

* #### `list_print` Family Functions

deprecated, use `list_printf`

---

## Notes

- All elements must be **fixed size** and homogeneous. All methods assume this.
- All functions perform bounds checks and return either pointers or `int` success status. (see `headers/errors.h`)
- All functions have their own – more detailed – docs in the header file

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
