# Monkey native query lifetime and attachment matrices

This source slice continues #2410 and merged PR #3882. Class reconstruction remains partial. It is separate from the conditional grouped Actor caller checkpoint `845382a394395a8420483fe56c20fdaa5c52635a`, which remains preserved on its own branch.

The saved input is `a67bb58e8bee6fb75224a3ca8fce4cbe3585f75f`. Remote main `53dc648eaf7d24847f461a193a3f81203cf4d098` was fetched and merged normally, retaining the published source history. The final production-source freeze is `079bbca6ea017a311c4a97d8ea91d9e2e82e34bb`; subsequent updates to this record do not change production inputs. Task `issue2410-finish1015-cont2` retains the original producer session, inherited findings and prior checkpoints.

## Source and evidence boundary

The automatic `dBgCh_Lin` local now begins after the countdown early return. Its ordinary construction, calls and destruction reproduce the retail lifetime. Model and cap matrices use the existing shared flat `Matrix4x3` representation and ordinary assignment, eliminating the private 12-word matrix view. The cap matrix member is typed accordingly. The attachment pointer at Actor offset `0xc8` occupies the existing alignment gap in both C and C++ layouts; no padding object or class-size change is introduced. Its original frozen field layer is also retained by the separate grouped Actor owner. The cage-push accumulator has a use-derived name, without an asserted physical unit. Angle interpolation and fixed division use ordinary C++ calls.

Independent Astra inspection caught an undefined identifier introduced while removing a zero temporary from the cap reset. The final source uses explicit zero for both attachment and cap-ID resets. It independently compiles, preserves the entire saved Monkey raw ELF literally, and matches the latest-main control after only the exact Bundle945/950 compiler-local name correspondence. All 46 Monkey functions are strict VERIFIED, blind 0, diffs empty. The exact compiler-only helper policy remains bounded; no new ROM aliases or objects are admitted.

The fresh full-ROM gate passes on this corrected source: 106/106 modules exact, 11,334/11,334 functions reproduced, 334/334 source-owned data claims reproduced, 241 BSS claims checked, and intact TU storage/alias gates pass with no new symbol errors. A separate current-main control comparison rebuilds all 306 non-Monkey consumers and proves their complete raw ELF objects literally identical, without normalization. All source/control files and both old headers are pinned to the actual base. Thus their payloads, symbol attributes and typed relocations are unchanged. The complete generic pre-push sweep checks 5,661 functions across 307 files: 5,449 VERIFIED, 212 inherited BLIND warnings and zero blocking failures. Every warning belongs to a consumer in the literal whole-object equality set. No warning is relabeled as verified. The sweep executes the repository verifier unchanged: file checks run in parallel, and the large Player TU is partitioned by its exact symbol enumeration using the same verifier function code. Source/header inputs remain identical before and after the run. Final exact-head independent Source judgment follows the producer handoff; private PR validation is a separate gate.

Global advisory ROM-data results are separate: 974 verified, 1,537 partial, one differing symbol and 15 unnamed. The differing dCcAcPos vtable is in unchanged consumers covered by the complete-object equality proof. These are not converted into a blanket metadata pass. The earlier preliminary full-ROM run failed on the undefined identifier and is not acceptance evidence. Historical full-ROM results below do not stand in for current validation.

## Remaining reconstruction

The seven canonical scalar API migrations, genuine shared asset handles/lifecycles, meaningful helper names, and native Monkey globals/initializer/profiles and metadata ownership remain unfinished. The common.h and math/Matrix.h definitions of Matrix4x3 remain incompatible C++ types selected by include order. Native scalar and shared-handle experiments disprove several earlier compiler-impossibility claims, but their canonical migrations are separate changes requiring complete consumer proof.

The four-file reservation recovery is now explicitly authorized and applied, preserving the stopped tasks' other resources, ownership, history and evidence. It does not complete their source work or this issue. The next bounded interface slice is dClipper's scalar projection method; its private proposal is being checked without editing the active Tree owner's source. Keep #2410 open and leave #2412 untouched.

## Preserved earlier checkpoint history

# daMky_c finishing checkpoint

WIP continuation of issue2410 and merged PR3882; this checkpoint does not complete the issue or accept production integration.

Base: df618e80271212106c9f1e225756f691abaa498b. Task issue2410-finish1015, producer codex-mky-finish-producer1015. Source is in the wired worktree C:/tmp/sm64ds-mky2410-finish1015, branch cpp/mky2410-finish1015. Independent Astra scouts have made no production edits.

