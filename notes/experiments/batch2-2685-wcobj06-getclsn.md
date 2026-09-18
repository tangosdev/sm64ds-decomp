# GetClsnPos mangled + startEnd flattened required (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_wc_obj06.cpp:18-21` keeps
  GetClsnPos as the mangled call (Vector3-by-value return emits D1)
  and startEnd as two flattened Vector3 copies with decompiler dead
  stores (not a clean int[6]-free spelling).
- attempted_change (scratch-applied, then reverted):

```diff
-    int startEnd[6]; /* start[3] then end[3] */
-    int hit[3];
+    Vector3 start, end, hitV;
@@
-            x = mPosX;
-            startEnd[3] = x;                 /* end.x */
-            y = mPosY;
-            startEnd[4] = y;                 /* end.y temp */
-            z = mPosZ;
-            startEnd[1] = y;                 /* start.y temp */
-            startEnd[5] = z;                 /* end.z */
-            startEnd[0] = x;                 /* start.x */
-            startEnd[2] = z;                 /* start.z */
-            startEnd[1] = y + kRayStartAboveFix12;
-            startEnd[4] = waterY;
-            line.SetObjAndLine(*(Vector3 *)&startEnd[0],
-                *(Vector3 *)&startEnd[3], this);
+            start.x = mPosX; start.y = mPosY; start.z = mPosZ;
+            end.x = mPosX; end.y = mPosY; end.z = mPosZ;
+            start.y += kRayStartAboveFix12;
+            end.y = waterY;
+            line.SetObjAndLine(start, end, this);
@@
-                _ZN9dBgCh_Lin10GetClsnPosEv((Vector3 *)hit, &line);
-                mPosY = hit[1];
+                hitV = line.GetClsnPos();
+                mPosY = hitV.y;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_wc_obj06.cpp \
  --func _ZN14daObjWcObj06_c13InitResourcesEv --addr 0x02111f58 --size 0xec \
  --version 2004/b56 --module ov029 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xec vs 0x10c
  (236 -> 268 bytes). Same pattern as the wc_obj01 twin.
- log (variant):

```text
TARGET _ZN14daObjWcObj06_c13InitResourcesEv @ 0x02111f58 size 0xec  bytes: 70402de9a0d04de2d8109fe50040a0e1ba8ffeeb000050e32e00000a0400a0e1d2f7fbeb601094e50060a0e1060051e11e0000da24008de20696fceb5c5094e5... (236 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xec vs candidate 0x10c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
