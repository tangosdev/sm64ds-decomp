# Warpkun radius textual param1 load vs spawnParam local (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_warpkun.cpp:62-67` loads param1 textually
  twice because routing radius through the spawnParam local swaps the
  r5/r6 register assignment and DIFFs.
- attempted_change (scratch-applied, then reverted):

```diff
     u32 spawnParam = param1;
-    Fix12i radius = ((param1 & 0xf) + 1) << 0x12;
+    Fix12i radius = ((spawnParam & 0xf) + 1) << 0x12;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_warpkun.cpp \
  --func _ZN11daWarpkun_c13InitResourcesEv --addr 0x020ec4c4 --size 0x70 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 7 word(s) differ at the same size
  0x70 (112 bytes). The register assignment swaps as claimed.
- log (variant):

```text
TARGET _ZN11daWarpkun_c13InitResourcesEv @ 0x020ec4c4 size 0x70  bytes: 70402de908d04de20040a0e1085094e50010a0e30f2005e2012082e20269a0e10620a0e17d3886e200108de54492fceb2502a0e10f0000e2013080e20200a0e3... (112 bytes)
  2004/b56: 7 word(s) differ

--- closest: 2004/b56 (7 differ) ---
```

- verdict: CONSTRAINT SUPPORTED
