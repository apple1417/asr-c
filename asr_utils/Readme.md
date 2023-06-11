# asr_utils
Various C++ helpers for writing ASR scripts. To get started, just include `asr_utils.h`, which will
include all other headers for you.

For full docs, you should read through all the headers. As a quick overview:

## C++ overloads
A lot of the base ASR functions have been given overloads to make calling them more convenient
\- e.g. functions taking strings now accept `std::string_view`s, you can read memory with just a
reference instead of also needing size.

The most notable upgrade is to `runtime_print_message` - it's overload is a passthrough to
`std::format`, so you can call it with format strings + args directly.

```cpp
runtime_print_message("Value is {:#x}", 0x1234);
```

## Process Info
`ProcessInfo` is a class caching various information about a process. Various following helpers take
these as input, to do things like automatically work out pointer size. It will automatically decay
back into the pid when calling a base function.

## Memory Readers
`read_mem` is an alternative to `process_read`, which simply returns the default-constructed version
of the templated type on failure. If you don't need to know about failures, it's often more
convenient to get the result as a return value.

`read_address` and `read_string` are specialized reads for their respective types, which
automatically handle pointer size and conversion to an stl string respectively.

`read_x86_offset` is useful to get the base address of a pointer out of a sigscan.

`swap_endianness` and `fix_endianness` help deal with endianness problems.

## Pointer Helpers
`RemotePointer32` and `RemotePointer64` are pointer-sized types which you can put into a struct
definition to let you more easily dereference the pointed at value.

```cpp
struct Node {
    RemotePointer32<Node> next;
    uint32_t val;
};

for (auto node = get_first_node(); node.next.addr != 0; node = node.dereference(game)) {}
```

`DeepPointer` represents a multi-step pointer path, which you can again dereference to get straight
to the end of.

## Sigscans
You can use `Pattern` to create a sigscan pattern from a string at compile time, or just pass it a
value and mask array directly. Once you have a pattern, you can search for it in a given range, or
over the main module by default.

```cpp
const constinit Pattern<15> GWORLD_PATTERN{
    "4C 8D 0C 40"        // lea r9,[rax+rax*2]
    "48 8B 05 ????????"  // mov rax,[Borderlands3.exe+6175420] <----
    "4A 8D 0C C8"        // lea rcx,[rax+r9*8]
    ,
    7};

auto gworld_ptr = sigscan(game, GWORLD_PATTERN);
auto gworld_addr = read_x86_offset(game, gworld_ptr);
```

Note you do need to explicitly specify the pattern byte size in the template, it won't be implicitly
resolved, though any incorrect size will cause a compilation error.

## Variables
`Variable` is a mostly drop in wrapper class, which automatically updates the timer variables with
any changes to its stored value.

```cpp
Variable<std::string> name{"Player Name"};
Variable<uint32_t> counter{"Counter", 100};

name = read_string(game, 0x1234);
counter = read_mem<uint32_t>(game, 0x5678);
```

You can subclass `Variable` to change it's formatting. `HexVariable` is one such class already
provided for you, which displays it's values in hex (and thus only works with integral types).

`Variable`s implicitly cast to their wrapped type, and implement assignment from and comparison with
their wrapped type - this is what makes them mostly drop in replacements. Note that this *does not
work* with other variables of the same type, if you try assign one variable directly to another you
just call the normal copy/move constructor, overwriting the key. Explicitly use `.value()` in these
cases. Since casting returns by value, if you're wrapping a more complex type you may also find it
beneficial to use `.value()`, which returns by reference.

## Mem Watchers
Combining several of the previous utilities, `MemWatcher`s watch a memory address for changes,
keeping track of it's old and new values every time you call `.update(game`. You can also
optionally provide them with a variable to store the watched value in.
