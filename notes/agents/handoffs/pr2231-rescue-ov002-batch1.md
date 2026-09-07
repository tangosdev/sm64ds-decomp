# Handoff: pr2231-rescue-ov002-batch1

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: no GitHub issue and no v2 queue
  task exist for these three classes; this is batch 1 of the rescue of
  https://github.com/tangosdev/sm64ds-decomp/pull/2231 (closed unmerged, branch
  `cpp/promote-da-obj-lava` @ `8015f733e`, 17 promotions / 135 functions), stage
  `reconstruct` + `integrate-candidate`, session `rescue-lava-0907`, harness
  Claude Code (`claude-fable-5-1`). Enqueuing is a coordinator act and was not
  done; nothing was claimed in `classqueue.py v2`.
- Source branch and previous accepted input SHA: branch `cpp/promote-ov002-batch1`.
  There is no accepted input: the rescued branch was NOT merged (a trial merge of
  `origin/main` into it produced 40 conflicts and was abandoned). Its content for
  these three classes was re-derived file by file on top of main, and one of its
  commits, `b98628358` "Enroll func_ov002_020f051c by compiling it as C++", was
  cherry-picked with `-x`. `cpp/promote-da-obj-lava` is untouched on the remote
  and must not be deleted: it still holds the other 14 classes. Its head is
  `8015f733e` (re-confirmed with `gh pr view 2231` and
  `git rev-parse origin/cpp/promote-da-obj-lava`). The `9622b484a` this session
  first cited is a LOCAL-ONLY merge of `8015f733e` with an older main
  (`0335d0fce`), reachable from no remote ref; later batches must start from
  `8015f733e`, not from it.
- Original source base SHA and installed workflow/tool SHA: the batch was built on
  `ac9106002` (main at the start of the session) and `origin/main` `9e630dab3`
  was then MERGED in (never rebased); the two conflicts were the append-only
  tails of `symbols/actor_renames.tsv` and `attribution.json`, resolved by
  keeping both sides with main's rows first. Workflow/tool revision
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Separate evidence commits and required artifacts in this commit: no separate
  evidence commit. Stage artifacts: `src/actors/daObjLava_c.cpp`,
  `src/actors/daObjFire_c.cpp`, `src/actors/daSCoin_c.cpp`, their headers in
  `include/`, and the three promoted manifests under
  `config/tu_manifest.d/ov002/`.
- Next action, responsible role and blockers: independent verification of this
  commit (verifier), then integration. No known blocker: the `tubuild.py
  linkcheck` [4/8] refusal the brief anticipated for an already-promoted TU did
  not occur on this tree (see Proof).
- Status: verified candidate (local proof below; the private validator has not
  run on this commit).
- Remaining uncommitted/local-only material and where it is preserved: the other
  14 classes of PR #2231 (heart on `cpp/promote-ov002-heart-tu` in the same
  worktree, still WIP; ov045/ov010/ov012/ov013/ov022/ov026/ov030/ov033/ov062 on
  the original branch only). Gate logs live in the worktree's ignored `build/`.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: three ov002 classes, each now
  one intact-object TU owning its .text run and its .data run:
  - `daObjLava_c` (was coined `PoppingLavaBubbles`): text
    `ov002:[0x020b6d28,0x020b6e08)` (5 functions), data
    `ov002:[0x021093a0,0x0210945c)` = `_ZTS` 0x021093ac, `_ZTI` 0x021093a0,
    spawn-info record, `_ZTV` storage with address point 0x021093e0.
  - `daObjFire_c` (was coined `BlueFlame`): text `ov002:[0x020b5734,0x020b5a18)`
    (8 functions; one class, two profiles OBJ_RED_FIRE/OBJ_BLUE_FIRE with
    byte-identical factories), data `ov002:[0x02108edc,0x02108fb4)` = `_ZTS`
    0x02108ee8, `_ZTI`, two spawn-info records, `_ZTV` with address point
    0x02108f38.
  - `daSCoin_c` (already the ROM name): text `ov002:[0x020f03c4,0x020f0894)`
    (9 functions: 3 methods, D1/D0, 3 file-local helpers, factory), data
    `ov002:[0x0210aff4,0x0210b0ac)`.
