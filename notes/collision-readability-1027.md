# Collision query readability, 2026-10-27

Local worktree: `C:/tmp/sm64ds-collision-readability1027`, branch
`cpp/collision-readability1027`, starting at
`69a9d665276e996f91f209b9032024a2c4bb9ef1`.

This PR is a checkpoint in the folder-wide cleanup. The method names below are inferred
from implementation and callers, not recovered Nintendo function spellings.
The class identities already have ROM RTTI evidence in their headers.

## Named interfaces and evidence

| Address | Interface | Evidence |
| --- | --- | --- |
| arm9 020354b0 | `dBgCh::IsDetectingOrdinary` | Filter's fallback accepts ordinary surfaces only when mask 1 is set. |
| arm9 0203547c | `dBgCh::IsDetectingWater` | Filter checks mask 2 for CLPS water bit 5. |
| arm9 0203543c | `dBgCh::IsDetectingToxic` | Filter checks mask 8 for CLPS toxic bit 25; daPly_c uses that surface predicate to update its toxic height. |
| arm9 02035468 | `dBgCh::EnableCameraMode` | Camera query initializer 0200897c always sets mask 4 after updating vanish behavior. |
| arm9 0203545c | `dBgCh::IsCameraMode` | The filter uses mask 4 to reject CLPS bit 24 and accept bit 26. |
| arm9 02037e2c | `CLPS::IsCameraPassThrough` | Paired camera-mode filter branch rejects these surfaces. |
| arm9 02037e14 | `CLPS::IsCameraOnly` | Paired branch rejects these surfaces when camera mode is absent. |
| arm9 02037464 | `dBgCh_Gnd::ResetResult` | Both ground registry scans reset the hit record, height sentinel, and hit byte before visiting colliders. |
| arm9 02037608 | `dBgCh_Lin::ResetResult` | Line setup restores the full endpoint/distance and clears the previous polygon hit. |
| arm9 02037b5c | `dBgCh_SphCrr::ResetQuery` | Sphere setup and actor reset clear displacement, flags, hit slots, and floor normal. |
| arm9 02037940 | `dBgCh_SphCrr::SetQueryFlags` | Moving/scaled mesh scans copy flags to their local sphere while clearing the floor/wall/under contact bits 0x1c. |
| ov002 020fea68 | `dM3dGLin::GetStart` | Copies the three words at line offset 0 into the caller result. |
| ov002 020fea4c | `dM3dGLin::GetEnd` | Copies the three words at line offset 0xc into the caller result. |

The flag predicates retain integer mask returns. Normalizing them to `bool`
would be a separate ABI/code-generation change. Both endpoint getters return
`Vector3`: mwccarm supplies the hidden result buffer in r0 and `this` in r1.
`lineEnd = GetEnd()` followed by `Vector3 startPoint = GetStart()` reproduces
the original local/temporary stack slots and call destinations exactly.

The camera setup caller uses `dCamera_c`, `daPly_c`, and `dBgCh` headers.
On the publication base it already lives in the promoted `d_camera.cpp`;
this change replaces its remaining raw camera-mode helper call in that TU.
Math calls use `math/CalcLib.h`, and the query/collider deallocators use
`Memory.h` instead of copying its declarations.

## Shared-header effects and publication base

The branch was rebased onto `3f77042948a870780ed0cad960f87ce8f0982d4e`
on 2026-10-29. Upstream camera promotion, actor cleanup and renamed callbacks
are preserved. `Memory.h` retains the base's declaration spelling.

Complete before/after objects for 325 affected manifest-backed translation
units compare equal in allocated section payloads, kind, flags, size and
alignment, defined-symbol attributes/order and typed relocation destinations.
Semantic helper renames resolve to the same addresses. Named prototype
parameters advance this compiler's local-symbol counter even when they do not
alter machine code; including the shared allocation declarations and removing
local helpers therefore requires source-identity updates in 156 manifests.
The mappings include 44 static-local identities in four TUs that previously
used their raw compiler names directly. Only compiler-local source mappings
change; retail destinations are unchanged.
The comparison report is `build/collision-publish-control/comparison.json`.

The earlier checkpoint evidence below describes the original base and is
retained as history. Publication-base verification is recorded separately.

## Validation

First checkpoint: 264/264 focused functions VERIFIED (23,840 bytes, no blind
relocations); full production build 106/106 exact modules, 11,380 reproduced
functions, intact-TU symbol/storage gates passing. Declaration agreement,
port references, dead references, language-mode ratchet and source style pass.
Reports are in this worktree under `build/collision-*.log` and
`build/collision-*.json`. The focused link check covers all directly changed
source files; the full production build covers shared-header consumers too.

