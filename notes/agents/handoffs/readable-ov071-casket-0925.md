# Handoff: readable-ov071-casket-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3082,
  `readable-ov071-casket-0925`, stage `revise`, session `claude-prod-readable-ov071-casket-0925`,
  Claude Code (Opus 5.5).
- Source branch and previous accepted input SHA: `readable/readable-ov071-casket-0925`, input
  `7423f8d73f391870e7244a19599eba122f3596d5`.
- Original source base SHA and installed workflow/tool SHA: both `7423f8d73f391870e7244a19599eba122f3596d5`.
- Separate evidence commits and required artifacts in this commit: none. The measurements below
  were scratch-applied and reverted, and the one existing experiment cited is
  `notes/experiments/batch2-2685-dossunbar-setfile.md`.
- Next action, responsible role and blockers: independent verification (verifier) of this commit:
  byte, relocation, whole-object and source review. There are no blockers.
- Status: verified candidate. Every local gate listed below exits 0.
- Remaining uncommitted or local-only material and where it is preserved: none.

## What changed and why

- Class, TU, symbol and module-qualified ROM scope: `ov071/daObjCasket_c`, ov071 `.text`
  `[0x02121fe4, 0x021226a0)`, 14 functions.
- Reserved surfaces actually touched: `src/actors/daObjCasket_c.cpp`, `include/daObjCasket_c.h`,
  `config/arm9/overlays/ov071/symbols.txt` (7 rows renamed in place),
  `config/tu_manifest.d/ov071/daObjCasket_c.json` (7 symbol fields plus the evidence prose) and
  this class's 7 override keys in `attribution.json` (re-keyed in place, with credit unchanged).
  The ov071 `delinks.txt` needs no change, because its row for this TU is a range with no symbol names.
- ROM observations: the state table at `data_ov071_02122ecc` holds `{ init, exec, name }` records
  named "WAIT" and "STANDUP". `__sinit_ov071_02122a64` copies the pointer-to-member constants at
  0x02122e74..0x02122e8c into it. Those constants are ROM data relocations to the function addresses,
  so they follow the symbols.txt rename without any source edit.
- Lineage or structural inference: the functions are reached as members through
  `void (daObjCasket_c::*)()` and take the object in r0. Member form is therefore the natural
  spelling, and it is byte-identical.
- Hypothesized names, explicitly not recovered facts: `St_Wait_Init`, `St_Wait_Main`,
  `St_StandUp_Init`, `St_StandUp_Main`, `SetState(int)`, `RunState()` and `UpdateModelTransform()`
  are descriptive. They follow the tree's existing spellings for the same shapes
  (`daKinopio_c::St_Idle_Init`, `SetState` and `RunState`, and `daObjC1Peach_c::UpdateModelTransform`),
  and the state words come from the ROM's "WAIT" and "STANDUP" strings.
- Compiler experiments and measured barriers (pinned mwccarm 2004/b56,
  `tools/match.py ... --module ov071 --strict-relocs --brief`; baseline MATCH in each case):
  - `mMeshCollider.SetFile(..., Fix12<int> scale, ...)` as a member call in place of the mangled
    bridge: InitResources is 0x120 bytes against the ROM's 0x114. This is the same result as the
    committed `notes/experiments/batch2-2685-dossunbar-setfile.md`.
  - The closest player's position read straight off `pl->mPosX..mPosZ`, or struct-copied as
    `*(Vector3 *)&pl->mPosX`: St_Wait_Main is 0x1f0 bytes against the ROM's 0x1f4 in both forms.
  - `mModel.mat4x3.t.x` in place of `m[9]`: does not compile, because the Matrix4x3 spelling
    this TU sees (common.h, ahead of Model.h, per `include/dBgActor_c.h`) has no `t` member.

## Findings from #3082

