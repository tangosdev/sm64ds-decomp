# Handoff: Propeller Heyho Vec3_HorzAngle parameter types

## Identity and resumption

- Task: correct the three local `Vec3_HorzAngle` declarations and calls in Propeller Heyho.
- Base: latest `main` at `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Candidate: isolated caller correction; independent source review remains outstanding.
- This TU has nine pre-existing unlicensed object sections/symbols, including `__sinit` and RTTI records, which block TU promotion independently of this call-signature change.

## Change and evidence

`src/Vec3_HorzAngle.cpp` defines the C-linkage helper as `short (const Vector3 *, const Vector3 *)`. Three local declarations spread across Propeller Heyho's C-linkage blocks used mutable `void *` arguments. They now use the exact const vector signature. The three calls pass the shared actor-position overlay and existing `Vector3` values; the home-position triple is explicitly viewed as `Vector3`.

`math/CalcLib.h` has the correct signature, but including it conflicts with this TU's retained opaque `Vec3_Dist` and `Vec3_VertAngle` declarations. Each local declaration carries that reason. No storage or control flow changed.

## Verification

- `python tools/tubuild.py verify ov070/daPropeller_Heyho_c --no-write --verdict`: 27/27 functions match; object isolation, relocation destinations, and emission order are clean. Exit 1 only because nine pre-existing `.text`/`.data` sections are not licensed by the TU manifest; no function mismatch.
- `python tools/check_decl_agreement.py --changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new declaration disagreements or local redeclarations of header declarations.
- `python tools/source_style.py --base a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7 --files src/game/actors/d_a_propeller_heyho.cpp`: zero findings with clang-format 19.1.7.
- Full ROM build, relocation linkcheck, port references, and dead-reference scan remain to be run before publishing.

## Verifier handoff

Inspect the actor/home position triples and each of the three caller contexts, then rerun the focused and full ROM proof on the pushed head. Keep the existing unlicensed sections as a separate source-ownership blocker.
