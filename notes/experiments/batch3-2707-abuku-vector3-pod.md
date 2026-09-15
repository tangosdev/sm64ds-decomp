# Abuku AbukuVector3 POD vs real Vector3 local (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_obj_abuku.cpp:33-34,46-48` keeps the
  AbukuVector3 POD stand-in because a real Vector3 local emits
  vague-linkage ~Vector3 into this text-only TU.
- attempted_change (scratch-applied, then reverted):

```diff
 int func_ov002_020b3344(daObjAbuku_c *c){
   dBgCh_Gnd rg;
-  AbukuVector3 v;
+  Vector3 v;
   rg.StartDetectingWater();
   int x = c->mPosX;
   int y = c->mPosY;
   int z = c->mPosZ;
   int yk = y + 0x1f4000;
   v.x = x;
   v.y = yk;
   v.z = z;
-  rg.SetObjAndPos(*reinterpret_cast<const Vector3 *>(&v), c);
+  rg.SetObjAndPos(v, c);
```

- command (function body):

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_obj_abuku.cpp \
  --func func_ov002_020b3344 --addr 0x020b3344 --size 0x98 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant body still MATCHes (0x98, no word
  differs). The claim is about an EXTRA emitted function, so the TU was
  also compiled with the production flags (`rombuild.CFLAGS`, C++ form)
  and its defined symbols plus the production `rombuild._isolate`
  verdict (7 enrolled keeps, 10 deadstrip-data rows) were recorded.
- log (variant, function body):

```text
TARGET func_ov002_020b3344 @ 0x020b3344 size 0x98  bytes: 10402de960d04de20040a0e10c008de28510feeb0c008de24e08feeb600094e564e094e55c3094e57dc980e20c008de200108de20420a0e100308de504c08de5... (152 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- log (variant, TU emitted defined STT_FUNC symbols):

```text
DEFINED FUNC SYMBOLS:
  _ZN12daObjAbuku_c13InitResourcesEv size=0x50
  _ZN12daObjAbuku_c8BehaviorEv size=0x13c
  _ZN12daObjAbuku_cD0Ev size=0x44
  _ZN12daObjAbuku_cD1Ev size=0x30
  _ZN7Vector3D1Ev size=0x4
  daObjAbuku_c_classInit size=0x38
  func_ov002_020b330c size=0x38
  func_ov002_020b3344 size=0x98
```

- log (variant, production isolate):

```text
PRODUCTION _isolate verdict: unlicensed content in text-only multi-symbol object: section[29] .text size 0x4 defines ['_ZN7Vector3D1Ev']
```

- log (baseline, same TU compile + isolate):

```text
DEFINED FUNC SYMBOLS:
  _ZN12daObjAbuku_c13InitResourcesEv size=0x50
  _ZN12daObjAbuku_c8BehaviorEv size=0x13c
  _ZN12daObjAbuku_cD0Ev size=0x44
  _ZN12daObjAbuku_cD1Ev size=0x30
  daObjAbuku_c_classInit size=0x38
  func_ov002_020b330c size=0x38
  func_ov002_020b3344 size=0x98
PRODUCTION _isolate verdict: OK (no error)
```

- verdict: CONSTRAINT SUPPORTED
