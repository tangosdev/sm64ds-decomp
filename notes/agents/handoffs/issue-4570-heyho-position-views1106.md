# Handoff: issue-4570-heyho-position-views1106

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-heyho-position-views1106`; stage `replace-views`; session `codex-heyho-position-views1106`; harness Codex.
- Source branch: `audit/heyho-position-views1106`; queue input/original source base: `1a882b6e17271993d32fb4e52adba1c079f4798a`.
- Candidate was rebased onto and tested against current `main` `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
- Workflow/tool pin: `1a882b6e17271993d32fb4e52adba1c079f4798a`.
- Artifacts: this handoff, `src/minigames/d_mg3d_heyho_obj_adapter.cpp`, and `config/tu_manifest.d/ov006/dMg3DHeyhoObjAdapter_c.json`.
- Next action: independent verifier checks the direct `Vector3` member use and 20 compiler-local symbol rekeys, then reruns the exact TU and full ROM proof. Do not self-verify.
- Status: verified candidate, pending independent source review.
- Queue receipts and build logs remain ignored under `build/`; no private files are committed.

## What changed and why

- Class/TU/symbol scope: `Heyho`, `ov006/dMg3DHeyhoObjAdapter_c`, `Heyho_StartMotionFlyIn` at `0x020c6ca4..0x020c6e4c` and `Heyho_StartMotionRegroup` at `0x020c68f4..0x020c6a9c`.
- Removed two local three-int `Vec3` views and their casts. Both functions now use `Vector3 *` directly for `Heyho::mPos` and the existing `AddVec3(Vector3 *, ...)` interface.
- `include/Heyho.h` declares `mPos` as `Vector3` at offset `0x9c`; all four pointers address that member. No new Vector3 object or lifetime was introduced. The six existing static Vector3 offsets remain untouched.
- The source comment now reflects that these starters use the member's actual type. This is a type/use correction, not a claim about recovered original spelling.
- Removing the local aliases shifted 20 emitted local symbol names. The manifest rekeys pair baseline/candidate ELF symbols at the same symtab index with section index, offset, size, binding and type unchanged. Exact TU verification confirms target destinations after those rekeys.

## Reconstruction dimensions

- Exact function bytes and relocation destinations: all 50 manifest functions match.
- No class layout, lifecycle, vtable/RTTI, initializer ownership, or attribution changed.
- Other Heyho helpers retain their existing partial views and ABI bridges.

## Proof

- `python tools/tubuild.py verify ov006/dMg3DHeyhoObjAdapter_c --no-write --verdict`: 50/50 functions match; object isolation, relocation destinations and emission order clean. Exit 1 only because the baseline and candidate both have the existing unlicensed `__sinit_d_mg3d_heyho_obj_adapter.cpp` section (0x68 bytes).
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, 11,395/11,395 functions and 523 source-owned data claims reproduce, 304 BSS claims pass, zero mismatching bytes.
- `python tools/check_decl_agreement.py --changed HEAD`: exit 0; no new declaration disagreements.
- `python tools/check_dead_references.py`: exit 0; no new dead references or broken Markdown links.
- `python tools/source_style.py --base HEAD --files src/minigames/d_mg3d_heyho_obj_adapter.cpp --clang-format C:\Users\andre\AppData\Local\Programs\Python\Python311\Scripts\clang-format.exe`: exit 0; 0 findings.
- No shared header was edited, so consumer expansion was not applicable. No source path was renamed or moved.
- `prepush_linkcheck.py` and private PR validation: pending commit/publication and independent verification.
