# int* Mtx zeroing vs named members in 020c7734 (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:251-259` zeroes the Mtx through
  `int *` because writing the named members instead costs 11 words.
- attempted_change (scratch-applied, then reverted):

```diff
-        int *mp = (int *)&m;
-        mp[0] = 0; mp[1] = 0; mp[2] = 0; mp[3] = 0;
+        m.a = 0; m.b = 0; m.c = 0; m.d = 0;
         m.d = r2res;
         m.a = r1res;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func func_ov006_020c7734 --addr 0x020c7734 --size 0x12c \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant 11 word(s) differ, same size 0x12c.
  Reproduces the comment's 11 exactly; the diff is localized to +0xb0..0xd8
  (the zeroing sequence). The log below elides the matching prefix/suffix
  runs; all 11 mismatch lines are verbatim.
- log (variant):

```text
TARGET func_ov006_020c7734 @ 0x020c7734 size 0x12c  bytes: 30402de924d04de20050a0e13500d5e5000050e324d08d023040bd081eff2f01ec009fe50000d0e5000050e33000000ae0009fe50c208de2000090e5141085e2... (300 bytes)
  2004/b56: 11 word(s) differ

--- closest: 2004/b56 (11 differ) ---
   +0xb0 | 10208de2 add r2, sp, #0x10        | 0000a0e3 mov r0, #0               | <<< MISMATCH
   +0xb4 | 0000a0e3 mov r0, #0               | 10008de5 str r0, [sp, #0x10]      | <<< MISMATCH
   +0xb8 | 000082e5 str r0, [r2]             | 1c008de5 str r0, [sp, #0x1c]      | <<< MISMATCH
   +0xbc | 040082e5 str r0, [r2, #4]         | 1c108de5 str r1, [sp, #0x1c]      | <<< MISMATCH
   +0xc0 | 080082e5 str r0, [r2, #8]         | 10408de5 str r4, [sp, #0x10]      | <<< MISMATCH
   +0xc4 | 0c0082e5 str r0, [r2, #0xc]       | 14008de5 str r0, [sp, #0x14]      | <<< MISMATCH
   +0xc8 | 1c108de5 str r1, [sp, #0x1c]      | 18008de5 str r0, [sp, #0x18]      | <<< MISMATCH
   +0xcc | 10408de5 str r4, [sp, #0x10]      | 0010e0e3 mvn r1, #0               | <<< MISMATCH
   +0xd0 | 0010e0e3 mvn r1, #0               | 00108de5 str r1, [sp]             | <<< MISMATCH
   +0xd4 | 00108de5 str r1, [sp]             | 04108de5 str r1, [sp, #4]         | <<< MISMATCH
   +0xd8 | 04108de5 str r1, [sp, #4]         | 10208de2 add r2, sp, #0x10        | <<< MISMATCH

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
