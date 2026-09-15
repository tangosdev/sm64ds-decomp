# Real daPiano_c PMF dispatch, no shadow layout (PIANO-2712-02, PR #2712)

- tested_commit: b19392a93 (cleanup-dapiano at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: PIANO-2712-01 is fixed on this branch -- the state dispatch uses the
  real `daPiano_c` PMF (`typedef void (daPiano_c::*PianoPMF)()`,
  `src/game/actors/daPiano_c/d_a_piano.cpp:238`) and the real `mStateIdx`
  (`:257-258`, `:269-271`); the non-polymorphic shadow layout is gone. No
  attempted change: this experiment baseline-matches the current source to
  confirm green (VERIFIED-CURRENT).
- attempted_change: none (verify-current, no scratch edit).
- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func func_ov063_0211ddac --addr 0x0211ddac --size 0x48 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func func_ov063_0211ddf4 --addr 0x0211ddf4 --size 0x48 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: both dispatch functions MATCH with the real-class PMF.
- log (current source):

```text
TARGET func_ov063_0211ddac @ 0x0211ddac size 0x48  bytes: 00402de904d04de2c81680e5c81690e52c209fe5013282e0041093e5c10080e0011011e20020901500109315011092170010930531ff2fe104d08de20040bde8... (72 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

```text
TARGET func_ov063_0211ddf4 @ 0x0211ddf4 size 0x48  bytes: 00402de904d04de2c81690e530209fe5011282e0083081e2041093e5c10080e0011011e20020901500109315011092170010930531ff2fe104d08de20040bde8... (72 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: VERIFIED-CURRENT
