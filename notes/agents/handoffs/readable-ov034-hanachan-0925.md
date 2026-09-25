# Handoff: readable-ov034-hanachan-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3140,
  `readable-ov034-hanachan-0925`, stage `revise`, session `claude-prod-readable-ov034-hanachan-0925`,
  Claude Code (Opus 5.5).
- Source branch and previous accepted input SHA: `readable/readable-ov034-hanachan-0925`, input
  `ace15a6c626d8fd133dd8accc41dd313be567d96`.
- Original source base SHA and installed workflow and tool SHA: both `ace15a6c626d8fd133dd8accc41dd313be567d96`.
- Separate evidence commits and required artifacts in this commit: none. Every experiment below was
  scratch-applied, measured with `tubuild verify` and reverted; there is no committed artifact for them.
- Next action, responsible role and blockers: independent verification (verifier) of this commit:
  byte, relocation, whole-object and source review. No blockers.
- Status: verified candidate. Every local gate listed below exits 0.
- Remaining uncommitted or local-only material: none.

## What changed and why

- Class, TU, symbol and module-qualified ROM scope: `ov034/daHanachan_c`, ov034 `.text`
  `[0x021111a0, 0x021136a4)`, 34 functions.
- Reserved surfaces actually touched: `src/actors/daHanachan_c.cpp`, `include/daHanachan_c.h`,
  `config/arm9/overlays/ov034/symbols.txt` (28 rows renamed in place),
  `config/tu_manifest.d/ov034/daHanachan_c.json` (28 symbol fields, the two `_ZN5Vec3DD1Ev`
  compiler-only entries removed, 26 notes and one reason text updated), this class's 28 override keys
  in `attribution.json` (re-keyed in place, credit unchanged) and one key added in place in
  `config/decl-agreement-baseline.json` (below). The ov034 `delinks.txt` needs no change: its row for
  this TU is an address range with no symbol names.
- ROM observations: the state table `data_ov034_02114538` is 11 rows of 20 bytes, each an Init
  pointer-to-member (8 bytes), a Main pointer-to-member (8 bytes) and a name string pointer.
  `__sinit_ov034_021138ec` fills the pointers; the strings are in ROM. By row: 0 DEMOWAIT,
  1 DEMOCALL, 2 MOVE_STOP, 3 MOVE_START, 4 MOVE, 5 DAMAGE, 6 DAMAGETALK, 7 DEAD, 8 DAMAGE_STOP,
  9 DAMAGE_WAIT, 10 DAMAGE_START. Every former `func_ov034_*` function takes the object in r0, so
  member form is the same ABI. The table reaches the state functions by address, which does not
  block the rename.
- Hypothesized names, explicitly not recovered facts: the 22 `StateXxxInit` and `StateXxxMain`
  names come from the ROM state strings. `SetState` and `ExecState` follow the table dispatch.
  `IsStompedBy`, `HandlePlayerHits`, `UpdateSegments` and `UpdateSegmentModels` are coined from their
  bodies, and so is every field name from `mSegmentPos` (0x3cc) to `mParticle` (0x8e4). The header
  marks all of these as coined.
- Compiler experiments and measured barriers (pinned mwccarm 2004/b56 via
  `python tools/tubuild.py verify ov034/daHanachan_c`; each was reverted and the committed form is
  34/34 MATCH):
  - A real `ModelAnim::SetAnim` call with a `Fix12<int>` speed: 33/34, StateDamageStartMain differs.
    The mangled bridge taking `void *anim` stays.
  - UpdateSegmentModels indexed as `mSegmentPos[i]` or through a typed pointer: 2 words differ (the
    +0x1c step folds into a 0x12c offset). The raw row pointer stays.
  - StateDemoWaitInit with a typed row pointer: 48 words differ.
  - InitResources: `mSegmentRot[prev].y` swaps the sp24 and sp28 stack slots; a `for` loop in place
    of the `do`-`while` differs. The first animation loop indexed with `i*3` differs.
  - UpdateSegments with a typed `rot` pointer differs.
  - HandlePlayerHits: `if (a->actorID != 0xbf) continue;` in place of the `isPlayer` flag differs;
    removing the volatile `playerPos` copy differs.
  - StateDeadMain: deleting the unused star position local changes bytes; it is now a plain
    `Vector3` in place of the `Vec3D` shadow.
  - The face-frame clamp: `bool ok = false` before the clamp gives 71 differing words; `bool done`
    in the animation loops differs.
  - Collapsing the adjacent `#pragma opt_strength_reduction on` and `off` pair before
    UpdateSegments into the one `off`: 34/34, object identical. Committed.

