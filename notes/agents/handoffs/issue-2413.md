# Handoff: issue-2413

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  <https://github.com/tangosdev/sm64ds-decomp/issues/2413>, task `issue-2413`,
  stage `reconstruct` (role `producer`), queue session `prod-2413-0907`,
  harness Claude Code / Claude Opus 5.
- Source branch and previous accepted input SHA: `cpp/daBmb_c-v2`, continued from
  `875505afa15f0a09fa07d99caab0c65d1bba4799` (branch `cpp/BobOmb-tu`, the v1
  source, which is left untouched at that tip). This branch descends from it, so
  the whole v1 history is preserved; `origin/main` was merged, not rebased onto.
- Original source base SHA and installed workflow/tool SHA: base
  `b2bd6a323832d244a12e750713b1405b26a94c66`; workflow/tools
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`. The earlier original-base record
  the coordinator asked to preserve is
  `2ab05bae36a6821679a7e647e811c0a3243a3c49`; `b2bd6a323` is the *current*
  merge-base with main, not a replacement for it. Both travel.
- Separate evidence commits and required artifacts in this commit: none pinned
  separately. `notes/data/class-facts/daBmb_c.json`,
  `config/tu_manifest.d/ov102/daBmb_c.json`, `include/daBmb_c.h` and
  `src/actors/daBmb_c.cpp` are all in this commit.
- Next action, responsible role and blockers: independent verification
  (`verifier`). One blocker stands between this candidate and a green PR, and it
  is a gate defect, not a source defect — see "Blocker" below.
- Status: verified candidate for the producer stage. Every gate this producer can
  run locally is recorded below with its exit code, including the one that fails.
- Remaining uncommitted/local-only material and where it is preserved: none.
  Build logs and reports are in the worktree's gitignored `build/` and are not
  part of any commit.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daBmb_c` in `ov102`, one
  contiguous `.text` run `0x0214a96c..0x0214c748` (35 functions), plus the
  `ov102` data records `_ZTV7daBmb_c` (`0x0214e558`) and `_ZTI7daBmb_c`
  (`0x0214e508`).
- Reserved source/header/config surfaces actually touched:
  `src/actors/daBmb_c.cpp`, `include/daBmb_c.h`,
  `config/tu_manifest.d/ov102/daBmb_c.json`,
  `config/arm9/overlays/ov102/symbols.txt`,
  `config/arm9/overlays/ov102/delinks.txt`.
- Also touched, outside the reservation and deliberately narrow: this class's own
  row in `notes/data/tu-promotion-queue.tsv` (a derived line count), and
  `langmode-baseline.json`, which is restored to main's own file rather than
  changed. `include/decl_common.h`, `attribution.json`,
  `symbols/actor_renames.tsv`, `config/converted-baseline.json`,
  `config/converted-backslide-exceptions.jsonl` carry v1 edits inherited from the
  input commit and were not reconciled here; they belong to the integration lane.
- ROM observations: `ov102 0x0214e4fc` holds `"7daBmb_c\0"`; `_ZTI7daBmb_c` at
  `0x0214e508` points its `+4` word at that string and its `+8` word at
  `0x021081c0` in `ov002`, which is `dEnemyBase_c`; the vtable's `-4` header word
  points at the `_ZTI`. The factory allocates `0x400` through
  `fBase_c::operator new` and stores this vtable, so `0x400` is the class size.
  The vtable is 31 slots; nine are overridden (0, 3, 6, 9, 16, 17, 18, 19, 29) and
  the class declares no new virtual.
- Lineage evidence or structural inference: `dEnemyBase_c` base is read from the
  RTTI record, not from vtable length — `_ZTV12dEnemyBase_c` is also 31 slots, so
  the length heuristic gives the wrong answer for this class. Member offsets
  `0x110/0x144/0x300/0x364` close exactly on one another against the sizes of
  `dCcAc_c`, `dBgCh_Actr`, `ModelAnim` and `ShadowModel`.
- Hypothesized names/filenames, explicitly not recovered facts: `State1`,
  `State3`, `State4`, `State5` are coined; the cartridge proves only that
  `func_ov102_0214b03c` switches on `mState` and dispatches six bodies for 0..5.
  `daBmb_c_classInit` is a reconstructed source-style spelling (historical alias
  `BobOmb_Spawn`); the ROM symbol is the bare name and nothing more. The file path
  `src/actors/daBmb_c.cpp` is this tree's convention, not a recovered filename.
