# C++ Naming Guide

This guide explains the C++ naming conventions and Itanium ABI mangling rules used in the sm64ds-decomp project. Understanding these is essential for writing C++ code that matches the ROM's original symbols and vtable structures.

## 1. Itanium ABI Name Mangling Quick Reference

The compiler (`mwccarm`) uses the standard Itanium C++ ABI for mangling symbols.

*   `_Z` - Mangled name prefix. All C++ symbols start with this.
*   `N...E` - Nested name sequence (used for classes/namespaces). The `N` begins the nesting, `E` ends it.
*   **Length-Prefixed Names:** Identifiers are prefixed by their length in characters (e.g., `6Player` for `Player`).

### Type Codes (Arguments & Returns)
*   `v` = `void`
*   `b` = `bool`
*   `c` = `char`
*   `h` = `unsigned char`
*   `s` = `short`
*   `t` = `unsigned short`
*   `i` = `int`
*   `j` = `unsigned int`
*   `l` = `long`
*   `m` = `unsigned long`
*   `x` = `long long`
*   `y` = `unsigned long long`
*   `f` = `float`
*   `d` = `double`

### Qualifiers
*   `P` = Pointer (`*`)
*   `R` = Reference (`&`)
*   `K` = `const`

### Templates & Substitutions
*   `I...E` = Template arguments.
*   `S_`, `S0_`, `S1_` = Substitutions (used to compress repeated types or names in the signature).

### Real Game Examples
*   `_ZN6Player11St_Owl_MainEv` &rarr; `Player::St_Owl_Main(void)`
*   `_ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_` &rarr; `Actor::SetRanges(Fix12<int>, Fix12<int>, Fix12<int>, Fix12<int>)`
    *   *Note the `S1_` substituting the `Fix12<int>` type to save space.*
*   `_Z14ApproachLinearR7Vector3RKS_5Fix12IiE` &rarr; `ApproachLinear(Vector3&, const Vector3&, Fix12<int>)`
    *   *`R7Vector3` is `Vector3&`. `RKS_` uses substitution to mean `const Vector3&`.*

## 2. Constructor & Destructor Variants

In C++, constructors and destructors have multiple variants in the compiled object file to handle virtual inheritance and allocation.

*   **Constructors:**
    *   `C1` - Complete object constructor.
    *   `C2` - Base object constructor (called when the class is a base of another class).
    *   `C3` - Allocating constructor.
*   **Destructors:**
    *   `D0` - Deleting destructor (destroys the object and calls `delete`/deallocator).
    *   `D1` - Complete object destructor.
    *   `D2` - Base object destructor.

### Real Game Examples
*   `_ZN5ActorC2Ev` &rarr; `Actor::Actor()` (Base constructor)
*   `_ZN5ActorD0Ev` &rarr; `Actor::~Actor()` (Deleting destructor, often appears as vtable slot 17)
*   `_ZN5ActorD1Ev` &rarr; `Actor::~Actor()` (Complete destructor, often appears as vtable slot 16)

You will frequently see `D1` and `D0` assigned as the last two slots in an actor's virtual function table.

## 3. Thunks Explained

Thunks are small compiler-generated functions that adjust the `this` pointer before jumping to the actual function implementation. They are necessary in C++ when using multiple inheritance.

*   `_ZThn<offset>_` - Non-virtual `this`-adjustment thunk.
*   `_ZTv<voff>_n<off>_` - Virtual `this`-adjustment thunk.

### Real Game Example
*   `_ZThn80_N9ModelAnimD0Ev` &rarr; Non-virtual thunk adjusting `this` by `-80` for `ModelAnim::~ModelAnim()`.
    *   If you call a virtual destructor through a base class pointer, and that base class is located at offset 80 within the derived class, the thunk subtracts 80 from the `this` pointer before invoking the derived destructor.

## 4. Vtable Layout

To achieve byte-matching virtual dispatches, you must replicate the exact vtable layout. For most objects inheriting from `ActorBase`, the standard virtual order has 18 slots.

