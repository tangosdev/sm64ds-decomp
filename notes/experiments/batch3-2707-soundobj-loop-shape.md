# SoundObj while(1)/break loop shape vs for form (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_sound_obj.cpp:108-118` keeps the
  while(1)/break loop shape because for (and assignment-in-condition)
  forms un-match InitResources.
- attempted_change (scratch-applied, then reverted):

```diff
-    /* while(1)/break is the ROM's loop shape: for and
-       assignment-in-condition forms both un-match InitResources. */
-    while (1) {
-        actor = FindWithActorID(0x167, actor);
-        if (actor == 0)
-            break;
+    for (actor = FindWithActorID(0x167, 0); actor != 0; actor = FindWithActorID(0x167, actor)) {
         if (actor != this) {
             actor->MarkForDestruction();
             _ZN5Sound7PlaySubEjjj5Fix12IiEb(current, 0x7f, 0, 0x7f000, 0);
         }
     }
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_sound_obj.cpp \
  --func _ZN12daSoundObj_c13InitResourcesEv --addr 0x020f95e0 --size 0x14c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; for-form variant 999 word(s) differ, size
  0x14c vs 0x16c (332 -> 364 bytes). The assignment-in-condition form
  (`while ((actor = FindWithActorID(0x167, actor)) != 0)`) was also
  attempted and likewise un-matches at 0x16c (999 differ). Both
  alternates grow the loop by 32 bytes.
- log (for-form variant):

```text
TARGET _ZN12daSoundObj_c13InitResourcesEv @ 0x020f95e0 size 0x14c  bytes: 70402de908d04de20050a0e1081095e5070051e308d08d820000a0837040bd881eff2f810c00a0e3910003e000219fe500119fe5032092e7fc309fe5d42085e5... (332 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x14c vs candidate 0x16c

========================================
MATCHING VERSIONS: none
```

- log (assignment-in-condition variant):

```text
TARGET _ZN12daSoundObj_c13InitResourcesEv @ 0x020f95e0 size 0x14c  bytes: 70402de908d04de20050a0e1081095e5070051e308d08d820000a0837040bd881eff2f810c00a0e3910003e000219fe500119fe5032092e7fc309fe5d42085e5... (332 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x14c vs candidate 0x16c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
