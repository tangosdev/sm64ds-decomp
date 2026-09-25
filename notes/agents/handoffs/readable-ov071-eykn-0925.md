# Handoff: readable-ov071-eykn-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3141,
  `readable-ov071-eykn-0925`, stage `revise`, session `claude-prod-readable-ov071-eykn-0925`,
  Claude Code (Opus 5.5).
- Round 2: stage `revise`, session `claude-prod-readable-ov071-eykn-0925-r2`, Claude Code (Opus 5.5),
  input `6a18279e12f052c02352883914e2d1145fa975d0`. It changes comments only (V1 and V2 below).
- Continuation `readable-ov071-eykn-0925-fix`: stage `revise`, session
  `claude-prod-readable-ov071-eykn-0925-fix`, Claude Code (Opus 5.5), input
  `7b303d2c52058cd310ed78ddb94f8d79caf0366f`, base main `1f7a34dd6811466e6812fab6a52d9dbdbb39fca3`.
  It changes comments and this handoff only (V4 and V5 below).
- Source branch and previous accepted input SHA: `readable/readable-ov071-eykn-0925`, input
  `ace15a6c626d8fd133dd8accc41dd313be567d96`.
- Original source base SHA and installed workflow/tool SHA: both `ace15a6c626d8fd133dd8accc41dd313be567d96`.
- Separate evidence commits and required artifacts in this commit: none. Every measurement below was
  applied to the working tree, compiled with the pinned compiler and reverted. None has a committed
  artifact.
- Next action, responsible role and blockers: independent verification (verifier) of this commit
  against main `1f7a34dd68`: comment accuracy (V4) and the handoff numbers (V5). No blockers.
- Status: verified candidate. Every local gate listed below exits 0.
- Remaining uncommitted or local-only material and where it is preserved: none.

## What changed and why

- Class, TU, symbol and module-qualified ROM scope: `ov071/daEykn_c`, ov071 `.text`
  `[0x02120668, 0x021219cc)`, 23 functions. That was 22 functions ending at 0x02121734 before this commit.
