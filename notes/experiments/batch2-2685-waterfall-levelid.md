# LEVEL_ID local alias size-DIFF 999 (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_waterfall.cpp:21-22` keeps
  data_0209f2f8 because a local `signed char &LEVEL_ID` alias
  size-DIFFs 999 (rename belongs in symbols.txt).
- attempted_change (scratch-applied, then reverted):

```diff
 extern "C" s8 data_0209f2f8; /* LEVEL_ID */
+signed char &LEVEL_ID = data_0209f2f8;
@@
-    switch (data_0209f2f8) {
+    switch (LEVEL_ID) {
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_waterfall.cpp \
  --func _ZN16daObjWaterfall_c13InitResourcesEv --addr 0x020b6eac --size 0x3c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x3c vs 0x40
  (60 -> 64 bytes). A function-local alias MATCHes (optimizes to a direct
  load); the TU-local file-scope alias above reproduces the claimed 999.
- log (variant):

```text
TARGET _ZN16daObjWaterfall_c13InitResourcesEv @ 0x020b6eac size 0x3c  bytes: 2420a0e32c109fe5b42dc0e1d010d1e1160051e30300000a210051e3eb10a003b41dc001010000ea7110a0e3b41dc0e10100a0e31eff2fe1f8f20902
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x3c vs candidate 0x40

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
