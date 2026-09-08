# Handoff: issue-2492-cup-source-review

This continues promoted `ov006/dScMgCup_c` from [PR #2348](https://github.com/tangosdev/sm64ds-decomp/pull/2348) and [scout #2343](https://github.com/tangosdev/sm64ds-decomp/issues/2343). [Issue #2492](https://github.com/tangosdev/sm64ds-decomp/issues/2492) owns the bounded correction and remaining reconstruction. The queue records the immutable final commit.

- Original source base/input: `51e74d3ea9b8f1a2342e944be6d7fa9d3c57e5ea`. The first checkpoint `0b76f9b4c970b3b9d0dc97a5637b95336341e89b` and its proof remain preserved; independent rework returned CUP-05 and CUP-03-P before the coordinator amended the same task.
- Current producer: `codex-cup-humanizer-producer-20260908-02`, Codex; branch `cpp/humanizer-cup-0908`. Prior producer session: `codex-cup-humanizer-producer-20260908-01`. Installed workflow: `6ef1dfb896e6074e83d2aa1a03cc3b701d07dcae`; byte tooling is from the original source base.
- Expanded ownership: five minigame class headers and their five slot-20 source definitions, complete Cup/Sound text ranges, Cup manifest and this handoff. The task reserves the base, Cup, Sound, 3DEsp and Teresa together. No actor contract, slot-21 declaration/body, source enrollment, attribution, shared helper definition or data ownership changes.
- Status: producer byte/source correction complete; **independent source acceptance and integration remain pending**. CUP-03 typed fields, CUP-06 lifecycle and remaining Sound/interface reconstruction are explicitly partial work owned by issue #2492, humanizer/integrator support through @andrewboudreau.

## Findings

| ID | Current disposition |
| --- | --- |
| CUP-01 | Fixed in the preserved first checkpoint: two real virtual `OnYoshiTryEat` self-calls replace the counterfeit twenty-slot `Obj` receiver. |
| CUP-02 | Fixed: current prose describes the real `dScMgCup_c::*` PMF receiver and seven own slots, while qualifying inferred member names. |
| CUP-03 | False naming/live-storage restrictions corrected. Typed reconstruction of `Obj6e`, inherited `mHudScore` accesses and raw tail fields remains owned by issue #2492; original English names are not a prerequisite for measured typed fields. |
| CUP-03-P | Fixed: live bytes reach 0x546d, not 0x5469. The component constructor writes scene 0x50dc and 0x50e0, so 0x19c is a raw-storage declaration boundary, not a proven component extent; `pad_50e0` is not unused. All layout declarations remain unchanged. |
| CUP-04 | Fixed in the first checkpoint: `cup = i` precedes both epilogue jumps, and the final-arm assignment remains. Every incoming source path initializes the local. The prior false “hazard closed” justification is gone. |
| CUP-05 | Fixed coherently: all five minigame slot-20 declarations/definitions are `void`. 3DEsp/Teresa also declare the actual `FreeGfxSlotsById` helper as void and call it without inventing a result. Base is empty; Cup/Sound forward to the actual void component helper. The name and return type remain a supported reconstruction, not recovered source text. |
| CUP-06 | Partial reconstruction: manual factory/lifecycle calls, component and array byte storage, runtime array helpers, mangled constructors and address-named interfaces remain. Fresh natural forms require their own measured continuation under issue #2492. |
| SOUND-PROV01 | Fixed in header and TU: constructor/destructor calls establish a component start/lifetime, not ownership of the whole 0x6f4 tail. Live scene state reaches 0x5628. Split literals are a measured source form, not recovered original spelling. Render has seven calls, six taking the scene. The PMF entry no longer claims one word, and the 0x0211a5ec helper address typo is corrected. |
| SOUND-RECON01 | Partial reconstruction: counterfeit PMF/SceneVt receiver views, manual decoders, raw arrays and empty-condition scaffolding remain owned by issue #2492. No untested compiler ceiling is asserted. |
| TERESA-PROV01 | Fixed: own-slot list includes slot 18, and slot 34 is accurately described as an existing declared member. |
| BASE-PROV01 | Fixed: minigame observations do not disprove the separate actor interface. Slot-19 naming is qualified; `mSceneKind` at 0x465e corrects the stale last-field claim. Earlier slot-21/27/28 observations remain historical, with no declaration/body change or new proof claim for those contracts. |

Sound still contains counterfeit receiver views, PMF casts, raw state storage and empty-condition scaffolding. Their retention is a reconstruction boundary, not a demonstrated compiler barrier. This task changes Sound's slot-20 contract and the specifically reviewed provenance; it does not claim that the entire class has been humanized. Cup retains its eight genuine State methods, five other non-destructor overrides, D1/D0, fourteen address-named helper bodies, two empty callbacks and manual factory. Historical scout facts and prior manifest measurements are preserved.

## Return-contract evidence and limits

The base hook is `bx lr`; Cup and Sound call `void func_ov006_020c2594(void*)`; 3DEsp and Teresa call the actual void `FreeGfxSlotsById(8)`. The confirmed minigame caller `func_ov004_020b6ddc` dispatches on the global populated by the scene constructor, already uses a void function pointer, and overwrites r0 after the call at `ov004:0x020b6e84`. This supports the coherent void reconstruction together with the callee bodies and exact compiler proof; discarded r0 alone would not recover a return type.

The distinct actor caller `func_ov002_020edb3c` tests r0 after `ov002:0x020edc28`, and `dActor_c::Virtual50()` returns 1. That hierarchy remains int. The recorded direct-load scan inspected 11,372 configured ARM and 26 Thumb functions in 93 available modules. It excludes missing/unsized configuration, ARM7, separately computed/indexed dispatch and opaque callbacks; it is not exhaustive caller proof. All five unchanged caller/helper/actor-control sources received fresh strict relocated-byte checks here.

The superseded slot-18 int/int table and seven-for-eight actor comparison are removed from the current header. The [original 51e header](https://github.com/tangosdev/sm64ds-decomp/blob/51e74d3ea9b8f1a2342e944be6d7fa9d3c57e5ea/include/dScMgBase_c.h#L155) preserves the earlier slot-21 int/void experiment and separate slot-27/28 body/caller observations. Those measurements were not repeated as signature experiments here and do not transfer an actor contract to the minigame hierarchy. Slot 21 retains its existing declaration and a concise statement of current uncertainty.

CUP-04's successful local form was researched by `inventory_remote`, then independently recompiled by this producer. Hoisting its assignment, using `i` directly, or assigning at the epilogue gives 0xa0 instead of the required 0xa4 bytes; those remain failing controls. The two per-arm assignments preserve the exact 0xa4-byte function. The coherent CUP-05 research patch was also contributed by `inventory_remote`; neither contributing producer/researcher should accept its own output.

## Producer proof

The pinned compiler is `2004/b56`; C++ production flags are `-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off`. The five unchanged controls use their own pinned language modes. Source/header executable-token guards permit only the five return-contract corrections and two local helper declaration/call corrections after the preserved first checkpoint.

- The five-header include closure is **171 sources: 148 production standalone files, 20 promoted TUs and 3 unenrolled InitResources drafts** (Slot1, Slot3, Smartball). All configured symbols resolve uniquely. `rombuild.enrolled()` authoritatively classifies **924 production function slots / 216,112 text bytes**; the complete source-qualified inventory has 927 slots.
- Fresh original-base headers/sources and the final candidate each pass **927/927 strict VERIFIED, zero blind checks/differences**. All function bodies, typed relocation records and raw metadata dispositions match the fresh control. The three draft successes do not make those sources production inputs.
- All 20 promoted consumers are covered completely: 18 text-only whole-TU audits pass, including **Cup 32/32 and Sound 82/82**; both Trampoline TUs pass the actual intact production object audit. The latter proves **620 owned data bytes and 112 relocation destinations** at `ov006:0x0213faa0..0x0213fd0c`, with zero differences or unresolved ownership. No data claim was added.
- Header offsets: **78 commented fields total, zero mismatches/UNPARSED**; spans remain Base 0x4660, Cup 0x5470, Sound 0x562c, 3DEsp 0x5558 and Teresa 0x4c28. Compile-time size assertions still hold.
- Changed-source raw metadata: **9 VERIFIED (368 bytes), 7 PARTIAL (80 equal bytes), zero DIFFER/UNNAMED**. These are diagnostic dispositions; raw RTTI/vtable passengers retain their prior ownership policy.
- Port references: **423 checked, zero stale**. Attribution and every original symbol override are unchanged; exact-commit attribution and supplemental prepush results accompany queue evidence after commit.
- Full production build actually compiled the **168 affected production sources**, reusing 8,806 unchanged cached inputs. Packing passes **106/106 exact modules; 11,199 reproducing functions, zero mismatches; 25/25 production data claims; 3 BSS claims / 172 bytes**. The actual 16,777,216-byte ROM has SHA-256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, identical to stock.
- Final review added only comments in Base/Teresa/Sound after packing. A token guard confirmed no executable change, then **all 171 sources were freshly recompiled and every complete raw object hash matched the strict/TU/packed-ROM input**. `final-prose-binding.v2.json` binds the final source hashes through the preserved first binding to those proof inputs; the preceding source snapshots remain preserved.

The first expanded attempt incorrectly invoked text-only isolation for the two already intact Trampoline consumers. It refused their licensed data equally in control/candidate; those diagnostic failures remain preserved. The final runner uses the actual intact production policy and checks every owned byte/relocation, without skipping or changing a manifest. Global raw metadata remains 704 VERIFIED, 223 PARTIAL, 4 DIFFER and 389 UNNAMED, and the 9 inherited symbol diagnostics remain with zero new errors. These are not universal metadata/symbol passes.

Proof and command hashes are in ignored `build/cup-expanded-final/`, including `plan.json`, `consumers.json`, `consumer-summary.json`, complete TU logs, `caller-helper-controls.json`, `metadata-cli.json`, `port.json`, `packed-rom.json`, `rom-command.local.json` and `final-prose-binding.v2.json`. The old `build/cup-proof-final/` and initial expanded attempt remain preserved. Receipts, compiler/ROM inputs, object files and local machine paths are excluded from tracked/public evidence.

Next: `inventory_worktrees` must freshly claim the exact offered verify stage, inspect the complete changed source/interfaces and every finding above, and independently prove the final candidate. Partial reconstruction must retain its owner and limits. Queue publication precedes independent verification; PR publication, current-base composition, terminal private validation, Source review activation and integration are separate coordinator/integrator responsibilities. No source branch push, PR or merge occurs in this producer stage.

## BASE-PROV02 continuation after independent rework

The independent verifier returned exact `63638f45e0ecb31c938219d7c3bb4590000eb565`
for current base-header prose. The same task resumed at that checkpoint with a
fresh producer lease; the original base remains
`51e74d3ea9b8f1a2342e944be6d7fa9d3c57e5ea`.

**BASE-PROV02 is corrected in this candidate:** slots 22–26 no longer call actor
parameter lists wrong based on unrelated minigame bodies. Slots 24–29 no longer
refer to the removed slot-21 comparison table or use cross-hierarchy agreement
as signature evidence. The recorded scene body/caller observations and the
historical int/void experiment remain, with their limits. Two stale OnKicked
caller references now correctly identify slot 24. Every declaration, field,
layout and executable token is unchanged; no signature or name was revised.
All earlier findings and the partial reconstruction ownership above remain.

Fresh `2004/b56` production compilation of **all 171 affected source files** gives
complete raw objects identical to the preserved `63638` producer proof input.
`build/cup-prov02/token-object-binding.json` records each source/object hash,
the unchanged executable tokens and the hash of the preceding binding. Thus the
927 strict source-qualified checks, 924 production slots, all 20 complete TUs,
620 owned data bytes / 112 relocations and packed stock-equal ROM result above
remain bound through identical objects. This comments-only step does not claim
a fresh ROM repack or replace independent verification. The original checkpoint,
both prior bindings and all proof logs remain preserved.

Next: offer this exact child through the existing task; a different verifier
must freshly claim it, inspect BASE-PROV02 and all prior dispositions, bind its
own independent proof and finish the remaining gates. PR/integration and source
review activation remain separate from producer publication.
