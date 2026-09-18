# common.h first for the flat Matrix4x3 in 020c76e0 (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:17-18,54-55` includes common.h
  first because the mat4x3 copy in func_ov006_020c76e0 wants the flat
  `s32 m[12]` spelling; math/Matrix.h's nested {r, t} scalarizes it.
  (Both headers define Matrix4x3 under MATRIX4X3_DEFINED; first seen wins.)
- attempted_change (scratch-applied, then reverted):

```diff
-#include "common.h"
 #include "dMgJump3DMario_c.h"
+#include "common.h"
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func func_ov006_020c76e0 --addr 0x020c76e0 --size 0x54 \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x54 vs 0x78
  (84 -> 120 bytes). Letting the nested spelling win scalarizes the copy
  as claimed.
- log (variant):

```text
TARGET func_ov006_020c76e0 @ 0x020c76e0 size 0x54  bytes: 10402de90040a0e1141094e5182094e51c3094e534009fe511d2fdebfe12d4e128009fe52dd2fdeb20e09fe568c084e20f00bee80f00ace80f00bee80f00ace8... (84 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x54 vs candidate 0x78

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
