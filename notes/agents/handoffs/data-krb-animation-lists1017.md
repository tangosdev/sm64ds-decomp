# Krb and KuriKing animation-handle lists

This document describes this commit; the queue records its immutable SHA.
Task `data-krb-animation-lists1017` continues the global-data campaign under
[issue #4045](https://github.com/tangosdev/sm64ds-decomp/issues/4045).
Producer: `codex-krb-lists1017`. Independent review and publication belong to
the parent campaign integrator. No merge is authorized.

The source base is `121ca67be3ac06751cc6d8106ff6487a76dc52fd`, the accepted
initialized-interior-label helper from PR4025. The activated queue workflow is
`18af52b49b9fb5663d417bdfa86109b7b157aaa3`. The helper preserves the existing
`data_ov074_02122954` label twelve bytes inside the boss animation list without
creating a separate object. No tooling change is part of this source work.

## Reconstructed objects and names

| Module and range | Previous name | Reconstructed name | Type |
| --- | --- | --- | --- |
| ov084 [0x02130278, 0x02130294) | `data_ov084_02130278` | `sKuriboAnimationFiles` | `SharedFilePtr *const [7]` |
| ov074 [0x0212292c, 0x02122948) | `data_ov074_0212292c` | `sKurikingMinionAnimationFiles` | `SharedFilePtr *const [7]` |
| ov074 [0x02122948, 0x02122978) | `data_ov074_02122948` | `sKurikingAnimationFiles` | `SharedFilePtr *const [12]` |

These are 104 bytes with 26 native pointer relocations. The pointer entries are
constant; their resource-handle pointees remain mutable for loading and reference
counting. Both normal and vanish KuriKing resource paths use the seven Krb handles
in the same order as Krb. The twelve-entry list uses KuriKing's own animation
handles. Consumer loops, current handle initializers and every relocated retail
word establish the roles and complete extents. The semantic list names are
reconstruction choices, not recovered historical identifiers. Assigning these
objects to the existing production TUs does not prove original source filenames
or original TU boundaries.

The old/new/address map is deliberately kept here and in the owned experiment
record. There was no existing rename-ledger chain for these three names; no
shared ledger or address-keyed author record was rewritten. Existing PR3937
DD273/DD252 candidates are preserved as prior evidence, not transplanted actor
sources. Both were authored by `lunavyqo` with `Cursor` coauthor attribution:
`eec8c97eb692015f42b764ab11064f3e0f45892f` (original `c680f0949185`) and
`a482dd7dd0449f09da5ec993f27b2bd4bbb1351e` (original `3ecca6924d0d`).

## Shared declaration and unchanged behavior

`daKrb_c.h` now supplies the existing animation-handle wrapper to its two actual
consumers. Its fields use the layout established by `src/SharedFilePtr.cpp`:
`u16 fileID`, `u8 numRefs`, natural padding and `BCA_File *ptr`. A compiler-enforced
size assertion retains eight-byte storage. The old packed `u32 id` view is no
longer promoted as animation-handle field semantics. The local model wrapper
still retains its packed first word, and its comment says so.

The shared declaration makes KuriKing's previously mismatched handle extern agree
with the actual Krb definition. The lists need no cast to remove constness and no
integer-shaped pointer payloads. Two redundant Krb consumer casts are removed.
Header consumers are exactly `src/game/actors/d_a_krb.cpp` and
`src/game/actors/d_a_kuri_king.cpp`; both are reserved and rebuilt.

Complete emitted-object comparison preserves all 79 prior Krb definitions and
120 prior KuriKing definitions, including 38 and 59 function definitions counting
compiler helpers. The 508-byte Krb initializer and all 44 relocations, and the
1,016-byte KuriKing initializer and all 78 relocations, are unchanged. KuriKing's
29 compiler temporary names advance after the shared declaration. The mapping is
derived bijectively from unchanged initializer relocation sites, types and
addends, then checked against every old symbol's size, binding, type, section,
payload and canonical relocation. Only manifest source keys change; canonical
destinations retain their identities. The precise map is in the experiment.

## Proof boundaries

The two changed TUs cannot initially admit each other's new range through the
ordinary per-TU scratch command: the peer's old link receipt does not yet cover
the new `.rodata`. Private `build/run-isolated-list-proof.py` keeps all normal
candidate audits, uses a full disposable manifest with only the peer omitted,
and demotes only that peer's exact complete entry to extracted retail bytes in
the disposable scratch configuration. It records the actual observed candidate
receipt only. This isolated step does not establish joint source ownership.
The final normal production run compiles both together and verifies their actual
link providers: all 82 requested objects pass, all 106 modules reproduce, and the
full 16 MiB ROM retains SHA256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
The build has 11,334 source functions, 340 complete data claims and 245 BSS claims,
with nine baseline strict-symbol diagnostics and zero new errors. The configured
interior label is a zero-size symbol in the final ELF's `OV074` section. The Krb
vtable provider request covers its 124-byte public extent; the complete 132-byte
storage including its preamble is covered by the full owned-section audit.
The owned experiment records measured results, configuration and artifact hashes.

Raw prepush checking reports 88 verified entries, with the existing initializer
warnings (Krb BLIND-20, KuriKing BLIND-31), two KuriKing factory BLIND-1 warnings,
and NO-SYM for the explicitly supplied header. These warnings are not reported
as strict passes: full owned-section relocation proofs separately cover those
functions, callbacks and initializers. Declaration agreement, source style,
header offsets (31 fields, extent 0x478), port references and rename chains pass.

Standalone `tubuild verify` reports all 35 Krb and 55 KuriKing text functions
matching, with one existing initializer-classification refusal per TU and zero
compiler-only policy errors. This is not described as an unconditional standalone
pass. Whole-TU relocation and production proofs separately cover the initializers.
The optional broad ROM-data diagnostic retains the unrelated legacy
`_ZTV10dCcAcPos_c` difference; it is not a global catalog pass.

## Remaining reconstruction and lineage

Completion is partial. Existing Krb model-wrapper packed fields, union state
descriptors and scalar ABI bridges remain. KuriKing retains its PMF stand-in,
free stubs, raw matrix/padding accesses, scalar Fix12 interfaces, resource-wrapper
field views and named compiler constraints. Other global tables and metadata
remain outside the new 104-byte slice. These limits are tracked by
`krb-lists-residual-001` on issue #4045 and the inherited `KURI-NATIVE-002` on
[issue #3769](https://github.com/tangosdev/sm64ds-decomp/issues/3769).

The queue preserves all 21 unique inherited findings from
`asset-readability1014`, `issue-2411-humanizer-2`, `kuriking-native1013`,
`pr2864-identity-promotion-0920` and `pr2878-contract-repair-0920`. Existing fixed
provenance/correctness dispositions remain historical; unrelated Koopa/da1up
reconstruction deferrals retain their issues and owners. This slice does not
claim to reconstruct those unrelated classes. The new shared-layout finding
`krb-animation-layout-001` is fixed by the evidenced fields and complete-object
proof, rather than treating a packed word as a recovered file-ID field.
