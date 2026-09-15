# Nested Matrix.h spelling scalarizes 02111d28 copy (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_pg_mthr.cpp:25-26` keeps
  common.h first because 02111d28 copies Matrix4x3 as twelve
  uniform words and the nested Matrix.h spelling scalarizes.
- attempted_change (scratch-applied, then reverted):

```diff
-#include "common.h"
-#include "daPgMthr_c.h"
+#include "daPgMthr_c.h"
+#include "common.h"
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_pg_mthr.cpp \
  --func func_ov018_02111d28 --addr 0x02111d28 --size 0x100 \
  --version 2004/b56 --module ov018 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x100 vs 0x13c
  (256 -> 316 bytes). Nested spelling scalarizes as claimed.
- log (variant):

```text
TARGET func_ov018_02111d28 @ 0x02111d28 size 0x100  bytes: 30402de90cd04de20040a0e1fe18d4e1f00084e20aa8fceb5c0094e50528a0e3c001a0e1140184e5601094e50400a0e1c111a0e1181184e5643094e5531f84e2... (256 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x100 vs candidate 0x13c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
