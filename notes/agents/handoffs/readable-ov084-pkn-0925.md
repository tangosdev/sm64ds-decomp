# Handoff: readable-ov084-pkn-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/2473, task `readable-ov084-pkn-0925`, stage `revise`, role producer, session `claude-prod-readable-ov084-pkn-0925` (round 1) and `claude-prod-readable-ov084-pkn-0925-r2` (round 2, handoff-only corrections V1 and V2 and the new V3 row), Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov084-pkn-0925`, round 1 input `ace15a6c626d8fd133dd8accc41dd313be567d96` (origin/main at enqueue), round 2 input `6aaaf287e337f04c5e5ef336ae5796debe35b207`. Round 2 changes only this handoff.
- Original source base SHA and installed workflow/tool SHA: both `ace15a6c626d8fd133dd8accc41dd313be567d96`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments below are prose records (the source change tried and the measured result); no experiment artifact is committed.
- Next action, responsible role and blockers: independent verification (round 2) of this commit, handoff accuracy (V1, V2). No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov084/daPkn_c`, `src/actors/daPkn_c.cpp`, ov084 .text 0x0212eaf0..0x02130174 (24 functions). Promoted in #3115; this is its first readability pass.
- Reserved source/header/config surfaces actually touched: `src/actors/daPkn_c.cpp`, `include/daPkn_c.h`, `config/arm9/overlays/ov084/symbols.txt` (nine rows renamed in place), `config/tu_manifest.d/ov084/daPkn_c.json` (see Changed paths), this handoff, and this class's nine keys in the `attribution.json` ledger. `config/arm9/overlays/ov084/delinks.txt` is reserved but unchanged: it holds only address ranges.
- ROM observations: the vtable at 0x02130c28 was reread from `extracted/overlays/overlay_0084.bin`. 31 nonzero slots end at 0x02130ca4, slot 29 (0x02130c9c) is 0x0212ec58 (OnAimedAtWithEgg), and seven zero words run to the .data end at 0x02130cc0. The .data records at 0x02130ba4..0x02130be4 hold the nine state bodies by address (ov084 relocs.txt rows from 0x02130ba4 to 0x02130be4), and `src/__sinit_ov084_02130654.c` copies them into the table at 0x02130e80 in state order 0 to 8: 0x0212fc10, 0x0212fa7c, 0x0212f6d8, 0x0212f630, 0x0212f588, 0x0212f460, 0x0212f33c, 0x0212f2dc, 0x0212f298.
- Lineage evidence or structural inference: the nine state bodies take the object in r0 and are reached only through the member-pointer table, so they are written as members. Field meanings come from their uses: 0x460 holds `dActor_c::ClosestPlayer()` and is dereferenced as a Player (mVertSpeed, mHorzSpeed, mIsAirborne); 0x464 holds `Vec3_Dist` to it or 0x7fffffff; 0x46c copies `Player::mIsAirborne` (0x6de); 0x474 is the second particle handle (effect 0xfd, zeroed with 0x470); 0x478 is the `Sound::PlayLong` handle; 0x45d is set to 1 when the fade of sub-sound 0x36 to 0 is accepted and to 0 when the fade-in is issued.
- Hypothesized names/filenames, explicitly not recovered facts: `StateWait` (0x0212fc10, `_ZN7daPkn_c9StateWaitEv`), `StateSleep` (0x0212fa7c, `_ZN7daPkn_c10StateSleepEv`), `StateBite` (0x0212f6d8, `_ZN7daPkn_c9StateBiteEv`), `StateWake` (0x0212f630, `_ZN7daPkn_c9StateWakeEv`), `StateDoze` (0x0212f588, `_ZN7daPkn_c9StateDozeEv`), `StateDie` (0x0212f460, `_ZN7daPkn_c8StateDieEv`), `StateShrink` (0x0212f33c, `_ZN7daPkn_c11StateShrinkEv`), `StateGone` (0x0212f2dc, `_ZN7daPkn_c9StateGoneEv`) and `StateRegrow` (0x0212f298, `_ZN7daPkn_c11StateRegrowEv`) are coined, and so are the field names `mSubSoundFadedOut`, `mClosestPlayer`, `mPlayerDist`, `mPlayerAirborne`, `mParticleHandle2` and `mLoopSoundHandle`. `include/daPkn_c.h` marks both groups as coined next to the declarations.
- Compiler experiments and measured barriers: see the table below. Each was run with `python tools/tubuild.py verify ov084/daPkn_c` on the working tree of this task and then kept or reverted as the Result column says.

| ID | Where | Change tried | Result |
|---|---|---|---|
| E1 | func_ov084_0212f204 | Named fields, `v.x = p->mPosX` and so on | 0x94 -> 0x90; the offset folds into the loads |
| E2 | func_ov084_0212f204 | Named fields, copy through `Vector3 *pos = (Vector3 *)&p->mPosX` member by member | 24/24. Adopted. |
| E3 | func_ov084_0212f204 | E2 with `v = *(Vector3 *)&p->mPosX` | DIFF |
| E4 | func_ov084_0212f204 | E2 with `p->mIsAirborne` in place of the reload through `c->mClosestPlayer` | DIFF |
| E5 | StateBite | Named fields, dCc_c `hitFlags` and `otherOwner`, Player fields, `dActor_c::FindWithID`, `Player::IncMegaKillCount` | 24/24. Adopted. |
| E6 | StateBite | `if (type == 0xbf)` and `if (type == 9)` in place of the int flags | DIFF, one relocation destination wrong |
| E7 | StateDie | Named fields with the volatile stack stores kept; then `zero` before `fp` | 24/24 both ways: the register-order comment was no longer true |
| E8 | StateDie | Volatile stores replaced by plain stores into `int space[3]` | DIFF |
| E9 | StateDie | A `Vector3 pos` local, no `fp`, no `zero`, no volatile | 24/24. Adopted. |
| E10 | StateShrink | `dActor_c::Spawn(0x122, 0, v, 0, mAreaId, -1)` in place of the bridge fed with `scale` | 24/24. Adopted. |
| E11 | StateShrink | `v.x = mPosX; v.y = mPosY + 0x78000; v.z = mPosZ;` in place of the y and z temporaries | 14 words differ |
| E12 | func_ov084_0212ef00 | Named fields and real calls, gotos kept | 24/24. Adopted, then E14. |
| E13 | func_ov084_0212ef00 | No goto, each outcome returning where it happens | 0x2d0 -> 0x2f0 |
| E14 | func_ov084_0212ef00 | No goto: else-if chain with one `return 1` for the first cylinder, the second falling through to `return 0` | 24/24. Adopted. |
| E15 | func_ov084_0212ef00 | E14 with `type == 0xbf` and `type == 9` tested directly | DIFF, one relocation destination wrong |
| E16 | func_ov084_0212ec60 | Named fields, the `AT()` launder dropped | 24/24. Adopted. |
| E17 | InitResources | `mHomePos = mHeadClsnOffset` | DIFF, three relocation destinations wrong |
| E18 | InitResources | `Vector3 *home = &mHeadClsnOffset; mHomePos = *home;` | 24/24. Adopted. |
| E19 | InitResources | Bubble position folded into three assignments | 36 words differ |
| E20 | Behavior | `mStateTimer = (u16)mStateTimer + 1` | 4 words differ |
| E21 | Header | Class operator new and the `_ZN7fBase_cnwEj` declaration removed | 24/24, undefined symbols unchanged. Adopted. |
| E22 | Whole TU | Animation::Finished, WillHitFrame and SetFlags through `mModelAnim`; `Sound::PlayLong`; `ApproachLinear(short &, short, short)`; `#include "Player.h"` and `"Sound.h"`; `decl_Animation.h` dropped | 24/24, undefined symbols unchanged. Adopted. |

