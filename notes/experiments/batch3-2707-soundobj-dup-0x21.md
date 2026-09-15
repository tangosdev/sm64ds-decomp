# SoundObj duplicate 0x21 sub-sound test (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_sound_obj.cpp:104-107` tests 0x21 twice,
  first and last, because both compares are in the ROM and dropping the
  duplicate un-matches InitResources.
- attempted_change (scratch-applied, then reverted):

```diff
-    /* 0x21 is tested twice, first and last: both compares are in the ROM,
-       and dropping the duplicate un-matches InitResources. */
     if (current == 0x20 || current == 0x29 || current == 0x21 || current == 0x1e || current == 0x50 ||
-        (current >= 0x19 && current <= 0x1d) || current == 0x4f || current == 0x22 || current == 0x2a || current == 0x21)
+        (current >= 0x19 && current <= 0x1d) || current == 0x4f || current == 0x22 || current == 0x2a)
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_sound_obj.cpp \
  --func _ZN12daSoundObj_c13InitResourcesEv --addr 0x020f95e0 --size 0x14c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x14c vs
  0x144 (332 -> 324 bytes). Dropping the duplicate shrinks the
  function by 8 bytes.
- log (variant):

```text
TARGET _ZN12daSoundObj_c13InitResourcesEv @ 0x020f95e0 size 0x14c  bytes: 70402de908d04de20050a0e1081095e5070051e308d08d820000a0837040bd881eff2f810c00a0e3910003e000219fe500119fe5032092e7fc309fe5d42085e5... (332 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x14c vs candidate 0x144

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
