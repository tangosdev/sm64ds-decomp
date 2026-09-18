# int t temp vs folded comparison in 020c87d0 (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:776-778` keeps the `int t` temp
  because folding the comparison into the `if` rewrites the whole
  0x16c-byte function.
- attempted_change (scratch-applied, then reverted):

```diff
     dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;
 
-    /* `t` is load-bearing, not a leftover: folding the comparison into the
-       `if` below rewrites the whole 0x16c-byte function. */
-    int t;
-
     if (func_020179b4(&data_ov006_02140450, &o->mModelAnim, 1) == 0)
```

```diff
-    t = data_0209f5c0->actorID == 0x175;
-    if (t != 0) {
+    if (data_0209f5c0->actorID == 0x175) {
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func func_ov006_020c87d0 --addr 0x020c87d0 --size 0x16c \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x16c vs 0x160
  (364 -> 352 bytes). Folding rewrites the member as claimed.
- log (variant):

```text
TARGET func_ov006_020c87d0 @ 0x020c87d0 size 0x16c  bytes: 10402de908d04de20040a0e114019fe54c1084e20120a0e3713cfdeb000050e308d08d020000a0031040bd081eff2f01f4009fe5503cfdebf0109fe5000081e5... (364 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x16c vs candidate 0x160

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