Where a ruled-out alternative leaves a codegen-forced form in the source (E3, E6, E11, E15, E17, E19, E20), that form carries one short comment. E4 and E8 need none: the reload is plain code, and E9 replaced the volatile stores.

## Findings

Self-audit of the base, numbered R1 to R33, with the four #2473 items folded in (I1 to I4). Deferred rows name the owner as the next producer the coordinator assigns on #2473 (https://github.com/tangosdev/sm64ds-decomp/issues/2473).

| ID | Category | Finding at base | Outcome | Reason or evidence |
|---|---|---|---|---|
| R1 (I1) | extern "C" state bodies | The nine state bodies are C-linkage `func_ov084_*` free functions | Fixed | Members under coined names; symbols.txt rows, manifest symbol fields and attribution keys renamed in place. The .data records reach them by address. |
| R2 | extern "C" helpers | `func_ov084_0212ebb4`, `0212ec04`, `0212ec60`, `0212ef00`, `0212f1d0`, `0212f204` are C-linkage | Partially fixed | Their bodies now use named fields and real calls. They keep C linkage because `include/decl_common.h` declares all six by these names and is off limits; `src/actors/daFPkn_c.cpp` also names `func_ov084_0212ec04`. Owner: next producer on #2473. |
| R3 | mangled bridges | C-linkage bridges for `dActor_c::FindWithID`, `ClosestPlayer`, `JumpedOnByPlayer`, `Spawn`, `Player::IncMegaKillCount`, `Animation::Finished`, `WillHitFrame`, `SetFlags` (through `include/decl_Animation.h`), `Sound::PlayLong`, `ApproachLinear` | Fixed | E5, E10, E12, E22. The undefined-symbol set is unchanged. |
| R4 (I3) | mangled bridges | `ModelAnim::SetAnim`, `dCcAc_c::Init`, `dCcAcPos_c::Init`, `dBgCh_Actr::Init` in InitResources and the states | Still deferred | Not remeasured here. The earlier alternatives (p06, p15 and the Init probes) are recorded in `notes/agents/handoffs/pr-2450-source-review-fixes.md`; `include/dBgCh_Actr.h` declares Init with `Fix12i`, which mangles to a different symbol. Owner: next producer on #2473. |
| R5 | mangled bridges | `Particle::System::New`, `NewSimple`, `Sound::PlaySub`, `Player::Hurt`, `Player::Bounce` | Still deferred | Each symbol carries `Fix12<int>` by value. `include/Player.h` declares neither Hurt nor Bounce and `include/Sound.h` leaves PlaySub out on purpose; both are shared headers outside this reservation. Not measured. Owner: next producer on #2473. |
| R6 | mangled bridges | The header forwards operator new to `extern "C" _ZN7fBase_cnwEj` | Fixed | E21: `new` binds the inherited `fBase_c::operator new`. |
| R7 | unk_ fields | `unk_45d`, `unk_460`, `unk_464`, `unk_46c`, `unk_474`, `unk_478` | Fixed | Coined names from their uses (see Lineage); 0x460 typed `Player *`. |
| R8 | unk_ fields | `unk_108`, read and written by InitResources, Behavior and StateShrink | Still deferred | Declared in `include/dEnemyBase_c.h`, a shared header outside this reservation. Owner: next producer on #2473. |
| R9 (I2) | raw offsets | StateBite (then `func_ov084_0212f6d8`) is written entirely in raw offsets | Fixed | E5. |
| R10 (I2) | raw offsets, launders | StateDie (then `func_ov084_0212f460`): raw offsets, volatile stack stores, a register-order trick | Fixed | E7, E9. |
| R11 (I2) | raw offsets | `func_ov084_0212f204` raw offsets | Fixed | E2; the member-by-member copy stays with a comment (E3). |
| R12 (I2) | raw offsets | `func_ov084_0212ef00` raw offsets | Fixed | E12. |
| R13 (I2) | raw offsets, launders | `func_ov084_0212ec60` raw offsets and the `AT()` launder | Partially fixed | E16. The `PknVec3` and `PknMtx43` views over named members stay; not remeasured. |
| R14 (I2) | raw offsets | `(char *)c + 0x160` Animation casts | Fixed | E22. |
| R15 (I2) | raw offsets | Raw `0x168` stores in StateDoze and `func_ov084_0212ec04` | Fixed | `mModelAnim.currFrame`; `func_ov084_0212ec04` also stores `(u16)frame << 12` in place of the shift pair. |
| R16 (I2, I3) | raw offsets | InitResources copies `(Vector3 *)((int)this + 0x440)` into mHomePos | Partially fixed | E18: named, through a pointer; the direct assignment differs (E17). |
| R17 (I2) | casts | `(Vector3 *)&mPosX`, `&mScaleX`, `&mCamSpacePosX` over inherited scalars | Still deferred | `include/dActor_c.h` declares them as scalars; shared header outside this reservation. Owner: next producer on #2473. |
| R18 | casts | `PknSharedFile` records cast to `SharedFilePtr` | Still deferred | `include/SharedFilePtr.h` declares no fields; shared header outside this reservation. |
| R19 | goto | `func_ov084_0212ef00` is built from gotos | Fixed | E13, E14. |
| R20 | temporaries | int flags `isPlayer`, `isNine`, `t`, `u` hold the actor-type compares | Deferred: measured, no committed artifact | E6, E15. One comment each in StateBite and `func_ov084_0212ef00`. |
| R21 | temporaries | Behavior counts `mStateTimer` through a `u16 *` | Deferred: measured, no committed artifact | E20. The comment is shortened to one line. |
| R22 | temporaries | Temporaries for the bubble position in InitResources and the spawn position in StateShrink | Deferred: measured, no committed artifact | E19, E11. One comment each. |
| R23 | launders | `volatile s16 ang[3]` and the dead bone sum in `func_ov084_0212ec60` | Still deferred | Not tried. The source says the sum is never read. |
| R24 | dead declarations | Local prototypes of `func_ov084_0212f33c` and `0212f460`, the R3 bridge declarations, the `AT()` macro, `_ZTV7daPkn_c` in the header, the `decl_Animation.h` include | Fixed | The eight local bridge declarations of R3 and the rest are removed; no consumer remains (`include/decl_common.h` still declares `_ZTV7daPkn_c` for the C files). |
| R25 | local declarations | `Vec3_Dist`, `Vec3_HorzAngle`, `LoadBlueCoinModel`, the matrix helpers and `func_0201267c`, `func_02012694`, `func_020105cc` are declared locally; `ApproachLinear` is now declared locally in its real C++ form | Still deferred | No header this TU includes declares them; `check_decl_agreement` reports no new local redeclaration. Not examined further. |
| R26 | address-named data | `data_ov084_02130dfc`, `02130df4`, `02130e0c`, `02130e14`, `02130e24`, `021302f4`, `0213030c`, `02130334`, `021302ec`, `02130e80` | Still deferred | Also named by `src/__sinit_ov084_02130654.c`, outside this reservation; renaming the rows alone would strand it. |
| R27 | address-named data | `data_ov002_0210da38`, `data_02082214`, `data_020a0e68`, `func_0201267c`, `func_02012694`, `func_020105cc` | Still deferred | Not ov084 rows. |
| R28 | unmeasured codegen notes | The file header listed walls: Finished and WillHitFrame keep +0x160, `func_ov084_0212f204` and `0212f460` named fields size-DIFF, `(int)this + 0x440`, StateDie's register order | Fixed | Each was tested (E2, E7, E9, E18, E22); the false ones are gone and each kept comment gives its measured result. |
| R29 | unmeasured codegen notes | The header listed Sound::PlayLong as a Fix12 wall | Fixed | `_ZN5Sound8PlayLongEjjjRK7Vector3s` carries no Fix12; the call is real (E22). |
| R30 (I4) | stale manifest notes | The vtable was said to end at the section boundary | Fixed before this task, verified | At base, both the manifest `_ZTV7daPkn_c` reason and the source header already say the 31 slots end at 0x02130ca4 with seven zero words before 0x02130cc0; the ROM bytes agree (see ROM observations). |
| R31 | stale manifest notes | 25 notes say "the declaration policy the file header states"; the `_ZN7Vector3D1Ev` reason lists the Vector3 users by their old names; no note maps the renamed states | Fixed | See Changed paths. |
| R32 | stale notes | `notes/data/class-facts/daPkn_c.json` still names the fields `unk_45d` to `unk_478` and the class PiranhaPlant | Still deferred | Not in this task's reservation. |
| R33 | stale ledgers | `config/match_attempts.jsonl` and `config/match_provenance.jsonl` rows keep the nine old names | Still deferred | Append logs keyed by address, left to their owners. |
| V3 | GNU extensions | StateShrink builds its spawn position with a GNU compound literal, `v = (Vector3){mPosX, y + 0x78000, z}`, already present at base as `(struct Vector3){c->mPosX, y + 0x78000, z}` in `func_ov084_0212f33c` | Deferred | New in verification; not attempted in this handoff-only revision. Owner: next producer on #2473 (https://github.com/tangosdev/sm64ds-decomp/issues/2473). |

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 24/24 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: 17 member functions (D1, D0, OnAimedAtWithEgg, InitResources, Behavior, Render, OnPendingDestroy, CleanupResources and the nine states) plus the factory and 6 C-linkage helpers (R2), 24 in all; 9 mangled callee bridges remain (R4, R5).
- Recovered layout/fields; remaining shadow structs/raw offsets: every daPkn_c field is named. No raw `this` offset remains. `PknVec3`, `PknMtx43` and `PknSharedFile` views remain (R13, R18), and so do the Vector3 casts over dActor_c scalars (R17).
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The nine new members are non-virtual, so layout and vtable are unchanged.
- Attribution preserved through each move/rename: the nine `attribution.json` keys follow the renamed symbols with the same authors (`ruspecial` keeps StateWait, `lunavyqo` keeps StateBite, StateDie and StateShrink, `tangosdev` the other five).
- Remaining agreed issue scope: R2, R4, R5, R8, R13, R17, R18, R20 to R23, R25 to R27, R32, R33, V3.

