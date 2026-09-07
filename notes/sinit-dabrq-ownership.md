# daBrq static-initializer ownership and generation proof

This is a production-boundary audit for `__sinit_ov070_02122d80`. It does not
change an enrolled source, linker configuration, or symbol name. The guarded
probe in `notes/sinit-probes/dabrq.cpp` contains ordinary global C++ objects and
no hand-written initializer.

## What this note is, and is not

It is a proof note. It was first written on a branch named
`cpp/dabrq-sinit-resource-tu`, and that name overstates the work: nothing here
regenerates the initializer from a "resource TU", and the production enrolment
of the initializer was never changed. On `main` today:

- `src/__sinit_ov070_02122d80.cpp` is still the hand-transcribed, separately
  enrolled initializer, byte-identical to the version this audit was measured
  against;
- `src/game/actors/daBrq_c.cpp`, described by
  `config/tu_manifest.d/ov070/daBrq_c.json`, is the promoted class TU. Its
  manifest records that the initializer, its `.ctor` word, the PMF input
  table, the resource globals, and the state-table BSS remain separately
  enrolled;
- `src/d_a_brq.c` holds the factory `daBrq_c_classInit` (historical project
  alias `Amp_Spawn`), enrolled outside the class TU because the pinned
  compiler cannot express placement construction against the retail `fBase_c`
  allocator. That factory is the nineteenth `daBrq_c` symbol in the overlay;
  the manifest's eighteen functions plus this factory cover every one.

The audit answers one question only: could the original `daBrq_c` source have
produced this initializer organically? It could, as shown below.

## Verdict

`__sinit_ov070_02122d80`, its `.ctor` word, six PMF descriptors, and the BSS
objects they initialize belong to the `ov070/daBrq_c` translation unit.
CodeWarrior 2004/b56 organically reproduces the entire initializer from five
resource objects, three state-handler pairs, and one `Vector3`-shaped object:

- `.init`: `0x1b0` bytes, raw-byte identical;
- `.rela.init`: all 34 offsets, types, and addends identical;
- `.ctor`: one 4-byte pointer to the generated initializer;
- `.data`: six independent 8-byte PMF descriptors (`0x30` bytes total);
- `.bss`: five 8-byte resource handles, six 12-byte registration nodes, one
  48-byte state-handler array, and one 12-byte vector (`0xac` bytes total);
- no hand-written call to `__register_global_object`.

The exact initializer transcription remains stronger evidence for final
relocation destinations. The neutral probe deliberately emits neutral resource
class names, so its constructor and destructor relocation targets differ in
spelling while every instruction and relocation slot agrees:

| Organic object | ROM constructor target | ROM destructor target |
| --- | --- | --- |
| model handle `0x2b1` | `func_02017acc` | `func_02017ab4` |
| model handle `0x2b3` | `func_02017acc` | `func_02017ab4` |
| animation handle `0x2b2` | `_ZN13SharedFilePtr9ConstructEj` | `SharedFilePtr_Destruct_Anim` |
| animation handle `0x2b5` | `_ZN13SharedFilePtr9ConstructEj` | `SharedFilePtr_Destruct_Anim` |
| texture-sequence handle `0x2b4` | `SharedFilePtr_Construct_TexSeq` | `SharedFilePtr_Destruct_TexSeq` |
| cylinder offset | inline three-word construction | `_ZN7Vector3D1Ev` |

The five resource globals are exactly eight bytes apart where applicable. The
shared constructor body proves their layout as `u16 fileID`, `u8 refCount`, one
pad byte, and a pointer at offset 4. The initializer's non-const PMF array copies
six 8-byte descriptors in this source order:

1. `daBrq_c::EnterCooldownState`, `daBrq_c::UpdateCooldownState`;
2. `daBrq_c::EnterActiveState`, `daBrq_c::UpdateActiveState`;
3. `daBrq_c::EnterDefeatedState`, `daBrq_c::UpdateDefeatedState`.

The final object is dynamically constructed in BSS as `{0, -0x28000, 0}` and
registered with the real `Vector3` destructor. A neutral type with the same
three-argument inline constructor reproduces those stores exactly. The current
shared `Vector3` declaration omits that constructor so C++ files can still use
legacy aggregate initialization; changing that widely included header is not
part of this audit.

## Symbol spacing on `main`: no absorbed array, no phantom index

An inferred initializer can silently absorb a short array into the entry below
it and invent indices that do not exist. The ov070 symbol table rules that out
here. In `config/arm9/overlays/ov070/symbols.txt`:

- five resource handles at `0x021235ec`, `0x021235f4`, `0x021235fc`,
  `0x02123604`, `0x0212360c`: every 8 bytes;
- six registration nodes at `0x02123614`, `0x02123620`, `0x0212362c`,
  `0x02123638`, `0x02123644`, `0x02123650`: every 12 bytes;
- the `Vector3` at `0x0212365c`: 12 bytes;
- the state-handler array at `0x02123668`, with the next symbol at
  `0x02123698`: exactly `0x30` = three pairs of two 8-byte PMF descriptors;
- six PMF descriptors in `.data` at `0x0212320c`, `0x02123214`, `0x0212321c`,
  `0x02123224`, `0x0212322c`, `0x02123234`: every 8 bytes, ending at
  `0x0212323c` where `_ZTS7daBrq_c` begins.

Every object the initializer touches has its own symbol at the expected
distance from its neighbours, so the six-entry PMF table and the three-pair
state array are what the ROM lays out, not an artifact of the transcription.

## Production boundary

Do not replace the exact initializer with the neutral probe classes. The ROM
does not preserve the original non-polymorphic resource class names, and the
current `SharedFilePtr_Construct_*` / `SharedFilePtr_Destruct_*` labels are
resource-family aliases rather than proof of original C++ type spellings.
Promoting neutral names would make the bytes match only after speculative
symbol renames.

The safe next step is an engine-level reconstruction of the resource wrapper
types at `0x020178b4..0x02017b64`. Once their genuine constructor/destructor
identities are settled, this initializer can move into the real `daBrq_c` TU
without a hand-written `extern "C"` initializer.

## Reproduction

Compile the guarded probe with the pinned production C++ flags (the
`CPP_FLAGS` string in `tools/swarm.py`, through `compile_c` in
`tools/match.py`) plus `-DSINIT_OWNERSHIP_PROBE`. Comparing `__sinit_dabrq.cpp`
in that object with `__sinit_ov070_02122d80` from the committed exact
transcription gives:

```text
bytes: 0x1b0/0x1b0 equal=True
relocations: 34/34
all 34 relocation offsets/types/addends: equal
```

Compiling without `SINIT_OWNERSHIP_PROBE` contributes no production sections.

Re-measured on 2026-09-07 against `main` at `88dbe66db` for
https://github.com/tangosdev/sm64ds-decomp/issues/2422, with the same result:
34 of 34 relocation slots agree in offset, type, and addend, and the only
differences are the 34 symbol spellings the table above maps (neutral probe
constructors, destructors, and objects against the ROM's `func_*`,
`SharedFilePtr_*`, `data_ov070_*`, and `__register_global_object` labels).
