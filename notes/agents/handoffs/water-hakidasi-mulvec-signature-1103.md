# Handoff: issue-4570-water-hakidasi-mulvec-signature-1103

## Identity and resumption

- Task: `issue-4570-water-hakidasi-mulvec-signature-1103`; producer session `codex-source-audit-1103-next2`.
- Input/base and workflow pin: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`.
- Scope: `daWater_Hakidasi_c`, `ov064/daWater_Hakidasi_c`, ROM range `ov064:0x02119330..0x02119a58`.
- Status: verified byte candidate; independent source review is the next queue stage.
- Next action: verifier should review the signature correction and call-boundary casts on this committed candidate.
- Remaining local-only material: ignored compiler reports and queue receipts under `build/`.

## What changed and why

- Corrected the local C-linkage declaration of `MulVec3Mat4x3` to match its definition in `src/MulVec3Mat4x3.c`: `Vector3 *`, `Matrix4x3 *`, `Vector3 *`.
- `include/math/MtxStack.h` does not yet declare this function, so the file-local declaration remains with an explicit reason.
- `State0` uses local three-int `Vec3` scratch records for input and output. Casts now mark those existing POD views at the helper boundary; the shared matrix global already has type `Matrix4x3`.
- The local scratch layout, function behavior and storage were not changed.

## Proof

- Baseline before edit: TU `12/12` exact; object isolation, relocation destinations and ROM-ascending emission order clean. One unlicensed `__sinit_d_a_water_hakidasi.cpp` section was already present.
- Candidate `python tools/tubuild.py verify ov064/daWater_Hakidasi_c --no-write --verdict`: `12/12` exact, object isolation and relocation destinations clean, expected emission order; the same unlicensed initializer remains, so no TU promotion is claimed.
- `python tools/rombuild.py -j16 --no-rom`: `106/106` modules exact, `11,395/11,395` functions and `468/468` initialized-data claims reproduce, zero mismatches; symbol check passes.
- `python tools/check_decl_agreement.py --changed origin/main`: no new disagreements or local header redeclarations.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_water_hakidasi.cpp`: zero findings.
- `prepush_linkcheck` must be run after the candidate commit; it was not meaningful before commit because the range contained no changed files.
- Dead-reference and TU-conflict checks remain to run on the completed handoff.

Byte identity establishes compiler output only. The independent reviewer should separately assess whether the POD-view boundary and file-local declaration are acceptable source reconstruction.
