# Jump3DVec scratch vs Vector3 local (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:388-391` keeps the plain
  Jump3DVec scratch because a Vector3 object would odr-use
  `_ZN7Vector3D1Ev` and add a compiler-only row for nothing.
  Emission-type experiment (cf. piano-2712-pianovec3): per-function
  compare cannot see this wall.
- attempted_change (scratch-applied, then reverted):

```diff
-    Jump3DVec v;
+    Vector3 v;
 
-    Vec3_Sub(&v, &mPos, &mAnchor);
-    if (NormalizeVec3IfNonZero(&v) != 0) {
+    Vec3_Sub((Jump3DVec *)&v, &mPos, &mAnchor);
+    if (NormalizeVec3IfNonZero((Jump3DVec *)&v) != 0) {
```

- command (per-function):

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func _ZN16dMgJump3DMario_c9EnterHoldEv --addr 0x020c7ba4 --size 0xc4 \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (identical bytes, same size 0xc4).
  The body is byte-identical, so per-function compare cannot see this wall.
- log (variant per-function):

```text
TARGET _ZN16dMgJump3DMario_c9EnterHoldEv @ 0x020c7ba4 size 0xc4  bytes: 10402de910d04de20040a0e104008de2141084e2042084e2ced5fdeb04008de298d4fdeb000050e30600000a04009de5200084e508009de5240084e50c009de5... (196 bytes)
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
