# Handoff: promote-ov098-wbm-0924

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/3081, task
  `promote-ov098-wbm-0924`, stage `revise`, role producer, session
  `claude-prod-promote-ov098-wbm-0924`, harness Claude Code (Opus 5.5).
- Source branch `promote/promote-ov098-wbm-0924`. Input commit
  `787633e5e7e2a7c4e101db521958920c68ebd9f2` (origin/main at enqueue); it is
  also the original source base and the installed workflow and tool SHA.
- Six commits on top of the input: the byte-neutral class rename, the
  promotion, the declaration agreement and baseline re-key, the first cut of
  this note, the docstring fix, and the rework-round-1 rewrite of this note. No separate
  evidence commits.
- Status: byte-verified candidate, every required gate green except the
  pre-existing `tiers_ratchet --check` failure recorded under Proof.
- Next action: independent verification (byte, relocation, whole-object and
  source review) of this commit.
- Nothing uncommitted. Gate logs stayed in the producer's private worktree and
  are summarised here; they are not needed to reproduce anything.

## Dead reference in a test docstring

- After the rename, `python tools/check_dead_references.py` flagged one new
  dead reference: the docstring of
  `test_two_classes_of_the_same_name_go_to_two_different_symbols` in
  `tools/test_demember_calls.py` cited the class's old InitResources source by
  its repo path. The coordinator ruled on #3081 to reword the docstring (a
  promotion must need no change to `config/dead-reference-baseline.json`) and
  added `file:tools/test_demember_calls.py` to this task's reservation. The
  docstring now names the symbol, `WaterBomb::InitResources`, now
  `daWbm_c::InitResources` in the promoted TU; nothing else in the file
  changed, and `python -m pytest tools/test_demember_calls.py -q` passes
  (16 passed).

## Rework round 1

- The independent verifier returned this task
  (`rework-promote-ov098-wbm-0924-verify-1`) with one finding, F1: the
  "Touched, in the reservation" list cited the retired sources by bare file
  name with the directory removed, which slips past the dead-reference
  scanner. The list now cites each retired unit by the symbol it held, with
  no file name in any spelling, and the old header as "the old class header".
  Nothing else in the note, and no source, changed in this round.

## What changed and why

- Scope: class `daWbm_c` (formerly `WaterBomb`), TU `ov098/daWbm_c`, ov098
  .text 0x0213b47c..0x0213bf60, 14 functions: D1, D0, seven func_ov098 state
  and helper bodies, the four members InitResources, Behavior, Render and
  CleanupResources, and the factory `daWbm_c_classInit`.
