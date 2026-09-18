# Same-on-both-arms select vs plain load in EnterHit (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:607-612` keeps the same-on-both-arms
  select because collapsing it to a plain `t1 = data_ov006_0213b078.b;`
  costs 4 words.
- attempted_change (scratch-applied, then reverted):

```diff
     t0 = data_ov006_0213b078.a;
-    /* Both arms of this select are the same word on purpose.  MEASURED:
-       collapsing it to a plain `t1 = data_ov006_0213b078.b;` costs 4 words.
-       That it is what makes mwccarm read .a before .b and keep both live is a
-       reading of the diff, not a second measurement. */
-    t1 = t0 ? data_ov006_0213b078.b : data_ov006_0213b078.b;
+    t1 = data_ov006_0213b078.b;
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func _ZN16dMgJump3DMario_c8EnterHitEv --addr 0x020c8270 --size 0xcc \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant 4 word(s) differ, same size 0xcc.
  Reproduces the comment's 4 exactly (the .a/.b load order and register
  assignment at +0xa0..0xac). The log below elides the matching runs; all 4
  mismatch lines are verbatim.
- log (variant):

```text
TARGET _ZN16dMgJump3DMario_c8EnterHitEv @ 0x020c8270 size 0xcc  bytes: 10402de908d04de20020a0e3a8109fe500208de50040a0e1001091e54c0084e20121a0e3023ba0e32a39fdeb0000a0e388209fe5a40084e53c3084e2001093e5... (204 bytes)
  2004/b56: 4 word(s) differ

--- closest: 2004/b56 (4 differ) ---
   +0xa0 | 001090e5 ldr r1, [r0]             | 041090e5 ldr r1, [r0, #4]         | <<< MISMATCH
   +0xa4 | 040090e5 ldr r0, [r0, #4]         | 000090e5 ldr r0, [r0]             | <<< MISMATCH
   +0xa8 | 3c1084e5 str r1, [r4, #0x3c]      | 3c0084e5 str r0, [r4, #0x3c]      | <<< MISMATCH
   +0xac | 400084e5 str r0, [r4, #0x40]      | 401084e5 str r1, [r4, #0x40]      | <<< MISMATCH

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
