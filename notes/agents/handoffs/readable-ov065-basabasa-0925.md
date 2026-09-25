# Handoff: readable-ov065-basabasa-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3152, task `readable-ov065-basabasa-0925`, stage `revise`, role producer, session `claude-prod-readable-ov065-basabasa-0925`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov065-basabasa-0925`, input `c31f43bacfde4eb8e184b75077f85f4ba45a72a9` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `c31f43bacfde4eb8e184b75077f85f4ba45a72a9`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments below are prose records (the source change tried and the measured result); no experiment artifact is committed.
- Next action, responsible role and blockers: independent verification (byte, relocation, whole-object and source review) of this commit. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov065/daBasabasa_c`, `src/actors/daBasabasa_c.cpp`, ov065 .text 0x02116f98..0x02117ee8 (21 functions). Promoted in #3124; this is its first readability pass, so the finding list below is a self-audit of the base.
- Reserved source/header/config surfaces actually touched: `src/actors/daBasabasa_c.cpp`, `include/daBasabasa_c.h`, `config/arm9/overlays/ov065/symbols.txt` (nine rows renamed in place), `config/tu_manifest.d/ov065/daBasabasa_c.json`, this handoff, and this class's rows in two integration-lane ledgers: nine `attribution.json` keys and one added key in `config/decl-agreement-baseline.json` (see R19). `config/arm9/overlays/ov065/delinks.txt` is reserved but unchanged: it holds only address ranges.
- ROM observations: ov065 relocs.txt rows `from:0x0211cc20` to `from:0x0211cc58` put the eight state bodies in .data as four member-pointer pairs: 0x0211cc30 0x021178fc and 0x0211cc50 0x02117888; 0x0211cc20 0x021177e4 and 0x0211cc40 0x02117780; 0x0211cc28 0x021176fc and 0x0211cc38 0x02117624; 0x0211cc58 0x021175b0 and 0x0211cc48 0x02117404. `__sinit_ov065_0211c2a8` copies them into the .bss records 0x0211d700, 0x0211d710, 0x0211d6e0 and 0x0211d6f0 in that order. `daBasabasa_c_classInit` passes 1088 (0x440) to `fBase_c::operator new`, which is the size the header asserts.
- Lineage evidence or structural inference: the function at 0x02117944 stores its argument at 0x420 and calls the first member pointer of the record through it, so it enters a state and the first pointer of each record is `enter`; Behavior calls the second one every frame (`execute`). The .data records reach the bodies by address, so the rename does not touch them. Field meanings come from their uses: 0x43c selects which ModelAnim is drawn, advanced and placed, and only the 0x300 one plays the wingbeat frames (flying); 0x434 is the yaw target handed to ApproachAngle and ApproachLinear; 0x430 counts frames spent near home and triggers the return to perch at 0x64; 0x3f0 is the matrix built for DropShadowRadHeight (0x30 bytes, a Matrix4x3).
- Hypothesized names/filenames, explicitly not recovered facts: `SetState` (0x02117944), `ExecuteReturn` (0x02117404), `EnterReturn` (0x021175b0), `ExecuteChase` (0x02117624), `EnterChase` (0x021176fc), `ExecuteDrop` (0x02117780), `EnterDrop` (0x021177e4), `ExecutePerch` (0x02117888), `EnterPerch` (0x021178fc), the `State` members `enter` and `execute`, and the fields `mShadowMatrix`, `mPerchTimer`, `mTargetAngleY` and `mIsFlying` are coined. `include/daBasabasa_c.h` marks them as coined next to the declarations. The labels on the shared files and state records in the source comments are inferred from their uses.
- Compiler experiments and measured barriers: each row was run with `python tools/tubuild.py verify ov065/daBasabasa_c` on this task's working tree and kept or reverted as the Result column says.

| ID | Where | Change tried | Result |
|---|---|---|---|
| E0 | Whole TU | States and SetState as members, State typed as two member pointers, named fields in every body, the twelve bridges of R4 as real calls, the gotos of `func_ov065_0211704c` and ExecuteReturn replaced by or-conditions | 21/21, undefined-symbol set unchanged. Adopted. |
| E1 | `func_ov065_0211704c` | `if (p->actorID != 0xbf)` in place of the int `isPlayer` | DIFF, 3 relocation destinations wrong |
| E2 | Render | `if (mFlags & 0x40000)` in place of the int `flag` | DIFF |
| E3 | ExecuteReturn | `mStateTimer < 0x14` without the `(u16)` | 3 words differ |
| E4 | ExecuteReturn | `mStateTimer == 0` without the `(u16)` | 1 word differs |
| E5 | ExecuteChase | same | 1 word differs |
| E6 | ExecuteDrop | same | 1 word differs |
| E7 | ExecuteReturn | `mPerchTimer++` | 21/21. Adopted. |
| E8 | ExecuteReturn | compound or-assignment into `mFlags` | 21/21. Adopted. |
| E9 | ExecuteChase | `Vector3 v` in place of `int v[3]` | 21/21. Adopted. |
| E10 | ExecutePerch | `v.x = p->mPosX` and so on, without the base pointer | DIFF |
| E11 | EnterReturn | Plain two-argument `func_02012694` call in place of the four-argument cast | 21/21. Adopted. |
| E12 | Behavior | Drop the reload and store-back of `unk_0ac` | DIFF, 2 relocation destinations wrong |
| E13 | ExecuteReturn | `(mFlags & 8)` without `!= 0` | 21/21. Adopted. |
| E15 | EnterDrop | A `Vector3` local in place of `PoofPos` | 12 words differ |
| E16 | Behavior | Compare `mCurrentState` directly, no `State *` local | 21/21. Adopted. |
| E17 | `func_ov065_0211704c` | `Vector3 pos` for the coin spawn in place of `int pos[3]` | 21/21. Adopted. |
| E18 | `func_ov065_0211704c` | `Vector3 pos` for Player::Hurt | 21/21. Adopted. |
| E19 | ExecuteReturn | `-0x8000` without the `(short)` cast | 21/21. Adopted. |
| E20 | `func_ov065_0211704c` | `static_cast` for the FindWithID result | 21/21; not kept, the C-style cast matches the rest of the file |
| E21 | ExecuteReturn | `v.x = v.y = v.z = 0` | 2 words differ |
| E22 | EnterChase | No `(short)` cast on the random yaw | 21/21. Adopted. |
| E23 | ExecuteChase | One statement per line for the zeroing | 21/21. Adopted. |

E1, E2, E3 to E6 (one comment at the first use), E10 and E15 leave a codegen-forced form with one short comment. E12's comment already said the reload is the ROM's. E21 needs none: the kept form is plain code.

## Findings

Self-audit of the base, numbered R1 to R19. Issue #3152 lists target categories rather than numbered findings; each maps onto the rows below. Deferred rows name the owner as the next producer the coordinator assigns on #3152 (https://github.com/tangosdev/sm64ds-decomp/issues/3152).

| ID | Category | Finding at base | Outcome | Reason or evidence |
|---|---|---|---|---|
| R1 | extern "C" state bodies | The eight state bodies (`func_ov065_02117404`, `021175b0`, `02117624`, `021176fc`, `02117780`, `021177e4`, `02117888`, `021178fc`) are C-linkage free functions on `char *` or `int *` | Fixed | Members under coined names; symbols.txt rows, manifest symbol fields and attribution keys renamed in place. The .data records reach them by address (ROM observations). |
| R2 | extern "C" helpers, shadow types | `func_ov065_02117944` takes a local `struct C { char pad[0x420]; PMF *pp; }` and a `PMF` typedef; callers cast `(C *)` and `(PMF *)` | Fixed | `daBasabasa_c::SetState(State *)`; `C` and `PMF` removed; `State` is now two typed member pointers. |
| R3 | extern "C" helpers | `func_ov065_0211704c` and `func_ov065_02117994` keep C linkage and work in raw offsets | Partially fixed | Bodies use named fields through one typed `self` pointer, and 0211704c's gotos are gone (E0). Linkage and name stay: `include/decl_common.h` declares both as `(char *)` and is off limits. Owner: next producer on #3152. |
| R4 | mangled bridges | C-linkage declarations of `dActor_c::FindWithID`, `BumpedUnderneathByPlayer`, `SmallPoofDust`, `KillAndTrackInDeathTable`, `JumpedOnByPlayer`, `ClosestPlayer`, `ClosestNonVanishPlayer`, `HorzAngleToCPlayer`, `Player::IsOnShell`, `dBgCh_Actr::IsOnWall`, `dEnemyBase_c::UpdateYoshiEat` and `_Z14ApproachLinearRsss` | Fixed | Real calls through the declaring headers, and `ApproachLinear(short &, short, short)` declared in its C++ form. Undefined-symbol set of the TU object unchanged (Proof). |
| R5 | mangled bridges | `ModelAnim::SetAnim`, `dCcAc_c::Init`, `dBgCh_Actr::Init`, `dEnemyBase_c::KillByInvincibleChar`, `dActor_c::SpawnCoins`, `dActor_c::DropShadowRadHeight`, `Player::Hurt`, `Player::Bounce` | Partially fixed | Their object parameters are now typed (`ModelAnim *`, `dEnemyBase_c *`, `dActor_c *`, `Player *`), removing the `char *` casts. The bridges stay: each symbol carries `Fix12<int>` by value, `include/dBgCh_Actr.h` declares Init with `Fix12i` (mangles as int), and `include/Player.h` declares neither Hurt nor Bounce; shared headers outside this reservation. Not remeasured. Owner: next producer on #3152. |
| R6 | unk_ fields | `unk_43c`, `pad_3f0[0x30]` and `pad_430[0xc]` hide used fields | Fixed | `mIsFlying` (0x43c), `mShadowMatrix` (0x3f0), `mPerchTimer` (0x430), `mTargetAngleY` (0x434), coined; `pad_436` covers the unread bytes. |
| R7 | unk_ fields | Inherited `unk_0a4`, `unk_0ac` (dActor_c), `unk_104`, `unk_108`, `unk_10a` (dEnemyBase_c) | Still deferred | Declared in `include/dActor_c.h` and `include/dEnemyBase_c.h`, shared headers outside this reservation. `unk_0a4` and `unk_0ac` are newly visible because raw `+ 0xa4` and `+ 0xac` accesses became member accesses. Owner: next producer on #3152. |
| R8 | raw offsets | Every state body and both C helpers address `this` by raw offset, including `c + 0x400 + 0x34` and `t[0xf0]` | Fixed | E0. No raw offset from `this` or a Player remains in the TU. |
| R9 | casts | `(Vector3 *)&mPosX` over dActor_c scalars, `(unsigned short *)&mStateTimer` for DecIfAbove0_Short, `(u16)mStateTimer` reads | Still deferred | dActor_c declares scalar positions and dEnemyBase_c declares `s16 mStateTimer`; shared headers outside this reservation. The `(u16)` reads are measured (E3 to E6). Owner: next producer on #3152. |
| R10 | casts | `((int *)&data_ov065_0211d690)[1]` for the loaded file, six times | Partially fixed | One `LOADED_FILE` macro with a one-line comment. `include/SharedFilePtr.h` declares no fields on purpose; a shared header outside this reservation. |
| R11 | goto | `goto kill` and `goto jumped` in `func_ov065_0211704c`, `goto skip` in ExecuteReturn | Fixed | E0. |
| R12 | temporaries, launders | `int *ip1 = (int *)((int)thiz + 0x430)` and `ip2`, `int v[3]`, `int pos[3]` twice, `(short)` casts, the four-argument `func_02012694` cast, Behavior's `*(int *)((char *)q + 8)` null test and `State *` locals | Fixed | E0, E7, E8, E9, E11, E16 to E19, E22. |
| R13 | temporaries | int `isPlayer` and `flag`, ExecutePerch's base pointer, Behavior's `keep` of `unk_0ac`, EnterDrop's `PoofPos` | Deferred: measured, no committed artifact | E1, E2, E10, E12, E15. One short comment each. |
| R14 | dead and local declarations | `C`, `PMF`, the twelve R4 bridges, local prototypes of `func_ov065_02117944` and `func_ov065_021177e4`, `char []` declarations of the state records | Fixed | Removed or retyped; `check_decl_agreement` reports no new local redeclaration. |
| R15 | local declarations | `Vec3_Dist`, `Vec3_HorzAngle`, `Vec3_Asr`, `ApproachAngle`, `MulVec3Mat4x3`, the `Matrix4x3_*` helpers, `RandomIntInternal`, `DecIfAbove0_Short`, `func_02012694`, `func_ov002_020aea30`, `data_0209e650`, `data_020a0e68` | Still deferred | No header this TU includes declares them; `include/decl_common.h` does and is off limits. Owner: next producer on #3152. |
| R16 | address-named data | Shared files `data_ov065_0211d690`, `0211d698`, `0211d6a0`, `0211d6a8`; state records `0211d6e0`, `0211d6f0`, `0211d700`, `0211d710`; .data pairs `0211cc20` to `0211cc58` | Still deferred | `__sinit_ov065_0211c2a8` (another source file, outside this reservation) and `include/decl_common.h` name them; renaming the rows alone would strand both. Each now carries a use label in the source. Owner: next producer on #3152. |
| R17 | unmeasured and stale codegen notes | Behavior said UpdateYoshiEat is not declared in dEnemyBase_c.h (it is); InitResources said the two Init methods are not declared (they are, with Fix12 parameters); Behavior's note that the member-pointer null test must read the word raw (E0 disproves it); the file header's Matrix4x3::t include-order claim; a 20-line history of `func_ov065_021177e4`'s old coined name; `recovered:` tags, `// recovered name: Swoop_*` lines, a `Compiler mwccarm 1.2/sp2p3` line and the ROM-ordinal banners | Fixed | Removed or replaced by the file header's measured known-limits list. |
| R18 | stale header notes | Header comment narrated the absorbed `unk_35c` and `unk_3c0` and called the size an observed span; the State comment said only the handler at +8 is evidenced | Fixed | Size now cites the classInit literal 0x440; State documents both members and where the records come from. |
| R19 | stale manifest notes and ledgers | Manifest `_ZN7PoofPosD1Ev` reason names `func_ov065_021177e4`; no note maps the renames. `config/match_attempts.jsonl`, `config/match_provenance.jsonl`, `CLAIMS.md` and `notes/agents/handoffs/daYurei_Mucho_c-ov065.md` still use the old labels | Partially fixed | Manifest fixed (Changed paths). The ledgers are append logs or another task's handoff, outside this reservation; left to their owners. The typed `data_ov065_0211d6e0` declaration contradicts the `char []` plurality set by `include/decl_common.h`; it is banked as this file's own key in `config/decl-agreement-baseline.json` (three added lines, nothing re-sorted), because decl_common is off limits and the typed spelling is the accurate one. |

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 21/21 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: 19 member functions (D1, D0, the nine renamed functions, CleanupResources, OnPendingDestroy, Render, Behavior, InitResources, OnAimedAtWithEgg, OnTurnIntoEgg, OnYoshiTryEat) and 2 C-linkage helpers (R3). 8 mangled callee bridges remain (R5).
- Recovered layout/fields; remaining shadow structs/raw offsets: every daBasabasa_c field is named. No raw offset remains. Inherited `unk_` fields (R7), Vector3 casts over scalars (R9) and the SharedFilePtr second-word view (R10) remain.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The new members are non-virtual, so layout and vtable are unchanged.
- Attribution preserved through each move/rename: the nine `attribution.json` keys follow the renamed symbols with the same authors (`tangosdev` for seven, `ruspecial` for ExecuteDrop, `andrewboudreau` for EnterDrop).
- Remaining agreed issue scope: R3, R5, R7, R9, R10, R13, R15, R16, R19 (ledgers).

