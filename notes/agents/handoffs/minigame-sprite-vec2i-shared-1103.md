# Handoff: issue-4570-minigame-sprite-vec2i-shared-1103b

This handoff describes the producer candidate. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-minigame-sprite-vec2i-shared-1103c`, a handoff correction to the source output produced by `issue-4570-minigame-sprite-vec2i-shared-1103b` (`codex-minigamevec2i-1103c`); follow-up producer session `codex-minigamevec2i-followup-1103`.
- Draft PR: https://github.com/tangosdev/sm64ds-decomp/pull/4712 (branch link is on the PR).
- Source base: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6` (`origin/main`); follow-up input: `eb5d443015cd00389f5bb2c5fe6b4b3803762292`; workflow/tool SHA: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`.
- Candidate status: exact-byte source candidate with the hosted dead-reference path corrected; pending fresh exact-head checks, independent verification/source review, and hosted PR validation.
- Next action: verifier should inspect the exact published source and manifest, rerun `tubuild` and ROM proof against the recorded base, and publish independent review evidence. The TU is not claimed promotable while `tubuild` reports an unlicensed `.text` section.
- Build output and queue receipts remain ignored under `build/`; no uncommitted source changes are intended after the candidate commit.

## What changed and why

- In `src/minigames/unit020b4aa4.cpp` (OV004 range `0x020b4aa4..0x020b67e8`), include `private/ov006_ec4dc_obj.h` and remove the local `struct Vec2i { int x, y; }` duplicate.
- The shared `Vec2i` is two `s32` components at offsets 0 and 4, matching the local two-`int` record. In this TU, the three `Sprite` vectors remain at offsets `0x08`, `0x10`, and `0x18`; the record is still 8 bytes.
- Including the shared header changes compiler-local names. The candidate ELF emits `@202` for the 12-byte destructor-registration node and `__arraydtor$201` for the array-destruction thunk. The TU manifest maps them to the existing configured destinations `data_ov004_020bfa28` and `__arraydtor$192` respectively. The relocation-aware TU check confirms those destinations.
- The shared header contents, ROM symbols, source paths, and attribution are unchanged. Only the source and its TU manifest are edited.

## Reconstruction dimensions

- This is a shared record cleanup in an existing TU, not a TU promotion. No original class identity is claimed for `Sprite`.
- The rest of the minigame's vector and pointer views remain as-is; no broader layout is inferred.
- No constructor/destructor semantics changed. Compiler-local naming changes are recorded above and in the manifest evidence.

## Proof

All checks ran in `C:\tmp\sm64ds-minigamespritevec21103`, against the pinned `2004/b56` compiler and base above.

- `python tools/tubuild.py verify ov004/unit020b4aa4 --no-write --verdict`: 43/43 functions MATCH; object isolation, relocation destinations, and ROM emission order clean. It reports one unlicensed `.text` section (`__sinit_unit020b4aa4.cpp`) and therefore refuses promotion; this change does not claim promotion.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 11,395 functions and 468 initialized-data claims reproduce with zero mismatches; 106/106 modules exact; ROM data check reports 2,198 symbols verified, 689 partial, zero differing.
- `python tools/check_decl_agreement.py --changed origin/main`: exit 0; no new disagreements or local header redeclarations.
- `python tools/check_dead_references.py`: exit 0; no new dead references or broken Markdown links.
- `python tools/check_tubuild_conflicts.py ov004/unit020b4aa4`: exit 0; no conflicts.
- `python tools/port_refcheck.py`: exit 0; all 217 references resolve.
- `python tools/source_style.py --base origin/main --patterns-only`: exit 0; zero findings. Formatting was not checked by this patterns-only run.
- `prepush_linkcheck.py --range origin/main..HEAD` passed on the source candidate before this documentation-only correction. Repeat it on the published follow-up head. Hosted private PR validation and independent source review are pending.
