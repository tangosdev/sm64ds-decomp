# DropShadowRadHeight method form size-DIFF (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_pg_mthr.cpp:19-24` keeps
  DropShadowRadHeight mangled because the method form size-DIFFs
  (`DropShadow 0x100->0x110`).
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(self, self->mShadowModel, *(Matrix4x3*)(s + 0xf0), 0x140000, 0x50000, 0xf);
+    Fix12<int> radius; radius.val = 0x140000;
+    Fix12<int> depth; depth.val = 0x50000;
+    self->DropShadowRadHeight(self->mShadowModel, *(Matrix4x3*)(s + 0xf0), radius, depth, 0xf);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_pg_mthr.cpp \
  --func func_ov018_02111d28 --addr 0x02111d28 --size 0x100 \
  --version 2004/b56 --module ov018 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x100 vs 0x110
  (256 -> 272 bytes). Reproduces the claimed 0x100->0x110 exactly.
- log (variant):

```text
TARGET func_ov018_02111d28 @ 0x02111d28 size 0x100  bytes: 30402de90cd04de20040a0e1fe18d4e1f00084e20aa8fceb5c0094e50528a0e3c001a0e1140184e5601094e50400a0e1c111a0e1181184e5643094e5531f84e2... (256 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x100 vs candidate 0x110

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
