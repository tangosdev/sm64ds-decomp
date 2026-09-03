# dScMgBase_c slots 18-35 — the keystone map

**THE RANGE IS COMPLETE (2026-08-31).** All eighteen slots, 18 through 35, are
declared on `dScMgBase_c` together with every descendant override, one slot per commit.
`_ZTV11dScMgBase_c` and the tables of all 32 descendants are emitted from source at
their full length; nothing in the family is a prefix any more. What follows is the map
that got us here, kept because the measurements in it are still the evidence.

`include/dScMgBase_c.h` used to say: *"Slots 18-35 are eighteen further virtuals new at
this class; their signatures are not reconstructed yet, so they stay undeclared."*

That single sentence was what capped every minigame class's emitted vtable. mwcc emits a
table only as long as the slots it has been told about, so `dScMgSingle3DBase_c` emitted
18 slots where the cartridge has 36 — a byte-exact **prefix**, not a disagreement, but
not ownable either. The same cap applied to all 32 descendants of `dScMgBase_c`.

## What finishing it actually bought (measured 2026-08-31, at slot 35)

`romdata_check` before slot 35 vs after — the whole-tree counters, not just this family:

| | slot 34 | slot 35 | delta |
|---|---|---|---|
| verified symbols | 465 | 496 | **+31** |
| verified bytes | 35,428 | 39,900 | **+4,472** |
| partial symbols | 253 | 223 | −30 |
| partial bytes | 12,120 | 7,924 | −4,196 |
| differing symbols | 6 | 5 | −1 |

Every slot from 18 to 34 moved exactly 128 bytes: `+4` verified and `+124` partial,
one word into each of the 32 tables. **Slot 35 does not**, and that is the point of it.
Completing a table does not add a word to it — it moves the *whole table* from PARTIAL
to VERIFIED. Thirty of them flipped PARTIAL → VERIFIED and `_ZTV12dScMgAmida_c` flipped
DIFFERS → VERIFIED, which is where the +31 and the last of the six DIFFERS come from.

Per-table, straight from `romdata_check.check_object` (the `--json` report carries
counts and a `differing` list only, so a PARTIAL → VERIFIED flip is invisible in it):

```text
_ZTV11dScMgBase_c  emitted 144  romExtent 144  blindWords 0  VERIFIED
```

Thirty-two of the thirty-three family tables read exactly like that — 144 bytes,
36 slots, emitted length equal to the cartridge's extent — except
`_ZTV12dScMgAmida_c`, which is 148 because slot 36 is genuinely its own.

**The one exception is not a missing virtual.** `_ZTV12dScMgSlot3_c` still scores
PARTIAL: emitted 144, `romExtent` **152**. The eight bytes past the end read
`{0x00000000, 0x0213e5a0}`, and 0x0213e5a0 is `_ZTI12dScMgSlot1_c` — that is the
offset-to-top / typeinfo header of the *next* vtable in the image.
`_ZTV12dScMgSlot3_c` is at 0x0213eaa8 and `_ZTV12dScMgSlot1_c` at 0x0213eb40, exactly
152 apart, and dsd points a `_ZTV` symbol at slot 0 rather than at the header, so the
two header words in between belong to no symbol and dsd's "extends to the next symbol"
sizing swallows them. So the class model is right and the config extent is eight bytes
long. Fixing it is a [config/arm9/overlays/ov006/symbols.txt](../config/arm9/overlays/ov006/symbols.txt) change with its own
gates, not part of a keystone commit. This is the mirror image of the standing
`verified-vtable-is-not-a-complete-vtable` hazard: there dsd cuts a table short, here
it runs one past.

## The map (read out of `_ZTV11dScMgBase_c`, [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020bc0c0)

