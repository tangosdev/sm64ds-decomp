# Dispatch table bound to the real daTrsTrap_c PMF (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp:38-42` -- the dispatch
  table is a pointer-to-member of daTrsTrap_c itself
  (`typedef void (daTrsTrap_c::*PMF)();`, line 64), not an opaque
  forward-declared class; under 2004/b56 the real class gives byte-identical
  bodies and relocation records. ALREADY FIXED on this branch, so there is no
  cleaner spelling to attempt -- this entry records the baseline as green.
- attempted_change: none (verification of current branch state only).

```diff
 (no change -- branch already carries the real-class PMF)
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp \
  --func _ZN11daTrsTrap_c8BehaviorEv --addr 0x0211ce74 --size 0x8c \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH with the real-class PMF dispatch
  (`(this->*data_ov063_0211ef38[idx])();`, line 229).
- log (baseline):

```text
TARGET _ZN11daTrsTrap_c8BehaviorEv @ 0x0211ce74 size 0x8c  bytes: 30402de904d04de20050a0e1400195e570109fe55041d5e5803181e0041093e5c10085e0011011e20020901500109315011092170010930531ff2fe1531f85e2... (140 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: VERIFIED-CURRENT
