# ModelAnim::SetAnim mangled-free vs header member form, wall 6az (PIANO-2712-02, PR #2712)

- tested_commit: b19392a93 (cleanup-dapiano at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daPiano_c/d_a_piano.cpp:35-38` keep
  `ModelAnim::SetAnim` (representative for the `SetAnim`/`Init`/`SetFile`
  group) a mangled free declaration because the ROM signature carries
  `Fix12<int>` by value while the header member form
  (`include/ModelAnim.h:88-89`: `void SetAnim(BCA_File*, int, Fix12<int>,
  u32)`) homes differently (wall 6az). Body-shape experiment on the caller,
  `_ZN9daPiano_c13InitResourcesEv` (`0x0211dfac`, size `0x17c`).
- attempted_change (scratch-applied, then reverted): the natural member-form
  spelling at `d_a_piano.cpp:156`. `Fix12<int>` has no implicit constructor
  from `int` (`include/math/Fix12.h:20-23` is a bare `{ T val; }`
  aggregate), so the value must be materialized:

```diff
     f = Animation::LoadFile(gPianoAttackAnimationFile);
-    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)f, 0, 0x1000, 0);
+    Fix12<int> speed = {0x1000};
+    mModelAnim.SetAnim((BCA_File *)f, 0, speed, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func _ZN9daPiano_c13InitResourcesEv --addr 0x0211dfac --size 0x17c \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant size 0x17c vs 0x184 (+8 bytes, the
  `Fix12<int>` local materialization), no match. Same shape as daBmb's
  report ("the header method form size-DIFFs", `src/actors/daBmb_c.cpp:14-17`).
- log (variant):

```text
TARGET _ZN9daPiano_c13InitResourcesEv @ 0x0211dfac size 0x17c  bytes: f04f2de90cd04de200a0a0e144019fe59ee6fbeb0010a0e1320e8ae20120a0e30030e0e3ffe3fbebe10f8ae2bedffbebeb0f8ae2bcdffbebf50f8ae2b3dffbeb... (380 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x17c vs candidate 0x184

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