- Compiler experiments and measured barriers, this stage: the vtable declaration
  `extern int _ZTV7daBmb_c[];` was moved from the body of
  `src/actors/daBmb_c.cpp` into `include/daBmb_c.h`. The compiled object is
  byte-identical across the move under the pinned mwccarm 2004/b56 —
  `build/src/actors/daBmb_c.o` sha256
  `7f2d71307a287beb07992fb5a409d82402e6ce730c509f24808a7bd77b4820eb` before and
  after — so the langmode `extern_vtable` rise the input commit banked was an
  accounting artifact of where the declaration was spelled, not a byte
  requirement. `langmode-baseline.json` is now main's file unchanged.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 35 of 35 functions in the run
  reproduce the cartridge, and all 35 slots pass relocation-destination checking.
  Two data records, `_ZTV7daBmb_c` and `_ZTI7daBmb_c`, are byte-verified against
  `ov102`. Five further data records the TU emits are PARTIAL — the object's
  coverage is shorter than the ROM's extent for them — and none DIFFER.
- Genuine methods; remaining free-function/ABI bridges: 13 of the 35 are C++
  methods (`~daBmb_c` D1 and D0 emitted from the inline in-class body,
  `OnYoshiTryEat`, `OnTurnIntoEgg`, `OnAimedAtWithEgg`, `InitResources`,
  `CleanupResources`, `Behavior`, `Render`, and the four coined state arms
  `State1`, `State3`, `State4`, `State5`). `daBmb_c_classInit` is a member in
  every sense but keeps C linkage because the cartridge's symbol is the bare
  name. The remaining 21 stay `func_ov102_*` free functions. The wall is one of
  exactly two things and never codegen:
  - SCOPE, for `func_ov102_0214bf64` and `func_ov102_0214bd90` (state arms 0 and
    2). Both compile to the ROM's exact size as members and the TU then fails to
    LINK: as members they lose their own block-scope `extern "C"` declarations,
    and the single file-scope linkage region cannot take them because other
    members already own contradicting spellings of the same symbols — decisively
    `func_0200fc44`, which arm 2 recovered with four parameters and
    `func_ov102_0214aa18` with three. `tubuild.py verify` calls the broken form
    35/35 MATCH with clean relocations; only `mwldarm` sees it.
  - NAMING, for the other 19. The cartridge carries their addresses and no
    identifier. Three are also called from outside `ov102`
    (`0x0214ad14` from `ov002`/`ov078`, `0x0214ae1c` from `ov014`/`ov098`,
    `0x0214b384` from `ov078`), so naming those is a cross-module rename.
- Recovered layout/fields; remaining shadow structs/raw offsets: the header types
  the four subobjects at `0x110`, `0x144`, `0x300`, `0x364` and names 17 further
  fields from `0x390` up, with the padding runs explicit. Three shadow types
  survive in the TU body — `Bmb_Vec3` and `Bmb_Bf64Obj`, which are
  `func_ov102_0214bf64`'s own fifteen-field view of the object, and `Bmb_VBase`,
  an abstract dispatch shim `Render` uses to reach slot 5 of the model object at
  `+0x300`. Those three are the measured spellings the members byte-match in.
  Byte-offset casts (`(char *)this + N`) remain throughout the free-function
  half. **Counted, because "some remain" understates it:** 303 expressions of
  the form `<identifier> + 0x<literal>` survive in
  `src/actors/daBmb_c.cpp`, of which 295 displace an object handle
  (`c` ×194, `self` ×50, `s` ×13, `p` ×10, `r4` ×7, `p2` ×6, `this` ×3,
  `into` ×3, and nine further handles once or twice each); the other 8 are
  scalar arithmetic. Eight more accesses use a hex word subscript. Every one of
  those is a field this reconstruction has not recovered a name for.
- Surviving mangled-name ABI bridges — a dimension the rest of this document did
  not count. The TU reaches other modules by spelling their mangled symbols
  directly instead of calling declared C++ methods: **17 distinct mangled names
  declared at file scope** (in the leading `extern "C"` prologue) and **29
  distinct declared at block scope** across 34 declaration sites inside function
  bodies, six of them inside `daBmb_c_classInit` alone (`_ZN7fBase_cnwEj`,
  `_ZN12dEnemyBase_cC2Ev`, `_ZN7dCcAc_cC1Ev`, `_ZN10dBgCh_ActrC1Ev`,
  `_ZN9ModelAnimC1Ev`, `_ZN11ShadowModelC1Ev`). Some are byte-forced and
  documented as such in the source — `_ZN9ModelBase7SetFileEP8BMD_Fileii`
  returns a value the header declares `void`, and the two `Init` calls carry
  `Fix12<int>` by value — but most are simply not yet expressed as calls on a
  real type. They are remaining reconstruction work, not a byte defect: the
  cartridge is reproduced either way.
