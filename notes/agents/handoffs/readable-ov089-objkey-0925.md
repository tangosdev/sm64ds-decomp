# Handoff: readable-ov089-objkey-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3154, task `readable-ov089-objkey-0925`, stage `revise`, role producer, session `claude-prod-readable-ov089-objkey-0925`, Claude Code.
- Round 2: stage `revise`, role producer, session `claude-prod-readable-ov089-objkey-0925-r2`, Claude Code, input `be3560b7134e0d0848116f20be7820b41d509185` (the round-1 output). Comment-only rework of verifier findings V1 and V2; see Round 2 below.
- Source branch and previous accepted input SHA: `readable/readable-ov089-objkey-0925`, input `c31f43bacfde4eb8e184b75077f85f4ba45a72a9` (origin/main at enqueue), which stays the tested base.
- Original source base SHA and installed workflow/tool SHA: both `c31f43bacfde4eb8e184b75077f85f4ba45a72a9`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments below are prose records (the source change tried and the measured result); no experiment artifact is committed.
- Next action, responsible role and blockers: independent verification (round 2) of this commit: comment accuracy of V1 and V2. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov089/daObjKey_c`, `src/actors/daObjKey_c.cpp`, ov089 .text 0x02131b18..0x021327d0 (10 functions). Promoted in #3126; this is its first readability pass. The issue carried no earlier finding list, so the list below is a self-audit of the base.
- Reserved source/header/config surfaces actually touched: `src/actors/daObjKey_c.cpp`, `include/daObjKey_c.h`, `config/arm9/overlays/ov089/symbols.txt` (two rows renamed in place), `config/tu_manifest.d/ov089/daObjKey_c.json`, this handoff, and this class's two keys in the `attribution.json` ledger. `config/arm9/overlays/ov089/delinks.txt` is reserved but unchanged: it holds only address ranges.
- ROM observations: ov089 relocs.txt has six of the eight state records point at 0x02131b18 (records 0x02132b00, 0x02132b08, 0x02132b10, 0x02132b20, 0x02132b28, 0x02132b38); 0x02132b18 points at 0x021311c0 and 0x02132b30 at 0x0213162c. `__sinit_ov089_021328d4` copies the records into the table at 0x02132cec, which Behavior indexes by the member at 0x444. No other reloc and no source names 0x02131b18. 0x02131f54 is called only from Behavior (relocs 0x021322f4, 0x02132364, 0x02132408). 0x02131dcc is also called from 0x021314dc (inside `func_ov089_021311c0`) and 0x02131df4 from 0x02131a78 (inside `func_ov089_0213162c`).
- Lineage evidence or structural inference: field meanings come from their uses in this TU and in the `func_ov089_0213115c`, `func_ov089_021311c0` and `func_ov089_0213162c` shards. 0x110 is stored with the Player passed to `func_ov089_02131df4` and read back for its position and angle. 0x41c..0x424 copy mPos in InitResources, and `func_ov089_021311c0` clamps mPosY to 0x420. 0x434 is the Vector3 that `func_ov089_021311c0` eases and hands to Camera::SetLookAt. 0x442 is the step counter each state switches on and increments. 0x443 counts bounces in `func_ov089_021311c0`. 0x448 is set by `func_ov089_0213115c` to an index into the collect-animation table (0 = none) and gates Render and Behavior. 0x44c and 0x458 hold Camera::lookAt and Camera::pos, restored when the key lands. 0x464..0x46c are the handles passed to and returned from Particle::System::New. 0x1f0 is the matrix handed to DropShadowRadHeight. 0x238, 0x240 and 0x244 are dCc_c::flags, hitFlags and otherOwner of the cylinder at 0x220.
- Hypothesized names/filenames, explicitly not recovered facts: `StateDrop` (0x02131b18, `_ZN10daObjKey_c9StateDropEv`) and `UpdateModelTransform` (0x02131f54, `_ZN10daObjKey_c20UpdateModelTransformEv`) are coined, and so are `mPlayer`, `mShadowMatrix`, `mHomePos`, `mCamLookAt`, `mStep`, `mBounceCount`, `mAnimID`, `mSavedCamLookAt`, `mSavedCamPos` and `mParticleID`. `include/daObjKey_c.h` marks each as coined next to its declaration.
- Compiler experiments and measured barriers: each was run with `python tools/tubuild.py verify ov089/daObjKey_c` on the working tree of this task, then kept or reverted as the Result column says.

| ID | Where | Change tried | Result |
|---|---|---|---|
| E1 | StateDrop | Member rename, then named fields, a `switch` on mStep in place of the goto dispatch, the `L()` launders dropped, Camera fields read as `cam->lookAt.x` | 9/10: step 0's copy differs (the offsets fold into the loads) |
| E2 | StateDrop | E1 with `Vector3 *lookAt = &cam->lookAt` and `pos` pointer copies | 10/10. Adopted. |
| E3 | StateDrop | `v.y = mPosY + 0x64000` inside the copy | DIFF, one relocation destination wrong |
| E4 | StateDrop | `mVertSpeed = -mVertSpeed >> 1; func_02012694(0x36, pos)`, two arguments as `src/func_02012694.cpp` defines it | 10/10. Adopted. |
| E5 | StateDrop | `data_0209f318` declared `Camera *` | bytes 10/10; decl gate FAIL (plurality `void *`). Declared `void *` with one cast. |
| E6 | 02131dcc, 02131df4 | Definitions retyped `(daObjKey_c *, Player *)` | bytes 10/10; decl gate FAIL against the `(char *, char *)` declarations in the two shards. Kept `(char *, char *)` with typed locals. |
| E7 | 02131df4 | `player->SetNoControlState(3, -1, 0)` and the local bridge declaration removed | bytes 10/10; decl gate FAIL: the removal flips the plurality for `_ZN6Player17SetNoControlStateEhih` and reds `src/game/actors/d_a_wanwan.cpp:678`. Bridge kept. |
| E8 | 02131df4 | Named fields; `ev`, `x` and the flags pointer temporaries dropped | 10/10. Adopted. |
| E9 | 02131df4 | Position copied as `key->mPlayer->mPosX` member by member | DIFF. The pointer copy stays with one comment. |
| E10 | UpdateModelTransform | Member rename; `mModelAnim.mat4x3`, `mModel.mat4x3`, `mShadowMatrix` | 10/10. Adopted. |
| E11 | UpdateModelTransform | `dActor_c::DropShadowRadHeight` with `Fix12<int>` locals | DIFF: the two values load from the literal pool (0x130 grows by 4 words). Mangled call kept with one comment. |
| E12 | Render | `Model *` locals hoisted and the flag dropped | DIFF, 35 words |
| E13 | Render | `mModelAnim.Render(0)` and `mModel.Render(0)` virtual calls, `hidden` flag kept | 10/10. Adopted; the `Sub` dummy class is gone. |
| E14 | Render | E13 with `if (mFlags & 0x40000)` and no flag | DIFF |
| E15 | Behavior | Named fields; `UpdateYoshiEat`, `dCc_c::Clear`, `Update`, `UpdatePos`, `SetPosRelativeToActor` on Vector3 locals, `MarkForDestruction`; the `LAUNDER` pointers on mSpinSpeed and mAngleY dropped | 10/10. Adopted. |
| E16 | Behavior | `data_020a0e68` declared `Matrix4x3`, translation read as `.m[9..11]` into a `Vector3 vec` | 10/10. Adopted. |
| E17 | Behavior | Player position copied as `player->mPosX` member by member | DIFF. Pointer copy kept with one comment. |
| E18 | Behavior | `if (actorID == 0x11a)` without the int flag | DIFF, two relocation destinations wrong |
| E19 | Behavior | State table typed `void (daObjKey_c::*)()` in place of the dummy `C` class and `PmfEntry` | 10/10. Adopted. |
| E20 | InitResources | `Animation::LoadFile`, `ModelBase::SetFile`, `Sound::PlayBank3`, `ShadowModel::InitCylinder`, `dBgCh_Actr::SetLimMovFlag`, Vector3 locals, named fields, a chained particle-handle clear | 10/10. Adopted. |
| E21 | InitResources | `dCcAcPos_c::Init` with `Fix12<int>` locals | DIFF, seven relocation destinations wrong. Mangled call kept. |
| E22 | OnTurnIntoEgg | `if (actorID == 0x11a)` without the flag | DIFF. Flag kept with one comment. |
| E23 | Whole TU | Dead declarations removed (the bridges replaced above, `L`, `LAUNDER`, `Sub`, `C`, `PmfEntry`, `decl_dBgCh_Actr.h`, local copies of `data_0209f2ac`, `func_ov089_0213115c` and `_ZN5Sound22LoadAndSetMusic_Layer3Ej` that `include/decl_common.h` already declares) | 10/10, undefined-symbol set unchanged. Adopted. |

## Round 2

Verifier findings on `be3560b713`. Only comment lines in `src/actors/daObjKey_c.cpp` and this handoff changed; `include/daObjKey_c.h` needed no change.

| ID | Finding | Outcome | Evidence |
|---|---|---|---|
| V1 | The bridge comment above the `extern "C"` block said every mangled name is a call the headers cannot spell; false for `Player::SetNoControlState` | Fixed | The comment now gives one reason per bridge, each checked against the header: `include/Camera.h` has no SetFlag_3 and `include/Particle__System.h` has no New (missing declaration); DropShadowRadHeight and dCcAcPos_c::Init take Fix12<int> by value and the real calls do not match (E11, E21); `include/ModelAnim.h:88` declares SetAnim with Fix12<int> and the real call is untried; `include/dBgCh_Actr.h:113` declares Init with Fix12i, a typedef of s32 that mangles as `i`, not `5Fix12IiE`; `include/Player.h:512` declares SetNoControlState and the real call matches, and the local declaration stays for the plurality `src/game/actors/d_a_wanwan.cpp:678` needs (E7, R8). |
| V2 | The banner called `func_ov089_0213162c` unmatched | Fixed | Banner now says it byte-matches but is enrolled without a `complete` marker (ov089 `delinks.txt:35-36`), so the build keeps the cartridge's bytes there. `python tools/match.py --c src/func_ov089_0213162c.c --func func_ov089_0213162c --addr 0x0213162c --size 0x4ec --version 2004/b56 --module ov089 --strict-relocs` reports MATCHING VERSIONS: 2004/b56. The banner's other claims were rechecked against ov089 `delinks.txt`: the run 0x02131b18..0x021327d0 (lines 38-40), D1 0x02130f00 and D0 0x02130f50 (lines 7-13), and the `func_ov089_02130fb4`, `UnloadKeyModels`, `LoadKeyModels`, `func_ov089_0213115c` and `func_ov089_021311c0` shards (lines 15-33), all with `complete` markers. |
| V2a | `config/tu_manifest.d/ov089/daObjKey_c.json` `boundary_evidence[0]` repeats "the unmatched func_ov089_0213162c" and "the unmatched hole" | Still deferred | Not a comment; round 2 is limited to comments and this handoff. Owner: next producer on #3154 (https://github.com/tangosdev/sm64ds-decomp/issues/3154). |
| V3 | `mState` is really the key kind | Still deferred | Left as instructed. |

Round 2 proof, run in `C:/tmp/claude-rd-ov089` on this commit's source:

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov089/daObjKey_c` | 0 | 10/10 MATCH, objisolate clean, reloc-destinations clean; manifest not rewritten |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/check_decl_agreement.py --changed c31f43bacf` | 0 | no new local redeclarations, no new declaration disagreements |

## Findings

Self-audit of the base, R1 to R24. Deferred rows name the owner as the next producer the coordinator assigns on #3154 (https://github.com/tangosdev/sm64ds-decomp/issues/3154).

| ID | Category | Finding at base | Outcome | Reason or evidence |
|---|---|---|---|---|
| R1 | extern "C" state bodies | `func_ov089_02131b18`, the state body for six key kinds, is a C-linkage free function on `char *` | Fixed | Member `StateDrop` (coined); symbols.txt row, manifest symbol and attribution key renamed in place. The state records reach it by address (see ROM observations). E1, E2. |
| R2 | extern "C" helpers | `func_ov089_02131f54` is a C-linkage free function | Fixed | Member `UpdateModelTransform` (coined), renamed the same way. `include/decl_common.h` still declares `func_ov089_02131f54(void *)`; nothing uses that declaration, and the file is off limits (R20). E10. |
| R3 | extern "C" helpers | `func_ov089_02131dcc` and `func_ov089_02131df4` are C-linkage on `char *` | Partially fixed | Bodies use typed locals, named fields and real calls (E6, E8). The names and `(char *, char *)` signature stay: the enrolled `src/func_ov089_021311c0.c` and `src/func_ov089_0213162c.c` shards call them by name, outside this reservation. Owner: next producer on #3154. |
| R4 | mangled bridges | Camera::SetLookAt and SetPos, dBgCh_Actr::JustHitGround, IsOnGround and SetLimMovFlag, dActor_c::FindWithID and UpdatePos, dEnemyBase_c::UpdateYoshiEat, dCc_c::Clear and Update, dCcAcPos_c::SetPosRelativeToActor, fBase_c::MarkForDestruction, Event::SetBit and ClearBit, Animation::LoadFile, ModelBase::SetFile, Sound::PlayBank3, ShadowModel::InitCylinder | Fixed | Real calls (E1, E13, E15, E20); Event is declared locally in namespace form as other sources do. Undefined-symbol set unchanged. |
| R5 | mangled bridges | `ModelAnim::SetAnim`, `dCcAcPos_c::Init`, `dActor_c::DropShadowRadHeight`, `Particle::System::New` | Deferred: measured, no committed artifact | Fix12<int> by value: E11, E21. SetAnim and Particle::System::New take the same Fix12<int> arguments and are not declared by any header this TU may edit; not tried separately. Owner: next producer on #3154. |
| R6 | mangled bridges | `dBgCh_Actr::Init` | Still deferred | `include/dBgCh_Actr.h` declares Init with `Fix12i`, which mangles to a different symbol; shared header outside this reservation. |
| R7 | mangled bridges | `Camera::SetFlag_3`, `Sound::LoadAndSetMusic_Layer3` | Still deferred | `include/Camera.h` does not declare SetFlag_3 and `include/Sound.h` does not declare LoadAndSetMusic_Layer3; both shared headers are outside this reservation. |
| R8 | mangled bridges | `Player::SetNoControlState` | Deferred: measured, no committed artifact | E7: the real call matches, but dropping the local bridge declaration flips the decl-gate plurality and reds an untouched file. |
| R9 | unk_ fields | `unk_110`, `unk_41c`, `unk_420`, `unk_424`, `unk_442`, `unk_443`, `unk_448`, `unk_464`, `unk_468`, `unk_46c` | Fixed | Coined names from their uses (see Lineage); 0x110 typed `Player *`, 0x41c a Vector3. |
| R10 | raw offsets | Whole free-function bodies (`func_ov089_02131b18`, `02131dcc`, `02131df4`, `02131f54`) written in raw `c + 0x...` offsets | Fixed | E1, E2, E8, E10. No raw `this` offset remains in the TU. |
| R11 | raw offsets | `pad_1f0` and `pad_44c` hid a matrix and two camera vectors | Fixed | `mShadowMatrix`, `mSavedCamLookAt`, `mSavedCamPos`; 0x434 named `mCamLookAt`. D1 and D0 still match (see Proof). |
| R12 | raw offsets | `*(int *)((char *)mModelAnim.data.bones + 0xc)` in Behavior | Still deferred | `BMD_Bone` declares 0x08..0x33 as padding in `include/BMD_File.h`, a shared header. |
| R13 | casts | `(Vector3 *)&mScaleX` and `(Vector3 *)&mCamSpacePosX` over inherited scalars | Still deferred | `include/dActor_c.h` declares them as scalars; shared header. |
| R14 | casts | SharedFilePtr records reached through `int` globals and `(char *)x + 4` file pointers | Still deferred | `include/SharedFilePtr.h` declares no fields; shared header. |
| R15 | goto | `func_ov089_02131b18` dispatched through gotos | Fixed | E1: a `switch` matches. |
| R16 | temporaries, launders | `L()` and `LAUNDER()` identity macros around pointer temporaries | Fixed | Both macros are gone (E2, E8, E15). Three pointer copies remain where the direct form differs (E2, E9, E17), each with one comment. |
| R17 | temporaries | int flags before the `actorID == 0x11a` tests and the Render flag | Deferred: measured, no committed artifact | E14, E18, E22. One comment each. |
| R18 | dead or local declarations | Dummy classes `C`, `PmfEntry`, `Sub`; the replaced bridges; local copies of three `include/decl_common.h` declarations; the `decl_dBgCh_Actr.h` include | Fixed | E19, E23. |
| R19 | local declarations | `LoadKeyModels`, the matrix and vector helpers, `func_02012694`, `func_ov002_020c3dbc` are declared locally | Partially fixed | `func_02012694` and `func_ov002_020c3dbc` now match their definitions. `LoadKeyModels` is declared in `include/Door.h`, which this TU does not include; the helpers have no header. The decl gate reports no new local redeclaration. |
| R20 | address-named data | `data_ov089_02132cec`, `021328b4`, `02132894`, `02132c40`, `02132c48`, `02132c60`, `02132c70`, `02132b40`, `02132ca4` | Still deferred | Also named by `__sinit_ov089_021328d4`, `LoadKeyModels`, `UnloadKeyModels` and `Door::InitResources` sources outside this reservation; several carry fractured declared types that a retype here would flip. `include/decl_common.h` (off limits) declares `data_ov089_02132894` and the now unused `func_ov089_02131f54`. |
| R21 | address-named data | `data_0209f318`, `data_0209b454`, `data_0209caa0`, `data_0209f2ac`, `data_020a0e68`, `data_0209cef0`, `data_ov002_02110964`, `data_ov002_0211094c` | Still deferred | Not ov089 rows. `data_020a0e68` is now declared `Matrix4x3`, the plurality type. |
| R22 | unmeasured codegen notes | The `func_ov089_02131df4` comment "force separate base materialization like ROM" | Fixed | Retested (E8, E9): the flags pointer was unnecessary and was removed; the position pointer is needed and its comment now names what it prevents. |
| R23 | stale manifest notes | The `_ZN7Vector3D1Ev` reason names `func_ov089_02131b18`; no note maps the renames | Fixed | See Changed paths. |
| R24 | stale ledgers | `config/match_attempts.jsonl`, `config/match_provenance.jsonl`, `notes/data/c-cpp-classification.tsv` and `notes/data/tu-merge-candidates.json` keep the old names | Still deferred | Not in this reservation; append logs and generated data keyed by address. |

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 10/10 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: 8 members (OnTurnIntoEgg, OnYoshiTryEat, CleanupResources, Render, Behavior, InitResources, StateDrop, UpdateModelTransform) and 2 C-linkage helpers (R3). Mangled callee bridges remain for 9 symbols (R5 to R8): ModelAnim::SetAnim, dCcAcPos_c::Init, dBgCh_Actr::Init, dActor_c::DropShadowRadHeight, Particle::System::New, Camera::SetFlag_3, Player::SetNoControlState, Sound::LoadAndSetMusic_Layer3 (through `include/decl_common.h`), plus the linker veneer `dBgCh_Actr_UpdateContinuous_Veneer`, which is itself a ROM symbol.
- Recovered layout/fields; remaining shadow structs/raw offsets: every daObjKey_c field is named except `pad_428`; no raw `this` offset remains. R12 to R14 remain.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The two new members are non-virtual. D1 and D0 stay shards and still match against the new header.
- Attribution preserved through each move/rename: the two `attribution.json` keys follow the renamed symbols with the same authors (`lunavyqo` keeps StateDrop, `tangosdev` keeps UpdateModelTransform).
- Remaining agreed issue scope: R3, R5 to R8, R12 to R14, R17, R19 to R21, R24.

Residue. The `lines` column is `wc -l` of `git show <rev>:<path>` (newline count; every revision listed ends in a newline). Each token column is the number of lines containing the token, `grep -c -F`, with `MSYS_NO_PATHCONV=1`. Occurrence counts differ only at base for the source (21 `unk_` and 31 `func_` occurrences) and at this commit for `func_` (21 occurrences on 20 lines).

| Path | Revision | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|---|
| `src/actors/daObjKey_c.cpp` | base `c31f43bacf` | 520 | 17 | 30 | 5 | 83 |
| `src/actors/daObjKey_c.cpp` | round 1 `be3560b713` | 456 | 0 | 20 | 3 | 38 |
| `src/actors/daObjKey_c.cpp` | this commit | 467 | 0 | 20 | 3 | 38 |
| `include/daObjKey_c.h` | base `c31f43bacf` | 92 | 13 | 0 | 0 | 1 |
| `include/daObjKey_c.h` | this commit | 75 | 0 | 0 | 0 | 1 |

Of the 38 `_ZN` lines, 8 are symbol-marker comments and 8 are the banner comments above them. The header's one `_ZN` line is the destructor symbol in the class comment.

## Changed paths

`git diff --name-only c31f43bacf..HEAD` lists six paths:

| Path | Change |
|---|---|
| `attribution.json` | two keys renamed with their symbols, authors unchanged |
| `config/arm9/overlays/ov089/symbols.txt` | two rows renamed in place (0x02131b18, 0x02131f54) |
| `config/tu_manifest.d/ov089/daObjKey_c.json` | `functions[0].symbol` and `functions[5].symbol` (the renames), `compiler_only_output[0].reason` (the Vector3 users by their current names), `notes[1]` added (the rename map and why 02131dcc and 02131df4 keep C linkage). `notes[0]`, `boundary_evidence`, `sections` and `verification` were reread against this commit and are unchanged; `droppedSections` stays `[7]`. |
| `include/daObjKey_c.h` | field names and types, the two new member declarations, class comment shortened |
| `notes/agents/handoffs/readable-ov089-objkey-0925.md` | this handoff |
| `src/actors/daObjKey_c.cpp` | the source changes above |

## Proof

Commands run in `C:/tmp/claude-rd-ov089` on the source of this commit (the handoff is the only later addition).

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov089/daObjKey_c` | 0 | 10/10 MATCH; objisolate clean; relocation destinations clean; at base also 10/10 |
| Object comparison with pyelftools, the object `tubuild verify` builds from base `c31f43bacf` against the one it builds from this commit, under the two-row rename map | 0 | 25 sections in each object; every function body identical (the 10 ROM functions plus `_ZN7Vector3D1Ev`, and the mapping symbols); 9/9 relocation sections identical; undefined-symbol set identical (60 symbols); defined global set (10 symbols) differs only by the two renames |
| `python tools/match.py --c src/_ZN10daObjKey_cD1Ev.cpp --func _ZN10daObjKey_cD1Ev --addr 0x02130f00 --size 0x50 --version 2004/b56 --module ov089`, and the same for D0 at 0x02130f50 size 0x64 | 0 | both MATCH (relocation-strict) with the new header |
| `python tools/rombuild.py -j16` (full, with ROM) | 0 | 7100 enrolled sources, 0 reused and 7100 compiled (`build/src/actors/daObjKey_c.o` rebuilt in this run); 11,213 of 11,213 source-built functions reproducing, 0 mismatching; module fidelity 106/106 exact; intact TU gates PASS with zero new symbol errors; `intactTuRom.identical` true. Its baseline control (no TU substitution) reports the pre-existing `dsd check symbols` failure on 9 arm9 and ITCM symbols. |
| `python tools/prepush_linkcheck.py --range c31f43bacf..HEAD` | 0 | 12 checked, 12 verified (D1, D0 and the 10 TU functions), 0 warnings, 0 blocking |
| `python tools/check_src_tu_compiles.py` | 0 | 299/299 translation units compile |
| `python tools/check_decl_agreement.py --changed c31f43bacf` | 0 | no new disagreements, no new local redeclarations |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links (rerun after the handoff was written) |
| `python tools/tiers_ratchet.py --check` | 0 | PASS (baseline 2998, current 3153) |
| `python tools/check_tubuild_conflicts.py` | 0 | 299 entries OK |
| `python tools/queue_audit.py --check-promoted` | 0 | OK |
| `python tools/port_refcheck.py` | 0 | 408 references, all resolve |
| `python tools/prepush_attribution.py --base c31f43bacf --head HEAD` | 0 | 7273 tracked, 0 changed, 0 lost |
| `python tools/affected_src.py include/daObjKey_c.h` | 0 | `src/_ZN10daObjKey_cD0Ev.cpp`, `src/_ZN10daObjKey_cD1Ev.cpp`, `src/actors/daObjKey_c.cpp` |
| `git diff c31f43bacf -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` | 0 | empty |
| `python tools/check_references.py` | 1 | not run: it needs `build/rombuild-eligibility.json`, which this worktree does not have |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify, the D1 and D0 match runs and prepush_linkcheck, above.
- Complete emitted TU and data/metadata checks: the object comparison covers every function, relocation and symbol of the TU object. This TU licenses .text only; its data and virtual members are unchanged.
- Shared-header consumer expansion: `include/daObjKey_c.h` reaches D1, D0 and this TU; all three verified. No other header changed.
- Port/path/reference and other applicable static gates: above.
- Private validation, if run, and the exact PR head/base it tested: not run.
