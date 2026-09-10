# Handoff: ov078/daBombking_c recovery

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: no GitHub issue and **no v2
  queue task**. `python tools/classqueue_v2.py list` shows fifteen tasks
  (`issue-2409` through `issue-2423`) and none of them is ov078/daBombking_c, so
  this producer holds no receipt and nothing was claimed or published through
  the queue. Stage: **recovery** of a finished branch that never had a PR
  opened, role producer, session `prod-kbo-0907`, harness Claude Code
  (`claude-opus-5`).
- Source branch and previous accepted input SHA: `origin/cpp/KingBobOmb-tu`,
  tip `2fa763963` (2026-09-05). This branch is `cpp/daBombking_c-recover`; it
  descends from that tip. Nothing was rebased, reset or amended -- the
  decompilation is the branch's, unchanged.
- Original source base SHA and installed workflow/tool SHA: merge-base with main
  `f3b11fa9b`. Workflow/tool revision
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`. `origin/main` was merged in three
  times with `--no-ff`, never rebased, because main landed a TU promotion under
  this branch twice while it was under test: `a278141a9`, then `6e04e1090`
  (ov030/daMky_c, #2430), then `ecc97391e` (ov002 batch-1 rescue, #2435).
  **The base this candidate is proved against is `ecc97391e`.**
- Separate evidence commits and required artifacts in this commit: no separate
  evidence commit. Facts: `notes/data/class-facts/daBombking_c.json`, carried
  from the source branch and re-measured here rather than trusted (see ROM
  observations). Stage artifacts present:
  `config/tu_manifest.d/ov078/daBombking_c.json`,
  `src/actors/daBombking_c.cpp`, `include/daBombking_c.h`.
- Next action, responsible role and blockers: independent verification of this
  commit (verifier). No blocker is known for this class. Two conditions in the
  tree are recorded in Proof and belong to main, not to this candidate: a stale
  `notes/data/tu-promotion-queue.tsv` and a langmode ratchet that main itself
  fails.
- Status: **verified candidate.** Every figure in Proof was produced on this
  tree at base `ecc97391e`; the private terminal validator has not run on it.
- Remaining uncommitted/local-only material and where it is preserved: none.
  Gate logs live in the ignored `build/` directories of two private worktrees
  and are not part of the candidate.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daBombking_c` in ov078 --
  the King Bob-omb that holds the star at the top of Bob-omb Battlefield. Text
  `ov078:[0x02123740,0x02126660)`, **52 functions**. Own metadata
  `_ZTS12daBombking_c` at 0x02126e18, `_ZTI12daBombking_c` at 0x02126e0c,
  vtable storage from 0x02126e44 with the address point at 0x02126e4c.
  Fifty-two one-function sources become one `src/actors/daBombking_c.cpp`;
  fifty-two `delinks.txt` entries become one `complete` span.
- Reserved source/header/config surfaces actually touched by this commit:
  `src/actors/daBombking_c.cpp` (new), the class header renamed from
  `KingBobOmb.h` to `include/daBombking_c.h`,
  `config/tu_manifest.d/ov078/daBombking_c.json`
  (new), `config/arm9/overlays/ov078/delinks.txt`,
  `config/arm9/overlays/ov078/symbols.txt`, `include/decl_common.h` (one line),
  `attribution.json`, `config/converted-baseline.json`,
  `config/match_attempts.jsonl`, `config/match_provenance.jsonl`,
  `symbols/actor_renames.tsv`, `notes/data/class-facts/daBombking_c.json` (new),
  and five generated or prose notes that name this class
  (`notes/cpp-tu-current-state.md`, `notes/data/tu-promotion-queue.tsv`,
  `notes/data/class-build-worklist.tsv`, `notes/enemy-provenance.md`,
  `notes/archive/n64-decomp-cross-reference.md`). Untouched and deliberately
  so: `config/converted-backslide-exceptions.jsonl`, every other module's
  `symbols.txt` and `delinks.txt`, and `notes/data/tu-merge-candidates.json`
  (see Repairs).
