# Handoff: readable-ov084-fpkn-0925b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3148, task `readable-ov084-fpkn-0925b`, stage `revise`, role producer, session `claude-prod-readable-ov084-fpkn-0925b`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov084-fpkn-0925b`, input `c31f43bacfde4eb8e184b75077f85f4ba45a72a9` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `c31f43bacfde4eb8e184b75077f85f4ba45a72a9`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments below are prose records (the source change tried and the measured result); no experiment artifact is committed.
- Next action, responsible role and blockers: independent verification of this commit (byte, relocation, whole-object and source review). No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov084/daFPkn_c`, `src/actors/daFPkn_c.cpp`, ov084 .text 0x0212d248..0x0212ea18 (18 functions). Promoted in #3115; this is its first readability pass.
- Reserved source/header/config surfaces actually touched: `src/actors/daFPkn_c.cpp`, `include/daFPkn_c.h`, `config/arm9/overlays/ov084/symbols.txt` (nine rows renamed in place), `config/tu_manifest.d/ov084/daFPkn_c.json` (see Changed paths), this handoff, and this class's nine keys in the `attribution.json` ledger. `config/arm9/overlays/ov084/delinks.txt` is reserved but unchanged: it holds only address ranges.
- ROM observations: none new. The nine helpers are reached only by `bl` from inside this run (the manifest's boundary evidence: ov084 relocs.txt has no caller outside it), so an in-place rename of their symbols.txt rows keeps every destination address.
- Lineage evidence or structural inference: the helpers take the object in r0 and read only its fields, so they are written as members. Field meanings come from their uses. 0x1f4: StateInit seeds the leader's copy with its own uniqueID, and StateWait writes the uniqueID of the member it hands the star marker to. 0x218: set to 0x1f40 on a hit, added to mAngleY every frame and run down to 0 by ApproachLinear (the spin). 0x21d: set to 10 on a hit and counted down once per finished animation (the spin count). 0x21c: set to 1 when the plant emerges, cleared once it is back under. 0x221: the leader's star-marker slot, set to -1 by StateInit and passed to SetStarMarker when non-negative. 0x224 and 0x228: `Particle::System::New` handles, fed back into New and zeroed together.
- Hypothesized names/filenames, explicitly not recovered facts: `SpawnDeathSmoke` (0x0212d2dc, `_ZN8daFPkn_c15SpawnDeathSmokeEv`), `SpawnDeathBurst` (0x0212d42c, `_ZN8daFPkn_c15SpawnDeathBurstEv`), `OnGroupMemberDefeated` (0x0212d560, `_ZN8daFPkn_c21OnGroupMemberDefeatedEv`), `UpdateClsnOffset` (0x0212d564, `_ZN8daFPkn_c16UpdateClsnOffsetEv`), `CheckClsnHits` (0x0212d86c, `_ZN8daFPkn_c13CheckClsnHitsEv`), `StateGrow` (0x0212dc30, `_ZN8daFPkn_c9StateGrowEv`), `StateSpit` (0x0212ddbc, `_ZN8daFPkn_c9StateSpitEv`), `StateWait` (0x0212e010, `_ZN8daFPkn_c9StateWaitEv`) and `StateInit` (0x0212e4e0, `_ZN8daFPkn_c9StateInitEv`) are coined. So are the field names `mMarkedMemberID`, `mSpinSpeed`, `mEmerged`, `mSpinCount`, `mStarMarkerIdx`, `mParticleHandle1` and `mParticleHandle2`. `include/daFPkn_c.h` marks both groups as coined next to the declarations.
- Compiler experiments and measured barriers: see the table below. Each was run with `python tools/tubuild.py verify ov084/daFPkn_c` on the working tree of this task, followed by a pyelftools comparison of the TU object against the base object, and then kept or reverted as the Result column says.

| ID | Where | Change tried | Result |
|---|---|---|---|
| E0 | All nine helpers | Signatures only: members over a `char *` alias of `this`, calls through the member names, symbols.txt rows and manifest symbols renamed | 18/18; object identical under the rename map. Superseded by the per-function rows below, which drop the alias. |
| E1 | SpawnDeathBurst | Each axis folded into one expression on a Vector3 local, no temporaries | 36 words differ |
| E2 | SpawnDeathBurst | Vector3 local, `fac` and `m` temporaries; no volatile stores, no `zero`, `bias` or table-pointer temporaries | 18/18. Adopted. |
| E3 | UpdateClsnOffset | Named fields (`mat4x3.m[9..11]`, `data.bones`, `data.transforms[6]`, `mClsnOffset`); `Locals` kept | 18/18. Adopted. |
| E4 | UpdateClsnOffset | E3 with compound assignments, no `v`, no `flag` | DIFF |
| E5 | UpdateClsnOffset | E3 without the `px`, `py`, `pz` pointer launders | 18/18. Adopted. |
| E6 | UpdateClsnOffset | E5 with `if (actorID == 0xfb)` in place of the int flag | DIFF |
| E7 | CheckClsnHits | Named fields; real `dActor_c::FindWithID`, `JumpedOnByPlayer`, `dEnemyBase_c::SpawnCoin`, `KillAndTrackInDeathTable`, `Player::IncMegaKillCount`, `Sound::PlayBank0`; gotos kept | 18/18, undefined symbols unchanged. Adopted. |
| E8 | CheckClsnHits | E7 with the two flag-pointer blocks written `mdCcAc_c.flags \|= 1; mFlags &= ~0x10000000;` | 18/18. Adopted. |
| E9 | CheckClsnHits | E8 with the `t` int flags replaced by direct actorID compares | DIFF, one relocation destination wrong |
| E10 | CheckClsnHits | Goto-free form: a `hit` flag and nested ifs | DIFF, one relocation destination wrong |
| E11 | StateGrow | Named fields; real `ApproachLinear(int &)`, `ApproachLinear(short &)`, `Animation::WillHitFrame` through `mModelAnim`, `ClosestPlayer`, `dActor_c::Spawn`, `MarkForDestruction`; `AT()` launder dropped; goto kept | 18/18, undefined symbols unchanged. Adopted. |
| E12 | StateGrow | E11 with `v.x = player->mPosX` and so on in place of the pointer `p` | DIFF |
| E13 | StateGrow | E11 with the `tail` goto folded into the early return | DIFF |
| E14 | StateSpit | Named fields; real `ApproachLinear`, `Animation::Finished` and `WillHitFrame`, `ClosestPlayer`; goto kept | 18/18. Adopted. |
| E15 | StateSpit | E14 with the `cold` branch moved into the early return | DIFF, two relocation destinations wrong |
| E16 | StateSpit | E14 with the fireball position built by compound assignments, no `speed`, `idx`, `dx`, `dz`, `tmp` or `y` | 18/18. Adopted. |
| E17 | StateSpit | E16 with the `b` int flags replaced by direct actorID compares | DIFF, one relocation destination wrong |
| E18 | StateWait | Named fields, the group leader typed `daFPkn_c *`; real `FindWithID`, `Spawn`, `DistToCPlayer`, `ClosestPlayer`, `KillAndTrackInDeathTable`, `TrackInDeathTable`, `Animation::Finished`, `ApproachLinear` | 18/18, undefined symbols unchanged. Adopted. |
| E19 | StateWait | E18 with `mAngleY += mSpinSpeed` in place of the `short *` temporary | 18/18. Adopted. |
| E20 | StateWait | E19 with a `Player *` local in place of reusing `int dist` for ClosestPlayer | 18/18. Adopted. |
| E21 | StateWait | E20 with the `b` int flags replaced by direct actorID compares | DIFF, two relocation destinations wrong |
| E22 | StateInit | Named fields, the members typed `daFPkn_c *`, real `dActor_c::FindWithActorID` | 18/18. Adopted. |
| E23 | StateInit | E22 with `while ((p = FindWithActorID(0xfb, p)) != 0)` | DIFF |
| E24 | InitResources | `mModelAnim.SetAnim` with a `Fix12<int>` local for the speed | DIFF (0x0c bytes more before the call) |
| E25 | Render | `if (v == 0 \|\| (mFlags & 0x40000) != 0)` without the int `b` | DIFF |
| E26 | OnYoshiTryEat | `return actorID == 0xfc ? 4 : 0;` | DIFF |
| E27 | Behavior | `mStateTimer = (u16)mStateTimer + 1` in place of the `u16 *` counter | 8 words differ |
| E28 | Behavior | `if ((mFlags & 0x60000) != 0)` without the int `b` | DIFF, three relocation destinations wrong |
| E29 | Behavior | `if (actorID != 0xfc && ...)` without the int `b2` | DIFF |
| E30 | InitResources | `if (actorID == 0xfc) ... else if (actorID == 0xfd)` without `id` and `cond` | DIFF |
| E31 | InitResources | `id` kept, `cond` dropped | DIFF |
| E32 | Declarations | `data_ov084_02130e04` declared as `PknSharedFile` and read as `.file`; the local copy of `include/decl_common.h`'s `data_ov084_02130e1c` removed | 18/18, relocations identical. Adopted. |
| E33 | CheckClsnHits | `Vector3` locals for the two `Player::Hurt` positions in place of `int[3]` | 18/18. Adopted. |
| E34 | SpawnDeathSmoke | `#include "Particle__System.h"`, `Particle::System::FromUniqueID` and `System::callbackScale` in place of the C-linkage bridge and the raw `+ 0x50` store | 18/18, undefined symbols unchanged. Adopted. |

