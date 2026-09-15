# common.h first for shadow matrix copy (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daMip_c.cpp:75-78` keeps
  common.h first because of the shadow matrix copy.
- attempted_change (scratch-applied, then reverted):

```diff
-#include "common.h"
-#include "daMip_c.h"
+#include "daMip_c.h"
+#include "common.h"
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daMip_c.cpp \
  --func _ZN7daMip_c21UpdateMatrixAndShadowEv --addr 0x0212bcc8 --size 0xf4 \
  --version 2004/b56 --module ov085 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (no bytes differ).
  UpdateMatrixAndShadow copies via a local `Mtx43 { s32 m[12]; }`,
  not Matrix4x3, so the include order does not affect it.
- log (variant):

```text
TARGET _ZN7daMip_c21UpdateMatrixAndShadowEv @ 0x0212bcc8 size 0xf4  bytes: f0402de944d04de20040a0e138008de25c1084e20320a0e31445fceb38109de53c209de540309de5c0009fe59240fcebfc18d4e1fe28d4e1f039d4e1ac009fe5... (244 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
