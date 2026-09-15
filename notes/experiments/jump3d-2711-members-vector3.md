# Jump3DVec members vs Vector3 members (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `include/dMgJump3DMario_c.h:24-27` keeps the mAnchor/mPos/mVel
  members as plain Jump3DVec because types.h's Vector3 declares an (empty)
  destructor, which would make this TU emit `_ZN7Vector3D1Ev`.
  Emission-type experiment (cf. piano-2712-pianovec3): per-function
  compare cannot see this wall.
- attempted_change (scratch-applied to the header and the TU, then reverted):

```diff
--- include/dMgJump3DMario_c.h
-    Jump3DVec mAnchor;      /* 0x04 ... */
+    Vector3 mAnchor;      /* 0x04 ... */
-    Jump3DVec mPos;         /* 0x14 ... */
-    Jump3DVec mVel;         /* 0x20 ... */
+    Vector3 mPos;         /* 0x14 ... */
+    Vector3 mVel;         /* 0x20 ... */
--- src/actors/dMgJump3DMario_c.cpp
-void Vec3_Sub(Jump3DVec *out, Jump3DVec *a, Jump3DVec *b);
-void AddVec3(Jump3DVec *a, Jump3DVec *b, Jump3DVec *c);
-int  NormalizeVec3IfNonZero(Jump3DVec *v);
+void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
+void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
+int  NormalizeVec3IfNonZero(Vector3 *v);
-void func_ov006_020bfec0(void *cam, Jump3DVec *pos, short *out);
+void func_ov006_020bfec0(void *cam, Vector3 *pos, short *out);
-    Jump3DVec v;
+    Vector3 v;
```

- command (per-function, representative D1):

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func _ZN16dMgJump3DMario_cD1Ev --addr 0x020c893c --size 0xc8 \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (identical bytes, same size 0xc8).
  C1 (0x2c) and EnterHold (0xc4) also still MATCH under the variant: no
  member-dtor calls are emitted anywhere measured, so per-function compare
  cannot see this wall.
- log (variant per-function):

```text
TARGET _ZN16dMgJump3DMario_cD1Ev @ 0x020c893c size 0xc8  bytes: 10402de90040a0e180109fe580009fe5001084e5833cfdeb78009fe5813cfdeb74009fe57f3cfdeb70009fe57d3cfdeb6c009fe57b3cfdeb68009fe5793cfdeb... (200 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- command (emission): compile the TU with production flags
  (rombuild.VERSION 2004/b56, rombuild.CFLAGS with -lang c++ for //cpp),
  apply the manifest deadstrip policy, `isolate_many` with the 28 ROM-order
  symbols. The TU carries one spanning delinks entry
  (`config/arm9/overlays/ov006/delinks.txt`: `complete`, `.text
  start:0x020c762c end:0x020c8a30`), so production takes the intact-object
  path. Unmodified-TU control emits exactly the 28 plus licensed
  compiler-only output with `error: None`.
- log (variant emission; assembler $a/$d mapping symbols elided):

```text
selected: 28 symbols in ROM order, 0x020c762c..0x020c8a04
compiled: True 19000
---defined STT_FUNC---
_ZN16dMgJump3DMario_c12Unk_020c762cEv size=0xa4 sec=.text[5]
_ZN16dMgJump3DMario_c12Unk_020c76d0Ev size=0x8 sec=.text[7]
_ZN16dMgJump3DMario_c12Unk_020c76d8Ev size=0x8 sec=.text[8]
func_ov006_020c76e0 size=0x54 sec=.text[9]
func_ov006_020c7734 size=0x12c sec=.text[11]
func_ov006_020c7860 size=0x8c sec=.text[13]
_ZN16dMgJump3DMario_c9StateDampEv size=0xbc sec=.text[15]
_ZN16dMgJump3DMario_c12EnterRespawnEv size=0x68 sec=.text[51]
_ZN16dMgJump3DMario_c9EnterDampEv size=0x88 sec=.text[17]
_ZN16dMgJump3DMario_c9StateHoldEv size=0x174 sec=.text[19]
_ZN16dMgJump3DMario_c8EnterHitEv size=0xcc sec=.text[36]
_ZN16dMgJump3DMario_c11EnterBounceEv size=0x1c sec=.text[40]
_ZN16dMgJump3DMario_c11StateBounceEv size=0x264 sec=.text[38]
_ZN16dMgJump3DMario_c9EnterMoveEv size=0x1c sec=.text[26]
_ZN16dMgJump3DMario_c9StateMoveEv size=0x3c4 sec=.text[24]
_ZN16dMgJump3DMario_c9EnterHoldEv size=0xc4 sec=.text[21]
_ZN7Vector3D1Ev size=0x4 sec=.text[23]
_ZN16dMgJump3DMario_c12StateFallOutEv size=0x3c sec=.text[28]
func_ov006_020c8658 size=0x28 sec=.text[47]
func_ov006_020c8084 size=0xc8 sec=.text[30]
_ZN16dMgJump3DMario_c12StateRiseOutEv size=0x94 sec=.text[32]
func_ov006_020c81e0 size=0x90 sec=.text[34]
_ZN16dMgJump3DMario_c11StateWindUpEv size=0x70 sec=.text[42]
func_ov006_020c862c size=0x20 sec=.text[44]
_ZN16dMgJump3DMario_c9StateIdleEv size=0xc sec=.text[46]
_ZN16dMgJump3DMario_c12StateRespawnEv size=0xe8 sec=.text[49]
func_ov006_020c87d0 size=0x16c sec=.text[53]
_ZN16dMgJump3DMario_cD1Ev size=0xc8 sec=.text[55]
_ZN16dMgJump3DMario_cD0Ev size=0xd0 sec=.text[57]
_ZN16dMgJump3DMario_cD2Ev size=0xc8 sec=.text[59]
_ZN16dMgJump3DMario_cC2Ev size=0x2c sec=.text[61]
_ZN16dMgJump3DMario_cC1Ev size=0x2c sec=.text[63]
_ZN22dMg3DHeyhoObjAdapter_cD2Ev size=0x10 sec=.text[65]
---deadstrip policy---
['_ZN16dMgJump3DMario_cC2Ev', '_ZN16dMgJump3DMario_cD0Ev', '_ZN16dMgJump3DMario_cD2Ev', '_ZN22dMg3DHeyhoObjAdapter_cD2Ev', '_ZTI16dMgJump3DMario_c', '_ZTI22dMg3DHeyhoObjAdapter_c', '_ZTS16dMgJump3DMario_c', '_ZTS22dMg3DHeyhoObjAdapter_c', '_ZTV16dMgJump3DMario_c', '_ZTV22dMg3DHeyhoObjAdapter_c']
deadstrip error: None
---isolate_many---
error: unlicensed content in text-only multi-symbol object: section[23] .text size 0x4 defines ['_ZN7Vector3D1Ev']
```

- result (emission): function bytes identical BUT packaging refuses the
  emission: `unlicensed content in text-only multi-symbol object:
  section[23] .text size 0x4 defines ['_ZN7Vector3D1Ev']`. Missing license
  confirmed: `config/tu_manifest.d/ov006/dMgJump3DMario_c.json` carries 10
  compiler_only_output rows (6 deadstrip-data RTTI/vtable, 4 deadstrip
  structors D2/D0/C2/base-D2), none covering `_ZN7Vector3D1Ev`, and no file
  under `config/tu_manifest.d/ov006/` names that symbol at all. The vague
  `deadstrip-duplicate` disposition rombuild documents for exactly this
  symbol is not licensed here.

- verdict: CONSTRAINT SUPPORTED