Every DIFF row was reverted. Where it leaves a codegen-forced form in the source (E6, E9, E10, E12, E13, E15, E17, E21, E23 to E31), that form carries one short comment.

## Findings

Self-audit of the base, numbered R1 to R24. Issue #3148 lists categories rather than numbered findings; each of its six bullets is mapped to the rows in the I column. Deferred rows name the owner as the next producer the coordinator assigns on #3148 (https://github.com/tangosdev/sm64ds-decomp/issues/3148).

| ID | Issue bullet | Category | Finding at base | Outcome | Reason or evidence |
|---|---|---|---|---|---|
| R1 | 1 | extern "C" helpers and state bodies | The nine helpers `func_ov084_0212d2dc` to `0212e4e0` are C-linkage free functions over a `char *` receiver, defined inside one `extern "C"` block | Fixed | E0 and the per-function rows: members under coined names, symbols.txt rows, manifest symbol fields and attribution keys renamed in place. Only `bl` from inside the run reaches them. |
| R2 | 1 | extern "C" helpers | `include/decl_common.h` still declares six of the old names (`func_ov084_0212d564`, `0212d86c`, `0212dc30`, `0212ddbc`, `0212e010`, `0212e4e0`) | Still deferred | Off limits (held by `jump-contract-repair-0918`). The declarations are unused: `grep` finds no caller in `src/`, and check_decl_agreement reports no new disagreement. Owner: next producer on #3148. |
| R3 | 1 | extern "C" helpers | StateGrow calls daPkn_c's `func_ov084_0212ec04` with a `(char *)` cast | Still deferred | `include/decl_common.h` declares it `(char *, int)`; daPkn_c's handoff keeps it C-linkage for that reason. Owner: next producer on #3148. |
| R4 | 3 | mangled bridges | C-linkage bridges for `dActor_c::FindWithID`, `FindWithActorID`, `ClosestPlayer`, `DistToCPlayer`, `JumpedOnByPlayer`, `Spawn`, `KillAndTrackInDeathTable`, `TrackInDeathTable`, `fBase_c::MarkForDestruction`, `dEnemyBase_c::SpawnCoin`, `Player::IncMegaKillCount`, `Animation::Finished`, `WillHitFrame`, `Sound::PlayBank0`, `ApproachLinear` (int and short), `Particle::System::FromUniqueID` | Fixed | E7, E11, E14, E18, E22, E34. The undefined-symbol set is unchanged. |
| R5 | 3 | mangled bridges | `ModelAnim::SetAnim`, `dCcAc_c::Init`, `dCcAcPos_c::Init` | Partially fixed | SetAnim is now measured (E24) and its comment says so. The two Init calls were not remeasured; the base comment claiming they DIFF is replaced by the file header's list of Fix12 bridges. Owner: next producer on #3148. |
| R6 | 3 | mangled bridges | `Particle::System::New`, `dActor_c::SpawnFireball`, `SpawnCoins`, `Player::Hurt`, `Player::Bounce` | Still deferred | Each symbol carries `Fix12<int>` by value (the same wall as E24); `include/Player.h` declares neither Hurt nor Bounce and is a shared header. Not measured. Owner: next producer on #3148. |
| R7 | 2 | unk_ fields | `unk_1f4`, `unk_21c`, `unk_21d`, `unk_224`, `unk_228`, and the pads `pad_218` and `pad_221` that the helpers read as fields | Fixed | Coined names from their uses (see Lineage); 0x218 typed `s16`, 0x221 typed `s8` with `pad_222[2]` after it. The size assertion still holds. |
| R8 | 2 | unk_ fields | `unk_108`, now written by name in CheckClsnHits (raw `0x108` at base) | Still deferred | Declared in `include/dEnemyBase_c.h`, a shared header outside this reservation. Owner: next producer on #3148. |
| R9 | 4 | raw offsets | Every helper reads `this` through raw offsets | Fixed | No raw `this` offset remains in the TU (E2, E3, E7, E11, E14, E18, E22). |
| R10 | 4 | raw offsets | UpdateClsnOffset walks bones as `base + idx * 0x34 + 0x1a` over `data.bones` | Still deferred | 0x34 is not `sizeof(BMD_Bone)` (0x40); no header types the record. Not measured. Owner: next producer on #3148. |
| R11 | 4 | casts | `(Vector3 &)mPosX`, `(Vector3 &)mCamSpacePosX`, `(Vector3_16 *)&mAngleX` and `&mPosX` passed as a Vector3 | Still deferred | `include/dActor_c.h` declares them as scalars; shared header outside this reservation. Owner: next producer on #3148. |
| R12 | 4 | casts | `PknSharedFile` view of the shared files, cast to `SharedFilePtr` elsewhere | Still deferred | `include/SharedFilePtr.h` declares no fields; shared header outside this reservation. |
| R13 | 4 | casts | UpdateClsnOffset reads the scratch matrix as `data_020a0e68[0x24 / 4]` | Still deferred | Not tried: `data_020a0e68` is an arm9 row declared `s32[]` here. |
| R14 | 5 | goto | `goto` in CheckClsnHits (two labels), StateGrow and StateSpit | Deferred: measured, no committed artifact | E10, E13, E15. One comment each. |
| R15 | 5 | temporaries | Int flags holding actorID and flag compares in UpdateClsnOffset, CheckClsnHits, StateSpit, StateWait, Behavior, Render, InitResources, OnYoshiTryEat | Deferred: measured, no committed artifact | E6, E9, E17, E21, E25, E26, E28 to E31. One comment each. |
| R16 | 5 | temporaries | StateGrow reads the player position through `int *p` | Deferred: measured, no committed artifact | E12. One comment. |
| R17 | 5 | temporaries | Behavior counts `mStateTimer` through a `u16 *` | Deferred: measured, no committed artifact | E27. Comment shortened to one line. |
| R18 | 5 | temporaries, launders | Volatile stack stores and `zero`, `bias`, table-pointer temporaries in SpawnDeathBurst; `px`, `py`, `pz` pointers in UpdateClsnOffset; `short *hp` in StateWait; `dist` reused for ClosestPlayer; flag-pointer blocks in CheckClsnHits, StateSpit, StateWait; the `AT()` launder | Fixed | E2, E5, E8, E11, E16, E19, E20. |
| R19 | 5 | temporaries | StateInit's `for (;;)` with a break | Deferred: measured, no committed artifact | E23. One comment. |
| R20 | 6 | dead or local declarations | 14 bridge prototypes made dead by R4, three local helper prototypes, `FPknVec3`, the `AT()` macro, `_ZN9Animation8FinishedEv`, and a local copy of `data_ov084_02130e1c` that `include/decl_common.h` already declares | Fixed | Removed (E32 for `data_ov084_02130e1c`). check_decl_agreement: no new local redeclarations. |
| R21 | 6 | local declarations | `func_02012694`, `func_0201267c`, `Vec3_HorzAngle`, `IsStarCollectedInCurLevel`, `SetStarMarker`, `LoadBlueCoinModel`, `UnloadBlueCoinModel` and the matrix helpers are declared locally over `void *`; `ApproachLinear` is now declared locally in its real C++ form | Still deferred | No header this TU includes declares them. Not examined further. |
| R22 | not listed | address-named data | `data_ov084_02130dfc`, `02130df4`, `02130e04`, `02130e14`, `02130e1c`, `02130e24`, `021302f4`, `0213029c`, `021302c4`, `02130294` | Still deferred | `data_ov084_02130e1c` is declared by `include/decl_common.h` (off limits), and the shared files are named by other sources (the static initializer, the classInit sources); renaming the rows alone would strand them. Each now carries a short comment. |
| R23 | 6 | unmeasured codegen notes | Four "Leftover" notes (Render, Behavior counter, InitResources Fix12 calls, OnYoshiTryEat) and the file-header claim that the helpers had to stay C-style | Fixed | Each kept note was remeasured (E24 to E27) and cut to one line; the file-header claim was false (E0) and is gone. The dCc Init part of the InitResources note was not remeasured and is dropped (R5). |
| R24 | not listed | stale manifest notes | `boundary_evidence[0]` and `[1]`, `notes[1]` and `notes[2]` name the old helper names or describe the extern "C" set; `compiler_only_output[1].reason` lists only InitResources as a Vector3 user | Fixed | See Changed paths. |

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 18/18 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: all 18 ROM functions are daFPkn_c members (D1, D0, the nine helpers and states, and the seven methods). The TU calls 8 mangled callee bridges, all carrying `Fix12<int>` by value (R5, R6), and one C-linkage daPkn_c helper (R3).
- Recovered layout/fields; remaining shadow structs/raw offsets: every daFPkn_c field is named. No raw `this` offset remains. Left: the bone-record stride (R10), the scratch-matrix index (R13), the `PknSharedFile` view (R12), the Vector3 casts over dActor_c scalars (R11) and `unk_108` in the base (R8).
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The nine new members are non-virtual, so layout and vtable are unchanged.
- Attribution preserved through each move/rename: the nine `attribution.json` keys follow the renamed symbols with the same authors (`tangosdev` keeps SpawnDeathSmoke, OnGroupMemberDefeated, StateWait and StateInit; `lunavyqo` keeps SpawnDeathBurst, CheckClsnHits and StateSpit; `ruspecial` keeps UpdateClsnOffset; `andrewboudreau` keeps StateGrow).
- Remaining agreed issue scope: R2, R3, R5, R6, R8, R10 to R17, R19, R21, R22.

