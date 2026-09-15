# SceneGateVt manual vtable vs virtual-call form (GAMEOVER-2711-01, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dScGameOver_c.cpp:30-32,57-58,273-276` reaches
  data_0209f5bc through a TU-local shadow vtable spelt out to slot 5.
- attempted_change (scratch-applied, then reverted):

```diff
-struct SceneGateVt { void *slots[5]; int (*slot5)(void *); };
-struct SceneGate { SceneGateVt *vt; };
+struct SceneGate { virtual int f00(); virtual int f01(); virtual int f02(); virtual int f03(); virtual int f04(); virtual int f05(); };
```

```diff
-    SceneGate *gate = data_0209f5bc;
-
-    if (gate->vt->slot5(gate) == 0)
+    if (data_0209f5bc->f05() == 0)
         goto end;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dScGameOver_c.cpp \
  --func _ZN13dScGameOver_c8BehaviorEv --addr 0x020b0894 --size 0x2a0 \
  --version 2004/b56 --module ov003 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (identical bytes, same size 0x2a0).
  The manual vtable spelling is not load-bearing; the natural virtual-call
  form compiles to the same code.
- log (variant):

```text
TARGET _ZN13dScGameOver_c8BehaviorEv @ 0x020b0894 size 0x2a0  bytes: 10402de978129fe50040a0e1000091e5001090e5141091e531ff2fe1000050e39400000a0400a0e152ffffeb9400d4e5020050e30100a0331040bd381eff2f31... (672 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
