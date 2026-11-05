# Handoff: issue-4570-hanachan-oam-decl-1106

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-hanachan-oam-decl-1106`; producer `reconstruct`; session `codex-hanachan-oam-decl-1106c`; Codex.
- Source branch and previous accepted input SHA: task branch; `682321c64de078383e3c66cea08b0a3cd451a5ef`.
- Original source base SHA and installed workflow/tool SHA: `682321c64de078383e3c66cea08b0a3cd451a5ef` for both.
- Separate evidence commits and required artifacts in this commit: none; `build/tu/` reports are local and ignored.
- Next action, responsible role and blockers: an independent verifier should inspect this commit and repeat the focused object, relocation and full ROM gates. Source review and PR validation are pending.
- Status: verified candidate for byte and relocation correctness; independent source acceptance is pending.
- Remaining uncommitted/local-only material and where it is preserved: no source changes; ignored build outputs and private queue receipts remain under this worktree's `build/`.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `dScMgHanachan_c`, TU `ov006/dScMgHanachan_c`, `0x020ea280..0x020ede18`.
- Reserved source/header/config surfaces actually touched: `src/minigames/d_s_mg_hanachan_methods.cpp`, `config/tu_manifest.d/ov006/dScMgHanachan_c.json`; `include/OAM.h` was included and read, not edited.
- ROM observations: the TU manifest licenses the contiguous 61-function run `0x020ea280..0x020ede18`; each function's bytes and relocation destinations were checked below.
- Lineage evidence or structural inference: the former local `OAM::Render` declaration has the exact shared-header signature: `OamAttr *Render(bool, OamAttr *, int, int, int, int, int, int)`. The shared header documents this as the 8-argument overload; the 10-argument `Fix12` overload remains a separate compiler limitation and was not changed.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: including the complete shared OAM class shifts all 35 TU-local PMF records from compiler names `@N` to `@(N+22)`. The candidate object's numbered-symbol set is exactly those 35 shifted IDs. The manifest's 35 `defined_symbol_renames` keys were re-keyed by +22; no function instruction words changed. Re-running `tubuild verify` resolves all destinations and reports 61/61 matches with clean object isolation and ROM order. It still exits 1 because it reports one unlicensed `.text` symbol, `__sinit_d_s_mg_hanachan_methods.cpp`; this change makes no TU-promotion claim.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all 61 functions in the registered TU match; 61/61 module-range functions match in the complete no-ROM build; `prepush_linkcheck` reports 61 verified, one blind warning, and zero blockers.
- Genuine methods; remaining free-function/ABI bridges: unchanged. This commit only replaces a duplicate static method declaration with the shared class declaration.
- Recovered layout/fields; remaining shadow structs/raw offsets: unchanged.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. `tubuild` reports one unlicensed emitted text symbol, `__sinit_d_s_mg_hanachan_methods.cpp`; no lifecycle/data ownership changes are claimed.
- Attribution preserved through each move/rename: no function ownership or source movement.
- Remaining agreed issue scope: the separate `Fix12` overload bridge and other Hanachan shadow layouts remain outside this focused change.

## Proof

The queue input and workflow are pinned at `682321c64de078383e3c66cea08b0a3cd451a5ef`; after main advanced, the candidate was rebased and all byte/link gates rerun on `8bbb4fc8c1ff3b7cd16559ba46e049afece894bf` with compiler `2004/b56`.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom` — exit 0; 11,395/11,395 source-built functions reproduced, 106/106 modules exact, 470/470 source-owned data claims reproduced, 302 BSS claims, 0 data differences, and 0 symbol errors. The control build was also 106/106 exact.
- Explicit function/consumer relocation checks: `python tools/prepush_linkcheck.py --files src/minigames/d_s_mg_hanachan_methods.cpp` — exit 0; 62 checked, 61 verified, one `BLIND-1` warning on `_ZN15dScMgHanachan_c7WigglerD1Ev`, zero blockers. `python tools/tubuild.py verify ov006/dScMgHanachan_c --no-write --verdict` — 61/61 byte matches, clean objisolate, all 61 in ROM order; exit 1 solely for the unlicensed `__sinit` section noted above.
- Complete emitted TU and data/metadata checks: `python tools/tu_object_diff.py --base HEAD --numbered src/minigames/d_s_mg_hanachan_methods.cpp` identified the +22 compiler-local renumbering; after the manifest rekeys, `tubuild verify` reported every function and relocation destination clean. No source code words changed.
- Shared-header consumer expansion: no header edit was made, so no shared-header consumer rebuild was needed. The candidate TU itself compiles with `include/OAM.h`.
- Port/path/reference and other applicable static gates: `python tools/check_decl_agreement.py --changed origin/main` — exit 0, no new declaration disagreements or local redeclarations; `python tools/port_refcheck.py` — exit 0, 217 references resolve; `python tools/check_dead_references.py` — exit 0, no new dead references or broken links; `git diff --check` — clean.
- Private validation, if run, and the exact PR head/base it tested: not run; PR not yet opened.
