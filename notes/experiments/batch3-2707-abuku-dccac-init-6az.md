# Abuku dCcAc_c::Init scalar wrapper vs header member form (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_abuku.cpp:25-26,67-69` keeps
  dCcAc_c::Init a TU-local mangled scalar call because Fix12i-by-value
  homes on the stack (6az).
- attempted_change (scratch-applied, then reverted):

```diff
 int daObjAbuku_c::InitResources() {
-  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
-      &mdCcAc_c, this, 0x96000, 0x96000, 0x100002, 0);
+  Fix12<int> r, h;
+  r.val = 0x96000;
+  h.val = 0x96000;
+  mdCcAc_c.Init(this, r, h, 0x100002, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_abuku.cpp \
  --func _ZN12daObjAbuku_c13InitResourcesEv --addr 0x020b3518 --size 0x50 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x50 vs
  0x64 (80 -> 100 bytes). The member form grows InitResources by 20
  bytes.
- log (variant):

```text
TARGET _ZN12daObjAbuku_c13InitResourcesEv @ 0x020b3518 size 0x50  bytes: 10402de908d04de23cc09fe50040a0e1962aa0e30410a0e10230a0e100c08de500c0a0e3d40084e204c08de5f084fdeb010c84e24b1fa0e3be10c0e10100a0e3... (80 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x50 vs candidate 0x64

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
