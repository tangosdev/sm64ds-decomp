# volatile v[2] store vs plain store in StateMove (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:456-462` keeps the volatile
  round-trip because a plain `v[2] = 0;` rewrites the whole 0x3c4-byte
  member.
- attempted_change (scratch-applied, then reverted):

```diff
-                    /* The volatile round-trip is load-bearing.  MEASURED:
-                       writing a plain `v[2] = 0;` rewrites the whole 0x3c4-byte
-                       member, with 4 relocation destinations wrong.  The stack
-                       demotion of v[] and the NewSimple() argument load order
-                       below are a reading of that diff, not a separate
-                       measurement. */
-                    *(volatile int *)&v[2] = 0;
+                    v[2] = 0;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func _ZN16dMgJump3DMario_c9StateMoveEv --addr 0x020c7c68 --size 0x3c4 \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x3c4 vs 0x3a8
  (964 -> 936 bytes). The plain store rewrites the member as claimed.
- log (variant):

```text
TARGET _ZN16dMgJump3DMario_c9StateMoveEv @ 0x020c7c68 size 0x3c4  bytes: 10402de910d04de20040a0e1b011d4e1010051e30e00001a0010a0e378039fe5b011c4e1000090e5240084e5440094e5000050e3441084150100a00344008405... (964 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x3c4 vs candidate 0x3a8

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
