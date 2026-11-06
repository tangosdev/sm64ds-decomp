# Handoff: issue-4570-jango-vector-position-type-1105

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-jango-vector-position-type-1105`.
- Candidate base: `0bab793a13bf137418b5078adb030359b9cfbeb4` (`origin/main`).
- Source: `src/game/actors/d_a_jango.cpp`, TU `ov062/daJango_c`.
- This revision supersedes closed cast-only draft PR #4839 and implements its requested `Vector3` field view.
- Status: producer verification complete; independent source review and hosted PR review remain.

## What changed and why

The class now overlays `Vector3 mSpawnPos` and `Vector3 mPathNodePos` on their existing scalar triples while preserving all component names and offsets. These are confirmed vector records: InitResources snapshots the actor position; PathPtr::GetNode writes the path point; angle, distance, and horizontal-distance helpers consume the triples as vectors. The five current call sites use those members directly.

`math/CalcLib.h` now supplies the established typed signatures for `Vec3_Dist`, `Vec3_HorzAngle`, `Vec3_HorzDist`, `Vec3_Asr`, `SubVec3`, `ApproachAngle`, and `RandomIntInternal`; duplicate local declarations were retired. `Vec3_Asr`'s output and the SWOOP player-position copy are now `Vector3` locals. The TU keeps its local `Vec3_VertAngle` and unrelated helper declarations. The manifest records the two empty union cleanup thunks and the measured `@NNN` registration-node shifts caused by the overlays.

## Verification on the candidate

- `python tools/tubuild.py verify ov062/daJango_c --no-write --verdict`: 24/24 functions match; object isolation, relocation destinations, and emission order are clean. The two new union cleanup thunks and existing `Vector3` destructor are licensed as compiler-only deadstrip output. The existing `__sinit_d_a_jango.cpp` output remains outside the TU manifest; this change does not promote the TU.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_header_offsets.py include/daJango_c.h`: 27 commented fields, zero mismatches, class remains `0x490`.
- `python tools/check_decl_agreement.py --changed origin/main`: no new disagreements or redeclarations.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/source_style.py --base origin/main --clang-format <pinned clang-format>`: 2 files, 0 findings.
- `git diff --check`: clean.

## Review handoff

Review the vector interpretation against the field-writing callers and the typed CalcLib definitions. Independently rerun the focused TU check and full stock ROM build on the exact PR candidate. Preserve the `__sinit` manifest caveat; function bytes and relocations pass, but TU promotion is outside this change.