## Collider property continuation

The next pass names 15 more helpers, for 28 named interfaces across both passes:

| Address | Interface | Caller evidence |
| --- | --- | --- |
| 02039624 | `dBgW::Reset` | Base constructor and both mesh SetFile implementations initialize owner, callbacks, clip settings and disabled slot sentinel. |
| 020395fc | `dBgW::SetActor` | Enable records both owner pointer and unique ID. |
| 02039404 / 020393fc / 020393f0 | `GetSlotIndex` / `SetSlotIndex` / `ClearSlotIndex` | Enable/Disable maintain the 24-entry registry; clearing the index alone does not unregister. |
| 020393b4 / 020393ac | `GetActor` / `GetActorUniqueID` | Every ground/line/sphere registry scan identifies ownership and writes the collider identity into its hit record. |
| 020393a4 / 0203939c | `SetClipRadius` / `GetClipRadius` | Actor setup writes the distance used by query culling; -0x1000 selects the owning actor's clip volume. |
| 02039394 / 0203938c | `SetClipOffsetY` / `GetClipOffsetY` | Actor setup writes the vertical offset added by those same culling tests. |
| 020393cc / 020393bc | `GetBeforeClsnCallback` / `GetStoodOnCallback` | Hit dispatch tests the first callback before the virtual BeforeClsn call, and invokes the second as (collider, owner, actor). |
| 020396d0 | `dBgW_Kc::SetDepthDirMinDot` | Moving mesh and actor setup select the minimum face-direction dot product used by the sphere scan. |
| 020396c0 | `dBgW_Kc::SetEdgeToleranceShift` | Three actor setup callers set the edge tolerance; the helper clamps negative shifts to zero. |

The changed source/header files use declared property members;
the hit callback dispatch uses the shared `StoodOnCallback` type and actor
pointers rather than integer-address views. The callback setters remain pending.

Complete before/after objects for 246 affected manifest-backed units compare
equal in allocated bytes, section kind/flags/size/alignment, defined-symbol
attributes/order and typed relocation destinations, with renamed interfaces
resolved to the same addresses. Removing local named-parameter declarations
renumbers compiler-local symbols in 23 manifests; only their source identities
were updated, leaving every retail destination unchanged. The report is
`build/collision-properties-control/comparison.json`.

Second checkpoint validation before rebasing:

- Full `rombuild.py -j16 --no-rom`: 106/106 exact modules, 11,380/11,380
  reproduced functions, 395/395 source-owned data claims; intact-TU symbol
  errors zero and storage aliases exact.
- 499 affected functions / 67,824 bytes verified, zero unresolved relocations.
  The raw function verifier covers 474 directly. Its 25 static-initializer
  cases require the manifest transformations and module-local anonymous-symbol
  addresses: checking those same functions from the final production objects
  closes all 233 raw unresolved slots. Report:
  `build/collision-properties-focused-production.json`.
- Declaration agreement: no new disagreements or local header redeclarations.
- Source style (including pinned formatter), port references, dead references,
  language-mode ratchet and `git diff --check`: pass.

## Publication-base validation, 2026-10-29

Against base `3f77042948a870780ed0cad960f87ce8f0982d4e`:

- `python tools/rombuild.py -j16 --no-rom`: PASS. 106/106 exact modules,
  11,380/11,380 reproduced functions, 438/438 source-owned data claims;
  intact-TU module gates pass, zero new symbol errors, storage aliases exact.
  Log: `build/collision-publish-rombuild.log`.
- Complete-object comparison: all 325 affected manifest-backed TUs preserve
  allocated bytes, section properties, defined-symbol attributes/order and
  typed relocation destinations; compiler-local mappings are reconciled as
  described above.
- Declaration agreement: no new disagreements or local header redeclarations.
- Source style: 49 files, zero findings and zero formatting deferrals.
- Port references: 227 checked, all resolve. Dead references, language-mode
  ratchet and `git diff --check` pass.

The PR's independent `validate` check still determines CI byte acceptance for
its candidate/base. These results do not claim folder-wide source acceptance.

## Remaining folder work

This change does not complete the folder-wide objective. Remaining work includes
`dBgCh_Actr` address-named accessors/workers, `dBgW` callback setters and registry helpers,
mesh-coordinate transforms and KCL helpers, the CLPS water/toxic helpers and their
actor callers, sphere hit forwarders, and collision-list/clipper helpers.

The sphere hit forwarders currently preserve an `int` triangle ID while
`dBgPi::RecordHit` is declared with `s16`; resolve the interface from callers and
bytes before removing that bridge. Unknown virtuals and flags still need evidence.
No blanket extern removal or invented original names is justified by this pass.
