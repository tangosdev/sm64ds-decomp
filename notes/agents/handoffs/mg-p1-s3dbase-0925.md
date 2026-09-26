# Handoff: mg-p1-s3dbase-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3171, `mg-p1-s3dbase-0925`, stage `revise`, role producer, session `claude-prod-mg-p1-s3dbase-0925`, Claude Code.
- Source branch and previous accepted input SHA: `readable/mg-p1-s3dbase-0925`; input `e63828444b7ad3fc20afb58de29857b618dddbcf` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `e63828444b7ad3fc20afb58de29857b618dddbcf`.
- Separate evidence commits and required artifacts in this commit: none. The source commit `65bdca9e4f16a3b2a0dc1e993354c8a86a4de196` and this handoff are the whole candidate.
- Next action, responsible role and blockers: independent verification (byte, relocation, whole-object and source review) of this commit. No blockers.
- Status: verified candidate, from the proof below.
- Remaining uncommitted/local-only material and where it is preserved: none. Private logs and the saved base object are under the worktree's ignored `build/`.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov006/dScMgSingle3DBase_c`, `.text` 0x0210a4b0..0x0210a8c0, nine functions, in `src/minigames/d_s_mg_single3_d_base.cpp`. Also the shared headers `include/dScMgSingle3DBase_c.h` and `include/dMgState_c.h`.
- Reserved source/header/config surfaces actually touched (`git diff --name-only e63828444b..HEAD`):
  - `include/dMgState_c.h`
  - `include/dScMgSingle3DBase_c.h`
  - `notes/agents/handoffs/mg-p1-s3dbase-0925.md`
  - `src/minigames/d_s_mg_single3_d_base.cpp`

  The reserved manifest `config/tu_manifest.d/ov006/dScMgSingle3DBase_c.json` is untouched. `include/decl_common.h`, `config/dead-reference-baseline.json`, `config/decl-agreement-baseline.json` and every `symbols.txt` are untouched.
- ROM observations:
  - `_ZTV19dScMgSingle3DBase_c` at ov006:0x0213e448 runs to the next symbol at 0x0213e4d8, 0x90 bytes, which is 36 slots. `python tools/rtti_vtables.py --own dScMgSingle3DBase_c` reports 36 slots with a base of 36: the class adds no virtual. `dScMgBase_c` has its own bodies at slot 26 (ov004:0x020b04e0) and slot 33 (ov004:0x020b265c).
  - The compiled object's `_ZTV19dScMgSingle3DBase_c` is 152 bytes: the two-word header plus 36 slots. `python tools/romdata_check.py --files src/minigames/d_s_mg_single3_d_base.cpp` reports it VERIFIED over 144 bytes, on the base and on this commit alike (6 VERIFIED over 200 bytes, 5 PARTIAL, 0 DIFFERS, identical JSON reports).
  - Virtual84 builds each VRAM destination in two instructions: `mov r1, #0x6400000` then `add r1, r1, #0x4000` at ov006:0x0210a7d0, and the same pair for 0x6600000 at 0x0210a7e0.
- Lineage evidence or structural inference: the Virtual84 trailing comments come from matched callees. `func_ov004_020b290c` calls the eight `Set*Bg*Offset(0, 0)`, and `func_ov004_020b2980` calls `GX::DisableAllBanks`. The `unk_024` comment comes from `dScMgBase_c::BeforeBehavior` and `BeforeRender` in `src/minigames/d_s_mg_base.cpp`, which step and draw the state machine only while it is 0.
- Hypothesized names/filenames, explicitly not recovered facts: none added. `dMgState_c` and its member names are coined, and the header now says so.
- Compiler experiments and measured barriers (each run with `python tools/tubuild.py verify ov006/dScMgSingle3DBase_c` and the pyelftools object comparison):
  - Deleting the local `InitialiseVramGlobals` and `data_ov004_020beb74[]` declarations: identical object.
  - `DecompressLZ16(fileData, (void *)0x6404000)` in Virtual84: DIFF, 44 words, with the relocations after it shifted. `(char *)0x6400000 + 0x4000`: the same DIFF. The split blocks stay, with one comment.
  - `REG_DISPCNT` and `REG_DISPCNT_SUB` from `include/nitro/hw/registers.h` in Virtual84: identical object.
  - `REG_DISPCNT &= ~0x300010u` for `&= 0xffcfffefu`: identical object.
  - `vu32` and `vu16` for `volatile int`, `volatile u16*` and `volatile u32*` in AfterCleanupResources and `func_ov006_0210a534`: identical object.
  - `void *LoadFile(int)` without the casts, as `dScMgCard_c` spells it: identical object, but `check_decl_agreement.py --symbol LoadFile` then lists this file against `int LoadFile(int)` in `src/LoadFile.c`, a new disagreement. Reverted.
  - The comment-only header edits: identical object.

## Findings

