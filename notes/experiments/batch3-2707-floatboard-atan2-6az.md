# FloatBoard cstd::atan2 mangled scalars vs Fix12 by value (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daObjFloatBoard_c.cpp:16-17` keeps cstd::atan2 a
  TU-local mangled scalar call (Fix12<int> by value, wall 6az).
- attempted_change (scratch-applied, then reverted):

```diff
 #include "daObjFloatBoard_c.h"
+#include "math/Fix12.h"
 #include "common.h"
-short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
+short _ZN4cstd5atan2E5Fix12IiES1_(Fix12<int> y, Fix12<int> x);
-            atanRes = _ZN4cstd5atan2E5Fix12IiES1_(mPosX - tpos.x, mPosZ - tpos.z);
+            Fix12<int> ay, ax;
+            ay.val = mPosX - tpos.x;
+            ax.val = mPosZ - tpos.z;
+            atanRes = _ZN4cstd5atan2E5Fix12IiES1_(ay, ax);
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daObjFloatBoard_c.cpp \
  --func _ZN17daObjFloatBoard_c8BehaviorEv --addr 0x020b5c4c --size 0x20c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x20c vs
  0x224 (524 -> 548 bytes). By-value Fix12 params home to the stack.
- log (variant):

```text
TARGET _ZN17daObjFloatBoard_c8BehaviorEv @ 0x020b5c4c size 0x20c  bytes: 30402de90cd04de20050a0e1490f85e20316a0e3cf0dfeeb0500a0e191ffffeb000050e35a00000a3c0395e5052aa0e3000050e30a09a013001060120010a003... (524 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x20c vs candidate 0x224

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
