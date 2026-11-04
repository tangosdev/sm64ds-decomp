# Handoff: Grock CalcLib vector helper types

## Identity and resumption

- Scope: `src/game/actors/d_a_grock.cpp`, Grock in overlay 021.
- Source branch: `grock-calclib-vector-types1104` in the audit namespace, based on current `origin/main` `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Overlap check: no open PR or issue was found for this source. The earlier Grock LevelRng change is already in current main.
- Status: focused byte and relocation proof, declaration agreement, style, and full no-ROM build passed. Independent source review and hosted PR checks remain outstanding.
- Next action: review shared helper signatures and raw three-word views on the exact PR head.

## What changed and why

This TU locally declared `Vec3_Asr`, `Vec3_HorzLen`, `AddVec3` and `RandomIntInternal`. Their shared declarations are in `math/CalcLib.h`; the first two local declarations widened the vector arguments to `void *`. The TU now includes that header and drops the duplicate prototypes. The scratch `int[3]` passed to `Vec3_Asr` and the contiguous actor speed fields passed to `Vec3_HorzLen` are explicitly viewed as `Vector3` at the helper boundary. No `Vector3` object is constructed and no storage is added.

Removing eight named prototype parameters shifted a compiler-local symbol. The baseline object defined `@967` as a 12-byte BSS symbol in ELF section 33; the candidate defines `@959` at the same section and size. The manifest keeps the established canonical target `@887` and records that measurement.

## Proof on current main base

- `python tools/tubuild.py verify ov021/daGrock_c --no-write --verdict`: 13/13 functions match; object isolation, relocation destinations, and emission order are clean. The pre-existing unlicensed `__sinit_d_a_grock.cpp` section (0x40 bytes) prevents promotion by policy.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_grock.cpp`: 14 checked, 13 verified, one `__sinit` BLIND-2 warning, zero blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or local redeclarations.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_grock.cpp` with pinned clang-format 19.1.7: zero findings, zero deferrals.
- `python tools/port_refcheck.py`: 217/217 references resolve. `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

These checks were run on the candidate source before the handoff was added. Re-run focused verification and the full build on the committed PR head before accepting it.
