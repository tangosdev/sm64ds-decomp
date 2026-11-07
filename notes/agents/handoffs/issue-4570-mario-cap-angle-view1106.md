# Handoff: issue-4570-mario-cap-angle-view1106

This handoff records the producer result. The queue records the immutable output SHA.

## Identity and resumption

- Issue and task: #4570; `issue-4570-mario-cap-angle-view1106`; producer stage.
- Producer session: `codex-mario-cap-angle-view1106`.
- Branch and current output: `the candidate worktree branch`; `7a69d312b223fbccf5c3886de8d45b454f22536b`.
- Source base: `21224a8fec6e0d57482b803080e31f79f247beff`; rebased onto current main `6228630fde61eccbb9563c1b5b1d6da70fc2e4e8`.
- Workflow/tool commit: `21224a8fec6e0d57482b803080e31f79f247beff`; queue source-review policy pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Next action: independent verifier reviews this exact output, repeats the Mario-cap TU proof and current-main ROM analysis, and records the narrow source judgment. No PR has been opened yet.
- Status: focused bytes, relocations, and full stock ROM analysis pass. The focused TU command exits 1 solely because the compiler-generated `__sinit_d_a_obj_mario_cap.cpp` section (0x414 bytes) remains unlicensed; the same section is present on the base and the production link deadstrips it.

## What changed and why

- Class/TU/range: `daObjMarioCap_c`, `ov002/daObjMarioCap_c`, `ov002:0x020b6fcc..0x020b71e8` (`CheckTouch`).
- Replaced the local `struct V16 { u16 x, y, z; }` with the shared `Vector3_16`. Its three `s16` fields match the signed angle storage read from `dActor_c::mAngleX/Y/Z` and written to this cap's signed angles. The existing raw `u16` reads and explicit `s16` casts remain to preserve the observed bit-copy path and compiler output.
- `include/dActor_c.h` already defines `Vector3_16`; no shared header or consumer was changed. The whole class reconstruction is not claimed complete.
- Removing the local type renumbered 13 compiler-local BSS names. The manifest's `from` names moved from `@1082..@1094` to `@1081..@1093`; each row's ROM target is unchanged. Baseline and candidate objects were paired by section index, offset, size, binding, and symbol type. The measured pairs are recorded in each manifest evidence string.

## Proof on the rebased output

- `python tools/tubuild.py verify ov002/daObjMarioCap_c --no-write --verdict`: 31/31 ROM functions match; object isolation and relocation destinations are clean; emission order is exact. It reports only the existing unlicensed `__sinit` and exits 1 for that policy reason.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 11,395/11,395 source-built functions match; 482/482 source-owned data claims match; 106/106 modules exact; no mismatches.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_obj_mario_cap.cpp`: 31 verified, one `BLIND-14` warning for the unlicensed `__sinit`, zero blockers.
- `python tools/check_decl_agreement.py --changed origin/main`: exit 0; no new declaration disagreements or local redeclarations.
- `python tools/port_refcheck.py`: exit 0; all 217 references resolve.
- `python tools/source_style.py --base origin/main`: exit 0; zero findings and zero deferrals. (Pinned clang-format 19.1.7.)
- Independent review should retain the partial reconstruction scope and the documented `__sinit` exception.
