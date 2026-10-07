# Handoff: mgread-cup-1007

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Continuation of [issue #2492](https://github.com/tangosdev/sm64ds-decomp/issues/2492), predecessor `issue-2492-cup-source-review`; producer session `codex-mgread-cup-1007`, Codex.
- Branch `readability/shell-game-1007`; source base, accepted input and installed tooling `f1333dd9a366eac154d404cad8d552f0270a0ccc`.
- Owned class `dScMgCup_c`, ov006 text `[0x020de988, 0x020e0638)`, all 32 functions in the existing production manifest.
- Changed surfaces: `src/actors/dScMgCup_c.cpp`, `include/dScMgCup_c.h`, and this handoff. No manifest, symbol, attribution, enrollment or data ownership change.
- Status: locally verified readability candidate. Root owns combined full-ROM validation and independent reviewer handoff. No PR or merge is authorized by this task.

## Readability changes

Name four existing byte fields from their observed uses: `mSwapsRemaining` counts completed swaps down, `mRoundRow` indexes the round table, `mCupContents` supplies the per-cup render mode and selection comparison, and `mTargetContent` supplies the requested content. These are descriptive reconstructed identifiers, not recovered original names; field types and offsets are unchanged.

The effect update helper `func_ov006_020ded00` now reads and writes `CupFx` position, velocity, row, delay, frame and active fields directly. Its frame reload retains the byte view. Typing only the decrement still reproduces the complete original object; the previously measured shared frame spelling remains a separate compiler limitation. The helper's existing integer parameter ABI is unchanged.

The initial four field renames plus typed non-frame accesses, then the typed frame decrement, and final prose polish all produced the same complete object as the fresh baseline. The odd `Render` index `mFrame[k]` is preserved exactly. Existing state-machine addressing, manual lifecycle calls and helper interfaces remain.

## Proof

Pinned compiler `2004/b56`, production flags from `build_pin.flags_for` for the existing `//cpp` TU. Private artifacts live in `build/cup-proof/`; the reproducible local runner is `build/cup-proof.py`.

- Fresh baseline and final full ELF objects: **22,056 bytes, byte-identical**, SHA-256 `777dbb9b72e9e30a39ad032bc99a0c1f4e746a3a7f3eaeb7d29b61cb819525ab`. This includes all emitted code, data, relocations and compiler metadata; it does not introduce new ROM-data ownership claims.
- `python build/cup-proof.py baseline` and `python build/cup-proof.py final`: each **32/32 strict VERIFIED**, zero blind relocations or byte differences; exit 0. Each function is checked after the production isolation helper, with all 32 `objisolate.plan` calls independently confirmed error-free. An initial diagnostic passed the raw object directly and reported vtable-preamble differences in D1/D0/factory; correcting the runner to use the build's isolation removed those diagnostic differences. No source repair or symbol exception was used.
- `python tools/affected_src.py include/dScMgCup_c.h --json`: sole source consumer `src/actors/dScMgCup_c.cpp`; exit 0, `affected.json`.
- `python tools/check_decl_agreement.py --changed f1333dd9a366eac154d404cad8d552f0270a0ccc`: no new disagreements or local header redeclarations; exit 0, `declarations.txt`. Existing banked disagreements and two unparsed declarators remain; this is a ratchet pass, not universal declaration agreement.
- `python tools/port_refcheck.py`: 313 references resolve, zero stale; exit 0, `port.txt`. No file or symbol moves occur.
- `git diff --check`: exit 0.
- `binding.json` records final source hashes, the baseline/final object equality and all isolation plans succeeding.
- Full-ROM build, independent source acceptance, private CI and exact-base composition are pending coordinator/verifier work and are not claimed by these local checks.

## Inherited findings and remaining reconstruction

The [predecessor handoff](issue-2492-cup-source-review.md) remains the detailed disposition record. CUP-01, CUP-02, CUP-03-P, CUP-04, CUP-05, SOUND-PROV01, TERESA-PROV01, BASE-PROV01 and BASE-PROV02 retain their recorded fixed dispositions; this narrow continuation does not rework those other-class surfaces or claim to re-prove their full historical scope.

CUP-03 remains partial: this pass improves names and one existing typed effect view, while raw tail accesses and inherited unknown fields remain. CUP-06 remains partial: manual factory/lifecycle calls, component byte storage, runtime array helpers, mangled constructors and address-named helpers remain. SOUND-RECON01 remains partial and untouched. Their outstanding scope remains owned by issue #2492, with root coordinating subsequent work.

No method, constructor, vtable, RTTI, initializer or data ownership reconstruction is added here. The next action is independent review of this exact commit and the root's complete composed-ROM proof before any publication decision.