## Findings (self-audit)

No readability review existed for this TU, so this list is the Step 0 self-audit. Owner of each
deferred one: the coordinator `claude-promo-coord-0923`, tracked on
https://github.com/tangosdev/sm64ds-decomp/issues/3140.

| ID | Category | Finding | Outcome | Owner | Issue | Reason |
|---|---|---|---|---|---|---|
| R1 | extern "C" state bodies | 22 state functions and 6 helpers were `extern "C"` free functions named `func_ov034_*` | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | All 28 are daHanachan_c members; symbols.txt rows, manifest symbols and attribution keys renamed in place. Object bytes, relocations and undefined symbols identical. |
| R2 | raw offsets and casts | state dispatch cast a `char[]` table to a local pointer-to-member row struct | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | The header's nested `daHanachan_c::State` types the table; SetState and ExecState call `(this->*data_ov034_02114538[mState].mInit)()` and `.mMain`. |
| R3 | unk_ fields | 15 `unk_` fields in the header, 34 `unk_` lines in the TU | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | All named (coined, marked in the header); the tail pad is now `mState`, `mPlayer`, `mHomePos`, `mStateTimer`, `mDeathDelay` and `mDeathSegment`. |
| R4 | mangled bridges | 26 mangled-name bridges to functions a header already declares | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Now real calls: ApproachLinear (both overloads), Animation::Finished, TextureSequence::Update and LoadFile, Model::LoadFile, ModelBase::SetFile, Player::ShowMessage, GetTalkState, HasFinishedTalking and StartTalk, Message::PrepareTalk and EndTalk, fBase_c::MarkForDestruction, dActor_c::FindWithID, PoofDustAt, ClosestPlayer, HorzAngleToCPlayer, TrackStar and UntrackAndSpawnStar, Animation::LoadFile (through a `SharedFilePtr` cast over the `void **` tables), dBgCh_Actr::IsOnWall, and a real `dBgCh_Gnd` local (constructor, SetObjAndPos, DetectClsn, destructor). |
| R5 | mangled bridges | `Fix12<int>` by-value bridges: ModelAnim::SetAnim, Player::Hurt, Player::Bounce (declared in `include/decl_Player.h`), Particle::RunningSlidingDustAt, Particle::System::New, TextureSequence::SetFile, dCcAcPos_c::Init, dBgCh_Actr::Init | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | SetAnim measured (33/34, above); the others share the by-value `Fix12<int>` parameter and were not tried. |
| R6 | mangled bridges | bridges to functions no header declares: Sound::LoadAndSetMusic_Layer3 and StopLoadedMusic_Layer3, Camera::SetFlag_3 | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Declaring them needs `include/Sound.h` or `include/Camera.h`, shared headers outside this reservation. |
| R7 | dead or local declarations | shadow structs `Vec3D`, `Ent`, `Sub`, `C125b8` and `C12604`; unused bridge declarations; three redundant includes | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Removed. `Vec3D` removal also drops the compiler-only `_ZN5Vec3DD1Ev` section. `check_decl_agreement` reports no new local redeclarations. |
| R8 | raw offsets and casts | row-pointer walks in StateDemoWaitInit, UpdateSegments, UpdateSegmentModels and the InitResources loop | partially fixed | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Named temporaries and typed pointers where they match; the remaining raw walks carry one short comment each. Deferred: measured, no committed artifact (experiments above). |
| R9 | raw offsets and casts | Camera reached through `data_0209f318` with `+0x154`, `+0x80` and `+0x8c` offsets; `func_020092c4` | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Camera layout and the arm9 helper are outside this reservation. |
| R10 | goto, temporaries and launders | HandlePlayerHits constant temporaries, `isPlayer` flag, volatile `playerPos` copy | partially fixed | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Constant temporaries removed. The flag and the volatile copy stay, with a comment. Deferred: measured, no committed artifact. |
| R11 | address-named data | `data_ov034_*` tables, `data_02082214` (sine table), `data_0209e650` (random seed), `data_020a0e68`, `data_0209f318` | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | `__sinit_ov034_021138ec` names the ov034 tables and is its own unit outside this reservation; `include/decl_common.h` declares `data_ov034_02114538`; the rest are arm9 rows. |
| R12 | unmeasured codegen notes | ROM-ordinal banners, "recovered:" lines and long codegen essays | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Removed. Each remaining codegen comment names a shape measured in this pass (above). |
| R13 | stale manifest notes | RAW notes for the retired `func_ov034_02111a0c` and `func_ov034_02112330` units; CONFLICT notes for declarations no longer carried; the Vector3s reason citing `unk_444` | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | 26 notes end with a "Superseded:" sentence naming the member or header declaration now used; the reason names `mSegmentRot`. The other CONFLICT notes still hold (their declarations remain). |
| R14 | dead or local declarations | `include/decl_common.h` declares `func_ov034_02112650` and `func_ov034_021129ec` (now members) and `data_ov034_02114538` as `int []` | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | The file is held by `jump-contract-repair-0918`. The two function lines name no defined symbol and nothing uses them; delete them when the file is free. |
| R15 | stale manifest notes | old `func_ov034_*` names in `CLAIMS.md`, `notes/mwccarm-codegen.md`, `symbols/actor_renames.tsv` and generated docs; the `config/decl-agreement-baseline.json` key `func_ov034_02112650` | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3140 | Outside this reservation; no gate requires them. The baseline key is inert. |