| slot | +off | ROM body | existing `recovered name:` | shape in the legacy source |
|---|---|---|---|---|
| 18 | 0x48 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b299c | `OnYoshiTryEat` — **corrected 2026-08-31** | `int(int)` \*\* — **DECLARED** |
| 19 | 0x4C | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b2994 | `OnTurnIntoEgg` | `int(void)`, `return 1;` |
| 20 | 0x50 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b2990 | `Virtual50` | `void(void)` |
| 21 | 0x54 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b298c | `OnGroundPounded` | `void(void)` |
| 22 | 0x58 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020ae198 | `OnAttacked1` | `int(void)` |
| 23 | 0x5C | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020ae1a0 | `OnAttacked2` | `int(void)` |
| 24 | 0x60 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020ae140 | `OnKicked` | `int(Obj*)` |
| 25 | 0x64 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020ae128 | `OnPushed` | `int(void*)` |
| 26 | 0x68 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b04e0 | `OnHitByCannonBlastedChar` | `int(void)`, `return 0;` |
| 27 | 0x6C | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020af27c | `OnHitByMegaChar` | `void(void*)` |
| 28 | 0x70 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020af04c | `OnHitFromUnderneath` | `void(Obj*)` |
| 29 | 0x74 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020af094 | `OnAimedAtWithEgg` | `void(Obj*)` |
| 30 | 0x78 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020aeed8 | `OnAimedAtWithEggReturnVec` | `void(char*)` |
| 31 | 0x7C | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b2880 | *(none)* -> `Virtual7C` | `void(void)` |
| 32 | 0x80 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b27f4 | *(none)* -> `Virtual80` | `void(void)` |
| 33 | 0x84 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b265c | *(none)* → `Virtual84` | `void(char *obj)` |
| 34 | 0x88 | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020ae3b4 | *(none)* → `Virtual88` | `void(char*,int,int,int,int)` |
| 35 | 0x8C | [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020ad660 | *(none)* → `Virtual8C` | `int(int*)` |

Fourteen of the eighteen carry a `recovered name:` comment in the legacy source, so
this is far less speculative than the header's wording suggests. Slot 18 has a name
too, from a different direction — see the next section. The three still unnamed (33,
34, 35) take the tree's established `Virtual<hex offset>` convention
(`include/fBase_c.h:143` — `Virtual34`, `Virtual38`).

\*\* The shape column records what the *legacy free function* looks like — except for
slot 18, which is now **measured** rather than read off the legacy shape:

- **Arity is `(int)`, not `()`.** All 24 free-function bodies are written `void(void)`
  or `void(char *self)`, but 13 of them read r1 and branch on it — `dScMgTeresa_c`
  takes an entirely different path when it is 0. Twelve unrelated classes do not read
  a garbage register by coincidence. The *base's* body ignores it, and that proves
  nothing either way: an unused argument is simply never read. Only an override that
  actually reads a parameter is evidence, and it gives a **lower** bound.
  `include/dActor_c.h:131` declares it with no parameter and is therefore **wrong for
  this branch** — a naming hint, never a signature authority.
- **Return type is `int`, not `void`.** `dScMgCoin_c::OnYoshiTryEat` is a real *member*
  definition ending `return 0;`; declaring `void` would have changed its bytes. The
  return type is not mangled, so the 24 free functions are unaffected. This is the same
  trap `dActor_c.h` documents for slots 21, 24 and 27 — re-measure each remaining slot
  against a real override before treating its legacy `void` as settled.

[ov006](../config/arm9/overlays/ov006/symbols.txt) named Coin's override
`_ZN11dScMgCoin_c13OnYoshiTryEatEv` — **wrong arity, corrected to `...Ei`.**

For slots 24, 25, 27, 28, 29, 30, 33, 34 and 35 the leading parameter in the legacy
source is really `this`; it becomes implicit when the free function turns into a method.

## Slot 18 is `OnYoshiTryEat`, not `Virtual48` (measured 2026-08-31)

Two independent sources agree, and neither of them is this table:

1. **`include/dActor_c.h:131`** declares `virtual int OnYoshiTryEat(); /* slot 18 */`,
   and a dozen further headers repeat the declaration at the same index — BabyPenguin,
   daObjFire_c, BobOmb, BookShot, Coin, Crate, CrazedCrate, `daDossyCap_c`, `daEyBm_c`,
   `daKpFr_c`, `daKrb_c`, `daKrpa_c`, `daObjMarioCap_c`, `daTrs_c`. `dActor_c` sits on
   the *other* branch of `fBase_c` (`fBase_c → dBase_c → dActor_c`, where this family
   is `fBase_c → dBase_c → dScene_c → dScMgBase_c`), so on its own this would only be
   suggestive.

2. **The cartridge.** Walking `_ZTV11dScMgCoin_c` ([ov006](../config/arm9/overlays/ov006/symbols.txt):0x0213bf50) against
   `_ZTV11dScMgBase_c` ([ov004](../config/arm9/overlays/ov004/symbols.txt):0x020bc0c0), 36 slots each, `dScMgCoin_c` overrides
   exactly three: 16 and 17 (the destructor pair) and **18, with [ov006](../config/arm9/overlays/ov006/symbols.txt):0x020de5b0** —
   which [ov006](../config/arm9/overlays/ov006/symbols.txt):545 **already names**
   `_ZN11dScMgCoin_c13OnYoshiTryEatEv`. That name was recovered and landed before this
   table existed, and it lands on this branch's slot 18.

So the `Virtual48` placeholder was wrong. The claim further down — "slot 18 stays
unnamed on both sides, which is consistent rather than a gap" — was consistent for the
wrong reason: two sources that both happened to lack the name, not two sources that
agreed there was none to have.

Watch the address point when reproducing this. A `_ZTV*` address in `symbols.txt`
**is** the address point, so slot *i* sits at `addr + 4*i` with no preamble to skip.
Adding `objisolate.VTABLE_PREAMBLE` (8) on top shifts every reading by two slots and
makes the destructor pair look like slots 18 and 19.

## The signature template for slots 18-30: `include/dActor_c.h:120-151`

`dActor_c`'s new-slot block is on the other branch but at **identical indices**, and
all thirteen names 18..30 agree with the `recovered name:` comments on this class's own
[ov004](../config/arm9/overlays/ov004/symbols.txt) bodies. It is the ready-made declaration block for PR B:

```c
/* --- new slots, 18..30, in declaration order. Do not reorder. --- */
virtual int  OnYoshiTryEat();                      /* slot 18 */
virtual int  OnTurnIntoEgg(Player &player);        /* slot 19 */
virtual int  Virtual50();                          /* slot 20 -- vtable+0x50 */
/* Slots 21, 24 and 27 return void, NOT int. Nothing in the tree reads them,
   so only an override with early returns can tell the difference -- and one
   of each does, allocating registers differently under `int` even with r0
   untouched. Measured, not assumed; see notes/actor-core-provenance.md 9. */
virtual void OnGroundPounded(dActor_c &other);          /* slot 21 */
virtual int  OnAttacked1(dActor_c &other);              /* slot 22 */
virtual int  OnAttacked2(dActor_c &other);              /* slot 23 */
virtual void OnKicked(dActor_c &other);                 /* slot 24 */
virtual int  OnPushed(dActor_c &other);                 /* slot 25 */
virtual int  OnHitByCannonBlastedChar(dActor_c &other); /* slot 26 */
/* The `Player &` this block proposed for slot 27 was dActor_c.h's and is
   MEASURED WRONG: both real bodies open `mov r4, r0` and then write r1 before
   reading it, so no second argument register is live on entry. Declared and
   landed as the no-parameter form. */
virtual void OnHitByMegaChar();                         /* slot 27 */
/* The `dActor_c &other` this block proposed for slot 28 came from
   dActor_c.h too, and came off for the same reason -- dScMgBase_c's own body
   writes r1 before it ever reads it and touches no other argument register.
   Measured once here rather than twice: dScMgSlot1_c's override calls the base
   as its first act, so a second argument would ride through untouched. */
virtual int  OnHitFromUnderneath();                     /* slot 28 */
virtual int  OnAimedAtWithEgg();                        /* slot 29 */
/* NOT a Vector3.  dActor_c.h:151 returns one by value, and slot 29's work
   refuted the transplant: at ov004:0x020ae168 slot 30 is dispatched with r0
   still holding `this` and r1 holding the loaded function pointer, where a
   12-byte return would put a hidden result pointer in r0 and `this` in r1
   under AAPCS.  The return type is open again; `int` is a placeholder. */
virtual int  OnAimedAtWithEggReturnVec();               /* slot 30 */
```

The parameter *types* are `dActor_c &` / `Player &` on that branch; on this one the
legacy sources pass raw `char *` / `void *`. The type does not change the slot's arity,
but it does change the mangled name — so settle it before renaming the [ov004](../config/arm9/overlays/ov004/symbols.txt) bodies,
not after.

**Slots 31-35 are not in `dActor_c.h`, and that is not an accident.** `dScMgBase_c`
is not a `dActor_c` descendant at all: `fBase_c -> dBase_c -> dScene_c -> dScMgBase_c`
against `fBase_c -> dBase_c -> dActor_c`. `fBase_c` declares slots 0-17 and `dBase_c`
adds no virtual, so the two branches each start appending their own at 18 and collide
on indices for the same reason two books have a page 19. Slots 18-30 borrowed
`dActor_c`'s names because the indices lined up; at 31 `dActor_c`'s table simply ends
and there is nothing left to borrow. `Kill` was carried in from `dBgActor_c`, which is
`dActor_c`'s CHILD -- a nephew branch, one fork further away still -- where
`_ZN10dBgActor_c4KillEv` at [ov002](../config/arm9/overlays/ov002/symbols.txt):0x020ee55c is that class's own new slot 31.
Slot 31 is recorded here as `Virtual7C`, the spelling `fBase_c` already uses for
`Virtual34`/`Virtual38` and the one slots 33-35 carry below.

**Slot 32 turned out to be the same defect one fork further out**, and it shipped as
`Virtual80`. `AfterClsn` IS a real ROM name -- `include/PathLift.h:58` declares it and
`_ZN16dPathLiftActor_c9AfterClsnEi` is a genuine mangled symbol -- but
`dPathLiftActor_c` derives from `dBgActor_c`, which derives from `dActor_c`:

```sh
fBase_c -> dBase_c -> dActor_c -> dBgActor_c -> dPathLiftActor_c
fBase_c -> dBase_c -> dScene_c -> dScMgBase_c
```

Two forks, not one, and that AfterClsn takes an `int` where this slot takes nothing.
The whole-image dispatch scan at +0x80 finds exactly three sites: [ov004](../config/arm9/overlays/ov004/symbols.txt):0x020b0900 (this branch) and [ov002](../config/arm9/overlays/ov002/symbols.txt):0x020effa4` + `[ov064](../config/arm9/overlays/ov064/symbols.txt):0x02116e58 (that one). Signatures for
33-35 still need reconstructing from their bodies: three slots, not eighteen.

**Slots 31 and 32 are one function twice, against the two display engines.** 31 is the
SUB screen, called last by `BeforeInitResources`; 32 is the MAIN screen, called first by
`AfterInitResources(u32)`. Same three read-modify-writes on `BG1CNT`, same scroll reset,
same `&= ~2` on the engine's BG-enable shadow (`data_0209d454` / `data_0209d45c` -- the
two words slot 30 restores the DISPCNTs from), same language-indexed character file plus
a shared screen map, file `0x5b` against `0x67`. 31 reaches them through
`G2S::GetBG1CharPtr` / `G2S::GetBG1ScrPtr`; 32 through `func_02054ea8` /
`_ZN2G212GetBG1ScrPtrEv`. That last pairing also identifies `func_02054ea8` as
`G2::GetBG1CharPtr` by position -- the G2S pair is `0x02054e88`/`0x02055148` and the G2
pair is `0x02054ea8`/`0x02055168`, the same `+0x20` apart -- but that is an arm9 rename
outside this campaign's scope and is deliberately NOT taken here.

