# Handoff: issue-4570-propeller-heyho-vec3-dist-type-1105

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-propeller-heyho-vec3-dist-type-1105`.
- Candidate base: `0bab793a13bf137418b5078adb030359b9cfbeb4` (`origin/main`).
- Source: `src/game/actors/d_a_propeller_heyho.cpp`, TU `ov070/daPropeller_Heyho_c`.
- Status: producer verification complete; independent source review and hosted PR review remain.

## What changed and why

The three local `Vec3_Dist` declarations now agree with the implementation signature, `int Vec3_Dist(const Vector3 *, const Vector3 *)`. Calls use `mPos`, the new `mHomePos` view over the class's existing `mHomePosX/Y/Z` triple, and local `Vector3` values (`t` and `vb`) directly. `Vec3_HorzAngle` and the opaque `Vec3_VertAngle` call also use the home-position view. `CalcLib.h` remains excluded locally because the TU's `Vec3_VertAngle` declaration is still opaque. The union retains the existing offsets and component names; the pinned compiler still reproduces every function. Its extra empty union-destructor thunk is explicitly recorded as deadstrip output, and the seven generated registration-node names shifted by the two anonymous aggregate slots are mapped to their existing fleet-unique aliases in the TU manifest.

## Verification on the candidate

- `python tools/tubuild.py verify ov070/daPropeller_Heyho_c --no-write --verdict`: 27/27 functions match; object isolation, relocation destinations, and ROM emission order are clean. The Vector3 destructor and class-union cleanup thunk are licensed as compiler-only deadstrip output. Nine unrelated pre-existing TU sections/symbols remain unlicensed (`__sinit_d_a_propeller_heyho.cpp` and RTTI/typeinfo data), so TU promotion is refused by policy.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or local redeclarations.
- `python tools/check_header_offsets.py include/daPropeller_Heyho_c.h`: 14 commented fields, 0 mismatches, struct remains `0x3e8`.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/source_style.py --base origin/main --clang-format <pinned clang-format>`: 1 file, 0 findings.
- `git diff --check`: clean.

## Review handoff

Independently confirm the scalar home-position triple is a valid three-word vector view at each changed call and that `mPos` is the class's intended position view. Re-run focused TU verification and the full stock ROM build on the exact PR candidate. Preserve the documented unlicensed `__sinit`/RTTI policy caveat; the function bytes and relocation checks pass, but the TU is not promotable under the current manifest policy.
