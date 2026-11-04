# Handoff: Demo Vec3_Add declaration

## Scope

- Base: current `main` at `8ed426be20b691b74e6bdea3a7221b20d8802f39`.
- Source: `src/game/actors/d_a_demo.cpp`, `daDemo_c::func_ov002_020f7538`.
- Change: give the local `Vec3_Add` declaration its definition's `Vector3 *, const Vector3 *, const Vector3 *` signature. Keep the three-word `Vec3` locals as-is and cast only where they cross that C ABI; this avoids adding `Vector3` construction/destruction to the local lifetimes.
- No shared header, object layout, or path changed.

## Verification

- `tubuild verify ov002/daDemo_c --no-write --verdict`: all 148 functions match; object isolation and relocation destinations are clean; emission order is correct. It refuses TU promotion because the existing manifest omits 15 sections/symbols, including `__sinit_d_a_demo.cpp` (0xc18) and RTTI/typeinfo. This is not a source-function mismatch.
- Strict relocation-aware match for `_ZN8daDemo_c19func_ov002_020f7538EPhi` at `ov002:0x020f7538/0x248`: pass.
- `prepush_linkcheck.py --files src/game/actors/d_a_demo.cpp`: 149 checked, 147 verified, 2 existing warnings (`param_c::C2` BLIND-1 and `__sinit_d_a_demo.cpp` BLIND-37), 0 blocking.
- `check_decl_agreement.py --changed 8ed426be20b691b74e6bdea3a7221b20d8802f39`: no new disagreements or local redeclarations.
- `check_dead_references.py`: no new dead references or broken Markdown links.

## Review request

Please independently inspect the raw-storage casts against the `Vec3` and `Vector3` definitions and rerun the focused byte/relocation checks on the exact PR head. A full `rombuild.py -j16 --no-rom` result will be recorded in the PR body after it completes on the committed candidate.
