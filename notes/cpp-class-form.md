# C++ class-form compiler observations

This technical reference preserves measured source forms from the retired
`decomp-cpp-class-form` skill. Use the pinned compiler to test a hypothesis against
the assigned ROM range; source appearance alone does not establish emitted ABI.

The measurements are candidate techniques, not universal class-layout rules.
For TU delivery, follow [the promotion workflow](tu-promotion-conventions.md);
for ownership and handoffs, follow [PIPELINE](agents/PIPELINE.md).
The examples below do not require creating a shadow or a separate staging PR.

## Ask the compiler, don't hand-mangle

```sh
python tools/mangle.py candidate.cpp
python tools/mangle.py candidate.cpp --expect _ZN5Actor8BehaviorEv
python tools/mangle.py candidate.cpp --mangled-only --json
```

It compiles with the pinned 2004/b56 C++ flags and reads externally visible defined
ELF symbols, including processor-specific bindings such as `STB_LOPROC`, so
substitutions, thunks, ctor/dtor variants and static data are the
compiler's answer, not a guess. The current tool derives its C++ flags
from the production build flags. Symbol spelling is a compiler result; a symbol
list alone does not establish instruction bytes or historical source provenance.

Function `st_value` is section-relative and normally zero for distinct per-function
sections. Inspect the ELF section indices and offsets when measuring emission
order. `mangle.py` sorts its display by section name and symbol name, so its printed
symbol list is not an emission-order report.

## Destructor variants: the form decides the order

The recorded ov045 experiment targets **D1 then D0 and no D2**: D1 at
`0x021111a0`, size `0x44`, and D0 at `0x021111e4`, size `0x58`. The following
results describe that source shape, not a universal destructor rule:

| source form | instantiated in TU? | emits | order |
|---|---|---|---|
| `X::~X() {}` out of line | no | D2, D0, D1 + `_ZTV` | **D0 before D1** — wrong |
| `X::~X() {}` out of line | yes (`new X()`) | D2, D0, D1 + `_ZTV` | **D0 before D1** — wrong |
| `virtual ~X() {}` in class body | **no** | **nothing at all** | no variants, **no `_ZTV`** |
| `virtual ~X() {}` in class body | **yes (`new X()`)** | **D1, D0 + `_ZTV`** | **D1 then D0** — matches |

In this experiment, defining the destructor inline and instantiating the class
produced the required pair without a homeless D2. An out-of-line variant with no
configured home still needs an explicit, supported output disposition under the
candidate's link model; do not silently drop it.

In the measured matching form the destructor pair was emitted **first** —
which is what a TU whose ROM range *starts* with D1 needs.

## Vtable and RTTI anchoring

An out-of-line destructor can anchor `_ZTV`/`_ZTI`/`_ZTS` ownership. Inline
source forms and instantiation can also emit those objects, as the table shows.
Inspect the actual object before assigning ownership. Observations to test:

- In the recorded experiment, moving the destructor inline required instantiation
  to emit the vtable. Recheck this when the key function or class model changes.
- A manual `extern int _ZTV15Foo[];` imports a configured symbol rather than
  expressing class ownership. Check for a competing declaration when the compiler
  should emit the vtable, and reprove all outputs after changing the source form.
- Conversely, adding a virtual declaration can make a file become the key-function TU
  and start emitting `_ZTV`, which drops it from the build while `build_pin` still says
  `True` in the recorded experiment. Check actual default-build coverage after
  adding a virtual declaration. It can also affect a sibling `D0` file you never touched — `delete p`, not `p->~X()`.
- A `_ZTV*` relocation's addend must **lose 8** on rebinding: mwcc's vtable symbol
  addresses the object's start, `symbols.txt`'s addresses the slot array. In source, the
  same fact appears as `+ 2` on an `int *`. Getting it wrong links clean and corrupts 34
  modules in the recorded audit. Verify the relocation addend and destination,
  including the final link's address point.

## Constructors

An implicit constructor can inline where the ROM instead calls
`_ZN8PlatformC2Ev`. An out-of-line declaration can preserve that call when its
real definition is owned elsewhere. Confirm the definition and link destination;
do not introduce an unresolved constructor merely to steer compilation.

A hand-rolled spawn like
`p = operator new(0x328); PlatformC2Ev(p); p[0] = _ZTV + 2;`
is the *un-migrated* form. Converting it to `return new X();` is what odr-uses the
vtable and unlocks the destructor pair above.

## C-vs-C++ codegen differences that cost bytes

- **Struct copy can scalarize in C++** where C produces a block move, leaving the
  recorded candidate 12 bytes short. A tested source form copies through a struct
  whose only member is an array
  (`struct M { int w[12]; };`), which restores the ROM's `ldm`/`stm` pair. Applying this
  mechanically to a corpus of C++ divergences recovered **66% of word-level mismatches
  with zero residual**.
- **Boolean intermediates**: `a == b` has type `int` in C and `bool` in C++, even
  when surrounding casts ultimately produce `int`. The recorded cast chain
  `(int)((long long)(a == b))` changed codegen by 17 words on a language-mode switch. Also `int f = (x==k); if (f)` is 3 instructions
  longer than `if (x==k)`; the temporary is load-bearing.
- **A qualified call suppresses virtual dispatch**: `obj.Base::Method()` emits a direct
  `bl`. This goes **both ways** — read the shadow struct before assuming.
- **Compound assignment costs an instruction**: `x += k` CSEs the field address.
  Raw-cast-vs-member is decided **per function**; only the byte gate settles it.
- **A bare C++ function declaration using an already-mangled identifier can mangle
  that identifier again.** This is a function-linkage warning, not a rule about
  namespace-scope `_ZTV` data declarations. Existing configured ABI bridges need
  correct linkage; genuine methods should let the compiler spell their names.
- **`extern "C" { int x; }` DEFINES `x`**, it does not declare it, and collides with the
  gap object. Use `extern`.

## Language mode is per file, and it is not the extension

The first-line `//cpp` marker swaps `-lang c99` for `-lang c++`. `build_pin.py` calls it
"the whole test"; the extension is never consulted. Whole-corpus measurement: **90.6% of
C99-matching functions reproduce byte-identically under `-lang c++` unchanged** (94.0%
excluding `__sinit_` files). These are historical survey counts. Common compile
failures included source-language differences: empty-paren prototypes (`func_x();` means "unspecified args" in C and "no
args" in C++), `this` used as a parameter name, `enum { false, true };`, `void*` needing
an explicit cast.

## Never trust bytes alone

`match.compare` **wildcards every relocated word**, so a class method can reproduce the
bytes while calling the wrong function. Require all three: byte comparison,
`objisolate` (relocation type/addend), and `reloc_audit` (destination identity). A
result that could not run the audits is "not verified", never "probably fine".
