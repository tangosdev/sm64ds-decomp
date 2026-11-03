# Handoff: issue-4570-wanwan-vec3-signature-1103b

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-wanwan-vec3-signature-1103b`; producer session `codex-audit-1103-wanwan` (Codex).
- Source branch and previous accepted input SHA: [feature branch](https://github.com/tangosdev/sm64ds-decomp/tree/audit/wanwan-vec3-sub-signature-1103); `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Original source base SHA and installed workflow/tool SHA: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`; `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none.
- Next action, responsible role and blockers: independent verifier reviews the exact source and proof, including source plausibility, then publishes structured evidence; PR creation follows accepted verification.
- Status: verified candidate for the declaration correction; independent verification and hosted CI remain pending.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daWanwan_c`, `ov014/daWanwan_c`, `src/game/actors/d_a_wanwan.cpp`; affected functions `func_ov014_02111fe0` (`0x02111fe0..0x02112114`), `func_ov014_021122dc` (`0x021122dc..0x0211236c`), `func_ov014_0211236c` (`0x0211236c..0x0211250c`), and `func_ov014_0211250c` (`0x0211250c..0x02112788`).
- Reserved source/header/config surfaces actually touched: this source file only; the task reserves the class, file, handoff, and all four function ranges.
- ROM observations: the shared `Vec3_Sub` definition in `src/Vec3_Sub.c` takes `Vector3 *` output and `const Vector3 *` inputs. All four affected function bodies remain byte-identical after correcting the declaration and typing field-storage boundaries.
- Lineage evidence or structural inference: all `Vec3_Sub` call sites use vector objects except the actor's contiguous `mPosX`/`mPosY`/`mPosZ` fields and spawn-position fields, where the source retains explicit pointer casts at the boundary.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: the corrected signature made MWCC reject `&c->mPosX` as `int *`; explicitly casting this contiguous three-word field storage to `const Vector3 *` restores the call without changing emitted bytes.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: full TU comparison reports 29/29 manifest functions MATCH, clean object isolation, clean relocation destinations, and expected emission order. Four changed functions also pass direct relocation-aware linked-byte checks with zero blind relocations.
- Genuine methods; remaining free-function/ABI bridges: `Vec3_Sub` remains a shared C-linkage free function; this change corrects its local declaration only.
- Recovered layout/fields; remaining shadow structs/raw offsets: actor storage layout is unchanged; the explicit cast documents the existing contiguous XYZ field view without claiming a new field-layout recovery.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged.
- Attribution preserved through each move/rename: no move or rename.
- Remaining agreed issue scope: independent verification and source review.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 106/106 modules exact, zero mismatching functions or source-owned data claims.
- Explicit function/consumer relocation checks: direct `linkcheck.py` on all four affected functions, all VERIFIED with zero blind relocations; `prepush_linkcheck.py --files src/game/actors/d_a_wanwan.cpp`, 28 verified, two blind-symbol warnings (`daWanwan_c_classInit`, `__sinit_d_a_wanwan.cpp`), zero blocking. The initializer/factory warnings are outside this declaration change.
- Complete emitted TU and data/metadata checks: `tubuild.py verify ov014/daWanwan_c --no-write` reports 29/29 MATCH, clean object isolation and relocation destinations, and expected section order. It exits 1 because the existing TU emits an unlicensed `.text __sinit_d_a_wanwan.cpp` section absent from the manifest; the same extra section and 29/29 result were present before this change.
- Shared-header consumer expansion: no shared header changed.
- Port/path/reference and other applicable static gates: `check_decl_agreement.py --changed origin/main`, no new declaration disagreements or local header redeclarations; `source_style.py --base origin/main --files src/game/actors/d_a_wanwan.cpp`, zero findings and zero formatting deferrals; `git diff --check` clean.
- Private validation, if run, and the exact PR head/base it tested: no PR exists yet. Local proof used source base `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