| Finding | Outcome | Reason |
|---|---|---|
| R1 stale "34-slot prefix" prose in `include/dScMgSingle3DBase_c.h` | fixed | The class emits all 36 slots and the table matches ov006:0x0213e448; the comment now says that in four lines. |
| R2 destructor comment: "declared first, so it takes slots 16 and 17", and process narration | fixed | An override takes the inherited destructor slots whatever its position. The inline-only constraint, the missing D2 and the key function are kept, shorter. |
| R3 header summary: "mahjong-carlo" and no camera | fixed | The 13 children are named by class stem from the ROM, and the camera at 0x4660 is listed next to the tracker. |
| R4 `include/dMgState_c.h` calls the name "inferred" and the layout "ROM-proven" | fixed | The name has no RTTI and is coined. The comment now says what the matched methods show, and `unk_024` gets one line from its two readers. |
| R5 local redeclarations of `InitialiseVramGlobals` and `data_ov004_020beb74[]` | fixed | `include/decl_common.h` declares both with the same types. |
| R6 raw MMIO addresses | partially fixed | Display control uses `REG_DISPCNT` and `REG_DISPCNT_SUB`. The light, sub BG3 control and sub BG3 offset registers have no name in `include/nitro/hw/registers.h`, which this task does not hold. |
| R7 three volatile spellings in one file | fixed | One `vu16` or `vu32` spelling, same widths. |
| R8 `0xffcfffefu` beside `~mask` neighbours | fixed | `~0x300010u`, the cleared bits, like the two lines above it. |
| R9 split `dst += 0x4000` blocks in Virtual84 | deferred, codegen-forced | Both plainer spellings DIFF (above). One comment names the mov and add pair they keep. |
| R10 `func_ov004_020adc68` call shape (SINGLE3D-ABI-01) | deferred | Kept as the brief requires. Its definition in `src/func_ov004_020adc68.c` still has no argument or result. |
| R11 particle tracker return types (SINGLE3D-HEADER-01) | no edit needed | The class header now takes `include/Particle__SysTracker.h`, whose `Initialise` and `Update` return void. `--symbol` reports 0 disagreements for both. |
| R12 `LoadFile` returns `int` here but `void *` in Card and Memory | deferred | The pointer spelling is byte-neutral here but adds a declaration disagreement. It needs one prototype for every caller (theme 8). |
| R13 stale vtable prose in the TU manifest | deferred | The `_ZTV19dScMgSingle3DBase_c` reason in `config/tu_manifest.d/ov006/dScMgSingle3DBase_c.json` still says "NOT YET OWNABLE" and "18-slot table ... PARTIAL". The object now emits 36 slots and `romdata_check` reports VERIFIED. The file is reserved but was not in this brief's editable list. |
| R14 unnamed ov004 callees and globals | deferred to pass 2 | Pass 1 adds no names. Two callees now carry a one-line comment on what they do. |

Every deferred row is for coordinator `claude-promo-coord-0923` under https://github.com/tangosdev/sm64ds-decomp/issues/3171.

## Theme results

| Theme | Result here | Where |
|---|---|---|
| 1 inherited base fields by name | not applicable | The TU has no raw-offset member access. |
| 2 `(char *)this` alias | not applicable | None at base (count 0). |
| 3 mangled calls to C++ methods | not applicable | Every C++ callee already goes through a declaration. |
| 9 split cursors (not assigned, seen here) | measured negative here | Virtual84's two `DecompressLZ16` destinations. |
| 11 hand-rolled vtable views | not applicable | None in the TU. |
| 13 resource loading | already applied, one lever measured | The calls are typed. `void *LoadFile(int)` is byte-neutral in `func_ov006_0210a534` but blocked by the declaration gate. |
| 14 Fix12 arithmetic | not applicable | The only 0x1000 is `G3X::SetFog`'s fog offset, a raw depth written to 0x0400035c. |
| candidate: named MMIO registers | proven here | `REG_DISPCNT` and `REG_DISPCNT_SUB` in Virtual84. |
| candidate: dead redeclarations of `decl_common.h` symbols | proven here | The `extern "C"` block above Virtual84. |

## Residue

Counts are lines containing the string (`git show <rev>:<path> | grep -c -F`, with `MSYS_NO_PATHCONV=1`); lines are `wc -l`.

| Measure | source base | source now | `dScMgSingle3DBase_c.h` base | now | `dMgState_c.h` base | now |
|---|---|---|---|---|---|---|
| lines | 179 | 182 | 87 | 71 | 33 | 35 |
| `func_` | 12 | 12 | 2 | 0 | 0 | 0 |
| `unk_` | 0 | 0 | 0 | 0 | 2 | 2 |
| `_ZN` | 6 | 6 | 1 | 1 | 0 | 0 |
| `extern "C"` | 2 | 2 | 0 | 0 | 0 | 0 |
| `goto` | 0 | 0 | 0 | 0 | 0 | 0 |
| `(char *)this` | 0 | 0 | 0 | 0 | 0 | 0 |

