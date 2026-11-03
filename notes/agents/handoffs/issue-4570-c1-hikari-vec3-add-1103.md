# Handoff: issue-4570-c1-hikari-vec3-add-deadref-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage and session: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-c1-hikari-vec3-add-deadref-1103b`; producer; `codex-c1hikari-deadref-1103b` (Codex).
- Source branch and input commit: producer branch recorded in the queue; input `db2542e772592f82d28d2c2610d22dc1c989f1dd`.
- Original source base SHA and workflow/tool SHA: both `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`. Active source-review policy workflow: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none; source and handoff are included.
- Next action, responsible role and blockers: verifier should independently review and verify this exact candidate; integrator can compose after acceptance. No known blocker.
- Status: verified candidate. Independent source review is pending.
- Remaining uncommitted/local-only material: private queue receipts and compiler/build outputs remain ignored under `build/`.

## What changed and why

- Class/TU/symbol and ROM scope: `ov010/daObjC1Hikari_c`, source `src/game/actors/d_a_obj_c1_hikari.cpp`, contiguous code range `[0x021119d0, 0x02111e10)`.
- Changed the local `Vec3_Add` declaration and both calls in `Behavior` to use the shared function's actual ABI: `Vector3 *out, const Vector3 *a, const Vector3 *b`. Removed the now-unused `struct Vec3` forward declaration. The Hikari scratch triples and casts stay local because constructing `Vector3` locals would introduce its destructor into this TU.
- No header, config, manifest, or symbol changes. This is a local declaration correction, not a claim that the spelling or scratch layout is original EAD source.
- ROM observations and naming: none newly inferred. Existing class identity and TU boundary are unchanged.
- Compiler experiment: the signature correction retains the exact emitted class functions and object checks below.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 8 manifest functions match; `objisolate` and relocation-destination checks are clean; emission order matches the ROM. The TU verifier also reports the existing local `__sinit_d_a_obj_c1_hikari.cpp` as unlicensed, so this does not complete metadata/data ownership or make the TU promotion-ready.
- Genuine methods and remaining bridges: unchanged.
- Recovered layout/fields: unchanged; local `HikariVec3` POD scratch views remain.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged; the unlicensed initializer remains an explicit limitation.
- Attribution: no function moves or renames.
- Remaining issue scope: continue evidence-backed source reconstruction for this class; no additional field or naming claims were made here.

## Proof

- `python tools/tubuild.py verify ov010/daObjC1Hikari_c --no-write`: exit 0; 8/8 MATCH, relocation-aware comparison, objisolate clean, relocation destinations clean, expected ROM-ascending function order. The one unlicensed local `__sinit` remains and promotion is refused.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, zero mismatches; ROM-build analysis PASS. The candidate-specific function and relocation evidence is the explicit TU check above.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_obj_c1_hikari.cpp`: exit 0; zero findings using clang-format 19.1.7.
- `python tools/port_refcheck.py`: exit 0; 217 references resolve. No path or port reference changed.
- `git diff --check`: clean before commit.
- `python tools/prepush_linkcheck.py --range origin/main..HEAD`: exit 0; 9 verified, 1 BLIND-2 __sinit warning, 0 blockers. `python tools/check_decl_agreement.py --changed origin/main`: exit 0; no new declaration disagreements or local header redeclarations.
- Shared-header consumer expansion: not applicable; no shared header changed.
- Private validation: not yet run; PR validation is pending after publication.


This successor changes only handoff metadata; source code is unchanged from tested source commit db2542e772592f82d28d2c2610d22dc1c989f1dd.
