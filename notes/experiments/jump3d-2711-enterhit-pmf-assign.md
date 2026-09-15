# Pair/select/int-stores vs plain PMF assignment in EnterHit (JUMP3D-2711-02, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dMgJump3DMario_c.cpp:64-66,596-615` keeps the Pair
  two-int view with the same-on-both-arms select and int* stores for
  record 0213b078 (while every other record installs via plain
  `mState = data_...`).
- attempted_change (scratch-applied, then reverted):

```diff
-extern Pair        data_ov006_0213b078;
+extern Jump3DState data_ov006_0213b078;
```

```diff
 void dMgJump3DMario_c::EnterHit()
 {
-    int t0, t1;
-
     _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214041c, 0x40000000, kAnimSpeed, 0);
```

```diff
     func_02012718(0x1b5, mScreenX << 12);
-    t0 = data_ov006_0213b078.a;
-    /* Both arms of this select are the same word on purpose.  MEASURED:
-       collapsing it to a plain `t1 = data_ov006_0213b078.b;` costs 4 words.
-       That it is what makes mwccarm read .a before .b and keep both live is a
-       reading of the diff, not a second measurement. */
-    t1 = t0 ? data_ov006_0213b078.b : data_ov006_0213b078.b;
-    ((int *)&mState)[0] = t0;
-    ((int *)&mState)[1] = t1;
+    mState = data_ov006_0213b078;
 }
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dMgJump3DMario_c.cpp \
  --func _ZN16dMgJump3DMario_c8EnterHitEv --addr 0x020c8270 --size 0xcc \
  --version 2004/b56 --module ov006 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (identical bytes, same size 0xcc).
  The plain PMF assignment every other record uses compiles to the same
  code here too; the Pair/select/int-stores form is not load-bearing even
  though collapsing only the select (t1 = .b) costs 4 words (see
  jump3d-2711-enterhit-same-arms-select.md). Two spellings, one codegen.
- log (variant):

```text
TARGET _ZN16dMgJump3DMario_c8EnterHitEv @ 0x020c8270 size 0xcc  bytes: 10402de908d04de20020a0e3a8109fe500208de50040a0e1001091e54c0084e20121a0e32a39fdeb0000a0e388209fe5a40084e53c3084e2001093e5... (204 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
