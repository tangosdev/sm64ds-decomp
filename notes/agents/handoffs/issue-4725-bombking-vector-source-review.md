# Handoff: issue-4725-bombking-calclib-signatures-1103

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4725; `issue-4725-bombking-calclib-signatures-1103`; producer; `codex-bombking-calclib-1103a`; Codex.
- Source branch and previous accepted input SHA: producer branch stacked on the Sanbo candidate; `360341c22f85bd4ae2ee5bd574a0a5c6098e66e0`.
- Original source base SHA and installed workflow/tool SHA: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`; `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none; exact commands and results are recorded here.
- Next action, responsible role and blockers: An independent verifier should accept this exact commit, inspect the helper types and casts, then rerun full-TU relocation and full-ROM checks. The PR is stacked on the Sanbo draft and should be retargeted to `main` after that base is accepted and merged.
- Status: verified candidate; independent source review is pending.
- Remaining uncommitted/local-only material and where it is preserved: baseline/candidate objects and compiler/build logs are under the ignored worktree `build/` directory; no source changes remain uncommitted.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daBombking_c`, `ov078/daBombking_c`, full `.text` range `ov078:34748224..34760288`.
- Reserved source/header/config surfaces actually touched: `src/game/actors/d_a_bombking.cpp`; the TU manifest was reserved but did not need edits.
- ROM observations: `include/math/CalcLib.h` declares `Vec3_HorzAngle`, `Vec3_Dist`, `Vec3_HorzLen`, `RandomIntInternal`, and `ApproachAngle` with shared types. The local declarations erased or contradicted some of those signatures. Calls that pass actor position triples or local three-word views now cast at the helper boundary. `HorzAngleToCPlayer()` returns `s16`; the local angle in `func_ov078_02124000` now keeps that type.
- Lineage evidence or structural inference: shared helper types come from `CalcLib.h` and the existing `dActor_c` declaration. No actor fields or object layout changed.
- Hypothesized names/filenames, explicitly not recovered facts: none introduced.
- Compiler experiments and measured barriers: the first typed compile isolated one codegen difference in `func_ov078_02124000` when an `int` local fed the shared `short` parameter. Changing the local to `s16` restored the exact 0x60-byte method. The complete pinned TU then matched 52/52 functions.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 52/52 TU functions match; object isolation and relocation destinations are clean. `prepush_linkcheck` reports 52 VERIFIED, one separately configured `__sinit_d_a_bombking.cpp` BLIND-15 warning, and zero blockers.
- Genuine methods; remaining free-function/ABI bridges: no method conversion was attempted; current method and ABI identities are unchanged.
- Recovered layout/fields; remaining shadow structs/raw offsets: no layout changes; existing position triples and local vector views remain, now with explicit vector-pointer casts at helper boundaries.
- Lifecycle, vtable/RTTI, initializer and data ownership: preserved. Full-ROM intact-TU checks report zero new symbol errors and exact storage aliases.
- Attribution preserved through each move/rename: no source moves, symbol renames or attribution changes.
- Remaining agreed issue scope: independent review of this focused shared-helper type correction; broader Bomb King reconstruction remains outside this task.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 106/106 modules exact, 11,395 source functions reproducing, zero mismatches. ROM data check: 2,200 symbols verified, 690 partial, zero differing, zero unnamed by config. Intact-TU gates passed with zero new symbol errors and exact storage aliases.
- Explicit function/consumer relocation checks: `python tools/prepush_linkcheck.py --files src/game/actors/d_a_bombking.cpp`; 52/52 functions VERIFIED, one BLIND-15 initializer warning, zero blockers.
- Complete emitted TU and data/metadata checks: `python tools/tubuild.py verify ov078/daBombking_c --no-write`; 52/52 MATCH, object isolation clean, relocation destinations clean, expected emission order. The checker reports the separately recorded initializer as an extra `.text` symbol; `rombuild` and prepush checks independently cover the configured initializer.
- Shared-header consumer expansion: no shared header changed. `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements and no new local redeclarations of header-declared symbols.
- Port/path/reference and other applicable static gates: `python tools/source_style.py --base 360341c22f85bd4ae2ee5bd574a0a5c6098e66e0 --files src/game/actors/d_a_bombking.cpp` reports zero findings; `python tools/check_dead_references.py` reports no new dead references or broken links; `git diff --check` passes. No paths or exported symbol names changed.
- Private validation, if run, and the exact PR head/base it tested: not run; this commit is intended for stacked draft review and hosted validation.