- Touched, in the reservation:
  [include/daWbm_c.h](../../../include/daWbm_c.h), which replaces the old
  class header; ov098 [delinks.txt](../../../config/arm9/overlays/ov098/delinks.txt)
  and [symbols.txt](../../../config/arm9/overlays/ov098/symbols.txt); the new
  [manifest entry](../../../config/tu_manifest.d/ov098/daWbm_c.json); the
  promoted [src/actors/daWbm_c.cpp](../../../src/actors/daWbm_c.cpp); and the
  fourteen one-function units it absorbs, all retired and all now defined in
  the promoted TU (the manifest's `legacy_source` fields keep the history):
  - the classInit factory `daWbm_c_classInit`;
  - the six class members `WaterBomb::~WaterBomb` in its D1 and D0 variants,
    `WaterBomb::CleanupResources`, `WaterBomb::Render`,
    `WaterBomb::Behavior` and `WaterBomb::InitResources`, now the matching
    `daWbm_c` members (the first commit renamed them in place);
  - the seven helpers `func_ov098_0213b520`, `func_ov098_0213b584`,
    `func_ov098_0213b63c`, `func_ov098_0213b6e0`, `func_ov098_0213b7e8`,
    `func_ov098_0213b9d8` and `func_ov098_0213bb1c`, reserved through the
    ov098 .text range rather than by name.
- Touched, in the integration-lane ledgers (this class's rows only, edited in
  place): `attribution.json` (14 `path#symbol` overrides from tu_promote),
  `config/converted-baseline.json` (the two destructor identities, renamed and
  then re-keyed to the TU), `config/decl-agreement-baseline.json` (below),
  `notes/data/tu-promotion-queue.tsv` (row renamed, promoted cell flipped),
  `notes/data/class-build-worklist.tsv` (header path; its stale alias
  annotation dropped), and `symbols/actor_renames.tsv` (the seven
  class_rename rows).
- Reverted after class_rename: its edits to `include/decl_common.h`,
  `notes/data/tu-merge-candidates.json`,
  `notes/experiments/pr2874-integration-0920.json` and
  `notes/handoff-marker-typing.md` (historical prose and data).
  `--with-derived` was not used. `git diff 787633e5e7 -- include/decl_common.h`
  is empty; its line 705 already declares `_ZTV7daWbm_c`, and the unused
  `_ZTV9WaterBomb` extern at line 743 stays for that header's owner.
- class_rename refused at first because `_ZTV7daWbm_c` already sat in
  ov098 symbols.txt beside `_ZTV9WaterBomb`, both at 0x0213c770. The coined
  row was deleted and the rename run with `--allow-collision`, so one `_ZTV`
  row remains at the unchanged address.
- ROM observations: the RTTI record at ov098 0x0213c740 reads
  [__si_class_type_info+8, 0x0213c734, _ZTI12dEnemyBase_c (ov002 0x021081c0)];
  0x0213c734 holds "7daWbm_c"; the typeinfo word before the vtable address
  point 0x0213c770 is 0x0213c740, and the 0x84-byte vtable ends at 0x0213c7ec.
  `__sinit_ov098_0213c2b4` fills the .bss state table at 0x0213c930 from
  three pointer-to-member constants at 0x0213c71c..0x0213c734, whose
  relocations resolve state 0..2 to 0x0213b9d8, 0x0213bb1c and 0x0213b7e8.
- Structural inference: the three bodies above are daWbm_c state methods
  called through that table; the other four func_ov098 bodies are helpers only
  this class calls. The header's new field names (`mShadowMat`,
  `mSpawnPosX/Y/Z`, `mBounceCount`, `mSquashSpeed`, `mSquashScale`,
  `mBouncePhase`, `mState`, `mVariant`) and the per-variant behaviour in the
  comments are read off the bodies. Offsets and types are proven by the bytes;
  the English names are not recovered facts.
- The factory is folded into the TU. It is contiguous with tu_map's run, ends
  at the module's .text end, and as `return (int *)new daWbm_c;` written first
  it emits last at its ROM address with a byte-exact synthesized constructor,
  like the folded factories in `src/actors/daObjYajirusi_c.cpp`.
- Compiler experiments, each re-verified 14/14 or measured to fail:
  - The TU includes `common.h` first so the flat Matrix4x3 spelling wins.
    With math/Matrix.h's {Matrix3x3, Vector3} spelling the two matrix copies
    in func_ov098_0213b584 become memberwise and the function grows from 0xb8
    to 0xec bytes.
  - func_ov098_0213b6e0 tests actorID through an int flag. The plain
    `if` and `else if` chain DIFFs (13/14).
  - func_ov098_0213b9d8 keeps its arithmetic one step per statement. Folding
    the lead computation into one expression DIFFs by 7 words (13/14);
    adding `(u16)angle` for the shift pair on top of that DIFFs further.
  - Byte-neutral: `mBouncePhase++` for the two raw `c + 0x3c0` increments;
    `mVariant = param1 & 0xf` without a temporary; `mModel.Render(...)` for
    the cast virtual call; `UpdateWMClsn(mWithMeshClsn, 0)` for its mangled
    call; `ApproachLinear` and `cstd::fdiv` by their real names; a two-argument
    `func_0201267c` in place of the four-argument spelling in the old
    func_ov098_0213b9d8 source.

## Reconstruction dimensions

