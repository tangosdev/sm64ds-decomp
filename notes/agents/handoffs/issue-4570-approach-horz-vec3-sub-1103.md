# Handoff: issue-4570-approach-horz-vec3-sub-deadref-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage and session: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-approach-horz-vec3-sub-deadref-1103b`; producer; `codex-approachvec-deadref-1103b` (Codex).
- Source branch and input commit: producer branch recorded in the queue; input `09eb4bd4feb96a10fa8708c126f9ea1ff51be568`.
- Original source base SHA and workflow/tool SHA: both `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`. Active source-review policy workflow: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none; source and handoff are included.
- Next action, responsible role and blockers: verifier should review the local POD view and independently rerun TU, relocation and base checks. Integrator can compose after acceptance. No known blocker.
- Status: verified candidate. Independent source review is pending.
- Remaining uncommitted/local-only material: private queue receipts and compiler/build outputs remain ignored under `build/`.

## What changed and why

- TU/symbol and ROM scope: ARM9 `Vec3_ApproachHorz` translation unit, source `src/Vec3_ApproachHorz.cpp`, `.text` range `[0x0203b7dc, 0x0203b958)`.
- Corrected the local `Vec3_Sub` declaration to `Vector3 *out, const Vector3 *a, const Vector3 *b`, matching `src/Vec3_Sub.c`. `ApproachLinear` passes its file-local POD `Vector3` directly; `Vec3_ApproachHorz` casts its separate POD `Vec3` values at the helper boundary.
- Updated the file comment to describe which local POD view each helper declaration uses. The local POD definitions stay in place; no shared header or config changed.
- This records local ABI views and inferred implementation semantics, not recovered EAD local names.
- Compiler experiment: both functions retain exact emitted output.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: both manifest functions match; objisolate and relocation destinations are clean; expected emission order. The verifier reports existing local `__sinit_Vec3_ApproachHorz.cpp` as unlicensed, so promotion remains refused.
- Genuine methods and remaining bridges: unchanged.
- Recovered layout/fields: no object layout changes; two local three-word POD views remain.
- Lifecycle, initializer and data ownership: unchanged; existing unlicensed `__sinit` remains a limitation.
- Attribution: no function moves or renames.
- Remaining issue scope: continue evidence-backed helper and source type cleanup; no new field-name claims were made.

## Proof

- `python tools/tubuild.py verify arm9/Vec3_ApproachHorz --no-write`: exit 0; 2/2 MATCH, relocation-aware comparison, objisolate clean, relocation destinations clean, expected ROM-ascending order. One unlicensed local `__sinit` remains; promotion refused.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, zero mismatches; ROM-build analysis PASS.
- `python tools/source_style.py --base origin/main --files src/Vec3_ApproachHorz.cpp`: exit 0; zero findings using clang-format 19.1.7.
- `git diff --check`: clean before commit.
- `python tools/prepush_linkcheck.py --range origin/main..HEAD`: exit 0; 3 checked, 2 verified, 1 BLIND-1 __sinit warning, 0 blockers. `python tools/check_decl_agreement.py --changed origin/main`: exit 0; no new declaration disagreements or local header redeclarations.
- Shared-header consumer expansion and port path check: no shared header or file path changed.
- Private validation: not yet run; PR validation is pending after publication.


This successor changes only handoff metadata; source code is unchanged from tested source commit 09eb4bd4feb96a10fa8708c126f9ea1ff51be568.
