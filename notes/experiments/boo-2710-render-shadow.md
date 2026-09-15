# Render flags local shadow vs typed member reads (BOO-2710-01, PR #2710)

- tested_commit: 8f5abbc11 (cleanup-datrs at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrs_c/d_a_trs_cleanup_render.cpp:53-58` reads the
  flags halfword through the typed mFlags_5d4 member because a local shadow
  compiles to a literal-pool address load where the ROM has add+ldrh.
- attempted_change (scratch-applied, then reverted):

```diff
-        if (!mFlags_5d4.b3)
-            return 1;
-        if (mFlags_5d4.b1) {
+        unsigned short fl = *(unsigned short *)((char *)this + 0x5d4);
+        if (!(fl & 8))
+            return 1;
+        if (fl & 2) {
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrs_c/d_a_trs_cleanup_render.cpp \
  --func _ZN7daTrs_c6RenderEv --addr 0x0211af70 --size 0x108 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x108 vs 0x100
  (264 -> 256 bytes). The local shadow breaks the match.
- log (variant):

```text
TARGET _ZN7daTrs_c6RenderEv @ 0x0211af70 size 0x108  bytes: 10402de90040a0e1b00094e5010710e20100a0130000a003000050e30100a0131040bd181eff2f11050c84e2b40dd0e1001ea0e1a11fb0e10100a0031040bd08... (264 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x108 vs candidate 0x100

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
