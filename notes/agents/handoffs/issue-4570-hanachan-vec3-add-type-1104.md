# Handoff: issue-4570-hanachan-vec3-add-type-1104-retry1

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-hanachan-vec3-add-type-1104-retry1`; this is the producer candidate for independent verification.
- Source input is current `main` `b8e6b7514b9370cf7e6f1b21a6dbe9c3434ed192`; the candidate is on the existing draft PR branch.
- Original task `issue-4570-hanachan-vec3-add-type-1104` produced the first candidate on 85690 and was canceled after `main` advanced; its source work is preserved as the predecessor.
- Next action: independent verifier checks this candidate and records source review; the existing `__sinit` manifest gap remains a TU-level policy blocker for promotion.
- Status: candidate for independent verification; no independent review has been accepted yet.

## What changed and why

`src/Vec3_Add.c` defines `Vec3_Add(Vector3 *, const Vector3 *, const Vector3 *)`; no shared header declares it. `daHanachan_c` now uses that signature instead of three `void *` parameters. Calls whose values are existing `Vector3` objects remain direct. The raw byte-offset segment pointer, `mPosX` triple, and two `s32[3]` buffers are explicitly viewed as the matching three-word vectors. No object layout or symbol ownership changed.

## Proof on current main base `b8e6b7514b9370cf7e6f1b21a6dbe9c3434ed192`

- `python tools/tubuild.py verify ov034/daHanachan_c --no-write --verdict`: 35/35 functions match; object isolation and relocation destinations are clean; ROM emission order is correct. It exits 1 only because `__sinit_d_a_hanachan.cpp` (0x78c bytes) is an unlicensed `.text` symbol outside the TU manifest. Promotion remains refused by policy; no function mismatch was reported.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed b8e6b7514b9370cf7e6f1b21a6dbe9c3434ed192`: no new declaration disagreements.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.
- Local changed-line style check is unavailable because clang-format 19.1.7 is not installed; hosted CI provides the style result.

## Verifier handoff

Review the typed declaration and each raw-storage cast against callers and `Vector3`, then independently rerun the focused TU proof and full stock ROM build on the exact candidate. Preserve the documented `__sinit` caveat. Publish the verification evidence only from a different session than the producer.
