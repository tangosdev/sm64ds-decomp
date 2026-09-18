# Spawn v16 field-write order x, y, z (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp:98-99` writes v16's
  three fields x, y, z in that order because any other order swaps the two
  strh (notes/mwccarm-codegen.md 6v).
- disposition: compiler_constraint. The cleaner alternative (any other field
  order) was measured under 2004/b56 and swaps the two strh; this entry is the
  pinned experiment and durable artifact.
- attempted_change (scratch-applied, then reverted):

```diff
         v16.x = 0;
         even = j;
-        v16.y = -0x8000;
         v16.z = 0;
+        v16.y = -0x8000;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp \
  --func _ZN11daTrsTrap_c13InitResourcesEv --addr 0x0211cf00 --size 0x370 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant 2 word(s) differ, same size. The y/z swap
  exchanges the two strh (+0x2a0/+0x2a4; the source comment said +0x29c/+0x2a0
  and has been corrected to the measured offsets). Control probe: a
  grouped same-order spelling (x, y, z consecutive, `even = j` moved after)
  still MATCHES, so the load-bearing part is the field order, not the
  interleaving. (--brief with equal sizes prints the full per-word table;
  the log below excerpts the 2 MISMATCH rows. All other rows read OK or
  reloc wildcard.)
- log (variant):

```text
TARGET _ZN11daTrsTrap_c13InitResourcesEv @ 0x0211cf00 size 0x370  bytes: f04f2de92cd04de200a0a0e108109ae50000a0e32114a0e1031001e25111cae528018ae50120a0e35621cae5bc10dae1410051e30020a011000052e33100000a... (880 bytes)
  2004/b56: 2 word(s) differ

--- closest: 2004/b56 (2 differ) ---
   +0x2a0 | ba00cde1 strh r0, [sp, #0xa]      | bc90cde1 strh sb, [sp, #0xc]      | <<< MISMATCH
   +0x2a4 | bc90cde1 strh sb, [sp, #0xc]      | ba00cde1 strh r0, [sp, #0xa]      | <<< MISMATCH

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
