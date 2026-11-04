# Handoff: issue-4570-bakubaku-levelrng-type-1104

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-bakubaku-levelrng-handoff-1104`; producer; `codex-bakubaku-handoff1104`; Codex.
- Source branch and previous accepted input SHA: [published branch](https://github.com/tangosdev/sm64ds-decomp/tree/audit/bakubaku-levelrng1104); `5d22962282981edb085949a8caa3af7b0f907b46`.
- Original source base SHA and installed workflow/tool SHA: `58a8e3f21b84d3825030a2c4ea09c58f568756a3` for both.
- Separate evidence commits and required artifacts in this commit: none.
- Next action, responsible role and blockers: verifier to inspect source provenance and independently rerun the requested checks; integrator follows verification. No technical blocker.
- Status: verified byte candidate; independent source review and verification are pending.
- Remaining uncommitted/local-only material and where it is preserved: none intended; build reports/logs remain gitignored in this worktree.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daBakubaku_c`, `src/game/actors/d_a_bakubaku.cpp`, overlay `ov032`, class code `0x021111a0-0x021124a8`.
- Reserved source/header/config surfaces actually touched: only `src/game/actors/d_a_bakubaku.cpp`; `include/LevelRng.h` was already the shared declaration and was not changed.
- ROM observations: the linked RNG object is defined as `LevelRng` in `src/unnamed/arm9/0201/func_0201fec8.cpp`; this actor had declared it as `int[]`. `LevelRng` wraps the single integer seed; `RandomIntInternal` remains declared as accepting `int *`.
- Lineage evidence or structural inference: reused the existing shared `LevelRng` declaration and made the conversion explicit at the raw RNG helper boundary. This does not claim the local C++ spelling uniquely recovers EAD's exact original declaration.
- Hypothesized names/filenames, explicitly not recovered facts: none added.
- Compiler experiments and measured barriers: none beyond the required relocation and ROM builds; no barrier found.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 24 concrete `daBakubaku_c` functions passed relocation-aware linkcheck; all 11,395 source-built functions reproduced in the full build.
- Genuine methods; remaining free-function/ABI bridges: unchanged. `RandomIntInternal(int *)` remains a raw storage ABI.
- Recovered layout/fields; remaining shadow structs/raw offsets: this change only corrects the shared global object's type; unrelated partial actor layouts remain.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. Existing static initializer linkcheck is blind and reported as such.
- Attribution preserved through each move/rename: no source moves or attribution changes.
- Remaining agreed issue scope: continue retiring evidenced shadow types and correcting source contracts for this class as separately scoped work.

## Proof

All commands ran in this worktree against source base `58a8e3f21b84d3825030a2c4ea09c58f568756a3`.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 11,395 functions reproduced, 470 data claims and 302 BSS claims, 106/106 modules exact, zero source data differences. ROM image construction was skipped by `--no-rom`.
- Explicit function/consumer relocation checks: `python tools/prepush_linkcheck.py --files src/game/actors/d_a_bakubaku.cpp`, exit 0; 25 entries checked, 24 verified, zero blockers. The `__sinit_d_a_bakubaku.cpp` entry is `BLIND-5`, not claimed as verified.
- Complete emitted TU and data/metadata checks: the full source build compiled the TU and all its functions reproduced; no source-owned data changes.
- Shared-header consumer expansion: no header changed. The TU includes the existing `LevelRng.h`; declaration agreement `--changed origin/main` found no new local header redeclarations or declaration disagreements.
- Port/path/reference and other applicable static gates: not applicable; no paths, symbols, manifests, or port references changed. `git diff --check` passed. `source_style.py --base origin/main` passed for one file with clang-format 19.1.7, zero findings and zero deferrals.
- Private validation: not run; candidate PR validation pending after publication.