- Coverage: 14/14 functions MATCH (tubuild verify), objisolate clean,
  relocation destinations clean, ROM emission order. All 14 VERIFIED again by
  prepush_linkcheck on the production file.
- Genuine methods: InitResources, Behavior, Render and CleanupResources are
  real members; D1 and D0 come from the header's inline destructor; the
  factory is a real new-expression. The three state bodies and four helpers
  stay `extern "C"` func_ov098 functions over `daWbm_c *`; member spellings
  would coin seven mangled names, so that is left as separate work. ABI
  bridges: dCcAc_c::Init, dBgCh_Actr::Init, dActor_c::DropShadowRadHeight,
  Player::Hurt and Particle::System::NewSimple stay mangled externs (Fix12 by
  value, wall 6az); ModelBase::SetFile stays mangled because
  `include/ModelBase.h` declares it void and InitResources reads its result.
- Layout: every access to this class goes through named fields; there are no
  raw offsets left in the TU. The class's one remaining `unk_` field is
  `unk_3b4`, zeroed twice and never read. func_ov098_0213b63c writes the
  inherited `unk_0a4` and `unk_0ac`, which are dActor_c's names.
- Lifecycle and data: the inline empty destructor makes Behavior the key
  function. The TU emits `_ZTV7daWbm_c` (0x84), `_ZTI7daWbm_c`,
  `_ZTS7daWbm_c` and the four inherited bases' RTTI records, plus a
  vague-linkage `_ZN7Vector3D1Ev`. All are licensed in `compiler_only_output`:
  the Vector3 destructor as deadstrip-duplicate of arm9 0x020072c0, the rest as
  deadstrip-data at their cartridge homes. There is no D2 and no
  `_ZN9Matrix4x3D1Ev`. D1 is at 0x0213b47c and D0 at 0x0213b4c4, in ROM
  order. The state table, the PMF constants, the fragment angle table, the
  model file records and the profile are unowned ov098 rows.
- Attribution: tu_promote added 14 `path#symbol` overrides carrying the
  original credit (andrewboudreau for func_ov098_0213b6e0, lunavyqo for
  func_ov098_0213b9d8, tangosdev for the other twelve). prepush_attribution
  reports 14 consolidated with credit intact, 0 changed and 0 lost. No credit
  went to a bot or to the coordinator.
- CONVERTED, per `tools/tiers.py` score_member: D1, D0, Behavior, Render,
  CleanupResources and daWbm_c_classInit pass all five criteria.
  InitResources fails `no_unk_field` (unk_3b4) and `no_mangled_refs` (the
  ABI bridges); the seven func_ov098 bodies fail `real_name`, and
  func_ov098_0213b520, func_ov098_0213b63c, func_ov098_0213b584 and
  func_ov098_0213b6e0 fail one more each.
- Remaining issue scope: member spellings for the three states and four
  helpers; naming `unk_3b4`; the unnamed ov098 data rows; the sound call
  `func_0201267c`. The stale `srcPath` of func_ov098_0213b9d8 in
  `config/match_provenance.jsonl` line 134 and `config/match_attempts.jsonl`
  line 629 (rows 610 and 625 name the same function) is deferred to
  integration (files held by promote-ov095-udlift-0923); the integrator
  retargets it to `src/actors/daWbm_c.cpp`.

## Proof

All runs use pinned mwccarm 2004/b56, in the producer's worktree. The source
and config are identical in this commit; only this note was added after the
gates ran.

- Rename commit, before any TU work: `python tools/rombuild.py -j16 --no-rom`
  exit 0, 7589 enrolled and compiled (0 from cache), 11,210 reproducing and 0
  mismatching, 106/106 modules exact. `python tools/prepush_linkcheck.py
  --range 787633e5e7..HEAD` exit 0: 1 changed header, 7 checked, 7 VERIFIED,
  0 warnings.
- `python tools/tubuild.py verify ov098/daWbm_c` exit 0: 14/14 MATCH,
  objisolate clean, reloc destinations clean, compiler-only exact deadstrip,
  TEXT-VERIFIED.
