# Readability Debt

Matching C sometimes needs to preserve compiler-shaped code that is harder to read
than the logical source would be. Track those spots here so a later cleanup or porting
pass can recover the clearer structure without weakening the matching workflow.

This file is descriptive only. Do not rewrite matched functions from this note unless
the current goal is a readability/port cleanup rather than byte matching.

## Pattern Template

| Function | Current Shape | Cleaner Intent | Confidence | Keep Ugly Because |
|---|---|---|---:|---|
| `func_xxxxxxxx` | Repeated stores / unrolled branches / manual field sequence | Loop/helper/struct operation | Medium | Matching may depend on instruction order, register allocation, or branch shape. |

## Candidates

| Function | Current Shape | Cleaner Intent | Confidence | Keep Ugly Because |
|---|---|---|---:|---|
| `func_02010da4` | Three hand-written clamp blocks for actor position fields `+0x5c`, `+0x60`, `+0x64`. | Clamp `Actor::pos.x/y/z` with a small helper or loop over vector components. | High | A loop/helper may change register allocation and branch layout. |
| `func_02039218` | Manual loop clears 0x18 global mesh-collider slots. | `memset`/`std::fill`-style clear of a collider slot array. | Medium-high | Library call or different loop form may not match mwccarm output. |
| `func_02012860` | Three independent flag checks call three adjacent low-level sound parameter setters. | Iterate over flag/parameter descriptors or name the three parameter setters. | Medium | Rewriting as a loop would likely change calls, constants, and register pressure. |
| `func_0203b438` | Pointer-tree/list insertion written with gotos and manual child/sibling traversal. | Named scene-node insertion helper. | Medium | Existing control-flow shape may be required for matching. |
| `func_ov002_020d3498` | Direct field mutation: negate speed, add `0x8000` to angle, copy facing angle. | `Player::TurnAround` helper with named fields. | Medium | Clearer struct fields are safe later, but expression reshaping may affect matching now. |

## Later Cleanup Rules

- Keep matched C as the behavioral oracle.
- Refactor only after a function or subsystem is no longer being used as a byte-match target.
- Replace repeated field operations with named structs first; loops/helpers come after the data layout is clear.
- For every cleanup rewrite, keep a before/after behavior test or compare against the matched implementation.
