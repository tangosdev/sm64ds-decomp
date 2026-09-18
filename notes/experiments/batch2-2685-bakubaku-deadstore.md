# Dead-store in[2] pair is load-bearing (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_bakubaku.cpp:29-30` keeps
  dead-store in[2] pairs because they are load-bearing S8/S15.
- attempted_change (scratch-applied, then reverted):

```diff
-    in[2] = 0;
     in[2] = 0x5000;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_bakubaku.cpp \
  --func func_ov032_02111e24 --addr 0x02111e24 --size 0x178 \
  --version 2004/b56 --module ov032 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x178 vs 0x174
  (376 -> 372 bytes). Removing the dead store shrinks as claimed.
- log (variant):

```text
TARGET func_ov032_02111e24 @ 0x02111e24 size 0x178  bytes: 10402de918d04de20040a0e148219fe5980084e2051aa0e305a4fceb0400a0e141fdffeb010050e30a00001a2c119fe5010c84e22830a0e3b030c0e1012b84e2... (376 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x178 vs candidate 0x174

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
