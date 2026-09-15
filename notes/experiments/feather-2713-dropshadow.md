# dActor_c::DropShadowRadHeight member form vs mangled scalar (FEATHER-2713-01, PR #2713)

- tested_commit: f9c1e4609a52783e8a611ba2608225d6e7419aac (cleanup-wingfeather at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daFeather_c/daFeather_c.cpp:277-278,305-306` keeps
  dActor_c::DropShadowRadHeight mangled scalar because the header member
  form with Fix12<int> by value changes codegen (6az).
- attempted_change (scratch-applied, then reverted):

```diff
-        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
-            c, &c->mShadowModel, &c->mShadowMtx, 0x96000, 0x320000, 0xf);
+        Fix12<int> ds_rad1; ds_rad1.val = 0x96000;
+        Fix12<int> ds_dep1; ds_dep1.val = 0x320000;
+        c->DropShadowRadHeight(c->mShadowModel, c->mShadowMtx, ds_rad1, ds_dep1, 0xf);
@@ second call site
-        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
-            c, &c->mShadowModel, &c->mShadowMtx, r4, r5 + 0x28000, 0xf);
+        Fix12<int> ds_rad2; ds_rad2.val = r4;
+        Fix12<int> ds_dep2; ds_dep2.val = r5 + 0x28000;
+        c->DropShadowRadHeight(c->mShadowModel, c->mShadowMtx, ds_rad2, ds_dep2, 0xf);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daFeather_c/daFeather_c.cpp \
  --func func_ov002_020b2c44 --addr 0x020b2c44 --size 0x1ec \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x1ec vs 0x208
  (492 -> 520 bytes, +0x1c).
- log (variant):

```text
TARGET func_ov002_020b2c44 @ 0x020b2c44 size 0x1ec  bytes: 70402de970d04de20060a0e114008de25c1086e20320a0e33529feeb14109de518209de51c309de5b4019fe5b324feebfe18d6e1a8019fe5cf24feebf019d6e1... (492 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x1ec vs candidate 0x208

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
