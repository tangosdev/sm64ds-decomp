# Handoff: Shark Vec3_VertAngle const parameters

## Scope

- Base: current `main` at `8ed426be20b691b74e6bdea3a7221b20d8802f39`.
- Source: `src/game/actors/d_a_shark.cpp`, `daShark_c::Behavior`.
- Match the local `Vec3_VertAngle` declaration's parameter qualifiers to the definition in `src/Vec3_VertAngle.cpp`: both inputs are `const Vector3 *`. This does not change callers or object layout.
- The whole-tree symbol audit reported 14 parameter disagreements before this fix and 12 after it; other caller files are outside this slice.

## Verification

- Strict relocation-aware match for `_ZN9daShark_c8BehaviorEv` at `ov090:0x021339d8/0x1c0`: PASS.
- `prepush_linkcheck.py --files src/game/actors/d_a_shark.cpp`: 14 checked, 13 verified, 1 warning (`__sinit_d_a_shark.cpp` BLIND-4), 0 blockers.
- `check_decl_agreement.py --symbol Vec3_VertAngle`: this file now agrees with the definition; 12 other parameter disagreements remain in unrelated TUs.
- `check_decl_agreement.py --changed 8ed426be20b691b74e6bdea3a7221b20d8802f39`: no new disagreements or local redeclarations.
- Full `rombuild.py -j16 --no-rom` must be recorded from the exact committed candidate before publishing.

## Review request

Confirm the const-qualified definition and rerun the strict Behavior match plus full ROM build against the exact PR head. Keep the other call-site declarations as separate reviewed slices.