- Reserved source/header/config surfaces actually touched:
  `config/arm9/overlays/ov002/symbols.txt` (renames only: the coined
  `PoppingLavaBubbles`/`BlueFlame` rows become the RTTI names,
  `func_ov002_020b6d84` becomes `_ZN11daObjLava_c8BehaviorEv`; the duplicate
  coined `_ZTV9BlueFlame` alias row is DELETED; no row is added),
  `config/arm9/overlays/ov002/delinks.txt` (three intact-object splices written by
  `tu_promote.py`), `include/decl_common.h` (the duplicate
  `extern int _ZTV11daObjFire_c[]` declaration deleted, net -1 line),
  `attribution.json` (22 per-function overrides written by `tu_promote.py`),
  `config/converted-baseline.json` (CONVERTED identity migration written by
  `tu_promote.py`; shape audited, not hand-merged), `symbols/actor_renames.tsv`
  (12 appended rows; five historical BlueFlame rows moved to the live names by
  `check_rename_ledger.py --fix`), `notes/actor-leaf-provenance.md` (one source
  citation repointed), `config/tu_manifest.d/ov002/daObjHatenaSwitch_c.json`
  (prose neighbour name only). Legacy one-function shards and the two coined
  headers are removed. The cherry-picked commit turned the C shard of
  `func_ov002_020f051c` into a C++ one (its first source build); the scoin
  promotion then folded that shard into the TU, so no shard file remains.
- ROM observations: `_ZTS11daObjLava_c` at ov002:0x021093ac and
  `_ZTS11daObjFire_c` at ov002:0x02108ee8 are the ROM's own class names, so the
  coined names are retired per the ROM-RTTI ruling. All three vtables are 31 slots
  (dActor_c-shaped, 0x84 bytes of storage each). Factories allocate 0xd8 (lava), 0x118 (fire), 0x114 (scoin)
  and plant `&_ZTV<C>[2]`. D1 precedes D0 in the cartridge; no D2 exists.
- Lineage evidence or structural inference: unchanged from main's headers; the
  only header edit per class is the destructor moved inline
  (`virtual ~X() {}`), which is what makes mwccarm emit the cartridge's D1/D0
  pair in the cartridge's order.
- Hypothesized names/filenames, explicitly not recovered facts:
  `daObjLava_c_classInit`, `daObjFire_c_classInit_OBJ_RED_FIRE`,
  `daObjFire_c_classInit_OBJ_BLUE_FIRE`, `daSCoin_c_classInit`, and the
  `g_profile_*` descriptor names are reconstructed spellings (the profile strings
  are ROM-proven, the identifier shapes are convention). `daObjLava_c::Behavior`
  is slot 6 of the ROM vtable, so the member name is slot-derived, not coined.
- Compiler experiments and measured barriers: `extern int _ZTV11daObjFire_c[]`
  inside the TU tripped the langmode `extern_vtable` ratchet (+1); the
  declaration now comes from `include/decl_common.h`, which is where main's own
  fire factories took it, and removing the declaration entirely does not
  compile, so this was the available option. `daSCoin_c.cpp` carried the same
  redundant extern next to its `decl_common.h` include; dropped as well.
  Byte-identical in both cases (8/8 and 9/9 MATCH after the change).

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 22/22 functions MATCH under
  `tubuild.py verify` (5+8+9), objisolate clean, reloc destinations clean; both
  ranges of each TU IDENTICAL in the scratch link (see Proof).
- Genuine methods; remaining free-function/ABI bridges: every vtable slot the
  classes override is a real member. The factories and daSCoin_c's three
  file-local helpers (`func_ov002_020f0438/051c/05f4`) stay `extern "C"` free
  functions, as on main.
