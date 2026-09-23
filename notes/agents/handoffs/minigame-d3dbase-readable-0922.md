# Shared 3D minigame base readability: first pass

This handoff describes this commit; the queue records the immutable output.

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2915.
- Task: `minigame-d3dbase-readable-0922`, reconstruct stage.
- Producer: `codex-minigame-d3dbase-readable-0922` (Codex).
- Branch/worktree: `cleanup/minigame-d3dbase-readable-0922`, `C:/tmp/sm64ds-mg-d3dbaseread0922`.
- Source base/workflow: `b93e467b9b7b6a75a32a547fcd1158399a3525dd`.
- Scope: production `src/actors/dScMgD3DBase_c.cpp`, 27 functions in ov006 `[0x020e6c28,0x020e7660)` (2,616 bytes), plus this handoff.
- Status: local producer candidate; independent exact-head review remains required. No source branch push, PR or merge is claimed here.

## Changes

Replace raw mangled base-call declarations with the existing qualified C++ methods. `AfterInitResources` explicitly forwards its incoming success argument, and the two forwarding int overrides return the inherited base contract. This does not establish a meaningful original result for slots 28/29: the shared header records that uncertainty. Calls retain direct base dispatch, not virtual redispatch.

Use the recovered menu and camera-selector fields in `OnKicked`, remove reconstructed self aliases, name the OAM entry/sprite/tile traversal and camera-address locals, and shorten obsolete generated inventories and repeated child-name correction prose. Keep the class identity, inline destructor/key-function constraints, all 25 source markers for 27 emitted functions, and the manifest's medium-confidence original TU boundary. The ten free helpers remain free helpers.

Use native GX/GXS/G2S/CP15/Particle namespace declarations from the existing callee definitions. Local declarations suffice; no shared header is invented or edited. The decompressor uses its actual pointer signature. Constant bank selections use the native u16 interfaces; four saved-bank restorations retain their existing full-word C-linkage ABI bridges for the measured reason below. The two Particle tracker calls remain ABI bridges because Stage.h's shared tracker view lacks those method declarations. Update uses its actual void result. Camera_UpdateMatrices takes an opaque Camera pointer at the existing complete-scene address boundary. The OAM comment acknowledges the existing OamAttr.h view without claiming that no shared OAM header exists.

## Measured constraints and remaining reconstruction

- `D3DBASE-READ-01`, fixed: native base calls and existing fields preserve the complete compiler object. Explicit success forwarding and qualified base calls remove unsupported local arity/receiver declarations.
- `D3DBASE-READ-02`, fixed: SDK calls use native namespace spelling and actual callee contracts where they reproduce. CP15's address argument and G2S's result are u32 in their existing definitions; explicit pointer/address boundary casts remain.
- `D3DBASE-CODEGEN-01`, retained measured experiment: converting every saved-bank restoration to native u16 arguments grows `Virtual7C` from 260 to 276 bytes and `OnAimedAtWithEggReturnVec` from 120 to 136 bytes. Both relocation layouts change. `native-sdk-all.{cpp,o,json}` is the rejected source; `native-sdk-retained-bank-bridges.*` and the final object show the successful retained form. The four raw calls are existing ABI reconstruction debt, not claims that the actual SDK definitions accept u32. A later layout/call-boundary investigation owns them under issue #2915.
- `D3DBASE-CODEGEN-02`, retained measured experiment: remove the int/long-long/int index round trips in `func_ov006_020e7428` and its body grows from 224 to 232 bytes with different relocation placement. `oam-simplification.{cpp,o,json}` retains the exact rejected source/object; restoring those three expressions recovers the identical complete object. This tests that specific simplification, not every possible implementation.
- `D3DBASE-READ-03`, fixed after independent review: correct the tracker Update result, the Camera_UpdateMatrices pointer parameter and the obsolete OAM-header rationale. Separate `review-tracker` and `review-camera` experiments and the combined `review-repaired` candidate preserve the complete baseline object.
- `D3DBASE-CODEGEN-03`, retained measured experiment: the existing LoadCompressedFileAt definition takes u16, while this caller retains its historical wide file-ID boundary. `review-fileid.{cpp,o,json}` changes only Virtual7C from 260 to 276 bytes; the combined all-contracts experiment reproduces that growth. This is explicitly deferred table/call-boundary reconstruction under issue #2915, not an assertion that the callee accepts u32.
- `D3DBASE-RECON-01`, partial: the original file cut, saved-bank types/calls, file-ID tables/calls, camera and OAM views, tracker method declarations, provisional slot names/returns and unnamed helpers/globals remain unresolved. Camera addresses stay relative to the complete scene; a 0xa0-byte padding array is not represented as a complete 0xbc camera. The minigame coordinator owns this remaining work under issue #2915. The wider second pass follows the complete first-pass sweep.

