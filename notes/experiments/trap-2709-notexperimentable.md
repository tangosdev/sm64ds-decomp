# Leftover blocks not amenable to compiler experiment (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: the remaining `deslop leftovers` in
  `src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp:30-47` that no cleaner
  spelling can test -- listed here with pointers instead of forced
  experiments.
- disposition: deferred (`completion: partial`). These are reconstruction
  findings with no cleaner spelling to measure, so they are proposed for the
  reviewer to accept as partial scope rather than closed. Durable thread:
  [PR #2709](https://github.com/tangosdev/sm64ds-decomp/pull/2709), used until a
  daTrsTrap_c class issue exists (the sibling ov063 classes carry #2720 and
  #2721). Next owner: the humanizer integrator through @andrewboudreau, who is
  coordinating this PR's integration. Each item's unblocking work is named with
  it below.
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
    spelling to attempt. Unblocked by: recovering dBgW's own setter for the
    BeforeClsn/unk_1c callback words, a dBgW header change rather than a
    daTrsTrap_c one.
  - `d_a_trs_trap.cpp:35-37` SharedFilePtr+4 re-read: NOT-EXPERIMENTABLE.
    The handle's layout is unrecovered (`include/SharedFilePtr.h:15-20`
    deliberately declares no fields); no typed spelling exists to try.
    Unblocked by: recovering SharedFilePtr's field layout, a shared-header
    change that lands the `+4` KCL_File member for every consumer.
  - `d_a_trs_trap.cpp:46-47` +0x418 / +0x154 / S14: NOT-EXPERIMENTABLE.
    daBookGen_c has no header (+0x418 target unknown), +0x154 has no
    recovered reader, and the S14 g_profile_* absence is a packaging
    observation, not a codegen choice. Unblocked by: a daBookGen_c
    (BOOK_SWITCH 0xd5) class recovery for +0x418, and a ROM-wide reader
    census for +0x154.
- log: none (no compiler invocation).
- verdict: NOT-EXPERIMENTABLE
