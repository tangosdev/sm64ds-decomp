# Handoff: prod-dossunbar-0907

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: no GitHub issue and **no queue
  task exist for this class**. Task ID `prod-dossunbar-0907`, stage
  `reconstruct` (role producer), session `prod-dossunbar-0907`, harness Claude
  Code. `classqueue.py v2 list` holds fifteen tasks and none of them reserves
  `class:daObjBk_Dossunbar_c`, so **no v2 claim was obtained and this output
  cannot be published through `v2 publish`** -- that needs a coordinator to
  enqueue the task first. The branch below is the deliverable until then.
- Source branch and previous accepted input SHA: branch
  `cpp/daObjBk_Dossunbar_c-tu`. No previous accepted input; this is the first
  stage on this class under v2. The scout evidence it consumes is
  `notes/data/class-facts/daObjBk_Dossunbar_c.json`, which was already banked on
  main under its pre-rename filename and is renamed, not rewritten, here.
- Original source base SHA and installed workflow/tool SHA: base
  `a278141a946931f7e243f2886de692bc4423674b`, which was `origin/main` when the
  task was issued and is still this branch's merge-base. `origin/main` has since
  advanced to `6e04e1090104949d2ca7a8902669aa063587da5e`; every base-versus-head
  gate below was therefore run against the pinned `a278141a9`, because running
  them against the moved ref reports other landed classes (`daMky_c`) as losses
  that this branch never touched. Workflow/tool revision
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Separate evidence commits and required artifacts in this commit: none separate.
  The source, the manifest, the ledger rows and this document are one branch of
  two commits: a rename-in-place commit, then the fold. That split is deliberate
  -- a commit may rewrite a file or move it, not both -- and it is what keeps the
  legacy shards' lineage attached through the consolidation.
- Next action, responsible role and blockers: independent verification. Two
  follow-ups belong to other owners and are recorded under "Remaining agreed
  scope" below; neither blocks this candidate.
- Status: verified candidate for the byte and reference gates listed under Proof;
  see the two named tool limitations, both reproduced on a landed sibling.
