# Handoff: issue2410-terminal-bss-tool1015

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2410
- Stage/session/harness: implement / codex-mky-bss-tool1015-producer / Codex.
- Branch: [BSS tooling branch](https://github.com/tangosdev/sm64ds-decomp/tree/tools/mky-terminal-bss1015).
- Accepted input, source base and workflow tools: e209dacfaaa95cb5490637ff053d62182689bebe.
- Reserved edits: tools/tubuild.py, tools/test_tubuild_terminal_bss.py, and this handoff.
- Status: locally verified tooling candidate; independent review belongs to the next stage.
- Next owner: root verifier/integrator, for independent review and a separate tooling PR.
- No src/, include/, symbol/config ownership, attribution, or Monkey source change is included.

## Behavior and evidence boundary

The existing BSS verifier accepts configured symbol starts and module boundaries.
A genuine final object can end before module alignment padding, so requiring its
source contribution to reach the module end incorrectly adds bytes to the source.
The new end-only fallback requires an already anchored start; one containing
configured BSS section with explicit power-of-two alignment; one emitted final
object whose name, address, and positive size agree with both its manifest and an
explicit `kind:bss(size=...)` configuration row; and a remaining gap strictly less
than that alignment which rounds exactly to the module end. Duplicate identities,
interior symbols, symbols in the tail, sized BSS extents crossing the end, and
existing entries owning the tail are refused. Accepted evidence is reported as
`terminalBssAnchor`; the claim and emitted size are never extended.

Ordinary anchors and all existing contribution-size, symbol-home, byte, binding,
and relocation checks remain in force. The fallback does not license a new start
boundary. Explicit configured sizes are reviewed reconstruction facts, not values
the tool infers from the candidate or its distance to the module end.

Pinned DSD 0.11.0 supports this spelling directly. Its primary documentation and
parser are:

- https://github.com/AetiasHax/ds-decomp/blob/v0.11.0/docs/symbols.md#bss
- https://github.com/AetiasHax/ds-decomp/blob/v0.11.0/lib/src/config/symbol.rs

`SymBss::parse` accepts `size=`, and `SymbolKind::size` uses that explicit value
instead of inferring an extent to the next symbol or section end. No DSD change is
required. This commit does not add any configured size itself.

## Validation

- `python -m unittest tools.test_tubuild_terminal_bss tools.test_tubuild_owned_relocs tools.test_tubuild_unwind_sections tools.test_tubuild_vtable_partitions`: exit 0, 41 tests passed. The 27 new tests include an actual `2004/b56` compile; it ran and passed locally, rather than skipping. The compiler-free cases exercise the real verifier with temporary configuration and a section-layout fixture.
- `python -m pytest tools/test_tubuild.py -q -k 'bss or nontext_verifier'`: exit 0, 3 passed, 73 deselected. These existing tests include the compiled unanchored-BSS rejection and non-text relocation/layout gates.
- `git diff --check`: exit 0.
- No full ROM rebuild or private CI validation was run: this is a tooling change, and the Monkey candidate still has a separate integration blocker described below.

The isolated worktree is C:/tmp/sm64ds-mky-bss-tool1015. Its private
`build/bss-review/` holds integration scripts, reports, and DSD logs. These scratch
artifacts are not production enrollment or durable acceptance of the source.

### Retail extent and real-object control

The original OV030 initializer loads r0 at 0x02114a60 from literal 0x02114d50,
whose retail value is 0x02115e0c. Between that load and 0x02114c1c, r0 is not
reassigned, and the initializer performs exactly 44 word stores at offsets
0x00, 0x04, ... 0xac: eleven pairs of two-word member pointers, occupying 0xb0
initialized bytes. The last store instruction is 0x02114c18. This evidence was
read from retail instructions independently of the candidate declaration.
The inspected 0x1bc-byte instruction span has SHA-256
993ffad7936c128d3730726a3385cc3cd71d03e9609a88114ed1718cedb88329.

A scratch copy of the existing Monkey preview configuration, with that reviewed
explicit size, passed the new verifier against its real ordered object: five
owned non-text sections, 163 relocation records, and the original exact-size
checks. The BSS object contribution remains [0x02115cd0,0x02115ebc); the module
BSS end remains 0x02115ec0. Report: build/bss-review/monkey-owned-sections.json.
This is conditional object/configuration evidence, not a complete production link.

### DSD and linker control

Against baseline configuration plus an isolated BSS-only fixture claim, the pinned
`dsd delink` and `dsd lcf` both exited 0. DSD produced
`_dsd_gap@ov030_3.o` with a four-byte SHT_NOBITS `.bss`, alignment 1, and selected
it after the source contribution. The generated LCF then applies ALIGN(32).
A synthetic compiled 0x1ec-byte footprint linked with this actual gap object
under mwldarm and measured start 0x02115cd0, object end 0x02115ebc, and module end
0x02115ec0. The synthetic fixture is only a linker control; it is not substituted
for the real-object proof or included in production.
Reports: build/bss-review/ov030-bss-lcf.txt and build/bss-review/bss-link.json.

The full Monkey preview's DSD delink was also attempted and failed on a separate
existing configuration issue: `_ZTV7daMky_c` has inferred extent
0x02115bfc..0x02115c80, exceeding the proposed `.data` claim ending at 0x02115c78.
That failure remains recorded in build/bss-review/dsd-delink.log and was sent to
the source owner. This tooling change does not widen that claim or solve that
unrelated vtable extent issue. No issue-completion claim is made.
