# Correct the Grock LevelRng object declaration

## Evidence and change

`data_0209e650` is defined in `func_0201fec8.cpp` as `LevelRng`, whose sole
stored word is `int value`; its constructor seeds that word. `d_a_grock.cpp`
declared the same external object as `int`, producing a banked declaration
disagreement. Grock only passes the object's address to `RandomIntInternal`,
whose established ABI takes `int*`.

The Grock declaration now forward-declares `LevelRng` and declares the object
with that type. The call casts its address to `int*` at the existing ABI
boundary. This removes one declaration disagreement without exposing or
duplicating the defining class body.

## Verification

- `_ZN9daGrock_c8BehaviorEv`, `0x02112854`, size `0x2d8`: exact 2004/b56
  strict-relocation match.
- `prepush_linkcheck.py --files src/game/actors/d_a_grock.cpp`: all 13 code
  functions verified; generated `__sinit_d_a_grock.cpp` retains its existing
  BLIND-2 warning; zero blockers.
- Full `rombuild.py -j16 --no-rom`: PASS; 106/106 modules and 11,395/11,395
  functions exact, 468/468 source-owned data claims reproduced, and no ROM
  data differences.
- `check_decl_agreement.py --changed origin/main`: no new disagreements; the
  whole-tree count fell by one (2,082 to 2,081).
- Changed-line source style, dead references, and `git diff --check`: pass.

This fixes the Grock declaration only. The other declarations of
`data_0209e650` remain separate audit work; they include incompatible C/C++
linkage and scalar/array spellings and need caller-by-caller review.
