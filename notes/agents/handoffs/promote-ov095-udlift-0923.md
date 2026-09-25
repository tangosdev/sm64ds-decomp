# Handoff: promote-ov095-udlift-0923

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/3071, task
  `promote-ov095-udlift-0923`, stage `revise`, role producer, session
  `claude-prod-promote-ov095-udlift-0923`, harness Claude Code (Opus 5.5).
- Source branch `promote/promote-ov095-udlift-0923`. Input commit
  `eb8f46d46a94a2a05cbcff9fd3d3ab228f2afe20` (origin/main at enqueue); it is
  also the original source base and the installed workflow and tool SHA.
- Three commits on top of the input: the byte-neutral class rename, the staged
  TU candidate, and the promotion. This commit adds the declaration-baseline
  re-key and this note. No separate evidence commits.
- Next action: independent verification (byte, relocation, whole-object and
  source review) of this commit, then the integrator. No blockers.
- Status: verified candidate, from the evidence below.
- Nothing uncommitted. Gate logs stayed in the producer's private worktree and
  are summarised here; they are not needed to reproduce anything.

## What changed and why

- Scope: class `daUdlift_c` (formerly `UpDownLiftBbh`), TU `ov095/daUdlift_c`,
  ov095 .text 0x02135ff4..0x02136764, 11 functions.
- Touched: [include/daUdlift_c.h](../../../include/daUdlift_c.h) (renamed from the coined
  header), ov095
  [delinks.txt](../../../config/arm9/overlays/ov095/delinks.txt) and [symbols.txt](../../../config/arm9/overlays/ov095/symbols.txt), the new
  [manifest entry](../../../config/tu_manifest.d/ov095/daUdlift_c.json), the promoted
  [src/actors/daUdlift_c.cpp](../../../src/actors/daUdlift_c.cpp), the three factory sources (comment and
  `_ZTV` spelling only), `symbols/actor_renames.tsv` and its report (the
  class_rename rows), and these ledgers, which the integrator reconciles:
  `attribution.json`, `config/converted-baseline.json`,
  `config/decl-agreement-baseline.json`, the six ov095 rows of each of
  `config/match_provenance.jsonl` and `config/match_attempts.jsonl` (rework
  round 1), `notes/data/tu-promotion-queue.tsv`
  (row renamed, promoted cell flipped), `notes/data/class-build-worklist.tsv`
  (header path; its stale alias annotation dropped), plus the prose in
  [notes/platform-provenance.md](../../../notes/platform-provenance.md) and [notes/player-provenance.md](../../../notes/player-provenance.md)
  that named the old header or the retired sources.
- Reverted after class_rename: its edits to `notes/data/c-cpp-classification.tsv`
  and `notes/data/tu-merge-candidates.json` (historical data).
  `--with-derived` was not used.
- ROM observations: the RTTI record at ov095 0x021375b0 reads
  [__si_class_type_info+8, 0x021375bc, _ZTI10dBgActor_c]; 0x021375bc holds
  "10daUdlift_c"; the typeinfo word before the vtable address point 0x02137628
  is 0x021375b0. `__sinit_ov095_0213722c` fills the .bss state table at
  0x02137910 with five pointer-to-member constants whose relocations resolve
  state 0..4 to 0x02136368, 0x02136298, 0x02136178, 0x02136090 and 0x02136104.
- Structural inference: the five func_ov095 bodies are daUdlift_c state
  methods. They are called through the table as members, and written in state
  order they emit in exactly the reverse ROM order. The 0x346 byte is set when
  the descent reaches mBottomY and cleared when the climb reaches mTopY, so it
  is now `mIsAtBottom`.
- Not recovered facts: the state descriptions in the source comments are
  inferred from the bodies. No original method names are claimed for the state
  functions.
- Compiler experiments (each re-verified 11/11 or measured to fail):
  both int-cast `(int)lift + 0x60` position updates became
  `lift->mPosY += lift->mVertSpeed` with no byte change; testing `actorID`
  inline instead of through an int flag DIFFs in both func_ov095_02136368 and
  func_ov095_02136178 (10/11 each), so the flags stay, each with a short note.

## Reconstruction dimensions

- Coverage: 11/11 functions MATCH (tubuild verify), objisolate clean,
  relocation destinations clean. All 11 VERIFIED again in prepush_linkcheck of
  the production file.
- Genuine methods: InitResources, Behavior, Render and CleanupResources are
  real members, and D1 and D0 come from the header's inline destructor. The five
  state bodies remain `extern "C"` func_ov095 helpers over `daUdlift_c *`.
  Giving them member spellings would coin five mangled names, so that is left
  as separate work. ABI bridges: dBgW_KcMbg::SetFile,
  dBgActor_c::IsClsnInRange and dActor_c::Earthquake stay mangled externs
  (Fix12 by value, wall 6az). func_020393c4 and func_020393d4 are unnamed arm9
  helpers.
