# SetSE volatile param1 RMW vs plain member RMW (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_set_se.cpp:72-75` keeps the volatile
  param1 RMW because `param1 &= 0xf` materializes an extra base pointer
  while the ROM loads and stores [this + 0x08] directly.
- attempted_change (scratch-applied, then reverted):

```diff
-    /* Volatile keeps CW from materializing an extra base pointer for this
-       inherited-field RMW; the ROM loads and stores [this + 0x08] directly. */
-    u32 soundID = *(volatile u32 *)((char *)this + 0x8);
-    *(volatile u32 *)((char *)this + 0x8) = soundID & 0xf;
+    param1 &= 0xf;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_set_se.cpp \
  --func _ZN9daSetSE_c13InitResourcesEv --addr 0x020f1ac4 --size 0xd0 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xd0 vs
  0xd4 (208 -> 212 bytes). The plain member RMW costs one extra word.
- log (variant):

```text
TARGET _ZN9daSetSE_c13InitResourcesEv @ 0x020f1ac4 size 0xd0  bytes: 10402de9ac109fe50040a0e1d000d1e1080050e30b00001a9c009fe50000d0e5010050e30400000a0800a0e30110a0e3ff86fceb000050e30200001a0000a0e3... (208 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xd0 vs candidate 0xd4

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
