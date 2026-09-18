# dBgW_KcMbg::SetFile method form size-DIFFs Init (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_bk_dossunbar.cpp:16-18` keeps
  dBgW_KcMbg::SetFile mangled because SetFile as a method
  size-DIFFs Init.
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
-        &mMeshCollider, (int)kclFile, &mClsnMat, 0x1000, mAngleY,
-        *(void **)((char *)&data_ov015_0211453c + j));
+    Fix12<int> scale; scale.val = 0x1000;
+    mMeshCollider.SetFile((KCL_File *)kclFile, mClsnMat, scale, mAngleY,
+        **(CLPS_Block **)((char *)&data_ov015_0211453c + j));
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_bk_dossunbar.cpp \
  --func _ZN19daObjBk_Dossunbar_c13InitResourcesEv --addr 0x021120fc --size 0x134 \
  --version 2004/b56 --module ov015 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x134 vs 0x140
  (308 -> 320 bytes).
- log (variant):

```text
TARGET _ZN19daObjBk_Dossunbar_c13InitResourcesEv @ 0x021120fc size 0x134  bytes: 30402de914d04de20040a0e1bc00d4e1350050e30100a0030000a013000050e30100a0132c0384052c1394e50c00a0e3910002e0dc009fe5... (308 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x134 vs candidate 0x140

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
