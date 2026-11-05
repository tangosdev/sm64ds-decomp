# Handoff: ibl-calclib-vector-types-1107

## Identity and resumption

- Work item: align Ibl's CalcLib helper declarations and call boundaries with the shared vector API.
- Base: `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7` (`origin/main`).
- Next action: an independent verifier reviews the scalar-triple views and manifest rename update, then reruns focused and full-build checks on the pushed candidate. Independent source review is outstanding.

## What changed and why

`d_a_ibl.cpp` locally declared `Vec3_HorzLen`, `Vec3_HorzDist`, and `Vec3_Equal` with `void *` parameters, while their definitions and `math/CalcLib.h` use `Vector3` pointers. Include the shared header and use its declarations; cast at call boundaries for the actor's contiguous position/speed fields. Those actor fields remain scalars, and the existing `Vector3 pos` local remains unchanged. The casts do not add vector construction or alter object layout.

Including `CalcLib.h` changes six compiler temporary names in ov100's emitted object. The base labels `@569`, `@571`, `@573`, `@575`, `@577`, and `@579` now emit as `@564`, `@566`, `@568`, `@570`, `@572`, and `@574`, respectively. The manifest's `defined_symbol_renames` records were updated according to the measured object symbol table and same-section/same-size slots. This is required for production TU isolation; without it the intact-TU gate refuses the candidate.

## Proof on base `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`

- `python tools/tubuild.py verify ov100/daIbl_c --no-write --verdict`: 16/16 functions match; object isolation, relocation destinations, and emission order clean. The existing nine unlicensed compiler-only records still refuse promotion; this work does not attempt TU promotion.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_ibl.cpp`: 16 verified, one BLIND-2 warning for the existing unmanifested `__sinit_d_a_ibl.cpp`, zero blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; intact-TU gates pass, 106/106 modules exact, 11,395/11,395 source-built functions and 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new disagreements or local redeclarations of header-declared symbols.
- `python tools/source_style.py`: 0 findings; `python tools/port_refcheck.py`: 217/217 resolve; `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

The source change is byte-verified; existing unlicensed initializer/RTTI emissions remain a separate TU-promotion policy issue.
