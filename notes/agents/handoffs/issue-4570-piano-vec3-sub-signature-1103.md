# Handoff: issue-4570-piano-vec3-sub-signature-deadref-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage and session: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-piano-vec3-sub-signature-deadref-1103b`; producer; `codex-pianovec-deadref-1103b` (Codex).
- Source branch and input commit: producer branch recorded in the queue; input `545d18fa76b1806d55b2054252b38a54c5a807e0`.
- Original source base SHA and workflow/tool SHA: both `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`. Active source-review policy workflow: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none; source and handoff are included.
- Next action, responsible role and blockers: verifier should independently review the source and rerun TU, relocation and current-base checks. Integrator can compose after acceptance. No known blocker.
- Status: verified candidate. Independent source review is pending.
- Remaining uncommitted/local-only material: private queue receipts and compiler/build outputs remain ignored under `build/`.

## What changed and why

- Class/TU/symbol and ROM scope: `ov063/daPiano_c`, source `src/game/actors/d_a_piano.cpp`, `.text` range `[0x0211d4b8, 0x0211e1c0)`.
- Corrected the local `Vec3_Sub` declaration and two calls in `IsTargetMoving`/`StateSearch` to match `src/Vec3_Sub.c`: `Vector3 *out, const Vector3 *a, const Vector3 *b`.
- Kept the existing POD scratch values and cast at the helper boundary. No header, manifest, config, or symbol changed.
- This is a declaration correction; it does not claim an original local variable spelling or new field layout.
- Compiler experiment: the corrected declaration and casts preserve the complete emitted TU.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 17 manifest functions match; objisolate and relocation destinations are clean; expected emission order; exact compiler-only output. The TU verifier reports existing local `__sinit_d_a_piano.cpp` as unlicensed, so promotion remains refused.
- Genuine methods and remaining bridges: unchanged.
- Recovered layout/fields: unchanged; local POD scratch remains.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged; the unlicensed `__sinit` remains an explicit limitation.
- Attribution: no function moves or renames.
- Remaining issue scope: continue evidence-backed reconstruction for this class; no additional field or naming claims were made here.

## Proof

- `python tools/tubuild.py verify ov063/daPiano_c --no-write`: exit 0; 17/17 MATCH, relocation-aware comparison, objisolate clean, relocation destinations clean, expected ROM-ascending order. One unlicensed local `__sinit` remains; promotion refused.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, zero mismatches; ROM-build analysis PASS.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_piano.cpp`: exit 0; zero findings using clang-format 19.1.7.
- `python tools/port_refcheck.py`: exit 0; 217 references resolve. No path or port reference changed.
- `git diff --check`: clean before commit.
- `python tools/prepush_linkcheck.py --range origin/main..HEAD`: exit 0; 18 checked, 17 verified, 1 BLIND-6 __sinit warning, 0 blockers. `python tools/check_decl_agreement.py --changed origin/main`: exit 0; no new declaration disagreements or local header redeclarations.
- Shared-header consumer expansion: not applicable; no shared header changed.
- Private validation: not yet run; PR validation is pending after publication.


This successor changes only handoff metadata; source code is unchanged from tested source commit 545d18fa76b1806d55b2054252b38a54c5a807e0.
