# Handoff: issue-4570-hanachan-vec3-add-type-1104-retry1

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-hanachan-vec3-add-type-1104-retry1`; this is the producer candidate for independent verification.
- Source input is current `main` `8189857952b6d188ab810f718e548f4f9328b01c`; the candidate updates draft PR #4792.
- Original task `issue-4570-hanachan-vec3-add-type-1104` produced the first candidate on 85690 and was canceled after `main` advanced; its source work is preserved as the predecessor.
- Next action: independent verifier checks this candidate and records source review; the existing `__sinit` manifest gap remains a TU-level policy blocker for promotion.
- Status: candidate for independent verification; no independent review has been accepted yet.

## What changed and why

`src/Vec3_Add.c` defines `Vec3_Add(Vector3 *, const Vector3 *, const Vector3 *)`; no shared header declares it. `daHanachan_c` now uses that signature instead of three `void *` parameters. In `UpdateSegments`, the original byte-offset calculation is retained, and its result is stored once in a `Vector3 *prev`; passing `prev` directly avoids repeating a cast at the call. In `Behavior`, the actor position is passed through the existing `dActor_c::mPos` union member, rather than casting `mPosX` to `Vector3 *`. `InitResources` retains explicit typed views of its existing three-word scratch buffers. No object layout or symbol ownership changed.

## Proof on current main base `8189857952b6d188ab810f718e548f4f9328b01c`

- `python tools/tubuild.py verify ov034/daHanachan_c --no-write --verdict`: 35/35 functions match; object isolation and relocation destinations are clean; ROM emission order is correct. It exits 1 only because `__sinit_d_a_hanachan.cpp` (0x78c bytes) is an unlicensed `.text` symbol outside the TU manifest. Promotion remains refused by policy; no function mismatch was reported.
- `python tools/prepush_linkcheck.py --range 8189857952b6d188ab810f718e548f4f9328b01c..HEAD`: 36 checked, 34 verified, 2 blind warnings (class-init and `__sinit`), 0 blocking.
- `python tools/rombuild.py -j16 --no-rom`: PASS on parent `d745d7bb058430ae95cbe64bb22797b253a8349f`; current main adds only `contributions.json`, outside source/build inputs.
- `python tools/check_decl_agreement.py --changed 8189857952b6d188ab810f718e548f4f9328b01c`: no new declaration disagreements.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.
- Local changed-line style check is unavailable because clang-format 19.1.7 is not installed; hosted CI provides the style result.

## Verifier handoff

Independent review remains outstanding. Review the typed declaration and the remaining scratch-buffer views against callers and `Vector3`; independently rerun the focused TU proof and full stock ROM build on the exact candidate. The conventional `&mSegmentPos[seg - 1]` rewrite was tested and changes `UpdateSegments` bytes; preserve the original byte offset. Preserve the documented `__sinit` caveat. Publish verification evidence only from a different session than the producer.
