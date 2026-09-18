# volatile aimPos pinning OnAimedAtWithEgg (DSN-2708-01, PR #2708)

- tested_commit: 7027888ce (cleanup-dadsnbase at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daDsnBase_c.cpp:302-306` keeps
  volatile aimPos to pin the OnAimedAtWithEgg call the distance check
  ignores. Leftover `src/actors/daDsnBase_c.cpp:43-44`.
- attempted_change (scratch-applied, then reverted):

```diff
-        volatile Vector3 aimPos;
+        Vector3 aimPos;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func func_ov091_02132dc0 --addr 0x02132dc0 --size 0xa4 \
  --version 2004/b56 --module ov091 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (same bytes, strict-relocs pass).
  The plain local reproduces, so volatile is not load-bearing.
- log (variant):

```text
TARGET func_ov091_02132dc0 @ 0x02132dc0 size 0xa4  bytes: 30402de90cd04de20910a0e30040a0e1bb76fbeb0050b0e11d00000a5c1094e50400a0e100108de5601094e504108de5641094e508108de5001090e5741091e5... (164 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
