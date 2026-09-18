# dCcAc_c::Init member form vs mangled scalar (FEATHER-2713-01, PR #2713)

- tested_commit: f9c1e4609a52783e8a611ba2608225d6e7419aac (cleanup-wingfeather at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daFeather_c/daFeather_c.cpp:126` keeps
  dCcAc_c::Init mangled scalar because the header member form with
  Fix12<int> by value changes codegen (6az).
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x6e000, 0x6e000, 0x100002, 0);
+    Fix12<int> dcc_radius; dcc_radius.val = 0x6e000;
+    Fix12<int> dcc_height; dcc_height.val = 0x6e000;
+    mdCcAc_c.Init(this, dcc_radius, dcc_height, 0x100002, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daFeather_c/daFeather_c.cpp \
  --func _ZN11daFeather_c13InitResourcesEv --addr 0x020b311c --size 0x12c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x12c vs 0x140
  (300 -> 320 bytes, +0x14).
- log (variant):

```text
TARGET _ZN11daFeather_c13InitResourcesEv @ 0x020b311c size 0x12c  bytes: 10402de908d04de20040a0e108019fe54292fdeb0120a0e30010a0e10230a0e1d40084e2a38ffdeb000050e308d08d020000a0031040bd081eff2f01c50f84e2... (300 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x12c vs candidate 0x140

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