Residue. The `lines` column is `wc -l` of `git show <rev>:<path>` (newline count; both revisions end in a newline). Each token column is the number of lines containing the token, `grep -c -F`, with `MSYS_NO_PATHCONV=1`. For `src/actors/daFPkn_c.cpp`, occurrences (`grep -o -F | wc -l`) are 6, 47, 3 and 87 at base and 1, 15, 1 and 43 at this commit.

| Path | Revision | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|---|
| `src/actors/daFPkn_c.cpp` | base `c31f43bacf` | 904 | 5 | 47 | 3 | 87 |
| `src/actors/daFPkn_c.cpp` | this commit | 813 | 1 | 15 | 1 | 43 |
| `include/daFPkn_c.h` | base `c31f43bacf` | 92 | 9 | 0 | 0 | 1 |
| `include/daFPkn_c.h` | this commit | 110 | 4 | 0 | 0 | 1 |

At this commit the one `unk_` line in the source is `unk_108` (R8). The 15 `func_` lines are the two local declarations and 12 calls of `func_02012694` and `func_0201267c`, plus the one call of `func_ov084_0212ec04` (R3). Of the 43 `_ZN` lines, 18 are symbol-marker comment lines (9 at base) and the other 25 are the eight Fix12 bridges' declarations and calls. The four `unk_` lines in the header are comment lines recording markers the member types absorbed.