Residue. The `lines` column is `wc -l` of `git show <rev>:<path>` (newline count; both revisions end in a newline). Each token column is the number of lines containing the token, `grep -c -F`, with `MSYS_NO_PATHCONV=1`. At base `src/actors/daPkn_c.cpp` has 30 `unk_` and 78 `_ZN` occurrences on 29 and 77 lines; at this commit lines and occurrences agree for every token.

| Path | Revision | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|---|
| `src/actors/daPkn_c.cpp` | base `ace15a6c62` | 939 | 29 | 78 | 16 | 77 |
| `src/actors/daPkn_c.cpp` | this commit | 839 | 4 | 59 | 8 | 52 |
| `include/daPkn_c.h` | base `ace15a6c62` | 122 | 7 | 0 | 1 | 5 |
| `include/daPkn_c.h` | this commit | 111 | 0 | 0 | 0 | 2 |

The four `unk_` lines left are `unk_108` (R8). Nine of the 52 `_ZN` lines are the marker comments naming the new state members.

## Changed paths

`git diff --name-only ace15a6c62..HEAD` lists six paths:

| Path | Change |
|---|---|
| `attribution.json` | nine keys renamed with their symbols, authors unchanged |
| `config/arm9/overlays/ov084/symbols.txt` | nine rows renamed in place (0x0212f298, 0x0212f2dc, 0x0212f33c, 0x0212f460, 0x0212f588, 0x0212f630, 0x0212f6d8, 0x0212fa7c, 0x0212fc10) |
| `config/tu_manifest.d/ov084/daPkn_c.json` | the keys below |
| `include/daPkn_c.h` | field names and types, the nine state declarations, operator new and `_ZTV7daPkn_c` removed, header comment tidied |
| `notes/agents/handoffs/readable-ov084-pkn-0925.md` | this handoff |
| `src/actors/daPkn_c.cpp` | the source changes above |