| ID | Finding | Outcome | Reason |
|---|---|---|---|
| R1 | extern "C" state helpers called through a pointer-to-member table | fixed | The seven helpers are now members of daObjCasket_c. The symbols.txt rows, manifest and attribution keys are renamed in place. The build is byte- and relocation-identical, and the undefined-symbol set is identical. |
| R2 | three mangled Fix12 bridges | still deferred, now measured | SetFile as a member call measures +0xc bytes (see above; artifact `notes/experiments/batch2-2685-dossunbar-setfile.md`). `dBgActor_c::IsClsnInRange` and `dActor_c::Earthquake` have no member declaration in `include/dBgActor_c.h` or `include/dActor_c.h`, which are shared headers outside this reservation. The source comment now cites the measurement. |
| R3 | func_020393d4 integer casts | still deferred | The `(int *, int)` declaration agrees with the definition `src/func_020393d4.c`. Retyping it here would add a new decl-gate disagreement, and fixing the definition is outside this reservation. |
| R4 | Vector3 casts over the base class's position scalars | still deferred | `include/dActor_c.h` declares only `mPosX..mPosZ` and `mCamSpacePosX..Z` scalars, with no Vector3 member. The header is shared and not reserved. |
| R5 | locally declared shared helpers | still deferred | The math, sound and timer helpers are declared in no shared header except `include/decl_common.h`, which is off limits. A new shared header needs its own reservation. |
| R6 | two unartifacted codegen notes | fixed | The player-copy note is one line naming what it forces, and it was re-measured (0x1f0 against 0x1f4). The Fix12 note now cites the measured size and the committed experiment. The manifest already records the player-copy measurement. |
| R7 | decl_common | still deferred | `include/decl_common.h` is held by `jump-contract-repair-0918`. It still declares `func_ov071_02122080`, `func_ov071_021223c8` and `func_ov071_02122414`. After this rename those declarations name no defined symbol. They are unused, so the build is unaffected, and they should be deleted when the file is free. |
| R8 | "Coffin" in `config/tu_manifest.d/ov071/daEyBm_c.json` | still deferred | Line 9 says "ending immediately before Coffin D1". Not in this reservation. |
| R10 | "COFFIN" in `src/game/actors/d_a_ey_bm.cpp` | still deferred | Line 9 lists "COFFIN" among the ov071 classes. Not in this reservation. |

R9 was fixed at promotion and is unchanged.

## Reconstruction dimensions

- Exact function, byte and relocation coverage: 14 of 14 functions, full TU range.
- Genuine methods, and the remaining free-function and ABI bridges: 13 methods plus the C-linkage
  factory `daObjCasket_c_classInit`. Bridges: 3 mangled Fix12 calls (R2) and `func_020393d4` (R3).
- Recovered layout and fields, and the remaining shadow structs and raw offsets: no `unk_` fields.
  The position arguments still cast over base scalars (R4).
- Lifecycle, vtable, RTTI, initializer and data ownership: unchanged from promotion. The state
  table and the two SharedFilePtr objects stay `data_ov071_*` rows owned by `__sinit_ov071_02122a64`.
- Attribution preserved through each rename: the 7 overrides were re-keyed to the new symbols
  with the same credit.
- Residue in `src/actors/daObjCasket_c.cpp` (matching lines, base to this commit): lines 299 to 291,
  `unk_` 0 to 0, `func_` 29 to 2, `extern "C"` 9 to 2, `_ZN` 11 to 18. The 7 added `_ZN` lines are
  the new marker lines. Non-marker `_ZN` lines stay at 7: the three Fix12 bridges.

## Proof

All commands were run in this worktree against this commit's tree, with base `7423f8d73f`.

- `python tools/tubuild.py verify ov071/daObjCasket_c`: exit 0. 14/14 MATCH, objisolate clean,
  reloc-destinations clean, emitted in ROM order (TEXT-VERIFIED). The baseline before any edit was
  also 14/14.
- `python tools/rombuild.py -j16`: exit 0. `enrolledFiles` 7189, all 7189 compiled, including
  `build/src/actors/daObjCasket_c.o`, which defines the new member symbols. 11,213 reproducing,
  0 mismatching, 106/106 modules exact. `intactTuRom.identical` is true, sha256 d1506e90...c478e8.
  The baseline control's `dsd check symbols` ITCM errors are pre-existing, and the intact gate
  reports zero new symbol errors.
- `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD`: exit 0. 14 checked, 14 VERIFIED.
- `python tools/prepush_attribution.py --base 7423f8d73f --head HEAD`: exit 0. 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed 7423f8d73f`: exit 0, no new disagreements.
- `python tools/check_dead_references.py`: exit 0, no new dead references.
- `python tools/tiers_ratchet.py --check`: exit 0. PASS, baseline 2994, current 3119. `--update` was not run.
- `python tools/check_tubuild_conflicts.py`: exit 0, 293 manifest entries.
- `python tools/queue_audit.py --check-promoted`: exit 0.
- `python tools/check_src_tu_compiles.py`: exit 0, 293/293.
- `python tools/port_refcheck.py`: exit 0, 408 checked.
- Undefined symbols of the TU object (pyelftools on `build/tu/ov071-daObjCasket_c/daObjCasket_c.o`):
  64 before and 64 after, character-identical, and 0 are missing from symbols.txt. The defined set
  differs only by the 7 renamed rows.
- `include/decl_common.h`, `config/dead-reference-baseline.json` and
  `config/decl-agreement-baseline.json`: empty diff against the base.
- Private validation: not run. The coordinator owns the PR.