- ROM observations, each re-measured on this tree and not quoted from the source
  branch's own notes:
  - `extracted/overlays/overlay_0078.bin` loads at 0x02123740. The bytes
    `12daBombking_c` NUL-terminated sit at 0x02126e18, length-prefixed, and the
    `__si_class_type_info` at 0x02126e0c has 0x02126e18 as its name word and
    0x021081c0 (`_ZTI12dEnemyBase_c`, ov002) as its base word. The vtable
    header at 0x02126e44 reads offset-to-top 0 then 0x02126e0c, so the symbol
    at 0x02126e4c IS the address point and storage begins eight bytes lower.
  - `python tools/rtti_vtables.py --own daBombking_c`: 31 slots, base
    `dEnemyBase_c` also 31 -- this class adds no slot, it only overrides -- and
    **8 own overrides at slots 0, 3, 6, 9, 12, 16, 17 and 29**, whose addresses
    are 0x02126368, 0x02125ff4, 0x02126104, 0x021260ac, 0x021260a8, 0x02123740,
    0x02123798 and 0x021265f4. Every one is inside the claimed run.
  - Scanning every extracted cartridge image (2,300 files, 21,144,797 bytes)
    for `KingBobOmb` finds it **zero times**. The coined name is not the
    cartridge's; `daBombking_c` is.
  - `config/arm9/overlays/ov078/symbols.txt` holds **52 function symbols in
    [0x02123740,0x02126660) with no internal gap**, the run ending exactly at
    the `.init` boundary; the lowest is `_ZN12daBombking_cD1Ev` and the highest
    is `daBombking_c_classInit` (0x021265fc, size 0x64). D1 and D0 are the two
    lowest addresses and there is no D2 anywhere in the module. No address in
    that file carries more than one symbol.
- Lineage evidence or structural inference: none added. This is recovery: no
  field was renamed, no body re-derived, no name coined by this task. The one
  coined identifier that survives in the TU is the free function
  `KingBobOmb_SetState` at 0x02125c48, for which the cartridge offers no name.
- Hypothesized names/filenames, explicitly not recovered facts: the six member
  spellings (`InitResources`, `CleanupResources`, `Behavior`, `Render`,
  `OnPendingDestroy`, `OnAimedAtWithEgg`) come from the shared `dEnemyBase_c`
  slot vocabulary, not from ov078; the factory spelling
  `daBombking_c_classInit` and the profile global `g_profile_BOMBKING` are
  reconstructions; `KingBobOmb_SetState` is coined at both ends; the `size`
  assert 0x50c is the observed field span rounded up and guards the declaration
  rather than evidencing the ROM.
- Compiler experiments and measured barriers: **none run here, by design.** The
  branch already had the shape that reproduces 52/52 and the destructor
  position was not touched. `#pragma defer_codegen off` and the three balanced
  `#pragma push`/`#pragma pop` pairs are the branch's and were left alone. The
  only compiler question this task answered is whether that shape still
  reproduces at today's main, and it does.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 52 of 52 functions byte-match
  with relocations resolved to the configured destinations, and all 52 verify
  individually through `tools/linkcheck.py` driven by symbol with 0 BLIND. The
  full ROM build links the TU inside the ov078 image at 106/106 modules exact.
- Genuine methods; remaining free-function/ABI bridges: the TU defines seven
  `daBombking_c::` bodies -- `InitResources`, `CleanupResources`, `Behavior`,
  `Render`, `OnPendingDestroy`, `OnAimedAtWithEgg` and `~daBombking_c` -- and
  the destructor emits both D1 and D0, so **8 of the 52 symbols are
  compiler-spelled class members and 44 remain free functions** (42
  `func_ov078_*`, plus `KingBobOmb_SetState` and `daBombking_c_classInit`).
  This is a promoted TU, not a reconstructed class. 51 `extern "C"`
  occurrences bridge to callees whose own headers do not yet declare them as
  methods.
- Recovered layout/fields; remaining shadow structs/raw offsets: 498 raw
  `+ 0xNNN` byte-offset expressions and 11 local shadow type definitions
  (`M12`, `Vec3`, `M3`, an opaque `BCA_File`, `C` with its `PMF` window, `P2`,
  `G`, `CView`, one anonymous struct typedef, and the `Base`/`Derived` pair)
  remain, all inherited from the shards. 74 distinct mangled identifiers are
  referenced by hand. `include/daBombking_c.h` names 23 fields with commented
  offsets, all agreeing with the struct's 0x50a span.