## What PR B actually costs (census, 2026-08-31)

Walking every `_ZTV*` in the family against `_ZTV11dScMgBase_c`:

- **106 overrides at slots ≥ 18**, across **33 classes**. 100 of them fall in 18..35;
  6 sit at slot ≥ 36, on classes that add virtuals of their own on top.
- `dScMgBase_c` itself owns 18 of those — **82 are descendant-side.**

Per class: `dScMg3DEsp_c` 2, `dScMgAmida_c` 5, `dScMgBSC_c` 3, `dScMgBase_c` 18,
`dScMgBomroom_c` 1, `dScMgCard_c` 3, `dScMgCoin_c` 1, `dScMgCup_c` 2,
`dScMgCurling2_c` 1, `dScMgCurling_c` 1, `dScMgD3DBase_c` 9, `dScMgFlower_c` 1,
`dScMgHanachan_c` 1, `dScMgJump2_c` 2, `dScMgJump_c` 4, `dScMgLuigi_c` 1,
`dScMgMCarlo2_c` 2, `dScMgMCarlo_c` 2, `dScMgMemory2_c` 3, `dScMgMemory_c` 3,
`dScMgPachinko2_c` 1, `dScMgPachinko_c` 3, `dScMgPanel_c` 1, `dScMgRoulette_c` 2,
`dScMgSingle3DBase_c` 2, `dScMgSlot1_c` 4, `dScMgSlot3_c` 4, `dScMgSmartball_c` 3,
`dScMgSnowball_c` 4, `dScMgSound_c` 2, `dScMgTeresa_c` 3, `dScMgTrampoline2_c` 6,
`dScMgTrampoline_c` 6.

