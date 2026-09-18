# Tree Clipper scalar wrapper vs header member form (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_tree.cpp:20` keeps the TU-local scalar-int
  wrapper (`lines 86-87`) for Clipper::Func_02015560 because of the same
  6az wall on its Fix12<int> scale (`include/Clipper.h:74`).
- attempted_change (scratch-applied, then reverted):

```diff
 #include "daTree_c.h"
 #include "dCcPos_c.h"
+#include "Clipper.h"
             int out[3];
-            int dist =
-                _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
-                    data_0209f43c, &data_0209b3ec, &node->pos, kCanopyLift,
-                    (Vector3 *)out);
+            Fix12<int> fscale;
+            fscale.val = kCanopyLift;
+            int dist = ((Clipper *)data_0209f43c)->Func_02015560(
+                data_0209b3ec, node->pos, fscale, *(Vector3 *)out);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_tree.cpp \
  --func _ZN8daTree_c6RenderEv --addr 0x020ec0a4 --size 0x134 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x134 vs
  0x14c (308 -> 332 bytes). The member form grows Render by 24 bytes.
- log (variant):

```text
TARGET _ZN8daTree_c6RenderEv @ 0x020ec0a4 size 0x134  bytes: f04f2de91cd04de210119fe510219fe508108de5001092e5d4a080e20000a0e304108de50c008de510b08de20050a0e10040a0e11f60a0e31e7aa0e304009de5... (308 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x134 vs candidate 0x14c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