All six `_ZN` lines in the source are symbol markers. The header's one `_ZN` names the missing D2.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 9 of 9 functions match and every relocation destination is clean. The whole object is unchanged (below).
- Genuine methods; remaining free-function/ABI bridges: seven members plus the compiler's D1 and D0. `func_ov006_0210a534` stays a free function; no vtable or name reaches it. The `func_ov004_020adc68` bridge stays (R10).
- Recovered layout/fields; remaining shadow structs/raw offsets: unchanged. The camera head `pad_4660` and `pad_471a` stay.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The inline destructor gives D1 and D0 with no D2, and AfterInitResources is the key function.
- Attribution preserved through each move/rename: no move or rename.
- Remaining agreed issue scope: R9, R10, R12, R13 and R14.

## Proof

All commands ran in `C:/tmp/claude-mgp-p1-s3dbase-0925` on source commit `65bdca9e4f16a3b2a0dc1e993354c8a86a4de196`. That tree differs from this commit's only by this file. The inputs were the pinned `tools/mwccarm` 2004/b56 compiler and the `extracted/` ROM, both copied from `C:/tmp/claude-mg`.

- TU verify: `python tools/tubuild.py verify ov006/dScMgSingle3DBase_c` exit 0: 9/9 MATCH, objisolate clean, reloc-destinations clean, TEXT-VERIFIED. The same on the untouched base, and after every lever.
- Object comparison (pyelftools, base object saved before any edit against the candidate's `build/tu/ov006-dScMgSingle3DBase_c/d_s_mg_single3_d_base.o`): 22 sections byte-identical; 109 relocations in 14 sections identical by offset, type, symbol and addend; 28 defined and 75 undefined symbols identical. Both objects are 9,272 bytes with SHA256 `d8c2029388fcd6a9d500ad77ec6745ec535704c2299ebf814dd9822d7a76e59b`.
- Full-ROM build: `python tools/rombuild.py -j8 --report-json build/rombuild.json --data-json build/rombuild-data.json` exit 0.
  - 7069 enrolled sources, 7069 compiled, 0 reused from cache.
  - 11,214 functions reproduce and 0 mismatch; 106/106 modules exact.
  - `intactTuRom.identical: true`, SHA256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
  - ROM data: 857 verified, 318 partial, 3 differ (`_ZTV10dCcAcPos_c`, `_ZTIN8daDemo_c10anmModel_cE`, `_ZTIN8daDemo_c13simpleModel_cE`), none from these files.
  - The baseline control's `dsd check symbols` has the tree's nine known ITCM and ARM9 errors; zero new ones.
- Shared-header consumer expansion:
  - `python tools/affected_src.py` lists 31 consumers of `include/dScMgSingle3DBase_c.h` and 147 of the two headers together. rombuild compiled 141 of the 147; the other six are not source-built (three `.text` entries without `complete`, three not enrolled).
  - All 147 were also compiled with the base `include/` tree and with this one, and every object is byte-identical (147 identical, 0 different, 0 failed). A positive control, an `#error` in the base copy of `dMgState_c.h`, failed that compile as expected.
  - `python tools/check_header_offsets.py include/dScMgSingle3DBase_c.h include/dMgState_c.h` exit 0: 1 commented field, 0 mismatched, struct spans 0x4f38; `dMgState_c.h` waived as before.
- Relocation link check: `python tools/prepush_linkcheck.py --range e63828444b..HEAD` exit 0: the two headers fan out to 147 sources; 1080 functions checked, 1080 VERIFIED, 0 warnings, 0 blocking. It skipped the three consumers still marked as non-matching drafts (`_ZN11dScMgJump_c13OnYoshiTryEatEi.cpp`, `_ZN12dScMgSlot1_c8BehaviorEv.cpp`, `func_ov006_020e5450.cpp`); the 147-consumer object comparison covers them.
- Static gates:
  - `python tools/prepush_attribution.py --base e63828444b --head HEAD` exit 0: 7241 tracked, 0 changed, 0 lost.
  - `python tools/check_decl_agreement.py --changed e63828444b` exit 0: no new local redeclarations of header-declared symbols, no new declaration disagreements.
  - `python tools/check_decl_agreement.py` exit 0: no new declaration disagreements. It also reports 1024 banked rows gone, the same count on the base files, so they are not from this change. The baseline was not edited.
  - `python tools/check_dead_references.py` exit 0, run with this handoff in place: no new dead references, no broken markdown links.
  - `python tools/tiers_ratchet.py --check` exit 0: PASS, baseline 3001, current 3216. `--update` was not run.
  - `python tools/check_tubuild_conflicts.py` exit 0: 302 entries, every conflict recorded.
  - `python tools/queue_audit.py --check-promoted` exit 0: every row agrees with the manifests.
  - `python tools/check_src_tu_compiles.py` exit 0: 302/302 compile.
  - `python tools/port_refcheck.py` exit 0: 408 checked, all resolve.
  - `git diff e63828444b..HEAD -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` is empty.
- Private validation, if run, and the exact PR head and base it tested: not run.
