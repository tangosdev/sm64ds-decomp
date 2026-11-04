# Handoff: Butterfly Vec3_HorzAngle parameter types

## Identity and resumption

- Task: match `daBtfly_c`'s local `Vec3_HorzAngle` declaration to the helper definition.
- Base: latest `main` at `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Candidate: isolated source correction; independent source review remains outstanding.
- The existing `__sinit_d_a_btfly.cpp` section is not in the TU manifest and remains a promotion blocker unrelated to this signature correction.

## Change and evidence

`src/Vec3_HorzAngle.cpp` defines the C-linkage helper as `short (const Vector3 *, const Vector3 *)`. `src/game/actors/d_a_btfly.cpp` previously declared its inputs as `const void *`. The local declaration now matches the definition, and its four callers provide typed views of the actor position, home-position triples, and the existing `Vector3 v`.

`math/CalcLib.h` already spells the correct declaration, but including it in this TU would conflict with its retained local `Vec3_Asr` and `Vec3_HorzLen` prototypes. The local declaration is therefore retained with an explicit reason. No storage or control flow changed.

## Verification

- `python tools/tubuild.py verify ov100/daBtfly_c --no-write --verdict`: 16/16 functions match; object isolation, relocation destinations, and emission order are clean. Exit 1 only for the pre-existing unlicensed `.text` symbol `__sinit_d_a_btfly.cpp` (0x12c bytes); no function mismatch.
- `python tools/check_decl_agreement.py --changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new declaration disagreements or local redeclarations of header declarations.
- `python tools/source_style.py --base a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7 --files src/game/actors/d_a_btfly.cpp --patterns-only`: zero pattern findings. Full formatting requires pinned clang-format 19.1.7, unavailable in this environment.
- Full ROM build and relocation linkcheck are pending; record their results here before publishing.

## Verifier handoff

Independently check that the explicit `Vector3` views point to the same three-word sequences the old `void *` calls passed, and rerun the focused and full ROM gates on the pushed head. Keep the `__sinit` manifest blocker separate from this helper declaration change.