- Lifecycle, vtable/RTTI, initializer and data ownership: the destructor is
  inline in the header and declared first, which is what makes mwccarm emit D1
  below D0 and no D2 — the cartridge's own order. That makes `OnYoshiTryEat` the
  first out-of-line virtual the class declares, so this TU is the key-function TU
  and owns `_ZTV7daBmb_c`, `_ZTI7daBmb_c` and `_ZTS7daBmb_c`. The vptr store in
  `daBmb_c_classInit` is written by hand, because the factory has C linkage and
  is not a constructor; it addresses `_ZTV7daBmb_c + 2` so that it agrees with the
  addend-8 stores the compiler emits in the destructor. The class owns no static
  initializer.
- Attribution preserved through each move/rename: NOT yet preserved, and this is
  the one substantive piece of work this stage is leaving behind. See
  "Attribution" below.
- Remaining agreed issue scope, corrected in the integration lane because the
  list above was short: the 19 NAMING-blocked free functions and the two
  SCOPE-blocked state arms; the three shadow types; the 17 file-scope and 29
  block-scope mangled-name ABI bridges; and the 295 raw byte-offset accesses on
  an object handle. The `attribution.json` overrides were the fourth item and
  are now applied (see the integration addendum). Confirmed content of the TU as
  it stands: 21 free functions, 13 methods (11 out-of-line plus D1/D0 from the
  inline destructor), 3 shadow types. **"Promoted" is a packaging state.** This
  class is one C++ translation unit that reproduces the cartridge exactly; it is
  not a finished reconstruction, and nothing in this document should be read as
  claiming it is.

## Blocker: a known-defective gate, not a defect in this source

`tools/validate_merge.py`'s `rom_data_regressions` builds data-symbol identity as
`(module, symbol)` and takes a raw set difference, so it reads a symbol RENAME at
one unchanged address as a data LOSS. This branch retires the coined name
`_ZTV6BobOmb` in favour of the cartridge's own `_ZTV7daBmb_c`, both at
`ov102 0x0214e558`, and the gate therefore reports:

    Validation failed: ROM data verification lost 1 exact symbol(s): ov102:_ZTV6BobOmb

That is the only reason in the report. The measurement it draws on moves the
other way: ROM data verified from source goes 685 -> 686 symbols and
44,264 -> 44,276 bytes between base and head, because `_ZTI7daBmb_c` becomes
verified as well, and the differing count is unchanged at 5. The fix belongs to
<https://github.com/tangosdev/sm64ds-decomp/issues/2409>, which must land alone
and first. Two workarounds were considered and rejected as wrong: adding an alias
row for the retired name to `config/arm9/overlays/ov102/symbols.txt`, and making
anything consult `symbols/actor_renames.tsv` to satisfy the gate. Both turn the
gate green while leaving it broken.

## Attribution

The fold moves 32 of the 35 functions from their per-shard files into one TU, and
this branch does not yet carry the `path#symbol` overrides that keep their
original credit. `validate_merge` reports it as a warning (32 changed, 0 lost)
rather than a blocker, and the totals stay at 11,279 attributed / 0 unattributed,
but the per-symbol credit currently moves to the promoter.

`attribution.json` is integration-lane state and is not reserved to this task, so
the producer recorded the rows here rather than applying them. **They are applied
now, in the integration lane** — see the addendum at the end of this document.
They belong in the `overrides` map, in the same `path#symbol` form the tree
already uses for `src/game/actors/d_a_obj_abuku.cpp`:

