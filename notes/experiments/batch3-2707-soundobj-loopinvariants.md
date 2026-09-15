# SoundObj file-global opt_loop_invariants off (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_sound_obj.cpp:84-87` keeps the file-global
  `#pragma opt_loop_invariants off` because dropping it un-matches the
  FindWithActorID loop in InitResources.
- attempted_change (scratch-applied, then reverted):

```diff
 // @symbol _ZN12daSoundObj_c13InitResourcesEv
-/* opt_loop_invariants is file-global last-wins: this off covers the
-   FindWithActorID loop below (dropping it un-matches InitResources),
-   and no other member has a loop. */
-#pragma opt_loop_invariants off
 int daSoundObj_c::InitResources()
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_sound_obj.cpp \
  --func _ZN12daSoundObj_c13InitResourcesEv --addr 0x020f95e0 --size 0x14c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x14c vs
  0x15c (332 -> 348 bytes). Dropping the pragma grows the loop by 16
  bytes.
- log (variant):

```text
TARGET _ZN12daSoundObj_c13InitResourcesEv @ 0x020f95e0 size 0x14c  bytes: 70402de908d04de20050a0e1081095e5070051e308d08d820000a0837040bd881eff2f810c00a0e3910003e000219fe500119fe5032092e7fc309fe5d42085e5... (332 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x14c vs candidate 0x15c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
