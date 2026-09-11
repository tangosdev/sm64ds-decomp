# PR #2488: SCRE source and provenance repair

This continues public `39820996ea4da0b02417410ff968964cf1eb5922` from source
base `224e660ea0219d0999c58483eb2dd9b38991f5ef`. The four-function promoted
TU retains its exact text range and ordinary C++ construction. Comments state
what the source does; research history and remaining limits live here.

Source ownership: `pr2488-source-review-0910`, producer `codex-r2445-0910`.
The separate `pr2488-attribution-repair-0910` reservation owns only
`attribution.json`. Both offer this same candidate for independent verification.
Review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.

| Finding | Disposition |
| --- | --- |
| SCRE-01 | Fixed in the inherited public head and retained: check the closest player once, spawn only at a distance strictly less than 100 units, then mark this trigger for removal regardless. |
| SCRE-02 | Partial under [#2566](https://github.com/tangosdev/sm64ds-decomp/issues/2566), assigned to @andrewboudreau through integrator follow-up. The leaf Pos() exposes inherited scalar storage as a Vector3 reference. It does not reconstruct a shared position member. |
| SCRE-03 | Fixed: the header briefly explains the unsigned-long allocation declaration and configured unsigned-int actor allocator. Real new and its exact destination remain. Fresh alternatives below bound the reason for the bridge. |
| SCRE-04 | Fixed separately by merged [#2493](https://github.com/tangosdev/sm64ds-decomp/pull/2493). The current parser checks this candidate header with zero mismatches/unparsed fields and span 0xd4. This source branch does not copy tooling backward; final main composition must retain that landed parser. |
| SCRE-05 | Fixed under the separate attribution reservation: replace only the obsolete path#daSCre_c_Spawn override with path#daSCre_c_classInit, preserving andrewboudreau. All other mappings are identical. Existing canonical credit was already intact; this cleans up the key rather than claiming a recovered author. |
| SCRE-06 | Fixed: manifest notes no longer refer to a deleted source banner or claim source ownership of RTTI/name/vtable data. The four text functions are licensed; all nine metadata records remain checked compiler-only outputs supplied from their canonical ROM homes. |
| SCRE-07 | Fixed: the four-byte tail at 0xd0 is unnamed storage, not proven unused padding. Its identifier is unk_0d0 and the unsupported unused claim is removed. No layout or executable statement changes. |

## Actual behavior and identity

Behavior uses DistToCPlayer(), whose existing interface describes the closest
player, and compares against 0x64000 (100 in 20.12 form). It creates actor 0xb2
with `(param1 & 0xf) | 0x40`, this trigger's position, no rotation, the current
area, and death-table ID -1. MarkForDestruction is outside the range condition.

The configured ov018 load base is 0x021111a0. Direct reads in the 10,784-byte
image give the null-terminated name `8daSCre_c` at 0x02113a38 and typeinfo words
`0209a764, 02113a38, 0208e390` at 0x02113a44. They identify the class and direct
base dActor_c. The classInit factory spelling is reconstructed from class/registry
context; RTTI does not store original function names.

Data and BSS ownership arrays remain empty. The canonical vtable address point
is ov018:0x02113a74. All nine emitted metadata records keep their existing
`deadstrip-data` policy: five VERIFIED and four PARTIAL, with no differences.
Comparison of emitted metadata with ROM data is distinct from source ownership.
Historical verification/partial-isolation blocks are preserved as historical proof.

## Measured boundaries

Fresh scratch probes use the exact public source/header with the recorded single
replacement and pinned 2004/b56 flags. They do not modify the shared headers.

- Changing only the allocation declaration to `operator new(unsigned)` is rejected
  as an illegal operator declaration.
- Removing only the leaf allocator preserves a 48-byte factory but changes the
  allocation reference to unresolved global `_Znwm`, giving BLIND-1. The retained
  bridge calls the configured actor allocator and verifies.
- A real stack Vector3 initialized from the three scalar fields, either before the
  range test or inside it, grows Behavior from 92 to 116 bytes and emits a four-byte
  Vector3 destructor. Both alternatives differ from the ROM. These measurements
  do not establish that every possible source form is impossible.

Related [Lava PR #2513](https://github.com/tangosdev/sm64ds-decomp/pull/2513)
proposes a shared dActor_c::Pos() overlay. It remains an open PR, not a landed
shared member or this follow-up issue. Issue #2566 owns evaluation of that shared
boundary and its consumers; the current source remains explicitly partial.

## Scoped proof

The baseline and repaired complete compiler objects are byte-identical, SHA256
`665d5d2618fadaadea534e431f62d124ecb61f5deabcc52edadd0b2ac5309668`.
All four functions verify: D1 36 bytes, D0 56, Behavior 92, classInit 48;
total 232 bytes with 12 module-correct relocation destinations and no blind
references or differences. All four isolation plans are clean, emission follows
ROM ordinals, and compiler-only policy leaves no unlicensed output. The leaf
header's complete source consumer closure is this TU alone.

The existing canonical authors of the four configured module/address identities
are preserved: D1/D0/Behavior retain github-actions[bot], and classInit retains
andrewboudreau. No enrollment, symbol/config address, shared header, registry,
rename ledger, or tooling baseline is edited. The port check resolves all 423
references.

Local reports are under `C:/tmp/sm64ds-r2488-r2445-0910/build`: baseline/,
candidate/, probes.json, baseline-credit.json, current-header-gate.log and
port-refcheck.log. Producer evidence records the immutable commit, final credit
and full-ROM gate identities; historical results are not relabeled as fresh runs.
Independent exact-head source acceptance, actual-main composition and required
private validation remain separate from this producer handoff.
