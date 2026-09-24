# Handoff: promote-ov071-casket-0924

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/3082, task
  `promote-ov071-casket-0924`, stage `revise`, role producer, session
  `claude-prod-promote-ov071-casket-0924`, harness Claude Code (Opus 5.5).
- Source branch `promote/promote-ov071-casket-0924`. Input commit
  `787633e5e7e2a7c4e101db521958920c68ebd9f2` (origin/main at enqueue); it is
  also the original source base and the installed workflow and tool SHA.
- Seven commits on top of the input: the byte-neutral class rename, the
  staged TU candidate, the promotion, the declaration-baseline re-key, a WIP
  checkpoint (9f7d8f4de4, this note plus the class-build worklist row), the
  citation fix (81f3794d0e, which rewords the two citations the rename left
  dead and finalizes this note), and this rework commit. No separate evidence
  commits.
- Status: verified candidate. The first attempt stopped at a WIP checkpoint
  because two prose citations of the old InitResources shard went dead with
  the rename; the coordinator then added `notes/converted-tier.md` and
  `tools/tiers.py` to the reservation (ruling: a promotion makes zero change
  to `config/dead-reference-baseline.json`), and the same session re-claimed
  the stage and reworded both citations.
- Rework round 1 (verifier request `rework-promote-ov071-casket-0924-verify-1`,
  re-claimed on input 81f3794d0e by the same session). F1: the "Retired"
  bullet named the retired sources as bare basenames with the folder split
  off, a form the dead-reference scanner cannot check, and two entries named
  no symbol. It now cites every retired unit by its symbol and the old header
  by its replacement; a grep of this note finds no other retired path or
  basename. R9 (optional) also fixed: the stale auto-generated banner of
  include/daObjCasket_c.h now describes the hand-maintained header. That edit
  is comment-only; `tubuild verify` still reports 14/14 MATCH,
  TEXT-VERIFIED. `check_dead_references` exit 0, `port_refcheck` exit 0 (408,
  all resolve), `check_decl_agreement --changed 787633e5e7` exit 0.
- Next action: independent verification (byte, relocation, whole-object and
  source review) of this commit, then the integrator. No blockers.
- Nothing uncommitted. Gate logs stayed in the producer's private worktree and
  are summarised here; they are not needed to reproduce anything.

## What changed and why

- Scope: class `daObjCasket_c` (formerly `Coffin`), TU `ov071/daObjCasket_c`,
  ov071 .text 0x02121fe4..0x021226a0, 14 functions including the factory. No
  .data is claimed.
- Touched: [include/daObjCasket_c.h](../../../include/daObjCasket_c.h) (renamed from the coined
  header), ov071 [delinks.txt](../../../config/arm9/overlays/ov071/delinks.txt) and
  [symbols.txt](../../../config/arm9/overlays/ov071/symbols.txt), the new
  [manifest entry](../../../config/tu_manifest.d/ov071/daObjCasket_c.json), the promoted
  [src/actors/daObjCasket_c.cpp](../../../src/actors/daObjCasket_c.cpp), `symbols/actor_renames.tsv`
  (the class_rename rows, rewritten in place), and these ledgers, each only in
  this class's own rows: `attribution.json` (14 overrides),
  `config/converted-baseline.json` (4 identities re-keyed),
  `config/decl-agreement-baseline.json`, `notes/data/tu-promotion-queue.tsv`
  (row renamed, promoted cell flipped to yes) and
  `notes/data/class-build-worklist.tsv` (header path; stale alias annotation
  dropped).
- Rename-forced citation fixes: `notes/converted-tier.md` line 86 and the
  `_reader_name` docstring in `tools/tiers.py` (line 255) cited the old
  InitResources shard by its pre-rename path. Both now name the symbol, the old
  `Coffin::InitResources` shard, now `daObjCasket_c::InitResources` in the
  promoted daObjCasket_c TU, with the sentence's meaning kept. The tools edit
  is docstring-only; `git diff` shows no code line changed and the module
  still parses.
- Retired, cited by symbol: the six `Coffin` members `Coffin::~Coffin` (D1
  and D0), `Coffin::InitResources`, `Coffin::Behavior`, `Coffin::Render` and
  `Coffin::CleanupResources`; the seven helpers `func_ov071_02122080`,
  `func_ov071_021220c8`, `func_ov071_02122194`, `func_ov071_021221bc`,
  `func_ov071_021223b0`, `func_ov071_021223c8` and `func_ov071_02122414`; and
  the classInit factory `daObjCasket_c_classInit`. All fourteen are now in the
  promoted TU, the members under their `daObjCasket_c` names. The old class
  header was retired too, replaced by
  [include/daObjCasket_c.h](../../../include/daObjCasket_c.h).
- Reverted after class_rename: its edits to `include/decl_common.h`
  (held by another task), `config/tu_manifest.d/ov071/daEyBm_c.json`,
  `notes/data/c-cpp-classification.tsv`, `notes/data/tu-merge-candidates.json`,
  `notes/experiments/pr2873-integration-0920.json`,
  `notes/experiments/pr2874-integration-0920.json`,
  `notes/data/class-facts/Scuttlebug.json` and `notes/handoff-marker-typing.md`
  (all historical or out of scope). Its one-line rewrite of
  `notes/converted-tier.md` was reverted too; that line was fixed by hand
  afterwards, as above.
  `--with-derived` was not used. `git diff 787633e5e7..HEAD --
  include/decl_common.h` is empty.
