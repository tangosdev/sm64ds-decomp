# DropShadowScaleXYZ mangled TU-local vs header member form (DSN-2708-01, PR #2708)

- tested_commit: 7027888ce (cleanup-dadsnbase at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daDsnBase_c.cpp:192-194` keeps
  the mangled TU-local call because the header member form
  (`include/dActor_c.h:276-278`, Fix12<int> by value) changes codegen.
  Leftover `src/actors/daDsnBase_c.cpp:36-39`.
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
-        self, &self->mShadowModel, (Matrix4x3 *)((char *)self + 0x360),
-        scaleX, heightAboveGround + 0x28000, scaleZ, 0xf);
+    Fix12<int> fx; fx.val = scaleX;
+    Fix12<int> fy; fy.val = heightAboveGround + 0x28000;
+    Fix12<int> fz; fz.val = scaleZ;
+    self->DropShadowScaleXYZ(self->mShadowModel, *(Matrix4x3 *)((char *)self + 0x360), fx, fy, fz, 0xf);
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func func_ov091_02133098 --addr 0x02133098 --size 0x120 \
  --version 2004/b56 --module ov091 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x120 vs 0x134
  (288 -> 308 bytes, +0x14). Strict-relocs on for both.
- log (variant):

```text
TARGET func_ov091_02133098 @ 0x02133098 size 0x120  bytes: f0472de910d04de20090a0e1601099e5940399e536ee89e2007041e0050957e30579a0d36000a0e30010a0e3974083e0973123e0c72fa0e10288a0e3923023e0... (288 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x120 vs candidate 0x134

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
