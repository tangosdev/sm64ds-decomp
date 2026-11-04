# Handoff: Owl Vec3_HorzAngle parameter types

## Identity and resumption

- Task: match Owl's local `Vec3_HorzAngle` declaration and three callers to the helper definition.
- Base: latest `main` at `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Candidate: isolated Owl caller correction; independent source review remains outstanding.

## Change and evidence

`src/Vec3_HorzAngle.cpp` defines the C-linkage helper as `short (const Vector3 *, const Vector3 *)`. Owl's local prototype used `const void *`. It now matches. The three callers use the shared actor-position overlay, an existing `Vector3` local, or an explicit `Vector3` view of the three contiguous anchor fields (`mAnchorX/Y/Z` at offsets `0x3d8`, `0x3dc`, and `0x3e0`). No storage or control flow changed.

`math/CalcLib.h` has the correct declaration, but including it conflicts with Owl's retained opaque `Vec3_Dist` and `Vec3_VertAngle` prototypes. The local declaration remains with that reason.

## Verification

- `python tools/tubuild.py verify ov094/daOwl_c --no-write --verdict`: 23/23 functions match; object isolation, relocation destinations, and emission order are clean. This TU has no unlicensed sections and is promotable under the verifier's TU policy.
- `python tools/prepush_linkcheck.py --range a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7..HEAD`: 22 verified, one blind `__sinit` warning, zero blocking.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules, 11,395/11,395 functions, and 470/470 source-owned data claims reproduce; zero mismatches.
- `python tools/check_decl_agreement.py --changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new declaration disagreements or local redeclarations of header declarations.
- `python tools/source_style.py --base a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7 --files src/game/actors/d_a_owl.cpp`: zero findings with clang-format 19.1.7.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

## Verifier handoff

Independently verify that `mPos` and the three anchor fields describe the same vectors the old calls passed. The focused proof reports no unlicensed section and a promotable TU policy result; future source ownership or TU restructuring work can build on this verification.