- Remaining uncommitted/local-only material and where it is preserved: none.
  Build logs and reports live in the worktree's gitignored build directory and
  are not part of this commit.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daObjBk_Dossunbar_c` in
  ov015, `.text` `0x02111ba0..0x02112290` (half-open), 25 functions, folded into
  `src/game/actors/d_a_obj_bk_dossunbar.cpp` and enrolled as one `complete` entry
  in `config/arm9/overlays/ov015/delinks.txt`.
- **The fold is 25, not the queue's 23.** Re-derived by address rather than taken
  from the queue row: the 25 delinks entries over that span are contiguous, they
  are bounded below by the botaosi shard that ends at `0x02111ba0` and above by
  the TowerStep destructor shard that starts at `0x02112290`, and they include
  the two zero-gap registry factories
  `daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L` and `..._S`, whose shards were
  named `d_a_obj_bk_dossunbar_bk_dossunbar_l.c` and `..._s.c`. The queue's
  `shard_count` of 23 is the known `_classInit` segmentation defect -- run
  extension looks for a coined-class `_classInit` while the tree spells factories
  with the ROM RTTI name -- reproduced here exactly, as
  `ov015/daObjBk_Dossunbar_c` (23) plus a stranded `@02112230-02112290` (2).
- Reserved source/header/config surfaces actually touched: no reservation exists
  (see Identity). Touched: `include/daObjBk_Dossunbar_c.h` (renamed from the
  coined name and rewritten), `include/decl_common.h` (shared: the class's
  `_ZTV` row renamed in place by the rename tool, and one exact duplicate of that
  same declaration removed), `config/arm9/overlays/ov015/symbols.txt` (renames in
  place at unchanged addresses; **no alias row was added**),
  `config/arm9/overlays/ov015/delinks.txt`, the class manifest,
  `symbols/actor_renames.tsv`, `attribution.json`,
  `config/converted-baseline.json`, and five notes files the rename and the fold
  keep current: `notes/data/class-facts/daObjBk_Dossunbar_c.json`,
  `notes/data/class-build-worklist.tsv`, `notes/data/tu-promotion-queue.tsv`,
  `notes/data/tu-merge-candidates.json` and `notes/bgobject-provenance.md`,
  whose section for this class gains the new field and loses its description of
  a C twin this commit deletes.
- ROM observations: `_ZTS19daObjBk_Dossunbar_c` at ov015 `0x0211451c` spells the
  class name, `_ZTI` at `0x02114510` points at `_ZTI10dBgActor_c` (ov002
  `0x021089ec`) through its `__si_class_type_info` base word, and `_ZTV` at
  `0x0211458c` is a 32-word table of which eight words differ from
  `_ZTV10dBgActor_c` and all eight land inside this overlay's `.text` -- slots 0,
  3, 6, 9, 16, 17, 27 and 31. Both factories allocate 0x338. The destructor pair
  is D1 at `0x02111ba0` and D0 at `0x02111be4`, and there is no D2 anywhere in
  the cartridge.
- Lineage evidence or structural inference: the direct base is `dBgActor_c` on
  the type_info base pointer, corroborated by the 32-word table length and by the
  destructor destroying the mesh collider at 0x124 and the model at 0xd4 before
  chaining. One new field is named in this commit: `mStateTimer` at 0x334. The
  six state-entry bodies seed it with 20, 10, 5, 0x18, 10 and 10 and the state
  bodies decrement and test it against 0 before advancing, so the offset, the
  width and the countdown role are proven; the word "Timer" is coined, and it was
  `pad_334` before.
- Hypothesized names/filenames, explicitly not recovered facts: the file name
  follows the registry profile, not a recovered original. `mHomePosX/Y/Z`,
  `mVariant`, `mState` and `mStateTimer` are coined readings of proven behaviour.
  The `classInit` spelling of the two factories comes from later EAD lineage, not
  from this cartridge. The fourteen address-named state helpers **keep their
  names on purpose**: fourteen pointer-to-member constants at `0x021144a0` are
  resolved by name, so renaming any of them is a same-commit
  `config/arm9/overlays/ov015/symbols.txt` edit, and the evidence for real names
  has not been gathered. That is the next concrete improvement for this class.
- Compiler experiments and measured barriers: the destructor is the load-bearing
  decision and the **inline** form -- `virtual ~daObjBk_Dossunbar_c() {}` written
  in the class body, declared ahead of the out-of-line virtuals -- reproduced D1
  at `0x02111ba0` followed by D0 at `0x02111be4` with no D2 on the first attempt.
  No partial was taken, so the out-of-line variants and the `defer_codegen off`
  lever were not needed. The cost of the inline form is that the object carries
  the vague-linkage RTTI records; ov015 delinks no `.data`, so all eleven are
  licensed as `deadstrip-data` and the TU emits no `_ZTV`, `_ZTI` or `_ZTS` of
  its own. `_ZN7Vector3D1Ev` is licensed as `deadstrip-duplicate` against its
  real home at arm9 `0x020072c0`. Members are written highest-address-first
  because the compiler emits `.text` in reverse source order; the object audit
  confirms the emitted order is ROM-ascending.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 25 of 25 functions VERIFIED by
  `linkcheck.py` driven **by symbol**, with 0 blind relocation slots and no
  differing bytes. Byte comparison is 23 of 25 under `tubuild.py verify`; the two
  exceptions are the factories and the reason is a tool route, not the source --
  see Proof.
- Genuine methods; remaining free-function/ABI bridges: seven compiler-spelled
  members (`InitResources`, `Behavior`, `Render`, `CleanupResources`,
  `OnHitByMegaChar`, `Kill`, and the destructor pair from one inline body).
  Sixteen bridges remain: the fourteen address-named state helpers and the two
  `extern "C"` registry factories, which are registry entry points and not
  methods.
- Recovered layout/fields; remaining shadow structs/raw offsets: the header
  declares the object down to 0x338 with a size assert. The TU still carries an
  opaque non-polymorphic shadow record for the pointer-to-member table, because a
  PMF's representation differs for a polymorphic class, plus small shadow records
  for the resource and dispatch tables. The net effect on the langmode shadow
  counters is a fall, not a rise, in every family.
- Lifecycle, vtable/RTTI, initializer and data ownership: the TU owns text only.
  It owns no `.data` and no `.bss`, declares no vtable of its own, and takes the
  vptr symbol from `include/decl_common.h`, which is where the pre-fold shards
  already read it from. The overlay's static initializer shard is a separate
  entry and is untouched.
- Attribution preserved through each move/rename: 22 legacy shards are carried by
  `path#symbol` rows placed **beside this class's own rows** in
  `attribution.json`, not appended at the tail. Two of those rows are keyed by
  the factories' legacy file stems, which is the landed shape for a shard whose
  basename is not a symbol name. One row was corrected from the tool's
  git-derived answer back to the owner the pre-fold path override recorded, so
  the merge gate reports no movement.
- Remaining agreed issue scope: (1) the fourteen address-named helpers need
  evidenced names plus the matching symbols.txt edit; (2) **a naming defect that
  is reported here, not fixed**: the ov015 `.bss` symbols
  `MovingBarSmall_ClsnFile` (`0x02114a5c`) and `MovingBarSmall_ModelFile`
  (`0x02114a64`) carry this class's old coined prefix but belong to TowerStep --
  the only file that references either is
  `src/_ZN9TowerStep16CleanupResourcesEv.cpp`. They are outside this fold, the
  rename tool left them alone because there is no word boundary after the coined
  prefix, and the promoted TU does not reference them. They need an owner.

## Proof

Base is `a278141a946931f7e243f2886de692bc4423674b` throughout. Both ROM builds
were regenerated for this candidate; no cached report was reused. Logs are in the
worktrees' gitignored build directories.

- Full-ROM build: `python tools/rombuild.py -j16` in this worktree, exit 0 --
  106/106 modules exact at 100.000000% of compared bytes, 11,191 source-built
  functions reproducing and 0 mismatching, source-owned data 20 of 20
  reproducing, ROM-build analysis PASS. Base build at the pinned SHA in a
  separate wired worktree, exit 0 -- the same 106/106 and 11,191/0, with
  `dsd check symbols --fail` failing on the same nine pre-existing errors the
  baseline control reports.
