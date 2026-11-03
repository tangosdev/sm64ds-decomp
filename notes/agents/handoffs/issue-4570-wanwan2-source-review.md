# Handoff: issue-4570-wanwan2-vec3-signature-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-wanwan2-vec3-signature-1103b`; producer session `codex-audit-1103-wanwan2` (Codex).
- Source branch and previous accepted input SHA: [feature branch](https://github.com/tangosdev/sm64ds-decomp/tree/audit/wanwan2-vec3-sub-signature-1103); `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Original source base SHA and installed workflow/tool SHA: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`; `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none.
- Next action, responsible role and blockers: independent verifier reviews the exact source and proof, including source plausibility, then publishes structured evidence; PR creation follows accepted verification.
- Status: verified candidate for the local declaration correction; independent verification and hosted CI remain pending.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daWanwan2_c`, `ov100/daWanwan2_c`, `src/game/actors/d_a_wanwan2.cpp`; `func_ov100_021437d4` (`0x021437d4..0x02143aa4`) plus `daWanwan2_c::Behavior` (`0x02143d64..0x02144088`).
- Reserved source/header/config surfaces actually touched: this source file only; the task reserves the class, source, handoff, and affected function range.
- ROM observations: the shared definition in `src/Vec3_Sub.c` takes `Vector3 *` output and `const Vector3 *` inputs. The two affected functions remain byte-identical with that signature.
- Lineage evidence or structural inference: `func_ov100_021437d4` deliberately uses local three-word `Vec3i` POD values for chain position and velocity. Their layout is passed to the shared vector helper through explicit casts at the call boundary; the member-layout interpretation is unchanged.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: no code-generation barrier. The explicit casts keep the local POD storage while making the call agree with the actual shared function signature.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: full TU reports 17/17 manifest functions MATCH, clean object isolation and relocation destinations, expected emission order. Both affected functions are separately VERIFIED by relocation-aware linkcheck with zero blind relocations.
- Genuine methods; remaining free-function/ABI bridges: `Vec3_Sub` remains a shared C-linkage free function; only the local declaration and POD call boundary changed.
- Recovered layout/fields; remaining shadow structs/raw offsets: the three-word `Vec3i` locals remain a local POD view; no actor layout claim changed.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged.
- Attribution preserved through each move/rename: no move or rename.
- Remaining agreed issue scope: independent verification and source review.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 106/106 modules exact, zero mismatching functions or source-owned data claims.
- Explicit function/consumer relocation checks: `linkcheck.py` for `func_ov100_021437d4` and `_ZN11daWanwan2_c8BehaviorEv`, both VERIFIED with zero blind relocations; `prepush_linkcheck.py --files src/game/actors/d_a_wanwan2.cpp`, 16 verified, two blind-symbol warnings (`daWanwan2_c_classInit`, `__sinit_d_a_wanwan2.cpp`), zero blocking.
- Complete emitted TU and data/metadata checks: `tubuild.py verify ov100/daWanwan2_c --no-write` reports 17/17 MATCH, clean object isolation and relocation destinations, expected section order. It exits 1 because the existing unlicensed `.text __sinit_d_a_wanwan2.cpp` section is absent from the manifest; that same section and 17/17 result were present before this change.
- Shared-header consumer expansion: no shared header changed.
- Port/path/reference and other applicable static gates: `check_decl_agreement.py --changed origin/main`, no new disagreements or header redeclarations; `source_style.py --base origin/main --files src/game/actors/d_a_wanwan2.cpp`, zero findings and zero formatting deferrals; `git diff --check` clean.
- Private validation, if run, and the exact PR head/base it tested: no PR exists yet. Local proof used source base `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
