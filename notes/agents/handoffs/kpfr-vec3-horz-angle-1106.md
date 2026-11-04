# Handoff: Kp Firework Vec3_HorzAngle declaration

## Identity and resumption

- Task: use the shared `Vec3_HorzAngle` declaration in Kp Firework.
- Base: latest `main` at `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Candidate: isolated helper type correction; independent source review remains outstanding.
- The TU's existing unlicensed `__sinit_d_a_kp_fr.cpp` section remains a promotion blocker unrelated to this edit.

## Change and evidence

`src/Vec3_HorzAngle.cpp` and `include/math/CalcLib.h` declare the C-linkage helper as `short (const Vector3 *, const Vector3 *)`. Kp Firework previously carried a local `void *` prototype. It now includes the shared header and removes that prototype. Its sole caller now uses the actor and player `mPos` vector overlays instead of taking the address of each scalar X field.

No storage or control flow changed.

## Verification

- `python tools/tubuild.py verify ov070/daKpFr_c --no-write --verdict`: 21/21 functions match; object isolation, relocation destinations, and emission order are clean. Exit 1 only for the pre-existing unlicensed `.text` symbol `__sinit_d_a_kp_fr.cpp` (0x74 bytes); no function mismatch.
- `python tools/prepush_linkcheck.py --range a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7..HEAD`: 22 verified, zero warnings or blocking.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules, 11,395/11,395 functions, and 470/470 source-owned data claims reproduce; zero mismatches.
- `python tools/check_decl_agreement.py --changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new declaration disagreements or local redeclarations of header declarations.
- `python tools/source_style.py --base a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7 --files src/game/actors/d_a_kp_fr.cpp`: zero findings with clang-format 19.1.7.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

## Verifier handoff

Independently confirm both `mPos` views alias the original three-word position triples, then rerun the focused and full ROM gates on the pushed candidate. Keep the existing `__sinit` manifest gap documented separately.
