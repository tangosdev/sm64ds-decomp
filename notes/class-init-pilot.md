# Class-initializer reconstruction pilot

Status: evidence report only.  Every `*_classInit` spelling and every C++ source
snippet below is a reconstruction hypothesis, not a symbol or source listing
preserved by the SM64DS ROM.

## Result

The factory architecture passes the pilot's structural threshold: 19 of 20
sampled entries (95%) are ordinary allocation/construction wrappers.  Eighteen
allocate the inferred class, directly install the most-derived vtable, and
return it; `PLAYER` allocates and calls a separate constructor.  The pilot does
not classify every non-vptr field store.  `STAR_CAMERA` remains uncertain and
is deliberately not forced into the model.

The exact global name `<Class>_classInit` does **not** pass as a universal rename
rule.  The complete candidate-level inventory yields 314 distinct class-based proposals,
and 42 of those proposals map to more than one factory address.  In the pilot,
five rows have a spelling collision.  Only 13 of 20 factory renames are marked
`apply_recommended=yes` in the dry-run map.

This distinguishes two conclusions:

- Tier A: most registry factories have the binary shape of a class initializer.
- Tier B: later EAD's `Class_classInit` spelling is a useful reconstruction
  candidate when it is unique and uncontradicted.
- Stop condition: a collision, unresolved most-derived class, or overlay-local
  ambiguity prevents an automatic rename.

## Method

`tools/profile_reconstruction.py` joins the actor registry to the existing
`tools/opnew_sizes.py`, RTTI/vtable, symbol, overlay, and TU evidence.  A
candidate must be a registry descriptor whose first word resolves to a known
operator-new site in the same runtime context.  This extra condition rejects
ordinary functions that happen to occupy the same overlay address.

Factory shapes mean:

- `new_plus_inlined_ctor`: operator new, direct most-derived vptr store, return;
  other field stores are not implied by the label.
- `new_plus_ctor_call`: operator new followed by a separate constructor call.
- `uncertain`: allocation is visible, but the most-derived class is not.
- `custom_factory`: substantial non-construction behavior (none in this sample).

Allocation and vtable values below are Tier A ROM observations.  Header class
sizes are independent project assertions checked against the allocation size.

## Per-profile evidence

