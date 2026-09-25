# Handoff: readable-ov092-onms-0925b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3161, task `readable-ov092-onms-0925b`, stage `revise`, role producer, session `claude-prod-readable-ov092-onms-0925b`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov092-onms-0925b`, input `3fa23a549903672f45d2d37433b289ec3126a720` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `3fa23a549903672f45d2d37433b289ec3126a720`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments below are prose records of the source change tried and the measured result; no experiment artifact is committed.
- Next action, responsible role and blockers: independent verification of this commit (byte, relocation, whole-object and source review). No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov092/daOnms_c`, `src/actors/daOnms_c.cpp`, ov092 .text 0x02130f00..0x02132018 (22 functions). Promoted in #3125; the issue had no recorded readability review, so this pass starts from the self-audit below.
- Reserved source/header/config surfaces actually touched: `src/actors/daOnms_c.cpp`, `include/daOnms_c.h`, `config/arm9/overlays/ov092/symbols.txt` (16 rows renamed in place), `config/tu_manifest.d/ov092/daOnms_c.json`, this handoff, and this class's 13 keys in the `attribution.json` ledger. `config/arm9/overlays/ov092/delinks.txt` is reserved but unchanged: it holds only address ranges.
- ROM observations, read from `extracted/overlays/overlay_0092.bin` and the ov092 config:
  - The nine member-pointer records at 0x02132240..0x02132288 reach the state bodies by address (ov092 relocs.txt rows from 0x02132240 to 0x02132280). `src/__sinit_ov092_021320cc.c` copies them into the table at 0x02132568 in state order 0 to 8: 0x021315ac, 0x02131578, 0x021316b0, 0x02131680, 0x02131650, 0x02131620, 0x021311b0, 0x02131010, 0x02130fcc.
  - 0x02132074..0x021320cc is .rodata (ov092 delinks.txt). It holds three s32 at 0x02132074 (0x7c000, 0x7c000, 0x8c000), three s32 at 0x02132080 (0x3c000, 0x3c000, 0xc000) and 64 s8 at 0x0213208c, values in -3..2.
- Lineage evidence or structural inference:
  - The state bodies take the object in r0 and are reached only through the member-pointer table, so they are written as members. The helpers `NextMove`, `Roll`, `Launch`, `CheckPlayerHit` and `UpdateModelMtx` are called only from this TU with the object in r0.
  - Field meanings come from their uses. 0x576 is set to 1 at the top of Behavior and to 0 by the wait state; UpdateModelMtx rotates the model matrix only while it is set (`mTumbling`). 0x577 is set to 0 in Behavior and to 1 by Roll on the last frame of a roll; Behavior then bakes the rotation into mBaseMtx (`mRollDone`). The three s16 at 0x4e0 are added to mAngleX, mAngleY and mAngleZ each frame of the knocked and bounce states, and Launch sets them from the sine table (`mTumbleVelX`, `mTumbleVelY`, `mTumbleVelZ`).
  - The state order follows mMoveDir's values: func_ov092_021313b0 picks 2, 3, 4 or 5 by path heading, and Roll steps Z by +0x3e800 in state 2, Z by -0x3e800 in state 3, X by -0x3e800 in state 4 and X by +0x3e800 in state 5.
- Hypothesized names/filenames, explicitly not recovered facts: every new member name is coined, and `include/daOnms_c.h` marks each one "coined" next to its declaration. The members are `StateLand` (0x021315ac), `StateRollPosZ` (0x021316b0), `StateRollNegZ` (0x02131680), `StateRollNegX` (0x02131650), `StateRollPosX` (0x02131620), `StateKnocked` (0x021311b0), `StateBounce` (0x02131010), `StateSink` (0x02130fcc), `NextMove` (0x021314d0), `Roll` (0x021316d8), `Launch` (0x02131878), `CheckPlayerHit` (0x021319b0) and `UpdateModelMtx` (0x02131aec). The static members are `sLaunchVertSpeed`, `sLaunchHorzSpeed` and `sBankAxis`, and the fields are `mTumbling`, `mRollDone` and `mTumbleVelX`, `mTumbleVelY` and `mTumbleVelZ`.
- Compiler experiments and measured barriers: see the table below. Each was run with `python tools/tubuild.py verify ov092/daOnms_c` on this task's working tree. Adopted changes were also checked with a pyelftools comparison of the TU object against the base object (see Proof). Each experiment was then kept or reverted as the Result column says.

| ID | Where | Change tried | Result |
|---|---|---|---|
| E1 | Whole TU | Thirteen functions as members; named fields in every body; real calls for MarkForDestruction, ClosestPlayer, UpdatePos, IsOnGround, IsOnWall, TriplePoofDust, LandingDustAt, FindWithID, BumpedUnderneathByPlayer, Sound::PlayBank3, PathPtr::FromID, dBgW_KcMbg::Transform; `&dBgW::UpdatePosWithTransform` for the address argument | 22/22. Adopted. |
| E2 | Earthquake calls | Bridge declared `(dActor_c *, const Vector3 *, int)`, the call casts dropped | 22/22, but `check_decl_agreement` fails: arity 3 against the definition's 2 in `src/_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE.cpp`. Reverted. |
| E3 | Veneer calls | `dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *)`, cast dropped | 22/22, but `check_decl_agreement` fails: arity 1 against `(void)` in `src/dBgCh_Actr_UpdateContinuous_Veneer.c`. Reverted. |
| E4 | InitResources | `Vec3_Asr(Vector3 *, const Vector3 *, int)` and a Vector3 temporary | 22/22, but `check_decl_agreement` fails against `src/Vec3_Asr.c`'s own `struct Vec3`. Reverted. |
| E5 | InitResources | `mdCcAcPos_c.Init(this, offset, radius, height, 2, 0x6003c0)` with `Fix12<int>` locals | DIFF, 2 relocation destinations wrong. Reverted. |
| E6 | InitResources | `mMeshCollider.SetFile(file, mClsnMat, scale, mAngleY, clps)` with a `Fix12<int>` local | DIFF, 3 relocation destinations wrong. Reverted. |
| E7 | InitResources | `int stk[3]` replaced by `Vector3 offset` for the bridge | 22/22. Adopted. |
| E8 | StateKnocked | `volatile int saved[3]` replaced by `Vector3 saved` | 22/22. Adopted. |
| E9 | StateKnocked | Player position read as `o->mPosX` and so on, with no `int *p` | DIFF. Reverted; one comment. |
| E10 | StateBounce | The escaped `struct Frame { Vector3 tmp, eq, dust; }` replaced by three plain Vector3 locals | 22/22, and the object no longer contains the local `Frame` destructor. Adopted. |
| E11 | StateBounce | `if (mVertAccel == 0) return 0;` | DIFF. Reverted; one comment. |
| E12 | StateBounce | `if (mWithMeshClsn.IsOnGround() == 0) return 0;` | DIFF. Reverted; covered by the E11 comment. |
| E13 | StateBounce | x, y, z temporaries replaced by `tmp` set member by member, `tmp.y -= 0x78000`, `dust = tmp` | 22/22. Adopted. |
| E14 | StateBounce | E13 with `dust` built directly and no `tmp` | DIFF. Reverted; one comment. |
| E15 | CheckPlayerHit | `if (o->actorID != 0xbf) return;` in place of the int flag | DIFF. Reverted; one comment. |
| E16 | StateSink | `(long long)(int)` launder dropped, plain `mPosY -= 0x5000` and one compare | 22/22. Adopted. |
| E17 | StateBounce, StateKnocked, Roll, NextMove, Launch, InitResources | Pointer temporaries (`p504`, `p8c`, `px`, `ip`, `bits`, `flags`, `tbl`) replaced by compound assignments on the members; `short lr` and `int z` zero temporaries dropped | 22/22. Adopted. |
| E18 | NextMove, Roll, func_ov092_021313b0, InitResources | `mMoveSeqIndex++`, one-line rest height, `old = self->mPathNode`, `mMoveKind = param1 & 3` without the `idx` round trip, `(s16)` casts dropped from the Roll arguments | 22/22. Adopted. |
| E19 | Launch, Roll | `data_ov092_02132074`, `02132080`, `0213208c` as `static const` members of daOnms_c | 22/22. Undefined names change exactly per the three symbols.txt renames. Adopted. |

## Findings

Self-audit of the base, numbered R1 to R24. The issue lists no numbered findings; its six target bullets map onto these rows. Deferred rows name the owner as the next producer the coordinator assigns on #3161 (https://github.com/tangosdev/sm64ds-decomp/issues/3161).

| ID | Category | Finding at base | Outcome | Reason or evidence |
|---|---|---|---|---|
| R1 | extern "C" state bodies | The eight state bodies `func_ov092_02130fcc`, `02131010`, `021311b0`, `021315ac`, `02131620`, `02131650`, `02131680`, `021316b0` are C-linkage free functions on `char *` | Fixed | E1. Members under coined names; symbols.txt rows, manifest symbol fields and attribution keys renamed in place. The .data records reach them by address. |
| R2 | extern "C" helpers | `func_ov092_021314d0`, `021316d8`, `02131878`, `021319b0`, `02131aec` are C-linkage | Fixed | E1. `NextMove`, `Roll`, `Launch`, `CheckPlayerHit`, `UpdateModelMtx`. |
| R3 | extern "C" helpers and state | `func_ov092_021313b0`, `02131578` (state 1) and `02131a88` are C-linkage | Partially fixed | Their bodies now read named fields through a typed `daOnms_c *self`. They keep C names because `include/decl_common.h` declares all three by name and is off limits. Owner: next producer on #3161. |
| R4 | mangled bridges | C-linkage `_ZN...` bridges for the E1 callees | Fixed | E1. The undefined-symbol set is unchanged. |
| R5 | mangled bridges | `dCcAcPos_c::Init` and `dBgW_KcMbg::SetFile`, both taking `Fix12<int>` by value | Deferred: measured, no committed artifact | E5, E6. |
| R6 | mangled bridges | `dActor_c::Earthquake`, `dBgCh_Actr::GetFloorResult`, `Particle::System::NewSimple`, `dBgActor_c::IsClsnInRange`, `dBgCh_Actr::Init` | Still deferred | `include/dActor_c.h`, `include/dBgActor_c.h` and `include/dBgCh_Actr.h` declare none of them in the form the ROM symbol mangles to (`dBgCh_Actr::Init` is declared with `Fix12i`, which mangles to `i`). All three are shared headers outside this reservation. Not measured. Owner: next producer on #3161. |
| R7 | casts | Function-pointer casts on the Earthquake bridge and the UpdateContinuous veneer | Deferred: measured, no committed artifact | E2, E3. The definitions in `src/_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE.cpp` and `src/dBgCh_Actr_UpdateContinuous_Veneer.c` declare `(void *, int)` and `(void)`; both files are outside this reservation. |
| R8 | unk_ fields | `unk_576`, `unk_577` | Fixed | `mTumbling`, `mRollDone`, coined; see Lineage. |
| R9 | raw offsets | `pad_4e0[8]` read and written as `((s16 *)(thiz + 0x400))[0xe0 / 2]` and the next two | Fixed | `mTumbleVelX`, `mTumbleVelY`, `mTumbleVelZ` and `pad_4e6[2]`. |
| R10 | raw offsets | Thirteen bodies written in `c + 0x...` offsets: collision flags at 0x500, 0x504, 0x508 and 0x50c, radius at 0x4ec, dActor_c fields, mModel's matrix at 0xf0 and 0x114, mClsnMat at 0x2ec and 0x310 | Fixed | E1, E17. No `this` offset remains; dCc_c's `flags`, `vulnFlags`, `hitFlags`, `otherOwner` and `radius` are used by name. |
| R11 | casts | `(Vector3 *)&mPosX`, `&mCamSpacePosX` and `&player->mPosX` over dActor_c scalars | Still deferred | `include/dActor_c.h` declares them as scalars; shared header outside this reservation. Owner: next producer on #3161. |
| R12 | casts | File-local `Vec3` typedef and the `(Vec3 *)&mPosX` cast in InitResources | Deferred: measured, no committed artifact | E4. The declaration must stay in agreement with `src/Vec3_Asr.c`. |
| R13 | launders | `volatile int saved[3]` in StateKnocked; escaped `Frame` struct in StateBounce; `(long long)(int)` launder in StateSink; the identity `LA()` macro | Fixed | E8, E10, E16, E17. `LA()` is gone. |
| R14 | temporaries | Pointer temporaries, `short lr`, `int z`, `idx`, x, y, z temporaries | Fixed | E13, E17, E18. |
| R15 | temporaries | The `int v` and `int g` early returns in StateBounce, the dead `tmp` copy there, the `int b` flag in CheckPlayerHit and `int *p` in StateKnocked | Deferred: measured, no committed artifact | E9, E11, E12, E14, E15. One comment each. |
| R16 | goto | None at base | Not applicable | |
| R17 | dead or local declarations | `struct MMC`, `struct Obj`, `Mtx43`, the second copies of `data_020a0e68` and the three matrix prototypes, local prototypes of `func_ov092_021316d8` and `021319b0`, the `ToxBoxState` typedef names, the header's dead C shadow struct (`#else` branch, no C consumer: `python tools/affected_src.py include/daOnms_c.h` lists only `src/actors/daOnms_c.cpp` and `src/d_a_onms.cpp`) | Fixed | Removed, or renamed after daOnms_c. The shadow struct also carried stale prose: it said a 0 entry ends a move sequence, where the body tests for -1. |
| R18 | local declarations | `Vec3_HorzAngle`, `Vec3_HorzDist`, `func_02012694`, the matrix helpers, `func_02037e38`, `func_02037e84`, `func_0203568c`, `func_020393d4`, `func_ov002_020de328` and `data_02082214` are declared locally | Still deferred | No header this TU includes declares them; `check_decl_agreement` reports no new local redeclaration. Owner: next producer on #3161. |
| R19 | address-named data | `data_ov092_02132074`, `02132080`, `0213208c` | Fixed | E19. Static const members, rows renamed in place. |
| R20 | address-named data | `data_ov092_02132540`, `02132548` and `02132568`; `data_ov092_02132220` and `02132294` | Still deferred | The first three are also named by `src/__sinit_ov092_021320cc.c`, outside this reservation. `include/decl_common.h` declares the last two. Owner: next producer on #3161. |
| R21 | address-named data | `data_02082214`, `data_020a0e68` | Still deferred | arm9 rows, not ov092's. |
| R22 | unmeasured codegen notes | The `func_ov092_02131010` note ("escaped stack struct", `mwccarm 1.2/sp2p3`), the "recovered: shared common types" banners, the file-header claim about twelve-word matrix copies | Fixed | E10 shows the struct is not needed; the notes are gone and each kept form carries one measured comment. |
| R23 | stale manifest notes | The `Frame` compiler-only entry named by source line number; the `_ZN7Vector3D1Ev` reason citing old function names; no note mapping the renames | Fixed | See Changed paths. |
| R24 | stale notes and ledgers | `notes/butterfly-tornado-provenance.md` rows cite the old function names and the removed C field names (`mRestPosX`, `mPathNodeX`); `notes/mwccarm-codegen.md` cites `func_ov092_02131010`, `021311b0`, `021316d8`; `config/match_attempts.jsonl` and `CLAIMS.md` carry ov092 rows | Still deferred | Not in this task's reservation; the ledgers are append logs keyed by address. Owner: next producer on #3161. |

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 22/22 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: 19 of the 22 functions are daOnms_c members: D1, D0, InitResources, CleanupResources, Behavior, Render, the eight states and the five helpers. The other 3 are C-linkage: `func_ov092_021313b0`, `02131578` and `02131a88` (R3). The factory `daOnms_c_classInit` is outside this TU, in `src/d_a_onms.cpp`. Seven mangled callee bridges remain (R5, R6).
- Recovered layout/fields; remaining shadow structs/raw offsets: every daOnms_c field is named. No raw `this` offset and no shadow struct remain. The Vector3 casts over dActor_c scalars remain (R11).
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The new members are non-virtual and the new statics are declarations only, so layout (0x594), vtable and RTTI are unchanged. The TU stays text-only.
- Attribution preserved through each move/rename: the 13 `attribution.json` keys follow the renamed symbols with the same authors (`ruspecial` keeps StateBounce, `lunavyqo` keeps Launch, `tangosdev` the other eleven). The data rows carry no attribution key.
- Remaining agreed issue scope: R3, R5 to R7, R11, R12, R15, R18, R20, R21, R24.

