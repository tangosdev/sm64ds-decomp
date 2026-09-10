# Handoff: issue-2412-humanizer-2

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Task: `issue-2412-humanizer-2`, stage `revise`, issue https://github.com/tangosdev/sm64ds-decomp/issues/2412.
- Producer session: `codex-humanizer-moneybag-20260907-01`, harness Codex. Global fleet coordinator: `fleet-0907`; delegated repair coordinator: `codex-humanizer-fixes-20260907-01`.
- Branch: `cpp/humanizer-moneybag-0907`. Accepted input and repair base: `26f54f8fc8faaf00568e86d33f37860a46470906`.
- This follows the landed daGmch_c promotion in #2431 (`9cc18bb3b`). The original class scout base remains `2ab05bae36a6821679a7e647e811c0a3243a3c49`, as recorded in the preserved facts; it is not this repair's input.
- Queue workflow pin: `f327f7b6460e157153eb7fc0749dbbe60dd854f1`. Source and proof tools are unchanged from accepted input `26f54f8fc8faaf00568e86d33f37860a46470906`; compiler pin is `2004/b56`.
- Required artifact: this handoff. Local proof artifacts remain under ignored `build/humanizer/`; no separate evidence commit is included.
- Status: locally proved candidate; independent verification is pending.
- Next action: coordinator reviews the local immutable output and offers it to a different verifier session. Source push, queue publication, independent verification, and private PR validation are pending. The producer has not published this stage.

## What changed and why

The scope is class `ov081/daGmch_c`, text `[0x02126504, 0x02127b34)`. The four changed source/metadata surfaces are `src/actors/daGmch_c.cpp`, `include/daGmch_c.h`, `config/tu_manifest.d/ov081/daGmch_c.json`, and four `why` cells in `symbols/actor_renames.tsv`. This handoff is the fifth reserved file.

The source/header now distinguish the ROM's RTTI class spelling from reconstructed member and factory names. The factory spelling follows project convention and later EAD lineage; it is not an original function identifier recovered from this cartridge. Two helper ledger rationales describe their calls using reconstructed project names without claiming cartridge mangled-name evidence.

The `Ei` in `SetState(int)` and `EPv` in `ApplySlopeToVertSpeed(void*)` now carry specific original-type uncertainty disclosures in the declaration comments, rename rationales, and manifest notes. Neither signature nor symbol spelling changed.

An added manifest note explicitly identifies the 24 block-scope conflict resolutions as superseded intermediate history and describes the final declarations. All original notes and verification metadata remain preserved. The source summary distinguishes 50 file-scope bridge declarations from seven factory declarations while retaining the original 54-declaration reconciliation as historical evidence. No new compiler experiment or original filename claim is introduced.

The coordinator reviewed the draft scope and requested two ledger wording refinements, which were applied before the fresh proof below.

## Reconstruction dimensions

- Function and byte coverage: all 37 manifest functions, 5,680 text bytes, within the recorded ov081 range. Fresh explicit linked-byte checks return VERIFIED for each, with zero blind slots and empty difference lists.
- Methods and bridges: this repair changes comments only. It preserves the landed methods, free factory, and 57 ABI bridge declarations; it does not claim another reduction in bridges.
- Layout and source quality: the landed named fields and method calls are preserved. The class remains size `0x3f4` with the five owned subobjects. Four local shadow type definitions remain, with their existing measured constraints. This repair makes no layout, field-width, or executable-body change.
- Lifecycle/data: the real destructor, D1/D0 order, documented compiler-only D2 handling, vtable `+8` address-point adjustment, RTTI, separate initializer, and text-only ownership are unchanged. The manifest still declares no promoted data or bss; compiler-only data retains its recorded ownership/disposition.
- Attribution: the landed map contains all 37 TU `path#symbol` overrides. No move, rename, owner reassignment, or attribution-file change occurs here.
- Remaining scope: independent verification and coordinator integration of these provenance corrections. Further body, shadow-type, lifecycle, or shared-header reconstruction requires separately owned work.

## Proof

