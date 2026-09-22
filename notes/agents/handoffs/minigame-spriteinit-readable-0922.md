# Handoff: minigame-spriteinit-readable-0922

This document describes this commit. The queue records its immutable output SHA.

## Identity and completion boundary

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2924.
- Producer/coordinator: `codex-spriteinit-readable-0922` (Codex); stage `produce`.
- Branch: `cleanup/minigame-spriteinit-readable-0922`; wired worktree `C:/tmp/sm64ds-mg-spriteinit0922`.
- Original source base, exact stage input and task workflow/tool revision: `9c6104f1f4df5554b19b3a72a9a32d0eba9361d0`.
- Active independent-review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`, distinct from that source/tool pin.
- Status: locally proved producer candidate, awaiting independent verification. Reconstruction completion remains **partial**.
- Exact reserved scope: `src/func_ov004_020b3278.cpp`, ov004 `[0x020b3278,0x020b35d8)`, one 864-byte function, and this handoff. No shared header, config, baseline, source-path or symbol changes.
- Next owner: an independent verifier assigned by the minigame coordinator, followed by the integrator for current-main composition and PR delivery. This producer does not self-verify acceptance or merge main.

## Source changes and supporting evidence

`DecompressLZ16` now has its actual `void(void*, void*)` contract, as defined in `src/DecompressLZ16.c`. Its six calls explicitly cast the existing integer resource and VRAM addresses to pointers. Address storage, arithmetic, call order and destination offsets remain unchanged.

The resource loader `src/func_ov004_020b2cb8.c` fills the 29-entry `data_ov004_020bf560` array with loaded resource addresses; `020b2c84` deallocates those pointers. The initializer argument is therefore named `resourceIndex`, not a file ID. The draw chain `020b369c -> 020b1c68 -> 020af868/020afc18 -> OAM::Render` establishes `x`, `y`, `screen` and `priority`: the dispatcher selects both/one screen and supplies a separate fixed palette argument. `initialState`, `stateIndex` and `stateEntry` describe the existing state-table accesses. These are descriptive reconstruction names, not recovered original names.

`tileOffsets` names the three-word copy of `data_ov004_020bc27c`. A fresh ROM read yields `0x380, 0x300, 0x340`, corresponding to the destination byte offsets `0x7000, 0x6000, 0x6800` divided by 32. The actual copy helper adjusts the low ten tile-index bits of each eight-byte sprite record by this value. Exact hashes of the eleven inspected sources/headers and this observation are retained in private `build/spriteinit-readable/contract-evidence.json`.

The existing default-arm branch form is preserved. Its inherited source comment attributes the form to avoiding a literal-pool load hoist; this task did not repeat an alternative-branch experiment and does not claim a newly measured compiler barrier. PMF call/storage representation remains unchanged. At the preliminary reviewer's request, a private-copy control and three exact simplification trials were compiled: removing the first `b ? a : a`, the second, or both. The control matches the complete baseline ELF; the first removal changes 13 raw function bytes, and the second/combined removals each change 17. All remain 864-byte functions in 1,960-byte objects. Neither simplification is adopted. These three trials establish a measured constraint under this compiler/flag pin, not a claim that every alternative rewrite must fail. Complete sources, objects, logs and hashes are retained in `build/spriteinit-readable/probes/` and `probes/results.json`. The `a`/`b` temporaries remain because their existing lifetimes span VRAM addresses and later PMF words; they were not given misleading VRAM-only names.

Preliminary independent source review by the coordinating root found no blocker in the pointer contract or grounded names and accepted retention of the two expressions after reviewing those measurements. Formal independent acceptance is still the next queue stage.

## Producer proof

The actual pinned compiler is `2004/b56` with `build_pin.flags_for` production flags: `-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off`.

- Fresh before/after raw compiles (`python build/spriteinit-readable/compile_objects.py baseline` and `final`) produce identical complete 1,960-byte ELF objects, SHA-256 `c97f05630d126aa47af6da5f607f91a375aaf51995b17847a12aba18525190c8`. This includes symbols, relocations and section tables, not just an isolated function comparison. The sole allocated section is the 864-byte `.text`; no lifecycle, vtable, RTTI, initializer or non-text data ownership is added.
- `python tools/prepush_linkcheck.py --files src/func_ov004_020b3278.cpp --json build/spriteinit-readable/strict-working.json`: exit 0, 1/1 VERIFIED, `diffs: []`, `blind: 0`.
- `python -u tools/rombuild.py -j16 --report-json build/spriteinit-readable/rombuild.json --data-json build/spriteinit-readable/romdata.json`: exit 0, 11,209/11,209 source functions reproduce; 106/106 exact modules; 26/26 source-data claims and 3 BSS claims (172 bytes) pass. The actual 16 MiB ROM was packaged at private `build/sm64ds.nds`, SHA-256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`. This full build reused 7,810 production objects and compiled two; the complete before/after initializer object compiles were fresh.
- The global raw-data census retains 775 verified, 272 partial and 3 distinct differing symbols across 6,068 object records: the existing `dCcAcPos_c` vtable and `daDemo_c::anmModel_c`/`simpleModel_c` RTTI in eight unrelated unchanged records. This is not a globally clean raw-metadata claim. The changed object has no allocated non-text data. Intact-TU linking reports zero new symbol errors; its control retains nine historical symbol-check errors.
- `python tools/check_decl_agreement.py --changed 9c6104f1f4df5554b19b3a72a9a32d0eba9361d0`: exit 0, no new disagreements; 380 expanded files and 1,757 scoped banked disagreements. No baseline was edited.
- `python tools/port_refcheck.py --json build/spriteinit-readable/port.json`: exit 0, all 408 references resolve.
- `python tools/tiers_ratchet.py --check`: exit 0, baseline 2,995/current 3,008.
- `python tools/langmode_audit.py --check build/spriteinit-readable/langmode-baseline.json --json build/spriteinit-readable/langmode.json`: exit 0, PASS against the public baseline pinned at `81db3eb6095df8800597bc015da4312c8045d6de`. The downloaded baseline is private evidence, not a repository override.
- `git diff --check`: PASS. Final committed-range strict/declaration and attribution checks are recorded after this commit in `build/spriteinit-readable/producer-evidence.json`, without rewriting this snapshot to embed its own SHA.

