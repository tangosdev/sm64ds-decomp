# Single-camera 3D minigame base readability: first pass

This handoff describes this commit; the queue records any published immutable output.

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2918.
- Task/session: `minigame-single3dbase-readable-0922` / `codex-single3d-readable-0922`.
- Worktree/branch: `C:/tmp/sm64ds-mg-single3dread0922`, `cleanup/minigame-single3dbase-readable-0922`.
- Source/workflow base: `d033c646262e878ce234bafd37d6b5b3abf40179`.
- Scope: `src/minigames/d_s_mg_single3_d_base.cpp` and this handoff; nine functions in ov006 `[0x0210a4b0,0x0210a8c0)` (1,040 bytes).
- Status: producer candidate with local proof complete; independent source/byte review remains required. No source branch push, PR or merge is claimed.

## Changes

Use native GX/GXS/CP15/G3X namespace calls and local declarations matching their existing definitions. CP15 takes an integer address, G3X takes bool, G2S::GetBG3CharPtr returns u32, LoadFile returns int, and DecompressLZ16 takes two non-const pointers. Address conversions are explicit at the boundary. These are current definition contracts, not uniquely recovered original declarations.

Name both temporary loaded buffers fileData, preserve load/decompress/palette/free order, and retain all MMIO access widths and read-modify-write sequences. Replace stale shadow/not-enrolled and raw-name/no-header claims with the present production contract and concise behavior comments. The source now describes the emitted complete 36-slot vtable (152 bytes including its two-word preamble), rather than the obsolete 34-slot prefix.

The seven explicit bodies and compiler-owned D1/D0 pair keep their source/emission order. Existing qualified base calls and native tracker calls remain intact. No shared header, manifest, symbol, enrollment, attribution or baseline change is included.

## Findings and remaining work

- `SINGLE3D-READ-01`, fixed: native SDK call spelling, local definition agreement and loaded-buffer names preserve the complete compiler object.
- `SINGLE3D-PROV-01`, fixed in source: obsolete shadow/enrollment, pointer-as-true-contract and short-vtable narratives are removed. Linker naming and register width alone do not recover original C++ types.
- `SINGLE3D-ABI-01`, deferred reconstruction: func_ov004_020adc68 currently spells an incomplete void(void) forwarding wrapper around LoadFile, whereas this caller supplies a file ID and consumes a buffer result. Preserve both arguments/result and the trampoline relocation; changing the caller to match that incomplete wrapper or bypassing it is not justified. A separately reserved wrapper/caller repair remains under issue2918 with the minigame coordinator.
- `SINGLE3D-HEADER-01`, deferred shared-interface reconstruction: this class header's Particle tracker view declares s32 Initialise/Update, while actual definitions and the separate tracker header return void. The calls here discard both results. Multiple shared views and consumers need coordinated header ownership; no conflicting local method declaration was added. Existing shared-header short-vtable prose is also outside this source-only scope and still needs correction.
- `SINGLE3D-RECON-01`, partial: opaque camera/tracker layout, original callback/global/helper identifiers and exact original file cut remain unresolved. The minigame coordinator owns continuation under issue2918; this checkpoint does not finish that reconstruction or the wider second pass.

## Proof and limits

The unchanged base and final candidate were compiled with `match.compile_c`, `build_pin.flags_for` and pinned 2004/b56. Their complete raw objects are identical: **9,272 bytes**, SHA256 `d8c2029388fcd6a9d500ad77ec6745ec535704c2299ebf814dd9822d7a76e59b`. This covers symbols, metadata and relocations, not only masked function bytes. Private baseline/final snapshots and reports are under `build/single3d-readable/`.

- `prepush_linkcheck.py --files src/minigames/d_s_mg_single3_d_base.cpp`: **9/9 VERIFIED**, zero warnings, blind relocations or differences.
- `romdata_check.py --files ...`: **6 VERIFIED / 200 bytes**, **5 PARTIAL / 56 compared bytes**, zero DIFFERS or UNNAMED. Partial inherited type-name coverage remains unchanged.
- `check_decl_agreement.py --changed d033c646262e878ce234bafd37d6b5b3abf40179`: no new disagreement; 67 scoped banked rows remain.
- `check_tubuild_conflicts.py ov006/dScMgSingle3DBase_c`: all records agree, no conflict notes or markers.
- `port_refcheck.py`: 408 references resolve; `git diff --check` passes.
- `rombuild.py -j16 --report-json build/single3d-readable/rombuild.json --data-json build/single3d-readable/rombuild-data.json`: full packaged ROM **PASS**, **11,208/11,208** source-built functions, **106/106** exact modules, **26** initialized-data claims, **3 BSS claims / 172 NOBITS bytes**, zero new stock symbol errors. The build reused7,874 objects and compiled1. Whole-tree metadata remains773 VERIFIED /272 PARTIAL /3 DIFFERS /274 UNNAMED; this is not universal metadata acceptance.
- Committed-range `prepush_linkcheck.py` passes **9/9**, blind0, diffs empty. `prepush_attribution.py` reports **8,052 tracked**, zero changed or lost credit. Both use source checkpoint `c2200500eed6619c49d5a74768cfcd7bafa0cf23` against immutable base `d033c646262e878ce234bafd37d6b5b3abf40179`; this follow-up changes only the handoff. Source and all build inputs remain identical to that tested checkpoint.
- `tiers_ratchet.py --check`: PASS (2,995 baseline /3,000 current). Language-mode ratchet: PASS against live chaos-data baseline `f3c0b367fc902958e2770e04ebcdbb16e9769240`. The installed worktree example omits the required `--check PATH`; that initial syntax-only error is retained separately and was corrected without changing any baseline.

No failed new source experiment occurred: the combined native-declaration/name cleanup and final comment/format follow-up both produced the identical object. Historical inline-destructor and header-offset-parser constraints in `notes/minigame-provenance.md` were read, not rerun; no universal impossibility is inferred. Preserve the inline destructor/key function and shared header layout until separately measured work proves a replacement.
