# Warpkun widened isPlayer bool vs folded if (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_warpkun.cpp:87-90` keeps the widened
  `int isPlayer` because folding it into the if changes Behavior's size
  and DIFFs (S8).
- attempted_change (scratch-applied, then reverted):

```diff
-                /* Widened bool: folding this into the if above changes
-                   Behavior's size and DIFFs (S8). */
-                int isPlayer = (hit->actorID == kPlayerActorID) ? 1 : 0;
-                if (isPlayer != 0) {
+                if (hit->actorID == kPlayerActorID) {
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_warpkun.cpp \
  --func _ZN11daWarpkun_c8BehaviorEv --addr 0x020ec410 --size 0xb4 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xb4 vs 0xa8
  (180 -> 168 bytes). Folding shrinks the function by 12 bytes.
- log (variant):

```text
TARGET _ZN11daWarpkun_c8BehaviorEv @ 0x020ec410 size 0xb4  bytes: 30402de904d04de20040a0e1f80094e5000050e31c00000afe18d4e1000051e31b00001ac092fceb0050b0e11800000abc00d5e1bf0050e30100a0030000a013... (180 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xb4 vs candidate 0xa8

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
