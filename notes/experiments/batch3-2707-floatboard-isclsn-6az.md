# FloatBoard IsClsnInRangeOnScreen mangled scalars vs Fix12 by value (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daObjFloatBoard_c.cpp:16-17` keeps
  dBgActor_c::IsClsnInRangeOnScreen a TU-local mangled scalar call
  (Fix12<int> by value, wall 6az).
- attempted_change (scratch-applied, then reverted):

```diff
 #include "daObjFloatBoard_c.h"
+#include "math/Fix12.h"
 #include "common.h"
-int   _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(dBgActor_c *self, int x, int z);
+int   _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(dBgActor_c *self, Fix12<int> x, Fix12<int> z);
-    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0) != 0)
+    Fix12<int> ix, iz;
+    ix.val = 0;
+    iz.val = 0;
+    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, ix, iz) != 0)
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daObjFloatBoard_c.cpp \
  --func _ZN17daObjFloatBoard_c8BehaviorEv --addr 0x020b5c4c --size 0x20c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x20c vs
  0x220 (524 -> 544 bytes). By-value Fix12 params home to the stack.
- log (variant):

```text
TARGET _ZN17daObjFloatBoard_c8BehaviorEv @ 0x020b5c4c size 0x20c  bytes: 30402de90cd04de20050a0e1490f85e20316a0e3cf0dfeeb0500a0e191ffffeb000050e35a00000a3c0395e5052aa0e3000050e30a09a013001060120010a003... (524 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x20c vs candidate 0x220

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
