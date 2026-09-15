# isDosun flag doubling as mVertSpeed zero (DSN-2708-01, PR #2708)

- tested_commit: 7027888ce (cleanup-dadsnbase at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daDsnBase_c.cpp:243-248` doubles
  isDosun as the mVertSpeed zero the ROM reuses out of the same register.
  Leftover `src/actors/daDsnBase_c.cpp:42-43`.
- attempted_change (scratch-applied, then reverted):

```diff
-    int isDosun = 0;
-    self->mVertSpeed = isDosun;
+    self->mVertSpeed = 0;
     *(s32 *)(c + 0x398) = 3;
     *(u8 *)(c + 0x39e) = 0xa;
-    if (self->actorID == kDosunActorID)
-        isDosun = 1;
+    int isDosun = (self->actorID == kDosunActorID);
     if (isDosun != 0) {
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func func_ov091_02132f04 --addr 0x02132f04 --size 0xf0 \
  --version 2004/b56 --module ov091 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (same bytes, strict-relocs pass).
  The split spelling reproduces, so the doubling is not load-bearing.
- log (variant):

```text
TARGET func_ov091_02132f04 @ 0x02132f04 size 0xf0  bytes: 10402de918d04de20040a0e1a81084e2000091e5602084e2010940e2000081e5001092e5a80094e5000081e0000082e5941394e5600094e5010050e118d08dc2... (240 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
