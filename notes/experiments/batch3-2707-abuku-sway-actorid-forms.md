# Abuku volatile sway reload and int-then-if (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_abuku.cpp:38-39,114-126` keeps the
  mSwayAngle volatile-u16 reload + >> 4 and the actorID == 0xbf
  int-then-if in matched form.
- attempted_change (scratch-applied, then reverted; both forms collapsed
  jointly, as the claim groups them):

```diff
     mSwayAngle += 0x400;
-    int v = *(volatile unsigned short*)(&mSwayAngle);
-    int x = v >> 4;
+    int x = mSwayAngle >> 4;
-            int b = (found->actorID == 0xbf);
-            if (b) {
+            if (found->actorID == 0xbf) {
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_abuku.cpp \
  --func _ZN12daObjAbuku_c8BehaviorEv --addr 0x020b33dc --size 0x13c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x13c vs
  0x130 (316 -> 304 bytes). Collapsing shrinks Behavior by 12 bytes.
- log (variant):

```text
TARGET _ZN12daObjAbuku_c8BehaviorEv @ 0x020b33dc size 0x13c  bytes: 10402de910d04de20040a0e1433f84e2f010d3e1010c84e210219fe5011b81e2b010c3e1bc00d0e108c194e5021ba0e34002a0e18000a0e1010080e28000a0e1... (316 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x13c vs candidate 0x130

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