- Recovered layout/fields; remaining shadow structs/raw offsets: main's headers
  as they were (daSCoin_c's named fields per `notes/actor-leaf-provenance.md`;
  lava's `mEffectHandle` at 0xd4). The factories still write the vptr and call
  the base ctor through raw `int *` as every intact-object TU on main does.
- Lifecycle, vtable/RTTI, initializer and data ownership: each TU emits its own
  `_ZTV`, `_ZTI`, `_ZTS` and spawn-info record(s); the .data run is a licensed
  claim in the manifest and reproduces byte-exactly.
- Attribution preserved through each move/rename: NOT preserved in the
  contributor chart. The 22 `attribution.json` overrides `tu_promote.py` wrote
  are `path#symbol` rows, and `chaos_db_ci.py` resolves overrides with a
  whole-path lookup (`overrides.get(<TU path>)` is `None` for all three TUs),
  so they are inert: tangosdev's and ruspecial's credit on the folded shards is
  lost in the chart even though `validate_merge` says `0 changed, 0 lost`.
  That is issue #2433's territory (2,230 such rows repo-wide today) and is not
  fixed here.
- Remaining agreed issue scope: the other 14 classes of PR #2231.

## Proof

Every command below was run in this worktree on the tree this document describes
(the batch merged with `origin/main` `cccae7692`), with the pinned mwccarm
2004/b56 and the extracted retail ROM wired in. "Green" means exit 0 and the
quoted verdict; nothing here is inherited from PR #2231's own manifests or logs.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom --report-json
  build/head-rom.json --data-json build/head-data.json`, exit 0. `module
  fidelity: 106/106 exact, 100.000000%`, `source-built functions: 11,192`,
  `mismatching: 0`, `source-owned data claims: 23 (reproducing 23, mismatching
  0)`, `ROM-build analysis: PASS`. Log `build/rombuild-head3.log` (ignored).
  The same command at `origin/main` `cccae7692` in a second wired worktree gives
  the base report (`106/106`, 20/20 data claims, `ROM data from source: 686
  verified, 215 partial, 5 differ`).
- Explicit function/consumer relocation checks:
  - `python tools/tubuild.py verify ov002/<id>` for the three ids: 5/5, 8/8 and
    9/9 MATCH, `objisolate clean`, `reloc-destinations clean`, emission order as
    expected -> TEXT-VERIFIED each.
  - `python tools/linkcheck.py --name <sym> --c src/actors/<Class>.cpp --module
    ov002 --addr ... --size ...` for all 22 functions (addresses and sizes from
    the manifests): 22 VERIFIED, 0 BLIND, 0 WRONG, 0 NO-REPRO
    (`build/lc-symbols.txt`). `prepush_linkcheck` cannot see promoted TUs
    (it looks for `src/<sym>.*`), which is why the per-symbol form was driven
    by hand.
  - `validate_merge.py --base cccae7692 --head HEAD` with the two ROM reports
    above: exit 0; byte-verified functions 11,164/11,347 (+1: the cherry-picked
    C++ compile of `func_ov002_020f051c`, +216 bytes); `Contributor credit 0
    added, 0 changed, 0 lost`; `Relocation check: 0 checked; no affected
    slots`; `ROM data reproduced from source: 693 exact, 217 partial, 4 differ`
    (base 686/215/5). Its warning that 21 address ranges "left the byte-verified
    set while enrolled totals held steady" is the 21 shard ranges being replaced
    by three whole-TU ranges; the function count went up, not down.
- Complete emitted TU and data/metadata checks:
  - `python tools/tubuild.py linkcheck ov002/<id>` on each PROMOTED TU (final
    tree): all three `SCRATCH-DATA-VERIFIED`; TU .text and .data ranges
    IDENTICAL (lava 224+188 bytes, fire 740+216, scoin 1232+184); 9/13/13
    LICENSED symbols, 0 COLLIDES, 0 HOMELESS; `106/106 exact`; scratch ROM
    sha256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`,
    IDENTICAL to the source-independent stock control. The [4/8] "intact
    production requires one .text claim and at least one non-text claim"
    refusal the brief anticipated for an already-promoted TU did NOT occur on
    this tree; the tool ran to completion. Caveat the tool itself prints: `dsd
    check symbols --fail` has 9 pre-existing errors that the stock control also
    has (0 new) -- that check is not green and is not reported as green here.
  - `python tools/romdata_check.py --files src/actors/daObjLava_c.cpp
    src/actors/daObjFire_c.cpp src/actors/daSCoin_c.cpp`: 19 data symbols,
    `VERIFIED 13 (552 bytes)`, `PARTIAL 6 (56 bytes equal)`, `DIFFERS 0`,
    `UNNAMED 0`. All 13 `_ZTI`/`_ZTV`/`g_profile_*` records are VERIFIED. The
    six PARTIALs are the NUL-terminated `_ZTS` strings (`_ZTS11daObjLava_c`,
    `_ZTS11daObjFire_c`, `_ZTS9daSCoin_c` and the vague-linkage copies of
    `_ZTS7fBase_c`, `_ZTS7dBase_c`, `_ZTS8dActor_c`): every compared byte is
    equal, and the verdict is the tool's documented extent-is-an-upper-bound
    rule for sizeless data symbols, not a byte difference.
  - `python tools/tu_order_check.py` on the three ids: exit 0, "all match in
    ROM order"; it lists `_ZN7Vector3D1Ev` as an EXTRA vague-linkage section in
    daSCoin_c, which the linkcheck licensing audit already accounts for.
- Shared-header consumer expansion: `python tools/check_src_tu_compiles.py`
  157/157 TUs compile; `check_header_offsets` green inside `premerge_check`;
  the only shared header touched is `include/decl_common.h` (one duplicate
  declaration removed).
- Port/path/reference and other applicable static gates:
  `python tools/premerge_check.py HEAD --base origin/main`: converted-ratchet,
  dead-references, duplicate-sources, header-offsets, langmode-ratchet,
  layout-check, src-tu-refs, source-coverage all pass on base and on the merge
  result; "nothing goes green -> red". Individually, all exit 0:
  `check_rename_ledger.py` (1961 mangled/vtable rows agree), `eligible.py`,
  `check_references.py`, `cpp_tu_compat.py`, `check_profile_campaign.py`,
  `port_refcheck.py` (423 references resolve), `check_tubuild_conflicts.py`,
  `tiers_ratchet.py --check` (CONVERTED 2698 -> 2701, not banked, no
  `--update` run), `langmode_audit.py --check langmode-baseline.json` (PASS;
  `extern_vtable` 164 against the banked 165 after the two redundant externs
  were dropped -- before the fire fix it read 165 -> 166).
- RED, known tool defect: `python tools/prepush_attribution.py` exits 1 with
  13 `CREDIT LOST` rows on this tree (it is in the pre-push hook). 12 are this
  batch's folded shards -- exactly the ones whose CLASS was renamed
  (`PoppingLavaBubbles*`, `BlueFlame*`, `d_a_obj_lava`, `d_a_obj_fire_*`,
  `d_a_s_coin`, `func_ov002_020b6d84`); the 5 daSCoin_c shards whose basename
  still equals the surviving symbol are reported "consolidated, credit intact".
  The 13th (`daBmb_c`) comes from main. This is issue #2433's basename-keyed
  false positive, adjudicated as such by the verifier; the underlying credit
  loss in the chart is real and is stated above.
- Private validation, if run, and the exact PR head/base it tested: not run.

Not run: a real `.nds` link (`--no-rom` everywhere) and the v2 queue
(`classqueue.py v2`) -- no task exists for these classes and none was created.