Residue. The `lines` column is `wc -l` of `git show <rev>:<path>` (newline count; both revisions end in a newline). Each token column is the number of lines containing the token, `grep -c -F`, run with `MSYS_NO_PATHCONV=1`.

| Path | Revision | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|---|
| `src/actors/daOnms_c.cpp` | base `3fa23a5499` | 683 | 3 | 66 | 17 | 55 |
| `src/actors/daOnms_c.cpp` | this commit | 573 | 0 | 27 | 4 | 36 |
| `include/daOnms_c.h` | base `3fa23a5499` | 185 | 3 | 14 | 0 | 4 |
| `include/daOnms_c.h` | this commit | 97 | 0 | 3 | 0 | 0 |

Of the 36 `_ZN` lines in the source, 19 are `// @symbol` marker lines for members. The 4 `extern "C"` lines are the declaration block and the three R3 definitions.

## Changed paths

`git diff --name-only 3fa23a5499..HEAD` lists six paths:

| Path | Change |
|---|---|
| `attribution.json` | 13 keys renamed with their symbols, authors unchanged |
| `config/arm9/overlays/ov092/symbols.txt` | 16 rows renamed in place: the 13 functions at 0x02130fcc, 0x02131010, 0x021311b0, 0x021314d0, 0x021315ac, 0x02131620, 0x02131650, 0x02131680, 0x021316b0, 0x021316d8, 0x02131878, 0x021319b0, 0x02131aec and the data rows at 0x02132074, 0x02132080, 0x0213208c |
| `config/tu_manifest.d/ov092/daOnms_c.json` | the keys below |
| `include/daOnms_c.h` | named fields, member and static declarations, C shadow struct removed, header comment tidied |
| `notes/agents/handoffs/readable-ov092-onms-0925b.md` | this handoff |
| `src/actors/daOnms_c.cpp` | the source changes above |

