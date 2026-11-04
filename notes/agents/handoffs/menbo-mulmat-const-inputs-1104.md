# Handoff: Menbo matrix helper const inputs

## Identity and resumption

- Task: `audit/menbo-mulmat-type1104`; candidate for draft PR after current-base byte proof.
- Base: `main` at `087f79fb22211ad869bbd2cea526b2c2b7d8b07c`.
- Independent source review is still required. This handoff records producer checks, not independent acceptance.

## Change and evidence

`MulMat4x3Mat4x3` reads both matrix inputs and writes the destination. Its implementation in `src/MulMat4x3Mat4x3.c` accepts `const int *` inputs; existing typed declarations in `d_a_dossy.cpp` and `d_a_holhei.cpp` likewise use `const Matrix4x3 *`. Menbo's local declaration and input view now match that usage. The output remains mutable. The declaration-agreement baseline change is limited to Menbo's two input parameters, because the raw implementation spells them as `const int *`.

In `InitResources`, `mAnmModel.data.transforms + idx` is a raw transform-storage view and is passed as `const Matrix4x3 *`; `data_020a0e68` is both the second input and output, matching the helper's in-place alias handling.

## Verification on `087f79fb22211ad869bbd2cea526b2c2b7d8b07c`

- `python tools/tubuild.py verify ov090/daMenbo_c --no-write --verdict`: 25/25 match, object isolation and relocation destinations clean, emission order correct. Exit 1 only for the pre-existing unlicensed `__sinit_d_a_menbo.cpp` section (0x1c0 bytes); no function mismatch.
- `python tools/prepush_linkcheck.py --range 087f79fb22211ad869bbd2cea526b2c2b7d8b07c..HEAD`: 26 checked, 25 verified, one blind `__sinit` warning, zero blocking.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed 087f79fb22211ad869bbd2cea526b2c2b7d8b07c`: no new declaration disagreements.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

## Verifier handoff

Inspect the implementation's read/write behavior, Menbo's transform storage type, the in-place second-input/output alias, and the narrow declaration-agreement baseline entry. Independently rerun focused TU verification and the full stock ROM build on the exact pushed candidate. The `__sinit` manifest gap remains a promotion blocker and is outside this declaration-only change.