- Layout: every access in the TU goes through named fields. There are no raw
  offsets left. `unk_349` is the one remaining unk_ field of the class, and the
  header records what is known about it.
- Lifecycle and data: the inline empty destructor makes InitResources the key
  function. The TU emits _ZTV10daUdlift_c (0x88), _ZTI and _ZTS10daUdlift_c and
  the four inherited bases' RTTI records, plus a vague-linkage
  `_ZN7Vector3D1Ev`. All are licensed in `compiler_only_output`: the
  Vector3 destructor as deadstrip-duplicate of arm9 0x020072c0, the rest as
  deadstrip-data at their cartridge homes. There is no D2. D1 is at
  0x02135ff4 and D0 at 0x02136038, in ROM order. The state table, the model and
  collision file tables and the CLPS table are unowned ov095 rows.
- Attribution: tu_promote added 11 `path#symbol` overrides carrying the
  original credit (andrewboudreau for D1 and D0, lunavyqo for InitResources,
  tangosdev for the rest). prepush_attribution reports 11 consolidated with
  credit intact, 0 changed and 0 lost. No credit went to a bot or to the
  coordinator.
- Remaining issue scope: member spellings for the five states; the rider
  collision callback func_ov095_02136764 and func_ov095_02136788 and the three
  factories, which tu_map.py puts in the next run and which stay one-function
  sources; InitResources still carries the legacy `goto load` variant switch and
  the `new_var` temporary from its original match; naming `unk_349`; the unnamed
  data tables.

## Proof

All runs use pinned mwccarm 2004/b56, in the producer's worktree at the listed
commits. The source is identical in this commit; only the declaration baseline
and this note were added after the byte and link gates ran.

- Rename commit, before any TU work: `python tools/rombuild.py -j16 --no-rom`
  exit 0, 7605 enrolled and compiled, 11,210 reproducing and 0 mismatching,
  106/106 modules exact. `python tools/prepush_linkcheck.py --range
  eb8f46d46a..HEAD` exit 0: 657 consumers of the two changed headers, 2057
  checked, 2055 verified, 0 blocking. The 2 warnings are BLIND-2 on
  `_ZN9dScDSMT_c8BehaviorEv` and BLIND-1 on `func_ov089_0213162c`, both files
  this change does not touch.
- `python tools/tubuild.py verify ov095/daUdlift_c`: 11/11 MATCH, objisolate
  clean, reloc destinations clean, compiler-only exact deadstrip, TEXT-VERIFIED.
- `python tools/tubuild.py linkcheck ov095/daUdlift_c -j16` exit 0:
  SCRATCH-LINK-VERIFIED. The whole range reproduces from one object, the object
  audit is 11 LICENSED and 0 unlicensed, 106/106 modules are exact, and dsd
  check symbols has 0 new errors. It still fails on the 9 errors that the
  baseline control also has, so that check is not green. The ROM sha256 is
  d1506e90...c478e8, IDENTICAL to the stock control.
- The emitted object's undefined symbols were each checked against
  config symbols.txt: all resolve under their exact mangled spelling.
- `python tools/tu_promote.py ov095/daUdlift_c --dry-run`, then applied: 11
  functions consolidated to src/actors/daUdlift_c.cpp (the srcpath-expected
  destination), 11 attribution overrides, 4 CONVERTED identities retained.
- `python tools/tubuild.py linkcheck --baseline --module ov095 -j16 --clean`
  exit 0 (control refreshed).
- Full-ROM: `python tools/rombuild.py -j16` exit 0. 7595 enrolled, with one
  fresh compile (the new TU). 11,210 reproducing and 0 mismatching, 106/106
  exact, "zero new symbol errors, storage aliases exact". The ROM sha256 is
  d1506e90...c478e8, the same as the stock control. That value is not the
  sha256 of the retail dump file in the main checkout, which is true of every
  build of this tree.
- `python tools/prepush_linkcheck.py --range eb8f46d46a..HEAD` on the
  promotion commit, exit 0: 652 consumers, 2062 checked, 2060 verified, 0
  blocking, the same 2 outside BLIND warnings. All 11 TU functions and the
  three factories are VERIFIED.
- `python tools/prepush_attribution.py --base eb8f46d46a --head HEAD` exit 0:
  0 changed, 0 lost, 11 consolidated with credit intact.
- `python tools/check_decl_agreement.py --changed eb8f46d46a`: exit 1 at first.
  It reported 4 disagreements that are re-keyed from the absorbed files, not
  new: the Earthquake arity 3, `data_ov095_021375a4`'s CLPS_Block type, and
  func_020393c4 and func_020393d4 param 2. The baseline keys for those
  symbols moved in place to src/actors/daUdlift_c.cpp. The 6 banked entries of
  the retired files that no longer disagree were dropped. After that it exits
  0 with no new disagreements, and the tree-wide run also exits 0. The
  baseline was not regenerated, re-sorted or union-merged.
