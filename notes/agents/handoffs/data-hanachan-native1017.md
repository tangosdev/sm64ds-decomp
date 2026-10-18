# Hanachan native callback data

This commit reconstructs the complete 288-byte Which Wiggler? callback/profile
group at `ov006:0x0213c95c..0x0213ca7c` in the existing production methods TU.
The 35 member-function pointer constants and eight-byte `BaseSpawnInfo<void>`
profile become compiler output through complete `.data` delinks ownership.
This is a modern reconstruction assignment; the original TU boundary and
original variable/method spellings remain unproven.

Task: `data-hanachan-native1017`, producer `codex-hanachan-native1017`,
[issue #4028](https://github.com/tangosdev/sm64ds-decomp/issues/4028).
Base: `b7acba2fb5aed0e887869ba72ff60f7909afc2f6`, accepted Memory2
[PR #4027](https://github.com/tangosdev/sm64ds-decomp/pull/4027), including
the long ELF-symbol-name support from
[PR #4024](https://github.com/tangosdev/sm64ds-decomp/pull/4024).
Independent root verification and publication follow this producer stage.
No merge is authorized by this task.

## Reconstruction and evidence

Nine Wiggler callbacks and two empty scene phases now have genuine native member
definitions. The shared class header types the three Wiggler callback fields and
the scene phase as member-function pointers. Assignments, comparisons and dispatch
use C++ expressions. The previous CB/Self and encoded-pair shadows are removed;
Retarget now uses the existing complete Wiggler record directly. The Point layout,
0x98-byte Wiggler layout, scene size, and previously agreed ABI declarations remain.

The pinned compiler emits eight-byte constants containing a method relocation and
a zero adjustment/virtual word. Repeated targets are identified by the baseline
consumer function and exact relocation offset, not by their identical payloads.
All 35 constants have unique correspondence to their configured ROM labels.
The profile uses the existing factory and two unsigned 16-bit priorities, both
0x182. The preserved PR #3937 candidate at
`7268d28db7515fd30509c1f261b532407dd297d1` covered only the first 33 constants;
the final Result and TurnTowardAim records are included here because their retail
bytes and current consumers establish the complete contiguous run.

The exact old/native/address map and record formats are in
[the experiment record](../../experiments/data-hanachan-native1017.json).
No callback rows exist in the reserved global `symbols/actor_renames.tsv`, so this
owned evidence file records the new mappings. Existing ledger chains are unchanged
and its checker passes. `function-authors.json` retains the existing address-keyed
credit; the retired attribution file is unchanged. No old callable identifiers
remain in current source/header definitions or calls. This mapping location is a
deliberate scoped alternative to adding new rows to the separately owned ledger.

## Verification

Completed producer measurements:

- Fresh raw object: all 64 emitted function bodies and canonical relocation
  destinations equal the base, including the initializer and compiler helpers.
- All 36 eight-byte initialized records manually relocated against configured
  targets and compared equal to retail, covering the complete 288-byte range.
- Standalone `tubuild verify`: 61/61 declared functions match; relocation
  destinations, isolation and emission order are clean. It retains one existing
  raw `.text` initializer classification refusal, with zero compiler-policy
  errors. This is not a standalone promotion pass.
- Declaration agreement: no new disagreement or local header redeclaration.
- Rename ledger, port references and dead-reference checks pass.
- Raw `romdata_check`: seven verified, six partial, zero differing records.
  This raw report covers the existing named metadata and profile; the 35 local
  compiler literals are covered separately by the complete record audit above.

Full scratch link: all five owned ranges, all 106 modules and the complete
16 MiB ROM reproduce. ROM SHA256 is
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
The strict symbol check retains the nine baseline errors, with zero new errors;
it is not claimed clean.

Default production: 11,334/11,334 functions, 340/340 complete initialized-data
claims and 106/106 modules reproduce. Fresh before/after link provenance proves
all 37 requested objects (36 data records and existing sprite BSS) come from
`build/src/minigames/d_s_mg_hanachan_methods.o`. The full ROM is identical.
The optional broader raw ROM-data diagnostic retains the existing four-byte
`_ZTV10dCcAcPos_c` difference in its two legacy destructor objects, unchanged
from accepted Memory2; it is outside this claim. All requested Hanachan records
pass. Pinned source style has zero findings or deferrals.

The final immutable-commit strict check is recorded in the producer queue
evidence before publication. Independent verification remains required.

## Remaining work

Completion is partial. Stable finding `hanachan-raw-layout-001` is deferred to
[issue #4028](https://github.com/tangosdev/sm64ds-decomp/issues/4028), owner
`codex-hanachan-native1017`: codegen-sensitive raw field/loop spellings, the private
StartPathA scalar view, popup shadow records and manual factory/lifecycle remain.
Old matching observations in source comments are inherited evidence, not new
experiments for this data slice. Native callback recovery is complete for this
range; those other reconstruction tasks remain explicit.

The scalar words at 0x0213c954/0x0213c958, RTTI/vtable starting at 0x0213ca7c,
and popup BSS remain outside the new data claim. The existing 44-byte sprite BSS,
164-byte initializer and ctor entry remain enrolled. In particular, this change
does not claim that the entire historical TU or the global-data backlog is done.