- Lifecycle, vtable/RTTI, initializer and data ownership: **text-only route.**
  The manifest claims one `.text` section and nothing else (`data: []`,
  `bss: []`), and after the fold `config/arm9/overlays/ov078/delinks.txt` holds
  exactly two source entries -- this TU's `complete` `.text` span and the
  pre-existing `__sinit_ov078_02126660.c` `.init` span. No `.data`, `.bss` or
  `.ctor` source entry exists in the module. A correction worth stating plainly,
  because the route name invites the opposite reading: **the TU does emit
  `_ZTV`, `_ZTI` and `_ZTS`.** The destructor is the first virtual declared and
  is defined out of line, so it is the key function and mwccarm emits the whole
  inherited metadata chain as vague linkage. Those emissions are not claimed as
  source-owned data; all thirteen are licensed in the manifest's
  `compiler_only_output` block -- `_ZN12daBombking_cD2Ev` as `deadstrip` (the
  cartridge has no D2), `_ZN7Vector3D1Ev` as `deadstrip-duplicate`, and eleven
  RTTI records as `deadstrip-data`: the class's own `_ZTV`/`_ZTI`/`_ZTS` plus
  `_ZTI`/`_ZTS` for `dEnemyBase_c`, `dActor_c`, `dBase_c` and `fBase_c` -- so
  mwldarm drops them and the cartridge's own copies survive. `romdata_check`
  confirms it: 0 DIFFERS.
- Attribution preserved through each move/rename: the fold moves 52 symbols out
  of 52 one-function shards into one TU, which would otherwise hand their credit
  to whoever pushed the fold. 52 `src/actors/daBombking_c.cpp#<symbol>`
  override rows carry it instead, each holding the author `chaos_db_ci` resolves
  for that symbol's own legacy shard on the merge base. Of the 52, thirteen are
  owed to contributors other than the maintainer account: nine to `lunavyqo`,
  two to `ruspecial` and two to `andrewboudreau`. The rows were inserted
  **beside this class's sorted neighbours** (immediately after the
  `src/actors/daBmb_c.cpp#...` block, indices 693-744 of 3,157) rather than
  appended at the tail, so a concurrently landing PR appending at the tail does
  not collide with them -- and `ecc97391e` did exactly that without conflict.
- Remaining agreed issue scope: (1) turn the 44 free functions into members as
  the evidence for each is established -- that is class reconstruction and is
  not attempted here; (2) the 498 raw offsets and 11 shadow types that go with
  them; (3) a tree-wide refresh of `notes/data/tu-merge-candidates.json`, which
  still holds the pre-promotion entry for this class and for `daGmch_c`,
  `daMky_c` and `daBmb_c` alike.

## The count: 52, not 51

The queue row said `KingBobOmb  51`, and the fold is 52. Re-derived by address
rather than by the queue: `config/arm9/overlays/ov078/symbols.txt` holds 52
function symbols in [0x02123740,0x02126660) with no internal gap, ending exactly
at the `.init` boundary. **`daBombking_c_classInit` (0x021265fc, size 0x64) and
its shard `d_a_bombking.c` are INSIDE the 52** -- it is the highest-addressed
member of the run, not a neighbour.

The mechanism is in `tools/queue_audit.py`. It extends a run over a zero-gap
neighbour only when that neighbour is spelled `<class>_classInit` for a class
already in the row's set. The row was keyed on the coined name `KingBobOmb`
while `symbols.txt` already spelled the factory `daBombking_c_classInit`, so the
stems never matched and the factory fell out of the count. `build/tu_map.json`
misses it for a related reason: `tu_map` cuts on symbol NAME, and a factory is
not spelled `_ZN<len><Class>...`, so it was never labelled. The queue's
`shard_count` is a floor, and this row was short by exactly one.

## Repairs this recovery made to the branch

Three defects in the source branch were repaired here. None of them touches a
byte of the decompilation.

- **`symbols/actor_renames.tsv` was rewritten in place.** The branch renamed the
  class by running `tools/class_rename.py` over the ledger, which is a plain
  regex substitution: it replaced the coined spelling in eight existing rows and
  left no record that a rename had happened. The ledger is an append log whose
  fourth column asserts what a symbol is called *now*, so the substitution
  destroyed the only record of where the name came from. Both classes that
  landed this same move on main record it two ways -- a `" (was <coined>)"`
  suffix on the in-place row, which `tools/check_rename_ledger.py` already
  parses, and an appended row per renamed symbol carrying the evidence. This
  commit adds both for ov078: eight suffixes and nine appended rows. `ecc97391e`
  landed while this was under test and appended twelve rows of exactly that form
  for its own three classes, which is independent confirmation of the
  convention.
