# Handoff: pr2231-rescue-ov022-ov013-batch2

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: no GitHub issue and no v2 queue
  task exist for these classes; this is batch 2 of the rescue of
  https://github.com/tangosdev/sm64ds-decomp/pull/2231 (closed unmerged, branch
  `cpp/promote-da-obj-lava` @ `8015f733e`, 17 promotions / 135 functions), stage
  `reconstruct` + `integrate-candidate`, session `rescue-lava-0907`, harness
  Claude Code (`claude-fable-5-1`). Enqueuing is a coordinator act and was not
  done; nothing was claimed in `classqueue.py v2`. Batch 1 landed as
  `ecc97391e` (#2435).
- Source branch and previous accepted input SHA: branch
  `cpp/promote-ov022-ov013-batch2`, content commit `851dc888c`, merged with
  `origin/main` `c27d8ea38` (`--no-ff`, no conflicts) as `8be0f4c0c`, verified
  independently as `450a93a65` (PASS), then restacked by merging `origin/main`
  `115cb5d74` (`--no-ff`) as `31499797c`: two conflicts, the ledger tail
  (main's rows first, then this batch's) and the C++ TU state note
  (regenerated, not hand-resolved). There is
  no accepted input: the rescued branch was NOT merged; its content for these
  classes was re-derived file by file on top of main. `cpp/promote-da-obj-lava`
  is untouched on the remote and must not be deleted: it still holds the other
  classes.
- Original source base SHA and installed workflow/tool SHA: built on
  `ecc97391e` (main after batch 1), then `origin/main` `c27d8ea38` merged in
  (never rebased), then `115cb5d74`. Workflow/tool revision
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`. On BOTH halves used for the ROM
  reports (`115cb5d74` and this branch) `tools/romdata_check.py` is blob
  `346b04c271b1` and `tools/validate_merge.py` is blob `a4d3b362ff58`, i.e.
  both post-#2425.
- Separate evidence commits and required artifacts in this commit: no separate
  evidence commit. Stage artifacts: `src/actors/daObjFl_London_c.cpp`,
  `src/actors/daObjClock_c.cpp`, `src_tu/actors/daObj_volcanoCannon_c.cpp`
  (shadow, text-verified), their headers in `include/`, and the three manifests
  under `config/tu_manifest.d/ov022/` and `config/tu_manifest.d/ov013/`.
- Next action, responsible role and blockers: independent verification of this
  commit (verifier), then integration. One class is HELD, not blocked by this
  batch: see "daObj_volcanoCannon_c" below; a separate lane owns the dsd extent
  question.
- Status: verified candidate (local proof below; the private validator has not
  run on this commit).
- Remaining uncommitted/local-only material and where it is preserved: the other
  classes of PR #2231 (heart on `cpp/promote-ov002-heart-tu`, still WIP;
  ov045/ov010/ov012/ov026/ov030/ov033/ov062 on the original branch only). Gate
  logs live in the worktree's ignored `build/`.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: two promotions and one rename.
  - `daObjFl_London_c` (was coined `LavaBridge`; base `dBgActor_c`, size
    0x320): text `ov022:[0x02111980,0x02111cac)` (8 functions, 812 bytes) and
    `.data` `ov022:[0x02113f2c,0x02113ff0)` (196 bytes: `_ZTI`, `_ZTS`,
    `g_profile_FL_LONDON`, the 32-slot `_ZTV` whose address point is
    0x02113f70). Promoted from eight one-function shards into
    `src/actors/daObjFl_London_c.cpp`.
  - `daObjClock_c` (was coined `ClockPaintingHandShort`; base `dActor_c`, size
    0x128): text `ov013:[0x021113bc,0x021116ac)` (9 functions, 752 bytes) and
    `.data` `ov013:[0x021121a4,0x0211227c)` (216 bytes: `_ZTI`, `_ZTS`,
    `g_profile_CLOCK_LONG`, `g_profile_CLOCK_SHORT`, the 31-slot `_ZTV` at
    0x02112200). Promoted from nine shards into `src/actors/daObjClock_c.cpp`.
    One class, two registry profiles: the byte-identical factories keep
    distinct names (`daObjClock_c_classInit_CLOCK_LONG` / `_CLOCK_SHORT`)
    because two C-linkage definitions cannot share one name. Its already
    promoted in-overlay sibling `daObjClockHuriko_c` was the style oracle.
  - `daObj_volcanoCannon_c` (was coined `VolcanoFire`; base `dActor_c`, size
    0x11c): RTTI rename of the six shards, header, `symbols.txt`, `decl_common`
    and ledger rows; the ten-function TU is carried as a text-verified shadow in
    `src_tu/` (manifest status `text-verified`). Its PROMOTION IS HELD (below).
    The promotion queue carries it as the multi-class row
    `daObj_volcanoCannon_c+daObjFlMaruta_c`; that row stays
    `already_promoted=no`.
- Reserved source/header/config surfaces actually touched:
  - `config/arm9/overlays/ov022/symbols.txt`, `ov013/symbols.txt`: coined
    method, destructor and vtable names replaced by the ROM RTTI spellings
    (`_ZN16daObjFl_London_c*`, `_ZN21daObj_volcanoCannon_c*`,
    `_ZN12daObjClock_c*`); the pre-existing duplicate `_ZTV21daObj_volcanoCannon_c`
    / `_ZTV12daObjClock_c` rows that main carried beside the coined ones are
    the surviving single rows; four interior phantom rows inside the two
    promoted data runs removed (`data_ov022_02113f78`, `data_ov013_021121b8`,
    `data_ov013_021121f8`, `data_ov013_02112258`), following the landed shape
    of the `daObjKm3_Kuruma_c` (#2119, 8 rows) and `daObjKm1_Dorifu_c` (#2120,
    6 rows) promotions. The removals are load-bearing, not tidying: with
    `data_ov022_02113f78` present, dsd declares `_ZTV16daObjFl_London_c` as
    an 8-byte symbol and hands the remaining 120 bytes of the one 128-byte
    table to the phantom, which the compiled TU cannot define (measured by
    the independent verifier of `450a93a65`); with the row present the
    linkcheck's `dsd check symbols` also reports it as the one NEW error.
    The row inside the held volcano vtable (`data_ov022_02114498`) stays as
    main has it. NO row was added.
  - `config/arm9/overlays/ov022/delinks.txt`, `ov013/delinks.txt`: the shard
    entries folded into one `complete` entry per promoted TU (by
    `tu_promote.py`).
  - `include/decl_common.h`: renamed declarations; the duplicate
    `_ZTV21daObj_volcanoCannon_c` / `_ZTV12daObjClock_c` lines removed.
  - `include/daObjFl_London_c.h`, `include/daObjClock_c.h`,
    `include/daObj_volcanoCannon_c.h`: the rescued class headers. London and
    clock carry an inline `virtual ~C() {}` (what makes mwccarm emit the
    retail D1/D0 pair with no D2); volcano's was ALREADY inline on main
    because its D1/D0 shards are forcing helpers, and it stays so.
  - `include/daObjClockHuriko_c.h` (comment), `notes/data/*.tsv/json`,
    `notes/plan-tu-merge-queue.md`, `symbols/actor_renames.tsv` (+34 present
    tense rows, 13 rewritten by `class_rename.py`), `attribution.json` (+18
    overrides placed beside the batch's own retired-shard rows),
    `config/converted-baseline.json` (12 shard identities rewritten to
    `TU#symbol`).
  - `notes/data/tu-promotion-queue.tsv`: regenerated with
    `queue_audit.py --write`; the diff is exactly six rows -- the three ov002
    rows #2435 landed without refreshing (`daObjFire_c`,
    `daObjLava_c+daObjLava_c`, `daSCoin_c`; main is red on `--check` by itself
    for them), the two promoted here, and the volcano row (shard count 10 to
    11, lines 180 to 203). `--check` is clean afterwards.
  - `notes/data/class-build-worklist.tsv`: the "header uses pre-rename name"
    note dropped on exactly the four rows whose header now carries the ROM
    name (`daObjLava_c`, `daObjFire_c`, `daObjClock_c`,
    `daObj_volcanoCannon_c`); nothing else touched.
  - `notes/cpp-tu-current-state.md`: regenerated with
    `cpp_tu_state.py --write-note`, not hand-merged.
- ROM observations: `_ZTS16daObjFl_London_c` at ov022:0x02113f38,
  `_ZTS21daObj_volcanoCannon_c` at ov022:0x02114440, `_ZTS12daObjClock_c` at
  ov013:0x021121b0 are the ROM's own names for these classes (the RTTI ruling
  applies). `build/rtti_vtables.json` (ROM walk) gives `dActor_c` 31 slots,
  `dBgActor_c` 32, clock 31, volcano 31, London 32, which the headers match.
- Lineage evidence or structural inference: unchanged from main's headers.
- Hypothesized names/filenames, explicitly not recovered facts: the two clock
  factory names are coined (`_CLOCK_LONG` / `_CLOCK_SHORT` after their
  profiles); `func_ov022_02111a1c` and `func_ov013_02111430` (the state
  helpers) keep their address names.
- Compiler experiments and measured barriers:
  - Baseline ordering: `linkcheck --baseline` builds the working tree, so a
    header whose destructor is already inline makes the still-enrolled D0/D1
    shards fail with `object 'X::~X()' redefined` (a compile refusal, not a
    link one). Order used: header declared-and-undefined, baseline, then the
    inline body and the shard fold in the same step. Volcano is the opposite
    case -- its shards are forcing helpers that need the inline body -- and
    porting it out-of-line broke the ov022 baseline (ov022 linked 64 bytes
    short, first missing symbol its D0 at 0x02112610); restoring the inline
    body fixed it.
  - daObj_volcanoCannon_c, the HELD promotion: on the shipped tree `tubuild
    linkcheck` refuses in `[2/8] dsd delink` -- `Last symbol
    'data_ov022_02114498' in section '.data' of file
    'src_tu/actors/daObj_volcanoCannon_c.cpp' has the range
    0x02114498..0x02114500 but is not contained within the file's section
    range (0x02114434..0x021144f8)`. With that interior row removed the same
    refusal names `_ZTV21daObj_volcanoCannon_c` (range
    0x0211447c..0x02114500) instead: dsd refuses either way, and the row's
    only effect is on which symbol overruns (its presence matters solely to
    `romdata_check`'s exact grade, see Proof). The vtable (storage
    0x02114474, 8-byte preamble + 31 slots, ends 0x021144f8) is the last
    `.data` object in ov022; the ROM's 0x021144f8..0x02114500 are eight zero
    bytes of `.data` end padding (`align:32`, the overlay file ends at
    0x02114500) and the next `symbols.txt` row is the `.bss` at 0x02114500.
    dsd, like `romdata_check`, sizes an unsized last symbol up to the next
    row, past the section end. PR #2231 solved this by adding a bounding row
    (`data_ov022_021144f8`) at an address main never carried, and #2120 did
    the same once (`data_ov043_021125dc`), so there is precedent -- the hold
    does not rest on its absence. It rests on the argument: a bounding row
    declares alignment padding to be a ROM object, and the honest fix is for
    both tools to stop sizing a section's last symbol past the section end.
    That is a tool change in its own lane; the class waits for it.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 27/27 functions MATCH under
  mwccarm 2004/b56 from the three TUs (`tubuild.py verify`: London 8/8, clock
  9/9, volcano 10/10; objisolate clean, reloc destinations clean, ROM-ascending
  emission order). Re-derived "not already landed" claim, keyed on blob hash
  and `(module, ROM address)` rather than paths: no blob of any of the three
  sources exists on `origin/main` and 27/27 functions are in no manifest there;
  the coined shards were present (21 shard files, now deleted; named by
  basename in `git show --stat`).
- Genuine methods; remaining free-function/ABI bridges: every vtable slot the
  ROM dispatches is a real member; the state helpers and the factories stay
  C-linkage free functions as on main.
- Recovered layout/fields; remaining shadow structs/raw offsets: main's headers
  (plus the rescued class bodies); London's Behavior still reads five fields
  through a local view struct, as the shard did.
- Lifecycle, vtable/RTTI, initializer and data ownership: each promoted TU
  emits its own `_ZTI`/`_ZTS`/`_ZTV` and profile descriptor(s) inside its
  `.data` claim (London 196 bytes, clock 216 bytes, both IDENTICAL to retail);
  the inherited base RTTI copies are `compiler_only_output` deadstrip-data
  policies, as in the landed ov002 batch.
- Attribution preserved through each move/rename: NOT preserved in the
  contributions chart -- `git` records the shard-to-TU folds as delete + add,
  and the `path#symbol` overrides added by `tu_promote.py` are inert there.
  `validate_merge` reads `Contributor credit 0 added, 0 changed, 0 lost` only
  because five overrides were set back to the shards' contributor
  (`tangosdev`) and one added for the renamed volcano D1 shard.
- Remaining agreed issue scope: the other classes of PR #2231, and volcano's
  promotion once the extent question is answered.

## Proof

Every command below was run in this worktree on the tree this document
describes (content `851dc888c` merged with `origin/main` `115cb5d74` as
`31499797c`; the figures below are the re-run on that restacked head, and
every one that differs from the verified `450a93a65` run is marked MOVED),
with the pinned mwccarm 2004/b56 and the extracted retail ROM
wired in. "Green" means exit 0 and the quoted verdict; nothing here is
inherited from PR #2231's own manifests or logs.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom --report-json
  build/head-romG.json --data-json build/head-dataG.json`, exit 0. `module
  fidelity: 106/106 exact, 100.000000%`, `source-built functions: 11,192`,
  `mismatching: 0`, `source-owned data claims: 25 (reproducing 25, mismatching
  0)`, `ROM-build analysis: PASS`, `ROM data from source: 704 verified, 223
  partial, 4 differ` (MOVED from 702/221/4: main landed #2437 and #2442 in
  between). The same command at `origin/main` `115cb5d74` in a second wired
  worktree gives the base report (`106/106`, 23/23 data claims, `697
  verified, 221 partial, 4 differ`; MOVED from 695/219/4 for the same
  reason). The batch's own delta is unchanged: +7 exact, +2 partial, 0 differ.
- Explicit function/consumer relocation checks:
  - `python tools/tubuild.py verify <id>` for the three ids: 8/8, 9/9, 10/10
    MATCH, objisolate clean, reloc-destinations clean, emission order as
    expected, exit 0 each.
  - `python tools/linkcheck.py --name <sym> --c src/actors/<Class>.cpp
    --module <ov> --addr ... --size ...` for all 17 promoted functions
    (addresses and sizes from the manifests): 17 VERIFIED, 0 BLIND, 0 other,
    exit 0 each (`build/G-lc-by-symbol.log`).
  - `python tools/validate_merge.py --base 115cb5d74 --head HEAD
    --base-rom-report ... --head-rom-report build/head-romG.json --out
    build/vmG.json`: exit 0, `status Passed`; byte-verified functions
    11,164/11,347 (+0); `Contributor credit 0 added, 0 changed, 0 lost`;
    `Relocation check: 0 checked; no affected slots`; `ROM data reproduced
    from source: 704 exact, 223 partial, 4 differ` (base 697/221/4). Its
    warning that 17 address ranges "left the byte-verified set while enrolled
    totals held steady" is the 17 shard ranges being replaced by two whole-TU
    ranges. An earlier run with the volcano interior row removed FAILED with
    `ROM data verification lost 1 exact symbol(s): ov022:_ZTV11VolcanoFire`:
    without that row `romdata_check` extends the vtable's ROM extent to the
    next row (0x84 to the `.bss`, i.e. into the padding) and scores the 124
    emitted bytes PARTIAL, so the rename had no exact arrival to cancel its
    departure. Keeping main's row for the held class restores the base
    extent and the symbol verifies exact under its ROM name.
- Complete emitted TU and data/metadata checks:
  - `python tools/tubuild.py linkcheck --baseline --module <ov> -j16 --clean`
    then `python tools/tubuild.py linkcheck <id> -j16` on each PROMOTED TU
    (final tree): both `SCRATCH-DATA-VERIFIED`; TU .text and .data ranges
    IDENTICAL (London 812+196 bytes, clock 752+216); 12 and 14 LICENSED
    symbols, 0 COLLIDES, 0 HOMELESS; `106/106 exact`; scratch ROM sha256
    `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`,
    IDENTICAL to the source-independent stock control. Caveat the tool itself
    prints: `dsd check symbols --fail` has 9 pre-existing errors that the
    stock control also has (0 new) -- that check is not green and is not
    reported as green here.
  - `python tools/romdata_check.py --files src/actors/daObjFl_London_c.cpp
    src/actors/daObjClock_c.cpp`: 17 data symbols, `VERIFIED 11 (404 bytes)`,
    `PARTIAL 6 (64 bytes equal)`, `DIFFERS 0`, `UNNAMED 0`. All `_ZTI`/`_ZTV`/
    `g_profile_*` records are VERIFIED; the six PARTIALs are the
    NUL-terminated `_ZTS` strings (own and inherited), the tool's documented
    extent-is-an-upper-bound rule, not a byte difference.
  - `python tools/tu_order_check.py` on the two ids: exit 0, `ALL MATCH, ROM
    order, nothing unlicensed`.
- Shared-header consumer expansion: `python tools/check_src_tu_compiles.py`
  162/162 TUs compile; `check_header_offsets` green inside `premerge_check`;
  the only shared header touched is `include/decl_common.h` (renames, two
  duplicate declarations removed).
- Port/path/reference and other applicable static gates:
  `python tools/premerge_check.py HEAD --base origin/main`: exit 0, "nothing
  goes green -> red" on base and merge result. Individually, all exit 0:
  `check_rename_ledger.py` (2026 mangled/vtable rows agree; was 2017 before
  the restack, main added rows), `check_dead_references.py`,
  `langmode_audit.py --check langmode-baseline.json` (PASS),
  `tiers_ratchet.py --check` (CONVERTED 2701 -> 2704, not banked; MOVED from
  2699 -> 2702 because main banked its own promotions), `check_references.py`
  (run after
  `eligible.py`, which it depends on), `cpp_tu_compat.py`,
  `check_profile_campaign.py`, `port_refcheck.py` (423 references resolve),
  `check_tubuild_conflicts.py`, `cpp_tu_state.py --check-note`.
  Set-theoretic ledger audit `(base + head-added + main-added) - removed`
  against the pre-restack base `ecc97391e`, the verified head `450a93a65`
  and main `115cb5d74`: `symbols/actor_renames.tsv` 3512 +34 -13 (batch)
  +24 -15 (main) = 3542, `attribution.json` overrides 3105 +18 (batch) +55
  (main) = 3178, `config/converted-baseline.json` 2706 +12 -12 (batch) +13
  -11 (main) = 2708, `config/converted-backslide-exceptions.jsonl` 415 +0
  (batch) +5 (main) = 420; zero missing, zero extra in each.
- `queue_audit.py --check` (needs `rtti_extract.py`, `rtti_vtables.py --out
  build/rtti_vtables.json` and `tu_map.py --out build/tu_map.json`
  regenerated AFTER the last config edit or it refuses on staleness): clean
  at `450a93a65`; after the restack it exited 1 on exactly ONE row,
  `daObjBk_Dossunbar_c` (ov015), which #2442 landed on `main` without
  refreshing the queue -- the same failure mode as #2435's three ov002 rows
  this batch refreshes. `origin/main` `115cb5d74` is red on the gate by
  itself (4 rows: that one plus the three). By the coordinator's decision
  that row is repaired here too, in its own commit: `queue_audit --write`
  changed only that line (`23 350 ... no` -> `1 603 ... yes`), and
  `--check` is green again ("queue agrees with the tree"). `check_dead_references`
  re-run green after the edit; nothing else in this section can move on a
  queue-row line.
- RED, known tool defect: `python tools/prepush_attribution.py` exits 1 with
  14 `CREDIT LOST` rows on this tree, all this batch's folded or renamed
  shards (5 London, 7 clock, 2 volcano) -- issue #2433's basename-keyed
  false positive, as adjudicated for batch 1. The underlying credit loss in
  the chart is real and is stated above. The pre-push hook only runs it for
  pushes to `main`, so no hook was bypassed to push this branch.
- Private validation, if run, and the exact PR head/base it tested: not run.

Not run: a real `.nds` link (`--no-rom` everywhere) and the v2 queue.
