# Handoff: collected star ID scalar declaration

## Scope

- Base: current `main` at `8ed426be20b691b74e6bdea3a7221b20d8802f39`.
- Source: `src/game/stages/d_s_stage_methods.cpp`, `StartFile`.
- `Message.cpp` and `d_a_star.cpp` both declare and use `gCollectedStarId` as a scalar `u8`; the stage TU had declared it as `u8[]` and initialized `[0]`. Make the stage declaration and initialization scalar as well. The symbol is a single collected-star ID byte at `0x0209f228`.

## Verification

- `check_decl_agreement.py --symbol gCollectedStarId`: 0 disagreements after the fix (before: scalar vs array disagreement).
- `check_decl_agreement.py --changed 8ed426be20b691b74e6bdea3a7221b20d8802f39`: no new local redeclarations or declaration disagreements.
- Strict relocation-aware match for `StartFile` at `arm9:0x0202ae88/0x80`: PASS.
- `prepush_linkcheck.py --files src/game/stages/d_s_stage_methods.cpp`: 124 checked, all 124 verified, 0 warnings, 0 blockers.
- Full `rombuild.py -j16 --no-rom` must be recorded from the exact committed candidate before publishing.

## Review request

Confirm the single-byte interpretation from the scalar reads/writes in `Message.cpp` and `d_a_star.cpp`; independently rerun the `StartFile` strict match and full ROM build on the exact PR head.
