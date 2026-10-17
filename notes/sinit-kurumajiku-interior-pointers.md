# sinit fold blocker: ov002/daObjKurumajiku_c (0x02100f84)

`__sinit_ov002_02100f84` cannot be folded into `src/game/actors/d_a_obj_kurumajiku.cpp`
under the current intact-object/delink machinery. Documented so the next attempt
starts from the mechanism wall, not from scratch.

## Shape

Retail `.init` 0x02100f84..0x02101064 (0xe0) constructs four 3-word
mount-offset vectors and registers each with `func_020731dc`, using
`_ZN7Vector3D1Ev` as the destructor and four registration nodes in `.bss`:

- nodes: 0x0210dda0, 0x0210ddac, 0x0210ddb8, 0x0210ddc4 (0xc each)
- array: 0x0210ddd0..0x0210de00 (0x30), elements at +0x00/+0x0c/+0x18/+0x24

The generated `__sinit_d_a_obj_kurumajiku.cpp` from a four-element wrapper
array matches retail byte-for-byte, including the literal pool, which contains
pointers at all four element addresses (0x0210ddd0/…dddc/…dde8/…ddf4).

## Why it cannot work today

Two independent constraints collide:

1. **Only a single four-element aggregate emits the retail schedule.**
   Four separate wrapper objects, and four `[1]` arrays, both take mwcc's
   per-object scalar codegen path (~16-29 word .init differences). The shared
   constant reuse across elements only happens inside one aggregate
   initialization.

2. **A single emitted object cannot expose interior symbols.** The `.init`
   literal pool needs real symbols at +0x0c/+0x18/+0x24 inside the emitted
   0x30 array:

   - `dsd delink` refuses the retail `.init` without configured symbols at each
     reloc target ("No symbol found for relocation").
   - `dsd check symbols` refuses the linked ELF when the interior names only
     exist as `ambiguous` phantom rows.
   - `symbols.txt` has function/bss/data kinds only; `ambiguous` is a
     module-ambiguity flag, not an interior-symbol flag.
   - `relocs.txt` `to:` targets are address-only; no symbol+addend spelling.
   - `partitionSymbols`/`storageAlias` are bound to vtable rebias policies
     (positive bias, vtable-shaped validation).
   - `symbol_binding_rewrites` changes STB_LOCAL/GLOBAL, not names.
   - `deadstrip`/`deadstrip-data` refuse: surviving `.init`/`.text` reference
     the array (only `_ZTV`/`_ZTI`/`_ZTS` shapes may be stripped under
     surviving references).
   - licensing the emitted array via manifest `bss` rows requires a `.bss`
     section claim, which makes the emitted contribution fill the whole range
     — the gap then cannot supply the interior names either.
   - leaving `.bss` unclaimed makes the emitted bss section an unlicensed
     content section (object audit refusal), and emitting
     `data_ov002_0210ddd0` while it is gap-owned is COLLIDES-GAP (fatal).
   - `complete` delink entries serve ROM bytes; the source anchor is never
     compiled, so the deleted `__sinit` file cannot emit the bss objects
     carrying the element names.

The original handwritten file worked precisely because `complete` serves
retail `.init` bytes directly while the gap supplies every `.bss` name — the
fold removes the only shape that can name the interior addresses.

## If dsd ever grows interior-symbol support

The fold is otherwise ready and byte-verified. Needed:

- a way to declare symbols at interior offsets of an emitted object, or
- `to:` reloc targets as symbol+addend, or
- a delink-time alias kind in `symbols.txt`.

Then: define `KurumajikuMountOffset KurumajikuMountOffsets[4]` (inline 3-arg
ctor storing to `Vector3` base + declared-undefined dtor aliased to
`_ZN7Vector3D1Ev`) at the end of `d_a_obj_kurumajiku.cpp`, keep the POD-local
trick in `Behavior` (declare `struct { int x,y,z; }` locals instead of
`Vector3` so `_ZN7Vector3D1Ev` stays importable), claim
`.text/.init/.ctor/.bss`, and delete the handwritten anchor.