- **A stale snapshot was being kept up to date.**
  `notes/data/tu-merge-candidates.json` had its ov078 entry rewritten to name 52
  sources spelled `_ZN12daBombking_c*.cpp` -- paths that have never existed in
  this tree under any commit. No tool writes that file and no workflow reads it;
  it already holds 3,134 `src/` paths that do not resolve, and it keeps the
  pre-promotion entry for every class already promoted. Reverted to main's blob,
  so the file leaves this candidate's diff entirely.
- **A generated banner named 51 deleted shards.** The TU header was still the
  `tubuild.py create` shadow banner, listing every retired source by path and
  declaring the file "NOT ENROLLED, NOT CANONICAL". It was replaced with a
  production banner that lists the 52 members by address and symbol instead;
  `include/daBombking_c.h` lost two dead paths from one field comment the same
  way. `check_dead_references` went from 53 findings to none.

## Proof

Everything below was run on this commit's tree in two private wired worktrees,
pinned compiler mwccarm 2004/b56. The base for every comparison is
`origin/main` at `26f54f8fc`; the two ROM builds were made one commit earlier
at `ecc97391e`, whose source tree is byte-identical (the delta is a `[skip ci]`
refresh of README.md, contributions.json and two files under `docs/`, and
`git diff --name-only ecc97391e 26f54f8fc -- src/ include/ config/` is empty).
Both ROM builds were **regenerated from scratch for this base**; no cached
report was reused. Logs are private `build/` files and scratch files; they are
not committed.