Private proof directory: `build/spriteinit-readable/`; complete controls, candidate object, source/callee identity report and gate logs remain available there. ROMs, compiler inputs, raw objects and private queue receipts are not committed as source artifacts. No source move or symbol rename changes existing credit. No shared-header consumer edit is involved; the declaration gate performs its own broader type/declaration expansion.

## Remaining findings and next work

- `SPRITEINIT-01` (correctness, fixed): the decompressor declaration and all six call boundaries now express their actual pointer contract.
- `SPRITEINIT-02` (reconstruction, fixed): resource, coordinate, screen, priority, state and tile-offset local names follow the inspected data use.
- `SPRITEINIT-03` (reconstruction, deferred): the raw address-named C entry, numbered `Obj`/`Base` virtual facades, raw object offsets, integer global-address storage, `Pair`/PMF layout, mixed-role temporaries and wider class/TU identity remain partial. The minigame coordinator retains issue #2924 for a separately reserved continuation.
- `SPRITEINIT-04` (reconstruction, deferred): the initializer still calls `func_ov004_020af5e0(int, void*, int, int)` with the resource index in the fourth position, while its actual production definition is `int func_ov004_020af5e0(Item*, Item*, int)`. The extra argument and raw first-argument type were preserved, not justified as a truthful complete API. Separately, `020b0cac` declares the initializer's coordinate arguments as `int` while the actual initializer takes `short`; those widths also remain unchanged. The coordinator must reserve the relevant caller/callee interfaces and prove a future reconstruction before altering them. This banked boundary debt is distinct from the corrected decompressor contract.

- `SPRITEINIT-05` (reconstruction, constrained): both redundant PMF-word conditionals remain after the three measured simplification trials above changed the complete object. This finding preserves the experiment scope and artifacts; it does not extend the historical default-arm claim.

No independent verifier verdict, private PR validation, PR publication, current-main composition or merge is claimed. Normal producer publication offers this exact candidate for independent source and byte acceptance; later stages record their own evidence.
