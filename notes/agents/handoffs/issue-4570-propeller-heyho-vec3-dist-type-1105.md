# Handoff: issue-4570-propeller-heyho-vec3-dist-type-1105

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-propeller-heyho-vec3-dist-type-1105`.
- Candidate base: `0bab793a13bf137418b5078adb030359b9cfbeb4` (`origin/main`).
- Source: `src/game/actors/d_a_propeller_heyho.cpp`, TU `ov070/daPropeller_Heyho_c`.
- Status: producer verification complete; independent source review and hosted PR review remain.

## What changed and why

The three local `Vec3_Dist` declarations now agree with the implementation signature, `int Vec3_Dist(const Vector3 *, const Vector3 *)`. Calls use `mPos` where the actor's existing vector view represents the position, and local `Vector3` values (`t` and `vb`) are passed directly. `mHomePosX/Y/Z` remain scalar storage, so those call boundaries retain explicit three-word vector views. `CalcLib.h` remains excluded locally because its other `Vec3_VertAngle` declaration is still opaque in this TU. The arrival-distance block was formatted with the pinned formatter; no object layout or symbol ownership changed.

## Verification on the candidate

- `python tools/tubuild.py verify ov070/daPropeller_Heyho_c --no-write --verdict`: 27/27 functions match; object isolation, relocation destinations, and ROM emission order are clean. It reports nine pre-existing unlicensed TU sections/symbols (the `__sinit_d_a_propeller_heyho.cpp` text plus RTTI/typeinfo data), so TU promotion is refused by policy.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or local redeclarations.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/source_style.py --base origin/main --clang-format <pinned clang-format>`: 1 file, 0 findings.
- `git diff --check`: clean.

## Review handoff

Independently confirm the scalar home-position triple is a valid three-word vector view at each changed call and that `mPos` is the class's intended position view. Re-run focused TU verification and the full stock ROM build on the exact PR candidate. Preserve the documented unlicensed `__sinit`/RTTI policy caveat; the function bytes and relocation checks pass, but the TU is not promotable under the current manifest policy.
