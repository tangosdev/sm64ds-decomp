# FloatBoard mBobPhase two spellings vs one member access (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daObjFloatBoard_c.cpp:65-68` keeps two spellings of
  mBobPhase (store through `ctr`, reload through a separate `char *`
  base) because collapsing them lets mwcc reuse the loaded value
  instead of reloading it, which the ROM does not.
- attempted_change (scratch-applied, then reverted):

```diff
-            short *ctr = &mBobPhase;
-            short cval = *ctr;
-            char *st = (char *)this + 0x300;
-            *ctr = (short)(cval + 0xa00);
-            j = (*(unsigned short *)(st + 0x38) >> 4) * 2;
+            mBobPhase = (short)(mBobPhase + 0xa00);
+            j = ((unsigned short)mBobPhase >> 4) * 2;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daObjFloatBoard_c.cpp \
  --func _ZN17daObjFloatBoard_c8BehaviorEv --addr 0x020b5c4c --size 0x20c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x20c vs
  0x208 (524 -> 520 bytes). Collapsing drops one reload instruction.
- log (variant):

```text
TARGET _ZN17daObjFloatBoard_c8BehaviorEv @ 0x020b5c4c size 0x20c  bytes: 30402de90cd04de20050a0e1490f85e20316a0e3cf0dfeeb0500a0e191ffffeb000050e35a00000a3c0395e5052aa0e3000050e30a09a013001060120010a003... (524 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x20c vs candidate 0x208

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
