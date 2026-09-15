# Leftover blocks not amenable to compiler experiment (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: the remaining `deslop leftovers` in
  `src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp:30-47` that no cleaner
  spelling can test -- listed here with pointers instead of forced
  experiments.
- attempted_change: none (not experimentable).

```diff
 (no change -- record only)
```

- command: none (header/manifest reads only).
- result:
  - `d_a_trs_trap.cpp:33-34` func_020393d4/func_020393c4 callback-word stores:
    HEADER-BLESSED. `include/dBgW.h:119-120` documents that call sites take
    the stock BeforeClsn callbacks' addresses and "store them via
    func_020393d4"; dBgW carries no setter, so there is no method-form
    spelling to attempt.
  - `d_a_trs_trap.cpp:35-37` SharedFilePtr+4 re-read: NOT-EXPERIMENTABLE.
    The handle's layout is unrecovered (`include/SharedFilePtr.h:15-20`
    deliberately declares no fields); no typed spelling exists to try.
  - `d_a_trs_trap.cpp:46-47` +0x418 / +0x154 / S14: NOT-EXPERIMENTABLE.
    daBookGen_c has no header (+0x418 target unknown), +0x154 has no
    recovered reader, and the S14 g_profile_* absence is a packaging
    observation, not a codegen choice.
- log: none (no compiler invocation).
- verdict: NOT-EXPERIMENTABLE
