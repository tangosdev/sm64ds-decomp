# SoundObj PlaySub loop stays int vs bool (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_sound_obj.cpp:25-28` keeps loop as int
  because the row byte passes through unconverted.
- attempted_change (scratch-applied, then reverted):

```diff
-extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 soundID, u32 volume, u32 pan, Fix12i distance, int loop);
+extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 soundID, u32 volume, u32 pan, Fix12i distance, bool loop);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_sound_obj.cpp \
  --func func_ov002_020f9468 --addr 0x020f9468 --size 0x94 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x94 vs
  0xac (148 -> 172 bytes). The bool conversion grows the starter by 24
  bytes.
- log (variant):

```text
TARGET func_ov002_020f9468 @ 0x020f9468 size 0x94  bytes: 10402de908d04de20140a0e1be1dd0e1b020d4e10f1041e2010052e10b0000aae030d0e5d81090e57f20a0e300308de57f3061e2d40090e50336a0e1cd65fceb... (148 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x94 vs candidate 0xac

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
