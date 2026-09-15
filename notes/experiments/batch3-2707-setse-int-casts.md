# SetSE (int)-cast re-tests vs direct tests (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_set_se.cpp:77-82` keeps the (int) casts
  because each forces its boolean to materialize into a word, and direct
  tests come out six words short of the ROM.
- attempted_change (scratch-applied, then reverted):

```diff
-    if ((int)(data_0209f2d8 == 0) != 0
+    if (data_0209f2d8 == 0
         && (data_0209caa0[2] & 0x80) == 0
-        && (int)(data_0209fc48 != 0) == 0)
+        && data_0209fc48 == 0)
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_set_se.cpp \
  --func _ZN9daSetSE_c13InitResourcesEv --addr 0x020f1ac4 --size 0xd0 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xd0 vs
  0xb8 (208 -> 184 bytes). Direct tests come out exactly six words
  short, as claimed.
- log (variant):

```text
TARGET _ZN9daSetSE_c13InitResourcesEv @ 0x020f1ac4 size 0xd0  bytes: 10402de9ac109fe50040a0e1d000d1e1080050e30b00001a9c009fe50000d0e5010050e30400000a0800a0e30110a0e3ff86fceb000050e30200001a0000a0e3... (208 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xd0 vs candidate 0xb8

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
