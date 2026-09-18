# HsBillboard defaultless actorID switch (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_hs_billboard.cpp:20-21,64-69` keeps
  the actorID switch defaultless in matched form.
- attempted_change (scratch-applied, then reverted):

```diff
     case 0x131: mVariant = 3; break; /* HS_B_STAR */
+    default: mVariant = 0; break;
     }
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_hs_billboard.cpp \
  --func _ZN18daObjHsBillboard_c13InitResourcesEv --addr 0x021112a8 --size 0x9c \
  --version 2004/b56 --module ov031 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x9c vs
  0xa8 (156 -> 168 bytes). A default case grows the jump table path.
- log (variant):

```text
TARGET _ZN18daObjHsBillboard_c13InitResourcesEv @ 0x021112a8 size 0x9c  bytes: 10402de90040a0e1bc10d4e180009fe5000041e0030050e300f18f900e0000ea020000ea040000ea060000ea080000ea0000a0e32401c4e5070000ea0100a0e3... (156 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x9c vs candidate 0xa8

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