Residue. The `lines` column is `wc -l` of `git show <rev>:<path>` (newline count; both revisions end in a newline). Each token column is the number of lines containing the token, `grep -c -F`, with `MSYS_NO_PATHCONV=1`.

| Path | Revision | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|---|
| `src/actors/daBasabasa_c.cpp` | base `c31f43bacf` | 687 | 11 | 68 | 11 | 64 |
| `src/actors/daBasabasa_c.cpp` | this commit | 512 | 11 | 28 | 3 | 41 |
| `include/daBasabasa_c.h` | base `c31f43bacf` | 85 | 2 | 0 | 0 | 1 |
| `include/daBasabasa_c.h` | this commit | 92 | 0 | 0 | 0 | 1 |

The 11 `unk_` lines left in the source are all inherited fields (R7), one of them a comment. 19 of the 41 `_ZN` lines are the per-function symbol marker comments; the other 22 are the eight R5 bridge declarations and their 14 call lines. The one `_ZN` line in the header is the destructor symbol cited in its opening comment.

## Changed paths

`git diff --name-only c31f43bacf..HEAD` lists seven paths:

| Path | Change |
|---|---|
| `attribution.json` | nine keys renamed with their symbols, authors unchanged |
| `config/arm9/overlays/ov065/symbols.txt` | nine rows renamed in place (0x02117404, 0x021175b0, 0x02117624, 0x021176fc, 0x02117780, 0x021177e4, 0x02117888, 0x021178fc, 0x02117944) |
| `config/decl-agreement-baseline.json` | one key added under `data_ov065_0211d6e0` (R19) |
| `config/tu_manifest.d/ov065/daBasabasa_c.json` | the keys below |
| `include/daBasabasa_c.h` | State, named fields, the nine member declarations, header comment |
| `notes/agents/handoffs/readable-ov065-basabasa-0925.md` | this handoff |
| `src/actors/daBasabasa_c.cpp` | the source changes above |

