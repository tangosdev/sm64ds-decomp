# Handoff: issue-4570-ct-rotate-block-addvec3-1103

## Identity and resumption

- Task: `issue-4570-ct-rotate-block-addvec3-1103`; producer session `codex-source-audit-1103-next`.
- Input/base: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`; workflow tools pinned to the same commit.
- Scope: `daObjCtRotateBlock_c`, `ov065/daObjCtRotateBlock_c`, ROM range `ov065:0x021196d8..0x02119f88`.
- Status: verified byte candidate; independent source review is the next queue stage.
- Next action: verifier should inspect the prototype/header choice and confirm the recorded object/relocation evidence on this exact committed candidate.
- Remaining local-only material: ignored build reports and queue receipts in `build/`.

## What changed and why

- Replaced the file-local `AddVec3(void *, void *, void *)` declaration with the existing `CalcLib.h` declaration, `AddVec3(Vector3 *, Vector3 *, Vector3 *)`.
- The implementation in `src/AddVec3.c` defines that same signature. `Behavior` passes `Vector3` objects (`vb`, `mPos`, `vb`), so no call-site cast or layout change is needed.
- The shared header changes MWCC's TU-local registration-node labels. The six `@NNN` manifest aliases were re-keyed by source construction order and checked against the emitted `.init` relocation order and their retained retail destinations.
- No `Vector3` object shape, field offset or static object's construction order changed.

## Proof

- Baseline before the edit: TU `11/11` exact, clean object isolation and relocation destinations; one unlicensed `__sinit_d_a_obj_ct_rotate_block.cpp` section already present.
- Candidate `python tools/tubuild.py verify ov065/daObjCtRotateBlock_c --no-write --verdict`: `11/11` exact, clean object isolation, relocation destinations and ROM-ascending emission order. The same pre-existing unlicensed initializer remains; promotion is not claimed.
- `python tools/rombuild.py -j16 --no-rom`: `106/106` modules exact, `11,395/11,395` functions and `468/468` initialized-data claims reproduce, zero mismatches; symbol check passes.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or local redeclarations.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_obj_ct_rotate_block.cpp`: zero findings.
- `git diff --check`: clean.
- `prepush_linkcheck` must be rerun after the candidate commit; it was not meaningful before commit because its range contained no changed files.

Byte identity establishes the source output only. The verifier should separately assess whether using the shared declaration and preserving the class's existing initialization scaffold are source-acceptable.
