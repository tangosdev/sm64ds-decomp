# Handoff: issue-4570-stage-paddata-shadow-1103

This document describes the producer candidate. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-stage-paddata-shadow-1103b`, a documentation correction to the source output produced by `issue-4570-stage-paddata-shadow-1103` (`codex-stagepad-shadow-1103`); follow-up producer session `codex-stagepad-followup-1103`.
- Draft PR: https://github.com/tangosdev/sm64ds-decomp/pull/4711 (branch link is on the PR).
- Source base: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6` (`origin/main`); follow-up input: `c7f64e776967f05d8a2dd0999dbe7ae6618aa587`; workflow/tool SHA: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`.
- Candidate status: verified source candidate with the hosted dead-reference finding corrected; pending fresh exact-head checks, independent verification/source review, and hosted PR validation.
- Next action: verifier should inspect the exact published candidate, repeat the relevant TU and ROM proof on base `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`, then publish a queue verification. The TU's unlicensed initializer remains a promotion blocker.
- No local-only source changes remain after the candidate commit. Build outputs and queue receipts remain ignored under `build/`.

## What changed and why

- In `src/game/stages/d_s_stage_methods.cpp` (ARM9 `arm9/dScStage_c`), include `private/controller_input.h` and remove the duplicate local `struct PadData`.
- The existing shared `PadData` is the same 4-byte record: `held` is a `u16` at offset 0 and `pressed` is a `u16` at offset 2. `data_020a0e58` already uses those fields; there are no new field names or speculative layout claims.
- No config, manifest, symbol, attribution or shared-header contents changed. All raw byte-addressed accesses remain as before.

## Reconstruction dimensions

- This is one duplicate-record cleanup, not a TU promotion. The TU remains a mixed reconstruction with existing raw views and unresolved names.
- No lifecycle, vtable, RTTI, initializer or data ownership changes are claimed.
- Remaining #4570 scope is unaffected.

## Proof

All commands ran in `C:\tmp\sm64ds-stagepad1103` against the pinned `2004/b56` toolchain and the base above.

- `python tools/tubuild.py verify arm9/dScStage_c --no-write --verdict`: 123/123 functions MATCH; object isolation, relocation destinations and ROM emission order clean. Exit 1 because the TU still emits one unlicensed `.text` section, `__sinit_d_s_stage_methods.cpp`; therefore `PROMOTION REFUSED`, and this change does not claim to fix that existing issue.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 11,395 functions reproduce, 468 source-owned initialized-data claims reproduce, zero mismatches; 106/106 modules exact; ROM data check reports 2,198 symbols verified, 689 partial, zero differing.
- `python tools/check_decl_agreement.py --changed origin/main`: exit 0; no new local header redeclarations or declaration disagreements.
- `python tools/check_dead_references.py`: exit 0; no new dead references or broken Markdown links.
- `python tools/check_tubuild_conflicts.py arm9/dScStage_c`: exit 0; one manifest entry checked, no conflicts.
- `python tools/port_refcheck.py`: exit 0; 217 references resolve.
- `python tools/source_style.py --base origin/main --patterns-only`: exit 0; zero findings. Formatting was not checked by this patterns-only run.
- The local pre-push linkcheck passed on the source candidate before the documentation-only correction. Repeat it against the published follow-up head. Hosted private PR validation and independent source review are pending.
