# dCcAc_c::Init method form size-DIFFs InitResources (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_pg_mthr.cpp:19-24` keeps
  dCcAc_c::Init mangled because the method form size-DIFFs
  (`Init 0x1ac->0x1c4`).
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x104000, 0x12c000, 0x4800004, 0x900000);
+    Fix12<int> cRadius; cRadius.val = 0x104000;
+    Fix12<int> cHeight; cHeight.val = 0x12c000;
+    mdCcAc_c.Init(this, cRadius, cHeight, 0x4800004, 0x900000);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_pg_mthr.cpp \
  --func _ZN10daPgMthr_c13InitResourcesEv --addr 0x021124d0 --size 0x1ac \
  --version 2004/b56 --module ov018 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x1ac vs 0x1c4
  (428 -> 452 bytes). Reproduces the claimed 0x1ac->0x1c4 exactly.
- log (variant):

```text
TARGET _ZN10daPgMthr_c13InitResourcesEv @ 0x021124d0 size 0x1ac  bytes: f0412de968d04de20040a0e188019fe55515fceb0120a0e30010a0e10230a0e1d40084e2b612fceb0060a0e36c519fe5060195e71015fceb016086e2020056e3... (428 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x1ac vs candidate 0x1c4

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
