# 169 references that are ambiguous between overlays

**Status:** open, needs a person. Drafted as a GitHub issue; not yet filed.

169 functions cannot be enrolled because a reference in them targets an address that
**two or more overlays both define**, and nothing in the data says which one was
resident when the code ran.

Tooling has taken this as far as it can. `tools/resolve_placeholders.py` resolves a
reference by joining the call site's own relocation to the owning module and then to
that module's symbol table:

```
from:0x021111e8 kind:load to:0x021099e4 module:overlay(2)
```

That works when dsd names one overlay. For these 169 it names several --
`module:overlays(0,4)` -- because overlays share address space and dsd could not tell
them apart either. Where only one candidate defines a symbol at the target address the
tool settles it automatically. These are the cases where **both candidates define
something, and the two disagree**:

| references | name in source | candidates |
|---:|---|---|
| 22 | `func_020beb68` | `ov000:data_ov000_020beb68` vs `ov004:data_ov004_020beb68` |
| 9 | `func_020aea30` | `ov002:func_ov002_020aea30` vs `ov004:_ZN5Enemy12KillByAttack...` |
| 8 | `func_020ada40` | `ov002:func_ov002_020ada40` vs `ov004:_ZN5Enemy20KillByInvincib...` |
| 6 | `func_020aed98` | `ov002:_ZN5EnemyC2Ev` vs `ov007:func_ov007_020aed98` |
| 5 | `func_020bc7d4` | `ov000:data_ov000_020bc7d4` vs `ov004:data_ov004_020bc7d4` |
| 5 | `_ZTV10dBgActor_c` | `ov006:data_ov006_0213c5bc` vs `ov098:data_ov098_0213c5bc` |

A guess here is not cheap. `match.py` compares relocated words as wildcards, so picking
the wrong overlay still byte-matches -- the mistake would surface only at the ROM link,
and only for whichever files happen to get enrolled. That is exactly the blind spot
that hid three wrong-callee bugs already (`Door::Behavior`, `func_ov004_020b29c0`,
`func_ov004_020b2a84`).

## What would settle it

Any of these, per case:

- **Overlay residency.** Which overlays are loaded together at the point this code
  runs? If ov002 and ov004 are never resident simultaneously, the referring module's
  own copy is the answer. An overlay load-order or scene-to-overlay map would resolve
  most of the table above mechanically, and would keep paying off afterwards.
- **Emulator trace.** Break on the call site and read the resolved target. See
  `notes/emu-trace-plan.md`.
- **Reading the code.** Several pairs are plainly the same function under two names --
  `_ZN5Enemy12KillByAttack...` in ov004 against an unnamed `func_ov002_...` in ov002.
  Confirming that also closes a naming gap in `symbols.txt`.

## Reproducing the list

```
python tools/eligible.py
python tools/resolve_placeholders.py          # report only; nothing is written
```

Everything reported as `0x... names {...}` is one of these.

## Related

- #1071 -- the resolver, and the ratchet that keeps new instances out
- `notes/declaration-centralization.md` -- the larger structural problem behind
  scattered, disagreeing references
- `notes/rom-build.md` -- how enrollment and the ROM link fit together
