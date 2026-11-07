# Handoff: issue-4570-blocks-render-type1106

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-blocks-render-type1106`; producer stage; session `codex-blocks-render-1106-retry`; Codex.
- Source branch and previous accepted input SHA: `audit/blocks-render1106`; input `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
- Original source base SHA and installed workflow/tool SHA: both `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
- Separate evidence commits and required artifacts in this commit: none; proof commands and results are recorded below.
- Next action, responsible role and blockers: independent verifier should verify this exact commit, including the two `defined_symbol_renames` rekeys in the reserved ov098 manifest; then integrator review. No known blocker.
- Status: verified candidate.
- Remaining uncommitted/local-only material and where it is preserved: build objects, ROM/compiler inputs and queue receipts are local/ignored under `build/`; none are part of this commit.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daObjBlockS_c`, `src/game/actors/d_a_obj_block_s.cpp`, `daObjBlockS_c::Render`, ov098 `[0x02139aa8, 0x02139b0c)`.
- Reserved source/header/config surfaces actually touched: source above and `config/tu_manifest.d/ov098/daObjBlockS_c.json`; the existing class and model headers were read but not edited.
- ROM observations: Render calls virtual slot 5 on the embedded `mModel`. `include/dExtSimpleModel_c.h` names that slot `Render(const Vector3 *)`; `include/daObjBlockS_c.h` declares `mModel` as `dExtSimpleModel_c`.
- Lineage evidence or structural inference: the shared header's vtable evidence lists slot 5 as `Render(Vector3 const *)`; the call passes null, so replacing the local `Sub::m5(int)` view with `mModel.Render(0)` preserves the argument value and dispatch target. The compiled bytes and relocation destination are measured below.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: removing local `Sub` shifts local BSS source names from `@1645/@1646` to `@1644/@1645`. ELF symbol-table indices 72/73 remain the two 0xc BSS entries, so the manifest's targets remain `@1560/@1561`; the source names were rekeyed accordingly. The untouched base run confirmed this is the only new metadata action required.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 37 TU functions match; object isolation and relocation destination checks are clean.
- Genuine methods; remaining free-function/ABI bridges: the render call now uses the existing typed virtual member. Other TU work is unchanged.
- Recovered layout/fields; remaining shadow structs/raw offsets: no object layout or fields changed; the local six-slot `Sub` vtable shim is removed.
- Lifecycle, vtable/RTTI, initializer and data ownership: no class lifecycle or vtable ownership changes. The existing `__sinit_d_a_obj_block_s.cpp` output remains an unlicensed extra and is identical to the untouched-base verifier result.
- Attribution preserved through each move/rename: no source moves or function renames; only two compiler-local manifest source names changed.
- Remaining agreed issue scope: broader issue #4570 reconstruction remains open.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0, base `ce959c55727fad6c5db2e3c38ed6435f222f7700`; 106/106 modules exact, 11,395/11,395 functions reproduce, 523 data claims and 304 BSS claims reproduce, zero mismatches.
- Explicit function/consumer relocation checks: `python tools/tubuild.py verify ov098/daObjBlockS_c`, exit 0 after rekeys; 37/37 functions match, object isolation clean, relocation destinations clean, expected emission order. The sole remaining unlicensed extra is `__sinit_d_a_obj_block_s.cpp` size 0x1bc; the same extra appeared on untouched base. The base also matched 37/37.
- Complete emitted TU and data/metadata checks: focused verifier covered the complete 37-function TU and emitted data/symbol policy; two BSS local names were paired by ELF symtab indices/sizes to the same configured targets. One compiler-generated `__sinit` remains baseline-only.
- Shared-header consumer expansion: no header changed, so no consumer expansion applies.
- Port/path/reference and other applicable static gates: `python tools/check_decl_agreement.py --changed origin/main` exit 0 (no new disagreements); `python tools/check_dead_references.py` exit 0 (no new dead references or markdown links); `python tools/check_tubuild_conflicts.py` exit 0 (all recorded conflicts agree). `git diff --check` clean. No path rename, so `port_refcheck.py` is not applicable.
- Private validation, if run, and the exact PR head/base it tested: CI will validate the published candidate; local proof above is pinned to `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