- The rename: class_rename refuses a class whose ROM `_ZTV` already exists.
  `_ZTV6Coffin` and `_ZTV13daObjCasket_c` both sat at 0x02122efc, so the coined
  alias row was deleted first and the rename then ran with
  `--allow-collision`. No duplicate symbol was created and the `_ZTV` address
  is unchanged.
- ROM observations (read from extracted/overlays/overlay_0071.bin, load
  0x0211f000): the RTTI record at 0x02122e94 reads [__si_class_type_info+8,
  0x02122ea0, _ZTI10dBgActor_c]; 0x02122ea0 holds "13daObjCasket_c"; the
  typeinfo word before the vtable address point 0x02122efc is 0x02122e94. The
  state table data_ov071_02122ecc holds two 20-byte { init, exec, name }
  entries whose name words point at "WAIT" (0x02122e64) and "STANDUP"
  (0x02122e6c). `__sinit_ov071_02122a64` fills the pointer-to-member words from
  the constants at 0x02122e74..0x02122e90, so state 0 WAIT is { 0x021223b0,
  0x021221bc } and state 1 STANDUP is { 0x02122194, 0x021220c8 }.
- Structural inference: the four state bodies and the two table helpers
  (func_ov071_021223c8 enters a state through init, func_ov071_02122414 runs
  exec) are daObjCasket_c methods called through the table. func_ov071_02122080
  writes the model matrix from the actor's angles and position.
- Factory: folded. Its delinks entry abutted InitResources with a zero-byte gap,
  ov071 ends each class run with its classInit, and the promoted Scuttlebug TU
  in the same overlay folds its factory the same way. It is now `return new
  daObjCasket_c();`, which matched on the first compile and replaces the legacy
  C seam (raw `_ZN7fBase_cnwEj`, `_ZN10dBgActor_cC2Ev` and vptr store).
- Not recovered facts: the state descriptions in the source comments are
  inferred from the bodies. No original method names are claimed.
- Compiler experiments (each re-verified 14/14 or measured to fail): the
  model-matrix update through a `Matrix4x3 &` local compiles to 0x54 against
  the ROM's 0x48, so it spells `casket->mModel.mat4x3` at each store; copying
  the closest player's position straight off `Player::mPosX..mPosZ` compiles
  func_ov071_021221bc to 0x1f0 against 0x1f4, so the copy stays through a
  `Vector3` pointer, with a one-line note. The legacy factory seam also
  matches, but emits no `_ZN10dBgActor_cD2Ev`; the `new` form was kept and the
  helper licensed, as ov002 daObjBC_Switch_c and ov009 daObjMcWater_c do.

## Reconstruction dimensions

- Coverage: 14/14 functions MATCH (tubuild verify), objisolate clean,
  relocation destinations clean, ROM emission order. All 14 VERIFIED again by
  prepush_linkcheck of the production file.
- Genuine methods: InitResources, Behavior, Render and CleanupResources are
  real members; D1 and D0 come from the header's inline destructor; the
  factory is a plain `new`. The four state bodies, the two table helpers and
  func_ov071_02122080 remain `extern "C"` func_ov071 helpers over
  `daObjCasket_c *`; member spellings would coin seven mangled names, so that
  is left as separate work. ABI bridges: dBgW_KcMbg::SetFile,
  dBgActor_c::IsClsnInRange and dActor_c::Earthquake stay mangled externs
  (Fix12 by value, wall 6az). func_020393d4 is an unnamed arm9 helper.
- Layout: every access in the TU goes through named fields; there are no raw
  offsets left. The header documents mState, mStateTimer, mAngleStep and
  mBehaviorTimer, and its `unk_32a` was tail padding (sizeof rounds 0x32a up
  to 0x32c, the allocation size; nothing touched it), so it is gone. The class
  has no unk_ fields left.
- Lifecycle and data: the inline empty destructor makes InitResources the key
  function. The TU emits _ZTV13daObjCasket_c (0x88 with its two-word header),
  _ZTI and _ZTS13daObjCasket_c and the four inherited bases' RTTI records, all
  licensed as deadstrip-data at their cartridge homes; `_ZN7Vector3D1Ev` as
  deadstrip-duplicate of arm9 0x020072c0; and `_ZN10dBgActor_cD2Ev` as
  deadstrip. There is no D2 of this class. D1 is at 0x02121fe4 and D0 at
  0x02122028, in ROM order. The state table, the pointer-to-member constants,
  the profile g_profile_CASKET and the two SharedFilePtr objects stay unowned
  ov071 rows.
- Attribution: tu_promote added 14 `path#symbol` overrides carrying the
  original credit (andrewboudreau for func_ov071_021221bc, ruspecial for
  func_ov071_02122194, aitddlabs for Behavior, tangosdev for the rest).
  prepush_attribution reports 14 consolidated with credit intact, 0 changed
  and 0 lost. No credit went to a bot or to the coordinator.
