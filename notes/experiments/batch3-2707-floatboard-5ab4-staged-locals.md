# FloatBoard 020b5ab4 staged locals vs direct stores (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daObjFloatBoard_c.cpp:202-208` stages the raycast
  origin through locals because direct member stores schedule
  differently.
- attempted_change (scratch-applied, then reverted):

```diff
-    /* Staged through locals: direct member stores schedule differently. */
-    int vx = board->mPosX;
-    int vz = board->mPosZ;
-    int vy = board->mPosY + 0x64000;
-    vec.x = vx;
-    vec.y = vy;
-    vec.z = vz;
+    vec.x = board->mPosX;
+    vec.z = board->mPosZ;
+    vec.y = board->mPosY + 0x64000;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daObjFloatBoard_c.cpp \
  --func func_ov002_020b5ab4 --addr 0x020b5ab4 --size 0xe4 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 8 word(s) differ at the same size
  0xe4 (228 bytes). Direct stores reschedule the loads.
- log (variant):

```text
TARGET func_ov002_020b5ab4 @ 0x020b5ab4 size 0xe4  bytes: 10402de960d04de2cc109fe50040a0e1d000d1e1150050e30a00001a441394e5b8009fe5241384e5000090e560d08de2010050e1240384c50100a0c30000a0d3... (228 bytes)
  2004/b56: 8 word(s) differ

--- closest: 2004/b56 (8 differ) ---
```

- verdict: CONSTRAINT SUPPORTED
