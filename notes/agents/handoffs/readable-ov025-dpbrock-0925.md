# Handoff: readable-ov025-dpbrock-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3080, task `readable-ov025-dpbrock-0925`, stage `revise`, role producer, session `claude-prod-readable-ov025-dpbrock-0925`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov025-dpbrock-0925`, input `7423f8d73f391870e7244a19599eba122f3596d5` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `7423f8d73f391870e7244a19599eba122f3596d5`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments are recorded below as source diffs with their measured results.
- Next action, responsible role and blockers: independent verification (byte, relocation, whole-object and source review) of this commit. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov025/daObjDpBrock_c`, `src/actors/daObjDpBrock_c.cpp`, ov025 .text 0x02111d40..0x021120e4 (9 functions).
- Reserved source/header/config surfaces actually touched: `src/actors/daObjDpBrock_c.cpp`, `include/daObjDpBrock_c.h`, `config/arm9/overlays/ov025/symbols.txt` (two rows renamed in place), `config/tu_manifest.d/ov025/daObjDpBrock_c.json` (two `symbol` fields and the notes that described the old helpers and the operator new leaf), and this class's two keys in the `attribution.json` ledger. `delinks.txt` holds only address ranges and needs no edit.
- ROM observations: none new. The class identity and layout evidence is unchanged from the promotion.
- Lineage evidence or structural inference: `func_ov025_02111dec` and `func_ov025_02111e30` take the object in r0 and are only called with it, from InitResources and Behavior. They are written as members. Nothing else in `src/` calls them; ov025 relocations reach them by address.
- Hypothesized names/filenames, explicitly not recovered facts: `daObjDpBrock_c::UpdateStepClsnPosAndRot` (0x02111dec, symbol `_ZN14daObjDpBrock_c23UpdateStepClsnPosAndRotEv`) and `daObjDpBrock_c::UpdateStepModelPosAndRotY` (0x02111e30, symbol `_ZN14daObjDpBrock_c25UpdateStepModelPosAndRotYEv`) are coined names, after dBgActor_c's UpdateClsnPosAndRot and UpdateModelPosAndRotY, which do the same work for the inherited collider and model. The header says they are coined.
- Compiler experiments and measured barriers: each run with `python tools/tubuild.py verify ov025/daObjDpBrock_c` and then handled as shown.

| ID | Finding | Change tried | Result |
|---|---|---|---|
| E1 | DPB-F4 | Deleted the class's own `operator new` and its `extern "C" _ZN7fBase_cnwEj` prototype, so `new` binds the inherited `fBase_c::operator new(size_t)` in `include/fBase_c.h` | 9/9 MATCH; object undefined symbols (51), defined symbols, section bytes and relocations identical. Adopted. |
| E2 | DPB-F2 | Both helpers written as members, symbols.txt rows renamed in place | 9/9 MATCH; object identical once the two old names are mapped to the new ones. Adopted. |
| E3 | DPB-F1 | `Fix12<int> scale = {0x1000}; mMeshCollider.SetFile(file, mClsnMat2, scale, mAngleY, data_ov025_02112ce8);` | 8/9; InitResources DIFFs and the aggregate emits an extra 4-byte local `.data` section. Reverted. |
| E3b | DPB-F1 | The same call with `Fix12<int> scale; scale.val = 0x1000;` | 8/9; InitResources grows from 0x10c to 0x118 bytes. Reverted. Wall 6az holds for this TU. |

## Deferred findings from #3080

| Finding | Outcome | Owner | Reason |
|---|---|---|---|
| DPB-F1 Fix12 calls | Still deferred | next daObjDpBrock_c readability task, https://github.com/tangosdev/sm64ds-decomp/issues/3080 | E3 and E3b measure wall 6az for `dBgW_KcMbg::SetFile`. `dBgActor_c::IsClsnInRange` has no declaration in `include/dBgActor_c.h`, a header this task does not hold. The source comment now records the measured size. |
| DPB-F2 extern "C" char * helpers | Fixed | producer, this task | Now members `UpdateStepClsnPosAndRot` and `UpdateStepModelPosAndRotY` (E2). |
| DPB-F3 func_020393d4 bridge | Still deferred | holder of `src/func_020393d4.c` and `include/dBgW.h`, https://github.com/tangosdev/sm64ds-decomp/issues/3080 | The definition is `(int *, int)`; a typed callback spelling would disagree with it and needs the definition changed, which is outside this reservation. |
| DPB-F4 operator new forwarder | Fixed | producer, this task | Removed; the inherited `fBase_c::operator new` gives identical code (E1). |
| DPB-F5 dead extern | Still deferred | holder of `include/decl_common.h` (`jump-contract-repair-0918`), https://github.com/tangosdev/sm64ds-decomp/issues/3080 | `include/decl_common.h` is off limits. It now carries three stale lines for this class: `_ZTV11PyramidStep` (line 447) and the `(char *)` declarations of the retired names `func_ov025_02111dec` (line 1443) and `func_ov025_02111e30` (line 2559). Nothing calls either. |
| DPB-F6 banked decl entries | Still deferred | holder of `src/__sinit_ov025_02112a44.c`, https://github.com/tangosdev/sm64ds-decomp/issues/3080 | `data_ov025_02113ab0` and `data_ov025_02113ab8` are `SharedFilePtr` here and `int []` in `src/__sinit_ov025_02112a44.c`, which is not in this reservation. The baseline file is unchanged. The `Matrix4x3_FromRotationY` entry keyed to the retired one-function source of `func_ov025_02111dec` is stale and is left in place. |
| DPB-F7 registry row | Still deferred | integrator or registry refresh, https://github.com/tangosdev/sm64ds-decomp/issues/3080 | `symbols/profile_reconstruction_registry.json` is not in this reservation. |
| DPB-F8 unnamed data | Still deferred | next daObjDpBrock_c readability task, https://github.com/tangosdev/sm64ds-decomp/issues/3080 | The two file handles are also named by `src/__sinit_ov025_02112a44.c`. `data_ov025_02112ce8` (the CLPS block) is only named here, but no overlay symbols.txt gives coined names to CLPS data, so there is no convention to follow. State names and the factory name stay inferred, as the source says. |
| DPB-F9 daDpLift_c.json prose | Reported, not edited | holder of `config/tu_manifest.d/ov025/daDpLift_c.json`, https://github.com/tangosdev/sm64ds-decomp/issues/3080 | That file still says "(PyramidStep, RTTI daObjDpBrock_c)". It is not in this reservation. |

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 9/9 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: 8 member functions (D1, D0, InitResources, Behavior, Render, CleanupResources, UpdateStepClsnPosAndRot, UpdateStepModelPosAndRotY) plus the C-linkage factory. The remaining bridges are the two mangled Fix12 calls (DPB-F1) and `func_020393d4` (DPB-F3).
- Recovered layout/fields; remaining shadow structs/raw offsets: unchanged; no `unk_` fields.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. Two non-virtual members add neither layout nor vtable slots, and removing the class's operator new leaves the factory's call target `_ZN7fBase_cnwEj` unchanged.
- Attribution preserved through each move/rename: the two `attribution.json` keys follow the renamed symbols with the same author. prepush_attribution reports 0 changed and 0 lost.
- Remaining agreed issue scope: DPB-F1, F3, F5, F6, F7 and F8 as in the table; F9 reported.

Residue in `src/actors/daObjDpBrock_c.cpp`, counted as lines containing the token (the brief's convention):

| | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|
| Input commit | 188 | 0 | 14 | 4 | 10 |
| This commit | 181 | 0 | 3 | 2 | 12 |

`_ZN` rises by two because the two helpers' marker lines now carry their mangled member symbols; no bridge was added.

## Proof

All commands were run in the worktree on source commit `3d236db5d3`, which this document sits on top of. This document is the only later change.

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov025/daObjDpBrock_c` (input commit) | 0 | 9/9 MATCH, objisolate clean, reloc destinations clean |
| `python tools/tubuild.py verify ov025/daObjDpBrock_c` (this commit) | 0 | 9/9 MATCH, objisolate clean, reloc destinations clean, all 9 in ROM order |
| Object comparison of `build/tu/ov025-daObjDpBrock_c/daObjDpBrock_c.o`, input commit against this commit, with pyelftools, the two old helper names mapped to the new ones | n/a | 51 and 51 undefined symbols, an identical set with no mapping needed; 23 of 23 defined function and object symbols identical in binding, type, value, size and section; 22 of 22 sections byte-identical; 15 of 15 relocation sections identical in offset, type, addend and target. Without the mapping the only differences are the two renamed definitions. |
| `python tools/rombuild.py -j16` | 0 | 7189 enrolled, 7189 compiled, 0 reused. `build/src/actors/daObjDpBrock_c.o` was rebuilt and defines both new member symbols. 11,213 functions reproducing, 0 mismatching, 106/106 modules exact. `intactTuRom.identical` is true, sha256 `d1506e90...c478e8`. The baseline control step reports the same pre-existing `dsd check symbols` errors (arm9 main and ITCM) before any TU is substituted. |
| `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD` | 0 | 9 checked, 9 VERIFIED, 0 blocking |
| `python tools/prepush_attribution.py --base 7423f8d73f --head HEAD` | 0 | 0 changed, 0 lost |
| `python tools/check_decl_agreement.py --changed 7423f8d73f` | 0 | no new declaration disagreements |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check` | 0 | PASS, baseline 2994, current 3115 |
| `python tools/check_tubuild_conflicts.py` | 0 | 293 entries, every conflict recorded |
| `python tools/queue_audit.py --check-promoted` | 0 | every row agrees with config/tu_manifest.d |
| `python tools/check_src_tu_compiles.py` | 0 | 293/293 compile |
| `python tools/port_refcheck.py` | 0 | 408 checked, all resolve |
| `python tools/affected_src.py include/daObjDpBrock_c.h` | 0 | only `src/actors/daObjDpBrock_c.cpp` consumes it |
| `git diff 7423f8d73f..HEAD -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` | 0 | empty |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify and prepush_linkcheck, above.
- Complete emitted TU and data/metadata checks: this TU licenses .text only. The object comparison covers every section, including the vtable and RTTI data; romdata_check was not rerun.
- Shared-header consumer expansion: `include/daObjDpBrock_c.h` has one consumer, this TU.
- Port/path/reference and other applicable static gates: above. Generated files (`docs/index.html`, `docs/progress-treemap.svg`, `notes/data/c-cpp-classification.tsv`, `notes/data/tu-merge-candidates.json`) still name the old helper symbols and refresh with their generators.
- Private validation, if run, and the exact PR head/base it tested: not run.
