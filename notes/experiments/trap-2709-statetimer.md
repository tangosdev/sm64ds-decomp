# Behavior mStateTimer dual spelling vs unified reset (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp:43-45` (code at 230-238)
  spells mStateTimer twice on purpose (this+0x14c, then this+0x100 plus 0x4c)
  because unifying them shares one address computation and comes out one
  instruction short (0x88, not 0x8c).
- disposition: fixed (the disproved constraint was removed from the source;
  no compiler constraint is retained here).
- attempted_change (measured as a scratch, since ADOPTED on the branch by
  1477adc72 -- the source below is the shipped spelling, not a reverted probe):

```diff
     u16 *ctr = &mStateTimer;
     *ctr = *ctr + 1;
     if (before != mState) {
-        u16 *base = (u16 *)((char *)this + 0x100);
-        base[0x4c / 2] = 0;
+        mStateTimer = 0;
     }
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp \
  --func _ZN11daTrsTrap_c8BehaviorEv --addr 0x0211ce74 --size 0x8c \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (confirmed on two consecutive runs
  with the scratch applied). The compiler keeps the two address computations
  apart even when both sites name mStateTimer, so the dual spelling is not
  load-bearing, and the "one instruction short (0x88, not 0x8c)" claim is false.
- follow-through: 1477adc72 adopted the unified spelling and deleted the
  claim from the source comment, so this entry now describes the SHIPPED
  form. Reverified at head 1477adc72 (plus the comment-only correction in
  this commit): `linkcheck.py --name _ZN11daTrsTrap_c8BehaviorEv --module
  ov063 --addr 0x0211ce74 --size 0x8c` returns VERIFIED, diffs [], blind 0.
- log (variant):

```text
TARGET _ZN11daTrsTrap_c8BehaviorEv @ 0x0211ce74 size 0x8c  bytes: 30402de904d04de20050a0e1400195e570109fe55041d5e5803181e0041093e5c10085e0011011e20020901500109315011092170010930531ff2fe1531f85e2... (140 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED -- and the disproved spelling is no longer in the source.
