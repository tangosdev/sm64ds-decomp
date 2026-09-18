# Tree dCcPos_c::Init scalar wrapper vs header member form (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_tree.cpp:17-19` keeps the TU-local scalar-int
  wrapper (`lines 83-85`) because the header member form
  (`include/dCcPos_c.h:51`, two by-value Fix12<int> params) homes them to
  the stack and size-DIFFs (wall 6az).
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
-        &node->clsn, *(const Vector3 *)&mPosX, kClsnRadius, kClsnHeight,
-        kClsnFlags, 0);
+    Fix12<int> radius;
+    Fix12<int> height;
+    radius.val = kClsnRadius;
+    height.val = kClsnHeight;
+    node->clsn.Init(
+        *(const Vector3 *)&mPosX, radius, height, kClsnFlags, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_tree.cpp \
  --func _ZN8daTree_c13InitResourcesEv --addr 0x020ec22c --size 0x100 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x100 vs
  0x118 (256 -> 280 bytes). The header member form grows InitResources
  by 24 bytes.
- log (variant):

```text
TARGET _ZN8daTree_c13InitResourcesEv @ 0x020ec22c size 0x100  bytes: 70402de908d04de20060a0e1080096e5d8109fe52002a0e1073000e2040053e30430a0a3030191e7035181e0000050e0... (256 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x100 vs candidate 0x118

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
