# Warpkun dCcAc_c::Init scalar wrapper vs header member form (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_warpkun.cpp:15-18` keeps dCcAc_c::Init a
  TU-local mangled scalar wrapper because the header form
  (`include/dCcAc_c.h:57`) takes Fix12<int> by value (6az). (The
  SetRanges half of the same Leftover is S13 decl placement, not
  codegen, and is not attempted here.)
- attempted_change (scratch-applied, then reverted):

```diff
     Fix12i height = (((spawnParam >> 4) & 0xf) + 1) << 0x12;
-    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
-        &mdCcAc_c, this, radius, height, 2, 0x400000);
+    Fix12<int> fradius, fheight;
+    fradius.val = radius;
+    fheight.val = height;
+    mdCcAc_c.Init(this, fradius, fheight, 2, 0x400000);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_warpkun.cpp \
  --func _ZN11daWarpkun_c13InitResourcesEv --addr 0x020ec4c4 --size 0x70 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x70 vs
  0x84 (112 -> 132 bytes). The header member form grows InitResources
  by 20 bytes.
- log (variant):

```text
TARGET _ZN11daWarpkun_c13InitResourcesEv @ 0x020ec4c4 size 0x70  bytes: 70402de908d04de20040a0e1085094e50010a0e30f2005e2012082e20269a0e10620a0e17d3886e200108de54492fceb2502a0e10f0000e2013080e20200a0e3... (112 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x70 vs candidate 0x84

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
