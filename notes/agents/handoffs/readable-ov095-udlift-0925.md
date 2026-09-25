# Handoff: readable-ov095-udlift-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3071, `readable-ov095-udlift-0925`, stage `revise`, role producer, session `claude-prod-readable-ov095-udlift-0925`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov095-udlift-0925`; input `7423f8d73f391870e7244a19599eba122f3596d5` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `7423f8d73f391870e7244a19599eba122f3596d5`.
- Separate evidence commits and required artifacts in this commit: none. The source commit and this handoff are the whole candidate.
- Next action, responsible role and blockers: independent verification (byte, relocation, whole-object and source review) of this commit. No blockers.
- Status: verified candidate, from the proof below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov095/daUdlift_c`, `.text` 0x02135ff4..0x02136764, 11 functions, in `src/actors/daUdlift_c.cpp` and `include/daUdlift_c.h`.
- Reserved source/header/config surfaces actually touched: `src/actors/daUdlift_c.cpp`, `include/daUdlift_c.h`, `config/arm9/overlays/ov095/symbols.txt` (five rows renamed in place), `config/tu_manifest.d/ov095/daUdlift_c.json` (symbols and notes), and this class's five rows in `attribution.json`. `include/decl_common.h` is untouched.
- ROM observations: ov095 `relocs.txt` resolves the five pointer-to-member constants at 0x0213757c..0x0213759c by address (state 0 to 0x02136368, 1 to 0x02136298, 2 to 0x02136178, 3 to 0x02136090, 4 to 0x02136104). Renaming the symbol rows therefore moves no linked byte.
- Lineage evidence or structural inference: `mMode` (was `unk_349`) is named from how the states branch on it. StateMoveUp stops at the top whenever it is non-zero. StateMoveDown parks in state 4 on 1 and in state 3 on 2. InitResources stores 1 only for actorID 0x83 (RC_RIFT02).
- Hypothesized names/filenames, explicitly not recovered facts: `StateWait`, `StateMoveUp`, `StateMoveDown`, `StateStop`, `StateStopAtBottom` and `mMode` are reconstructed names. The ROM records no member names.
- Compiler experiments and measured barriers (each run with `tools/tubuild.py verify ov095/daUdlift_c`):
  - Member `mMeshCollider.SetFile(file, mClsnMat, scale, mAngleY, *clps)` with a `Fix12<int>` local: InitResources DIFF, 10/11. The size changes and three relocation destinations go wrong. The mangled bridge stays.
  - Plain `if (actorID == 0x20) ... else if ...` in InitResources (as an if chain and as a switch): DIFF, 10/11. `(int)(actorID == N) != 0` in an else-if chain matches. The goto, the `t` and `b` locals and the `idx` local are gone.
  - Plain `actorID == 0x83` in StateWait and `actorID == 0x21` in StateMoveDown: each DIFF, 10/11. The `(int)` flag form stays, with one comment each.
  - Dropping `new_var` and passing `&dBgW::UpdatePosWithTransform` straight to `func_020393d4`: MATCH.
  - State bodies as members with implicit `this`: MATCH, and the object is identical section by section.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 11/11 functions, and every relocation destination is clean. Section bytes and relocations of all 24 sections in the emitted object equal the base object's, after mapping the five renamed state symbols.
- Genuine methods; remaining free-function/ABI bridges: all 11 functions are now `daUdlift_c` members. The remaining bridges are the three Fix12-by-value mangled calls (`dBgW_KcMbg::SetFile`, `dBgActor_c::IsClsnInRange`, `dActor_c::Earthquake`) and the unnamed arm9 setters `func_020393d4` and `func_020393c4`.
- Recovered layout/fields; remaining shadow structs/raw offsets: every own field is named. The flat-C shadow struct under `#else` is deleted, because no C file includes the header. The `*(Vector3 *)&mCamSpacePosX` pun remains, because `dActor_c` spells camera-space position as three scalars in a shared header.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The inline empty destructor gives D1 and D0 and no D2. InitResources is the key function.
- Attribution preserved through each move/rename: the five `attribution.json` keys follow the symbol renames with the same credit. `prepush_attribution` reports 0 changed and 0 lost.
- Remaining agreed issue scope: see the table.

### Deferred findings from #3071