All checks below ran against the final four-file content in this commit, based on accepted input `26f54f8fc8faaf00568e86d33f37860a46470906`. `build/humanizer/static.json` records content SHA-256 values. The handoff itself was added afterward; no proved source or metadata content was changed afterward.

- Compiler wiring canary: `build_pin.verify(Path('src/func_ov006_020cb030.cpp'), 'func_ov006_020cb030', 0x020cb030, 0x104, 'ov006')`, exit 0, `(True, '2004/b56')`; `build/humanizer/canary.json`.
- Static preservation: comparisons against the accepted input found identical C++ text after removing comments and whitespace, identical manifest fields outside `notes`, every original note retained, and only four changed ledger rationale cells with module/address/from/to columns identical. `git diff --check` exits 0; `build/humanizer/static.json` records the comparisons and file hashes.
- Whole emitted TU: `python tools/tubuild.py --manifest build/humanizer/tu-manifest.scratch.json verify ov081/daGmch_c`, exit 0. All 37 functions MATCH; objisolate type/addend checks and relocation destinations are clean; emission order is ROM-ascending. The scratch manifest is a wrapper containing the current entry (`{"schema_version": 1, "entries": [entry]}`), so the tool does not overwrite tracked historical proof. Log: `build/humanizer/tubuild-verify.log`.
- Explicit linked-byte proof: for each row of the current manifest's `functions`, ran `python tools/linkcheck.py --c src/actors/daGmch_c.cpp --name <symbol> --addr <address> --size <size> --module ov081`. All 37 subprocesses exited 0 and returned `VERIFIED`, `blind: 0`, `diffs: []`. Exact expanded commands and results: `build/humanizer/function-links.json`. This is the relocation proof, not the legacy basename shortcut.
- Shared-header consumers: `python tools/affected_src.py include/daGmch_c.h --json`, exit 0; the only consumer is `src/actors/daGmch_c.cpp`, whose entire manifest was verified above. Log: `build/humanizer/affected-src.log`.
- Legacy prepush wrapper: `python tools/prepush_linkcheck.py --files src/actors/daGmch_c.cpp --json build/humanizer/prepush-linkcheck.json`, exit 0 but **NO-SYM**, zero verified, one warning. Its TU-basename lookup is not evidence of a pass; the explicit 37-function proof supplies the missing coverage. Log: `build/humanizer/prepush-linkcheck.log`.
- Emitted data: `python tools/romdata_check.py --files src/actors/daGmch_c.cpp --json build/humanizer/romdata.json`, exit 0. Nine records: five VERIFIED (168 bytes), four PARTIAL (32 compared bytes, short of configured extents), zero DIFFERS, zero UNNAMED. PARTIAL is not counted as VERIFIED. The class vtable and all four RTTI records are in the verified set; the four type-name records remain partial. Log: `build/humanizer/romdata.log`.
- Full-ROM module build: exit 0, analysis PASS. All 106 modules are exact; 11,192 source functions reproduce with zero mismatches. All 23 source-owned data claims reproduce; three BSS claims cover 172 NOBITS bytes. The production intact-TU gate passes with zero new symbol errors and exact storage aliases. Command: `python tools/rombuild.py -j16 --no-rom --report-json build/humanizer/rombuild.json`; log `build/humanizer/rombuild.log`, report `build/humanizer/rombuild.json`, exit record `build/humanizer/rombuild.exit.json`.
- The full-build command automatically generated a fresh strict ROM-gap control: its modules passed, its packed 16,777,216-byte ROM had SHA-256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, and its known symbol-check errors formed the baseline for the zero-new-errors comparison. The global emitted-data census reports 695 verified, 219 partial, four differing, and 399 unnamed records; those global totals are not the scoped TU result above. No separately requested scratch TU substitution or final production `.nds` packing run is claimed. The existing promoted production TU is covered by the explicit function checks and the production module build. Port/path rename checks are not rerun: no path or symbol rename occurs in this repair. No private PR validation or independent queue verification has run for this output.

Local artifacts are evidence retained with this worktree, not tracked inputs. A later log or independent result belongs in separately recorded evidence; do not amend this candidate merely to insert its own output SHA.
