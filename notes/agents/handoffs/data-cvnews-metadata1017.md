# CvNewsLift native metadata

This commit makes the existing ov021 actor TU provide the 204-byte range
`[0x02114740,0x0211480c)`: four platform yaw values, a typed actor profile,
and the compiler-generated RTTI, type-name string and full vtable. Existing
methods only change references to the named heading table; the class header
is unchanged. Completion is partial; independent review is required.

The base is `121ca67be3ac06751cc6d8106ff6487a76dc52fd`, the accepted PR4025
initialized-interior helper. Task `data-cvnews-metadata1017` and issue #4048
record ownership and continuation; workflow is
`18af52b49b9fb5663d417bdfa86109b7b157aaa3`. Parent campaign root independently
verifies the immutable candidate and publishes the source PR. No merge.

| Object | Address | Storage | Evidence |
|---|---|---:|---|
| `sNewsLiftPlatformYaws` | 0x02114740 | 8 | `const u16[4]`: 0, 0x8000, 0x4000, 0xc000; Behavior and both transform loops |
| `_ZTI17daObjCvNewsLift_c` | 0x02114748 | 12 | compiler single-inheritance typeinfo |
| `_ZTS17daObjCvNewsLift_c` | 0x02114754 | 20 | compiler type-name string |
| `g_profile_CV_NEWS_LIFT` | 0x02114768 | 28 | `SpawnInfo<daObjCvNewsLift_c>` and typed factory |
| `_ZTV17daObjCvNewsLift_c` | storage 0x02114784/public 0x0211478c | 136 | eight-byte ABI preamble and 32 slots |

The heading name describes observed consumers; it is not a recovered historical
identifier. Compiler ABI names stay unchanged. Modern source assignment does
not prove an original filename or original TU boundary. The exact old/new map
is `data_ov021_02114740` -> `sNewsLiftPlatformYaws` at ov021:0x02114740;
there is no existing global rename-ledger row for it. Existing ledger chains
and address-keyed authorship are untouched.

The module keeps its previous regions as `.rodata_pre` and `.data_pre`, with
their original kind/address/alignment, and exposes the new 8-byte `.rodata`
followed by `.data`. The complete source claims are exact. Existing configured
interiors `data_ov021_02114750` and `data_ov021_021147c4` are preserved as
zero-size labels in final ELF section `OV021`; they are not extra objects.

Proof: 32 existing emitted definitions, including 21 FUNC symbols, retain their
full bytes and canonical relocation identities. All 204 data bytes and 37
module-qualified destinations match retail. The inherited Kill slot's actual
configured destination set is `overlays(2,7)`. Focused strict checks verify
19/19 configured functions with zero warnings or blocking findings.

Fresh final-config control and full three-section scratch link reproduce all 106
modules and the 16 MiB retail ROM SHA-256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
Normal default production also passes, with five actual final-link source
providers. Their public extents total 196 bytes; the eight-byte vtable preamble
is additionally covered by the whole-section proof. Nine inherited symbol
audit errors remain, with no new errors. The optional ROM-data diagnostic
retains its unrelated legacy `dCcAcPos_c` vtable difference.

The committed experiment records exact hashes, counts, provider bindings,
all 23 findings, and the early corrected metadata refusals. Formatting,
declaration agreement, rename-ledger and port-reference checks pass. The
header gate correctly finds no header edits. Committed whitespace and the
final immutable-range strict check are recorded by producer queue evidence.

Remaining work is explicit. The preserved native lifecycle checkpoint
`e56ffc1953e40dfafc578704400bd7c129f6fc90` has 13 scheduling-word mismatches
under already measured ordinary vector forms; this task does not repeat that
search. Its old queue link #2438 names an unrelated closed Dossunbar issue;
#4048 is the current continuation authority. Both 16-byte CLPS blocks stay
outside this slice because the reserved shared header declares them
`Matrix4x3`. A separately owned contract correction must precede their
migration. Existing scalar ABI, raw matrix, callback and object-layout debt
remains partial. The earlier promotion/readability findings are retained;
findings already fixed upstream are identified as historical fixes, not work
performed by this commit.
