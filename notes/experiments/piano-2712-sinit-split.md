# __sinit split vs merging __sinit_ov063_0211e5fc into the TU (PIANO-2712-02, PR #2712)

- tested_commit: b19392a93 (cleanup-dapiano at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daPiano_c/d_a_piano.cpp:33-34` keep
  `__sinit_ov063_0211e5fc` split in
  `src/game/actors/daPiano_c/__sinit_ov063_0211e5fc.c` because delinks
  places one range per file and nothing owns `.text` plus `.init`.
  Structural/packaging claim: the merge was attempted twice (naive, then
  adapted) and every failure mode recorded.
- attempted_change, naive (scratch-applied, then reverted): move the sinit
  body plus its externs verbatim to the end of `d_a_piano.cpp` (real symbol
  names, `SharedFilePtrLayout` decls, `()` prototypes, `struct Quad`
  `data_ov063_0211efbc`).
- command (naive):

```text
.venv/bin/python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func __sinit_ov063_0211e5fc --addr 0x0211e5fc --size 0x100 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result (naive): does not compile. `gPianoModelFile` etc. are macros for
  `data_ov063_0211ef80/...` (`include/MadPianoResources.h:46-48`), so the
  sinit's `SharedFilePtrLayout` decls redeclare this file's `SharedFilePtr`
  decls; and the `.c` file's `()` prototypes mean unspecified args in C but
  zero args in C++, so every sinit call mismatches.
- log (naive, direct compile, verbatim):

```text
returncode: 1
---stdout---
mwccarm.exe: warning: Environment variable 'MWCIncludes' not found
src\game\actors\daPiano_c\d_a_piano.cpp:600: identifier 'data_ov063_0211ef80' redeclared
src\game\actors\daPiano_c\d_a_piano.cpp:600: was declared as: 'SharedFilePtr'
src\game\actors\daPiano_c\d_a_piano.cpp:600: now declared as: 'SharedFilePtrLayout'
src\game\actors\daPiano_c\d_a_piano.cpp:602: identifier 'data_ov063_0211ef88' redeclared
src\game\actors\daPiano_c\d_a_piano.cpp:602: was declared as: 'SharedFilePtr'
src\game\actors\daPiano_c\d_a_piano.cpp:602: now declared as: 'SharedFilePtrLayout'
src\game\actors\daPiano_c\d_a_piano.cpp:604: identifier 'data_ov063_0211ef90' redeclared
src\game\actors\daPiano_c\d_a_piano.cpp:604: was declared as: 'SharedFilePtr'
src\game\actors\daPiano_c\d_a_piano.cpp:604: now declared as: 'SharedFilePtrLayout'
src\game\actors\daPiano_c\d_a_piano.cpp:617: function call 'func_02017acc(SharedFilePtr *, int)' does not match
src\game\actors\daPiano_c\d_a_piano.cpp:617: 'func_02017acc()'
src\game\actors\daPiano_c\d_a_piano.cpp:618: function call 'func_020731dc(SharedFilePtr *, {lval} int[], {lval} int[])' does not match
src\game\actors\daPiano_c\d_a_piano.cpp:618: 'func_020731dc()'
src\game\actors\daPiano_c\d_a_piano.cpp:619: function call 'func_02017b4c(SharedFilePtr *, int)' does not match
src\game\actors\daPiano_c\d_a_piano.cpp:619: 'func_02017b4c()'
src\game\actors\daPiano_c\d_a_piano.cpp:620: function call 'func_020731dc(SharedFilePtr *, {lval} int[], {lval} int[])' does not match
src\game\actors\daPiano_c\d_a_piano.cpp:620: 'func_020731dc()'
src\game\actors\daPiano_c\d_a_piano.cpp:621: function call '_ZN13SharedFilePtr9ConstructEj(SharedFilePtr *, int)' does not match
src\game\actors\daPiano_c\d_a_piano.cpp:621: '_ZN13SharedFilePtr9ConstructEj()'
src\game\actors\daPiano_c\d_a_piano.cpp:622: function call 'func_020731dc(SharedFilePtr *, {lval} int[], {lval} int[])' does not match
src\game\actors\daPiano_c\d_a_piano.cpp:622: 'func_020731dc()'

---stderr---

Errors caused tool to abort.
```

- attempted_change, adapted (scratch-applied, then reverted): same move but
  reusing this file's own `SharedFilePtr` decls (same 8-byte layout),
  C-linkage prototypes with real parameters, and `extern "C"` on the
  definition (else the name mangles and the symbol is not found).
- result (adapted): compiles, but the sinit body differs by 19 words at the
  same 0x100 size; every mismatch is in +0x5c..+0xac, the `Quad` struct-copy
  tail (the call prologue +0x00..+0x58 is identical). Baseline (sinit in its
  own `.c` file) MATCH.
- log (adapted per-function, summary verbatim):

```text
TARGET __sinit_ov063_0211e5fc @ 0x0211e5fc size 0x100  bytes: 00402de904d04de2ac009fe5ac109fe52ee5fbeba0009fe5a4109fe5a4209fe5ee52fdeba0009fe5a0109fe547e5fbeb94009fe598109fe598209fe5e752fdeb... (256 bytes)
  2004/b56: 19 word(s) differ

--- closest: 2004/b56 (19 differ) ---
```

- mechanism probe (why the tail differs): the enrolled sinit compiles as C
  (`-lang c99`) while this TU compiles as C++ (`-lang c++`). A minimal
  scratch probe of just the four `Quad` copies, compiled both ways
  (`/tmp`, not committed), differs:

```text
C   : 70402de954309fe554209fe554109fe554009fe5006093e550409fe5045093e500e092e504c092e5003091e5042091e5001090e5040090e5006084e5045084e508e084e50cc084e5103084e5142084e5181084e51c0084e57040bde81eff2fe10000000000000000000000000000000000000000
C++ : 70402de954c09fe554209fe554109fe554009fe500609ce550309fe504509ce5004092e504e092e500c091e5042091e5001090e5040090e5006083e5045083e5084083e50ce083e510c083e5142083e5181083e51c0083e57040bde81eff2fe10000000000000000000000000000000000000000
identical: False
```

  Same register-allocation shape as the merged-sinit mismatch (`str
  r2,[r3]` vs `str r1,[r2,...]`). The sinit body is language-lane
  sensitive: a verbatim move into a `.cpp` TU cannot reproduce the `.c`
  codegen.
- packaging half (adapted object): the merged TU compiles to 16 `.text`
  sections totalling 0xccc bytes against the single licensed range
  `0x0211d5f4..0x0211e1c0` (0xbcc) -- an exact 0x100 overflow -- and the
  multi-symbol path refuses:

```text
total .text = 0xccc (3276 bytes)
d_a_piano.cpp delinks range: 0x0211d5f4..0x0211e1c0 = 0xbcc (3020 bytes)
---plan_many(15)---
unlicensed content in text-only multi-symbol object: section[5] .text size 0x100 defines ['__sinit_ov063_0211e5fc']
```

  Honest scope note: multi-range delinks entries spanning `.text`+`.init`
  exist (ov023 `d_a_obj_fm_battan`, ov044 `d_a_obj_kb1_billboard`, ov100
  `d_a_star_gate`), but only on promoted TUs with manifests and
  section-partition isolation. So the packaging half is promotion scope,
  while the compile conflict and the `-lang` codegen divergence are hard
  walls for the move itself. The file's comment names only the packaging
  wall; the experiment shows two more in front of it.
- verdict: CONSTRAINT SUPPORTED