- **Full-ROM build.** `python tools/rombuild.py -j16` -- exit 0 on both sides.
  - Base: 9,037 enrolled sources; 11,192 source-built functions, **11,192
    reproducing / 0 mismatching**; 23 source-owned data claims reproducing / 0
    mismatching; 3 source-owned BSS claims; **module fidelity 106/106 exact,
    100.000000% of compared bytes**; ROM-build analysis PASS; ROM data from
    source 695 verified / 219 partial / 4 differ / 399 unnamed.
  - Head: 8,986 enrolled sources (**-51**, the fold); **11,192 reproducing / 0
    mismatching**; 23 data claims reproducing / 0 mismatching; 3 BSS claims;
    **106/106 exact, 100.000000%**; ROM-build analysis PASS; ROM data 696
    verified / 220 partial / **4 differ** / 397 unnamed. Verified is up one and
    differ is unchanged, so no ROM data was lost.
  - Both builds produce a ROM with sha256
    `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, and
    each reports "vs this tree's last stock build: IDENTICAL". The head TU is
    linked inside the ov078 image, not compared in isolation.
  - `dsd check symbols --fail` FAILs on the baseline control and on both sides
    with **the same nine pre-existing errors** (`overlay_100`, `overlay_102`,
    two arm9 `data_020ad5xx`, four ITCM `func_01ff9xxx` and `_deq`), diffed
    line-for-line between base and head. None is in ov078.
- **Explicit function and relocation checks.**
  - `python tools/tubuild.py verify ov078/daBombking_c` -- exit 0.
    **52/52 MATCH** (match.py extract_func + compare, relocation-aware),
    objisolate plan clean, reloc-destinations clean, emission order all 52 in
    ROM-ascending section order -> **TEXT-VERIFIED**. The manifest is
    byte-identical after the run, so this is my own measurement and not a
    replay of the block the branch stored.
  - `python tools/linkcheck.py` driven **by symbol**, once per member with
    `--module ov078 --name <sym> --addr <a> --size <n> --c
    src/actors/daBombking_c.cpp`: **52 VERIFIED, 0 BLIND, 0 anything else.**
    Driven by symbol deliberately, because the file-stem path reports NO-SYM on
    a multi-symbol promoted TU.
  - `python tools/pr_linkcheck.py --files src/actors/daBombking_c.cpp` -- exit
    0, `ok  src/actors/daBombking_c.cpp (52 slot(s))`.
  - `python tools/prepush_linkcheck.py --range origin/main..HEAD` -- exit 0;
    904 checked, 610 verified, 294 warnings, **0 blocking**. `daBombking_c`
    appears as `[WARN] NO-SYM`: that tool keys a file by its stem and a
    promoted TU has no symbol of that name, so its warning is a shape
    limitation and is **not** evidence about this TU either way. The
    by-symbol run above is the real check.
- **Complete emitted TU and data/metadata checks.**
  - `python tools/tubuild.py linkcheck ov078/daBombking_c` -- **exit 1, and not
    a source failure.** It stops at step [4/8] with "isolate: intact TU
    preparation refused: intact production requires one .text claim and at
    least one non-text claim". Control: the same command against
    **`ov102/daBmb_c`, which is landed, merged and CI-green on main**, refuses
    at the same step with the same message. The refusal is a pre-existing tool
    limitation for any enrolled text-only promoted TU, not a property of this
    candidate. For this route the whole-tree link proof is the full ROM build
    above. The manifest's stored `scratch-link-verified` block predates
    enrollment and is carried, not re-established, by this commit.
  - `python tools/romdata_check.py --files src/actors/daBombking_c.cpp` -- exit
    0. 11 data symbols emitted: **5 VERIFIED** (56 bytes equal), **6 PARTIAL**
    (172 bytes equal), **0 DIFFERS, 0 UNNAMED**. The five VERIFIED are every
    `_ZTI` record -- `_ZTI12daBombking_c` at ov078 0x02126e0c and the four
    ancestors' -- and the six PARTIALs are the five `_ZTS` type-name strings
    plus the class's own `_ZTV`, each emitted shorter than the extent derived
    from `symbols.txt` because that extent runs to the next symbol. Short
    coverage, not a byte disagreement.
- **Shared-header consumer expansion.** `python tools/affected_src.py
  include/daBombking_c.h` -- the only consumer is `src/actors/daBombking_c.cpp`,
  compiled by every check above. `python tools/check_header_offsets.py
  include/daBombking_c.h` -- exit 0, 23 commented fields, **0 mismatched**, 0
  unparsed, struct spans 0x50a.
- **Merge validation.** `python tools/validate_merge.py --base origin/main
  --head HEAD` with both fresh `rombuild-report.json` files, the `pr_linkcheck`
  report and the `port_refcheck` report -- exit 0, **status "Passed", 0
  reasons**, one warning. Byte-verified functions 11,164 / 11,347 (**+0**);
  byte-verified code bytes **+0**; **contributor credit 0 added, 0 changed, 0
  lost**; relocation check 52 checked / 52 VERIFIED; port references 423 checked
  / 0 stale; module fidelity 106/106 exact; ROM data 696 exact / 220 partial /
  4 differ. The single warning is the expected consolidation: "52 address
  range(s) left the byte-verified set while enrolled totals held steady" --
  those 52 ranges are the fold's own, and the totals confirm they did not leave
  the build.
  - On credit, stated precisely: **the gate passed and no contributor's totals
    moved.** That is a stronger claim than the gate's own line, and it was
    checked separately, because `chaos_db_ci` resolves overrides by whole path
    and `path#symbol` rows are inert there. Recomputing
    `validate_merge.attribution_snapshot` on both revisions gives **13
    contributors on each side with identical function AND byte totals** and
    11,279 attributed functions on both -- `tangosdev` 7,546 fn / 1,163,676 B,
    `andrewboudreau` 1,642 / 462,978, `ruspecial` 832 / 176,430, `lunavyqo` 415
    / 213,040, and nine smaller accounts, every one unchanged. What this does
    **not** prove is that the published contributions chart survives; that is a
    separate consumer.
  - `rom_data_regressions` did not fire. It anchors on `(module, addr, bytes)`
    since #2425, so retiring the coined `_ZTV10KingBobOmb` alias at an unchanged
    address reads clean, and ROM data rose 695 -> 696 exact rather than falling.
- **Merge safety.** `python tools/premerge_check.py --base origin/main HEAD` --
  exit 0. All eight gates pass -> pass on base and on the composed merge tree
  (`converted-ratchet`, `dead-references`, `duplicate-sources`,
  `header-offsets`, `langmode-ratchet`, `layout-check`, `src-tu-refs`,
  `source-coverage`): **"nothing goes green -> red"**. Source-coverage info:
  entries 9,066 -> 9,015 (-51), "consolidation, not a loss -- bytes are flat".