Completed source changes in this checkpoint:

- Native dBgCh_Lin local constructed after the reset-timer early return; ordinary method calls and destructor now reproduce the retail lifetime.
- Native shared flat Matrix4x3 assignment and a typed mCapMtx field, replacing a private matrix view and raw scalar array. The repository's incompatible common.h/math/Matrix.h type definitions remain a shared-source defect; this checkpoint explicitly does not solve it.
- dActor_c::mCarryMatrix recovered at0xc8 in the existing alignment gap, without changing field count or class size. Monkey cap/player access uses this pointer.
- Cage-push accumulator named mCagePushProgress from observed increments; no unit is asserted and unobserved padding stays padding.

Evidence (ignored local build artifacts): native-checkpoint-verify.log reports46/46 MATCH, clean exact compiler-only policy, clean isolation, clean relocation destinations, correct order. actor-header-equivalence.json reports306/306 other production source raw ELF objects identical; the remaining consumer is the intentionally changed Monkey TU. This is not a fresh full-ROM or private CI pass.

Remaining: seven shared scalar API migrations, meaningful helper names, genuine owned globals/initializer/profiles and nontext production enrollment, canonical matrix type review, full-ROM and module-aware consumer checks, independent exact-commit source/byte review and terminal private validation. Conditional scalar maps are experiments, not production acceptance. Metadata tool reports4 VERIFIED/5 PARTIAL; complete vtable slots and NUL-terminated strings are measured separately, with alignment tails identified rather than hidden.

Shared API/rename work is pending explicit recovery of four bookkeeping reservations whose stopped owners confirmed missing receipts. Review-only plan: build/queue-resource-recovery-plan.json. No foreign resources or queue ownership have been changed. Preserve all old checkpoints and evidence. Do not start2412 or close2410 at this checkpoint.


## Verified continuation and remaining dependency (2026-10-15)

Merged refreshed remote main e209dacfaaa95cb5490637ff053d62182689bebe normally into this branch; the accepted input remains an ancestor. That base refresh changed contributions.json only. Native UpdateAngle and cstd::fdiv calls now replace their two hand-mangled imports. Pinned19.1.7 formatting preserves token streams and the COMPLETE raw Monkey ELF object; no codegen identity mask was used.

Fresh full-ROM check:106/106 modules exact,11334/11334 functions reproduced,332/332 source-owned data claims reproduced,0 mismatches, intact-TU gates pass and no new symbol errors. The declaration ratchet reports no new disagreements/local copies; the changed-line style check has0 findings/0 deferrals. Only the owned manifest verification metadata was refreshed through a scratch manifest. Full hashes and report scope are in notes/experiments/mky-finish1015.json. This is local byte proof, not private CI or final Source acceptance.

The old native line/matrix failures are now disproved by this source. The seven scalar interface probes also succeed with genuine native callee bodies, so the old Fix12 wall must not be carried as compiler impossibility. Their canonical migration is still unapplied. Independent Astra Source review rejected four provisional Monkey-specific file-handle lifecycle aliases; a real shared8-byte model and seven native lifecycle bodies now match conditionally, including the full1196-byte initializer. Shared layout migration reaches264 sources, including391 duplicate-storage wrappers in199 source files. Active actor owners retain their leases; no foreign paths were edited.

Native global candidate,58 owned objects and163 nontext relocations are preserved in build/completion-scout/preview and interface-finish/native-handles. They are experiments, not accepted production. The shared handle/lifecycle migration must precede native global production enrollment. The separate tooling task issue2410-terminal-bss-tool1015 preserves four linker-owned BSS alignment bytes using independently configured actual object size, without padding objects; its own producer/verifier stages and PR are separate.

Keep2410 OPEN and do not start2412. Next action after the explicit reservation recovery decision: coordinate actual shared API/type migrations with current actors' owners, preserve existing accepted outputs and debt, compose and revalidate the completed source. User authorization for the four-file administrative recovery remains pending. Recovery review artifact: build/queue-resource-recovery-plan.json. No Source-stage completion has been published for this WIP task.

Exact full-ROM source checkpoint was a24cdcbf48348bd1ce9bca8f8ffea4a9ef1d462f. The later formatting is covered by token equality and complete raw Monkey object equality; subsequent manifest changes refresh verification metadata only. The experiment record keeps that bridge explicit instead of representing a fresh full-ROM execution on every documentation/formatting commit.
