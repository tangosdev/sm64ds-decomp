# ModelAnim::SetAnim method form size-DIFF (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_pg_mthr.cpp:19-24` keeps
  ModelAnim::SetAnim mangled because the method form size-DIFFs
  (`SetAnim 0x58->0x64`).
- attempted_change (scratch-applied, then reverted):

```diff
-  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (BCA_File *)data_ov018_02113bf0[1], 0, 0x1000, 0);
+  Fix12<int> speed; speed.val = 0x1000;
+  self->mModelAnim.SetAnim((BCA_File *)data_ov018_02113bf0[1], 0, speed, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_pg_mthr.cpp \
  --func func_ov018_021121dc --addr 0x021121dc --size 0x58 \
  --version 2004/b56 --module ov018 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x58 vs 0x64
  (88 -> 100 bytes). Reproduces the claimed 0x58->0x64 exactly.
- log (variant):

```text
TARGET func_ov018_021121dc @ 0x021121dc size 0x58  bytes: 10402de908d04de20020a0e340109fe500208de50040a0e1041091e5d40084e2013aa0e35011fceb010aa0e3300184e50000a0e38803c4e53c00a0e38903c4e5... (88 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x58 vs candidate 0x64

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
