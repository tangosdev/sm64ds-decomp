# Handoff: issue-4725-bombking-vertangle-caller-1104

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue and task: https://github.com/tangosdev/sm64ds-decomp/issues/4725; `issue-4725-bombking-vertangle-caller-1104`.
- Producer session: `codex-bombking-vertangle-1104a`.
- Source base and input: `a30423b61858fb7857c49bda379e715a1aad860b` (current main when claimed).
- Previous task: `issue-4725-bombking-calclib-signatures-1103`; its deferred cross-consumer finding is inherited.
- Next action: an independent verifier should rerun the exact TU and whole-ROM checks on this commit and review the type evidence below.
- The shared `include/math/CalcLib.h` is reserved by `issue-4570-levelrng-grock-object-type-1103`; this change leaves it untouched.

## Change and source evidence

- `src/Vec3_VertAngle.cpp` defines `s16 Vec3_VertAngle(const Vector3 *, const Vector3 *)`.
- `d_a_bombking.cpp` previously declared both arguments as `const void *`. The declaration now matches the definition, and the call at `func_ov078_021247bc` explicitly views the adjacent `mPosX/Y/Z` and `mArenaPosX/Y/Z` words as `Vector3` triples, consistent with the existing `Vec3_HorzAngle` and `Vec3_Dist` calls there.
- No fields, layout, exported names, or attribution changed. The local declaration remains because the shared header does not yet declare `Vec3_VertAngle`; its reason is recorded beside the declaration.

## Proof

- `python tools/tubuild.py verify ov078/daBombking_c --no-write`: 52/52 functions match, object isolation is clean, and relocation destinations are clean. The command exits 1 after reporting the generated `__sinit_d_a_bombking.cpp` as outside its `functions` inventory; the manifest separately records the initializer and constructor word. This is the same existing initializer caveat recorded in the preceding handoff.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_bombking.cpp`: 52 verified, one `__sinit` BLIND-15 warning, zero blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395 functions reproducing, zero mismatches; intact-TU symbol and storage-alias gates pass.
- `python tools/check_decl_agreement.py --changed a30423b61858fb7857c49bda379e715a1aad860b`: no new declaration disagreements or local redeclarations.
- `git diff --check`: PASS. Local `source_style.py` could not run because clang-format 19.1.7 is unavailable; hosted PR validation remains required.

## Follow-up

The source-backed signature exposes the remaining `Vec3_VertAngle` declaration disagreements across other consumers. Promote it through the shared header and update those consumers after the active `CalcLib.h` reservation clears, with affected-consumer and full-ROM proof. Do not treat this one-caller correction as completion of the shared-helper cleanup.
