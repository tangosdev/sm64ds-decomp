# CleanupResources file-table reloads after each Release (DSN-2708-01, PR #2708)

- tested_commit: 7027888ce (cleanup-dadsnbase at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daDsnBase_c.cpp:142-146` reloads
  mFileTable after each Release (a Release clobbers); the three loads are
  the ROM's. Leftover `src/actors/daDsnBase_c.cpp:40-41`.
- attempted_change (scratch-applied, then reverted):

```diff
-    DsnBaseFileTable *files;
-    if (mMeshCollider.IsEnabled())
-        mMeshCollider.Disable();
-    files = (DsnBaseFileTable *)mFileTable;
-    files->model->Release();
-    files = (DsnBaseFileTable *)mFileTable;
-    files->collision->Release();
-    files = (DsnBaseFileTable *)mFileTable;
-    if (files->texAnim != 0)
-        files->texAnim->Release();
+    if (mMeshCollider.IsEnabled())
+        mMeshCollider.Disable();
+    DsnBaseFileTable *files = (DsnBaseFileTable *)mFileTable;
+    files->model->Release();
+    files->collision->Release();
+    if (files->texAnim != 0)
+        files->texAnim->Release();
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func _ZN11daDsnBase_c16CleanupResourcesEv --addr 0x021331b8 --size 0x58 \
  --version 2004/b56 --module ov091 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x58 vs 0x50
  (88 -> 80 bytes, -0x8, two loads gone). Strict-relocs on for both.
- log (variant):

```text
TARGET _ZN11daDsnBase_c16CleanupResourcesEv @ 0x021331b8 size 0x58  bytes: 10402de90040a0e1490f84e28418fceb000050e30100000a490f84e2d917fceb200394e5000090e55f92fbeb200394e5040090e55c92fbeb200394e50c0090e5... (88 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x58 vs candidate 0x50

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