## Reconstruction dimensions

- Exact function, byte and relocation coverage: 34 of 34 functions, full TU range.
- Genuine methods, and the remaining bridges: all 34 functions are daHanachan_c members (D1, D0,
  Behavior, Render, InitResources, CleanupResources and 28 non-virtual members). Bridges remain for
  R5 and R6.
- Recovered layout and fields: no `unk_` names remain. The pad at 0x462 shrinks to 2 bytes and
  `mSegmentSpacing[5]` (s32, 0x464) is named. The remaining raw offsets are R8 and R9.
- Lifecycle, vtable, RTTI, initializer and data ownership: unchanged. The state table stays the
  `data_ov034_02114538` row owned by `__sinit_ov034_021138ec` (R11).
- Attribution: the 28 overrides were re-keyed to the new symbols with the same credit.
- Declaration baseline: `data_ov034_02114538` gains a `src/actors/daHanachan_c.cpp` entry
  `return:daHanachan_c::State []` beside the existing `include/decl_common.h` entry. The typed
  declaration disagrees with the `int []` in `include/decl_common.h`, which this task cannot edit
  (R14); the build is byte- and relocation-identical.
- Residue (matching lines, base to this commit):

  | Path | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
  |---|---|---|---|---|---|
  | `src/actors/daHanachan_c.cpp` | 1424 to 1173 | 34 to 0 | 124 to 13 | 36 to 7 | 122 to 78 |
  | `include/daHanachan_c.h` | 100 to 144 | 15 to 0 | 1 to 1 | 0 to 0 | 2 to 2 |

  Of the 78 `_ZN` lines in the TU, 34 are marker lines (6 at the base), so the other `_ZN` lines go
  from 116 to 44: the 10 bridge declarations of R5 and R6, their call sites, and the call site of
  Player::Bounce, whose bridge `include/decl_Player.h` declares. The 13 `func_` lines
  are the declarations and calls of `func_0201267c`, `func_020092c4` and `func_ov002_020c51d0`, all
  outside this module. The 7 `extern "C"` lines are the C-linkage block, five C helper declarations and one data declaration.
