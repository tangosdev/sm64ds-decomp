# Handoff: issue-2412

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  https://github.com/tangosdev/sm64ds-decomp/issues/2412 -- task `issue-2412`,
  stage `reconstruct` (role producer), session `prod-2412-0907`, harness Claude
  Code (`claude-fable-5-1`).
- Source branch and previous accepted input SHA: branch `cpp/daGmch_c-v2`,
  which continues `cpp/Moneybag-tu` without rewriting it. Accepted input
  `2f34c64aa90c712c0da8b936842592a39ad853cf` (the tip of `cpp/Moneybag-tu` and the
  head of https://github.com/tangosdev/sm64ds-decomp/pull/2395). This commit
  descends from it; nothing was rebased, merged or reset.
- Original source base SHA and installed workflow/tool SHA: merge-base with main
  `b2bd6a323832d244a12e750713b1405b26a94c66`; the earlier source-base record from
  https://github.com/tangosdev/sm64ds-decomp/issues/2403 is
  `2ab05bae36a6821679a7e647e811c0a3243a3c49` (a verified ancestor, kept separate).
  Workflow/tool revision `f327f7b6460e157153eb7fc0749dbbe60dd854f1`. `origin/main`
  was not merged in: every file it would have brought into conflict is
  integration-lane state this task does not own.
- Separate evidence commits and required artifacts in this commit: no separate
  evidence commit. Facts: `notes/data/class-facts/daGmch_c.json` (unchanged from
  the input). Stage artifacts present: `src/actors/daGmch_c.cpp`,
  `config/tu_manifest.d/ov081/daGmch_c.json`.
- Next action, responsible role and blockers: independent verification of this
  commit (verifier). Blocker for integration only, not for verification:
  `rom_data_regressions` in `tools/validate_merge.py` reads the coined-to-ROM
  vtable rename as a data loss (see Proof). Tracked as
  https://github.com/tangosdev/sm64ds-decomp/issues/2409, which must land alone
  and first; this candidate carries no workaround for it.
- Status: verified candidate (local proof below; the terminal private validator
  has not run on this commit).
- Remaining uncommitted/local-only material and where it is preserved: none.
  Private gate logs and the queue receipt live in this worktree's ignored
  `build/` directory and are not part of the candidate.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daGmch_c` in ov081, text
  `ov081:[0x02126504,0x02127b34)` (37 functions), own metadata
  `_ZTS8daGmch_c` at 0x02128bc8, `_ZTI8daGmch_c` at 0x02128bd4, vtable storage
  `ov081:[0x02128bfc,0x02128c80)` with the public address point at 0x02128c04.
- Reserved source/header/config surfaces actually touched by this commit:
  `src/actors/daGmch_c.cpp` and `include/daGmch_c.h` only. Two generated notes
  were re-derived because this edit changed what they measure (the TU's line
  count in `notes/data/tu-promotion-queue.tsv`, 1207 -> 1119; the strict
  CONVERTED count in `notes/cpp-tu-current-state.md`, 2698 -> 2700); each diff is
  exactly one line. Untouched and deliberately so: `include/decl_common.h`,
  `symbols/actor_renames.tsv`, `config/converted-baseline.json`,
  `config/converted-backslide-exceptions.jsonl`, `attribution.json`,
  `config/match_attempts.jsonl`, `config/match_provenance.jsonl`,
  `config/arm9/overlays/ov081/symbols.txt`, `config/arm9/overlays/ov081/delinks.txt`,
  the manifest.
- ROM observations: unchanged from the input and restated in the TU header and
  `include/daGmch_c.h` -- the RTTI name string, the `__si_class_type_info` record
  naming `dActor_c` as base, the 31-slot vtable with 10 overrides, `sizeof` 0x3f4
  from the factory's own literal, D1 below D0 with no D2 in the cartridge, and 64
  in-module relocations landing in the run (35 text, 18 pointer-to-member
  records, 10 vtable slots, 1 profile word), none from text outside it.
- Lineage evidence or structural inference: the field names used here are the
  ones `include/daGmch_c.h` and `include/dActor_c.h` already declared, plus
  three this commit adds from what the bytes do with the storage: `mHolder`
  (0x0d0, an actor pointer whose speed, heading and position EnterState7 reads
  and then clears; typed `dActor_c *` because those are the only fields read
  through it), `mSpinAngleY` (0x3ec, the Y rotation applied to `mModel`'s
  matrix, advanced 0xc00 per frame by UpdateState0) and `mTargetAngleY` (0x3ee,
  the heading UpdateState1 approaches and EnterState1 randomises). `mMatrix`
  becomes a real `Matrix4x3` (0x3a0, same 0x30 bytes). All three are inference
  from use, not recovered identifiers, and the header says so at each field.
- Hypothesized names/filenames, explicitly not recovered facts: the 26 member
  names for the auto-named functions, the factory spelling
  `daGmch_c_classInit`, the profile spelling `g_profile_GAMAGUCHI`, and the three
  field names above. The state handlers assert only a state index and an
  enter/update half; what any state means is not claimed.
- Compiler experiments and measured barriers: the field conversion was applied as
  one batch across 25 members and measured once -- `tubuild verify` 37/37 MATCH
  on the first compile, so no member needed a raw-offset fallback. Retained
  because the measurement says so, not from taste: the explicit read-modify-write
  spelling of `mSpinAngleY += 0xc00` in UpdateState0 (the compound form
  materialises the whole 0x3ec offset instead of the ROM's split address), the
  pointer-through-local spelling of the `mFlags` and `mPhase` updates in
  UpdateState0/EnterState3/EnterState7 (kept as the shards wrote them), and the
  `unsigned int` return of `RandomIntInternal` (the input's measurement: `int`
  breaks ChooseNextState alone). `mHolder` is re-read before each use in
  EnterState7 exactly as the shards reloaded it; the bytes keep the reloads.
  Nothing hit a compiler wall in this pass.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 37 of 37 functions byte-match
  with relocations resolved to the configured destinations (`tubuild verify`:
  match.py compare + objisolate plan + reloc_audit check_destinations, all
  clean); the full ROM build links the TU inside the ov081 image at 106/106
  modules exact. Not covered by this task's proof: the 4 BLIND and 1 DRAFT
  relocations in the validator's whole-tree sweep on the input commit belong to
  other files (RollingIronBall, daObjMarioCap_c, dScDSMT_c, dScStarSel_c), none
  in ov081.
- Genuine methods; remaining free-function/ABI bridges: 36 of 37 are
  `daGmch_c::` methods; `daGmch_c_classInit` stays a free function by design
  (the cartridge's own symbol is free, and it constructs rather than operates
  on an object). Calls into other classes: `Animation::Advance`/`Finished`,
  `ModelAnim::Render`/`Model::Render` (slot 5, virtual) and
  `Player::IsOnShell`/`IncMegaKillCount` are now real method calls; 50
  `extern "C"` bridge declarations remain (was 54), every one a callee whose own
  header does not yet declare it as a method (`dActor_c::UpdatePos`,
  `dCc_c::Clear`, `Player::Bounce`, `Player::Hurt`, ...). Each bridge is one
  row in the file-scope block; none was introduced by this task.
- Recovered layout/fields; remaining shadow structs/raw offsets: raw
  `(char *)this + 0xNNN` accesses went from about 240 to 6, and the 6 are all
  in the factory (member-object placement and the vptr store on a freshly
  allocated block, which is what a factory does). Shadow types went from 8 to 4:
  `Vec3_26e28` (the flat aggregate DetectRaycastClsn takes; `Vector3` has a
  declared destructor and is not one), the two pointer-to-member windows
  `C_27708`/`C_27744` that CallStateUpdate/CallStateEnter dispatch through, and
  `Bca2` (the two-word BCA record two entry states read). Two Player-side reads
  now go through `Player::actorID` and `Player::mIsMetal`.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged from the
  input. Out-of-line `~daGmch_c` under `#pragma defer_codegen off` emits D1, D0
  then a homeless D2 (licensed deadstrip); the destructor is the key function so
  this TU emits `_ZTV/_ZTI/_ZTS`, licensed as deadstrip-data against the
  cartridge copies at their `symbols.txt` addresses; `_ZN7Vector3D1Ev` is a
  licensed deadstrip-duplicate of arm9 0x020072c0. The TU owns no `.data`/`.bss`;
  the pointer-to-member band and the .bss mirror stay ROM-gap owned and the
  module static initializer stays enrolled outside the claimed range. The
  `extern int _ZTV8daGmch_c[];` declaration sits in the class header, the
  daEyBm_c form rather than the majority in-TU form of
  `notes/tu-promotion-conventions.md` section 2: the in-TU form raises the
  `langmode_audit` extern-vtable counter, which the ratchet refuses, and the
  header is the declaration site the pre-promotion shard already had through
  `decl_common.h`. Recorded as a deliberate divergence, not silently inherited.
- Attribution preserved through each move/rename: this commit moves and
  renames no symbol and no file. The fold itself (input commit) carries
  per-symbol `legacy_source` rows in the manifest for all 37 functions; the
  validator's run on the input reported 36 credit reassignments and 0 lost.
  `attribution.json` overrides are integration-lane state and were not written
  here.
- Remaining agreed issue scope: (1) integrate once
  https://github.com/tangosdev/sm64ds-decomp/issues/2409 lands, re-proving on
  the composed base; (2) bank or decline the +2 strict CONVERTED members
  (2698 -> 2700, unbanked, integration lane); (3) turn the remaining bridges
  into methods as their owning headers gain the declarations, `dActor_c::UpdatePos`,
  `Player::Bounce` and `Player::Hurt` first since they are the most called;
  (4) a typed factory body, if a form reproduces the vptr store; (5) the two
  pointer-to-member windows, if a `daGmch_c` member-pointer spelling reproduces
  mwccarm's representation.

## Proof

All checks below ran in a private wired worktree on this commit's tree
(`include/daGmch_c.h`, `src/actors/daGmch_c.cpp` and the two regenerated notes
staged; nothing else differs from the input), pinned compiler mwccarm 2004/b56,
base for `--changed`/ratchet comparisons `b2bd6a323832d244a12e750713b1405b26a94c66`.
Logs are private `build/` files named below; they are not committed.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom` -- exit 0. 11,191
  source-built functions reproducing, 0 mismatching; module fidelity 106/106
  exact, 100.000000% of compared bytes; ROM-build analysis PASS; intact TU gates
  "zero new symbol errors"; `dsd check symbols --fail` FAIL with the same 9
  pre-existing errors on baseline control and head (ITCM x4, `_deq`, two arm9
  `data_020ad5xx`, `overlay_100`/`overlay_102`), none in ov081. Run twice: once
  on the unmodified input (log `rombuild-2f34c64.log`, same figures) and once on
  this tree (`rombuild-batch2.log`).
- Explicit function/consumer relocation checks:
  `python tools/tubuild.py verify ov081/daGmch_c` -- exit 0, 37/37 MATCH,
  objisolate clean, reloc-destinations clean, emission order ROM-ascending,
  TEXT-VERIFIED; manifest byte-identical after the run (`verify-batch2.log`).
  `python tools/tu_order_check.py ov081/daGmch_c` -- exit 0, all 37 in ROM
  order, the two extras (`_ZN8daGmch_cD2Ev`, `_ZN7Vector3D1Ev`) licensed by the
  manifest. `python tools/prepush_linkcheck.py --files src/actors/daGmch_c.cpp`
  -- exit 0 but NOT a proof for this shape: it keys files by symbol and reports
  `NO-SYM` for a promoted TU (1 checked, 0 verified, 0 blocking). Consumers:
  `python tools/affected_src.py include/daGmch_c.h` (and the retired coined
  header Moneybag.h, which the input deletes) -- the only consumer is the TU
  itself.
- Complete emitted TU and data/metadata checks:
  `python tools/tubuild.py linkcheck ov081/daGmch_c` -- exit 1, NOT a source
  failure and not applicable to this route: it stops at step [4/8] with
  "intact TU preparation refused: intact production requires one .text claim
  and at least one non-text claim". This is a text-only promotion
  (`data: []`, `bss: []`, no `production_mode`), the manifest is byte-identical
  to the input, and for this route the whole-tree link proof is the full ROM
  build above, which links the TU inside the ov081 image under rombuild's own
  baseline control and compares every module. The manifest's stored
  `scratch-link-verified` block dates from before enrollment and is carried,
  not re-established, by this commit.
  `python tools/romdata_check.py --files src/actors/daGmch_c.cpp` -- exit 0; 9
  data symbols emitted, 5 VERIFIED (168 bytes equal), 4 PARTIAL (the ancestor
  `_ZTI/_ZTS` records, coverage short of the ROM extent), 0 DIFFERS, 0 UNNAMED
  (`gate-romdata.log`, `romdata-batch2.json`).
- Shared-header consumer expansion: `include/daGmch_c.h` has one includer (the
  TU), compiled by every check above.
  `python tools/check_header_offsets.py --changed b2bd6a32...` -- exit 0;
  `include/daGmch_c.h` 18 commented fields, 0 mismatched, struct spans 0x3f4;
  `include/decl_common.h` 0 mismatched.
- Port/path/reference and other applicable static gates (exit codes):
  `port_refcheck.py` 0 (402 checked, 0 stale);
  `check_rename_ledger.py --repo .` 0 (1,949 rows agree with `symbols.txt`);
  `check_dead_references.py` 0 (no new dead references, no broken links);
  `langmode_audit.py --check <origin/chaos-data:langmode-baseline.json>` 0
  (ratchet PASS); `tiers_ratchet.py --check` 0 (PASS, baseline 2698, current
  2700, +2 not banked); `queue_audit.py --check` 0 after `--write` re-derived
  this class's line count (one row); `cpp_tu_state.py --check-note` 0 after
  `--write-note` (one line); `check_src_tu_compiles.py --quiet` 0 (154/154);
  `layout_check.py` 0; `check_duplicate_sources.py` 0 (9,299 stems);
  `check_profile_campaign.py --repo .` 0; `cpp_tu_compat.py --require-ready` 0.
  Not run here: `eligible.py`, `check_references.py`, `check_data_definitions.py`,
  `source_coverage.py`, `check_tubuild_conflicts.py` (all ran green on the input
  per the PR record; not re-run on this commit).
- Private validation, if run, and the exact PR head/base it tested: NOT run on
  this commit. The most recent run is on the input, PR head
  `2f34c64aa90c712c0da8b936842592a39ad853cf` against main at
  2026-09-07 05:30 UTC: FAILED with exactly one reason, `ROM data verification
  lost 1 exact symbol(s): ov081:_ZTV8Moneybag`, while reporting module fidelity
  106/106 exact, byte-verified functions +0, relocation check 2,055 checked /
  2,050 VERIFIED / 4 BLIND / 1 DRAFT (none in ov081), port references 0 stale,
  and no `verified bytes fell` companion failure. The prediction on the issue is
  therefore confirmed by a result, not inherited. The failure is the gate's:
  `_data_symbol_set()` keys `verifiedSymbols` by `(module, symbol)` and
  `rom_data_regressions()` takes a raw base-minus-head set difference, so a
  symbol renamed at an unchanged address (`_ZTV8Moneybag` -> `_ZTV8daGmch_c`,
  0x02128c04, 0x84 bytes, verified in the head run) reads as a loss. Tracked
  as https://github.com/tangosdev/sm64ds-decomp/issues/2409. No alias row was
  added to `symbols.txt` and nothing consults `symbols/actor_renames.tsv` to
  satisfy the gate; neither will be.

A log generated after this commit belongs in separately recorded evidence. Do not
amend a proved candidate just to paste its own SHA or a later result into it.