- `python tools/check_dead_references.py` exit 0, no new dead references, no
  broken markdown links.
- `python tools/tiers_ratchet.py --check` exit 0: PASS, baseline 2993, current
  3029. The gain is not banked, for the integration lane.
- `python tools/check_tubuild_conflicts.py` exit 0.
- `python tools/queue_audit.py --check-promoted` exit 0.
- `python tools/check_src_tu_compiles.py` exit 0, 269/269.
- `python tools/port_refcheck.py` exit 0, 408 checked, 0 stale.
- Private validation and Source review: not run; they belong to later stages.

## Rework round 1

Session `claude-prod2-promote-ov095-udlift-0923`, input commit
`f7cd384dd2f36b75e1b67f1bcce64448d5cf982c` (rejected by independent
verification), base `eb8f46d46a94a2a05cbcff9fd3d3ab228f2afe20`. One fix commit
plus this note on top of the input; history not rewritten.

- **S1 (fixed):** `include/decl_common.h` is restored to the base bytes;
  `git diff eb8f46d46a -- include/decl_common.h` is empty. The base already
  declares `extern int _ZTV10daUdlift_c[];` at line 445, which is what the
  three udlift factories (`src/d_a_udlift_udlift.c`,
  `src/d_a_udlift_udlift_teresa.c` and `src/d_a_udlift_rc_rift02.c`) read, so the rename of line 503 was a
  duplicate. The now-unused `_ZTV13UpDownLiftBbh` extern at line 503 stays,
  because that header belongs to another task. The three factories compile in
  the full `rombuild` and link VERIFIED in `prepush_linkcheck` (below).
- **S2 (fixed):** following #2809, which rewrote the `name` and `srcPath`
  of such rows in place, the six rows for ov095 0x02135ff4, 0x02136038,
  0x0213645c, 0x021364b0, 0x021364d8 and 0x021365d8 in
  `config/match_provenance.jsonl` (lines 451-456) and
  `config/match_attempts.jsonl` (lines 1842-1847) now carry the
  `_ZN10daUdlift_c*` names and `srcPath` `src/actors/daUdlift_c.cpp`, the
  promoted TU (as the `tubuild promote` history-tracker note asks: retarget,
  do not delete). Only those two fields changed; key order, line endings and
  row order are preserved and every other field (including `note` and
  `label`) is verbatim. Deliberately left: `match_attempts.jsonl` line 1848,
  a *failed* attempt at 0x021367fc named `UpDownLiftBbh_Spawn` with no
  `srcPath`. It is not a `_ZN13UpDownLiftBbh*` symbol, it records what the
  attempt tried at the time, and the factory was already renamed
  `daUdlift_c_classInit_UDLIFT_TERESA` in the base's symbols.txt.
- **S3 (fixed):** the `include/daUdlift_c.h` header comment (lines 13-14 and
  25) now names `mPrevAngleX`/`mPrevAngleZ` instead of `unk_092`/`unk_096`.
- S4-S8 remain deferred to #3071, unchanged.

Proof on the fix commit (all run from the rework worktree):

- `python tools/tubuild.py verify ov095/daUdlift_c` exit 0: 11/11 MATCH,
  objisolate clean, reloc-destinations clean, ROM emission order, TEXT-VERIFIED.
- `python tools/rombuild.py -j16` exit 0: 7595 enrolled, 7595 compiled (none
  from cache); intact TU gates "dsd modules PASS, zero new symbol errors,
  storage aliases exact"; 11,210 reproducing, 0 mismatching; 106/106 modules
  exact; `intactTuRom.identical` true (sha256 d1506e90...c478e8, equal to the
  expected value).
- `python tools/prepush_linkcheck.py --range eb8f46d46a..HEAD` exit 0: 14
  checked, 14 VERIFIED, 0 warnings, 0 blocking (the 11 TU functions and the
  three factories). The header is no longer changed, so consumers no longer
  expand to the 2062 of the first round.
- `python tools/prepush_attribution.py --base eb8f46d46a --head HEAD` exit 0:
  11 consolidated with credit intact, 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed eb8f46d46a` exit 0, no new
  declaration disagreements.
- `python tools/check_dead_references.py` exit 0; `python tools/tiers_ratchet.py
  --check` exit 0 (PASS, baseline 2993, current 3029);
  `python tools/check_tubuild_conflicts.py` exit 0;
  `python tools/queue_audit.py --check-promoted` exit 0;
  `python tools/check_src_tu_compiles.py` exit 0 (269/269);
  `python tools/port_refcheck.py` exit 0 (408 checked, 0 stale).
- Undefined-symbol audit of `build/tu/ov095-daUdlift_c/daUdlift_c.o` exit 0:
  0 unknown undefined symbols.
- Next action: independent verification of rework round 1 (byte, relocation,
  whole-object and source review of this exact commit).
