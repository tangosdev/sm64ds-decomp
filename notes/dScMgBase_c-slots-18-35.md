# dScMgBase_c slots 18-35 — the keystone map

`include/dScMgBase_c.h` says: *"Slots 18-35 are eighteen further virtuals new at this
class; their signatures are not reconstructed yet, so they stay undeclared."*

That single sentence is what caps every minigame class's emitted vtable. mwcc emits a
table only as long as the slots it has been told about, so `dScMgSingle3DBase_c` emits
18 slots where the cartridge has 36 — a byte-exact **prefix**, not a disagreement, but
not ownable either. The same cap applies to all 32 descendants of `dScMgBase_c`.

## The map (read out of `_ZTV11dScMgBase_c`, ov004:0x020bc0c0)

| slot | +off | ROM body | existing `recovered name:` | shape in the legacy source |
|---|---|---|---|---|
| 18 | 0x48 | ov004:0x020b299c | *(none)* → `Virtual48` | `void(void)` |
| 19 | 0x4C | ov004:0x020b2994 | `OnTurnIntoEgg` | `int(void)`, `return 1;` |
| 20 | 0x50 | ov004:0x020b2990 | `Virtual50` | `void(void)` |
| 21 | 0x54 | ov004:0x020b298c | `OnGroundPounded` | `void(void)` |
| 22 | 0x58 | ov004:0x020ae198 | `OnAttacked1` | `int(void)` |
| 23 | 0x5C | ov004:0x020ae1a0 | `OnAttacked2` | `int(void)` |
| 24 | 0x60 | ov004:0x020ae140 | `OnKicked` | `int(Obj*)` |
| 25 | 0x64 | ov004:0x020ae128 | `OnPushed` | `int(void*)` |
| 26 | 0x68 | ov004:0x020b04e0 | `OnHitByCannonBlastedChar` | `int(void)`, `return 0;` |
| 27 | 0x6C | ov004:0x020af27c | `OnHitByMegaChar` | `void(void*)` |
| 28 | 0x70 | ov004:0x020af04c | `OnHitFromUnderneath` | `void(Obj*)` |
| 29 | 0x74 | ov004:0x020af094 | `OnAimedAtWithEgg` | `void(Obj*)` |
| 30 | 0x78 | ov004:0x020aeed8 | `OnAimedAtWithEggReturnVec` | `void(char*)` |
| 31 | 0x7C | ov004:0x020b2880 | `Kill` | `void(void)` |
| 32 | 0x80 | ov004:0x020b27f4 | `AfterClsn` | `void(void)` |
| 33 | 0x84 | ov004:0x020b265c | *(none)* → `Virtual84` | `void(char *obj)` |
| 34 | 0x88 | ov004:0x020ae3b4 | *(none)* → `Virtual88` | `void(char*,int,int,int,int)` |
| 35 | 0x8C | ov004:0x020ad660 | *(none)* → `Virtual8C` | `int(int*)` |

Fourteen of the eighteen already carry a `recovered name:` comment, so this is far less
speculative than the header's wording suggests. The four unnamed ones take the tree's
established `Virtual<hex offset>` convention (`include/fBase_c.h:143` — `Virtual34`,
`Virtual38`).

For slots 24, 25, 27, 28, 29, 30, 33, 34 and 35 the leading parameter in the legacy
source is really `this`; it becomes implicit when the free function turns into a method.

## Two hazards, both measured

**1. Slot 35 is a cross-overlay address collision.** `func_ov002_020ad660` and
`func_ov004_020ad660` both exist, at the same address 0x020ad660, in different overlays.
Only ov002's is decompiled (`src/func_ov002_020ad660.cpp`) — and it is a *different
function*. The slot-35 body is ov004's, which has no source file. Any rename must be
scoped to the ov004 symbol alone; a filename- or address-keyed rename hits ov002's
unrelated function and every byte gate still passes. See the standing
`cross-overlay-symbol-collision` note.

**2. The change has to be atomic.** Declaring the eighteen virtuals makes mwcc emit
vtable slots referencing `_ZN11dScMgBase_c*` mangled names. Those symbols do not exist
until the eighteen ov004 bodies are renamed. Declarations without renames leave the
link with eighteen undefined externals; renames without declarations orphan them. One
commit, or neither.

## What it unlocks

Full 36-slot vtable emission for `dScMgBase_c` and all 32 descendants, which is the
precondition for `production_mode: "intact-object"` ownership of the minigame family's
`_ZTV`/`_ZTI`/`_ZTS` instead of the `deadstrip-data` licence they carry today.
