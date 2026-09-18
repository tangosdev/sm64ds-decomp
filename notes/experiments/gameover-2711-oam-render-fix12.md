# OAM::Render scalar spelling vs by-value Fix12 form (GAMEOVER-2711-01, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dScGameOver_c.cpp:27-29,95-98` keeps the mangled
  scalar spelling of OAM::Render (Fix12i scalars for the scale pair)
  because the by-value Fix12<int> form changes codegen (same wall family
  as SetAnim 6az; OAM.h excludes those overloads).
- attempted_change (scratch-applied, then reverted):

```diff
 #include "dScGameOver_c.h"
 #include "Stage.h"
 #include "OamAttr.h"
 #include "decl_common.h"
+#include "math/Fix12.h"
+
+namespace OAM {
+void Render(bool sub, OamAttr *data, int x, int y,
+            int palette, int priority, Fix12<int> scaleX, Fix12<int> scaleY,
+            int rotation, int mode);
+}
```

```diff
     int i;
+    Fix12<int> _sx; _sx.val = 0x1000;
+    Fix12<int> _sy; _sy.val = 0x1000;
     for (i = 0; i < 8; i++) {
-        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
+        OAM::Render(
             0, data_ov003_020b1824[i],
             mGlyphX[i], mGlyphY[i],
-            -1, -1, 0x1000, 0x1000, 0, -1);
+            -1, -1, _sx, _sy, 0, -1);
     }
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dScGameOver_c.cpp \
  --func _ZN13dScGameOver_c6RenderEv --addr 0x020b0814 --size 0x80 \
  --version 2004/b56 --module ov003 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x80 vs 0x90
  (128 -> 144 bytes). The variant object references the same
  `_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii` callee (verified in the
  object symbol table), so the diff is the by-value homing, not a wrong
  symbol.
- log (variant):

```text
TARGET _ZN13dScGameOver_c6RenderEv @ 0x020b0814 size 0x80  bytes: f0432de91cd04de20090a0e10080a0e364409fe50850a0e10070e0e3016aa0e300708de504708de508608de50c608de510508de514708de5880089e0f025d0e1... (128 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x80 vs candidate 0x90

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
