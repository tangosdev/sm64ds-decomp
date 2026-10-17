# Nested Matrix.h spelling scalarizes 12-word copies (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_bakubaku.cpp:20-21` keeps
  common.h first because the nested Matrix.h spelling scalarizes
  the 12-word mat4x3 / mShadowMat copies.
- attempted_change (scratch-applied, then reverted):

```diff
-#include "common.h"
-#include "daBakubaku_c.h"
+#include "daBakubaku_c.h"
+#include "common.h"
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_bakubaku.cpp \
  --func func_ov032_02112044 --addr 0x02112044 --size 0xe0 \
  --version 2004/b56 --module ov032 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xe0 vs 0x114
  (224 -> 276 bytes). Nested spelling scalarizes as claimed.
- log (variant):

```text
TARGET func_ov032_02112044 @ 0x02112044 size 0xe0  bytes: f0402de914d04de20040a0e108008de25c1084e20320a0e335acfceb08109de50c209de510309de5ac009fe5b3a7fcebfc18d4e1fe28d4e1f039d4e198009fe5... (224 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xe0 vs candidate 0x114

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
