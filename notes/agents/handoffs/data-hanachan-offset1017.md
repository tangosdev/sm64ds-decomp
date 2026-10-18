# Hanachan member-pointer size assertion

Task `data-hanachan-offset-ci1017` continues accepted task
`data-hanachan-native1017` at `db0f846cbce64ed992692c31e24fe3b7c103f5e7`.
It belongs to [issue #4028](https://github.com/tangosdev/sm64ds-decomp/issues/4028)
and updates [PR #4030](https://github.com/tangosdev/sm64ds-decomp/pull/4030).
The producer is `codex-hanachan-offset1017`; independent verification and
publication remain separate stages. No merge is authorized.

Public header-offset CI failed at the native `Phase mPhase` field because the
checker lacked the typedef's size. Finding `hanachan-offset-ci-001` is fixed by
a qualified, compiler-enforced C++98 typedef-char assertion that
`sizeof(dScMgHanachan_c::Phase) == 0x08`. The existing checker already reads this
assertion format. Every field, offset comment, existing size assertion, method,
and ownership configuration is retained.

The checker now walks all eight top-level offset-commented fields: `mPhase`,
`mDelay`, `mPhaseTimer`, `mTimeLeft`, `mTimeTick`, `mWiggler`, `mTarget`, and
`mHit`. It reports zero mismatches and zero unparsed fields. Its data extent
is `0x4f65`, immediately after the byte `mHit` at `0x4f64`; the existing target
compiler assertion independently verifies the padded class size `0x4f68`.
This is not a zero-field skipped pass. Nested Wiggler layout checking is not
newly claimed by this repair.

Fresh pinned `2004/b56` compiles of both affected consumers before and after
the final formatted assertion produce identical complete ELF byte strings:

| Consumer | Object bytes | SHA256, unchanged |
| --- | ---: | --- |
| `src/minigames/d_s_mg_hanachan.cpp` | 1272 | `4109e5e6935a246a178b17c158fdb63eb3dca44b0447dfb8c69b2a899688a945` |
| `src/minigames/d_s_mg_hanachan_methods.cpp` | 43600 | `11a2522fe3067c46200f74a4220d99ea2f300f5355074b3115880ff262958350` |

Equality includes all emitted instructions, data, BSS descriptions, symbol
tables, and relocation records. The accepted predecessor's scratch full-ROM
and 37-provider proof remains explicitly evidence for that immutable commit;
the exact-object comparison establishes this assertion's unchanged output.
Fresh checks and their scope are recorded in
`notes/experiments/data-hanachan-offset1017.json`.

The following inherited findings remain deferred without reducing their scope:

- `hanachan-raw-layout-001`, issue #4028, owner
  `codex-hanachan-native1017`: existing raw field/loop spellings, popup record
  views, private StartPathA scalar view, and manual factory/lifecycle.
- `ASSET-NAMING-RECONSTRUCTION-01`, issue #2750, owner the original asset
  coordinator and subsequently claimed C++ reconstruction producers: remaining
  reserved/encoded asset candidates and existing class/ABI/raw-field debt.

This repair completes the reported header-offset failure only. It does not
complete the class reconstruction or broaden the predecessor's data ownership.
