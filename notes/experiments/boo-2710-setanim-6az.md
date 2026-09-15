# ModelAnim::SetAnim member form vs mangled call (BOO-2710-01, PR #2710)

- tested_commit: 8f5abbc11 (cleanup-datrs at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrs_c/_ZN7daTrs_c13InitResourcesEv.cpp:107,126` keeps
  the mangled SetAnim call because the header member form carries Fix12<int>
  BY VALUE and size-DIFFs (the 6az wall).
- attempted_change (scratch-applied, then reverted):

```diff
-        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File **)((char *)&data_ov063_0211ede4 + 4), 0, 0x1000, 0);
+        { Fix12<int> spd; spd.val = 0x1000; mModelAnim.SetAnim(*(BCA_File **)((char *)&data_ov063_0211ede4 + 4), 0, spd, 0); }
-        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File **)((char *)&data_ov063_0211eddc + 4), 0, 0x1000, 0);
+        { Fix12<int> spd; spd.val = 0x1000; mModelAnim.SetAnim(*(BCA_File **)((char *)&data_ov063_0211eddc + 4), 0, spd, 0); }
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrs_c/_ZN7daTrs_c13InitResourcesEv.cpp \
  --func _ZN7daTrs_c13InitResourcesEv --addr 0x0211b9bc --size 0x9a0 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x9a0 vs 0x9b8
  (2464 -> 2472 bytes). The by-value Fix12<int> member call breaks the match.
- log (variant):

```text
TARGET _ZN7daTrs_c13InitResourcesEv @ 0x0211b9bc size 0x9a0  bytes: 70402de910d04de20040a0e1050c84e20010a0e3b41dc0e19c1484e5bc00d4e1d20050e30110a003000051e36100000a080094e50f0000e20c0080e2cf05c4e5... (2464 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x9a0 vs candidate 0x9b8

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