Changed keys in `config/tu_manifest.d/ov065/daBasabasa_c.json`: `functions[3..11].symbol` (the nine renames), `notes[3]` added (the rename map and the two helpers kept in C linkage), `compiler_only_output[1].reason` (`_ZN7PoofPosD1Ev`: EnterDrop in place of its old label), and one entry of `verification.compilerOnlyOutput.droppedSections` (15 to 13): `tools/tubuild.py verify` rewrites that list from the object it compiles, and the section indices moved with the emission order. Both objects have 63 sections. `notes[0]` to `notes[2]`, `boundary_evidence`, `sections` and the other `compiler_only_output` entries were reread against this commit and are unchanged; `legacy_source` fields keep the historical per-function paths.

## Proof

Run in `C:/tmp/claude-rd-ov065` on the source of this commit (the source commit before it differs only by this handoff).

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov065/daBasabasa_c` (base, untouched tree) | 0 | 21/21 MATCH; objisolate clean; relocation destinations clean |
| `python tools/tubuild.py verify ov065/daBasabasa_c` | 0 | 21/21 MATCH; objisolate clean; relocation destinations clean; ROM-ascending emission order |
| Object comparison with pyelftools, the object `tubuild verify` builds from base `c31f43bacf` against the one it builds from this commit, under the nine-row rename map | 0 | 63 sections in each object; 24/24 named function bodies identical (the 21 ROM functions plus D2, `_ZN7PoofPosD1Ev` and `_ZN7Vector3D1Ev`); 23/23 relocation sections identical; undefined-symbol set identical (87 symbols); defined global set (35 symbols) identical after the nine renames |
| `python tools/rombuild.py -j16` (full, with ROM) | 0 | 7100 enrolled sources, 0 reused and 7100 compiled (`build/src/actors/daBasabasa_c.o` rebuilt in this run); 11,213 of 11,213 source-built functions reproducing, 0 mismatching; module fidelity 106/106 exact; intact TU gates PASS with zero new symbol errors; `intactTuRom.identical` true. The baseline control's `dsd check symbols` lists 9 pre-existing errors with no TU substituted. |
| `python tools/prepush_linkcheck.py --range c31f43bacf..HEAD` | 0 | 21 checked, 21 verified, 0 warnings, 0 blocking |
| `python tools/prepush_attribution.py --base c31f43bacf --head HEAD` | 0 | 7273 tracked, 0 changed, 0 lost |
| `python tools/check_decl_agreement.py --changed c31f43bacf` | 0 | no new disagreements, no new local redeclarations (after the R19 key) |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check` | 0 | PASS (baseline 2998, current 3156) |
| `python tools/check_tubuild_conflicts.py` | 0 | 299 entries OK |
| `python tools/queue_audit.py --check-promoted` | 0 | OK |
| `python tools/check_src_tu_compiles.py` | 0 | 299/299 translation units compile |
| `python tools/port_refcheck.py` | 0 | 408 references, all resolve |
| `python tools/affected_src.py include/daBasabasa_c.h` | 0 | `src/actors/daBasabasa_c.cpp` only |
| `git diff c31f43bacf -- include/decl_common.h config/dead-reference-baseline.json` | 0 | empty |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify and prepush_linkcheck, above.
- Complete emitted TU and data/metadata checks: the object comparison covers every function, relocation and symbol of the TU object. This TU licenses .text only; the vtable and RTTI are unchanged and were not separately rechecked.
- Shared-header consumer expansion: `include/daBasabasa_c.h` has one consumer. No shared header changed.
- Port/path/reference and other applicable static gates: above.
- Private validation, if run, and the exact PR head/base it tested: not run; no PR is opened by this stage.