Changed keys in `config/tu_manifest.d/ov092/daOnms_c.json`, from a python parse of both revisions:

- `functions[2,3,4,6,8..15,17].symbol`: the 13 renames.
- `notes[2]` added: the rename map and why three functions keep C names.
- The `compiler_only_output` entry for the local `Frame` destructor is removed. At base it was named `_ZN21Frame$518daOnms_c_cppD1Ev`; the compiler no longer emits it (E10). The `_ZN7Vector3D1Ev` reason now names the current functions.
- `verification.compilerOnlyOutput` (`requested`, `deadstripped`, `droppedSections`) is rewritten by `tools/tubuild.py verify` from the object it compiles. The Frame destructor has left `requested` and `deadstripped`. `droppedSections` lost 18 and 68 and gained 51: section indices (`st_shndx`) move because the object now has 68 sections where it had 69.
- `notes[0]`, `notes[1]`, `boundary_evidence`, `sections` and the other `compiler_only_output` entries are unchanged, and each was reread against this commit.

No `Frame$NNN` symbol remains in the object, so the line-number trap on this TU is gone.

## Proof

All commands ran in `C:/tmp/claude-rd-ov092` on the source of this commit. This handoff is the only file this commit adds after the source commits they tested.

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov092/daOnms_c` | 0 | 22/22 MATCH; objisolate clean; relocation destinations clean; ROM-ascending order |
| Object comparison with pyelftools, `build/tu/ov092-daOnms_c/daOnms_c.o` from base `3fa23a5499` against the one `tubuild verify` builds from this commit, under the 16-row rename map | 0 | 24/24 named function bodies identical: the 22 ROM functions plus the compiler-only D2 and `_ZN7Vector3D1Ev`. 24/24 per-function relocation sets identical by symbol name, and 163 relocations in 28 relocation sections in each object. The undefined-symbol set is identical, 92 symbols, apart from the three data renames. The defined global set, 24 symbols, differs only by the 13 function renames. The base object's local `_ZN21Frame$518daOnms_c_cppD1Ev` (four-byte `bx lr`, deadstripped) is absent at this commit, which is why the object has 68 sections where the base had 69. |
| `python tools/rombuild.py -j16` (full, with ROM) | 0 | 7100 enrolled sources, 7098 reused and 2 compiled (`src/actors/daOnms_c.cpp` and `src/d_a_onms.cpp`, the two consumers of the header); 11,214 of 11,214 source-built functions reproducing, 0 mismatching; module fidelity 106/106 exact; intact TU gates PASS with zero new symbol errors; `intactTuRom.identical` true. Its romData report lists 3 differing data symbols, `_ZTV10dCcAcPos_c` and two daDemo_c typeinfos, none in ov092 or in files this commit changes. |
| `python tools/prepush_linkcheck.py --range 3fa23a5499..HEAD` | 0 | 23 checked, 23 verified, 0 warnings, 0 blocking (every daOnms_c function plus `daOnms_c_classInit`) |
| `python tools/prepush_attribution.py --base 3fa23a5499 --head HEAD` | 0 | 7272 tracked, 0 changed, 0 lost |
| `python tools/check_decl_agreement.py --changed 3fa23a5499` | 0 | no new disagreements, no new local redeclarations |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check` | 0 | PASS (baseline 2998, current 3185) |
| `python tools/check_tubuild_conflicts.py` | 0 | 299 entries OK |
| `python tools/queue_audit.py --check-promoted` | 0 | every row agrees with the manifests |
| `python tools/check_src_tu_compiles.py` | 0 | 299/299 translation units compile |
| `python tools/port_refcheck.py` | 0 | 408 references, all resolve |
| `python tools/affected_src.py include/daOnms_c.h` | 0 | `src/actors/daOnms_c.cpp`, `src/d_a_onms.cpp` |
| `git diff 3fa23a5499..HEAD -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` | 0 | empty |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify and prepush_linkcheck, above.
- Complete emitted TU and data/metadata checks: the object comparison covers every function, relocation and symbol of the TU object. This TU licenses .text only; the vtable, RTTI and data are ROM bytes, and the romdata check from the promotion was not rerun.
- Shared-header consumer expansion: only `include/daOnms_c.h` changed; its two consumers compiled in the rombuild run and pass linkcheck.
- Port/path/reference and other applicable static gates: above.
- Private validation, if run, and the exact PR head/base it tested: not run.
