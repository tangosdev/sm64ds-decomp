# dBgW_KcMbg::SetFile mangled TU-local spelling vs method form (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp:30-32` keeps the mangled
  TU-local spelling of dBgW_KcMbg::SetFile because its signature carries
  Fix12<int> by value (wall 6az; `include/dBgW_KcMbg.h:68-69`), so the method
  form size-DIFFs.
- attempted_change (scratch-applied, then reverted):

```diff
     {
         int m = mIndex;
+        Fix12<int> fx;
         if (m == 3) {
-            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
-                &mMovingMeshCollider, *(void **)((char *)data_ov063_0211e28c[m] + 4), &mClsnMat,
-                0x1000, mAngleY, data_ov063_0211e9e8[m]);
+            fx.val = 0x1000;
+            mMovingMeshCollider.SetFile(
+                *(KCL_File **)((char *)data_ov063_0211e28c[m] + 4), mClsnMat, fx,
+                mAngleY, *data_ov063_0211e9e8[m]);
         } else {
-            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
-                &mMovingMeshCollider, *(void **)((char *)data_ov063_0211e28c[m] + 4), &mClsnMat,
-                0x199, mAngleY, data_ov063_0211e9e8[m]);
+            fx.val = 0x199;
+            mMovingMeshCollider.SetFile(
+                *(KCL_File **)((char *)data_ov063_0211e28c[m] + 4), mClsnMat, fx,
+                mAngleY, *data_ov063_0211e9e8[m]);
         }
     }
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp \
  --func _ZN11daTrsTrap_c13InitResourcesEv --addr 0x0211cf00 --size 0x370 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x370 vs 0x388
  (880 -> 904 bytes). The method form passes Fix12<int> by value as a struct
  instead of the TU-local int, and the body grows by 6 instructions.
- log (variant):

```text
TARGET _ZN11daTrsTrap_c13InitResourcesEv @ 0x0211cf00 size 0x370  bytes: f04f2de92cd04de200a0a0e108109ae50000a0e32114a0e1031001e25111cae528018ae50120a0e35621cae5bc10dae1410051e30020a011000052e33100000a... (880 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x370 vs candidate 0x388

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
