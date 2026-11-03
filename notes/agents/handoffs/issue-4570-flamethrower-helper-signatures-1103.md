# Handoff: issue-4570-flamethrower-helper-signatures-deadref-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage and session: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-flamethrower-helper-signatures-deadref-1103b`; producer; `codex-flamevec-deadref-1103b` (Codex).
- Source branch and input commit: producer branch recorded in the queue; input `2d41c609e293b9cb35f8b159bcca386802d72556`.
- Original source base SHA and workflow/tool SHA: both `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`. Active source-review policy workflow: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none; source and handoff are included.
- Next action, responsible role and blockers: verifier should independently review the source and recheck the exact TU, relocations and base. Integrator can compose after acceptance. No known blocker.
- Status: verified candidate. Independent source review is pending.
- Remaining uncommitted/local-only material: private queue receipts and compiler/build outputs remain ignored under `build/`.

## What changed and why

- Class/TU/symbol and ROM scope: `ov095/daObjFlamethrower_c`, source `src/game/actors/d_a_obj_flamethrower.cpp`, `.text` range `[0x0213682c, 0x02136f58)`.
- Matched the local declarations to the implementations: `MulVec3Mat4x3(Vector3 *, Matrix4x3 *, Vector3 *)` in `src/MulVec3Mat4x3.c` and `Vec3_Add(Vector3 *, const Vector3 *, const Vector3 *)` in `src/Vec3_Add.c`. Updated call-boundary casts for local three-word scratch values, `mPosX`, and `mFlamePositions`.
- Kept local POD scratch arrays/structs to avoid introducing constructors into their lifetime; no shared header, config, manifest, or symbol changed.
- These are declaration and ABI corrections. They do not claim original local variable names or recover new object fields.
- Compiler experiment: the corrected declarations and boundaries preserve the full TU output under the pinned compiler.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 5 manifest functions match; objisolate and relocation destinations are clean; expected emission order. Compiler-only Vector3/Matrix4x3 constructor/destructor output is exact under the manifest's existing policy.
- Genuine methods and remaining bridges: unchanged.
- Recovered layout/fields: unchanged; local three-word scratch remains.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged.
- Attribution: no function moves or renames.
- Remaining issue scope: continue evidence-backed reconstruction for this class; no additional layout or naming claims were made here.

## Proof

- `python tools/tubuild.py verify ov095/daObjFlamethrower_c --no-write`: exit 0; 5/5 MATCH, relocation-aware comparison, objisolate clean, relocation destinations clean, expected ROM-ascending emission order, exact compiler-only output.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, zero mismatches; ROM-build analysis PASS.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_obj_flamethrower.cpp`: exit 0; zero findings using clang-format 19.1.7.
- `python tools/port_refcheck.py`: exit 0; 217 references resolve. No path or port reference changed.
- `git diff --check`: clean before commit.
- `python tools/prepush_linkcheck.py --range origin/main..HEAD`: exit 0; 5 checked, 4 verified, 1 BLIND-1 classInit warning, 0 blockers. `python tools/check_decl_agreement.py --changed origin/main`: exit 0; no new declaration disagreements or local header redeclarations.
- Shared-header consumer expansion: not applicable; no shared header changed.
- Private validation: not yet run; PR validation is pending after publication.


This successor changes only handoff metadata; source code is unchanged from tested source commit 2d41c609e293b9cb35f8b159bcca386802d72556.