- Remaining issue scope: member spellings for the seven helpers; naming the
  state table and the file objects; owning the class's .data (vtable, RTTI,
  profile) in the TU. Stale-row grep: `config/match_provenance.jsonl` and
  `config/match_attempts.jsonl` carry no row naming Coffin, daObjCasket_c or
  any of the 14 addresses; nothing was edited there.

## Proof

All runs use pinned mwccarm 2004/b56, in the producer's worktree. The source
is identical in this commit to the promotion commit; only ledgers and this
note changed after the byte and link gates ran.

- Rename commit, before any TU work: `python tools/rombuild.py -j16 --no-rom`
  exit 0, 7589 enrolled and compiled (none from cache), 11,210 reproducing
  and 0 mismatching, 106/106 exact. `python tools/prepush_linkcheck.py --range
  787633e5e7..HEAD` exit 0: the changed header fans out to 7 sources, 7
  checked, 7 VERIFIED, 0 warnings, 0 blocking.
- `python tools/tubuild.py verify ov071/daObjCasket_c` exit 0: 14/14 MATCH,
  objisolate clean, reloc destinations clean, compiler-only exact deadstrip
  (`_ZN10dBgActor_cD2Ev`, `_ZN7Vector3D1Ev`), TEXT-VERIFIED.
- `python tools/tubuild.py linkcheck --baseline --module ov071 -j16 --clean`
  exit 0 (control), then `python tools/tubuild.py linkcheck
  ov071/daObjCasket_c -j16` exit 0: SCRATCH-LINK-VERIFIED. The whole range
  0x02121fe4..0x021226a0 (1724 bytes) is IDENTICAL from one object, 106/106
  modules exact, dsd check symbols 0 new errors. It still fails on the 9
  errors the baseline control also has, so that check is not green. ROM
  sha256 d1506e90...c478e8, IDENTICAL to the stock control.
- Undefined-symbol audit of the emitted object: all 64 undefined symbols
  resolve under their exact spelling in config symbols.txt (0 unknown).
- `python tools/tu_promote.py ov071/daObjCasket_c --dry-run`, then applied:
  14 functions consolidated to src/actors/daObjCasket_c.cpp (the
  srcpath-expected destination), 14 attribution overrides, 4 CONVERTED
  identities retained. The control was refreshed afterwards (exit 0).
- Full ROM: `python tools/rombuild.py -j16` exit 0. 7576 enrolled, one fresh
  compile (the new TU), "dsd modules PASS, zero new symbol errors, storage
  aliases exact", 11,210 reproducing and 0 mismatching, 106/106 exact; the
  built ROM's sha256 is d1506e90...c478e8, the stock control's value.
- `python tools/prepush_linkcheck.py --range 787633e5e7..HEAD` exit 0: 14
  checked, 14 VERIFIED, 0 warnings, 0 blocking (the header's only consumer is
  the TU).
- `python tools/prepush_attribution.py --base 787633e5e7 --head HEAD` exit 0:
  14 consolidated with credit intact, 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed 787633e5e7`: exit 1 at
  first, 12 disagreements. Eleven are the legacy files' banked entries in a
  new place (AddVec3 and Vec3_Add `Vector3 *` against `Vec3 *`, SetFile's
  reference parameters, the Earthquake arity 3, and the two SharedFilePtr
  data objects); their keys moved in place to src/actors/daObjCasket_c.cpp.
  One is new: `include/decl_common.h` declares func_ov071_02122080(char *)
  and the definition now takes `daObjCasket_c *`. It was banked under that
  symbol with the same shape as the existing func_ov071_02122414 entry,
  because decl_common.h is held by another task. The retired files' other
  entries were dropped. After that `--changed` exits 0 with no new
  disagreements, and the tree-wide run also exits 0. The baseline was not
  regenerated, re-sorted or union-merged.
- `python tools/check_dead_references.py` exit 0: no new dead references, no
  broken markdown links. Before the citation rewording it exited 1 on exactly
  the two citations above (0 at the base). `config/dead-reference-baseline.json`
  is unchanged.
- `python tools/tiers_ratchet.py --check`: exit 1, baseline 2994, current
  3058 (+65 gained, -1 lost). The one loss is
  `src/actors/dScMgLuigi_c.cpp#_ZN12dScMgLuigi_c10ResetBoardEv`, which this
  change does not touch; the same check exits 1 with that same loss at the
  base (current 3057 there), so it is pre-existing; #3077 fixes it. The gain
  is not banked, for the integration lane. Re-run after the citation fix:
  same result.
- `python tools/check_tubuild_conflicts.py` exit 0.
- `python tools/queue_audit.py --check-promoted` exit 0.
- `python tools/check_src_tu_compiles.py` exit 0, 271/271.
- `python tools/port_refcheck.py` exit 0, 408 checked, 0 stale (also re-run
  after the citation fix).
- `python tools/romdata_check.py`: not run; the TU owns no non-text data.
- Private validation and Source review: not run; they belong to later stages.
