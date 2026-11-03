# Pachinko flyer shared layout

## Scope

`PachinkoFlyer_StateDescend` previously used a local `Obj`/`Ent` overlay whose
base began at `dScMgPachinko_c + 0x5bb4`, naming the shared flyer fields
`entries[].y` and `entries[].vy`. The class already declares this storage as
`dScMgPachinko_c::mFlyer[1]` (`dScMgPachinko_flyer`, beginning at `0x5bb0`).

The function now takes `dScMgPachinko_c*` and accesses `mFlyer[i].y` and
`mFlyer[i].vy`. Its old `volatile` view of `y` was retained locally as
`volatile int&` access expressions because byte comparison showed the ROM
reloads `y` after the store. The shared field declaration remains unchanged.

## Verification

- `PachinkoFlyer_StateDescend`, `0x020fb45c`, size `0x84`, strict-relocation
  match: 2004/b56 exact.
- `prepush_linkcheck.py --files src/minigames/d_s_mg_pachinko.cpp`: 77/77
  functions verified, 0 warnings, 0 blockers.
- Full `rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395
  functions exact, 468/468 source-owned data claims reproduced, 2,200 data
  symbols verified with 690 partial and 0 differing or unnamed.
- Changed-line `source_style.py` with pinned clang-format 19.1.7: passed.
- `git diff --check`: passed.

## Limitation

This recovers the shared class field spelling and removes an overlapping local
view. Byte identity proves emitted code for the pinned compiler, not that this
is uniquely the original EAD spelling.
