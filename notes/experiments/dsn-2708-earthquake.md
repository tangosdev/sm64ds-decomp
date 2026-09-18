# Earthquake scalar tail vs Fix12<int> by value (DSN-2708-01, PR #2708)

- tested_commit: 7027888ce (cleanup-dadsnbase at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daDsnBase_c.cpp:261` (decl `:107`) keeps
  the scalar-int tail because Fix12<int> by value changes codegen.
  Leftover `src/actors/daDsnBase_c.cpp:36-39`. No header declares
  dActor_c::Earthquake (verified); the (void *, const Vector3 &, int)
  spelling is daDkk_c.cpp's, so the member form is reconstructed as
  `dActor_c::Earthquake(const Vector3 &, Fix12<int>)` per the mangled name.
- attempted_change (scratch-applied, then reverted):

```diff
-extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, const Vector3 &pos, int magnitude);
+extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, const Vector3 &pos, Fix12<int> magnitude);
 ...
-    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(self, quakePos, 0x7d0000);
+    Fix12<int> mag; mag.val = 0x7d0000;
+    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(self, quakePos, mag);
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func func_ov091_02132f04 --addr 0x02132f04 --size 0xf0 \
  --version 2004/b56 --module ov091 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xf0 vs 0xfc
  (240 -> 252 bytes, +0xc). Strict-relocs on for both.
- log (variant):

```text
TARGET func_ov091_02132f04 @ 0x02132f04 size 0xf0  bytes: 10402de918d04de20040a0e1a81084e2000091e5602084e2010940e2000081e5001092e5a80094e5000081e0000082e5941394e5600094e5010050e118d08dc2... (240 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xf0 vs candidate 0xfc

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
