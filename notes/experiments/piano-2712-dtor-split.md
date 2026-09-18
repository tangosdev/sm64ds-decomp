# D0/D1 split vs defining ~daPiano_c in the TU (PIANO-2712-02, PR #2712)

- tested_commit: b19392a93 (cleanup-dapiano at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daPiano_c/d_a_piano.cpp:29-32` keep D0/D1 split
  (one line each in two per-function sources that sat beside it and no
  longer exist -- promoting the class absorbed them; the two
  `legacy_source` rows in `config/tu_manifest.d/ov063/daPiano_c.json`
  keep their full paths) because the vtable is a `symbols.txt` blob
  (`_ZTV9daPiano_c`) and defining `~daPiano_c` in the TU would emit a
  competing `_ZTV` (key function) the fail-closed path refuses.
  Emission-type experiment.
- attempted_change (scratch-applied, then reverted): append the natural
  destructor definition to the end of `d_a_piano.cpp`:

```diff
 }
 }
+
+// @symbol _ZN9daPiano_cD1Ev
+// @symbol _ZN9daPiano_cD0Ev
+daPiano_c::~daPiano_c()
+{
+}
```

- command (per-function; baselines first: D1 `0x0211d4b8/0x94` in its own
  file MATCH, D0 `0x0211d54c/0xa8` in its own file MATCH):

```text
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func _ZN9daPiano_cD1Ev --addr 0x0211d4b8 --size 0x94 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func _ZN9daPiano_cD0Ev --addr 0x0211d54c --size 0xa8 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: both bodies MATCH inside the merged TU too. The destructor bodies
  are not the wall.
- log (variant per-function):

```text
TARGET _ZN9daPiano_cD1Ev @ 0x0211d4b8 size 0x94  bytes: 10402de974109fe50040a0e170209fe5010084e0002084e5c867fceb64009fe564309fe5000084e00210a0e34020a0e36757fdebf50f84e2c0e2fbebeb0f84e2... (148 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

```text
TARGET _ZN9daPiano_cD0Ev @ 0x0211d54c size 0xa8  bytes: 10402de984109fe50040a0e180209fe5010084e0002084e5a367fceb74009fe574309fe5000084e00210a0e34020a0e34257fdebf50f84e29be2fbebeb0f84e2... (168 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- command (emission): same method as `piano-2712-factory.md` (compile the TU
  with production flags, list defined symbols, `plan_many` with the 15
  ROM-order symbols).
- log (variant emission):

```text
compiled: True 14592
---defined STT_FUNC---
_ZN9daPiano_cD0Ev size=0xa8 sec=.text[24]
_ZN9daPiano_c6RenderEv size=0x2c sec=.text[47]
_ZN9daPiano_c8BehaviorEv size=0xf4 sec=.text[48]
_ZN9daPiano_c16CleanupResourcesEv size=0x50 sec=.text[45]
_ZN9daPiano_c13InitResourcesEv size=0x17c sec=.text[50]
_ZN9daPiano_cD1Ev size=0x94 sec=.text[26]
_ZN9daPiano_cD2Ev size=0x94 sec=.text[22]
func_ov063_0211d5f4 size=0x234 sec=.text[28]
func_ov063_0211d828 size=0x64 sec=.text[30]
func_ov063_0211d88c size=0x40 sec=.text[32]
func_ov063_0211d8cc size=0x2d8 sec=.text[34]
func_ov063_0211ddac size=0x48 sec=.text[41]
func_ov063_0211dba4 size=0x14 sec=.text[36]
func_ov063_0211dbb8 size=0x1c0 sec=.text[37]
func_ov063_0211dd84 size=0x28 sec=.text[40]
func_ov063_0211dd78 size=0xc sec=.text[39]
func_ov063_0211ddf4 size=0x48 sec=.text[43]
daPiano_c_classInit size=0x98 sec=.text[52]
---defined data/object---
_ZTI7fBase_c type=STT_OBJECT size=0x8 sec=.data[5]
_ZTS7fBase_c type=STT_OBJECT size=0x9 sec=.data[8]
_ZTS7dBase_c type=STT_OBJECT size=0x9 sec=.data[7]
_ZTS8dActor_c type=STT_OBJECT size=0xa sec=.data[9]
_ZTS9daPiano_c type=STT_OBJECT size=0xb sec=.data[10]
_ZTI10dBgActor_c type=STT_OBJECT size=0xc sec=.data[11]
_ZTS10dBgActor_c type=STT_OBJECT size=0xd sec=.data[19]
_ZTI8dActor_c type=STT_OBJECT size=0xc sec=.data[15]
_ZTI7dBase_c type=STT_OBJECT size=0xc sec=.data[13]
_ZTI9daPiano_c type=STT_OBJECT size=0xc sec=.data[17]
_ZTV9daPiano_c type=STT_OBJECT size=0x88 sec=.data[20]
---plan_many(15)---
unlicensed content in text-only multi-symbol object: section[5] .data size 0x8 defines ['_ZTI7fBase_c']; section[7] .data size 0x9 defines ['_ZTS7dBase_c']; section[8] .data size 0x9 defines ['_ZTS7fBase_c']; section[9] .data size 0xa defines ['_ZTS8dActor_c']; section[10] .data size 0xb defines ['_ZTS9daPiano_c']; section[11] .data size 0xc defines ['_ZTI10dBgActor_c']; section[13] .data size 0xc defines ['_ZTI7dBase_c']; section[15] .data size 0xc defines ['_ZTI8dActor_c']; section[17] .data size 0xc defines ['_ZTI9daPiano_c']; section[19] .data size 0xd defines ['_ZTS10dBgActor_c']; section[20] .data size 0x88 defines ['_ZTV9daPiano_c']; section[22] .text size 0x94 defines ['_ZN9daPiano_cD2Ev']; section[24] .text size 0xa8 defines ['_ZN9daPiano_cD0Ev']; section[26] .text size 0x94 defines ['_ZN9daPiano_cD1Ev']
```

- verdict: CONSTRAINT SUPPORTED at the time, SUPERSEDED by the promotion of
  ov063/daPiano_c. The refusal quoted in the log above is `unlicensed content
  in text-only multi-symbol object` -- a property of a text-only enrollment
  with no manifest entry to license the vague-linkage data, not a property of
  the destructor. A manifest entry licenses those eleven records as
  `deadstrip-data` against their configured ROM homes, and declaring the
  destructor inline and empty in `include/daPiano_c.h` -- rather than out of
  line as the diff above does -- emits D1 then D0, the cartridge's own order,
  and no D2 at all. All 17 functions then MATCH inside the one TU and
  objisolate reduces the object cleanly. The out-of-line spelling this note
  tested is still the wrong one: its own emission log shows D2 and D0 ahead
  of D1.
