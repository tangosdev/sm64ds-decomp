# dBgCh_Actr::Init header Fix12i vs ROM Fix12<int> (FEATHER-2713-01, PR #2713)

- tested_commit: f9c1e4609a52783e8a611ba2608225d6e7419aac (cleanup-wingfeather at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daFeather_c/daFeather_c.cpp:128` keeps
  dBgCh_Actr::Init mangled scalar because the header method form takes
  Fix12i, which mangles as `i`, while the ROM is Fix12<int>; the method
  form links Undefined.
- attempted_change (scratch-applied, then reverted):

```diff
-    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x28000, 0xa000, 0, 0);
+    mWithMeshClsn.Init(this, 0x28000, 0xa000, 0, 0);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daFeather_c/daFeather_c.cpp \
  --func _ZN11daFeather_c13InitResourcesEv --addr 0x020b311c --size 0x12c \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH and linkcheck VERIFIED (blind 0); variant bytes
  MATCH (relocs wildcarded) but linkcheck BLIND-1 (one reloc cannot be
  resolved: the header form mangles to
  `_ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_`, which no
  config/**/symbols.txt defines; ROM is
  `_ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_`).
  A link failure IS the result.
- log (variant match.py):

```text
TARGET _ZN11daFeather_c13InitResourcesEv @ 0x020b311c size 0x12c  bytes: 10402de908d04de20040a0e108019fe54292fdeb0120a0e30010a0e10230a0e1d40084e2a38ffdeb000050e308d08d020000a0031040bd081eff2f01c50f84e2... (300 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- link command:

```text
.venv/bin/python tools/linkcheck.py --c src/game/actors/daFeather_c/daFeather_c.cpp \
  --name _ZN11daFeather_c13InitResourcesEv --addr 0x020b311c --size 0x12c --module ov002
```

- log (variant linkcheck):

```text
{
 "name": "_ZN11daFeather_c13InitResourcesEv",
 "module": "ov002",
 "addr": "0x020b311c",
 "verdict": "BLIND-1",
 "diffs": [],
 "blind": 1
}
```

- log (variant undefined symbols, mwccarm 2004/b56 -c):

```text
UNDEFINED symbols referencing dBgCh_Actr Init:
  _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_ shndx=SHN_UNDEF type=STT_NOTYPE size=0
```

- verdict: CONSTRAINT SUPPORTED
