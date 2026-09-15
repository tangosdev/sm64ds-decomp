# Bracketed strength pragma vs file-global (GAMEOVER-2711-01, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dScGameOver_c.cpp:45-48` keeps the pragma file-global
  because bracketing it around the one member does nothing (trailing `on`
  wins for the whole file), so there is no narrower place to put it.
- attempted_change (scratch-applied, then reverted):

```diff
  * here, all 9 members verify, so it costs the other eight nothing. */
-#pragma opt_strength_reduction off
```

```diff
  * member-index form perturbs the tail) are the ROM's own shape too. */
+#pragma opt_strength_reduction off
 extern "C" {  /* .c-derived member: C linkage for the whole block */
 void func_ov003_020b060c(void *self)
```

```diff
     o->mIntroPhase = 2;
     data_0209d45c |= 1;
     data_0209d454 |= 3;
 }
 }
+#pragma opt_strength_reduction on
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dScGameOver_c.cpp \
  --func func_ov003_020b060c --addr 0x020b060c --size 0x124 \
  --version 2004/b56 --module ov003 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x124 vs 0x140
  (292 -> 320 bytes) -- byte-identical outcome to dropping the pragma
  outright. The trailing `on` wins for the whole file; bracketing is not a
  narrower form.
- log (variant):

```text
TARGET func_ov003_020b060c @ 0x020b060c size 0x124  bytes: 70402de99410d0e5000051e30300000a010051e32b00000a7040bde81eff2fe100e0a0e30ac0a0e3e8209fe50140a0e38e3080e0b017d3e18e50a0e1000051e3... (292 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x124 vs candidate 0x140

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