**This is why the change cannot be staged class by class** — but it decomposes
cleanly **slot by slot**, which is how it is actually being landed. Roughly 34 of these
classes already emit a vtable today. Declaring the base's eighteen slots *at once*
widens every one of those tables from 18 slots to 36 — and a descendant that does not
simultaneously declare its own overrides gets the *base's* body written into the
widened slot, where the cartridge holds its own. Those tables go from PARTIAL (a
byte-exact 18-slot prefix, ownable later) to **DIFFERS** (wrong bytes, a real
regression `romdata_check` will catch).

Declaring **one** base slot only widens every table by one word. After declaring base
slots 18..k an emitted table is k+1 slots long, and for every i ≤ k that slot is right
if the class either declares its own override or genuinely inherits the base body.
Slots above k are simply not emitted — still a byte-exact prefix, never a DIFFERS. So
the atomic unit is **one base slot across the whole family**: base declaration, base
body rename, and every descendant's override declaration plus rename, together. That
is eighteen reviewable commits, not one 158-change commit.

Measured on slot 18 (2026-08-31): `module fidelity 106/106`, `ROM data from source`
unchanged at 463 verified / 251 partial / 7 differ, and `partialBytes` **9,980 →
10,100** — exactly 30 emitted vtables each one word longer, all byte-exact. No table
regressed.

