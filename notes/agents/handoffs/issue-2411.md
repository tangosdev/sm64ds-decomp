# Handoff: issue-2411

This document preserves the source-stage handoff and later integration notes.
The queue records immutable stage outputs; source acceptance does not mean every
source-reconstruction opportunity below is complete.

## Integration status through 19df96cc21d5e2a87a97a22d4de67191319505bd

The accepted source output is `bb5bbfae682dccfc1cdb0aab0defdd702ad919c9`.
The eight attribution reassignments described as owed in the preserved record
were reconciled by `e3d7f6e6633078ace023d8478002c36526996933`: ten member
rows cover eight symbols, including both spellings for D0 and Behavior, and the
two obsolete bare-path overrides were removed. Behavior retains the raw handle
`alexsobolew7`, which the aliases map resolves to `ruspecial`. The two unchanged
credits without explicit member rows remain a durability opportunity, not an
outstanding reassignment.

The producer-time attribution counts and recipes below are retained as historical
evidence. Their present-tense statements that eight credits are still owed are
superseded by that integration commit. This note does not restate old build logs
as a new validation result.

The source remains a partial reconstruction: nine existing native method symbols
were consolidated and renamed, while 27 free-function bodies and several ABI
bridges remain. The non-void `func_ov002_020af684` still has no return statement;
reproducing residual register contents has not established a valid C++ return
contract. The inherited `OnTurnIntoEgg` signature also needs caller-use evidence.
These source questions require an owned follow-up stage and fresh independent
proof. The documentation corrections in this revision do not resolve them.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  https://github.com/tangosdev/sm64ds-decomp/issues/2411, task `issue-2411`,
  stage `reconstruct` (role producer, third attempt), session
  `prod-2411c-0907`, Claude Code. The earlier producer sessions were
  `prod-2411-0907` (first attempt) and `prod-2411b-0907` (second); the
  independent verifications were `vfy-2411-0907` (rejected the first attempt)
  and the session whose report is
  https://github.com/tangosdev/sm64ds-decomp/issues/2411#issuecomment-5572536511
  (returned the second attempt for these four text corrections).