Changed keys in `config/tu_manifest.d/ov084/daPkn_c.json`, from a python parse of both revisions: `functions[9..17].symbol` (the nine renames), `notes[1]` to `notes[25]` ("states" to "then stated"), `notes[27]` added (the rename map and what this pass changed), and `compiler_only_output[0].reason` (the Vector3 users by their current names). `verification.compilerOnlyOutput.droppedSections[17]` went from 38 to 35: `tools/tubuild.py verify` rewrites that list from the object it compiles, and the value is the section index (`st_shndx`) of the dropped `_ZN7Vector3D1Ev` .text, which moved from 38 at base to 35 at this commit because the emission order changed. Both objects have 67 sections. `notes[0]`, `notes[26]`, `boundary_evidence`, `sections`, `verification` and the other `compiler_only_output` entries are unchanged; each was reread against this commit.

## Proof

All commands were rerun in round 2 in `C:/tmp/claude-rd-ov084` on the source of input `6aaaf287e3`, which this commit does not change; every result matches round 1.

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov084/daPkn_c` | 0 | 24/24 MATCH; objisolate clean; relocation destinations clean |
| Object comparison with pyelftools, the object `tubuild verify` builds from base `ace15a6c62` against the one it builds from this commit, under the nine-row rename map | 0 | 67 sections in each object; 25/25 function bodies identical (25 functions in 25 .text sections each: the 24 ROM functions plus `_ZN7Vector3D1Ev`); 26/26 relocation sections identical (20 on .text, 6 on .data); undefined-symbol set identical (95 symbols); defined global set (36 symbols) differs only by the nine state renames |
| `python tools/rombuild.py -j16` (full, with ROM) | 0 | 7122 enrolled sources, 0 reused and 7122 compiled (`build/src/actors/daPkn_c.o` rebuilt in this run); 11,213 of 11,213 source-built functions reproducing, 0 mismatching; module fidelity 106/106 exact; intact TU gates PASS with zero new symbol errors; `intactTuRom.identical` true |
| `python tools/prepush_linkcheck.py --range ace15a6c62..HEAD` | 0 | 24 checked, 24 verified, 0 warnings, 0 blocking (all nine renamed states VERIFIED) |
| `python tools/check_src_tu_compiles.py` | 0 | 297/297 translation units compile |
| `python tools/check_decl_agreement.py --changed ace15a6c62` | 0 | no new disagreements, no new local redeclarations |
| `python tools/check_dead_references.py` | 0 | no new dead references |
| `python tools/tiers_ratchet.py --check` | 0 | PASS (baseline 2994, current 3143) |
| `python tools/check_tubuild_conflicts.py` | 0 | 297 entries OK |
| `python tools/queue_audit.py --check-promoted` | 0 | OK |
| `python tools/port_refcheck.py` | 0 | 408 references, all resolve |
| `python tools/prepush_attribution.py --base ace15a6c62 --head HEAD` | 0 | 7295 tracked, 0 changed, 0 lost |
| `python tools/affected_src.py include/daPkn_c.h` | 0 | `src/actors/daPkn_c.cpp` only |
| `git diff ace15a6c62 -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` | 0 | empty |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify and prepush_linkcheck, above.
- Complete emitted TU and data/metadata checks: the object comparison above covers every function, relocation and symbol of the TU object. This TU licenses .text only; the vtable, RTTI and romdata checks from the promotion were not rerun, and the class's data and virtual members are unchanged.
- Shared-header consumer expansion: `python tools/affected_src.py include/daPkn_c.h` lists only `src/actors/daPkn_c.cpp`. No other header changed.
- Port/path/reference and other applicable static gates: above.
- Private validation, if run, and the exact PR head/base it tested: not run.