**Read that number twice.** Applying the 30 descendant declarations *without* the
base declaration also builds green — 106/106, PASS, no new DIFFERS — but at **10,088**
/ 27 tables. With the base silent above slot 17, each descendant's own
`virtual int OnYoshiTryEat(int);` introduces a brand-new virtual that happens to land
at index 18 in that class, pointing at the same body: byte-identical, semantically
wrong, and invisible to every gate in this tree. The three tables that only appear
once the base declares the slot are the classes with **no** override of their own;
their bytes matching is the independent confirmation that they genuinely inherit.
The widened-table **count** is the only check that distinguishes the two.

## Three hazards, all measured

**1. Slot 35 is a cross-overlay address collision.** 0x020ad660 is an overlay LOAD BASE, so [ov000](../config/arm9/overlays/ov000/symbols.txt), [ov002](../config/arm9/overlays/ov002/symbols.txt), [ov003](../config/arm9/overlays/ov003/symbols.txt), [ov004](../config/arm9/overlays/ov004/symbols.txt) and [ov007](../config/arm9/overlays/ov007/symbols.txt) each have a different, unrelated symbol there — [ov003](../config/arm9/overlays/ov003/symbols.txt)'s is `dScTitle_c`'s D1. The slot-35 body is [ov004](../config/arm9/overlays/ov004/symbols.txt)'s. The rename was scoped to the module-qualified symbol `func_ov004_020ad660` alone, which is what `apply_rename` takes; a filename- or address-keyed rename would have hit four unrelated overlays and every byte gate would still have passed. See the standing `cross-overlay-symbol-collision` note. (Handled — recorded here because the hazard is a property of the address, not of this campaign.)

**2. Each slot has to be atomic.** Declaring a virtual makes mwcc emit a vtable slot
referencing a `_ZN11dScMgBase_c*` mangled name. That symbol does not exist until the
[ov004](../config/arm9/overlays/ov004/symbols.txt) body is renamed. A declaration without its rename leaves the link with an undefined external; a rename without its declaration orphans it. Per slot: one commit,
or neither. Across slots, they are independent.

