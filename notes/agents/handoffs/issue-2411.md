# Handoff: issue-2411

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  https://github.com/tangosdev/sm64ds-decomp/issues/2411, task `issue-2411`,
  stage `reconstruct` (role producer, second attempt after a rework), session
  `prod-2411b-0907`, Claude Code. The first attempt's session was
  `prod-2411-0907` and the independent verification that rejected it was
  `vfy-2411-0907`.
- Source branch and previous accepted input SHA: branch `cpp/da1up_c-v2b`;
  accepted input `39229e56110d805c876d7c51958cdf4f21bb4403` (the rejected
  candidate, tip of `cpp/da1up_c-v2`). That input in turn descends from
  `f38a8f74f584658e02e0ab4ae435a0fa2e949c5f` (the tip of the v1 branch
  `cpp/OneUpMushroom-tu`, PR https://github.com/tangosdev/sm64ds-decomp/pull/2398)
  by two merges of `origin/main`
  (`aec3fbf4305aef087adcffccd65af6bf368ba9a3` at `5af2bcc9e`, then
  `95a79f7ea068f9652b71cf09b7e2400dfa6b1f97` at `69d973f1259889d1ac588283e6cab54bd0f28190`);
  both merges were conflict-free and the second brought notes only.
- Scope of this revision: **this commit changes this handoff document and
  nothing else.** Source, headers, the TU manifest, `symbols.txt`, `delinks.txt`
  and `attribution.json` are byte-identical to the rejected input
  `39229e56110d805c876d7c51958cdf4f21bb4403`, so every byte, relocation and
  ROM-data measurement recorded under Proof was taken on a tree whose source
  and config are byte-identical to this one -- this document is the only
  difference -- and none of it is restated from an older or differently shaped
  tree. The revision corrects three statements the
  independent verification found unsupported (the `tubuild linkcheck` figure,
  the attribution coverage claim and the shadow-struct count) and records the
  attribution rows the integration lane owes. It deliberately does not move the
  source file; see the placement note under blockers.
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
  Integrator: the private validator's `rom_data_regressions` line on the
  predecessor was the defect tracked by
  https://github.com/tangosdev/sm64ds-decomp/issues/2409 -- it read the alias
  collapse `_ZTV13OneUpMushroom` -> `_ZTV7da1up_c` at the one address
  ov002:0x021083c8 as a lost symbol. **That defect is now fixed and landed on
  main** as commit `1c93d26632ff222f11bcf19d3e55860d6b6e66c8`
  (https://github.com/tangosdev/sm64ds-decomp/pull/2425), which anchors the
  ROM-data diff on `(module, address, bytes)` instead of the symbol name, so a
  rename reads as a rename. Confirmed present on `origin/main` by this session.
  This candidate is based on `69d973f125...`, which PREDATES that fix, so the
  integrator must compose it onto a base containing `1c93d2663` and must
  REGENERATE the base ROM report rather than serve a cached one: a base report
  built before `1c93d2663` still shows the phantom loss. Two workarounds remain
  forbidden -- do NOT re-add an alias row to
  `config/arm9/overlays/ov002/symbols.txt` and do NOT make the gate consult
  `symbols/actor_renames.tsv`; both would let this change certify its own rename.
- Source placement (raised as blocking by the first verification, overturned):
  the TU stays at `src/actors/da1up_c.cpp`. The measurement behind that ruling,
  made by the domain coordinator and posted on
  https://github.com/tangosdev/sm64ds-decomp/issues/2426, is that all 18 most
  recently landed promoted TUs on main sit at `src/actors/<Class>.cpp`, with
  none landing under `src/game/actors/` as `d_a_<snake>.cpp` since
  https://github.com/tangosdev/sm64ds-decomp/pull/2270; that
  `tools/srcpath.py`, the resolver this workflow designates, returns the flat
  path for this class (re-checked in this worktree:
  `python tools/srcpath.py _ZN7da1up_c6RenderEv` -> `src/actors/da1up_c.cpp`);
  and that no gate rejects `src/actors/`. The forward convention for new class
  TUs -- flat versus a nested `src/game/actors/` stem such as `d_a_1up` -- is
  being decided on issue #2426 and is not this task's to settle. A later
  reviewed convention change would move this file with every other one.
- Status: candidate for a fresh independent verification. Its predecessor
  `39229e56` was REJECTED, and it is worth being precise about what failed:
  every byte, relocation, emitted-object and ROM-data check passed on that
  commit and none of it was disputed. The rejection rested on the source
  placement (overturned above, on measurement, by the domain coordinator) and
  on three unsupported statements in this document (all three corrected here,
  each marked at the place it was corrected). Every check the producer and the
  independent verification could run locally is listed under Proof, including
  the one that is refused and the one gate whose row disagrees on unrelated
  pre-existing drift. The terminal private validation has not been run on this
  commit or its input; on the predecessor `f38a8f74` it failed only on the
  `rom_data_regressions` line now fixed by `1c93d2663`, and noted 8 contributor
  credit reassignments, which are tabulated below as owed integration work.
- Remaining uncommitted/local-only material and where it is preserved: none.
  Build state and the queue receipts are in the ignored `build/` of the
  worktrees `C:/tmp/sm64ds-a1up2411` (first producer attempt),
  `C:/tmp/sm64ds-vfy2411` (independent verification) and
  `C:/tmp/sm64ds-sm64ds-1up2411b` (this revision).

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
  Session `prod-2411-0907` added only this handoff document; session
  `prod-2411b-0907` (this revision) edited only this handoff document.
  `attribution.json` in particular is still NOT reconciled: the eight rows the
  integration lane owes are tabulated under Reconstruction dimensions, and this
  revision deliberately did not add them, because that file is global
  bookkeeping outside this task's reservation.
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
  byte-match with relocation destinations verified (`tubuild verify`); full ROM
  106/106 modules exact with 11,191 source-built functions and 0 mismatching.
  Factories 2/2 verified in the full build. Every relocation slot in those 36
  functions and in both factories was then linked and compared by symbol:
  `tools/linkcheck.py --module ov002 --name <sym> --addr <a> --size <s> --c
  <file>` driven once per symbol gives **38/38 VERIFIED, 0 BLIND, 0 diffs**.
  That measurement was made by the independent verification session
  `vfy-2411-0907` in `C:/tmp/sm64ds-vfy2411` against this tree; it is the same
  tool `prepush_linkcheck` wraps, driven by symbol instead of by file stem, and
  it is what closes the `NO-SYM` gap the stem-keyed wrapper leaves on a
  multi-symbol promoted TU (see the range-link entry under Proof).
  NOT a claim of this candidate: `tubuild linkcheck ov002/da1up_c` is REFUSED on
  this commit (exit 1 at step [4/8]; reproduced by this session, see Proof).
  The "whole range `[0x020aee40,0x020b0530)`, 0 differing bytes" figure that
  earlier revisions of this document quoted for it is the PRE-PROMOTION scratch
  link: it is the `verification.linkcheck` record inside
  `config/tu_manifest.d/ov002/da1up_c.json`, measured against the shadow TU
  before promotion and written into the manifest by commit `557b9a797`
  (2026-09-06). It is a historical record of a different tree shape, it cannot
  be re-run on a promoted `.text`-only entry at this tool revision, and it is
  not offered as proof of this commit. The proof that covers that range here is
  the 106/106 module compare plus the 38/38 by-symbol link verification above.
  Coverage limit: the private validator on the predecessor reported 4 BLIND +
  1 DRAFT among 2,057 relocation checks tree-wide; the rows its per-file table
  names are shards of `dScStarSel_c` (draft), `RollingIronBall`,
  `daObjMarioCap_c` and `dScDSMT_c` (blind), none of them in this TU's range,
  and this change does not resolve them.
- Genuine methods; remaining free-function/ABI bridges: 9 of 36 symbols are
  `da1up_c::` members (8 definitions; the destructor is one definition emitting
  D1 and D0). 27 remain `extern "C"` free functions taking `char *`/`void *`
  `this` bridges: 14 of them are ROM-proven non-static members (the PMF
  descriptors) whose NAMES are unknown, and 13 are helpers with no member-ness
  evidence. This is a naming/evidence wall, not one measured compiler wall.
- Recovered layout/fields; remaining shadow structs/raw offsets: the header
  is a real `struct da1up_c : dEnemyBase_c` closing at 0x398 with a size
  assert; the eight methods use member names. The 27 free bodies still carry
  raw `(base + 0xNN)` accesses -- 139 over 39 distinct offsets by this
  session's regex, 172 over 42 by the independent verification's; the count is
  regex-dependent and only its order of magnitude should be relied on -- plus
  32 block-scope mangled `extern` declarations and **three** local shadow
  structs, not two: `C` (line 1042, the opaque pointer-to-member holder),
  `Obj` (line 1050, a six-slot vtable-shape stand-in so ordinal 33 can reach
  the model's slot 5 by index) and `ModelCache` (line 1061, ordinal 35's view
  of `data_ov002_0210d9b8`). Each carries a comment naming what is proven about
  it. `Behavior` indexes the dispatch array through a pointer-to-member on a
  cast `this`.
- Lifecycle, vtable/RTTI, initializer and data ownership: destructor
  out-of-line first-declared = key function; this TU emits `_ZTV7da1up_c`,
  `_ZTI7da1up_c`, `_ZTS7da1up_c` and the ancestor chain's RTTI as compiler-only
  passengers (manifest `compiler_only_output`, each with its ROM home). The full
  build's ROM-data measurement verifies `_ZTV7da1up_c`, `_ZTI7da1up_c` and
  `_ZTI12dEnemyBase_c` at ov002 from this object. The sinit, descriptors, bss
  array and profiles are not owned by the TU.
- Attribution preserved through each move/rename: **partially, and the exact
  gap is measured below.** `attribution.json` carries
  `src/actors/da1up_c.cpp#<symbol>` overrides for **26 of the 36** folded
  shards (commit `f38a8f74` on the input branch) -- 33 rows on that path in
  all, because the 7 renamed members each carry a row under both the old
  `OneUpMushroom` and the new `da1up_c` mangled spelling, and the 19
  `func_ov002_*` bodies carry one each. An earlier revision of this document
  said "every folded shard"; that was wrong. `python tools/prepush_attribution.py --base
  69d973f1259889d1ac588283e6cab54bd0f28190 --head HEAD` reports `9720 tracked,
  26 consolidated with credit intact, 0 changed, 0 lost` -- the 26 with rows.
  The remaining **10** symbols have no override row and fall back through
  `chaos_db_ci.match_finishers` / `first_matchers` on a source path the fold
  deleted, so the private validator resolves them to the TU's adder
  (`andrewboudreau`). They are `_ZN7da1up_cD0Ev`, `_ZN7da1up_c8BehaviorEv`,
  `func_ov002_020aeee4`, `func_ov002_020aefa4`, `func_ov002_020aefb8`,
  `func_ov002_020af724`, `func_ov002_020af950`, `func_ov002_020afc68`,
  `func_ov002_020afd10` and `func_ov002_020aff10`. For two of them
  (`func_ov002_020aeee4`, `func_ov002_020afd10`) the base matcher IS the adder,
  so nothing moves. For the other **8** the base matcher is someone else, and
  those 8 are exactly the "8 changed credits" the predecessor's private
  validation reported. This is owed work, not a resolved item.
  `attribution.json` is global bookkeeping, is NOT among this task's reserved
  resources, and was deliberately left untouched by this commit; the integration
  lane applies the rows below. Each base author was recomputed in this worktree
  with the validator's own formula (`overrides[path#symbol]` else
  `overrides[path]` else `match_finishers[path]` else `first_matchers[path]`,
  evaluated at base `69d973f125...`), which reproduces the count of 8 exactly.
  Each old shard path is recorded per function as `legacy_source` in
  `config/tu_manifest.d/ov002/da1up_c.json`.

  | # | Ordinal | Address | Override key to add | Credit owed to | Where the base credit comes from |
  |---|---|---|---|---|---|
  | 1 | 1 | 0x020aee88 | `src/actors/da1up_c.cpp#_ZN7da1up_cD0Ev` | `tangosdev` | an existing `attribution.json` path override, now keyed on a shard path the fold deleted |
  | 2 | 3 | 0x020aefa4 | `src/actors/da1up_c.cpp#func_ov002_020aefa4` | `ruspecial` | an existing `attribution.json` path override, now keyed on a shard path the fold deleted |
  | 3 | 4 | 0x020aefb8 | `src/actors/da1up_c.cpp#func_ov002_020aefb8` | `lunavyqo` | `match_finishers` on the deleted shard |
  | 4 | 15 | 0x020af724 | `src/actors/da1up_c.cpp#func_ov002_020af724` | `tangosdev` | `match_finishers` on the deleted shard |
  | 5 | 20 | 0x020af950 | `src/actors/da1up_c.cpp#func_ov002_020af950` | `tangosdev` | `match_finishers` on the deleted shard |
  | 6 | 26 | 0x020afc68 | `src/actors/da1up_c.cpp#func_ov002_020afc68` | `tangosdev` | `match_finishers` on the deleted shard |
  | 7 | 30 | 0x020aff10 | `src/actors/da1up_c.cpp#func_ov002_020aff10` | `tangosdev` | `match_finishers` on the deleted shard |
  | 8 | 34 | 0x020b00e8 | `src/actors/da1up_c.cpp#_ZN7da1up_c8BehaviorEv` | `alexsobolew7` | `match_finishers` on the deleted shard |

  Rows 1 and 2 are the two whose credit currently survives only through an
  `attribution.json` row keyed on a path this promotion deleted; re-key those
  onto the TU rather than adding a second row, so no stale key is left behind.
  Three named contributors other than the project account are owed credit here
  -- `ruspecial` (1), `lunavyqo` (1) and `alexsobolew7` (1) -- and `tangosdev`
  is owed the other 5. For completeness the integrator may also add rows for
  `func_ov002_020aeee4` (ordinal 2, 0x020aeee4) and `func_ov002_020afd10`
  (ordinal 27, 0x020afd10), both `andrewboudreau`: no credit moves today, but a
  row makes them durable against a later move of this file. That would bring
  the TU to 36/36 explicit rows.
- Remaining agreed issue scope: name the 14 dispatch members and the 13 helpers
  only if evidence appears (none in the ROM); replace raw offsets in the 27 free
  bodies with member access without moving bytes; give the dispatch array and
  descriptors a typed owner. None of that is required for this candidate.

## Proof

Unless an entry says otherwise, the commands ran in `C:/tmp/sm64ds-a1up2411`
(producer session `prod-2411-0907`) on this commit's tree -- source and config
identical to `95a79f7ea068f9652b71cf09b7e2400dfa6b1f97`, and identical again to
the rejected input `39229e56`, since this revision changes only this document --
with pinned compiler mwccarm 2004/b56 and base
`69d973f1259889d1ac588283e6cab54bd0f28190`. Entries marked **[verifier]** were
measured independently by session `vfy-2411-0907` in `C:/tmp/sm64ds-vfy2411` on
the same tree; entries marked **[this revision]** were measured by session
`prod-2411b-0907` in `C:/tmp/sm64ds-sm64ds-1up2411b`. Logs are in each
worktree's ignored `build/`; they are not committed.

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
  from `tubuild verify` (below) and from the by-symbol run in the next entry.
  Log `build/linkcheck.log`.
- Per-symbol link verification, which is what closes that `NO-SYM` gap
  **[verifier]**: `python tools/linkcheck.py --module ov002 --name <sym>
  --addr <a> --size <s> --c <file>` driven once for each of the 36 TU functions
  against `src/actors/da1up_c.cpp` and once for each of the two factories
  against `src/d_a_1up_oneupkinoko.c` / `src/d_a_1up_scaleup_kinoko.c` -> exit 0,
  **38/38 VERIFIED, 0 BLIND, 0 diffs**: every relocation slot in all 38 symbols
  links to bytes identical to the cartridge. Same tool `prepush_linkcheck`
  wraps, driven by symbol rather than by the file stem that produced `NO-SYM`
  (`prepush_linkcheck.py:110` keys `_load_symbol(Path(path).stem)`, and a
  promoted TU's stem is not a symbol). Logs
  `C:/tmp/sm64ds-vfy2411/build/vfy-linkcheck-per-function.log` and
  `build/vfy-linkcheck-per-function.json`.
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
  `python tools/tubuild.py linkcheck ov002/da1up_c` -> **exit 1, REFUSED** at
  step `[4/8]` before any byte comparison: `FAIL src/actors/da1up_c.cpp:
  isolate: intact TU preparation refused: ov002/da1up_c: intact production
  requires one .text claim and at least one non-text claim`. Re-run and
  reproduced verbatim **[this revision]** at this commit. This is a tool-shape
  limit, not a byte result: the scratch link forces the candidate through
  `tu_production.prepare_intact_object`, which needs a non-text claim, while
  this TU deliberately claims `.text` only (vtable, RTTI, descriptors and bss
  stay with their shards). The production build's intact policy is opt-in
  (`production_mode: intact-object`), so `rombuild.py` compiled this TU on the
  ordinary route and the whole range `[0x020aee40,0x020b0530)` is inside its
  `106/106 exact`. **There is therefore NO `tubuild linkcheck` result for this
  commit, and none is claimed.** The 0-differing-bytes figure over that range,
  with ROM sha256
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, is the
  PRE-PROMOTION scratch link: it was measured on the shard-shaped tree before
  the fold ("LINKCHECK (pre-promotion, the only usable kind)" in that commit's
  own message) and written into `verification.linkcheck` of
  `config/tu_manifest.d/ov002/da1up_c.json` by commit `557b9a797` on
  2026-09-06, the promotion commit -- the only commit that has ever touched
  that manifest. It is a historical record of a different tree shape and is not
  re-runnable on a promoted `.text`-only entry at this tool revision. What
  covers that range for THIS commit is the 106/106 module compare plus the
  38/38 by-symbol link verification above. Logs
  `build/tubuild-linkcheck.log` and `build/tubuild-linkcheck-rerun.log`.
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
- Contributor attribution: `python tools/prepush_attribution.py --base
  69d973f1259889d1ac588283e6cab54bd0f28190 --head HEAD --json
  build/attrib.json` -> exit 0, `9720 tracked, 0 moved with credit intact,
  0 renamed with credit intact, 26 consolidated with credit intact, 0 changed,
  0 lost`. Re-run **[this revision]**; the verifier ran the same command with
  the same result. Coverage limit, and this is the point: this gate only sees
  the 26 shards that HAVE an `attribution.json` override row. The other 10 do
  not appear in its ledger at all, so `0 changed` here does not contradict the
  private validator's 8 reassignments -- the two tools disagree because they
  measure different sets. The 8 were recomputed **[this revision]** with the
  validator's own resolution order against base `69d973f125...` and are
  tabulated under Reconstruction dimensions. `attribution.json` is unreserved
  global bookkeeping and was not edited by this commit. Log
  `build/attrib.json`.
- Gates re-run for THIS revision, and only these **[this revision]**: this
  commit changes one Markdown file under `notes/` and no source, header,
  manifest, `symbols.txt` or `delinks.txt`, so the byte, relocation and
  ROM-data evidence above was NOT re-measured -- it was taken on the identical
  tree and stands unchanged. What was re-run is what a prose edit can plausibly
  break, plus the two claims being corrected:
  `python tools/check_dead_references.py` -> exit 0, `10329 scan target(s) ...
  896 markdown link(s), 0 that do not resolve relative to their own file`,
  `no new dead references`, `no broken markdown links` (it also notes that 3
  already-baselined prose references now resolve, which is not a failure and is
  left for a separate baseline-shrinking change);
  `python tools/check_python_names.py` -> exit 0, `278 tracked file(s) checked,
  0 unresolvable names`;
  `python tools/tubuild.py linkcheck ov002/da1up_c` -> exit 1, the documented
  refusal, reproduced verbatim; `python tools/prepush_attribution.py` as above;
  `python tools/srcpath.py _ZN7da1up_c6RenderEv` -> `src/actors/da1up_c.cpp`.
  No full ROM rebuild, no `tubuild verify`, no `romdata_check` and no
  `prepush_linkcheck` run belongs to this revision, and none is claimed for it.
- Private validation, if run, and the exact PR head/base it tested: not run on
  this commit. On the predecessor (PR head `f38a8f74f584658e02e0ab4ae435a0fa2e949c5f`,
  https://github.com/tangosdev/sm64ds-decomp/pull/2398#issuecomment-5565596161)
  every check passed except the literal line
  `Validation failed: ROM data verification lost 1 exact symbol(s): ov002:_ZTV13OneUpMushroom`,
  plus a non-blocking `8 contributor credit reassignment(s)` note -- the 8 rows
  tabulated above. The failing line was reproduced locally against the real
  gate function: feeding `validate_merge.rom_data_regressions` a base row
  `(ov002, _ZTV13OneUpMushroom)` and a head row `(ov002, _ZTV7da1up_c)` at the
  same address returned exactly that message, because the old `_data_symbol_set`
  keyed identity by `(module, symbol)` and set-subtracted. That was the defect
  tracked by https://github.com/tangosdev/sm64ds-decomp/issues/2409, not a
  defect in this source: this commit's own full build verifies `_ZTV7da1up_c`
  at ov002 and the cartridge holds one vtable at 0x021083c8. **That defect is
  now fixed on main** by `1c93d26632ff222f11bcf19d3e55860d6b6e66c8`
  (https://github.com/tangosdev/sm64ds-decomp/pull/2425), which anchors the
  ROM-data diff on `(module, address, bytes)`; confirmed an ancestor of
  `origin/main` **[this revision]**. The next validation must run on a base
  containing it, with the base ROM report regenerated rather than served from a
  per-SHA cache built before it.

A log generated after this commit belongs in separately recorded evidence. Do not
amend a proved candidate just to paste its own SHA or a later result into it.