| symbol | original credit |
|---|---|
| `_ZN7daBmb_cD1Ev` | tangosdev |
| `_ZN7daBmb_cD0Ev` | tangosdev |
| `_ZN7daBmb_c16OnAimedAtWithEggEv` | tangosdev |
| `_ZN7daBmb_c13OnTurnIntoEggER6Player` | tangosdev |
| `_ZN7daBmb_c16CleanupResourcesEv` | tangosdev |
| `_ZN7daBmb_c6RenderEv` | tangosdev |
| `_ZN7daBmb_c8BehaviorEv` | lunavyqo |
| `_ZN7daBmb_c13InitResourcesEv` | tangosdev |
| `_ZN7daBmb_c6State1Ev` | tangosdev |
| `_ZN7daBmb_c6State3Ev` | tangosdev |
| `_ZN7daBmb_c6State4Ev` | tangosdev |
| `_ZN7daBmb_c6State5Ev` | tangosdev |
| `daBmb_c_classInit` | tangosdev |
| `func_ov102_0214aa18` | tangosdev |
| `func_ov102_0214ad14` | tangosdev |
| `func_ov102_0214ad40` | tangosdev |
| `func_ov102_0214ae1c` | tangosdev |
| `func_ov102_0214b03c` | tangosdev |
| `func_ov102_0214b128` | tangosdev |
| `func_ov102_0214b248` | lunavyqo |
| `func_ov102_0214b384` | ruspecial |
| `func_ov102_0214b3b8` | tangosdev |
| `func_ov102_0214b3f0` | tangosdev |
| `func_ov102_0214b444` | tangosdev |
| `func_ov102_0214b53c` | lunavyqo |
| `func_ov102_0214b988` | tangosdev |
| `func_ov102_0214baa0` | aitddlabs |
| `func_ov102_0214bd20` | tangosdev |
| `func_ov102_0214bd90` | tangosdev |
| `func_ov102_0214beb4` | tangosdev |
| `func_ov102_0214bf64` | tangosdev |
| `func_ov102_0214c0b8` | tangosdev |

Each key is that symbol prefixed with `src/actors/daBmb_c.cpp#`. The three
functions of the 35 not listed — `func_ov102_0214ab1c`, `func_ov102_0214bc20` and
`_ZN7daBmb_c13OnYoshiTryEatEv` — already credit the promoter and need no row.

## Proof