**3. Declaration order IS the index** — found while applying slot 18.
`dScMgSlot1_c` already declared `OnHitByMegaChar /* slot 27 */` and
`OnHitFromUnderneath /* slot 28 */`, and `dScMgAmida_c` declared `Unk36 /* slot 36 */`,
but with the base declaring nothing above 17 those comments were aspirational: the
declarations actually sat at indices 18/19 and 18. That is precisely why those two were
the last DIFFERS vtables left after PR #2081. Both of `dScMgSlot1_c`'s have since been
reconciled -- 27 with the base's slot-27 commit, 28 with slot 28's -- so that class now
declares nothing mwcc numbers for itself; `dScMgAmida_c`'s `Unk36` is the last one left,
and slot 35 lands it. A new base slot's override must be
inserted **before** any such pre-existing slot≥18 declaration, and after the class's
first declared virtual so the key function does not move. Slot 18 moved their
wrongness up one index without changing its size (Amida 4 bytes, Slot1 8); they clear
only once every intervening slot is declared.

## The per-slot worklist (measured 2026-08-31)

How large each PR was. Counts are descendant overrides; every slot also carried the
base declaration and the [ov004](../config/arm9/overlays/ov004/symbols.txt) base-body rename. All eighteen are landed or open.

| slot | name | base body ([ov004](../config/arm9/overlays/ov004/symbols.txt)) | descendant overrides |
|---|---|---|---|
| 18 | `OnYoshiTryEat` | 0x020b299c | **30 — this PR** |
| 19 | `OnTurnIntoEgg` | 0x020b2994 | 11 |
| 20 | `Virtual50` | 0x020b2990 | 4 |
| 21 | `OnGroundPounded` | 0x020b298c | 4 |
| 22 | `OnAttacked1` | 0x020ae198 | **0** |
| 23 | `OnAttacked2` | 0x020ae1a0 | 3 |
| 24 | `OnKicked` | 0x020ae140 | 6 |
| 25 | `OnPushed` | 0x020ae128 | 7 |
| 26 | `OnHitByCannonBlastedChar` | 0x020b04e0 | 19 |
| 27 | `OnHitByMegaChar` | 0x020af27c | 6 - **DONE**, 2 declarations |
| 28 | `OnHitFromUnderneath` | 0x020af04c | 6 - **DONE**, 2 declarations; the second and last occupied slot |
| 29 | `OnAimedAtWithEgg` | 0x020af094 | 6 - **DONE**, 2 declarations; the first since 26 with no occupied-slot trap, and the first whose NAME the ROM contradicts |
| 30 | `OnAimedAtWithEggReturnVec` | 0x020aeed8 | 6 - **DONE**, 2 declarations; the slot that settles 29 (it restores, word for word, what 29 saves) and the first whose name the ROM refutes in BOTH halves |
| 31 | `Virtual7C` | 0x020b2880 | 7 - **DONE**, 3 declarations; the first slot ABOVE `dActor_c`'s table, which is what proves the borrowed names never applied -- `dScMgBase_c` is a SCENE, a sibling branch, not an actor |
| 32 | `Virtual80` | 0x020b27f4 | 1 - **DONE**, 1 declaration; the MAIN-engine twin of 31, and the cheapest slot in the campaign after 22 |
| 33 | `Virtual84` | 0x020b265c | 19 - **DONE**, 2 declarations; structurally slot 26 again (same 19 tables, same two owning classes, 17 inherited) and the first slot whose override lives inside a PROMOTED intact-object TU |
| 34 | `Virtual88` | 0x020ae3b4 | 4 - **DONE**, 5 declarations; the first slot that takes ARGUMENTS (four, unanimous across seven call sites), the first whose purpose is legible from the body alone (a 4bpp pixel brush), and the first whose rename breaks a cross-file reference -- dScMgAmida_c's override calls the base body directly |
| 35 | `Virtual8C` | 0x020ad660 | 1 - **DONE**, 2 declarations; the LAST slot of the range, the smallest, and the best-evidenced -- 13 dispatch sites across four leaf classes, all 13 of which consume the return with `cmp r0,#0` |