- Source branch and previous accepted input SHA: branch `cpp/da1up_c-v2c`;
  accepted input `9c361560ff07147d1944597fbf0e5a22de8638cf` (tip of
  `cpp/da1up_c-v2b`). That input is a single-parent child of
  `39229e56110d805c876d7c51958cdf4f21bb4403` (tip of `cpp/da1up_c-v2`), which
  in turn descends from `f38a8f74f584658e02e0ab4ae435a0fa2e949c5f` (the tip of
  the v1 branch `cpp/OneUpMushroom-tu`, PR
  https://github.com/tangosdev/sm64ds-decomp/pull/2398) by two merges of
  `origin/main` (`aec3fbf4305aef087adcffccd65af6bf368ba9a3` at `5af2bcc9e`,
  then `95a79f7ea068f9652b71cf09b7e2400dfa6b1f97` at
  `69d973f1259889d1ac588283e6cab54bd0f28190`); both merges were conflict-free
  and the second brought notes only.
- Scope of this revision: **this commit changes this handoff document and
  nothing else.** Source, headers, the TU manifest, `symbols.txt`, `delinks.txt`
  and `attribution.json` are byte-identical to the accepted input
  `9c361560ff07147d1944597fbf0e5a22de8638cf` and, through it, to
  `39229e56110d805c876d7c51958cdf4f21bb4403`, so every byte, relocation and
  ROM-data measurement recorded under Proof was taken on a tree whose source
  and config are byte-identical to this one -- this document is the only
  difference -- and none of it is restated from an older or differently shaped
  tree. The independent verification checked that claim rather than accepting
  it: a blob-level `git ls-tree -r` comparison of this document's input against
  `39229e56`, with the handoff path removed, is byte-identical at 10,935
  entries, every mode and hash equal. This revision corrects four statements
  that verification found wrong -- the owed-attribution table's re-key recipe
  (which, applied literally, FAILED `prepush_attribution`), the stated
  mechanism behind that gate's blindness, the contributor count, and the
  block-scope extern count -- and changes nothing else. It deliberately does
  not move the source file; see the placement note under blockers.
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
- Why this candidate has NOT merged `origin/main`, and why that is deliberate:
  the base stays at `69d973f1259889d1ac588283e6cab54bd0f28190`. Merging main
  now would rewrite this tree and invalidate the byte, relocation and ROM-data
  proof recorded below, which was taken on it; holding the base is what lets
  that proof carry unchanged through the two text-only revisions since
  `39229e56`. The second
  independent verification checked the deferral rather than granting it, and
  the third flagged its figures as stale by construction -- a drift count ages
  every time main moves, and this one aged six times while the task sat in the
  integration lane. **Re-measured by the integrator at every compose**, most
  recently against `26f54f8fc`: since `69d973f125` main has moved **219 files,
  177 of them under `src/`, `config/` or `include/`**. The figures recorded
  here earlier -- 21/2, then 79/48, 123/88 and 177/140 -- are each superseded.
  This number is expected to keep moving; re-measure it, never quote it.
  **One earlier conclusion did NOT survive re-measurement and is withdrawn.**
  This note previously said main touches 0 ov002 files. That stopped being true
  when the ov002 lava batch #2435 landed: main now touches **2** ov002 files,
  `config/arm9/overlays/ov002/delinks.txt` and `symbols.txt`, the same two this
  change edits. The overlap is nonetheless positional rather than semantic, and
  that is the claim to carry forward: #2435's ov002 edits begin at `0x020b5734`
  while this change spans `0x020aee40..0x020b0530`, so the two diffs share zero
  changed lines and both files auto-merged. `include/decl_common.h` is the same
  story -- main's hunks land at lines 514, 701, 749, 773, 1561, 2811 and 2898
  while this change's single hunk removes the `_ZTV13OneUpMushroom` declaration
  at line 507, the nearest being seven lines clear, and git merged them with no
  conflict. The integrator composes onto
  a current base containing `1c93d2663` -- an integration-lane merge with its
  own gate run, not a rebase this task should perform.
- Source placement (raised as blocking by the first verification, overturned):
  the TU stays at `src/actors/da1up_c.cpp`. The measurement behind that ruling,
  made by the domain coordinator and posted on
  https://github.com/tangosdev/sm64ds-decomp/issues/2426, is that all 18 most
  recently landed promoted TUs on main sit at `src/actors/<Class>.cpp`, with
  none landing under `src/game/actors/` as `d_a_<snake>.cpp` since
  https://github.com/tangosdev/sm64ds-decomp/pull/2270; that
  `tools/srcpath.py`, the resolver this workflow designates, returns the flat
  path for this class (re-checked **[rev b]**:
  `python tools/srcpath.py _ZN7da1up_c6RenderEv` -> `src/actors/da1up_c.cpp`);
  and that no gate rejects `src/actors/`. The forward convention for new class
  TUs -- flat versus a nested `src/game/actors/` stem such as `d_a_1up` -- is
  being decided on issue #2426 and is not this task's to settle. A later
  reviewed convention change would move this file with every other one.
- Status: candidate for a fresh independent verification. It is worth being
  precise about what has and has not failed across three attempts. **No byte,
  relocation, emitted-object or ROM-data check has ever failed on this source**,
  and none has been disputed by either verification; the second verification
  additionally proved the scope claim above at blob level, so that evidence
  carries onto this commit without being re-cut. What failed was text. The
  first attempt (`39229e56`) was rejected on source placement (overturned
  above, on measurement, by the domain coordinator) and on three unsupported
  statements. The second (`9c361560`) had those three confirmed accurate but
  was returned for four further defects, all corrected here and each marked at
  the place it was corrected. One of the four was not a wording problem: the
  owed-attribution table's re-key recipe, applied literally to a scratch commit,
  made `prepush_attribution` exit 1 with `CREDIT LOST _ZN13OneUpMushroomD0Ev`.
  That recipe is rewritten below and re-tested the same way; the number this
  session measured is recorded under Proof. Every check the producer and the
  independent verifications could run locally is listed under Proof, including
  the one that is refused and the one gate whose row disagrees on unrelated
  pre-existing drift. The terminal private validation has not been run on this
  commit or any of its inputs; on the predecessor `f38a8f74` it failed only on
  the `rom_data_regressions` line now fixed by `1c93d2663`, and noted 8
  contributor credit reassignments, which are tabulated below as owed
  integration work.
- Remaining uncommitted/local-only material and where it is preserved: none.
  Build state and the queue receipts are in the ignored `build/` of the
  worktrees `C:/tmp/sm64ds-a1up2411` (first producer attempt),
  `C:/tmp/sm64ds-vfy2411` (first independent verification),
  `C:/tmp/sm64ds-sm64ds-1up2411b` (second producer attempt) and
  `C:/tmp/sm64ds-sm64ds-1up2411c` (this revision). The three scratch commits
  this session used to test the owed-attribution table were made on a throwaway
  branch, measured, and deleted with the branch; `attribution.json` in this
  commit is byte-identical to the input's.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `da1up_c`, ov002, the
  contiguous `.text` run `ov002:[0x020aee40,0x020b0530)` (36 functions, ROM
  ordinals 0..35) promoted into `src/actors/da1up_c.cpp`. The two factories
  `ov002:[0x020b0530,0x020b05d0)` now live in that same cpp with the rest of
  the class; only their vtable reference moved to the ROM name.
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
  Session `prod-2411-0907` added only this handoff document; sessions
  `prod-2411b-0907` and `prod-2411c-0907` (this revision) each edited only this
  handoff document.
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
  evidence. This is partial reconstruction, not an established compiler wall:
  original names are unknown, but explicitly coined identifiers are permitted
  once the implementation role and types are supported.
- Recovered layout/fields; remaining shadow structs/raw offsets: the header
  is a real `struct da1up_c : dEnemyBase_c` closing at 0x398 with a size
  assert; the eight methods use member names. The 27 free bodies still carry
  raw `(base + 0xNN)` accesses -- 139 over 39 distinct offsets by this
  session's regex, 172 over 42 by the independent verification's; the count is
  regex-dependent and only its order of magnitude should be relied on -- plus
  **33** block-scope mangled `extern` declarations (an earlier revision said 32;
  counting indented `extern` lines naming a `_Z` symbol in
  `src/actors/da1up_c.cpp` gives 33 declarations of only **16 distinct**
  symbols -- `_ZN5Sound9PlayBank3EjRK7Vector3` is redeclared 7 times and
  `_ZN8dActor_c15IsPlayerInRangeEi` 5, each local block spelling its own
  parameter types, which is itself part of the remaining cleanup)
  and **three** local shadow
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
  all, because the 7 renamed members that have rows each carry one under both
  the old `OneUpMushroom` and the new `da1up_c` mangled spelling (7x2 = 14),
  and the 19 `func_ov002_*` bodies carry one each: 14 + 19 = 33. That doubling
  is load-bearing, not redundancy, and the reason is spelled out under the
  table below. An earlier revision of this document
  said "every folded shard"; that was wrong. `python tools/prepush_attribution.py --base
  69d973f1259889d1ac588283e6cab54bd0f28190 --head HEAD` reports `9720 tracked,
  26 consolidated with credit intact, 0 changed, 0 lost` -- the 26 with rows.
  The remaining **10** symbols have no `#symbol` override row. For the private
  validator, which resolves on the HEAD path, they miss
  `overrides[src/actors/da1up_c.cpp#<sym>]`, miss the bare TU path, and land on
  `first_matchers[src/actors/da1up_c.cpp]` -- a path created by this fold --
  so it credits them to the TU's adder (`andrewboudreau`). They are
  `_ZN7da1up_cD0Ev`, `_ZN7da1up_c8BehaviorEv`,
  `func_ov002_020aeee4`, `func_ov002_020aefa4`, `func_ov002_020aefb8`,
  `func_ov002_020af724`, `func_ov002_020af950`, `func_ov002_020afc68`,
  `func_ov002_020afd10` and `func_ov002_020aff10`. For two of them
  (`func_ov002_020aeee4`, `func_ov002_020afd10`) the base matcher IS the adder,
  so nothing moves. For the other **8** the base matcher is someone else, and
  those 8 are exactly the "8 changed credits" the predecessor's private
  validation reported. This is owed work, not a resolved item.
  `attribution.json` is global bookkeeping, is NOT among this task's reserved
  resources, and was deliberately left untouched by this commit; the integration
  lane applies the rows below. Each base author was recomputed independently
  **[rev b]** and again **[this revision]** with the validator's own formula
  (`overrides[path#symbol]` else
  `overrides[path]` else `match_finishers[path]` else `first_matchers[path]`,
  evaluated at base `69d973f125...`), which reproduces the same 8 rows and the
  same 8 handles both times.
  Each old shard path is recorded per function as `legacy_source` in
  `config/tu_manifest.d/ov002/da1up_c.json`.

  **Two of these eight need a row under BOTH mangled spellings, and an earlier
  revision of this table asked for only the new one.** That is not a cosmetic
  point: applied literally, the earlier table made the pre-push gate FAIL. The
  two tools that read these rows key them differently, and both have to hit:

  - `validate_merge.attribution_snapshot` builds `f"{path}#{rec['name']}"` from
    the built report, so at HEAD it looks up the **new** spelling
    (`...#_ZN7da1up_cD0Ev`) on the TU path.
  - `prepush_attribution.member_overrides_at` splits on the `#` and keys the
    result by the bare symbol, then consults it for each **deleted shard's
    basename** (`prepush_attribution.py:256-265`). A promoted shard's basename
    is the **old** spelling (`_ZN13OneUpMushroomD0Ev`), because that is what
    the file was called. Without a row under the old spelling the rescue misses
    and the gate reports the credit lost.

  This is exactly what the 26 rows already on that path do -- it is why 26
  shards carry 33 rows -- and the earlier table failed to apply its own
  arithmetic to the two shards it was adding. Measured on a scratch commit by
  this session; see the `prepush_attribution` entry under Proof for the exit
  codes.

  | # | Ordinal | Address | Override key(s) to add on `src/actors/da1up_c.cpp` | Value | Where the base credit comes from |
  |---|---|---|---|---|---|
  | 1 | 1 | 0x020aee88 | `#_ZN7da1up_cD0Ev` **and** `#_ZN13OneUpMushroomD0Ev` | `tangosdev` | an `attribution.json` **bare-path** override keyed on the deleted shard `_ZN13OneUpMushroomD0Ev.cpp`; drop that row |
  | 2 | 3 | 0x020aefa4 | `#func_ov002_020aefa4` | `ruspecial` | an `attribution.json` **bare-path** override keyed on the deleted shard `func_ov002_020aefa4.c`; drop that row |
  | 3 | 4 | 0x020aefb8 | `#func_ov002_020aefb8` | `lunavyqo` | `match_finishers` on the deleted shard |
  | 4 | 15 | 0x020af724 | `#func_ov002_020af724` | `tangosdev` | `match_finishers` on the deleted shard |
  | 5 | 20 | 0x020af950 | `#func_ov002_020af950` | `tangosdev` | `match_finishers` on the deleted shard |
  | 6 | 26 | 0x020afc68 | `#func_ov002_020afc68` | `tangosdev` | `match_finishers` on the deleted shard |
  | 7 | 30 | 0x020aff10 | `#func_ov002_020aff10` | `tangosdev` | `match_finishers` on the deleted shard |
  | 8 | 34 | 0x020b00e8 | `#_ZN7da1up_c8BehaviorEv` **and** `#_ZN13OneUpMushroom8BehaviorEv` | `alexsobolew7` | `match_finishers` on the deleted shard |

  Ten rows for eight shards. Rows 3-7 need only one spelling because a
  `func_ov002_*` body was never renamed -- its shard basename and its symbol
  are the same string. Rows 1 and 8 are the two renamed members and need both.

  Two of the eight, rows 1 and 2, additionally require a **deletion**: their
  credit today survives only through a bare-path `attribution.json` override on
  a path this promotion deleted, and they appear in neither `first_matchers`
  nor `match_finishers`. Re-key them onto the TU -- add the `#symbol` row(s),
  drop the bare-path row -- rather than leaving a second, stale key behind. The
  other six are `match_finishers`-only and need no deletion.

  On the **value** column: write the base handle verbatim, as
  `match_finishers` / the bare-path override returns it at base
  `69d973f125...`. Notably that means `alexsobolew7` for row 8, not
  `ruspecial`. `prepush_attribution` string-compares the row's value against
  the base handle with no alias resolution, so a canonicalised value can read
  as CREDIT CHANGED; `validate_merge` applies `attribution.json`'s own
  `aliases` map afterwards (`validate_merge.py:319-333`) and gets the right
  person either way.

  Which person: `attribution.json`'s own `aliases` map collapses
  `alexsobolew7` onto `ruspecial`, so **two** distinct contributors outside the
  project account are owed here, not three -- `ruspecial` **x2** (rows 2 and 8)
  and `lunavyqo` **x1** (row 3) -- with `tangosdev` owed the other 5. An
  earlier revision of this document counted `alexsobolew7` as a third person;
  it is the same person as `ruspecial`, and `lunavyqo` is not aliased to
  anyone.

  For completeness the integrator may also add rows for `func_ov002_020aeee4`
  (ordinal 2, 0x020aeee4) and `func_ov002_020afd10` (ordinal 27, 0x020afd10),
  both `andrewboudreau`: no credit moves today, but a row makes them durable
  against a later move of this file. That would bring the TU to 36/36 shards
  covered, on 45 rows -- the 9 renamed members at two spellings each plus the
  27 `func_ov002_*` bodies at one.
- Remaining reconstruction scope: investigate and type the 14 proven dispatch
  members, recording explicitly coined identifiers where original names are
  unavailable. Determine membership separately for the 13 helpers. Replace raw
  offsets with supported member access and give the dispatch array/descriptors
  a typed owner when byte proof supports it. These remain follow-up work beyond
  the accepted packaging candidate, not a claim that reconstruction is finished.

## Proof

Unless an entry says otherwise, the commands ran in `C:/tmp/sm64ds-a1up2411`
(producer session `prod-2411-0907`) on this commit's tree -- source and config
identical to `95a79f7ea068f9652b71cf09b7e2400dfa6b1f97`, and identical again to
the inputs `39229e56` and `9c361560`, since the two revisions after `39229e56`
changed only this document -- with pinned compiler mwccarm 2004/b56 and base
`69d973f1259889d1ac588283e6cab54bd0f28190`. Session markers:
**[verifier]** was measured independently by `vfy-2411-0907` in
`C:/tmp/sm64ds-vfy2411`; **[rev b]** by `prod-2411b-0907` in
`C:/tmp/sm64ds-sm64ds-1up2411b`; **[this revision]** by `prod-2411c-0907` in
`C:/tmp/sm64ds-sm64ds-1up2411c`. All on the same tree. Logs are in each
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
  now also in that cpp -> exit 0,
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
  `python tools/romdata_check.py --files src/actors/da1up_c.cpp --json
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
  reproduced verbatim **[rev b]** at this tree. This is a tool-shape
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
  0 lost`. Re-run **[rev b]** and again **[this revision]**; both verifications
  ran the same command with the same result. Coverage limit, and an earlier revision of this
  document had its mechanism **exactly backwards**. It said the gate sees only
  the 26 rowed shards and that the other 10 are absent from its ledger. It is
  the other way round, and it was measured this time rather than reasoned
  about, by comparing each shard's `legacy_source` basename against
  `prepush_attribution.lineage(HEAD)`:
  - **0 of the 26** rowed shards appear in the HEAD ledger. Their deleted paths
    drop out of `first_matchers`/`match_finishers`/bare overrides entirely, so
    each falls into the "present at base, gone at head" branch and is rescued
    by its `#symbol` row. That rescue is precisely what `26 consolidated`
    counts -- the number is the rowed shards, but it counts them because they
    VANISHED, not because the gate can see them.
  - **10 of the 10** unrowed shards DO appear in the HEAD ledger: 8 because
    `match_finishers` never expires a deleted source path, and 2
    (`_ZN13OneUpMushroomD0Ev`, `func_ov002_020aefa4`) through the stale
    bare-path overrides that rows 1 and 2 of the owed table remove. Being
    present at head with the same handle as at base, they compare equal and are
    reported as nothing at all.

  The conclusion the earlier revision drew survives intact: `0 changed` here
  does not contradict the private validator's 8 reassignments, because the two
  tools measure different sets. Only the stated reason was wrong. The 8 were
  recomputed **[this revision]** with the validator's own resolution order
  against base `69d973f125...`, reproducing the same 8 rows and the same base
  handles, and are tabulated under Reconstruction dimensions.
  `attribution.json` is unreserved global bookkeeping and was not edited by
  this commit. Log `build/attrib.json`.
- Scratch test of the owed-attribution table **[this revision]**: the table
  above is a recipe an integrator will apply literally, so it was applied
  literally -- to `attribution.json` on a throwaway branch off this commit, one
  commit per variant, then measured with the same
  `prepush_attribution.py --base 69d973f125... --head HEAD` invocation and the
  branch deleted. Three variants:
  - The table **as the previous revision wrote it** (new mangled spelling only,
    both bare-path overrides dropped) -> **exit 1**, `27 consolidated with
    credit intact, 0 changed, 1 lost`, and the line
    `CREDIT LOST     _ZN13OneUpMushroomD0Ev`, whose tail names the deleted
    shard stem and `tangosdev`. `func_ov002_020aefa4` is rescued (its shard
    basename and its symbol are the same string); the renamed destructor is not.
  - The table **as corrected above** (both spellings for rows 1 and 8) ->
    **exit 0**, `9718 tracked, 0 moved with credit intact, 0 renamed with
    credit intact, 28 consolidated with credit intact, 0 changed, 0 lost`. The
    two extra consolidations over the 26 baseline are exactly
    `_ZN13OneUpMushroomD0Ev` and `func_ov002_020aefa4`; `tracked` falls by 2
    because dropping the two stale bare-path overrides removes two names from
    the head ledger. Rows 3-8 change this gate's tally by nothing, for the
    reason in the coverage limit above -- their names never leave the ledger.
  - The corrected table with row 8 valued `ruspecial` instead of
    `alexsobolew7` -> also **exit 0**, `28 consolidated, 0 changed, 0 lost`,
    because that shard's name stays in the head ledger and its `#symbol` row is
    never consulted by this gate. Recorded so nobody reads that as licence:
    write the base handle verbatim, since the moment `match_finishers` stops
    carrying the deleted path the row IS consulted and a canonicalised value
    reads as CREDIT CHANGED.

  The scratch commits were discarded and their branch deleted;
  `git diff 9c361560ff07147d1944597fbf0e5a22de8638cf -- attribution.json`
  on this commit is empty. Applying the table remains the integration lane's
  work, not this task's.
- Gates re-run for THIS revision, and only these **[this revision]**: this
  commit changes one Markdown file under `notes/` and no source, header,
  manifest, `symbols.txt` or `delinks.txt`, so the byte, relocation and
  ROM-data evidence above was NOT re-measured -- it was taken on the identical
  tree and stands unchanged, and the identity of that tree was itself checked
  at blob level by the second verification. What was re-run is what a prose
  edit can plausibly break, plus every claim this revision corrects:
  `python tools/check_dead_references.py` -> exit 0, `10329 scan target(s) ...
  896 markdown link(s), 0 that do not resolve relative to their own file`,
  `no new dead references`, `no broken markdown links` (it still notes the same
  3 already-baselined prose references that now resolve, which is not a failure
  and is left for a separate baseline-shrinking change). Worth recording for
  whoever edits this document next: its first draft named the two deleted shard
  paths in full and this gate went red on all three tokens, since it reads any
  `dir/file`-shaped string in prose as a live path claim. The prose now names
  those shards by basename only, which is why row 1 and row 2 of the table
  above read the way they do;
  `python tools/check_python_names.py` -> exit 0, `278 tracked file(s) checked,
  0 unresolvable names`;
  `python tools/prepush_attribution.py --base 69d973f125... --head HEAD` on
  this commit -> exit 0, `26 consolidated, 0 changed, 0 lost`, and on the three
  scratch variants of the owed table -> exit 1 / exit 0 / exit 0 as recorded
  above; the ledger-membership measurement behind the corrected coverage limit
  (0 of the 26 rowed shards and 10 of the 10 unrowed ones present in
  `prepush_attribution.lineage(HEAD)`); the base-handle recomputation
  for all 36 shards through the validator's resolution order; the
  `attribution.json` `aliases` read that settles `alexsobolew7` -> `ruspecial`;
  the block-scope mangled `extern` count (33 lines, 16 distinct symbols); and
  `git merge-base --is-ancestor 1c93d2663 origin/main` -> exit 0.
  Gates NOT run for this revision, and why: `rombuild.py`, `tubuild verify`,
  `tubuild linkcheck`, `romdata_check.py`, `prepush_linkcheck.py`,
  `linkcheck.py`, `check_header_offsets.py`, `affected_src.py`,
  `port_refcheck.py`, `check_rename_ledger.py`, `check_src_tu.py`,
  `check_tubuild_conflicts.py`, `check_profile_campaign.py`,
  `tiers_ratchet.py`, `langmode_audit.py`, `layout_check.py`,
  `cpp_tu_state.py`, `queue_audit.py`, `tu_map.py` and `srcpath.py` -- every
  one of them reads source, headers, config, manifests or the ROM, none of
  which this commit touches; their recorded results were taken on this exact
  tree and are not restated as this revision's own work. No claim in this
  document rests on a measurement this revision made but did not name.
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
