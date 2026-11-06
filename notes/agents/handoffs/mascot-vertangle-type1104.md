# Handoff: Mascot Vec3_VertAngle declaration

## Scope

- Base: current `main` at `8ed426be20b691b74e6bdea3a7221b20d8802f39`.
- Source: `src/minigames/unit020bfec0.cpp`, `Mascot_Draw`.
- Change the local `Vec3_VertAngle` declaration from two `const void *` parameters to the implementation's `const Vector3 *` parameters. Both call arguments are `Vector3` members (`Mascot::mPos` and `Mascot::mLookTarget`); no casts or layout changes are needed.
- `Vec3_VertAngle` has no shared header declaration, so this local declaration remains appropriate.
- `Vec3_HorzAngle` is a separate declaration cleanup: `math/CalcLib.h` owns its shared prototype, while this TU has neighboring legacy `void *` declarations for other CalcLib helpers. Reconcile those together before adding that header here.

## Verification

- Strict relocation-aware match for `Mascot_Draw` at `ov006:0x020c201c/0x128`: PASS.
- `prepush_linkcheck.py --files src/minigames/unit020bfec0.cpp`: 93 checked, 93 verified, 0 warnings, 0 blockers.
- `check_decl_agreement.py --changed 8ed426be20b691b74e6bdea3a7221b20d8802f39`: no new declaration disagreements or local redeclarations.
- The full ROM result is recorded in the PR body for the exact candidate; rerun it on the exact head when independently verifying.

## Review request

Confirm the `Mascot` member types and the `Vec3_VertAngle` definition, then independently rerun the strict `Mascot_Draw` match and full ROM build on the exact PR head. Keep the adjacent horizontal-angle/header migration separate.
