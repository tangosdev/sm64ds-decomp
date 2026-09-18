# Animation::Advance member form vs mangled call (BOO-2710-01, PR #2710)

- tested_commit: 8f5abbc11 (cleanup-datrs at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrs_c/_ZN7daTrs_c8BehaviorEv.cpp:345` keeps
  the mangled Animation::Advance call because the member form goes through
  the thunk.
- attempted_change (scratch-applied, then reverted):

```diff
-        _ZN9Animation7AdvanceEv(c + 0x3d0);
+        mModelAnim.Advance();
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrs_c/_ZN7daTrs_c8BehaviorEv.cpp \
  --func _ZN7daTrs_c8BehaviorEv --addr 0x0211b0a4 --size 0x7e4 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH. The member form folds the +0x50
  secondary-base adjustment inline with no thunk and no codegen change.
- log (variant):

```text
TARGET _ZN7daTrs_c8BehaviorEv @ 0x0211b0a4 size 0x7e4  bytes: 30402de9dcd04de20040a0e1611f84e2a9d1fbeb9c0494e5000050e30c00000a9cd7fbeb8c0484e58c1494e5000051e30500000a5c0094e55c0081e5600094e5... (2020 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
