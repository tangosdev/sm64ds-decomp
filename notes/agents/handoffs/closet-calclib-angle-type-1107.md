# Handoff: closet-calclib-angle-type-1107

## Identity and resumption

- Work item: type the closet call to `Vec3_HorzAngle` against its shared declaration.
- Base: `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7` (`origin/main`).
- Candidate: the branch attached to draft PR #4818.
- Next action: independent verifier reviews the POD-to-Vector3 cast at the helper boundary and reruns focused/link/full-build checks on the pushed head. Source acceptance review remains outstanding.

## What changed and why

`src/Vec3_HorzAngle.cpp` defines `Vec3_HorzAngle(const Vector3 *, const Vector3 *)`, and `include/math/CalcLib.h` declares the same API. `d_a_obj_closet.cpp` had a local C declaration accepting `Vec3Raw *`, so it disagreed with the callee's shared signature. The local `Vec3Raw` scratch objects stay POD because making these locals `Vector3` adds destructor code and changes `Behavior` from the retail 0x3e0 size to 0x3f0. The edit includes `CalcLib.h`, removes the local conflicting declaration, and casts the two contiguous triples to `const Vector3 *` only at the call. No object layout or scratch-local lifetime changed.

## Proof on base `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`

- `python tools/tubuild.py verify ov058/daObjCloset_c --no-write --verdict`: 6/6 functions match; object isolation and relocation destinations clean; expected emission order. `daObjCloset_c` promotion verdict remains allowed.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_obj_closet.cpp`: 6 checked, 6 verified, 0 warnings, 0 blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact; 11,395/11,395 source-built functions exact; 470/470 source-owned data claims exact; zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements (rerun after final split before commit).
- `python tools/source_style.py --clang-format ...`: 0 findings; `python tools/port_refcheck.py`: 217/217 resolve; `python tools/check_dead_references.py`: no new dead refs or broken Markdown links.

## Separate C1 trap follow-up

A parallel experiment to correct `d_a_obj_c1_trap.cpp`'s local `Vec3_HorzAngle`/`Vec3_HorzDist` declarations matched all 17 functions in focused `tubuild`, with clean function relocations. It was excluded from this candidate because branch-wide `rombuild.py` refused to isolate `ov010/daObjC1_Trap_c`: defined symbol rename source `@807` has zero symbols. `prepush_linkcheck --files` also flags the existing unmanifested `__sinit_d_a_obj_c1_trap.cpp` as WRONG. Do not infer a production-safe change from the focused function results; investigate the `@807`/`__sinit` manifest/ownership issue in a separate work item before reattempting.
