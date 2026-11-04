# Handoff: Bombking matrix transform input type

## Identity and resumption

- Task: Bombking `MulVec3Mat4x3` matrix parameter type correction.
- Base: `main` at `8189857952b6d188ab810f718e548f4f9328b01c`.
- Candidate: one-file source correction; independent source review remains outstanding.

## Change and evidence

`src/MulVec3Mat4x3.c` defines the helper as `MulVec3Mat4x3(Vector3 *, Matrix4x3 *, Vector3 *)`. Bombking's local declaration had left the matrix parameter as `void *`, although all three call sites pass the matrix scratch at `data_020a0e68`.

The scratch remains declared as the file-local `M12` (`int w[12]`) because whole-matrix copies use that flat view to preserve MWCC block-move codegen. `common.h` is included first and provides the flat 12-word `Matrix4x3` view for helper calls. The three call sites now make that same-address type view explicit with `(Matrix4x3 *)&data_020a0e68`; storage, initialization, and copy behavior are unchanged.

## Verification on `8189857952b6d188ab810f718e548f4f9328b01c`

- `python tools/tubuild.py verify ov078/daBombking_c --no-write --verdict`: 52/52 functions match; object isolation, relocation destinations, and emission order are clean. Exit 1 only for the pre-existing unlicensed `.text` symbol `__sinit_d_a_bombking.cpp` (0x674 bytes); no function mismatch.
- `python tools/prepush_linkcheck.py --range 8189857952b6d188ab810f718e548f4f9328b01c..HEAD`: 53 checked, 52 verified, one blind `__sinit` warning, zero blocking.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules, 11,395/11,395 source-built functions, and 470/470 source-owned data claims exact; zero mismatches.
- `python tools/check_decl_agreement.py --changed 8189857952b6d188ab810f718e548f4f9328b01c`: no new declaration disagreements.
- `python tools/source_style.py --base 8189857952b6d188ab810f718e548f4f9328b01c --files src/game/actors/d_a_bombking.cpp`: zero findings with pinned clang-format 19.1.7.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

## Verifier handoff

Review that the local `M12` and shared flat `Matrix4x3` views both span twelve 32-bit words, and that the casts preserve the original `data_020a0e68` storage address. Independently rerun the focused TU proof and full ROM build on the pushed candidate. The `__sinit` manifest gap remains a promotion blocker outside this helper-signature change.