- Reserved surfaces actually touched: `src/actors/daEykn_c.cpp`, `include/daEykn_c.h`,
  `config/arm9/overlays/ov071/symbols.txt` (16 rows renamed in place),
  `config/arm9/overlays/ov071/delinks.txt` (the TU's one range, end 0x02121734 to 0x021219cc),
  `config/tu_manifest.d/ov071/daEykn_c.json`, the retired stand-alone source of `_ZN8daEykn_c13InitResourcesEv`,
  this class's keys in `attribution.json` (16 re-keyed in place and 1 added) and this handoff.
- ROM observations: the state table `data_ov071_02123088` holds three 16-byte `{ init, exec }` rows of
  pointers to members, with no name strings. `__sinit_ov071_021228c8` copies them from the
  constants at 0x02122ca8..0x02122cd0. Those constants are ROM data relocations (ov071 relocs.txt
  rows 606-611) to 0x021214f4, 0x0212110c, 0x02121570, 0x0212152c, 0x02120d30 and 0x021211e0. They
  reach the functions by address, so the symbols.txt renames need no data edit. InitResources
  (0x02121734, 0x298 bytes) byte- and relocation-matched as a stand-alone file before this commit.
  It had no delinks.txt entry, so the build took its bytes from the ROM.
- Lineage or structural inference: every helper takes the actor in r0, and the state functions are
  called through a pointer to a daEykn_c member function, so member form is the natural spelling. Actor 0x106 is the
  small Mr. I and 0x107 the big one: only 0x107 calls TrackStar and UntrackAndSpawnStar.
- Hypothesized names, explicitly not recovered facts: the 16 method names (`UpdateCircling`,
  `UpdateEyeAnim`, `ResetEyeAnim`, `StartEyeAnim`, `LookForPlayer`, `CheckAttacks`,
  `UpdateModelTransform`, `St_Wait_*`, `St_Attack_*`, `St_Die_*`, `RunState`, `RunStateInit`,
  `SetState`) and the field names from 0x1b4 on are coined. The header says so next to the
  declarations. They follow the tree's spellings for the same shapes in `daObjCasket_c`.
- Compiler experiments and measured barriers (pinned mwccarm 2004/b56; each experiment was measured
  with `tools/tubuild.py verify` or `tools/match.py --module ov071`; none has a committed artifact):
  - A `Matrix4x3 mShadowMat` member emits D2, D0, D1 instead of the ROM's D1, D0, D2, because its
    `Vector3` has an inline destructor. It stays `s32 mShadowMat[12]`.
  - `*(Matrix4x3 *)mShadowMat = IDENTITY_MATRIX4X3` makes InitResources 0x2b4 bytes against 0x298.
    The copy goes through a 12-word view type instead.
  - `DropShadowRadHeight` as a member call with `Fix12<int>` aggregates makes UpdateModelTransform
    0xb0 bytes against 0xa0. The Fix12 bridges were kept on that basis. Only this one was measured.
  - `LoadBlueCoinModel` declared `(void)`: InitResources rotates the SetFile argument setup at +0x20.
  - `rot` filled from `(unsigned short)mAngleX` compiles to ldrsh where the ROM has ldrh.
  - The following plainer spellings each miss (the function differs or changes size):
    - `actorID == 0x107` tested without the int flag, in UpdateCircling.
    - `mWobblePhase += 0xe000`, `mAngleY += mHorzSpeed` and the kind test without int flags, in St_Die_Main.
    - `pos.x = mPosX; pos.y = mPosY + 0x78000; pos.z = mPosZ` in St_Die_Main.
    - The egg-or-explosion test as one `||`, in CheckAttacks.
    - target2 or target3 read straight off `mTarget->mPos*`, in St_Attack_Main.

## Findings (self-audit at the base; the issue lists categories only)

| ID | Kind | Finding at base | Outcome | Reason |
|---|---|---|---|---|
| R1 | extern "C" helpers or state bodies | 16 `func_ov071_*` helpers and state bodies in extern "C" blocks | fixed | Each one is a member now, with its symbols.txt row, manifest symbol and attribution key renamed in place. 23/23 match, relocation destinations are clean and linkcheck verifies 23/23. |
| R2 | extern "C" helpers or state bodies | InitResources lived outside the TU, in a 153-line file with 22 extern "C" lines, and was not linked | fixed | It is absorbed as the TU's last function. The delinks range now ends at 0x021219cc, the file is deleted and the credit override is added. rombuild reproduces 11,214 source-built functions with it enrolled. |
| R3 | mangled bridges | 72 `_ZN` lines in the TU and 27 in the InitResources file | partially fixed | Replaced by real calls: ClosestNonVanishPlayer, DetectRaycastClsn, FindExplosionActor, FindWithID, Spawn, PoofDust, UntrackAndSpawnStar, TrackStar, KillAndTrackInDeathTable, MarkForDestruction, Sound::PlayBank0, Particle::System::FromUniqueID, the Animation and TextureSequence members, the Model, TextureSequence and Animation LoadFile statics, TextureSequence::Prepare, both ApproachLinear overloads, UpdateAngle, the dBgCh_Gnd object, and ModelAnim::Render in place of the `Sub` shadow vtable. Still deferred (owner: next ov071 producer, https://github.com/tangosdev/sm64ds-decomp/issues/3141): 8 bridges that take a `Fix12<int>` by value (measured for DropShadowRadHeight only, no committed artifact). Also `FindEgg`: as a member call the tree's plurality spelling flips, and `src/actors/Scuttlebug.cpp` then fails the decl gate. That fix needs `src/actors/Scuttlebug.cpp` or `include/decl_Actor.h`, and this task holds neither. |
| R4 | `unk_` fields | `unk_1ec`, `unk_217`, and `u8 mShadowMat` followed by a 0x37-byte pad | partially fixed | Named: mShadowMat (flat, see measurements), mState, mStateID, mTarget, mScale (was mShadowRadiusScale), mCircleAngle, mDeathSpinAngle, mWobbleAmp, mParticleID0 and mParticleID1, mDeathSpinSpeed, mWobblePhase, mShotTimer, mSubState, mSubTimer, mCircleTimer (was mTimer) and mStarTrackID. `unk_212` is still deferred (owner: next ov071 producer, https://github.com/tangosdev/sm64ds-decomp/issues/3141): it is only ever written (0xf0), and nothing reads it to evidence a name. |
| R5 | raw offsets and casts | Every helper worked through `char *c` and `c + 0x...` | partially fixed | No `this + offset` access remains. Still deferred (owner: next ov071 producer, https://github.com/tangosdev/sm64ds-decomp/issues/3141): `(Vector3 *)&mPosX`, `&mCamSpacePosX` and `&mScaleX`, because `include/dActor_c.h` has only scalars and is a shared header this task does not hold. The `SharedFilePtr` +4 file reads, because `include/SharedFilePtr.h` has no fields and is shared. The ldrh angle reads, the ldrsh wobble phase and the flat matrix, all three measured. |
| R6 | goto, temporaries and launders | the `b200` launder, `int *` accumulator temps, the `spin` pointer, the `v` temps, InitResources' `char *c`, the `C` and `PMF` shadow call | partially fixed | All of those are removed. Still deferred (owner: next ov071 producer, https://github.com/tangosdev/sm64ds-decomp/issues/3141): the CheckAttacks gotos, the int flag temps, the St_Die_Main position load order and the St_Attack_Main target locals. Each was measured to miss. The gotos, the UpdateCircling flag, the load order, the target locals, the ldrh reads and the wobble-phase cast carry a one-line comment. |
| R7 | dead or local declarations | the shadow types `Vector3_16f`, `AnimData`, `Item16`, `Sub`, `C`, `E` and `M48`, the BMD, BTP and BCA forward declarations, duplicated bridge prototypes, and two prototypes of the same `_ZN` names with different types | partially fixed | All of those are deleted. Still deferred (owner: next ov071 producer, https://github.com/tangosdev/sm64ds-decomp/issues/3141): the local prototypes of AngleDiff, DecIfAbove0_Byte, the Vec3 helpers, Matrix4x3_FromRotationXYZExt and the BlueCoin loaders, because no shared header except the off-limits `include/decl_common.h` declares them. |
| R8 | address-named data | `data_ov071_02123088`, `data_ov071_02123038`, `data_ov071_02123040`, `data_ov071_02123048`, `data_ov071_02123050`, `data_ov071_021226a0`, `data_ov071_021226a4`, `data_02082214`, `data_0209f2f8`, `func_0201267c`, `func_0200f760` | deferred | The ov071 rows are defined and filled by `src/__sinit_ov071_021228c8.c`, which this task does not hold. The arm9 rows are not reserved. Owner: next ov071 producer, https://github.com/tangosdev/sm64ds-decomp/issues/3141. The TU does type them now: `daEykn_c::State[]`, and a `LoadedFile` view for the loaded-file word. |
| R9 | unmeasured codegen notes | the St_Attack_Main "previous draft sat at div 7" essay, and InitResources' 30-line history | fixed | Each surviving note is short and was measured in this session (see the list above). The LoadBlueCoinModel note was re-measured. |
| R10 | stale manifest notes | the boundary evidence and notes said InitResources "stays a shard" | fixed | Both boundary entries and the notes are rewritten for the 23-function span. The state-helper renames are recorded. |
| R11 | dead or local declarations | `include/decl_common.h` lines 2656-2657 declare `func_ov071_02120c90` and `func_ov071_02121634` | deferred | Those names no longer exist. The file is held by `jump-contract-repair-0918`. The declarations are unused, so nothing is affected. They should be deleted when the file is free. Owner: holder of `include/decl_common.h`, https://github.com/tangosdev/sm64ds-decomp/issues/3141. |
| R12 | stale manifest notes | `config/decl-agreement-baseline.json` still has keys for the retired `_ZN8daEykn_c13InitResourcesEv` source and entries this commit resolves | deferred | Stale entries do not fail the gate. The rules forbid regenerating that file. Owner: integrator, https://github.com/tangosdev/sm64ds-decomp/issues/3141. |

## Verifier findings, round 1

| ID | Finding | Outcome | Change |
|---|---|---|---|
| V1 | The `daEykn_c::UpdateEyeAnim` comment said "close (steps 1-3), open (4-6), done (7)". Cases 1 and 4, 2 and 5, and 3 and 6 share bodies, so each range plays the same two sequences. | fixed | The comment now says the function steps through two identical blinks: steps 1-3 and steps 4-6 each play the `data_ov071_02123038` sequence, then the `data_ov071_02123040` sequence, and step 7 resets to 0 and returns 1, one call after the second blink finishes. |
| V2 | The Fix12 bridge comment gave the DropShadowRadHeight measurement as the reason for all eight bridges. | fixed | The comment now says only DropShadowRadHeight was measured as a member call. Nothing else changed. |

## Verifier findings, composition review

| ID | Finding | Outcome | Change |
|---|---|---|---|
| V4 | The St_Die_Main comment said the eye would "swell, then vanish", and the `mScale` comment in `include/daEykn_c.h` said "death swell". The eye shrinks: `mScale` starts at 0x1000 (small) or 0x2000 (big), and `ApproachLinear(mScale, 0xa4, 0xa4)` negates the step when the value is above the target, so it steps down to 0xa4. | fixed | St_Die_Main now reads "spin with a wobble, play the death animation, shrink to scale 0xa4, then despawn". This matches sub-states 0 (wobble), 1 (animation), 2 (shrink) and 3 (spawn, then KillAndTrackInDeathTable or MarkForDestruction). The `mScale` comment now reads "shrinks while dying". No other swell or grow wording remains in the TU, the header or this handoff. |
| V5 | The residue table gave `src/actors/daEykn_c.cpp` as 753 to 786 lines. It was 788 at `90c5b2ce`. | fixed | Every number in the residue table was recomputed from this commit (see Proof). The file is 789 lines now, because the St_Die_Main comment grew by one line. The other counts did not change. |

## Reconstruction dimensions

- Exact function, byte and relocation coverage: 23 of 23 functions, the whole tu_map cut
  `[0x02120668, 0x021219cc)`.
- Genuine methods, and the remaining free-function and ABI bridges: 23 methods (D1 and D0, 6 virtuals and
  16 helpers). Bridges: 8 Fix12 by-value calls and `FindEgg` (R3).
- Recovered layout and fields, and the remaining shadow structs and raw offsets: every field from
  0x1b4 to 0x217 has a type. The only unnamed one is `unk_212` (R4). There are no raw offsets. The
  casts that remain are listed in R5.
- Lifecycle, vtable, RTTI, initializer and data ownership: unchanged. The out-of-line destructor is
  the key function. `_ZTV`, `_ZTI` and `_ZTS` are externalized. D2 and `_ZN7Vector3D1Ev` are
  deadstripped. The state table and file handles stay with `__sinit_ov071_021228c8`.
- Attribution preserved through each rename: 16 keys re-keyed in place with the same credit.
  `src/actors/daEykn_c.cpp#_ZN8daEykn_c13InitResourcesEv` was added for andrewboudreau, the file's
  first matcher. `prepush_attribution` reports 23 consolidated, 0 changed and 0 lost.
- Residue. Counting convention: the number of lines containing each token, from
  `MSYS_NO_PATHCONV=1 git show <rev>:<path> | grep -c -F <token>` and `wc -l`:

  | Path at base to this commit | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
  |---|---|---|---|---|---|
  | `src/actors/daEykn_c.cpp` | 753 to 789 | 0 to 3 | 68 to 7 | 21 to 1 | 72 to 50 |
  | retired `_ZN8daEykn_c13InitResourcesEv` source | 153 to deleted | 2 to - | 4 to - | 22 to - | 27 to - |
  | `include/daEykn_c.h` | 68 to 105 | 4 to 1 | 0 to 0 | 0 to 0 | 4 to 4 |

  23 of the 50 `_ZN` lines in the TU are symbol-marker comment lines, against 6 at base. The other 27
  are the 9 bridges: 9 declaration lines and 18 call lines.
  The 3 `unk_` lines in the TU are the writes of `unk_212`. The 7 `func_` lines are the declarations
  and calls of the arm9 `func_0201267c` and `func_0200f760` (R8). The 1 `extern "C"` line opens the
  one remaining declaration block. In the header, the one `unk_` line declares unk_212.
- Remaining agreed issue scope: R3, R4 (unk_212), R5, R6, R7, R8, R11 and R12, as tabled. The class
  is `partial`.

## Proof

Composition fix (this commit against `7b303d2c52`; `git diff 7b303d2c52..HEAD` shows only comment
lines in `src/actors/daEykn_c.cpp` and `include/daEykn_c.h`, plus this handoff):

- `python tools/tubuild.py verify ov071/daEykn_c`: exit 0. 23/23 MATCH, objisolate clean,
  reloc-destinations clean, all 23 in ROM order, TEXT-VERIFIED. The manifest was not rewritten.
- `python tools/check_dead_references.py`: exit 0. No new dead references and no broken markdown links.
- `python tools/check_decl_agreement.py --changed 1f7a34dd68`: exit 0. No new local
  redeclarations and no new disagreements.
- `python tools/check_src_tu_compiles.py`: exit 0. 299/299.
- Residue: `MSYS_NO_PATHCONV=1 git show HEAD:<path> | wc -l` and `grep -c -F <token>` on this
  commit, and the same on `ace15a6c62` for the before column, give the table above: 789, 3, 7, 1
  and 50 for the TU, and 105, 1, 0, 0 and 4 for the header. 23 of the 50 `_ZN` lines match
  `// @symbol _ZN`, against 6 at base.

Round 2 (this commit against `6a18279e12`; `git diff 6a18279e12..HEAD` shows only comment lines in
`src/actors/daEykn_c.cpp` and this handoff):

- `python tools/tubuild.py verify ov071/daEykn_c`: exit 0. 23/23 MATCH, objisolate clean,
  reloc-destinations clean, all 23 in ROM order, TEXT-VERIFIED. The manifest was not rewritten.
- `python tools/check_dead_references.py`: exit 0. No new dead references and no broken markdown links.
- `python tools/check_decl_agreement.py --changed ace15a6c62`: exit 0. No new local
  redeclarations and no new disagreements.

Round 1:

All commands were run in this worktree against this commit's tree. The base is `ace15a6c62`.

- `python tools/tubuild.py verify ov071/daEykn_c`: exit 0. 23/23 MATCH, objisolate clean,
  reloc-destinations clean, all 23 in ROM order, TEXT-VERIFIED. The baseline before any edit was
  22/22 TEXT-VERIFIED.
- `python tools/rombuild.py -j16`: exit 0. `enrolledFiles` 7122, and the run recompiled exactly one
  object, `build/src/actors/daEykn_c.o`, which defines all 23 `daEykn_c` symbols. An earlier full
  run with an empty cache compiled all 7122. 11,214 functions reproduce and 0 mismatch. 106/106
  modules are exact. `intactTuRom.identical` is true, sha256 d1506e90...c478e8. The intact TU gates
  report zero new symbol errors. The baseline control's `dsd check symbols` ITCM errors are
  pre-existing.
- `python tools/prepush_linkcheck.py --range ace15a6c62..HEAD`: exit 0. 23 checked, 23 VERIFIED.
- `python tools/prepush_attribution.py --base ace15a6c62 --head HEAD`: exit 0. 23 consolidated
  with credit intact, 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed ace15a6c62`: exit 0. No new local
  redeclarations and no new disagreements. The first draft failed on the Hurt and New spellings and
  on the FindEgg plurality flip. The fixes are in R3 and in the declarations, which now agree with
  the definitions.
- `python tools/check_dead_references.py`: exit 0. No new dead references and no broken markdown links.
- `python tools/tiers_ratchet.py --check`: exit 0. PASS, baseline 2994, current 3152. `--update` was not run.
- `python tools/check_tubuild_conflicts.py`: exit 0. 297 manifest entries.
- `python tools/queue_audit.py --check-promoted`: exit 0.
- `python tools/check_src_tu_compiles.py`: exit 0. 297/297.
- `python tools/port_refcheck.py`: exit 0. 408 checked.
- `python tools/affected_src.py include/daEykn_c.h`: the only consumer is `src/actors/daEykn_c.cpp`.
- Undefined symbols (pyelftools):
  - `build/tu/ov071-daEykn_c/daEykn_c.o` has 95 undefined symbols.
  - The base TU object has 82. The base stand-alone `_ZN8daEykn_c13InitResourcesEv` source, compiled with the
    same flags, has 24.
  - The new set equals the union of those two, minus three symbols this TU now defines:
    `_ZN8daEykn_c13InitResourcesEv`, and `func_ov071_02120c90` and `func_ov071_02121634` under
    their renamed rows.
  - Every other name is character-identical.
- `include/decl_common.h`, `config/dead-reference-baseline.json` and
  `config/decl-agreement-baseline.json`: empty diff against the base.
- Private validation: not run. The coordinator owns the PR.