| Finding | Outcome | Reason |
|---|---|---|
| S4 state bodies are extern "C" helpers | fixed | Now non-virtual members StateWait, StateMoveUp, StateMoveDown, StateStop and StateStopAtBottom. The ov095 symbol rows are renamed in place, and the object is byte and relocation identical. |
| S5 Fix12 mangled bridges | partially fixed | The SetFile member call was measured and DIFFs (see experiments). IsClsnInRange and Earthquake have no member declaration in `include/dBgActor_c.h` or `include/dActor_c.h`, which this task does not hold. |
| S6 goto, new_var and int flags | partially fixed | goto, new_var and the flag locals are gone. The `(int)(actorID == N) != 0` form is codegen-forced in three places, each with one comment. |
| S7 Vector3 pun | still deferred | `dActor_c` has no Vector3 member for camera-space position, and `include/dActor_c.h` is shared and not reserved. |
| S8 local prototypes, unk_349 and data tables | partially fixed | `unk_349` is `mMode`. The `ApproachLinear` prototype stays local, because no shared header declares `_Z14ApproachLinearRiii`. `data_ov095_02137910` is also named by `__sinit_ov095_0213722c`, and `data_ov095_021375a4` is declared in `include/decl_common.h`. Neither is held here, so the tables keep their address names. |
| S9 dead decl_common extern | still deferred | `include/decl_common.h` is held by `jump-contract-repair-0918`. |
| S10 flat-C shadow struct | fixed | Deleted. The header's only consumer is `src/actors/daUdlift_c.cpp`. |

Residue in `src/actors/daUdlift_c.cpp`, base to this commit:

| Measure | Base | This commit |
|---|---|---|
| lines | 352 | 304 |
| `unk_` | 5 | 0 |
| lines with `func_` | 23 | 5 |
| `extern "C"` | 6 | 1 |
| `_ZN`, all | 11 | 16 |
| `_ZN` outside marker lines | 7 | 7 |

The `_ZN` total rises because the five state markers now carry mangled names. `include/daUdlift_c.h` goes from 158 lines and 8 `unk_` to 73 lines and 0.

Prose outside this reservation still names the old spellings: the daUdlift_c table in `notes/platform-provenance.md` cites the five `func_ov095_*` state names and `unk_349`. Its links still resolve and no gate reads the names, but it needs a follow-up edit under a reservation for that file.

## Proof

All commands were run in `C:/tmp/claude-rd-ov095` on the source commit this handoff sits on. That commit's tree matches this one's except for this file. The inputs were the pinned `tools/mwccarm` 2004/b56 compiler and the `extracted/` ROM copied from the main checkout.

- Full-ROM build: `python tools/rombuild.py -j16` exit 0. 7189 enrolled source files, 7189 compiled, including `src/actors/daUdlift_c.cpp` (its object defines the five State symbols). 11,213 functions reproduce and 0 mismatch. 106/106 modules exact. `build/rombuild-report.json` has `intactTuRom.identical: true` with sha256 d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8. The baseline control's `dsd check symbols` errors are the tree's 9 pre-existing ones.
- Explicit function/consumer relocation checks:
  - `python tools/tubuild.py verify ov095/daUdlift_c`: 11/11 MATCH, objisolate clean, reloc-destinations clean, ROM-ascending order, TEXT-VERIFIED. It was re-run after every edit.
  - `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD` exit 0. 11 checked, 11 VERIFIED, 0 warnings, 0 blocking.
- Complete emitted TU and data/metadata checks: the undefined-symbol sets of `build/tu/ov095-daUdlift_c/daUdlift_c.o`, before and after (pyelftools), are identical at 58 names. The defined globals differ only in the five renamed state symbols. A section-by-section comparison of bytes and relocations (offset, type and target, with the renamed states mapped) is identical across all 24 sections.
- Shared-header consumer expansion: `include/daUdlift_c.h` has one consumer, `src/actors/daUdlift_c.cpp`. `python tools/check_header_offsets.py include/daUdlift_c.h`: 14 commented fields, 0 mismatched.
- Port/path/reference and other applicable static gates:
  - `python tools/prepush_attribution.py --base 7423f8d73f --head HEAD` exit 0: 0 changed, 0 lost.
  - `python tools/check_decl_agreement.py --changed 7423f8d73f` exit 0: no new declaration disagreements.
  - `python tools/check_dead_references.py` exit 0: no new dead references, no broken markdown links.
  - `python tools/tiers_ratchet.py --check` exit 0: PASS, baseline 2994, current 3116. `--update` was not run.
  - `python tools/check_tubuild_conflicts.py` exit 0: 293 entries, every conflict recorded.
  - `python tools/queue_audit.py --check-promoted` exit 0.
  - `python tools/check_src_tu_compiles.py` exit 0: 293/293.
  - `python tools/port_refcheck.py` exit 0: 408 checked, 0 stale.
  - `git diff 7423f8d73f..HEAD -- include/decl_common.h` is empty.
- Private validation, if run, and the exact PR head/base it tested: not run.
