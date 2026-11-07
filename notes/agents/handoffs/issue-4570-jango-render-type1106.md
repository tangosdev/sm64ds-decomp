# Handoff: issue-4570-jango-render-type1106

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-jango-render-type1106`; producer stage; session `codex-jango-render-1106`; Codex.
- Producer branch recorded in the queue; previous accepted input SHA: `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
- Original source base SHA and installed workflow/tool SHA: both `ce959c55727fad6c5db2e3c38ed6435f222f7700`.
- Separate evidence commits and required artifacts in this commit: none; proof commands and results are recorded below.
- Next action, responsible role and blockers: independent verifier should verify this exact commit, including the 12 `defined_symbol_renames` rekeys in the reserved ov062 manifest; then integrator review. No known blocker.
- Status: verified candidate.
- Remaining uncommitted/local-only material and where it is preserved: build objects, ROM/compiler inputs and queue receipts are local/ignored under `build/`; none are part of this commit.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daJango_c::Render`, `src/game/actors/d_a_jango.cpp`, ov062 `[0x0211c920, 0x0211c94c)`.
- Reserved source/header/config surfaces actually touched: source above and `config/tu_manifest.d/ov062/daJango_c.json`; `include/daJango_c.h` and `include/dExtBlendAnmModel_c.h` were read but not edited.
- ROM observations: the call dispatches slot 5 on the embedded `mBlendAnmModel`. The shared blend-model header declares this slot as `Render(const Vector3 *)`; the actor header types the member as `dExtBlendAnmModel_c`.
- Lineage evidence or structural inference: class and member identity come from the existing shared headers; the old local `Base` shim named slot 5 as `m(int)`. The source now calls the known virtual member with a null scale. Compiled bytes and relocation destination are measured below.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: removing the local `Base` declaration shifts 12 compiler-local data/BSS names. Baseline and candidate ELF symbol-table indices 50–57 and 59–62 preserve each section and size; the manifest retains their configured targets with the new source names. Index 58 (`@632` to `@692`) is unchanged. The untouched base verifier confirms no extra section/symbol besides `__sinit`.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 24 TU functions match; object isolation and relocation destination checks are clean.
- Genuine methods; remaining free-function/ABI bridges: the render call now uses the existing typed virtual member. Other TU work is unchanged.
- Recovered layout/fields; remaining shadow structs/raw offsets: no object layout or fields changed; the local six-slot `Base` vtable shim is removed.
- Lifecycle, vtable/RTTI, initializer and data ownership: no lifecycle or vtable ownership changes. The existing `__sinit_d_a_jango.cpp` output remains an unlicensed extra and also appears on the untouched base.
- Attribution preserved through each move/rename: no source moves or function renames; only 12 compiler-local manifest source names changed.
- Remaining agreed issue scope: broader issue #4570 reconstruction remains open.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0, base `ce959c55727fad6c5db2e3c38ed6435f222f7700`; 106/106 modules exact, 11,395/11,395 functions reproduce, 523 data claims and 304 BSS claims reproduce, zero mismatches.
- Explicit function/consumer relocation checks: `python tools/tubuild.py verify ov062/daJango_c`, exit 1 solely because the existing `__sinit_d_a_jango.cpp` size 0x1d0 is not in the manifest. All 24 functions match, object isolation and relocation destinations are clean, and emission order matches. The untouched base has the same sole extra and 24/24 matches.
- Complete emitted TU and data/metadata checks: focused verifier covered all 24 functions and emitted data/symbol policy; the 12 local names were paired by ELF symbol-table indices, section and size to the same configured targets. One compiler-generated `__sinit` remains baseline-only.
- Shared-header consumer expansion: no header changed, so no consumer expansion applies.
- Port/path/reference and other applicable static gates: `python tools/check_decl_agreement.py --changed origin/main` exit 0 (no new disagreements); `python tools/check_dead_references.py` exit 0 (no new dead references or markdown links); `python tools/check_tubuild_conflicts.py` exit 0 (all recorded conflicts agree). `git diff --check` clean. No path rename, so `port_refcheck.py` is not applicable.
- Private validation, if run, and the exact PR head/base it tested: `python tools/rombuild.py -j16 --no-rom` passed with the candidate on base `ce959c55727fad6c5db2e3c38ed6435f222f7700`. `python tools/prepush_linkcheck.py --files src/game/actors/d_a_jango.cpp` reports 24 verified, 0 blockers, 1 `__sinit_d_a_jango.cpp` BLIND-6 warning; the same explicit-file check on the untouched base reports the same result.
