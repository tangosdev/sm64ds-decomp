# flag scratch reuse vs straight into b in StateHold (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:334-337` reuses `flag` as the
  scratch because reading the table straight into `b` and leaving `flag`
  alone costs 5 words.
- attempted_change (scratch-applied, then reverted):

```diff
     if (flag != 0) {
-        /* Reusing `flag` as the scratch is load-bearing, not leftover: reading
-           the table straight into `b` and leaving `flag` alone costs 5 words. */
-        flag = data_020a0deb[idx][0];
-        int b = flag;
+        int b = data_020a0deb[idx][0];
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func _ZN16dMgJump3DMario_c9StateHoldEv --addr 0x020c7a30 --size 0x174 \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant 5 word(s) differ, same size 0x174.
  Reproduces the comment's 5 exactly (a register-allocation shuffle at
  +0x58..0x6c). The log below elides the matching runs; all 5 mismatch
  lines are verbatim.
- log (variant):

```text
TARGET _ZN16dMgJump3DMario_c9StateHoldEv @ 0x020c7a30 size 0x174  bytes: 10402de950119fe550219fe500c0d1e50040a0e10030a0e30c01d2e70c11a0e1000050e30300000a34019fe50100d0e7000050e30130a013000053e31200000a... (372 bytes)
  2004/b56: 5 word(s) differ

--- closest: 2004/b56 (5 differ) ---
   +0x58 | 001051e0 subs r1, r1, r0          | 000051e0 subs r0, r1, r0          | <<< MISMATCH
   +0x5c | 00106142 rsbmi r1, r1, #0         | 00006042 rsbmi r0, r0, #0         | <<< MISMATCH
   +0x60 | 0c01d3e7 ldrb r0, [r3, ip, lsl #2] | 0c11d3e7 ldrb r1, [r3, ip, lsl #2] | <<< MISMATCH
   +0x64 | 180051e3 cmp r1, #0x18            | 180050e3 cmp r0, #0x18            | <<< MISMATCH
   +0x6c | 000052e0 subs r0, r2, r0          | 010052e0 subs r0, r2, r1          | <<< MISMATCH

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
