# mFlags_5d4 member-spelled bit reads vs shift-extracts (BOO-2710-01, PR #2710)

- tested_commit: 8f5abbc11 (cleanup-datrs at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrs_c/_ZN7daTrs_c8BehaviorEv.cpp:141,273,283,313`
  keeps the mFlags_5d4 bit tests as shift-extracts because the member-spelled
  reads change codegen (only b1/b3 reads are member-spelled; Render proves
  those).
- attempted_change (scratch-applied, then reverted):

```diff
-            if ((((u32)((*(u16 *)(c + 0x5d4)) << 0x18)) >> 0x1f) == 0) {
+            if (mFlags_5d4.b7 == 0) {
-    if ((((u32)((*(u16 *)(c + 0x5d4)) << 0x1d)) >> 0x1f) != 0)
+    if (mFlags_5d4.b2 != 0)
-        if (((((u32)((*(u16 *)(c + 0x5d4)) << 0x1f)) >> 0x1f) != 0) && (mPosZ < -0x12c000))
+        if ((mFlags_5d4.b0 != 0) && (mPosZ < -0x12c000))
-                    if ((((u32)((*(u16 *)(c + 0x5d4))) << 0x1a) >> 0x1f) != 0) {
+                    if (mFlags_5d4.b5 != 0) {
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrs_c/_ZN7daTrs_c8BehaviorEv.cpp \
  --func _ZN7daTrs_c8BehaviorEv --addr 0x0211b0a4 --size 0x7e4 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH. All four member-spelled bit reads
  (b7/b2/b0/b5) codegen identically to the shift-extracts under 2004/b56.
- log (variant):

```text
TARGET _ZN7daTrs_c8BehaviorEv @ 0x0211b0a4 size 0x7e4  bytes: 30402de9dcd04de20040a0e1611f84e2a9d1fbeb9c0494e5000050e30c00000a9cd7fbeb9c0494e5000050e30c00000a9cd7fbeb8c0484e58c1494e5000051e30500000a5c0094e55c0081e5600094e5... (2020 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
