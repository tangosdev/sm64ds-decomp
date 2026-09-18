# common.h first for 02112094 matrix copy (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daMky_c.cpp:18-20` keeps
  common.h first because of the 02112094 matrix copy.
- attempted_change (scratch-applied, then reverted):

```diff
-#include "common.h"
-#include "types.h"
-#include "daMky_c.h"
+#include "types.h"
+#include "daMky_c.h"
+#include "common.h"
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daMky_c.cpp \
  --func func_ov030_02112094 --addr 0x02112094 --size 0x21c \
  --version 2004/b56 --module ov030 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (no bytes differ).
  02112094 copies via a local `M4x3 { int w[12]; }`, not Matrix4x3,
  so the include order does not affect it.
- log (variant):

```text
TARGET func_ov030_02112094 @ 0x02112094 size 0x21c  bytes: 70402de928d04de20050a0e1b00095e5010c10e20100a0130000a003000050e32100000aa80395e5000050e31e00000ac81090e5000051e31b00000a0040a0e3... (540 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