## Changed paths

`git diff --name-only c31f43bacf..HEAD` lists six paths:

| Path | Change |
|---|---|
| `attribution.json` | nine keys renamed with their symbols, authors unchanged |
| `config/arm9/overlays/ov084/symbols.txt` | nine rows renamed in place (0x0212d2dc, 0x0212d42c, 0x0212d560, 0x0212d564, 0x0212d86c, 0x0212dc30, 0x0212ddbc, 0x0212e010, 0x0212e4e0) |
| `config/tu_manifest.d/ov084/daFPkn_c.json` | the keys below |
| `include/daFPkn_c.h` | field names and types, the nine member declarations, coined-name comments |
| `notes/agents/handoffs/readable-ov084-fpkn-0925b.md` | this handoff |
| `src/actors/daFPkn_c.cpp` | the source changes above |

Changed keys in `config/tu_manifest.d/ov084/daFPkn_c.json`: `functions[2..10].symbol` (the nine renames), `boundary_evidence[0]` and `[1]`, `notes[1]` and `notes[2]` (current names, what the rename did), and `compiler_only_output[1].reason` (the current Vector3 users). `verification.compilerOnlyOutput.droppedSections[2]` went from 22 to 15: `tools/tubuild.py verify` rewrites that list from the object it compiles, and the value is the section index of the dropped `_ZN7Vector3D1Ev` .text, which moved because SpawnDeathBurst now holds the first Vector3 local in the file. Both objects have 57 sections. `notes[0]`, `sections`, the other `verification` keys and the other `compiler_only_output` entries are unchanged; each was reread against this commit.

