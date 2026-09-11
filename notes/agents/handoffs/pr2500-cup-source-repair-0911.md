# Cup member repair — 2026-09-11

This continues PR #2500 at `d4d4013bffc661487ed2521cf7f376ebffb52cab`, whose
source composition base is `cd3a7eb5966a8758722bde1ec78f61abc4b5c126`.
The earlier independently accepted correction is preserved at
`f7fc175eb2bd77dfc2ec4a1cadbe311372063d99`, with its original source base
`51e74d3ea9b8f1a2342e944be6d7fa9d3c57e5ea`. Its handoff and all returned
findings remain historical evidence. The queue records this continuation's
candidate and actual producer identity; independent and final-main acceptance
are subsequent stages.

The Cup scene now uses its existing score, prompt, state and shuffle members.
A class-local `CupPosition { s32 x, y; }` replaces the raw three-position array,
and `InitResources` uses that array plus `mOnes`, `mIds` and `mFlags` directly.
The whole-object `Obj6e` view is removed. Member names are reconstructed from
uses, not recovered English spellings. Unresolved byte storage uses `unk_`
names rather than claiming padding. The manual factory and component/array
lifetime calls remain; the position type has no invented constructor.

All successful probes used the complete production TU, its pinned flags and
an explicit scratch-header include directory. Separate score, inherited-field,
position-array and seven own-member probes passed before their combination.
The combined source and the final prose successor both emit exactly the same
complete object as the immutable PR input:
`777dbb9b72e9e30a39ad032bc99a0c1f4e746a3a7f3eaeb7d29b61cb819525ab`.
This includes the helper bodies, destructor variants and emitted metadata.

The earlier StateShuffle experiments remain measurements of their recorded
input. They do not rule out the newly proven direct member stores. The current
source retains the cached earlier loads and explicit angle read-modify-write;
the separate speed negation remains byte-neutral. The two original Render
address forms and Behavior integer-address forms remain unchanged. Their prior
removal results are retained, not relabeled as fresh experiments.

The manifest formerly called ten canonical ancestor RTTI/type-name copies
independently source-enrolled. The actual complete delinks ranges contain no
source owner at any of those addresses, or at Cup's own three class-data
addresses. They remain ROM-supplied. Natural compiler output is compared at the
canonical addresses before isolation discards it. Symbol configuration and
compiler emission do not establish source ownership. No data/BSS ranges,
compiler-only policy, symbol identities, enrollments or attribution changed.

## Findings retained

| ID | Disposition in this continuation |
| --- | --- |
| CUP-01 | Fixed: both real virtual self-dispatch calls are retained. |
| CUP-02 | Fixed: real Cup PMF receiver, seven own slots and inferred naming are retained. |
| CUP-03 | Partial, narrowed: Obj6e and measured inherited/own member accesses are fixed; raw component and remaining tail/helper reconstruction stay with issue #2492. |
| CUP-03-P | Fixed: component extent remains unknown; live storage through 0x546d is acknowledged. |
| CUP-04 | Fixed: every epilogue path initializes cup; the exact 164-byte body is unchanged. |
| CUP-05 | Fixed: the five minigame Virtual50 contracts remain void; the separate actor contract remains int. |
| CUP-06 | Partial: manual factory/lifecycle, component arrays and address-named helpers remain with issue #2492. |
| SOUND-PROV01 | Fixed: the accepted Sound corrections are unchanged. |
| SOUND-RECON01 | Partial: the accepted Sound reconstruction limits remain with issue #2492. |
| TERESA-PROV01 | Fixed: the accepted member-slot correction is unchanged. |
| BASE-PROV01 | Fixed: accepted minigame/actor hierarchy and last-field corrections are unchanged. |
| BASE-PROV02 | Fixed: accepted cross-hierarchy prose corrections are unchanged. |
| CUP-07 | Fixed: current source/header prose no longer claims file-local linkage for global helpers, universal raw-offset requirements, or an obsolete Obj6e/member state. Historical observations remain at their original commits. |
| CUP-08 | Fixed: all thirteen canonical class-data ranges are accurately described as ROM-supplied at this input. |
| CUP-09 | Fixed within this scope: actual Cup position storage and measured existing-member access replace the whole-object shadow view and raw accesses without changing the complete object. |

[Issue #2492](https://github.com/tangosdev/sm64ds-decomp/issues/2492) remains open
for CUP-03, CUP-06 and SOUND-RECON01, owned by the humanizer/integrator follow-up
through @andrewboudreau. This leaf repair does not claim complete Cup or Sound
reconstruction, a general compiler limit, or a resolved component layout.

## Proof and provenance

Fresh `2004/b56` strict checks cover all **32 functions, 7,344 text bytes and
143 module-qualified relocation references**, with no blind words or differences.
All 32 isolation plans and actual whole-TU output/order policies pass. Emitted
metadata remains **7 VERIFIED and 6 PARTIAL**, with no differing record. Production
continues to own text only. The class size remains 0x5470. The position type
has an explicit checked eight-byte size assertion. The header gate now checks
25 commented fields with zero mismatches or unparsed declarations. Its first
run reported the new nested type as unknown until that size assertion was
added; the diagnostic and final clean result are preserved.

The earlier five-header return-contract review proved 171 source objects,
924 production slots, three separately classified unenrolled drafts and 20 TUs.
That proof keeps its original identities and limitations. This continuation
changes only the Cup header/TU, manifest prose and this handoff; it adds no shared
Base/Sound/Teresa/3DEsp interface change. Its current consumer and full-ROM results
are recorded separately in the exact candidate's queue evidence. Final live-main
composition, independent acceptance and terminal private PR validation remain
root integrator responsibilities.

Local artifacts are `build/cup-probes/`, `build/cup-final/proof.json`,
`build/cup-metadata-ownership.json`, and the corresponding probe/proof drivers.
They preserve the exact source/header inputs, complete objects and checks;
ROM/compiler inputs and queue receipts are not tracked.
