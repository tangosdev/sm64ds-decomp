# Handoff: issue-4570-trmp-line-vtx-twins1106

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570
- Task: `issue-4570-trmp-line-vtx-twins1106`; stage `replace-views`; session `codex-trmp-line-vtx-twins1106`; harness Codex.
- Source branch: `audit/trmp-line-vtx-twins1106`; input/base: `c1b98e90c6a1b1997dd66b74b9e138965529c48f`.
- Workflow/tool pin: `c1b98e90c6a1b1997dd66b74b9e138965529c48f`.
- Evidence artifacts: this handoff, `src/minigames/unit020cd744.cpp`, and `config/tu_manifest.d/ov006/unit020cd744.json`.
- Next action: independent verifier checks the `TrmpLine::Vtx` aliases, compiler-local symbol rekeys, and exact TU/full ROM proof. Do not self-verify.
- Status: verified candidate, pending independent source review.
- Private queue receipts and build logs remain in ignored `build/` and are not committed.

## What changed and why

- Class/TU/symbol scope: `TrmpLine`, `ov006/unit020cd744`, `TrmpLine_Draw` at `0x020cf2fc..0x020cf758` and `TrmpLine_StateFlex` at `0x020cfa44..0x020cfc58`.
- Replaced two local three-word `Vec3` definitions with `typedef TrmpLine::Vtx Vec3;`.
- `include/TrmpLine.h` defines `TrmpLine::Vtx` as three `Fix12i` fields and uses it for `mVerts[16]` at `+0x05c` and `mRestVerts[16]` at `+0x11c`. Its size is asserted as `0x0c`. Both functions access those arrays through pointers; this does not instantiate a `Vector3` or alter the deliberately POD vertex-array lifecycle.
- This is a recovered type relationship to the already-declared vertex view, not a claim about the original local spelling or source file.
- The compiler renumbered five local output symbols. The manifest `from` names were rekeyed by baseline/candidate ELF symbol-table index, with section index, offset, size, binding and type equal: indices 139–142 and 145. The whole-TU verifier then confirmed relocation destinations after manifest resolution.

## Reconstruction dimensions

- Exact functions: no function body changed; both target functions and all other owned functions match retail.
- Class lifecycle, vtable/RTTI, static initialization and attribution were not changed.
- Remaining class/TU reconstruction scope is unchanged.

## Proof

- `python tools/tubuild.py verify ov006/unit020cd744 --no-write --verdict`: 59/59 functions match; object isolation and relocation destinations clean; emission order correct. Exit 1 only because the existing `__sinit_unit020cd744.cpp` section (0xcc bytes) is not licensed in the manifest; baseline has the same compiler-only limitation.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact, 11,395/11,395 functions reproduce, 523/523 source-owned data claims reproduce, 304 BSS claims pass, 0 mismatching bytes.
- `python tools/check_decl_agreement.py --changed HEAD`: exit 0; no new redeclarations or disagreements.
- `python tools/check_dead_references.py`: exit 0; no new dead references or broken Markdown links.
- `python tools/source_style.py --base HEAD --files src/minigames/unit020cd744.cpp --clang-format C:\Users\andre\AppData\Local\Programs\Python\Python311\Scripts\clang-format.exe`: exit 0; 0 findings.
- No shared header was edited, so consumer expansion was not applicable. Port paths were not renamed or moved.
- `prepush_linkcheck.py` and private PR validation: pending commit/publication and independent verification.