Every command below was run in the wired worktree `C:/tmp/sm64ds-bmb2413` on this
branch, against the pinned mwccarm 2004/b56 and the extracted retail ROM, at this
commit's tree unless a base tree is named. Base is `origin/main` at
`88dbe66db2cb3f0cd1dc704e9f2775eb37aea646`.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom` — exit 0. Module
  fidelity 106/106 exact, 100.000000% of compared bytes; 11,191 source-built
  functions reproducing and 0 mismatching; 20 source-owned data claims
  reproducing, 0 mismatching. The same command at the base tree is also exit 0
  with the identical function totals, so nothing regressed.
- Whole-TU byte and relocation verification:
  `python tools/tubuild.py verify ov102/daBmb_c` — exit 0. 35/35 MATCH,
  `objisolate` relocation type/addend check clean, relocation destinations clean,
  and all 35 sections in the expected ROM-ascending emission order.
- Explicit function/consumer relocation checks:
  `python tools/pr_linkcheck.py --base origin/main -j16` — exit 0. Two changed
  headers fan out to 926 further sources; 927 files and 2,055 slots checked;
  `src/actors/daBmb_c.cpp` reports ok on all 35 slots. Tally 2,050 VERIFIED,
  1 DRAFT, 4 BLIND. None of those five is a file this branch changes, none of
  them references any symbol this branch renames, and all five are in modules
  this branch does not touch (`ov003`, `ov100`, `ov002`, `ov007`, `ov089`). Their
  verdicts at the base tree were not measured, so "pre-existing" is an inference
  from those two facts, not a measurement.
- `python tools/prepush_linkcheck.py --range origin/main..HEAD` — exit 0,
  926 checked, 0 blocking. Note its coverage limit: it keys on the one-symbol
  filename convention, so a promoted multi-function TU is reported NO-SYM and is
  NOT checked by this tool. `pr_linkcheck` above is what actually covers it.
- Complete emitted TU and data/metadata checks:
  `python tools/romdata_check.py --files src/actors/daBmb_c.cpp` — exit 0.
  11 data symbols emitted; VERIFIED 6 (180 bytes), PARTIAL 5 (44 bytes),
  DIFFERS 0, UNNAMED 0. The verified set includes `ov102:_ZTV7daBmb_c` and
  `ov102:_ZTI7daBmb_c`.
- Shared-header consumer expansion:
  `python tools/affected_src.py include/daBmb_c.h` — exit 0, one consumer,
  `src/actors/daBmb_c.cpp`, which the full-ROM build compiles. The wider
  `include/decl_common.h` fan-out is covered by the 927-file `pr_linkcheck` run.
- Object identity across the header move: `build/src/actors/daBmb_c.o` compared
  before and after with `cmp` — identical, sha256
  `7f2d71307a287beb07992fb5a409d82402e6ce730c509f24808a7bd77b4820eb`.
- Static gates on the MERGE TREE:
  `python tools/premerge_check.py <candidate>` — exit 0. converted-ratchet,
  dead-references, duplicate-sources, header-offsets, langmode-ratchet,
  layout-check, src-tu-refs and source-coverage all pass on both base and merge;
  nothing goes green to red. It reports source-coverage entries 9194 -> 9160
  (-34), which is the promotion's consolidation, with bytes flat. Run against
  `origin/main` at `88dbe66db2cb`, on the merge tree for this branch's last
  source commit; only this handoff document was added afterwards.
- `python tools/check_src_tu_compiles.py`, via the pre-push hook — exit 0,
  154/154 translation units compile.
- Other repository gates, each run individually at this tree and each exit 0:
  `port_refcheck` (402 references, none stale),
  `langmode_audit --check langmode-baseline.json` (ratchet PASS),
  `check_rename_ledger`, `check_tubuild_conflicts`, `check_python_names`,
  `check_data_definitions`, `check_references` (42 unresolved against a 45
  baseline; no new unresolvable references), `check_src_tu`,
  `check_duplicate_sources`, `check_profile_campaign`, `check_layout_free`,
  `check_dead_references`, `cpp_tu_state --check` ("state note is current"), and
  `python -m unittest tools.test_srcpath tools.test_bytegate` (74 tests, OK).
- `python tools/check_decl_return_types.py` — exit 1, and NOT caused by this
  branch: all three findings are `dScMgBase_c` methods whose class header and
  `decl_common.h` disagree. No file this branch touches is involved, and no CI
  workflow runs this tool.
- `python tools/queue_audit.py` — after this commit, one row still disagrees and
  it is not this class's: `dScMgAmida_c` (shard_count 25 -> 26, total_lines
  2001 -> 2148, no-legacy-source 2 -> 1), drift that arrived from main. The
  `daBmb_c` row was corrected here by hand rather than by `--write`, so that
  another class's row is not banked from this branch.
- Private validation: NOT run. The private validator is not available to this
  producer. What WAS run is the policy tool the validator calls,
  `python tools/validate_merge.py` with locally produced base and head ROM
  reports, this branch's `pr_linkcheck` JSON and a `port_refcheck` JSON — status
  **Failed**, with exactly one reason, the `rom_data_regressions` defect
  described above. Its warnings are the 32 attribution changes, "35 address
  ranges left the byte-verified set while enrolled totals held steady" (the
  per-function delink entries folding into one TU entry), and the 4 unresolved
  relocations already accounted for. This was measured on this branch against
  `origin/main` locally; it is not a run on a real PR head/base, and no PR is
  open yet.

A log generated after this commit belongs in separately recorded evidence.

## Integration addendum (stage `integrate`, session `integ-2413-0907`)

Written in the integration lane, on top of the accepted candidate. It records
what the integrator changed, what it measured, and the corrections it made to
the text above. The producer's and verifier's records stand as written.

### Composition

- Accepted candidate: `123ea08645d12a52f43a5cdc70b6c019ac1301dd`, branch
  `cpp/daBmb_c-v2`, which is left untouched at that tip.
- Composed by MERGING `origin/main` into a fresh branch off the candidate — never
  rebased. Main moved during the run; the last base composed against is
  `9e630dab3f7b65210a18ce76fe718f38dd20af2b`. Both merges were clean, no
  conflicts, no file resolved by taking one side whole.
- Wired integration worktrees: `C:/tmp/sm64ds-integ2413` (head) and
  `C:/tmp/sm64ds-integ2413-base` (base at `9e630dab3`). Reports and logs live in
  their gitignored `build/`.

### Integration-lane rows touched, and only these

The three concurrent class integrations share these files, so this branch keeps
its edits to this class's own rows and reformats nothing:

| file | this branch's change |
|---|---|
| `attribution.json` | +32 lines, 0 removed, one hunk, inserted beside this class's existing retired-shard rows — the same placement the tree already uses for `src/actors/daDgr_c.cpp#…` and `src/actors/dScGameOver_c.cpp#…` |
| `config/converted-baseline.json` | 6 whole-file identities out, 6 `path#symbol` identities in; `count` stays 2696 and the list stays sorted and duplicate-free |
| `config/converted-backslide-exceptions.jsonl` | +6 rows appended, one per retired one-function shard of this class |
| `symbols/actor_renames.tsv` | 7 ov102 rows re-spelled in place, +14 appended |
| `include/decl_common.h` | 1 vtable declaration re-spelled, 1 now-member declaration removed |

