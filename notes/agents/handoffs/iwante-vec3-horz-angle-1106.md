# Handoff: Iwante Vec3_HorzAngle parameter types

## Identity and resumption

- Task: match `daIwante_c`'s local `Vec3_HorzAngle` declaration to the helper definition.
- Base: latest `main` at `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Candidate: isolated Iwante caller correction; independent source review remains outstanding.
- The existing `__sinit_d_a_iwante.cpp` section is not in the TU manifest and remains a promotion blocker unrelated to this signature correction.

## Change and evidence

`src/Vec3_HorzAngle.cpp` defines the C-linkage helper as `short (const Vector3 *, const Vector3 *)`. `src/game/actors/d_a_iwante.cpp` previously declared both inputs as `const void *`. The declaration now matches the definition. Its two calls use the actor's shared `mPos` vector overlay and an explicit `Vector3` view of `mTargetPosX/Y/Z`, which are adjacent `s32` fields at offsets `0x4bc`, `0x4c0`, and `0x4c4`.

`math/CalcLib.h` has the correct declaration, but including it conflicts with this TU's retained local opaque `Vec3_Dist`, `Vec3_Asr`, and `Vec3_HorzDist` prototypes. The local declaration is therefore kept with an explicit reason. No storage or control flow changed.

## Verification

- `python tools/tubuild.py verify ov066/daIwante_c --no-write --verdict`: 64/64 functions match; object isolation, relocation destinations, and emission order are clean. Exit 1 only for the pre-existing unlicensed `.text` symbol `__sinit_d_a_iwante.cpp` (0x7a8 bytes); no function mismatch.
- `python tools/prepush_linkcheck.py --range a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7..HEAD`: 65 checked, 63 verified, two blind warnings (`daIwante_c_classInit` and `__sinit`), zero blocking.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules, 11,395/11,395 functions, and 470/470 source-owned data claims reproduce; zero mismatches.
- `python tools/check_decl_agreement.py --changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new declaration disagreements or local redeclarations of header declarations.
- `python tools/source_style.py --base a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7 --files src/game/actors/d_a_iwante.cpp`: zero findings with clang-format 19.1.7.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

## Verifier handoff

Independently verify that `mPos` aliases the original actor position triple and that the `mTargetPosX` view spans the three documented adjacent target-position words. Rerun the focused TU proof and full ROM gates on the pushed candidate. Keep the `__sinit` manifest blocker separate from this helper declaration change.
