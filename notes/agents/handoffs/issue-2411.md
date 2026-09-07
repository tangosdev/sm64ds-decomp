# Handoff: issue-2411

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  https://github.com/tangosdev/sm64ds-decomp/issues/2411, task `issue-2411`,
  stage `reconstruct` (role producer), session `prod-2411-0907`, Claude Code.
- Source branch and previous accepted input SHA: branch `cpp/da1up_c-v2`;
  accepted input `f38a8f74f584658e02e0ab4ae435a0fa2e949c5f` (the tip of the v1
  branch `cpp/OneUpMushroom-tu`, PR https://github.com/tangosdev/sm64ds-decomp/pull/2398).
  This commit descends from that input by two merges of `origin/main`
  (`aec3fbf4305aef087adcffccd65af6bf368ba9a3` at `5af2bcc9e`, then
  `95a79f7ea068f9652b71cf09b7e2400dfa6b1f97` at `69d973f1259889d1ac588283e6cab54bd0f28190`);
  both merges were conflict-free and the second brought notes only.
- Original source base SHA and installed workflow/tool SHA: source base
  `b2bd6a323832d244a12e750713b1405b26a94c66` (the earlier duplicate issue
  https://github.com/tangosdev/sm64ds-decomp/issues/2405 recorded
  `2ab05bae36a6821679a7e647e811c0a3243a3c49`); workflow/tool commit
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`. The tested base for every
  proof below is `69d973f1259889d1ac588283e6cab54bd0f28190`.
- Separate evidence commits and required artifacts in this commit: no separate
  evidence commit. Scout facts are in this commit at
  `notes/data/class-facts/OneUpMushroom.json` (file named for the old queue
  row; its `class` field is `da1up_c`). Required artifacts present:
  `src/actors/da1up_c.cpp`, `include/da1up_c.h`,
  `config/tu_manifest.d/ov002/da1up_c.json`.
- Next action, responsible role and blockers: verifier claims the `verify`
  stage on this exact commit and re-runs the proof below in its own worktree.
  Integrator: the private validator's `rom_data_regressions` line is a KNOWN
  DEFECTIVE GATE (https://github.com/tangosdev/sm64ds-decomp/issues/2409): it
  reads the alias collapse `_ZTV13OneUpMushroom` -> `_ZTV7da1up_c` at the one
  address ov002:0x021083c8 as a lost symbol. Land #2409 first, then make sure
  the base ROM report is regenerated rather than served from a per-SHA cache,
  then validate this candidate. Do NOT re-add an alias row to
  `config/arm9/overlays/ov002/symbols.txt` and do NOT make the gate consult
  `symbols/actor_renames.tsv`; both would let this change certify its own rename.
- Status: verified candidate for every check this producer can run locally
  (listed under Proof). The terminal private validation has not been run on
  this commit; on the predecessor it failed only on the defective line quoted
  under Proof.
- Remaining uncommitted/local-only material and where it is preserved: none.
  Build state and the queue receipt are in the ignored `build/` of the worktree
  `C:/tmp/sm64ds-a1up2411`.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `da1up_c`, ov002, the
  contiguous `.text` run `ov002:[0x020aee40,0x020b0530)` (36 functions, ROM
  ordinals 0..35) promoted into `src/actors/da1up_c.cpp`. The two factories
  `ov002:[0x020b0530,0x020b05d0)` stay in `src/d_a_1up_scaleup_kinoko.c` and
  `src/d_a_1up_oneupkinoko.c`; only their vtable reference moved to the ROM name.
  Data of this class (`_ZTV7da1up_c` 0x021083c8, `_ZTI7da1up_c` 0x0210837c,
  `_ZTS7da1up_c` 0x02108370, the two profile records, the 14 PMF descriptors at
  0x02108300..0x02108370 and the bss dispatch array at 0x0210dc00) remains owned
  by the existing shards; the TU claims `.text` only.
- Reserved source/header/config surfaces actually touched:
  `src/actors/da1up_c.cpp` (new), `include/da1up_c.h` (new; replaces the coined
  header OneUpMushroom.h, deleted), `config/tu_manifest.d/ov002/da1up_c.json`,
  `config/arm9/overlays/ov002/delinks.txt`,
  `config/arm9/overlays/ov002/symbols.txt` (nine mangled members renamed at their
  unchanged addresses and sizes; the coined `_ZTV13OneUpMushroom` alias row at
  0x021083c8 dropped, leaving `_ZTV7da1up_c` there), `include/daTBasket_c.h`
  (comment-only: the sibling's prose named OneUpMushroom).
  Not reserved, inherited unchanged from the accepted input and NOT reconciled
  here: `attribution.json`, `symbols/actor_renames.tsv`,
  `config/converted-baseline.json`, `config/converted-backslide-exceptions.jsonl`,
  `config/match_attempts.jsonl`, `config/match_provenance.jsonl`,
  `include/decl_common.h` (one stale `extern int _ZTV13OneUpMushroom[];` removed).
  This session added only this handoff document.
- ROM observations: `7da1up_c\0` at ov002 file offset 0x5ad10 == 0x02108370;
  `_ZTI7da1up_c` is a `__si_class_type_info` whose +8 word is
  `_ZTI12dEnemyBase_c` (0x021081c0); both factories allocate 0x398 through
  `fBase_c::operator new` and store `_ZTV7da1up_c`; the vtable is 31 slots with
  nine own overrides (0, 3, 6, 9, 12, 16, 17, 18, 19); D1 (0x020aee40) precedes
  D0 (0x020aee88), no D2; fourteen `{fn, 0}` pointer-to-member records at
  0x02108300 are copied by `__sinit_ov002_02100adc` into a 14-element array
  that only `Behavior` reads.
- Lineage evidence or structural inference: base `dEnemyBase_c` from RTTI;
  member layout 0x110 dCcAc_c / 0x144 dBgCh_Actr / 0x300 Model / 0x350
  ShadowModel from both factories and the destructor agreeing; tail fields
  0x378..0x394 from displacement census (base register not proven to be `this`).
- Hypothesized names/filenames, explicitly not recovered facts: every field
  name in `include/da1up_c.h`; the `da1up_c_classInit_*` and `g_profile_*`
  spellings (the KINOKO ids are ROM strings, the affixes are later EAD lineage);
  no original filename claimed. The 27 `func_ov002_*` names are address-derived,
  not names.
- Compiler experiments and measured barriers (all inherited from the accepted
  input's file header, re-proved here by `tubuild verify`): `#pragma
  defer_codegen off` with ROM-ascending source order is load-bearing; the
  destructor is out of line and declared first so this TU owns the key function
  and emits D1, D0 then a dead-stripped D2; `include/decl_common.h` cannot be
  included (four of its declarations contradict byte-matched definitions);
  ordinal 3 must stay `void` (an `int` return costs four of five words);
  `_ZN8dActor_c12ReflectAngleE5Fix12IiES1_s` keeps by-value Fix12 spelled as
  `int` at the call site.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 36/36 functions of the run
  byte-match with relocation destinations verified (`tubuild verify`); whole
  range `[0x020aee40,0x020b0530)` links with 0 differing bytes
  (`tubuild linkcheck`); full ROM 106/106 modules exact. Factories 2/2 verified
  in the full build. Coverage limit: the private validator on the predecessor
  reported 4 BLIND + 1 DRAFT among 2,057 relocation checks tree-wide; the rows
  its per-file table names are shards of `dScStarSel_c` (draft),
  `RollingIronBall`, `daObjMarioCap_c` and `dScDSMT_c` (blind), none of them in
  this TU's range, and this change does not resolve them.
- Genuine methods; remaining free-function/ABI bridges: 9 of 36 symbols are
  `da1up_c::` members (8 definitions; the destructor is one definition emitting
  D1 and D0). 27 remain `extern "C"` free functions taking `char *`/`void *`
  `this` bridges: 14 of them are ROM-proven non-static members (the PMF
  descriptors) whose NAMES are unknown, and 13 are helpers with no member-ness
  evidence. This is a naming/evidence wall, not one measured compiler wall.
- Recovered layout/fields; remaining shadow structs/raw offsets: the header
  is a real `struct da1up_c : dEnemyBase_c` closing at 0x398 with a size
  assert; the eight methods use member names. The 27 free bodies still carry
  139 raw `(base + 0xNN)` accesses over 39 distinct offsets, 32 block-scope
  mangled `extern` declarations and two local shadow structs (`ModelCache`, the
  PMF holder `C`). `Behavior` indexes the dispatch array through a
  pointer-to-member on a cast `this`.
- Lifecycle, vtable/RTTI, initializer and data ownership: destructor
  out-of-line first-declared = key function; this TU emits `_ZTV7da1up_c`,
  `_ZTI7da1up_c`, `_ZTS7da1up_c` and the ancestor chain's RTTI as compiler-only
  passengers (manifest `compiler_only_output`, each with its ROM home). The full
  build's ROM-data measurement verifies `_ZTV7da1up_c`, `_ZTI7da1up_c` and
  `_ZTI12dEnemyBase_c` at ov002 from this object. The sinit, descriptors, bss
  array and profiles are not owned by the TU.
- Attribution preserved through each move/rename: `attribution.json` carries
  `src/actors/da1up_c.cpp#<symbol>` overrides for every folded shard under both
  the old and new mangled spellings (commit `f38a8f74` on the input branch). The
  predecessor's private validation reported 0 lost, 8 changed credits; the
  verifier should confirm that count does not rise on this commit.
- Remaining agreed issue scope: name the 14 dispatch members and the 13 helpers
  only if evidence appears (none in the ROM); replace raw offsets in the 27 free
  bodies with member access without moving bytes; give the dispatch array and
  descriptors a typed owner. None of that is required for this candidate.

## Proof

All commands ran in `C:/tmp/sm64ds-a1up2411` at this commit's tree (source and
config identical to `95a79f7ea068f9652b71cf09b7e2400dfa6b1f97`; this handoff
document is the only later addition), pinned compiler mwccarm 2004/b56, base
`69d973f1259889d1ac588283e6cab54bd0f28190`. Logs are in the worktree's ignored
`build/`; they are not committed.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom` -> exit 0.
  `module fidelity: 106/106 exact, 100.000000% of compared bytes`;
  `source-built functions: 11,191 ... mismatching: 0`; `source-owned data
  claims: 20 (reproducing 20)`; `ROM data from source: 686 symbol(s) verified,
  215 partial, 5 differ`. The BASELINE CONTROL step's `dsd check symbols --fail`
  reports 9 pre-existing ITCM/overlay-table symbols; that run has no TU
  substitution and the tool labels it tree-owned. Log `build/rombuild.log`,
  report `build/rombuild-report.json`.
- Explicit function/consumer relocation checks:
  `python tools/prepush_linkcheck.py --range origin/main..HEAD` -> exit 0;
  `3 changed header(s) fan out to 928 source file(s)`; `927 checked - 629
  verified, 298 warning(s), 0 blocking`, 1 NONMATCHING draft skipped. The five
  `daTBasket_c` consumers of `include/daTBasket_c.h` are VERIFIED. Coverage
  limit: this tool keys a file by its stem, so `src/actors/da1up_c.cpp` and the
  two factory files report `NO-SYM` (a warning, not a verdict) like every other
  promoted TU and `d_a_*` factory in the tree; the three BLIND rows are the
  same `RollingIronBall`, `daObjMarioCap_c` and `dScDSMT_c` shards the private
  validator names. Relocation proof for this TU's 36 functions therefore comes
  from `tubuild verify` (below), whose reloc-destination audit is module-aware.
  Log `build/linkcheck.log`.
- Complete emitted TU and data/metadata checks:
  `python tools/tubuild.py verify ov002/da1up_c` -> exit 0: `byte comparison:
  36/36 MATCH (relocation-aware)`, `objisolate check: clean` (relocation
  type/addend), `emission order: all 36 function(s) in the expected
  ROM-ascending section order`, reloc-destinations clean ->
  `TEXT-VERIFIED`. Log `build/tubuild-verify.log`.
  `python tools/romdata_check.py --files src/actors/da1up_c.cpp
  src/d_a_1up_oneupkinoko.c src/d_a_1up_scaleup_kinoko.c --json
  build/romdata-da1up.json` -> exit 0: 11 data symbols emitted, `VERIFIED 6
  (180 bytes equal to the cartridge)`, `PARTIAL 5 (44 bytes equal, coverage
  short of the ROM's extent)`, `DIFFERS 0`, `UNNAMED 0`. The six VERIFIED are
  `_ZTV7da1up_c`, `_ZTI7da1up_c`, `_ZTI12dEnemyBase_c` (ov002) and
  `_ZTI8dActor_c`, `_ZTI7dBase_c`, `_ZTI7fBase_c` (arm9); the five PARTIAL are
  by elimination the five `_ZTS` type-name strings of that chain (the 11
  emitted names are the manifest's `compiler_only_output` list). Nothing
  differs from the cartridge.
  `python tools/tubuild.py linkcheck ov002/da1up_c` -> exit 1, REFUSED at
  step 4/8 before any byte comparison: `intact TU preparation refused:
  ov002/da1up_c: intact production requires one .text claim and at least one
  non-text claim`. This is a tool-shape limit, not a byte result: the scratch
  link forces the candidate through `tu_production.prepare_intact_object`,
  which needs a non-text claim, while this TU deliberately claims `.text`
  only (vtable, RTTI, descriptors and bss stay with their shards). The
  production build's intact policy is opt-in (`production_mode:
  intact-object`), so `rombuild.py` compiled this TU on the ordinary route and
  the whole range `[0x020aee40,0x020b0530)` is inside its `106/106 exact`.
  The pre-promotion scratch link recorded in the manifest (0 differing bytes
  over the range, ROM sha256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`)
  was made when the source still lived under `src_tu/`; it is not re-runnable
  in that mode on a promoted entry. Log `build/tubuild-linkcheck.log`.
- Shared-header consumer expansion: `python tools/affected_src.py
  include/da1up_c.h` -> `src/actors/da1up_c.cpp` only; `include/daTBasket_c.h`
  -> five daTBasket_c shards (the edit is comment-only); both headers pass
  `python tools/check_header_offsets.py include/da1up_c.h include/daTBasket_c.h`
  -> exit 0 (13 and 7 commented fields, 0 mismatched, spans 0x398 / 0x380).
  The consumers are covered by the range link check above. `include/decl_common.h`
  loses one dead extern; its consumers are covered by the full build.
- Port/path/reference and other applicable static gates (each exit 0 unless
  stated): `tools/port_refcheck.py` (402 checked, 0 stale);
  `tools/check_dead_references.py` (no new dead references, no broken links);
  `tools/check_rename_ledger.py --repo .` (1933 rows agree);
  `tools/check_src_tu.py --json build/src_tu_refs.json` (every reference
  resolves); `tools/check_python_names.py`; `tools/check_tubuild_conflicts.py
  --list`; `tools/check_profile_campaign.py --repo .` (0 diverge);
  `tools/tiers_ratchet.py --check` (CONVERTED 2696/2696);
  `tools/langmode_audit.py`; `tools/layout_check.py --quiet` (clean);
  `tools/cpp_tu_state.py --check-note` (note current);
  `tools/queue_audit.py --check` -> exit 1 on ONE row, `dScMgAmida_c` (ov006),
  which is drift already on `main` and outside this task's reservation; the
  `da1up_c` row agrees with the tree (verified by regenerating with `--write`,
  reading the diff, and reverting). `tools/tu_map.py` was regenerated from
  fresh `rtti.json`/`rtti_vtables.json` and reports 691 TUs / 333 with a class.
- Private validation, if run, and the exact PR head/base it tested: not run on
  this commit. On the predecessor (PR head `f38a8f74f584658e02e0ab4ae435a0fa2e949c5f`,
  https://github.com/tangosdev/sm64ds-decomp/pull/2398#issuecomment-5565596161)
  every check passed except the literal line
  `Validation failed: ROM data verification lost 1 exact symbol(s): ov002:_ZTV13OneUpMushroom`.
  Reproduced locally against the real gate function: feeding
  `validate_merge.rom_data_regressions` a base row `(ov002, _ZTV13OneUpMushroom)`
  and a head row `(ov002, _ZTV7da1up_c)` at the same address returns exactly
  that message, because `_data_symbol_set` keys identity by `(module, symbol)`
  and set-subtracts. That is the defect tracked by
  https://github.com/tangosdev/sm64ds-decomp/issues/2409, not a defect in this
  source: this commit's own full build verifies `_ZTV7da1up_c` at ov002 and the
  cartridge holds one vtable at 0x021083c8.

A log generated after this commit belongs in separately recorded evidence. Do not
amend a proved candidate just to paste its own SHA or a later result into it.
