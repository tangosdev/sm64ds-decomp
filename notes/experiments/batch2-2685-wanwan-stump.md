# Stump.h include changes matrix copies (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_wanwan.cpp:20-23` keeps
  stump +0x320 raw because including Stump.h pulls
  dBgActor_c/common.h and would change this TU's matrix copies.
- attempted_change (scratch-applied, then reverted):

```diff
 #include "common.h"
 #include "daWanwan_c.h"
+#include "Stump.h"
 #include "decl_common.h"
@@
-    *(unsigned char *)((unsigned char *)spawned + 0x320) = (unsigned char)one;
+    ((Stump *)spawned)->mBusy = (unsigned char)one;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_wanwan.cpp \
  --func func_ov014_02112788 --addr 0x02112788 --size 0x1c4 \
  --version 2004/b56 --module ov014 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (no bytes differ).
  InitResources with mBusy also MATCHes. Stump.h first also MATCHes.
  dBgActor_c.h includes common.h before Model.h, so flat still wins.
- log (variant):

```text
TARGET func_ov014_02112788 @ 0x02112788 size 0x1c4  bytes: f04f2de93cd04de20090a0e1fc18d9e1fe28d9e1f039d9e15b0f89e2bca5fceb5c0099e5c001a0e1900189e5600099e5c001a0e1940189e5640099e5c001a0e1... (452 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