`config/match_attempts.jsonl` and `config/match_provenance.jsonl` are untouched.

### Attribution: the 32 rows are applied

Derived independently rather than copied: `tools/validate_merge.py` on the
composed tree against `origin/main` reports **32 changed, 0 lost**, and each
changed address maps through `config/arm9/overlays/ov102/symbols.txt` to a head
symbol. That derivation reproduces the producer's table exactly — same 32
symbols, same 32 authors, **zero mismatches** — so the table was applied verbatim
to `attribution.json` under the `src/actors/daBmb_c.cpp#<symbol>` key form.

Credit preserved: **lunavyqo ×3** (`_ZN7daBmb_c8BehaviorEv`,
`func_ov102_0214b248`, `func_ov102_0214b53c`), **ruspecial ×1**
(`func_ov102_0214b384`), **aitddlabs ×1** (`func_ov102_0214baa0`),
tangosdev ×27. `func_ov102_0214ab1c`, `func_ov102_0214bc20` and
`_ZN7daBmb_c13OnYoshiTryEatEv` already credit andrewboudreau at the base and get
no row. After the rows are applied the same tool reports **0 changed, 0 lost**.

### What the integrator measured

| check | tree | exit | result |
|---|---|---|---|
| `rombuild.py -j16 --no-rom` | head | 0 | 106/106 modules exact; 11,191 source-built functions reproducing, 0 mismatching; ROM data 686 verified / 44,276 bytes / 5 differ |
| `rombuild.py -j16 --no-rom` | base `9e630dab3` | 0 | 106/106 exact; 11,191 reproducing, 0 mismatching; 685 verified / 44,264 bytes / 5 differ |
| `build/src/actors/daBmb_c.o` | head | — | sha256 `7f2d71307a287beb07992fb5a409d82402e6ce730c509f24808a7bd77b4820eb`, the value the producer and verifier both measured |
| `pr_linkcheck.py --base origin/main -j16` | head | 0 | 2 changed headers fan out to 926 further sources, 927 files checked; `src/actors/daBmb_c.cpp` ok on all 35 slots; 5 non-`ok` rows, the same 1 DRAFT + 4 BLIND files the verifier measured at the base |
| `port_refcheck.py` | head | 0 | 423 references checked, 0 stale |

Both ROM reports were produced by `tools/romdata_check.py` blob `346b04c2`, which
is `origin/main`'s copy — the post-`1c93d2663` one. Their `VERIFIED`, `PARTIAL`
and `DIFFERS` rows all carry `module`/`addr`/`bytes`, so
`validate_merge._data_anchor` resolves an address for every one of them and the
ROM-data comparison is anchored on the cartridge address, not on the symbol name.
A report generated by a pre-`1c93d2663` `romdata_check.py` emits `{module,
symbol}` only, `_data_anchor` returns `None`, the comparison silently falls back
to names, and this class's `_ZTV6BobOmb` -> `_ZTV7daBmb_c` rename is read as a
lost data symbol. That is why the base report is regenerated here rather than
reused.

### Corrections to the record above

1. The original-base record `2ab05bae36a6821679a7e647e811c0a3243a3c49` is now
   carried alongside the current merge-base.
2. The remaining-work list was short. It omitted the surviving mangled-name ABI
   bridges (17 distinct at file scope, 29 distinct at block scope across 34
   declaration sites, 6 of them in `daBmb_c_classInit`) and the raw byte-offset
   accesses (295 on an object handle, of 303 `<name> + 0x<literal>` expressions,
   plus 8 hex word subscripts). Both are counted in place above.
3. Confirmed content, unchanged: 21 free functions, 13 methods, 3 shadow types.
4. "Promoted" is a packaging state. This class ships as one byte-exact C++
   translation unit; the reconstruction is not finished, and the issue stays open
   for the work item 2 names.
