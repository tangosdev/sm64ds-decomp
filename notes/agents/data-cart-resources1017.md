# ov047 cart resource data

This candidate continues issue #4018 and the global-data work in PR #4015.
Source base: e8dc126bcda1865cce7a4a4cb920fb2095deefb9. Tool dependency:
PR #4025, 121ca67be3 (independently reviewed implementation 0e6f7941f1). Producer task:
`data-cart-resources1017`; producer session `codex-data-cart-producer1017`.

The descriptive names are reconstructed from consumers, not recovered identifiers:

| Address (ov047) | Name | Layout |
| --- | --- | --- |
| 02112408 | gKm3KurumaResources | Three pointers: model handle, collision handle, CLPS block; 12 bytes |
| 02112638 | gKm3KurumaModelFile | u16 filesystem ID, u8 reference count, padding, file pointer; 8 bytes |
| 02112630 | gKm3KurumaCollisionFile | Same handle layout; 8 bytes |
| 02111ab4 | gKm3KurumaClps | CLPS magic, u16 stride, inferred u16 count, one 8-byte property entry; 16 bytes |

The initializer uses runtime asset handles 0x67f and 0x680, resolved to NitroFS
IDs 0x4d0 and 0x4d1: `data/special_obj/km3_kuruma/km3_kuruma.bmd` and `.kcl`.
These handle IDs and filesystem IDs are different namespaces. The KCL has twelve
prisms, all using CLPS entry zero. The lookup at 020381cc checks stride eight and
indexes from header+8; it does not check the inferred count halfword. Existing
CLPS accessors describe the property bits; this change does not invent bitfields.

The local handle fields follow src/SharedFilePtr.cpp and the constructor at
02017e0c. SharedFilePtr.h remains fieldless, so the local wrapper storage is needed.
Compiler-generated RTTI, vtable, initializer, and destructor-registration objects
retain their ABI identities. Historical partitioned-link records retain their old
symbol spellings; this candidate's fresh proof must be read separately.

## Source ownership and linker layout

The CLPS object is assigned to the source that consumes it. Its original TU and
input-section name are unknown. This is an explicit reconstruction choice.
The compiler emits the const object in .rodata, and the complete cart entry now
claims exactly 02111ab4..02111ac4. The module's previous data section is split into:

- .data_pre 021119e0..02111ab4: unchanged mutable ROM-gap storage.
- .rodata 02111ab4..02111ac4: the compiler-emitted CLPS object.
- .data 02111ac4..021125e0: the original remainder, with existing source claims.

This avoids duplicate same-named input sections and does not claim intervening
objects. The following sixteen FF bytes remain padding in the ROM gap. DSD lcf
and delink accept the segmentation. Static inventory retains all prefix addresses
and bytes, but the PR4015 provenance request API currently supports only standard
.data/.rodata/.bss names; a future prefix migration must account for that limitation.
The descriptor's opaque CLPS_Block pointer follows the existing API; observed
SetFile, CLPS_BlockRef, lookup, and property consumers only read this const block.

## Validation and continuation

Fresh whole-TU admission on the tool dependency is SCRATCH-DATA-VERIFIED:
all six licensed section ranges reproduce from one object, all 106 module binaries
are exact, and the complete 16 MiB ROM has SHA256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`,
identical to the independently built retail reference. Strict symbol checking
still reports the same nine control errors, zero new; it is not globally green.
The interior CLPS entry label now resolves to the compiler-defined block at +8.

Focused relocation checks verify five retail functions; the generated initializer
has the existing BLIND-6 warning and is instead covered by whole-object and full
ROM evidence. Declaration agreement reports no new disagreements or local header
redeclarations. All 247 port references resolve.

The final default production build passes: 11,334/11,334 source functions,
339/339 complete data claims, 245 BSS claims (31,216 bytes), and 106/106 exact
modules. Its full ROM has the same retail SHA256 above. The four requested
objects all have verified compiler providers: descriptor and CLPS initialized
bytes match; the two handles have exact NOBITS storage and symbol layout.
The provider audit alone does not establish runtime initialization or pointer
semantics; the whole-TU linked initializer, relocation checks, and consumer
research supply that separate evidence.

Private receipts: build/cart-clps-rombuild.json, build/cart-clps-provenance.json,
build/cart-clps-production-guarded.log, build/cart-prepush-final.json, and
build/tu/ov047-daObjKm3_Kuruma_c/link/linkcheck.json. Source object SHA256:
`2e1c1f7af9617ff13b75a0932c4a2d889e1fbe3df5f7869ca6e6d65e2818c911`.
The broad optional ROM-data scan still reports the unchanged unrelated
`_ZTV10dCcAcPos_c` difference from its two legacy destructor objects. All complete
source data claims and all final module bytes pass. No ROM-wide data-completion
claim is implied. Ten unrelated manifest serialization changes were verified
JSON-equivalent and restored before the production audit. The temporary
Windows audit wrapper needs a guarded main for multiprocessing; its first unguarded
run was stopped and discarded before it established production evidence.
The wider global-data backlog, sibling collision blocks, and all ROM gap padding
remain separate work. No whole-ROM source completion or original-name recovery is
claimed. Publication is a PR for review; merging is outside this task's authority.

## Formatting review follow-up

Finding `cart-style-001` requested the pinned formatter on the newly added lines.
clang-format 19.1.7 now reports zero findings and zero deferrals. Before/after
complete raw compiler objects are byte-identical (8,352 bytes), SHA256
`1dec8f239c7a7e06494dfe0d8c49cee3940837da9faa5218554418ba49c3f3d8`.
The formatted source was then freshly whole-TU linked: all six ranges, all modules,
and the full retail ROM pass, with the same nine baseline symbol diagnostics.

The current standalone verification summary was also regenerated instead of
retaining an older unlicensed-definition count. It reports 5/5 exact retail
functions and clean relocation destinations, but refuses raw promotion for eleven
unlicensed symbols (the initializer and ten base RTTI/type-name objects), with
zero compiler-only policy errors. The normal TU pipeline separately licenses the
initializer and externalizes those base definitions to their canonical homes;
its successful six-range/full-ROM proof remains the production admission gate.
Historical partitioned-link evidence is preserved. The corrected production
receipt uses build/cart-clps-production-formatted.log; the standalone result is
build/cart-standalone-current.log. An empty-string reset attempt exposed the
existing verification-summary helper's indexing assumption; it was discarded,
and removing the stale field allowed the normal tool to regenerate its result.

The corrected final default production run and all four provider requests pass.
Its full ROM again has the retail SHA256 above.
