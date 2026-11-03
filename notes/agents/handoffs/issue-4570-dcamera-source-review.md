# Handoff: issue-4570-dcamera-vec3-signature-1103c

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-dcamera-vec3-signature-1103c`; producer session `codex-audit-1103-dcamera` (Codex).
- Source branch and previous accepted input SHA: [feature branch](https://github.com/tangosdev/sm64ds-decomp/tree/audit/dcamera-vec3-sub-signature-1103); `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Original source base SHA and installed workflow/tool SHA: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`; `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none.
- Next action, responsible role and blockers: independent verifier reviews the exact source and proof, including source plausibility, then publishes structured evidence; PR creation follows accepted verification.
- Status: verified candidate for the local declaration correction; independent verification and hosted CI remain pending.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `dCamera_c`, `arm9/dCamera_c`, `src/engine/view/d_camera.cpp`; the complete reserved TU range is `0x020072c4..0x0200e494`. The affected functions are `func_0200c394` (`0x0200c394..0x0200c66c`), `func_0200bec4` (`0x0200bec4..0x0200c394`), and `func_020089f8` (`0x020089f8..0x02008a70`).
- Reserved source/header/config surfaces actually touched: this source file only; the task reserves the camera class, source file, handoff, and full TU range.
- ROM observations: the shared `Vec3_Sub` definition in `src/Vec3_Sub.c` takes `Vector3 *` output and `const Vector3 *` inputs. All 142 manifest functions remain byte-identical.
- Lineage evidence or structural inference: two old handlers use local three-word POD `Vec3` scratch values, and one uses `int[3]` output plus contiguous camera vector components. MWCC rejected these three calls against the typed shared signature; casts at those call boundaries preserve the local storage representations.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: direct MWCC diagnostics identified exactly three incompatible calls (lines 1355, 1491 and 3466 before formatting). The explicit pointer casts compile without changing emitted instructions.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: full TU comparison reports 142/142 manifest functions MATCH, clean object isolation and relocation destinations, expected emission order. The complete relocation-aware file check reports 143/143 VERIFIED, zero blind relocations and zero warnings.
- Genuine methods; remaining free-function/ABI bridges: `Vec3_Sub` remains a shared C-linkage free function; only the local declaration and three call boundaries changed.
- Recovered layout/fields; remaining shadow structs/raw offsets: legacy local `Vec3` POD scratch and the `int[3]` result remain unchanged; no camera object layout claim changed.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged.
- Attribution preserved through each move/rename: no move or rename.
- Remaining agreed issue scope: independent verification and source review.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 106/106 modules exact, zero mismatching functions or source-owned data claims.
- Explicit function/consumer relocation checks: `linkcheck.py` for all three changed functions, each VERIFIED with zero blind relocations; `prepush_linkcheck.py --files src/engine/view/d_camera.cpp`, 143 verified, zero warnings, zero blocking.
- Complete emitted TU and data/metadata checks: `tubuild.py verify arm9/dCamera_c --no-write` reports 142/142 MATCH, clean object isolation and relocation destinations, and expected section order. It exits 1 because `.text __sinit_d_camera.cpp` is an existing unlicensed section absent from the manifest; the same section was present in the pre-change baseline.
- Shared-header consumer expansion: no shared header changed.
- Port/path/reference and other applicable static gates: `check_decl_agreement.py --changed origin/main`, no new disagreements or header redeclarations; `source_style.py --base origin/main --files src/engine/view/d_camera.cpp`, zero findings and zero formatting deferrals; `check_dead_references.py` and `git diff --check` clean.
- Private validation, if run, and the exact PR head/base it tested: no PR exists yet. Local proof used source base `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
