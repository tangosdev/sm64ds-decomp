# Klass as daBakubaku_c ICE claim (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_bakubaku.cpp:13-15` keeps
  `struct Klass;` incomplete because completing it as daBakubaku_c
  ICEs mwccarm's PMF.
- attempted_change (scratch-applied, then reverted):

```diff
-struct Klass;
-typedef void (Klass::*PMF)();
+typedef void (daBakubaku_c::*PMF)();
 /* Tables are 0x10 apart. SetState calls enter at +0; Behavior calls main at +8. */
 struct StateEntry { PMF enter; PMF main; };
@@
-        (((Klass *)this)->*(state->main))();
+        (this->*(state->main))();
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_bakubaku.cpp \
  --func _ZN12daBakubaku_c8BehaviorEv --addr 0x021121b4 --size 0x128 \
  --version 2004/b56 --module ov032 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (no ICE, no bytes differ).
  The minimal `typedef daBakubaku_c Klass;` spelling was also tried and
  MATCHes. No compiler crash observed on 2004/b56.
- log (variant):

```text
TARGET _ZN12daBakubaku_c8BehaviorEv @ 0x021121b4 size 0x128  bytes: 10402de90040a0e1191e84e2d32f84e20330a0e39a6dfeeb000050e30100a0131040bd181eff2f11010c84e2f5a2fcebe8009fe5000084e0f2a2fcebb01394e5... (296 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
