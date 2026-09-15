# PianoVec3 second-Vector3-type vs real Vector3 (PIANO-2712-02, PR #2712)

- tested_commit: b19392a93 (cleanup-dapiano at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daPiano_c/d_a_piano.cpp:245-249` (PianoVec3 comment)
  and deslop bullet `:42-45` keep the TU-local POD `PianoVec3` for the
  C-origin helpers' scratch vectors because `types.h`'s `Vector3` carries an
  inline destructor that would make this TU emit `_ZN7Vector3D1Ev`,
  unlicensed content the production multi-symbol path refuses. (The review
  pointed at the factory comment's `:105-109` naming the same symbol; the
  second type itself lives at `:249`.) Emission-type experiment.
- attempted_change (scratch-applied, then reverted): the natural spelling,
  making every `PianoVec3` local a real `Vector3` with one typedef so all
  call sites are untouched:

```diff
-struct PianoVec3 { int x, y, z; };
+typedef Vector3 PianoVec3;
```

- command (per-function, representative `func_ov063_0211d5f4` which holds
  `PianoVec3 in, out` locals):

```text
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func func_ov063_0211d5f4 --addr 0x0211d5f4 --size 0x234 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH. The body is byte-identical with real
  `Vector3` locals, so per-function compare cannot see this wall.
- log (variant per-function):

```text
TARGET func_ov063_0211d5f4 @ 0x0211d5f4 size 0x234  bytes: 10402de928d04de20020a0e30118a0e30137a0e3001061e20040a0e10c308de510208de518208de51c208de520208de514108de5fe18d4e1e8019fe5cd79fceb... (564 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- command (emission): same method as `piano-2712-factory.md` (compile the TU
  with production flags, list defined `STT_FUNC`, `plan_many` with the 15
  ROM-order symbols; unmodified-TU control emits exactly the 15 with
  `error: None`).
- log (variant emission):

```text
compiled: True 9504
---defined STT_FUNC---
func_ov063_0211d5f4 size=0x234 sec=.text[5]
_ZN7Vector3D1Ev size=0x4 sec=.text[7]
func_ov063_0211d828 size=0x64 sec=.text[8]
func_ov063_0211d88c size=0x40 sec=.text[10]
func_ov063_0211d8cc size=0x2d8 sec=.text[12]
func_ov063_0211ddac size=0x48 sec=.text[19]
func_ov063_0211dba4 size=0x14 sec=.text[14]
func_ov063_0211dbb8 size=0x1c0 sec=.text[15]
func_ov063_0211dd84 size=0x28 sec=.text[18]
func_ov063_0211dd78 size=0xc sec=.text[17]
func_ov063_0211ddf4 size=0x48 sec=.text[21]
_ZN9daPiano_c16CleanupResourcesEv size=0x50 sec=.text[23]
_ZN9daPiano_c6RenderEv size=0x2c sec=.text[25]
_ZN9daPiano_c8BehaviorEv size=0xf4 sec=.text[26]
_ZN9daPiano_c13InitResourcesEv size=0x17c sec=.text[28]
daPiano_c_classInit size=0x98 sec=.text[30]
---plan_many---
unlicensed content in text-only multi-symbol object: section[7] .text size 0x4 defines ['_ZN7Vector3D1Ev']
```

- verdict: CONSTRAINT SUPPORTED
