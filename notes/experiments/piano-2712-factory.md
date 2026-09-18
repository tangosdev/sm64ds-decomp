# Hand-rolled factory vs `return new daPiano_c()` (PIANO-2712-02, PR #2712)

- tested_commit: b19392a93 (cleanup-dapiano at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daPiano_c/d_a_piano.cpp:99-116` (factory comment) and
  deslop bullet `:21-28` keep the hand-rolled factory because `return new
  daPiano_c()` matches the factory's own bytes but makes mwccarm emit
  unlicensed `_ZN10dBgActor_cD2Ev` (0x38) and `_ZN7Vector3D1Ev` (0x4) copies
  the multi-symbol path refuses. Emission-type experiment: per-function
  compare is insufficient, so the TU object is compiled and inspected and the
  production isolate refusal is reproduced.
- attempted_change (scratch-applied, then reverted):

```diff
 // @symbol daPiano_c_classInit
 extern "C" daPiano_c *daPiano_c_classInit()
 {
-    daPiano_c *actor = (daPiano_c *)_ZN7fBase_cnwEj(sizeof(daPiano_c));
-    if (actor) {
-        _ZN10dBgActor_cC2Ev(actor);
-        *(void **)actor = &_ZTV9daPiano_c;
-        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
-        _ZN11ShadowModelC1Ev(&actor->mShadowModel1);
-        _ZN11ShadowModelC1Ev(&actor->mShadowModel2);
-        _ZN11ShadowModelC1Ev(&actor->mShadowModel3);
-        __cxa_vec_ctor(actor->mCylinderClsn, 2, sizeof(dCcAcPos_c),
-            (void *)_ZN10dCcAcPos_cC1Ev, (void *)_ZN10dCcAcPos_cD1Ev);
-        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
-    }
-    return actor;
+    return new daPiano_c();
 }
```

- command (per-function):

```text
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func daPiano_c_classInit --addr 0x0211e128 --size 0x98 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH. The factory's own 0x98 bytes are
  identical under the cleaner spelling, reproducing the file's comment.
- log (variant per-function):

```text
TARGET daPiano_c_classInit @ 0x0211e128 size 0x98  bytes: 10402de908d04de270009fe5c294fceb0040b0e11500000a4242ffeb60109fe5320e84e2001084e500e2fbebe10f84e2c2dffbebeb0f84e2c0dffbebf50f84e2... (152 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- command (emission): compile the TU once with the production flags
  (`-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e
  -gccext,on -msgstyle gcc -Cpp_exceptions off`, i.e. `rombuild.CFLAGS` with
  `-lang c++`) via `tools/match.py` (`compile_c`), list defined `STT_FUNC`
  symbols, and run `tools/objisolate.py` (`plan_many`) with the 15 ROM-order
  symbols
  (`func_ov063_0211d5f4` .. `daPiano_c_classInit`). Control: the unmodified TU
  emits exactly those 15 `.text` functions, no data, and `plan_many` returns
  `error: None`.
- log (variant emission):

```text
compiled: True 9920
---defined STT_FUNC---
func_ov063_0211d5f4 size=0x234 sec=.text[5]
func_ov063_0211d828 size=0x64 sec=.text[7]
func_ov063_0211d88c size=0x40 sec=.text[9]
func_ov063_0211d8cc size=0x2d8 sec=.text[11]
func_ov063_0211ddac size=0x48 sec=.text[18]
func_ov063_0211dba4 size=0x14 sec=.text[13]
func_ov063_0211dbb8 size=0x1c0 sec=.text[14]
func_ov063_0211dd84 size=0x28 sec=.text[17]
func_ov063_0211dd78 size=0xc sec=.text[16]
func_ov063_0211ddf4 size=0x48 sec=.text[20]
_ZN9daPiano_c16CleanupResourcesEv size=0x50 sec=.text[22]
_ZN9daPiano_c6RenderEv size=0x2c sec=.text[24]
_ZN9daPiano_c8BehaviorEv size=0xf4 sec=.text[25]
_ZN9daPiano_c13InitResourcesEv size=0x17c sec=.text[27]
daPiano_c_classInit size=0x98 sec=.text[29]
_ZN7Vector3D1Ev size=0x4 sec=.text[31]
_ZN10dBgActor_cD2Ev size=0x38 sec=.text[32]
---defined data/object---
---plan_many---
unlicensed content in text-only multi-symbol object: section[31] .text size 0x4 defines ['_ZN7Vector3D1Ev']; section[32] .text size 0x38 defines ['_ZN10dBgActor_cD2Ev']
```

- verdict: CONSTRAINT SUPPORTED
