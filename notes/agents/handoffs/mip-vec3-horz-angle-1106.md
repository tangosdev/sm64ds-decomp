# Handoff: Mip Vec3_HorzAngle parameter types

## Identity and resumption

- Task: match `daMip_c`'s local `Vec3_HorzAngle` declaration and eight callers to the helper definition.
- Base: latest `main` at `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Candidate: isolated Mip caller correction; independent source review remains outstanding.
- The existing `__sinit_d_a_mip.cpp` section is not in the TU manifest and remains a promotion blocker unrelated to this signature correction.

## Change and evidence

`src/Vec3_HorzAngle.cpp` defines the C-linkage helper as `short (const Vector3 *, const Vector3 *)`. Mip's local declaration used `const void *` for both arguments. It now matches the helper. Eight call sites use the actor's `mPos` overlay, typed shared `Vector3` values, or explicit `::Vector3` views over local three-word scratch/field triples.

The global qualification is necessary: `daMip_c` has a nested `Vector3` type, which differs from the shared global type in `types.h`, even where the local triple has the same three-word representation. The explicit views do not construct a `Vector3` or change storage. `math/CalcLib.h` cannot be added by itself because its other helper declarations conflict with retained local prototypes (`Vec3_Asr`, `Vec3_HorzDist`, and `Vec3_Dist`), so the local declaration stays with that reason.

## Verification

- `python tools/tubuild.py verify ov085/daMip_c --no-write --verdict`: 32/32 functions match; object isolation, relocation destinations, and emission order are clean. Exit 1 only for the existing unlicensed `.text` symbol `__sinit_d_a_mip.cpp` (0x3d0 bytes); no function mismatch.
- `python tools/check_decl_agreement.py --changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new declaration disagreements or local redeclarations of header declarations.
- `python tools/source_style.py --base a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7 --files src/game/actors/d_a_mip.cpp`: zero findings with clang-format 19.1.7.
- Full ROM build, relocation linkcheck, port references, and dead-reference scan remain to be run before publishing.

## Verifier handoff

Independently inspect the local and nested `Vector3` scopes and confirm each cast retains the exact three words used before the edit. Rerun the focused TU proof and full ROM gates on the pushed candidate. Keep the `__sinit` manifest blocker separate from this helper declaration change.
