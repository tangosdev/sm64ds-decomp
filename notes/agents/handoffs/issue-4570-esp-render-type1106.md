# Handoff: issue-4570-esp-render-type1106

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-esp-render-type1106`; producer stage; session `codex-esp-render-1106-retry`; Codex.
- Source branch and previous accepted input SHA: `audit/esp-render1106`; input `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
- Original source base SHA and installed workflow/tool SHA: both `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
- Separate evidence commits and required artifacts in this commit: none; proof commands and results are recorded below.
- Next action, responsible role and blockers: independent verifier should verify this exact commit, including the ten `defined_symbol_renames` rekeys in the reserved ov006 manifest; then integrator review. No known blocker.
- Status: verified candidate.
- Remaining uncommitted/local-only material and where it is preserved: build objects, ROM/compiler inputs and queue receipts are local/ignored under `build/`; none are part of this commit.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `dMg3DEspAnimSet_c::Render`, `src/minigames/d_mg3d_esp_anim_set.cpp`, ov006 `[0x020e777c, 0x020e7818)`.
- Reserved source/header/config surfaces actually touched: source above and `config/tu_manifest.d/ov006/dMg3DEspAnimSet_c.json`; the existing class and model headers were read but not edited.
- ROM observations: the call uses slot 5 on each 0x64-byte `dExtAnmModel_c` array element. `include/dExtAnmModel_c.h` names slot 5 `Render(const Vector3 *)`; `include/dMg3DEspAnimSet_c.h` declares the array as `mModels[3]`.
- Lineage evidence or structural inference: the existing vtable and object headers establish the class/member identity. The original local `Obj_777c` exposes slot 5 as `m(void *)`; the replacement casts the existing null `zero` to `const Vector3 *` and calls the named virtual. Compiled bytes and relocation destination are measured below.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: removing the unused local `Obj_777c` view shifts ten compiler-local BSS source names down one. Baseline and candidate ELF symbol-table indices 138–147 each point to 0xc BSS symbols; the exact candidate map `@1831–@1840` to configured `@1800–@1809` is recorded by symbol-table index in the manifest evidence. No other extra symbols appeared versus the untouched base.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 68 TU functions match; object isolation and relocation destination checks are clean.
- Genuine methods; remaining free-function/ABI bridges: the rendering call now uses the existing typed virtual member. Other TU work is unchanged.
- Recovered layout/fields; remaining shadow structs/raw offsets: no object layout or fields changed; the local six-slot `Obj_777c` vtable shim is removed.
- Lifecycle, vtable/RTTI, initializer and data ownership: no lifecycle or vtable ownership changes. The existing `__sinit_d_mg3d_esp_anim_set.cpp` output remains an unlicensed extra and is identical to the untouched-base verifier result.
- Attribution preserved through each move/rename: no source moves or function renames; only ten compiler-local manifest source names changed.
- Remaining agreed issue scope: broader issue #4570 reconstruction remains open.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0, base `ce959c55727fad6c5db2e3c38ed6435f222f7700`; 106/106 modules exact, 11,395/11,395 functions reproduce, 523 data claims and 304 BSS claims reproduce, zero mismatches.
- Explicit function/consumer relocation checks: `python tools/tubuild.py verify ov006/dMg3DEspAnimSet_c`, exit 1 solely because the existing `__sinit_d_mg3d_esp_anim_set.cpp` size 0x3f0 is not in the manifest. All 68 functions match, object isolation and relocation destinations are clean, and emission order matches. Running the untouched base gives the same sole extra and 68/68 matches.
- Complete emitted TU and data/metadata checks: focused verifier covered the complete 68-function TU and emitted data/symbol policy; all ten BSS local names were paired by ELF symtab indices 138–147 and sizes 0xc to the same configured targets. One compiler-generated `__sinit` remains baseline-only.
- Shared-header consumer expansion: no header changed, so no consumer expansion applies.
- Port/path/reference and other applicable static gates: `python tools/check_decl_agreement.py --changed origin/main` exit 0 (no new disagreements); `python tools/check_dead_references.py` exit 0 (no new dead references or markdown links); `python tools/check_tubuild_conflicts.py` exit 0 (all recorded conflicts agree). `git diff --check` clean. No path rename, so `port_refcheck.py` is not applicable.
- Private validation, if run, and the exact PR head/base it tested: CI will validate the published candidate; local proof above is pinned to `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
