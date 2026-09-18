# ModelAnim::SetAnim member form vs scalar mangled spelling (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:24-28,83-85` keeps the mangled
  scalar spelling because the member form (Fix12<int> by value) size-DIFFs.
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214042c, 0, kAnimSpeed, 0);
+    Fix12<int> _spd; _spd.val = kAnimSpeed;
+    mModelAnim.SetAnim(data_ov006_0214042c, 0, _spd, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func _ZN16dMgJump3DMario_c9EnterDampEv --addr 0x020c79a8 --size 0x88 \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x88 vs 0x94
  (136 -> 148 bytes).
- log (variant):

```text
TARGET _ZN16dMgJump3DMario_c9EnterDampEv @ 0x020c79a8 size 0x88  bytes: 10402de908d04de264109fe50040a0e1000091e5010050e3020000da54009fe5612bfdeb010000ea4c009fe55e2bfdeb0020a0e344009fe500208de5001090e5... (136 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x88 vs candidate 0x94

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
