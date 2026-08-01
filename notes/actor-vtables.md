# The Actor hierarchy vtables

Recovered from the ROM, not inferred. Every slot in all three tables resolves to a named
function in `config/arm9/symbols.txt`; none is a thunk, none is a secondary vtable, and
there is no address-point offset — CodeWarrior 1.2 emits no RTTI header, so the address
stored into `[this+0x0]` *is* slot 0.

## The hierarchy

    ActorBase  →  ActorDerived  →  Actor

`Actor` does **not** derive from `ActorBase` directly. `src/_ZN5ActorC2Ev.cpp` calls
`ActorBase::ActorBase`, stores `&data_0208e4b8`, then immediately overwrites it with
`&data_0208e3a4` — two consecutive vptr stores, which is what an inlined intermediate-base
constructor looks like. `src/_ZN12ActorDerivedD1Ev.c` closes the loop: it stores
`data_0208e4b8` and calls `_ZN9ActorBaseD2Ev`, so ActorDerived's immediate base is
ActorBase.

`ActorDerived::ActorDerived` and `~ActorDerived` are inline in the original — no standalone
C2 symbol exists — so a header that promotes them must keep them inline to reproduce
Actor's C2/D2 bytes.

## Where they live

| class | vtable | slots | also named |
|---|---|---:|---|
| `ActorBase` | `_ZTV9ActorBase` | 18 | `data_02099edc` |
| `ActorDerived` | `_ZTV12ActorDerived` | 18 | `data_0208e4b8` |
| `Actor` | `_ZTV5Actor` | 31 | `data_0208e3a4` |

The `data_` names are kept as aliases because 45 `src/` files reference them. `eligible.py`
drops any file with an unresolvable reference, and gap objects import carved-out symbols
weakly — so a stale name links silently to zero rather than erroring.

## ActorBase — 0x02099edc, 18 slots

| # | target |
|---:|---|
| 0 | `InitResources` |
| 1 | `BeforeInitResources` |
| 2 | `AfterInitResources(u32)` |
| 3 | `CleanupResources` |
| 4 | `BeforeCleanupResources` |
| 5 | `AfterCleanupResources(u32)` |
| 6 | `Behavior` |
| 7 | `BeforeBehavior` |
| 8 | `AfterBehavior(u32)` |
| 9 | `Render` |
| 10 | `BeforeRender` |
| 11 | `AfterRender(u32)` |
| 12 | `OnPendingDestroy` |
| 13 | `Virtual34(u32, u32)` |
| 14 | `Virtual38(u32, u32)` |
| 15 | `OnHeapCreated` |
| 16 | `~ActorBase` (D1) |
| 17 | `~ActorBase` (D0) |

## ActorDerived — 0x0208e4b8, 18 slots

ActorBase's table with **slot 2** overridden by `ActorDerived::AfterInitResources`
(0x02013ef4) and its own D1/D0 at 16/17. Every other slot still points at ActorBase's
implementation.

## Actor — 0x0208e3a4, 31 slots

Overrides slots **1, 2, 4, 5, 7, 8, 10, 11, 16, 17**. Slots 0, 3, 6, 9, 12, 13, 14, 15
still point at ActorBase. Then thirteen of its own:

| # | target |
|---:|---|
| 18 | `OnYoshiTryEat` |
| 19 | `OnTurnIntoEgg(Player&)` |
| 20 | `Virtual50` |
| 21 | `OnGroundPounded(Actor&)` |
| 22 | `OnAttacked1(Actor&)` |
| 23 | `OnAttacked2(Actor&)` |
| 24 | `OnKicked(Actor&)` |
| 25 | `OnPushed(Actor&)` |
| 26 | `OnHitByCannonBlastedChar(Actor&)` |
| 27 | `OnHitByMegaChar(Player&)` |
| 28 | `OnHitFromUnderneath(Actor&)` |
| 29 | `OnAimedAtWithEgg` |
| 30 | `OnAimedAtWithEggReturnVec` |

The placeholder names self-confirm the table: `Virtual34`, `Virtual38` and `Virtual50` sit
at byte offsets 0x34, 0x38 and 0x50 — slots 13, 14 and 20.

## Two traps for whoever writes the headers

**Destructor position.** D1/D0 land at slots **16 and 17**, not 0 and 1. That differs from
`include/Fader.h`, where the destructor is declared first and takes slots 0/1. Slot index
follows declaration order, so `ActorBase` must declare its destructor *after*
`OnHeapCreated`. Copying the Fader header shape puts it first and shifts sixteen slots.

**Which vptr store to read.** `Actor`'s constructor contains two. Taking "the" vptr store
yields ActorDerived's vtable. Read each class's D1 destructor and take its *first* store.

## Reproducing this

The vtable extent is `[start, next symbol)` — the config names the symbol that follows, so
the array cannot run past its own object. Read the words out of `extracted/arm9_dec.bin` at
`addr - 0x02004000` and resolve each through `config/arm9/symbols.txt`.

## What still is not checked

Nothing verifies a header's declared slot order against these bytes. The vtable is
gap-object data supplied from the ROM, never compiled, so neither gate sees it:

- `match.py` catches a wrong slot *index* — that is an immediate in the dispatch `ldr`
  (`[r3, #0x30]` = slot 12), an instruction word rather than a relocation.
- `rombuild.py` catches a wrong vtable *identity* — storing the wrong class's address is a
  relocation, which `match.py` wildcards.
- Neither covers the vptr stores themselves: `_ZN5ActorD1Ev`, `_ZN5ActorD2Ev`,
  `_ZN9ActorBaseD1Ev` and `_ZN9ActorBaseD2Ev` are not enrolled as `complete`. The ActorBase
  pair currently fails eligibility on an invented symbol `VT0`.

Until each slot is exercised by a virtual call inside a byte-checked file, this table is
the only record of the order.