- `python tools/tubuild.py linkcheck ov098/daWbm_c -j16`, before promotion,
  exit 0: SCRATCH-LINK-VERIFIED. The range 0x0213b47c..0x0213bf60 reproduces
  from one object, 106/106 modules exact, dsd check symbols has 0 new errors
  (it still fails on the 9 errors the baseline control also has, so that
  check is not green), ROM sha256 d1506e90...c478e8, IDENTICAL to the stock
  control. After promotion the same command exits 1 by design ("intact
  production requires one .text claim and at least one non-text claim"); the
  production proof is rombuild below.
- Every undefined symbol of `build/tu/ov098-daWbm_c/daWbm_c.o` resolves under
  its exact spelling in a config symbols.txt.
- `python tools/tu_promote.py ov098/daWbm_c --dry-run`, then applied: 14
  functions consolidated to src/actors/daWbm_c.cpp (the srcpath-expected
  destination), 14 attribution overrides, 2 CONVERTED identities retained.
- `python tools/tubuild.py linkcheck --baseline --module ov098 -j16 --clean`
  exit 0 (control refreshed).
- Full ROM on the final source: `python tools/rombuild.py -j16` exit 0. 7576
  enrolled (7589 less 14 retired plus the TU), one fresh compile; "dsd modules
  PASS, zero new symbol errors, storage aliases exact"; 11,210 reproducing and
  0 mismatching; 106/106 exact. `build/sm64ds.nds` sha256 d1506e90...c478e8,
  equal to the stock control.
- `python tools/prepush_linkcheck.py --range 787633e5e7..HEAD` exit 0: the
  changed header fans out to the TU only; 14 checked, 14 VERIFIED, 0 warnings,
  0 blocking. `python tools/affected_src.py include/daWbm_c.h` lists only
  src/actors/daWbm_c.cpp.
- `python tools/check_header_offsets.py include/daWbm_c.h` exit 0: 15
  commented fields, 0 mismatched, 0 unparsed, spans 0x3cc.
- `python tools/prepush_attribution.py --base 787633e5e7 --head HEAD` exit 0:
  14 consolidated with credit intact, 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed 787633e5e7`: exit 1 at
  first, with 14 disagreements from the new file. Three were fixed in the
  source (Player::Hurt's return and u8 flags, NewSimple's `void *` return,
  DropShadowRadHeight's u8 opacity; 14/14 still MATCH). The rest were banked
  in place: the dBgCh_Actr::Init row re-keyed from the retired InitResources
  file, new file keys for Vec3_Asr and `data_ov098_0213c930`, and
  `include/decl_common.h` keys for func_ov098_0213b584 and
  func_ov098_0213b6e0, whose `char *` declarations there now disagree with the
  `daWbm_c *` definitions. 14 rows of the retired files that no longer apply
  were dropped. After that it exits 0, and the tree-wide run exits 0. The
  baseline was not regenerated, re-sorted or union-merged.
- `python tools/check_dead_references.py` exit 0, after the docstring fix
  above.
- `python tools/tiers_ratchet.py --check` exit 1: FAILED, pre-existing. The
  only backslide is `src/actors/dScMgLuigi_c.cpp#_ZN12dScMgLuigi_c10ResetBoardEv`
  (no_unk_field), a file this change does not touch; the same check at the
  input commit fails identically (current 3057 there, 3061 here). Nothing was
  banked; `--update` was not run.
- `python tools/check_tubuild_conflicts.py` exit 0.
- `python tools/queue_audit.py --check-promoted` exit 0 (it failed on the
  unflipped promoted cell until that row was edited).
- `python tools/check_src_tu_compiles.py` exit 0, 271/271.
- `python tools/port_refcheck.py` exit 0, 408 checked, 0 stale.
- romdata_check: not run; the TU owns no non-text data.
- Private validation and Source review: not run; they belong to later stages.
