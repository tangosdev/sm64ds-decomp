# Handoff: pr2859-source-repair-0920

This commit is the completed bounded producer repair for PR #2859. Independent
source review and current-main composition acceptance remain pending.

- Producer: codex-rnk-producer-0920 (Codex), stage `produce`.
- Input: `166dffa1d0767416bd386294a324f00ec267fcc9`; source base:
  `4407c8471ed3299ea16d9cc899020e3232ac6ee9`.
- Branch: `review/pr2859-0920`; worktree: `C:/tmp/sm64ds-review2859-0920`.
- Protocol pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- PR: https://github.com/tangosdev/sm64ds-decomp/pull/2859.
- Seven owned paths only: class TU/header/manifest, this handoff, the experiment
  record, dMeter_c RenderTimeTimer, and the Timer static initializer.

## Changes and findings

RNK-01/02 fix false production/provenance statements and remove 18 deleted-file
references. RNK-03 gives all 16 non-destructor definitions unique symbol markers.
RNK-04 corrects two word-sized coordinate fields and the Player pointer without
moving fields or changing the 0x3e0 class size. RNK-05 gives the TU and both Timer
consumers the actual Timer class and call contracts; both consumer objects remain
identical. RNK-07 fixes the s16 atan2 result together with the real short/short
GetSubtraction method. The earlier one-sided cast nonmatch is superseded and
does not establish a compiler limitation. Remaining owned direct call contracts
were inventoried against actual definitions; return and scalar-width defects were
corrected and rebuilt.

RNK-06 remains partial. Real Vector3, SharedFilePtr, native member-function-pointer
dispatch, references, and real Player/actor/animation/collision methods replace
substantial scaffolding. Eleven address-named helpers still have raw field views;
scalar fixed-point bridges and mixed external handle-data views remain. The
experiment record distinguishes measured particle/scale nonmatches from invalid
source probes and external interface defects. It makes no claim that all cleaner
forms were exhausted or that the source is recovered original text.

Two external dependencies remain explicit: the SetAnim C definition uses u16
startFrame while ModelAnim.h declares u32; this TU's scalar bridge now follows the
actual definition. The dBgCh_Actr initializer erases actor/vector pointers to int,
while its header and mangled identity evidence pointers; this TU retains the
actor pointer view. Its header also uses scalar Fix12i, so a real-method call does
not yet carry the required mangling. These are shared interface defects, not
accepted compiler constraints. Exact paths and signatures are in the experiments.

The subsequent narrow reviewer repair changes Vec3_HorzAngle and Vec3_Dist to
their actual const Vector3* parameter contracts, with typed position views at the
one previously untyped call. Against 9a5fd9eb, all 30 allocated sections, 30
global/weak definitions and 235 relocation records remain identical, including
section order. Fresh complete-TU and all 18 strict function checks pass; the
declaration ratchet passes and removes four disagreements. The full-ROM result
below was run at 9a5fd9eb; this later repair preserves its complete TU object.

## Proof and limits

[Committed experiments](../../experiments/pr2859-source-repair-0920.json) contain
ordered replayable patches, compiler flags, corrected-contract provenance, one
baseline/final whole-object signature, and per-function strict results.

- Complete TU: 18/18 MATCH, 6108 bytes, object isolation and relocations clean.
- TU plus Timer consumers: 20 VERIFIED; every blind=0 and diffs=[].
- Full ROM, pristine input and final repair: 106/106 exact modules, 11208
  reproducing functions, zero mismatches; all 26 source data claims reproduce.
  Whole-tree source data remains 754 verified / 261 partial / 3 differ / 306
  unnamed, identical to the pristine input. No packaged ROM was generated.
- TU data: 6 verified / 180 bytes, 5 partial / 44 bytes, zero differences or
  unnamed symbols. All 26 header offsets check; only this TU consumes the header.
- Declaration ratchet passes with no new disagreements; converted ratchet passes
  at 2982 vs 2973 baseline (+9 unbanked). Neither baseline was rewritten.
- Port references: 423 resolve. No new dead references or broken Markdown links.
- Full queue audit is red: 39 shard-count, 48 line-count, 17 compiler-only, and
  one no-header stale cells. Exact original-input comparison shows no derived
  row changes from this repair. No daRNk_c/Koopa row exists; dMeter_c remains 19 shards
  and 1364 lines. No repair-specific TSV edit is proposed.

The licensed four-byte Vector3 weak destructor moves earlier in the object;
its body and sole-home policy are unchanged. Every allocated section body,
global/weak definition and relocation agrees when keyed by owning symbol.

## Next action

An independent reviewer must judge this exact candidate and the coordinator's
eventual current-main composition. The coordinator owns the reserved attribution
and declaration-baseline reconciliation, publication and integration. Remaining
reconstruction is tracked in [follow-up #2866](https://github.com/tangosdev/sm64ds-decomp/issues/2866). No external ledger or declaration header was
edited. The independent reviewer must confirm the follow-up scope and owner before
partial source acceptance; shared interface debt is not a compiler limitation.
Local logs, probe objects and private receipts remain under ignored build paths.
