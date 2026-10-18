# Cup gameplay table ownership

This document describes this commit; the queue records its immutable SHA. Task `data-cup-tables1017`, producer `codex-cup-tables1017`, follows [issue #4039](https://github.com/tangosdev/sm64ds-decomp/issues/4039) and the continuing class reconstruction in [issue #2492](https://github.com/tangosdev/sm64ds-decomp/issues/2492). Independent review belongs to the root verifier. Publication is authorized; merging is not.

The source/review base is `121ca67be3ac06751cc6d8106ff6487a76dc52fd`, accepted [PR #4025](https://github.com/tangosdev/sm64ds-decomp/pull/4025). The queue uses activated workflow `18af52b49b9fb5663d417bdfa86109b7b157aaa3`. No tool change is included. The older PR #3937 candidate is retained as research evidence; this change uses the current production source and header.

## Source and storage

The existing `src/minigames/d_s_mg_cup.cpp` now owns ov006 `.data` `[0x0213c094, 0x0213c0f4)`: four native objects, 96 bytes total.

| Object | Address | Native type | Bytes |
|---|---|---|---:|
| `sCupRoundTable` | `0x0213c094` | `RoundRow[10]`, swap byte plus two four-bit speed-row indices | 20 |
| `sCupHomePositions` | `0x0213c0a8` | `RestPos[3]`, two Q12 screen coordinates | 24 |
| `sCupSwapFirst` | `0x0213c0c0` | `IdxPair[3]`, the two cup-slot indices | 24 |
| `sCupAnimTable` | `0x0213c0d8` | seven pointers to `AnimStep` sequences | 28 |

The existing project names describe the observed consumers; they are not recovered original identifiers. This is an evidenced production reconstruction assignment, not a claim to know the historical original TU filename.

`sCupSlots` at `0x0213c0ac` is the first position's y field; `sCupSwapSecond` at `0x0213c0c4` is the first pair's second slot. Both configured labels remain unchanged. The source accesses `.y` and `.b` on the parent arrays and removes the fictitious separate-array declarations. The accepted initialized-data interior-label helper preserves both ELF identities inside the actual compiled parent objects, without adding storage. The two labels are not separate data claims or providers.

The animation table targets remain ROM-owned. Local `AnimStep[]` declarations express the observed pointer/duration sequence contract; no integer-cast addresses, raw byte payloads or replacement copies are used. The existing class header, all attribution, and all configured symbol names remain unchanged. The initializer still copies eight existing native PMF descriptors into the 64-byte state table; those descriptors, profile, RTTI, vtable and other tables remain outside this slice.

## Proof

The experiment JSON records final compiler-input hashes, raw object comparisons, strict checks, full linking, production providers and ROM evidence. Pinned compiler: `2004/b56`, normal production flags.

The fresh baseline raw object is 22,944 bytes, SHA-256 `cb34081ecc8b3015a1bff8a082c3bd0c9f5c4feed3e5e413a2f99e5f0368d67e`; the typed object is 23,608 bytes, SHA-256 `c69bd5bf04d423f6fee3b4f96af4536b6d5bc2783b7fb67e7c66d6931c578a6f`. All 34 emitted function bodies retain identical raw bytes and canonical relocation destinations, including the 32 production functions, duplicate D2 and compiler initializer. Canonicalization maps the old interior symbols to their parent plus four; it does not ignore relocation destinations. All four new objects match their full retail extents, including all seven animation pointer targets and all 20 packed bitfield bytes.

Focused strict verification reports 33/33 VERIFIED, zero warnings and zero blocking findings, including the initializer. Standalone `tubuild verify` reports 32/32 matched methods with clean relocation destinations but retains its existing initializer classification refusal: one extra initializer, zero compiler-only policy errors. It is not a complete standalone acceptance.

Final baseline and candidate whole-TU links reproduce all five owned ranges, all 106 modules and the full 16 MiB ROM (SHA-256 d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8). The normal production build also passes: 11,334 reproducing functions, 339 complete data claims, 245 BSS claims and five requested compiler providers (four new arrays plus the existing state table). Both interior identities are zero-size symbols in the final OV006 section. The baseline retains nine strict-symbol errors, with zero new errors; the optional ROM-data diagnostic retains its existing dCcAcPos vtable difference. No old diagnostic is claimed as fixed by this slice.

## Findings and remaining work

Queue predecessors `mg-promotions-finish-0930`, `mgread-cup-1007` and `asset-readability1014` preserve all prior finding IDs and producer lineage. Historical findings for unrelated classes remain explicitly inherited; this slice does not freshly review those classes. The accepted fixed Cup/provenance findings retain their dispositions.

CUP-03, CUP-06 and CUP-09 remain deferred under issue #2492: component layout, manual lifecycle/factory calls, raw accesses and remaining helper interfaces need wider class reconstruction. SOUND-RECON01 and inherited asset-name limitations are unchanged historical scope. This table ownership change does not claim whole-class completion.

`cup-table-interiors-001` is fixed: the two field aliases have no fabricated separate storage, and native field accesses preserve complete emitted code and relocation targets. `cup-data-residual-001` remains deferred under issue #4039, owner `codex-cup-tables1017`: remaining ROM-owned PMF/profile/metadata and animation sequence data need separate coherent ownership work. Keep issue #4039 open after this partial PR.
