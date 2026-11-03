# Handoff: pr-4718-hanachan-source-review2

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4718; `pr-4718-hanachan-source-review2`; source producer `codex-audit-1103-pr4718` (Codex).
- Source branch and previous accepted input SHA: [feature branch](https://github.com/tangosdev/sm64ds-decomp/tree/audit/hanachan-vec3-sub-signature-1103); `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Original source base SHA and installed workflow/tool SHA: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`; `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none.
- Next action, responsible role and blockers: independent source plausibility review of the exact PR head/base, then structured source-review evidence; role `humanizer`.
- Status: verified candidate for the signature change; independent source review and hosted CI are pending.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daHanachan_c`, `ov034/daHanachan_c`, `src/game/actors/d_a_hanachan.cpp`; three `Vec3_Sub` call sites.
- Reserved source/header/config surfaces actually touched: this source file only.
- ROM observations: changing the local function declaration and adding explicit argument casts preserves all three functions' linked ROM bytes and relocation destinations.
- Lineage evidence or structural inference: `Vec3_Sub`'s shared declaration uses `Vector3 *` output and `const Vector3 *` inputs; existing integer-array and member storage remain unchanged.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: none; no constructor-bearing object view was introduced.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: `StateMoveMain`, `StateDemoWaitMain`, and `UpdateSegments` are VERIFIED with zero blind relocations; complete TU verification reports 35/35 matches and clean relocation destinations.
- Genuine methods; remaining free-function/ABI bridges: `Vec3_Sub` remains a shared free function; this change corrects only its local declaration.
- Recovered layout/fields; remaining shadow structs/raw offsets: existing Hanachan layout is unchanged; this patch does not claim additional layout recovery.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged.
- Attribution preserved through each move/rename: no move or rename.
- Remaining agreed issue scope: independent source review and hosted CI for this PR.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 106/106 modules exact, 0 mismatching functions or source-owned data claims.
- Explicit function/consumer relocation checks: `linkcheck.py` on the three changed functions, all VERIFIED with zero blind relocations; `prepush_linkcheck.py --files src/game/actors/d_a_hanachan.cpp`, 34 verified, 2 pre-existing blind-symbol warnings (`daHanachan_c_classInit`, `__sinit_d_a_hanachan.cpp`), 0 blocking.
- Complete emitted TU and data/metadata checks: `tubuild.py verify ov034/daHanachan_c --no-write`, 35/35 functions matched, relocation destinations clean.
- Shared-header consumer expansion: no shared header changed.
- Port/path/reference and other applicable static gates: `check_decl_agreement.py --changed origin/main`, no new disagreement or local redeclaration; `git diff --check` clean. `source_style.py` could not run locally because clang-format 19.1.7 is unavailable.
- Private validation, if run, and the exact PR head/base it tested: local proofs tested the code change on base `76e2...` before adding this documentation. GitHub PR #4718 head is pending hosted validation; the source itself is unchanged by this handoff file.