- Explicit function/consumer relocation checks: `tools/linkcheck.py` run once per
  manifest function with `--name`, `--addr`, `--size`, `--module ov015` and
  `--c src/game/actors/d_a_obj_bk_dossunbar.cpp` -- **25 VERIFIED, 0 WRONG,
  0 BLIND, 0 differing bytes**. Driven by symbol on purpose: the file-stem driver
  returns NO-SYM on a multi-symbol promoted TU.
  `python tools/check_header_offsets.py --changed a278141a9` exit 0 over both
  changed headers. `python tools/check_src_tu_compiles.py` exit 0, 156 of 156
  translation units compile.
- Complete emitted TU and data/metadata checks: `python tools/tubuild.py verify
  ov015/daObjBk_Dossunbar_c` reports **23 of 25 MATCH**, emission order
  ROM-ascending for all 25, and two DIFF lines, both on the factories, both
  reading `_ZTV19daObjBk_Dossunbar_c: unexpected reloc type=2 addend=0`. **This
  is a property of that command's one-function isolation route, not of this
  class**: its externalise branch accepts only a non-zero addend while the
  consolidated production route accepts addend 0, and the same command reports
  the same message on the landed, merged ov015 siblings' own factories. The
  addend-0 `R_ARM_ABS32` is correct, and the per-symbol link check above verifies
  both factories' linked bytes against the cartridge.
  `python tools/tubuild.py linkcheck ov015/daObjBk_Dossunbar_c` now **refuses**
  at its compile step with "intact production requires one .text claim and at
  least one non-text claim" -- a text-only promoted TU cannot take that route
  once it is enrolled. **Control: the same command refuses identically on
  `ov015/daObjBk_Rotebar_c`, which is landed and merged.** The whole-range link
  proof for this span was taken before promotion and is recorded in the manifest:
  scratch-link-verified, 25 of 25 MATCH including both factories, 0 differing
  bytes over `0x02111ba0..0x02112290`, 25 LICENSED objects with 0 unlicensed, and
  a linked ROM sha256 identical to the stock control.
  `python tools/romdata_check.py` exit 0 -- 688 symbols verified, 217 partial,
  5 differ; the five are pre-existing and none belongs to this class.
- Shared-header consumer expansion: `include/decl_common.h` lost one exact
  duplicate declaration of a symbol it still declares once, and
  `include/daObjBk_Dossunbar_c.h` was rewritten. The consumer proof is the full
  ROM build above, which compiles all 9,074 enrolled sources, plus the header
  offset check and the 156-TU compile.
- Port/path/reference and other applicable static gates: `check_rename_ledger`
  exit 0 (1,963 mangled/vtable rows agree with symbols.txt);
  `check_dead_references` exit 0, no new dead references and no broken markdown
  links; `port_refcheck` exit 0, 423 references resolve; `check_src_tu` exit 0;
  `layout_check` clean; `tiers_ratchet --check` PASS, baseline 2,698 against
  current 2,700 -- **`--update` was not run**, because it appends and would
  double-bank rows permanently. `premerge_check --base a278141a9 HEAD` exit 0:
  all eight static gates pass on the **merge tree** and nothing goes green to
  red.
- Language-mode ratchet: `langmode_audit.py --check` against the banked baseline
  **fails on this candidate and fails identically on the pinned base**, so it is
  already red on main. The four failing keys are the same four in both runs and
  every one of them is lower or equal here: local struct bodies 1,718 to 1,715,
  local bodies without an include 917 to 915, pad layouts and laundered
  constructs unchanged. An earlier draft of this TU **did** raise `extern_vtable`
  and `extern_vtable_classes` by one, by restating the vptr declaration locally;
  the pre-fold shards read that symbol from `include/decl_common.h`, so the local
  declaration was removed. Removing it is byte-free: the verify byte comparison
  and the full ROM build are unchanged before and after.
- Attribution: `prepush_attribution.py --base a278141a9` exit 0 -- 0 changed,
  0 lost, 22 consolidated with credit intact. `validate_merge.py --base
  a278141a9 --head HEAD` with both ROM reports supplied, exit 0 -- **contributor
  credit 0 added, 0 changed, 0 lost**, byte-verified functions and code bytes
  both +0, module fidelity 106/106 exact. That comparison is per ROM function
  address, so no contributor's total moved; the separate count of credited
  source PATHS necessarily falls by 24, because 25 files became one, and that is
  the fold's arithmetic rather than credit movement. Its one warning is that 25 address
  ranges left the byte-verified set while the enrolled totals held steady, which
  is the arithmetic of replacing 25 delinks entries with the one span that covers
  them. The gate's own model is what passed here; the published contributions
  chart resolves overrides by whole path, so it will not read these
  `path#symbol` rows.
- Private validation, if run, and the exact PR head/base it tested: not run. No
  PR was opened and nothing was merged, as instructed.