| Profile | RTTI class | Factory at address | Allocation / class | Installed vtable | Constructor behavior | Shape | Candidate source | Confidence / exception |
|---|---|---|---:|---:|---|---|---|---|
| `BOOT` | `dScBoot_c` | `func_02023624` at `0x02023624` | `0x58 / 0x58` | `0x02091528` | most-derived vptr stored in factory | inlined | `return new dScBoot_c();` | B+ |
| `STAGE` | `dScStage_c` | `Stage_Spawn` at `0x0202e088` | `0x9c8 / 0x9c8` | `0x020921c0` | most-derived vptr stored in factory | inlined | `return new dScStage_c();` | B+ |
| `MINIGAME` | `dScMiniGm_c` | `func_ov005_020c21ec` at `0x020c21ec` | `0xb0 / 0xb0` | `0x020c2490` | most-derived vptr stored in factory | inlined | `return new dScMiniGm_c();` | B+ |
| `CANNON_SHUTTER` | `daObjCannonShutter_c` | `daObjCannonShutter_c_Spawn` at `0x020bcccc` | `0x330 / 0x330` | `0x02109d38` | most-derived vptr stored in factory | inlined | `return new daObjCannonShutter_c();` | B+ |
| `C1_TRAP` | `daObjC1_Trap_c` | `Trap_Spawn` at `0x02111998` | `0x3b0 / ?` | `0x02112ae4` | most-derived vptr stored in factory | inlined | `return new daObjC1_Trap_c();` | B+ shape; registry pointer is overlay-multiplexed |
| `STAR_CAMERA` | unresolved | `StarCamera_Spawn` at `0x020ebe5c` | `0xd4 / 0xd0` | base `dActor_c` vtable `0x0208e3a4` | calls `dActor_c` constructor; no most-derived vptr observed | uncertain | none | C; do not coin a class or `classInit` name |
| `PLAYER` | `daPly_c` | `Player_Spawn` at `0x020e6c0c` | `0x768 / 0x768` | `0x0210a83c` | calls `ov002:_ZN6PlayerC1Ev` | ctor call | `return new daPly_c();` | B+; class/constructor spellings differ in current symbols |
| `WATERFALL` | `daObjWaterfall_c` | `WaterfallMist_Spawn` at `0x020b6ee8` | `0xdc / 0xdc` | `0x021094a0` | most-derived vptr stored in factory | inlined | `return new daObjWaterfall_c();` | B+ |
| `KURIBO` | `daKrb_c` | `Goomba_Spawn` at `0x0212c0b0` | `0x478 / 0x478` | `0x02130948` | most-derived vptr stored in factory | inlined | `return new daKrb_c();` | B; three factory addresses collide on the candidate name |
| `TERESA` | `daTrs_c` | `Boo_Spawn` at `0x0211c590` | `0x5e0 / 0x5e0` | `0x0211e828` | most-derived vptr stored in factory | inlined | `return new daTrs_c();` | B; collides with `BOSS_TERESA` in the same TU |
| `BOSS_TERESA` | `daTrs_c` | `BigBoo_Spawn` at `0x0211c520` | `0x5e0 / 0x5e0` | `0x0211e828` | most-derived vptr stored in factory | inlined | `return new daTrs_c();` | B; collides with `TERESA` in the same TU |
| `WANWAN` | `daWanwan_c` | `ChainChomp_Spawn` at `0x02112d1c` | `0x620 / 0x620` | `0x021147ec` | most-derived vptr stored in factory | inlined | `return new daWanwan_c();` | B+ |
| `OBJ_MIP_KEY` | `daObj_Mip_Key_c` | `RabbitKey_Spawn` at `0x0212d4e8` | `0x1a0 / 0x1a0` | `0x021301d8` | most-derived vptr stored in factory | inlined | `return new daObj_Mip_Key_c();` | B+ |
| `PROPELLER_HEYHO` | `daPropeller_Heyho_c` | `FlyGuy_Spawn` at `0x02120520` | `0x3e8 / 0x3e8` | `0x02123168` | most-derived vptr stored in factory | inlined | `return new daPropeller_Heyho_c();` | B+ |
| `KINOKO_CREATE_TAG` | `daObjKinokoTag_c` | `daObjKinokoTag_c_Spawn` at `0x020b4a00` | `0x110 / 0x110` | `0x02108cf4` | most-derived vptr stored in factory | inlined | `return new daObjKinokoTag_c();` | B; two factory addresses collide on the candidate name |
| `SHOOT_BOOK` | `daBook_c` | `BookShot_Spawn` at `0x021128dc` | `0x454 / 0x454` | `0x0211495c` | most-derived vptr stored in factory | inlined | `return new daBook_c();` | B; three factory addresses collide on the candidate name |
| `BOOK_GENERATOR` | `daBookGen_c` | `BookShotSpawner_Spawn` at `0x02112850` | `0xd8 / 0xd8` | `0x021148d8` | most-derived vptr stored in factory | inlined | `return new daBookGen_c();` | B+ |
| `MG_CUP` | `dScMgCup_c` | `func_ov006_020e0574` at `0x020e0574` | `0x5470 / 0x5470` | `0x0213c154` | most-derived vptr stored in factory | inlined | `return new dScMgCup_c();` | B+ |
| `MG_CURLING` | `dScMgCurling_c` | `MgShuffleShell_Spawn` at `0x020e3820` | `0x4eec / 0x4eec` | `0x0213c304` | most-derived vptr stored in factory | inlined | `return new dScMgCurling_c();` | B+ |
| `MG_CURLING_J` | `dScMgCurling2_c` | `func_ov006_020e6bf4` at `0x020e6bf4` | `0x55c4 / 0x55c4` | `0x0213c510` | most-derived vptr stored in factory | inlined | `return new dScMgCurling2_c();` | B+ |

The compact word `inlined` in the table corresponds to the machine-readable
`new_plus_inlined_ctor` value.

## WATERFALL worked hypothesis

Direct SM64DS observations:

- registry profile `WATERFALL` points to the descriptor at `ov002:0x0210947c`;
- its first word points to `WaterfallMist_Spawn` at `0x020b6ee8`;
- the factory passes `0xdc` to `_ZN7fBase_cnwEj` at `0x02043444`, matching
  the known class size;
- after a non-null check it calls `_ZN8dActor_cC2Ev` at `0x0201150c`;
- it installs vtable `0x021094a0`, whose RTTI names
  `daObjWaterfall_c`;
- the body is construction-only and returns the allocation.

The evidence is naturally explained by this Tier B reconstruction:

```cpp
// Reconstructed source shape; exact SM64DS spelling is not preserved.
daObjWaterfall_c* daObjWaterfall_c_classInit()
{
    return new daObjWaterfall_c();
}
```

That source form is not yet a byte/link proof.  A natural `new` expression can
select a different allocator relocation even when its instruction bytes look
right, so any future migration still requires strict relocation and ROM gates.

## Collision evidence and naming consequence

The collision cases are architectural evidence, not noise:

- `daKrb_c_classInit`: three factories in `ov084`;
- `daTrs_c_classInit`: two factories in `ov063`, both in one recovered TU;
- `daObjKinokoTag_c_classInit`: two factories in `ov002`;
- `daBook_c_classInit`: three factories in `ov020`.

Later code could have used local/static functions, generated unique names, a
profile-qualified wrapper, or some other arrangement.  The ROM does not select
among those possibilities.  The pilot therefore keeps the structural
`profile -> factory -> class` model while rejecting a blind global rename rule.

## Acceptance decision

The architecture succeeds (95% ordinary wrappers).  The exact naming proposal
is conditional, not universal.  No source file or canonical symbol is renamed
by this pilot.