### The ActorBase Virtual Order
0. `InitResources`
1. `BeforeInitResources`
2. `AfterInitResources(u32)`
3. `CleanupResources`
4. `Before~`
5. `After~(u32)`
6. `Behavior`
7. `BeforeBehavior`
8. `AfterBehavior(u32)`
9. `Render`
10. `BeforeRender`
11. `AfterRender(u32)`
12. `OnPendingDestroy`
13. `Virtual34(u32, u32)`
14. `Virtual38(u32, u32)`
15. `OnHeapCreated`
16. `~Actor()` (D1 complete destructor)
17. `~Actor()` (D0 deleting destructor)

### Declaring Dummy Virtuals
If you are matching a virtual function call on an object, say calling `Behavior()` (slot 6), you must ensure your C++ structure defines the preceding virtuals so `Behavior()` lands at offset `6 * 4 = 24` in the vtable.

```cpp
struct Actor {
    virtual void dummy0();
    virtual void dummy1();
    virtual void dummy2();
    virtual void dummy3();
    virtual void dummy4();
    virtual void dummy5();
    virtual void Behavior(); // Slot 6
};
```

## 5. File Naming Conventions

When writing matching C/C++ source in the `src/` directory, adhere to the following rules:

*   **Extensions:** Use `.c` for pure C files, and `.cpp` for C++ files.
*   **The `//cpp` Marker:** For C++ files (even some `.c` files compiled as C++ to get the correct codegen), put `//cpp` exactly on the first line. This tells the compiler/build system to use `-lang c++`.
*   **`extern "C"` Patterns:** When declaring global functions that the game references by unmangled names, wrap them in `extern "C"`.
*   **One Function per File:** Generally, each matching file contains one function.
*   **Filenames:** The source filename should precisely match the mangled symbol name of the function it contains (e.g., `_ZN13OneUpMushroom8BehaviorEv.cpp`).

### Example (e.g., `_ZN13OneUpMushroom8BehaviorEv.cpp`)
```cpp
//cpp
#include "sm64ds.h"

extern "C" {
    // Other unmangled dependencies
}

void OneUpMushroom::Behavior() {
    // ...
}
```

## 6. Reading a Mangled Name by Hand

Let's break down a real symbol manually: `_ZN13OneUpMushroom8BehaviorEv`

1.  `_Z` - Mangled name prefix.
2.  `N` - Indicates a nested name (Class::Method).
3.  `13` - The next identifier is 13 characters long.
4.  `OneUpMushroom` - The class name.
5.  `8` - The next identifier is 8 characters long.
6.  `Behavior` - The method name.
7.  `E` - Ends the nested name sequence.
8.  `v` - The argument type is `void` (takes no arguments).

Result: `OneUpMushroom::Behavior(void)`

## 7. Using the Tools

The project provides Python tools to help you work with C++ names and symbols:

*   **Demangle a Symbol:** Use `demangle.py` for a quick breakdown.
    `python tools/demangle.py _ZN13OneUpMushroom8BehaviorEv`
*   **Browse Classes:** Use the C++ indexer to find all known methods for a class.
    `python tools/cpp_index.py --class Player`
*   **Rename Files:** If you deduce a function's real name and update the ledger, safely rename the source file and update its references (run dry-run first).
    `python tools/cpp_rename.py --dry-run`
*   **Cross-Check:** You can also use standard `c++filt` from GNU binutils to demangle standard Itanium names.

## 8. Common Patterns in the Codebase

When decompiling, you will frequently encounter these top classes. The number of matched methods (as of recent metrics) indicates how pervasive they are:

*   **Player** (246 methods) - Extremely complex state machines (`St_*_Init`, `St_*_Main`).
*   **Actor** (81 methods) - Base behaviors, spawning patterns.
*   **Stage** (44 methods)
*   **Heap** (31 methods)
*   **Sound** (29 methods)
*   **WithMeshClsn** (27 methods)
*   **IRQ** (24 methods)
*   **Scene** (24 methods)
*   **GX** (22 methods)
*   **Message** (22 methods)
*   **ActorBase** (22 methods)
*   **Model** (21 methods)

**Actor Spawn Patterns:** Spawn functions typically allocate the instance (`mov r0, #size` before the first `bl`) and store the class vtable (the last pool literal stored).
**Destructor Chains:** `func_ovNN_...` functions returning `this` often consist of a vtable install, several `SubObjectD1(this+off)` calls in reverse construction order, and a final `Deallocate` call.
