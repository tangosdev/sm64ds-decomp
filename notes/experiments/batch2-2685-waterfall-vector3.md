# Vector3 overlay of &mPosX size-DIFFs 999 (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_waterfall.cpp:23-24` keeps
  three scalar mPosX/Y/Z into New because a Vector3 overlay of
  `&mPosX` size-DIFFs 999 (z is the first stack arg, not an ldm).
- attempted_change (scratch-applied, then reverted):

```diff
-    mParticleHandle = Particle::System::New(
-        mParticleHandle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
+    Vector3 &pos = *(Vector3 *)&mPosX;
+    mParticleHandle = Particle::System::New(
+        mParticleHandle, mParticleID, pos.x, pos.y, pos.z, 0, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_waterfall.cpp \
  --func _ZN16daObjWaterfall_c8BehaviorEv --addr 0x020b6e64 --size 0x48 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x48 vs 0x4c
  (72 -> 76 bytes). Reproduces the claimed 999.
- log (variant):

```text
TARGET _ZN16daObjWaterfall_c8BehaviorEv @ 0x020b6e64 size 0x48  bytes: 10402de910d04de20040a0e1641094e50000a0e300108de504008de508008de5b41dd4e1d80094e55c2094e5603094e5b9affdebd80084e50100a0e310d08de2... (72 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x48 vs candidate 0x4c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
