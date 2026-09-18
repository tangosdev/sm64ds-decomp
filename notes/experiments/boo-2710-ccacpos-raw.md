# mdCcAcPos_c flags/vulnFlags member-anchored RMW (BOO-2710-01, PR #2710)

- tested_commit: 8f5abbc11 (cleanup-datrs at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrs_c/_ZN7daTrs_c13InitResourcesEv.cpp:158,281` keeps
  the mdCcAcPos_c.flags / .vulnFlags raw 0x19c/0x1a0 RMWs because the
  member-anchored address does not fold (two adds where the ROM has one).
- attempted_change (scratch-applied, then reverted):

```diff
-        *(int *)((long long)(c + 0x1a0)) |= 0x8000;
+        mdCcAcPos_c.vulnFlags |= 0x8000;
-            *(int *)((long long)(c + 0x19c)) |= 1;
+            mdCcAcPos_c.flags |= 1;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrs_c/_ZN7daTrs_c13InitResourcesEv.cpp \
  --func _ZN7daTrs_c13InitResourcesEv --addr 0x0211b9bc --size 0x9a0 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH. The member-anchored RMWs codegen
  identically to the raw (long long)(c + off) forms under 2004/b56.
- log (variant):

```text
TARGET _ZN7daTrs_c13InitResourcesEv @ 0x0211b9bc size 0x9a0  bytes: 70402de910d04de20040a0e1050c84e20010a0e3b41dc0e19c1484e5bc00d4e1d20050e30110a003000051e36100000a080094e50f0000e20c0080e2cf05c4e5... (2464 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
