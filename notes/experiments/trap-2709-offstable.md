# Spawn offset table by symbol vs named local (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp:92-97` references the
  offset table by SYMBOL at the use site, never through a named local, because
  naming it makes LICM hoist it to the front of the preheader instead of
  first-use order (notes/mwccarm-codegen.md 6v).
- attempted_change (scratch-applied, then reverted):

```diff
+        s32 *offs = data_ov063_0211e9f8;
         for (; j < 3; j++) {
-            base.x = mHomePosX - data_ov063_0211e9f8[even];
-            base.y = mHomePosY + data_ov063_0211e9f8[odd];
+            base.x = mHomePosX - offs[even];
+            base.y = mHomePosY + offs[odd];
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp \
  --func _ZN11daTrsTrap_c13InitResourcesEv --addr 0x0211cf00 --size 0x370 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result: baseline MATCH; variant 7 word(s) differ, same size. The table-base
  `ldr r6, [pc, ...]` lands at +0x294 (front of the preheader) instead of
  +0x2ac (first-use order), reproducing the review's LICM description.
  (--brief with equal sizes prints the full per-word table; the log below
  excerpts the 7 MISMATCH rows. All other rows read OK or reloc wildcard.)
- log (variant):

```text
TARGET _ZN11daTrsTrap_c13InitResourcesEv @ 0x0211cf00 size 0x370  bytes: f04f2de92cd04de200a0a0e108109ae50000a0e32114a0e1031001e25111cae528018ae50120a0e35621cae5bc10dae1410051e30020a011000052e33100000a... (880 bytes)
  2004/b56: 7 word(s) differ

--- closest: 2004/b56 (7 differ) ---
   +0x294 | 0980a0e1 mov r8, sb               | d0609fe5 ldr r6, [pc, #0xd0]      | <<< MISMATCH
   +0x298 | 24108de5 str r1, [sp, #0x24]      | 0980a0e1 mov r8, sb               | <<< MISMATCH
   +0x29c | b890cde1 strh sb, [sp, #8]        | 24108de5 str r1, [sp, #0x24]      | <<< MISMATCH
   +0x2a0 | ba00cde1 strh r0, [sp, #0xa]      | b890cde1 strh sb, [sp, #8]        | <<< MISMATCH
   +0x2a4 | bc90cde1 strh sb, [sp, #0xc]      | ba00cde1 strh r0, [sp, #0xa]      | <<< MISMATCH
   +0x2a8 | 0170a0e3 mov r7, #1               | bc90cde1 strh sb, [sp, #0xc]      | <<< MISMATCH
   +0x2ac | b8609fe5 ldr r6, [pc, #0xb8]      | 0170a0e3 mov r7, #1               | <<< MISMATCH

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
