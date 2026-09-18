# Volatile u16 re-read vs plain re-read (FEATHER-2713-01, PR #2713)

- tested_commit: f9c1e4609a52783e8a611ba2608225d6e7419aac (cleanup-wingfeather at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daFeather_c/daFeather_c.cpp:180` keeps
  the `*(volatile u16*)&mSwayAngle` re-read because it holds the
  cartridge's reload.
- attempted_change (scratch-applied, then reverted):

```diff
-        *(short*)&mSwayAngle += 0x400;
-        u16 newv = *(volatile u16*)&mSwayAngle;
+        *(short*)&mSwayAngle += 0x400;
+        u16 newv = mSwayAngle;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daFeather_c/daFeather_c.cpp \
  --func _ZN11daFeather_c8BehaviorEv --addr 0x020b2e9c --size 0x280 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (the reload `ldrh` is emitted
  either way; `volatile` is not load-bearing here). Per instructions the
  source is NOT fixed; the constraint as stated is disproved.
- log (variant):

```text
TARGET _ZN11daFeather_c8BehaviorEv @ 0x020b2e9c size 0x280  bytes: 10402de910d04de25c129fe50040a0e10000d1e5010050e30100a0030000a013000050e30f00001a0400a0e10277fdeb081090e5000051e30200001aed2e00eb... (640 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
