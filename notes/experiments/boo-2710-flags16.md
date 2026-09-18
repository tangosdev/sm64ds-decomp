# FLAGS16 macros vs direct halfword form (BOO-2710-01, PR #2710)

- tested_commit: fc81e17a0 (lunavyqo/cleanup-datrs at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrs_c/_ZN7daTrs_c13InitResourcesEv.cpp:77-78` keeps
  the FLAGS16/FLAGS16T pointer/integer round trips because the direct
  `*(unsigned short *)(c + 0x5d4)` form changes codegen.
- attempted_change (scratch-applied, then reverted):

```diff
-#define FLAGS16 (*(unsigned short *)((long long)(c + 0x5d4)))
-#define FLAGS16T (*(unsigned short *)((long long)((char *)((long long)c) + 0x5d4)))
+#define FLAGS16 (*(unsigned short *)(c + 0x5d4))
+#define FLAGS16T (*(unsigned short *)(c + 0x5d4))
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrs_c/_ZN7daTrs_c13InitResourcesEv.cpp \
  --func _ZN7daTrs_c13InitResourcesEv --addr 0x0211b9bc --size 0x9a0 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x9a0 vs 0x998
  (2464 -> 2456 bytes). Reproduces the review's independent measurement.
- log (variant):

```text
TARGET _ZN7daTrs_c13InitResourcesEv @ 0x0211b9bc size 0x9a0  bytes: 70402de910d04de20040a0e1050c84e20010a0e3b41dc0e19c1484e5bc00d4e1d20050e30110a003000051e36100000a080094e50f0000e20c0080e2cf05c4e5... (2464 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x9a0 vs candidate 0x998

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
