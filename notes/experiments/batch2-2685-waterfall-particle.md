# Particle::System::New Fix12 temps DIFF (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_waterfall.cpp:15-20` keeps
  the TU-local int forwarder because declaring the real Fix12<int>
  parameters will not convert from s32 mPosX (no ctor) and the
  pun/temps DIFF.
- attempted_change (scratch-applied, then reverted):

```diff
-extern "C" int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
-    unsigned handle, unsigned effectID, int x, int y, int z,
+extern "C" int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
+    unsigned handle, unsigned effectID, Fix12<int> x, Fix12<int> y, Fix12<int> z,
@@
-    static int New(unsigned handle, unsigned effectID,
-                   int x, int y, int z, const void *dir, Callback *cb)
+    static int New(unsigned handle, unsigned effectID,
+                   Fix12<int> x, Fix12<int> y, Fix12<int> z, const void *dir, Callback *cb)
@@
-    mParticleHandle = Particle::System::New(
-        mParticleHandle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
+    Fix12<int> fx; fx.val = mPosX;
+    Fix12<int> fy; fy.val = mPosY;
+    Fix12<int> fz; fz.val = mPosZ;
+    mParticleHandle = Particle::System::New(
+        mParticleHandle, mParticleID, fx, fy, fz, 0, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_waterfall.cpp \
  --func _ZN16daObjWaterfall_c8BehaviorEv --addr 0x020b6e64 --size 0x48 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x48 vs 0x64
  (72 -> 100 bytes). Fix12 temps DIFF as claimed.
- log (variant):

```text
TARGET _ZN16daObjWaterfall_c8BehaviorEv @ 0x020b6e64 size 0x48  bytes: 10402de910d04de20040a0e1641094e50000a0e300108de504008de508008de5b41dd4e1d80094e55c2094e5603094e5b9affdebd80084e50100a0e310d08de2... (72 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x48 vs candidate 0x64

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