- **Ratchets.**
  - `python tools/langmode_audit.py --check <origin/chaos-data
    8ac85ba45:langmode-baseline.json>` -- **exit 0, ratchet PASS**, and a clean
    checkout of the base passes too. Worth recording because it did not start
    that way: against the same baseline file this candidate FAILED while it was
    still one main-commit behind, and so did clean main, worse. Comparing the
    raw counters head-against-main settles it -- every counter this candidate
    moves, it moves **down**: `local_struct_body` -7, `local_body_no_include`
    -5, `local_struct_body_cpp` -4, `local_body_no_include_cpp` -3,
    `launder_or_forced` -2, `pad_layout` -1, `src_files` -51, and
    `extern_vtable` / `extern_vtable_classes` **+/-0**. The vptr store uses the
    `extern int _ZTV12daBombking_c[];` declaration that
    `include/decl_common.h` already carried under the coined name, so the
    declaration site did not move and the extern-vtable counters do not rise.
  - `python tools/tiers_ratchet.py --check` -- exit 0, **PASS**, baseline 2699,
    current 2702, +3 gained and **not banked**. A clean checkout of the base
    reports the identical +3, so the gain is main's and predates this
    candidate. `--write`/`--update` was never run; that command appends rather
    than replaces, and a restack plus a re-run would double-bank rows
    permanently.
- **Static gates, all exit 0** unless noted: `check_rename_ledger.py --repo .`
  (2,005 mangled/vtable rows agree with their modules' `symbols.txt`; 1,532
  coined rows out of scope); `check_dead_references.py` (no new dead
  references, no broken markdown links); `port_refcheck.py` (423 checked, 0
  stale); `cpp_tu_state.py --check-note`; `eligible.py`; `check_references.py`;
  `layout_check.py`; `check_duplicate_sources.py`; `check_src_tu.py`;
  `check_tubuild_conflicts.py`; `check_data_definitions.py`;
  `cpp_tu_compat.py --require-ready`; `check_profile_campaign.py --repo .`;
  `source_coverage.py`; `check_src_tu_compiles.py --quiet` (**160/160**
  translation units compile).
  - `python -m unittest` over `test_tu_manifest`, `test_validate_merge`,
    `test_attribution`, `test_tiers_ratchet`, `test_check_dead_references` and
    `test_check_rename_ledger`: **171 tests, all passing.** One of them
    (`test_the_committed_baseline_matches_the_tree`) failed on a first run and
    caught a real defect in this very document: an earlier draft named the
    retired class header by its full path, which this candidate deletes. It is
    named by basename now. `check_dead_references` then caught a second one, a
    literal `ovNNN` placeholder written as a path; both were fixed and both
    tools were re-run green on the text as it now stands.
  - **`queue_audit.py --check` exits 1, and the failure is main's.** It reports
    3 `already_promoted`, 2 `compiler-only`, 3 `shard_count`, 3 `total_lines`
    and 1 `unmatched` row disagreeing with the tree. A clean checkout of the
    base fails **identically, category for category**, and re-running `--write`
    on this tree rewrites exactly three rows -- `daObjFire_c`,
    `daObjLava_c+daObjLava_c` and `daSCoin_c`, the three classes #2435 landed
    without re-deriving the queue. This candidate's own row is not among them
    and `--write` leaves it untouched, so the stale rows were deliberately not
    banked here: doing so would carry another PR's bookkeeping into this
    candidate. The one row this class owns was re-derived and `--check` agrees
    with it.
- **The two forbidden workarounds were not used, and this is checkable.**
  No alias row was added to any module's `symbols.txt`: in
  `config/arm9/overlays/ov078/symbols.txt` **zero addresses carry more than one
  symbol**, no other module's copy is touched at all, and the branch in
  fact *deletes* the one alias that existed, the coined `_ZTV10KingBobOmb` at
  0x02126e4c, because dsd resolves the `.data` records pointing there by name
  and only one spelling may survive. Nothing consults
  `symbols/actor_renames.tsv` to satisfy `rom_data_regressions` or any other
  gate; that gate did not fire at all, and `check_rename_ledger` only ever
  compares the ledger against each module's own `symbols.txt`.
- **Private validation:** NOT run on this commit. No PR has been opened and no
  merge performed; sequencing is the integrator's.

A log generated after this commit belongs in separately recorded evidence. Do
not amend a proved candidate just to paste its own SHA or a later result into
it.
