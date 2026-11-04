# Handoff: issue-4570-savedata-memory-types-1104

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-savedata-memory-types-1104`; producer; `codex-savedata-memory1104`; Codex.
- Source branch and previous accepted input SHA: [published branch](https://github.com/tangosdev/sm64ds-decomp/tree/audit/savedata-memory-types1104); `d001a6e7391c27a7d96f631122bac40c2257071e`.
- Original source base SHA and installed workflow/tool SHA: `d001a6e7391c27a7d96f631122bac40c2257071e` for both.
- Separate evidence commits and required artifacts in this commit: none.
- Next action, responsible role and blockers: verifier to inspect the copy direction and independently rerun relocation and byte checks; integrator follows verification. No technical blocker.
- Status: verified byte candidate; independent source review and verification are pending.
- Remaining uncommitted/local-only material and where it is preserved: none intended; build artifacts remain gitignored in this worktree.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `SaveData`, `src/engine/save/SaveData.cpp`, ARM9 `0x02013568-0x02013e80`; helper dependency `func_0205a588` at `0x0205a588`.
- Reserved source/header/config surfaces actually touched: only `src/engine/save/SaveData.cpp`; `src/engine/mi/MI_Memory.c` was inspected as the helper definition, not changed.
- ROM observations: `func_0205a588` is defined as `void(void *, u8, u32)` and `CpuCopy8` as `void(const void *, void *, u32)` in MI_Memory.c. The SaveData local declarations disagreed on the fill helper's value and size types and mislabeled CpuCopy8's source/destination order.
- Lineage evidence or structural inference: save-menu call sites in `src/game/stages/d_s_dsmt.cpp` pass the selected file buffer first and the live/save-slot destination fourth; `func_02013c84` copies into that fourth pointer and then saves it. Renaming those two parameters and using `CpuCopy8(src, dest)` makes the source direction explicit while preserving the call's register values. The SDK helper definitions establish the exact primitive signatures.
- Hypothesized names/filenames, explicitly not recovered facts: no symbol or file names changed.
- Compiler experiments and measured barriers: none beyond the pinned ROM and relocation builds; no barrier found.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 44 functions in SaveData.cpp passed relocation-aware linkcheck; all 11,395 source-built functions reproduced in the full build.
- Genuine methods; remaining free-function/ABI bridges: unchanged. The file's C wrappers and SDK helper boundaries retain their ROM symbols.
- Recovered layout/fields; remaining shadow structs/raw offsets: unchanged.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged.
- Attribution preserved through each move/rename: no source moves or attribution changes.
- Remaining agreed issue scope: continue reconstructing other SaveData members and callers as separately evidenced work.

## Proof

All commands ran in this worktree against source base `d001a6e7391c27a7d96f631122bac40c2257071e`.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 11,395 functions reproduced, 470 data claims and 302 BSS claims, 106/106 modules exact, zero source data differences. ROM image construction was skipped by `--no-rom`.
- Explicit function/consumer relocation checks: `python tools/prepush_linkcheck.py --files src/engine/save/SaveData.cpp`, exit 0; 44 checked, 44 verified, zero warnings and blockers.
- Complete emitted TU and data/metadata checks: the full source build compiled the TU and all functions reproduced; no source-owned data changes.
- Shared-header consumer expansion: no header changed. `python tools/check_decl_agreement.py --changed origin/main` found no new local header redeclarations or declaration disagreements. `--symbol func_0205a588` no longer reports the SaveData.cpp declaration; other consumers remain outside this slice.
- Port/path/reference and other applicable static gates: no paths, manifests or port references changed. `git diff --check` passed. `source_style.py --base origin/main` passed for one file with clang-format 19.1.7, zero findings and zero deferrals.
- Private validation: not run; candidate PR validation pending after publication.
