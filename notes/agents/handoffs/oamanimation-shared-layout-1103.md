# OamAnimation shared layout handoff

Task: `issue-4570-oamanimation-shared-layout-1103`  
Base: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`  
Queue producer session: `codex-oam-1103-a`

## Change

Made `include/OamAnimation.h` usable from C and C++ and removed the two
file-local C layouts from `func_0202044c.c` and `func_02020768.c`. The shared
header now includes the 8-byte `OamAnimFrame` record and the complete 0x2c-byte
`OamAnimation` layout, with constructors visible only in C++. Its frame table
is typed as `OamAnimFrame *`; `OAM.cpp` now casts the table address to that
type when it initializes the field.

## Verification so far

- `func_0202044c` (`arm9`, `0x0202044c`, `0x40`), `func_02020768`
  (`arm9`, `0x02020768`, `0xb8`), and `func_02020820` (`arm9`, `0x02020820`,
  `0x64`): focused matches passed; strict linkchecks returned `VERIFIED`,
  `diffs: []`, `blind: 0` for each.
- `python tools/rombuild.py -j16 --no-rom`: 11,395 functions and 468
  source-owned data claims reproduce; 106/106 modules exact; zero mismatches.
- Declaration agreement has no new branch disagreements. Dead references,
  TU conflicts, and port references pass.
- `python tools/prepush_linkcheck.py --range origin/main..HEAD`: 119 functions
  checked, 117 verified, 2 nonblocking `BLIND` warnings on existing
  `__sinit_d_s_entry*` functions, 0 blocking results. The seven-source header
  fanout compiled and linked.

## Remaining review

Independently inspect the shared C/C++ declaration and typed frame pointer,
then rerun focused linkchecks and the full build on the queued candidate SHA.
Keep the PR draft until source review and hosted byte validation finish.