- Counting conventions: each count is a line count, not an occurrence count. Lines are
  `git show <rev>:<path> | wc -l`; each residue is `git show <rev>:<path> | grep -c -F '<text>'` with
  the literal texts `unk_`, `func_`, `extern "C"` and `_ZN`, run with `MSYS_NO_PATHCONV=1`, where
  `<rev>` is `ace15a6c62` or this commit. Marker lines are the symbol-marker comment lines containing `_ZN`.
- `git diff --name-only ace15a6c62..HEAD` lists seven paths: `attribution.json`,
  `config/arm9/overlays/ov034/symbols.txt`, `config/decl-agreement-baseline.json`,
  `config/tu_manifest.d/ov034/daHanachan_c.json`, `include/daHanachan_c.h`,
  `notes/agents/handoffs/readable-ov034-hanachan-0925.md` and `src/actors/daHanachan_c.cpp`.

## Proof

All commands were run in this worktree against this commit's tree, with base `ace15a6c62`.

- `python tools/tubuild.py verify ov034/daHanachan_c`: exit 0. 34/34 MATCH, objisolate clean,
  reloc-destinations clean (TEXT-VERIFIED). The baseline before any edit was also 34/34.
- `python tools/rombuild.py -j16`: exit 0. `enrolledFiles` 7122; `status` passed; 11,213
  reproducing, 0 mismatching, 106/106 modules exact; `intactTuRom.identical` true, sha256
  d1506e90...c478e8. An earlier full run in this worktree compiled all 7122 (0 reused). The run on
  this commit compiled 1 and reused 7121; the 1 is `build/src/actors/daHanachan_c.o`. The run
  before it, after the header comment edit, also recompiled `build/src/d_a_hanachan.o`, the other
  consumer of `include/daHanachan_c.h`. The baseline control's
  `dsd check symbols` errors (ITCM symbols) are pre-existing; the intact gate reports zero new
  symbol errors.
- `python tools/prepush_linkcheck.py --range ace15a6c62..HEAD`: exit 0. 35 checked, 35 VERIFIED, 0 blocking.
- `python tools/prepush_attribution.py --base ace15a6c62 --head HEAD`: exit 0. 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed ace15a6c62`: exit 1 before the baseline key above
  (one disagreement, `data_ov034_02114538`); exit 0 after it, no new disagreements and no new local
  redeclarations.
- `python tools/check_dead_references.py`: exit 0, no new dead references, no broken markdown links.
- `python tools/tiers_ratchet.py --check`: exit 0. PASS, baseline 2994, current 3154. `--update` was not run.
- `python tools/check_tubuild_conflicts.py`: exit 0, 297 manifest entries.
- `python tools/queue_audit.py --check-promoted`: exit 0.
- `python tools/check_src_tu_compiles.py`: exit 0, 297/297.
- `python tools/port_refcheck.py`: exit 0, 408 checked, 0 stale.
- Undefined symbols of the TU object (pyelftools on `build/tu/ov034-daHanachan_c/daHanachan_c.o`,
  base copy against this commit): 116 before and 116 after, character-identical. The defined global
  set differs by the 28 renamed functions and the removed `_ZN5Vec3DD1Ev` only; sections,
  relocations and defined symbols are otherwise identical after mapping the renames.
- `include/decl_common.h` and `config/dead-reference-baseline.json`: empty diff against the base.
- Private validation: not run. The coordinator owns the PR.
