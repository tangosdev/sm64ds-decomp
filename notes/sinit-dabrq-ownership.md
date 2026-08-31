# daBrq static-initializer ownership and generation proof

This is a production-boundary audit for `__sinit_ov070_02122d80`. It does not
change an enrolled source, linker configuration, or symbol name. The guarded
probe in `notes/sinit-probes/dabrq.cpp` contains ordinary global C++ objects and
no hand-written initializer.

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

Compile the guarded probe with the pinned production C++ flags and
`-DSINIT_OWNERSHIP_PROBE`. Comparing `__sinit_dabrq.cpp` in that object with
`__sinit_ov070_02122d80` from the committed exact transcription gives:

```text
bytes: 0x1b0/0x1b0 equal=True
relocations: 34/34
all 34 relocation offsets/types/addends: equal
```

Compiling without `SINIT_OWNERSHIP_PROBE` contributes no production sections.