## Proof

All commands were run in `C:/tmp/claude-rd-ov084` on the source of this commit (the handoff is the only later change).

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov084/daFPkn_c` (base, before any edit) | 0 | 18/18 MATCH; objisolate clean; relocation destinations clean |
| `python tools/tubuild.py verify ov084/daFPkn_c` | 0 | 18/18 MATCH; objisolate clean; relocation destinations clean; ROM-ascending order |
| Object comparison with pyelftools, the object `tubuild verify` builds from base `c31f43bacf` against the one it builds from this commit, under the nine-row rename map | 0 | 57 sections in each object; 22/22 function bodies identical; 21/21 relocation sections identical (offset, type, symbol, addend); undefined-symbol set identical (93 symbols); defined global set (31 symbols) differs only by the nine renames |
| `python tools/rombuild.py -j16` (full, with ROM) | 0 | 7100 enrolled sources; first run 0 reused and 7100 compiled, rerun after the last source edit 7099 reused and 1 compiled (`build/src/actors/daFPkn_c.o` rebuilt); 11,213 of 11,213 source-built functions reproducing, 0 mismatching; module fidelity 106/106 exact; intact TU gates PASS with zero new symbol errors; `intactTuRom.identical` true. Its baseline control step (no TU substitution) reports 9 pre-existing `dsd check symbols` errors in arm9 and ITCM, none in ov084. |
| `python tools/prepush_linkcheck.py --range c31f43bacf..HEAD` | 0 | 18 checked, 18 verified, 0 warnings, 0 blocking (all nine renamed members VERIFIED) |
| `python tools/prepush_attribution.py --base c31f43bacf --head HEAD` | 0 | 7273 tracked, 0 changed, 0 lost |
| `python tools/check_decl_agreement.py --changed c31f43bacf` | 0 | no new local redeclarations, no new declaration disagreements |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check` | 0 | PASS (baseline 2998, current 3155) |
| `python tools/check_tubuild_conflicts.py` | 0 | 299 entries OK |
| `python tools/queue_audit.py --check-promoted` | 0 | OK |
| `python tools/check_src_tu_compiles.py` | 0 | 299/299 translation units compile |
| `python tools/port_refcheck.py` | 0 | 408 references, all resolve |
| `python tools/affected_src.py include/daFPkn_c.h` | 0 | `src/actors/daFPkn_c.cpp` only |
| `git diff c31f43bacf..HEAD -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` | 0 | empty |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify and prepush_linkcheck, above.
- Complete emitted TU and data/metadata checks: the object comparison above covers every function, relocation and symbol of the TU object. This TU licenses .text only; the vtable, RTTI and romdata checks from the promotion were not rerun, and the class's data and virtual members are unchanged.
- Shared-header consumer expansion: `include/daFPkn_c.h` has one consumer, `src/actors/daFPkn_c.cpp`. No shared header changed; `include/Particle__System.h` is newly included, unchanged.
- Port/path/reference and other applicable static gates: above.
- Private validation, if run, and the exact PR head/base it tested: not run.
