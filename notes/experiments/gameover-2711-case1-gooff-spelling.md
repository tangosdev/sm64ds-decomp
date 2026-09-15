# case1 (char *)o + GO_OFF spelling vs member index (GAMEOVER-2711-01, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dScGameOver_c.cpp:455-457,505-510` keeps case1's
  `(char *)o + GO_OFF` spelling because the member-index form perturbs the
  tail.
- attempted_change (scratch-applied, then reverted):

```diff
 case1:
-    if (*(unsigned short *)((char *)o + GO_OFF(mGlyphDelay)) != 0) {
-        unsigned short *p = (unsigned short *)((char *)o + GO_OFF(mGlyphDelay));
-        *p = (unsigned short)(*p - 1);
+    if (o->mGlyphDelay[0] != 0) {
+        o->mGlyphDelay[0] = (unsigned short)(o->mGlyphDelay[0] - 1);
         return;
     }
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dScGameOver_c.cpp \
  --func func_ov003_020b060c --addr 0x020b060c --size 0x124 \
  --version 2004/b56 --module ov003 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x124 vs 0x11c
  (292 -> 284 bytes). The member-index form perturbs as claimed.
- log (variant):

```text
TARGET func_ov003_020b060c @ 0x020b060c size 0x124  bytes: 70402de99410d0e5000051e30300000a010051e32b00000a7040bde81eff2fe100e0a0e30ac0a0e3e8209fe50140a0e38e3080e0b017d3e18e50a0e1000051e3... (292 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x124 vs candidate 0x11c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