## Local proof

The baseline was compiled from the immutable base before editing. Baseline and final complete raw objects are identical: **15,720 bytes**, SHA256 `e18d3f77f3e8957fb707fa62fba31606e6f54f466c19b5343ac8e380e5d9d575`, using `match.compile_c` with `build_pin.flags_for` and pinned `2004/b56`. This comparison includes all emitted data, symbol records and relocations, not masked text alone. Private snapshots and reports are in `build/d3dbase-readable/`.

- `prepush_linkcheck.py --files src/actors/dScMgD3DBase_c.cpp`: **27/27 VERIFIED**, no blind relocations or differences; `strict.json`/`strict.log`.
- `rombuild.py -j16 --report-json build/d3dbase-readable/rombuild.json --data-json build/d3dbase-readable/rombuild-data.json`: **11,208/11,208** source-built functions, **106/106** exact modules, **26** initialized-data claims and **3** BSS claims, zero new stock symbol errors; actual ROM packaged. Subsequent comments, local-name/formatting edits and equivalent tracker argument spelling, and the independently reviewed tracker/camera contract corrections were compiled again and preserve that identical complete object; no second full build is claimed.
- `romdata_check.py --files src/actors/dScMgD3DBase_c.cpp`: **6 VERIFIED / 200 bytes**, **5 PARTIAL / 52 compared bytes**, zero DIFFERS/UNNAMED. Partial type-name coverage is unchanged; no new ownership claim.
- `check_decl_agreement.py --changed b93e467b9b7b6a75a32a547fcd1158399a3525dd`: no new disagreements. Banked interface debt remains.
- `check_tubuild_conflicts.py ov006/dScMgD3DBase_c`: all records agree. No conflict marker was removed without its recorded resolution.
- `git diff --check`: clean. Exact committed-range relocation, credit and port results are recorded in producer evidence before publication.

No header, manifest, symbol, enrollment, attribution or baseline file changed. No source path moved. Whole-tree metadata still contains three unrelated differences; this is not universal metadata acceptance. Independent source review and private PR validation remain separate requirements.

## Current-main composition proof

The producer composed observed main `1e2d7dcc692060f35458355d21ab7efb1ac0dc09` before handoff. Source, headers relevant to this TU and its complete raw object remain identical after the merge. The original task/workflow pin above remains historical; this section records the newer tested base.

A fresh `rombuild.py -j16` on composition `e01d8ab67473640b05070b2ae49bf0855eae977c` packaged the 16,777,216-byte ROM with SHA256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`: 11,208 matching functions, 106/106 exact modules,26 initialized-data claims,3 BSS claims and zero new stock symbol errors. Exact-range prepush verification passes all27 functions with no blind relocations or differences. Declaration agreement has zero new disagreements (502 scoped banked entries), port references408/408, attribution8,052 tracked with zero lost credit. Current-base reports are in the private `current-main-*` producer artifacts.

The only follow-up to that tested composition is this handoff update; no source or other build input changed. Whole-tree metadata still has the same three unrelated differences. Source reconstruction remains partial, and formal independent verification and PR acceptance remain separate.
