# DecIfAbove0_Byte interface already fixed (DSN-2708-01, PR #2708)

- tested_commit: 7027888ce (cleanup-dadsnbase at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/daDsnBase_c.cpp:94` spells
  DecIfAbove0_Byte from its definition (`u8 DecIfAbove0_Byte(u8 *)` at
  src/DecIfAbove0_Byte.c), not its call sites; fixed on this branch by
  3fab96894. No attempted_change; baseline verification only.
- attempted_change: N/A (already fixed -- baseline match only).

- command:

```text
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func func_ov091_02132ff4 --addr 0x02132ff4 --size 0x2c \
  --version 2004/b56 --module ov091 --strict-relocs --brief
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func func_ov091_02132e98 --addr 0x02132e98 --size 0x6c \
  --version 2004/b56 --module ov091 --strict-relocs --brief
.venv/bin/python tools/match.py --c src/actors/daDsnBase_c.cpp \
  --func func_ov091_02132e64 --addr 0x02132e64 --size 0x34 \
  --version 2004/b56 --module ov091 --strict-relocs --brief
```

- result: all three baselines MATCH under strict-relocs.
- log (baselines):

```text
TARGET func_ov091_02132ff4 @ 0x02132ff4 size 0x2c  bytes: 10402de91c109fe50040a0e1010084e0721ffceb000050e30200a003980384051040bde81eff2fe19e030000
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
TARGET func_ov091_02132e98 @ 0x02132e98 size 0x6c  bytes: 10402de950109fe50040a0e1010084e0c91ffceb000050e31040bd181eff2f1138009fe50410a0e3981384e5b122fceb2c109fe52038a0e1930181e024209fe5... (108 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
TARGET func_ov091_02132e64 @ 0x02132e64 size 0x34  bytes: 10402de924109fe50040a0e1010084e0d61ffceb000050e30000a003980384052800a0039e03c4051040bde81eff2fe19e030000
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: VERIFIED-CURRENT