**One deferred cleanup, from slot 31 onward.** The base bodies for slots 31, 32 and
33 (0x020b2880, 0x020b27f4, 0x020b265c -- ROM ordinals 47, 46 and 45) all live inside
the same src_tu candidate unit, [unit_ov004_020b0a38](../src_tu/actors/unit_ov004_020b0a38.cpp), which still
defines them under their old `func_ov004_*` names and whose manifest
[unit020b0a38.json](../config/tu_manifest.d/ov004/unit020b0a38.json) still lists the old
`legacy_source` paths.  Nothing is wrong today: that unit is `"status":
"text-verified"`, it is not enrolled, `src_tu/` is not in the ROM build, and both
gates that read it -- `check_src_tu_compiles.py` and `tiers_ratchet.promoted_moves()`,
the latter only looking at `"status": "promoted"` entries -- are green.  But the
unit is stale, and it will be stale again after 32 and after 33.  Regenerate it ONCE,
through `tools/tubuild.py`, rather than three times.

**That regeneration is DUE as of slot 33** -- all three base bodies carry their
mangled names now.  It is deliberately NOT folded into the slot-33 commit: one slot
per change is the whole safety argument for this campaign, and regenerating a
`src_tu/` unit touches a different tree read by a different gate.  It is a follow-up
PR of its own.


134 descendant overrides plus the base's 18 declarations. Slot 18 was the outlier;
the median slot touches six classes. **Slot 22 has no descendant overrides at all** —
base declaration and base rename only, the smallest commit in the campaign.

Four classes are intact TUs, so at every slot their override is renamed *inside*
`src/actors/<class>.cpp` rather than through a standalone source file:
`dScMgBSC_c`, `dScMgCard_c`, `dScMgMCarlo_c`, `dScMgMCarlo2_c`.

## What it unlocks

Full 36-slot vtable emission for `dScMgBase_c` and all 32 descendants, which is the
precondition for `production_mode: "intact-object"` ownership of the minigame family's
`_ZTV`/`_ZTI`/`_ZTS` instead of the `deadstrip-data` licence they carry today.

## Independent confirmation from a derived class (2026-08-30)

[ov006/dScMgBSC_c](../src/actors/dScMgBSC_c.cpp), promoted as a 19-function TU, **overrides three of these slots**, and its own bodies confirm the map from a second direction:

| slot | base body ([ov004](../config/arm9/overlays/ov004/symbols.txt)) | base name here | dScMgBSC_c's override |
|---|---|---|---|
| 18 | 0x020b299c | `OnYoshiTryEat` | [ov006](../config/arm9/overlays/ov006/symbols.txt):0x02125364 |
| 19 | 0x020b2994 | `OnTurnIntoEgg` | [ov006](../config/arm9/overlays/ov006/symbols.txt):0x0212527c |
| 21 | 0x020b298c | `OnGroundPounded` | [ov006](../config/arm9/overlays/ov006/symbols.txt):0x02125248 |

Two independent sources agree on 19 and 21. This table was built by walking
`_ZTV11dScMgBase_c`; the override addresses come from walking `_ZTV10dScMgBSC_c`. The
promoted source carries `// recovered name: dScMgBSC_c_OnTurnIntoEgg` and
`dScMgBSC_c_OnGroundPounded` comments that were recovered *before* either walk and
land on exactly those two addresses. Slot 18 carried no recovered name on either
side, which read as consistency at the time; the section above shows it was simply
absent from both, and the name is `OnYoshiTryEat`.

**This is also the measurement of what PR B costs.** `dScMgBSC_c`'s emitted
`_ZTV10dScMgBSC_c` is 0x50 bytes -- an 8-byte preamble plus 18 slots, so slots 0..17,
stopping exactly one slot short of 18. That is not a coincidence: the class declares
virtuals only through the destructor pair at 16/17, so the compiler has no name to put
in slot 18 and truncates. The TU reaches slot 18 today through a cast to a local shim
struct (`SelfVtblShim`) because no header declares it. Declaring slots 18-35 on
`dScMgBase_c` widens this table to the full 36 and lets that cast become a real virtual
call -- but it also means `dScMgBSC_c` starts emitting 18 more slots of vtable data
that must be licensed and byte-compared, on top of the eighteen [ov004](../config/arm9/overlays/ov004/symbols.txt) renames. Budget for the descendants' tables, not just the base's.
