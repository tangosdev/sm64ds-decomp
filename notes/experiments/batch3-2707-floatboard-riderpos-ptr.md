# FloatBoard rider position single-pointer loads (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daObjFloatBoard_c.cpp:97-100` reads the rider position
  through one `Vector3 *` because member loads cost an instruction and
  DIFF.
- attempted_change (scratch-applied, then reverted):

```diff
-            /* The rider's position as one address: spelling the three loads
-               as rider->mPosX/Y/Z instead of [riderPos+0/4/8] costs an
-               instruction and DIFFs. */
-            Vector3 *riderPos = (Vector3 *)&mRider->mPosX;
-            tpos.x = riderPos->x;
-            tpos.y = riderPos->y;
-            tpos.z = riderPos->z;
+            tpos.x = mRider->mPosX;
+            tpos.y = mRider->mPosY;
+            tpos.z = mRider->mPosZ;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daObjFloatBoard_c.cpp \
  --func _ZN17daObjFloatBoard_c8BehaviorEv --addr 0x020b5c4c --size 0x20c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x20c vs
  0x210 (524 -> 528 bytes). Member loads cost exactly one instruction.
- log (variant):

```text
TARGET _ZN17daObjFloatBoard_c8BehaviorEv @ 0x020b5c4c size 0x20c  bytes: 30402de90cd04de20050a0e1490f85e20316a0e3cf0dfeeb0500a0e191ffffeb000050e35a00000a3c0395e5052aa0e3000050e30a09a013001060120010a003... (524 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x20c vs candidate 0x210

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
