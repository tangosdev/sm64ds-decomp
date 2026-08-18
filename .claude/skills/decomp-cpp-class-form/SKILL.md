---
name: decomp-cpp-class-form
description: Choose the C++ source form that makes mwccarm 2004/b56 emit what the SM64DS ROM actually contains - destructor variant order (D1/D0/D2), vtable and RTTI anchoring, constructor inlining, struct copies, bool widening, and virtual-vs-direct calls. Use when converting a shadow struct into a real C++ class, when D0/D1 are missing or emitted in the wrong order, when _ZTV/_ZTI is absent or multiply-defined, when a .c -> .cpp rename costs bytes, or when a class compiles but the whole-range link will not reproduce.
---

# Making mwccarm emit the ROM's shape

Byte-matching a real C++ class is a question about **source form**, not about being
"more correct". The compiler is an oracle — ask it rather than reasoning about the ABI.

## Ask the compiler, don't hand-mangle

```
python tools/mangle.py candidate.cpp
python tools/mangle.py candidate.cpp --expect _ZN5Actor8BehaviorEv
python tools/mangle.py candidate.cpp --mangled-only --json
```

It compiles with the pinned 2004/b56 C++ flags and reads defined GLOBAL/WEAK ELF
symbols, so substitutions, thunks, ctor/dtor variants and static data are the
compiler's answer, not a guess. It uses `match.DEFAULT_FLAGS`, which differs from
`rombuild.CFLAGS` (`-w illpragmas` vs `-Cpp_exceptions off`) — **verified not to change
the emitted symbol set**, so it is sound for the checks below.

**Ordering cannot be read from `st_value`.** mwccarm emits one `.text` section per
function, so every symbol's `st_value` is 0. Order lives in the **section index**.

## Destructor variants: the form decides the order

The ROM typically has **D1 then D0 and no D2** (e.g. ov045: D1 `0x021111a0` size 0x44,
D0 `0x021111e4` size 0x58, landing exactly on the next function). Measured:

| source form | instantiated in TU? | emits | order |
|---|---|---|---|
| `X::~X() {}` out of line | no | D2, D0, D1 + `_ZTV` | **D0 before D1** — wrong |
| `X::~X() {}` out of line | yes (`new X()`) | D2, D0, D1 + `_ZTV` | **D0 before D1** — wrong |
| `virtual ~X() {}` in class body | **no** | **nothing at all** | no variants, **no `_ZTV`** |
| `virtual ~X() {}` in class body | **yes (`new X()`)** | **D1, D0 + `_ZTV`** | **D1 then D0** — matches |

Two conditions, both necessary: **define the destructor inline in the class body, and
make the TU actually instantiate the class.** The inline form also emits no `D2`, which
removes a homeless `_ZN…D2Ev` that has no `symbols.txt` entry and cannot be dropped
under `-nodead`.

In the winning form the destructor pair is emitted **first**, ahead of everything else —
which is what a TU whose ROM range *starts* with D1 needs.

## Vtable and RTTI anchoring

**mwcc anchors `_ZTV`/`_ZTI`/`_ZTS` to the TU that defines the destructor out of line.**
Consequences:

- Move the destructor inline and the anchor disappears; nothing emits the vtable unless
  something odr-uses it (instantiation).
- A TU that declares `extern int _ZTV15Foo[];` imports the vtable and will never define
  it — and will not emit the destructor variants the vtable would reference either.
  If the ROM puts the vtable in *this* TU, that `extern` is the bug.
- Conversely, adding a virtual declaration can make a file become the key-function TU
  and start emitting `_ZTV`, which drops it from the build while `build_pin` still says
  `True`. **Check `eligible-names.txt` after adding any virtual.** It can also un-build a
  sibling `D0` file you never touched — `delete p`, not `p->~X()`.
- A `_ZTV*` relocation's addend must **lose 8** on rebinding: mwcc's vtable symbol
  addresses the object's start, `symbols.txt`'s addresses the slot array. In source, the
  same fact appears as `+ 2` on an `int *`. Getting it wrong links clean and corrupts 34
  modules. `objisolate`'s addend check is the only gate that sees it.

## Constructors

**An implicit constructor gets inlined**, so a TU that expects a real `bl` to
`_ZN8PlatformC2Ev` will not produce one. Declaring `X();` and never defining it is the
fix — read this before migrating any constructor.

A hand-rolled spawn like
`p = operator new(0x328); PlatformC2Ev(p); p[0] = _ZTV + 2;`
is the *un-migrated* form. Converting it to `return new X();` is what odr-uses the
vtable and unlocks the destructor pair above.

## C-vs-C++ codegen differences that cost bytes

- **Struct copy is scalarized in C++** but a block move in C — typically 12 bytes short.
  Fix: copy through a struct whose only member is an array
  (`struct M { int w[12]; };`), which restores the ROM's `ldm`/`stm` pair. Applying this
  mechanically to a corpus of C++ divergences recovered **66% of word-level mismatches
  with zero residual**.
- **`bool` widening**: `(int)((long long)(a == b))` is `int` in C, `bool` in C++ — a
  `.c` -> `.cpp` rename cost 17 words. Also `int f = (x==k); if (f)` is 3 instructions
  longer than `if (x==k)`; the temporary is load-bearing.
- **A qualified call suppresses virtual dispatch**: `obj.Base::Method()` emits a direct
  `bl`. This goes **both ways** — read the shadow struct before assuming.
- **Compound assignment costs an instruction**: `x += k` CSEs the field address.
  Raw-cast-vs-member is decided **per function**; only the byte gate settles it.
- **A bare `extern` on an already-mangled name inside a `//cpp` file mangles it twice.**
  Wrap ROM symbols spelled by their mangled name in `extern "C"`.
- **`extern "C" { int x; }` DEFINES `x`**, it does not declare it, and collides with the
  gap object. Use `extern`.

## Language mode is per file, and it is not the extension

The first-line `//cpp` marker swaps `-lang c99` for `-lang c++`. `build_pin.py` calls it
"the whole test"; the extension is never consulted. Whole-corpus measurement: **90.6% of
C99-matching functions reproduce byte-identically under `-lang c++` unchanged** (94.0%
excluding `__sinit_` files), so pure C++ is reachable. The failures are source hygiene,
not codegen: empty-paren prototypes (`func_x();` means "unspecified args" in C and "no
args" in C++), `this` used as a parameter name, `enum { false, true };`, `void*` needing
an explicit cast.

## Never trust bytes alone

`match.compare` **wildcards every relocated word**, so a class method can reproduce the
bytes while calling the wrong function. Require all three: byte comparison,
`objisolate` (relocation type/addend), and `reloc_audit` (destination identity). A
result that could not run the audits is "not verified", never "probably fine".
