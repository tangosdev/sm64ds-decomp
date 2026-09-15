# e-loop skeleton vs member indexing in 020b060c (GAMEOVER-2711-01, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dScGameOver_c.cpp:449-457` keeps the `e = base + (i << 1)`
  loop skeleton because the member-index form (o->mGlyphX[i] /
  o->mGlyphDelay[i]) changes codegen.
- attempted_change (scratch-applied, then reverted):

```diff
     do {
-        char *e = (char *)o + (i << 1);
-        if (*(unsigned short *)(e + GO_OFF(mGlyphDelay)) != 0) {
-            *(unsigned short *)((long long)(int)(e + GO_OFF(mGlyphDelay))) =
-                (unsigned short)(*(unsigned short *)((long long)(int)(e + GO_OFF(mGlyphDelay))) - 1);
+        if (o->mGlyphDelay[i] != 0) {
+            o->mGlyphDelay[i] = (unsigned short)(o->mGlyphDelay[i] - 1);
         } else {
             short tgt = data_ov003_020b1774[i];
-            if (*(short *)(e + GO_OFF(mGlyphX)) != tgt) {
+            if (o->mGlyphX[i] != tgt) {
                 if (i < 4) {
-                    short *q = (short *)(e + GO_OFF(mGlyphX));
-                    *q = (short)(*q + 0xc);
-                    if (*(short *)(e + GO_OFF(mGlyphX)) >= tgt) {
-                        *(short *)(e + GO_OFF(mGlyphX)) = tgt;
-                        *(unsigned short *)(e + GO_OFF(mGlyphDelay)) = (unsigned short)ip;
+                    o->mGlyphX[i] = (short)(o->mGlyphX[i] + 0xc);
+                    if (o->mGlyphX[i] >= tgt) {
+                        o->mGlyphX[i] = tgt;
+                        o->mGlyphDelay[i] = (unsigned short)ip;
                     }
                 } else {
-                    short *q = (short *)(e + GO_OFF(mGlyphX));
-                    *q = (short)(*q - 0xc);
-                    if (*(short *)(e + GO_OFF(mGlyphX)) <= tgt) {
-                        *(short *)(e + GO_OFF(mGlyphX)) = tgt;
-                        *(unsigned short *)(e + GO_OFF(mGlyphDelay)) = (unsigned short)ip;
+                    o->mGlyphX[i] = (short)(o->mGlyphX[i] - 0xc);
+                    if (o->mGlyphX[i] <= tgt) {
+                        o->mGlyphX[i] = tgt;
+                        o->mGlyphDelay[i] = (unsigned short)ip;
                         o->mIntroPhase = 1;
                     }
                 }
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dScGameOver_c.cpp \
  --func func_ov003_020b060c --addr 0x020b060c --size 0x124 \
  --version 2004/b56 --module ov003 --strict-relocs --brief
```

- result: baseline MATCH; variant 69 word(s) differ, same size 0x124 (whole
  function rescheduled: different frame shape and register allocation
  throughout). A second, surgical variant keeping the q re-reads, launders
  and unsigned loads while changing only e+GO_OFF addressing to member
  indexing also breaks the match (size 0x124 vs 0x128), isolating the
  address computation itself as load-bearing. The comment's in-place "3
  words" number is not reproduced by either pinned spelling; the constraint
  itself is confirmed.
- log (variant):

```text
TARGET func_ov003_020b060c @ 0x020b060c size 0x124  bytes: 70402de99410d0e5000051e30300000a010051e32b00000a7040bde81eff2fe100e0a0e30ac0a0e3e8209fe50140a0e38e3080e0b017d3e18e50a0e1000051e3... (292 bytes)
  2004/b56: 69 word(s) differ

--- closest: 2004/b56 (69 differ) ---
   [69 mismatch lines; first and last shown, middle elided]
   +0x00 | 70402de9 push {r4, r5, r6, lr}    | 30402de9 push {r4, r5, lr}        | <<< MISMATCH
   +0x04 | 9410d0e5 ldrb r1, [r0, #0x94]     | 04d04de2 sub sp, sp, #4           | <<< MISMATCH
   ...
   +0x10c | 0020c0e5 strb r2, [r0]            | 04d08de2 add sp, sp, #4           | <<< MISMATCH
   +0x110 | 7040bde8 pop {r4, r5, r6, lr}     | 3040bde8 pop {r4, r5, lr}         | <<< MISMATCH

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
