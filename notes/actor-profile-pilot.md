# Actor/process profile reconstruction pilot

Status: evidence pilot complete; no symbols were renamed and no source files were
moved.  The machine-readable authority for the 20 sampled rows is
[`symbols/profile_reconstruction_pilot.tsv`](../symbols/profile_reconstruction_pilot.tsv).

## Verdict

`SpawnInfo` is functioning as the actor form of a broader SM64DS process-profile
system, but it is not the only descriptor layout.  The pilot contains:

- 14 actor profiles with the existing 0x1c `SpawnInfo` layout; and
- 6 scene/process profiles with an 0x08 `{factory, execute priority, draw priority}`
  layout.

The class and profile namespaces are demonstrably independent.  Examples include
`WATERFALL -> daObjWaterfall_c`, `KURIBO -> daKrb_c`,
`PROPELLER_HEYHO -> daPropeller_Heyho_c`, and the required pair
`MG_CURLING -> dScMgCurling_c` / `MG_CURLING_J -> dScMgCurling2_c`.

`g_profile_<ROM_ID>` is a consistent Tier-B spelling for the 20 sampled globals.
All 391 non-sentinel ROM debug identifiers are unique, so the spelling itself has no
full-population collision.  That does not make it a surviving SM64DS symbol.  One
sample, `C1_TRAP`, is deliberately marked do-not-apply because its numeric registry
pointer is overlay-multiplexed even though the profile ID is unique.

## Evidence boundary

- **Tier A:** actor-table index and pointer, literal ROM debug string and string
  address, descriptor bytes, factory pointer, allocation size, vtable, RTTI class,
  inheritance, and overlay/TU observations.
- **Tier B:** `g_profile_<ID>`, `<Class>_classInit`, and Nintendo-style filenames.
  These are reconstruction candidates, never relabeled as ROM symbols.
- **Tier C:** English/community labels such as `WaterfallMist`, `RabbitKey`,
  `Goomba`, and `FlyGuy`.

The lineage reference was pinned at NSMBW-Decomp revision
[`2e010f8708d8232c736b1ece507400dfd76aaa9c`](https://github.com/NSMBW-Community/NSMBW-Decomp/commit/2e010f8708d8232c736b1ece507400dfd76aaa9c).
Its profile header defines a base-profile and actor-profile split, generates
`className##_classInit`, and generates `g_profile_##profName` in
[`f_profile.hpp`](https://github.com/NSMBW-Community/NSMBW-Decomp/blob/2e010f8708d8232c736b1ece507400dfd76aaa9c/include/game/framework/f_profile.hpp).
That public decomp is lineage evidence, not direct proof of SM64DS spelling or macro
syntax.

## Sample design

The selection is fixed in `PILOT_SELECTION`; the same rationale is emitted in
the dataset's `selection_reason` column.

| Profile | Why it was selected |
|---|---|
| `BOOT` | base process on the scene/process side of the common registry |
| `STAGE` | ordinary stage scene and an arm9 TU-boundary stress case |
| `MINIGAME` | minigame root scene; tracked overlay gloss is stale |
| `CANNON_SHUTTER` | object actor with existing readable-C++ migration |
| `C1_TRAP` | true overlay-multiplexed registry pointer |
| `STAR_CAMERA` | factory whose most-derived RTTI/vtable does not survive |
| `PLAYER` | factory that calls a standalone constructor |
| `WATERFALL` | required canonical worked example |
| `KURIBO` | ordinary enemy whose ROM class differs from its English gloss |
| `TERESA` | first of two profiles constructing `daTrs_c` |
| `BOSS_TERESA` | second same-class profile; forces a `classInit` collision |
| `WANWAN` | ordinary enemy/actor with established RTTI and vtable |
| `OBJ_MIP_KEY` | required profile with current RabbitKey community naming |
| `PROPELLER_HEYHO` | enemy with existing migrated C++ |
| `KINOKO_CREATE_TAG` | object-tag actor with a prior evidence-bounded factory rename |
| `SHOOT_BOOK` | profile in a recovered multi-class, multi-profile TU |
| `BOOK_GENERATOR` | related second profile in that same TU |
| `MG_CUP` | minigame scene/process entry |
| `MG_CURLING` | required profile/class mismatch |
| `MG_CURLING_J` | related mismatch and same-address false-profile rejection case |

## Twenty sampled relationships

The direct-evidence text below is deliberately address-heavy.  The proposed names in
columns 3 and 5 remain Tier B.

| ROM profile ID | current descriptor | proposed profile | RTTI class | current factory | direct SM64DS evidence | profile/factory confidence |
|---|---|---|---|---|---|---|
| `BOOT` | `data_020914a8` | `g_profile_BOOT` | `dScBoot_c` | `func_02023624` | debug `BOOT` @ `0x0208f698`; descriptor `arm9:0x020914a8` -> factory `0x02023624` -> RTTI `0x020914c8` | `B+ / B+` |
| `STAGE` | `_ZN5Stage9spawnDataE` | `g_profile_STAGE` | `dScStage_c` | `Stage_Spawn` | debug `STAGE` @ `0x0208f708`; descriptor `arm9:0x0209213c` -> factory `0x0202e088` -> RTTI `0x02092158` | `B+ / B+` |
| `MINIGAME` | `data_ov005_020c2440` | `g_profile_MINIGAME` | `dScMiniGm_c` | `func_ov005_020c21ec` | debug `MINIGAME` @ `0x0208fabc`; descriptor `ov005:0x020c2440` -> factory `0x020c21ec` -> RTTI `0x020c2448` | `B+ / B+` |
| `CANNON_SHUTTER` | `daObjCannonShutter_c_SpawnInfo` | `g_profile_CANNON_SHUTTER` | `daObjCannonShutter_c` | `daObjCannonShutter_c_Spawn` | debug @ `0x02090678`; descriptor `ov002:0x02109d14` -> factory `0x020bcccc` -> RTTI `0x02109cf0` | `B+ / B+` |
| `C1_TRAP` | `Trap_SpawnInfo` | `g_profile_C1_TRAP` | `daObjC1_Trap_c` | `Trap_Spawn` | debug @ `0x0208f958`; `ov010:0x02112ac0` -> `0x02111998` -> RTTI `0x02112aa0`; the same numeric pointer has a second valid ov035 interpretation | `B / B+` |
| `STAR_CAMERA` | `StarCamera_SpawnInfo` | `g_profile_STAR_CAMERA` | unresolved most-derived class | `StarCamera_Spawn` | debug @ `0x020901f4`; descriptor `ov002:0x0210aa94` -> factory `0x020ebe5c`; allocation is `0xd4`, but only the `dActor_c` vtable survives | `B+ / C` |
| `PLAYER` | `Player_SpawnInfo` | `g_profile_PLAYER` | `daPly_c` | `Player_Spawn` | debug @ `0x0208f858`; descriptor `ov002:0x0210a704` -> factory `0x020e6c0c` -> RTTI `0x0210a56c` | `B+ / B+` |
| `WATERFALL` | `WaterfallMist_SpawnInfo` | `g_profile_WATERFALL` | `daObjWaterfall_c` | `WaterfallMist_Spawn` | debug @ `0x0208fd08`; descriptor `ov002:0x0210947c` -> factory `0x020b6ee8` -> RTTI `0x0210945c` | `B+ / B+` |
| `KURIBO` | `Goomba_SpawnInfo` | `g_profile_KURIBO` | `daKrb_c` | `Goomba_Spawn` | debug @ `0x0208f7d8`; descriptor `ov084:0x021308ec` -> factory `0x0212c0b0` -> RTTI `0x021308e0` | `B+ / B` |
| `TERESA` | `Boo_SpawnInfo` | `g_profile_TERESA` | `daTrs_c` | `Boo_Spawn` | debug @ `0x0208f7a8`; descriptor `ov063:0x0211e770` -> factory `0x0211c590` -> RTTI `0x0211e738` | `B+ / B` |
| `BOSS_TERESA` | `BigBoo_SpawnInfo` | `g_profile_BOSS_TERESA` | `daTrs_c` | `BigBoo_Spawn` | debug @ `0x02090284`; descriptor `ov063:0x0211e78c` -> factory `0x0211c520` -> the same RTTI `0x0211e738` | `B+ / B` |
| `WANWAN` | `ChainChomp_SpawnInfo` | `g_profile_WANWAN` | `daWanwan_c` | `ChainChomp_Spawn` | debug @ `0x0208f810`; descriptor `ov014:0x02114750` -> factory `0x02112d1c` -> RTTI `0x02114718` | `B+ / B+` |
| `OBJ_MIP_KEY` | `RabbitKey_SpawnInfo` | `g_profile_OBJ_MIP_KEY` | `daObj_Mip_Key_c` | `RabbitKey_Spawn` | debug @ `0x020902d8`; descriptor `ov085:0x021301b4` -> factory `0x0212d4e8` -> RTTI `0x02130194` | `B+ / B+` |
| `PROPELLER_HEYHO` | `FlyGuy_SpawnInfo` | `g_profile_PROPELLER_HEYHO` | `daPropeller_Heyho_c` | `FlyGuy_Spawn` | debug @ `0x02090748`; descriptor `ov070:0x02123144` -> factory `0x02120520` -> RTTI `0x02123120` | `B+ / B+` |
| `KINOKO_CREATE_TAG` | `MegaMushroomCreateTag_SpawnInfo` | `g_profile_KINOKO_CREATE_TAG` | `daObjKinokoTag_c` | `daObjKinokoTag_c_Spawn` | debug @ `0x02090824`; descriptor `ov002:0x02108cd0` -> factory `0x020b4a00` -> RTTI `0x02108c94` | `B+ / B` |
| `SHOOT_BOOK` | `BookShot_SpawnInfo` | `g_profile_SHOOT_BOOK` | `daBook_c` | `BookShot_Spawn` | debug @ `0x02090134`; descriptor `ov020:0x021148b4` -> factory `0x021128dc` -> RTTI `0x02114844` | `B+ / B` |
| `BOOK_GENERATOR` | `BookShotSpawner_SpawnInfo` | `g_profile_BOOK_GENERATOR` | `daBookGen_c` | `BookShotSpawner_Spawn` | debug @ `0x02090708`; descriptor `ov020:0x0211487c` -> factory `0x02112850` -> RTTI `0x02114838` | `B+ / B+` |
| `MG_CUP` | `data_ov006_0213c020` | `g_profile_MG_CUP` | `dScMgCup_c` | `func_ov006_020e0574` | debug @ `0x0208f830`; descriptor `ov006:0x0213c020` -> factory `0x020e0574` -> RTTI `0x0213c048` | `B+ / B+` |
| `MG_CURLING` | `MgShuffleShell_SpawnInfo` | `g_profile_MG_CURLING` | `dScMgCurling_c` | `MgShuffleShell_Spawn` | debug @ `0x0209008c`; descriptor `ov006:0x0213c214` -> factory `0x020e3820` -> RTTI `0x0213c2c4` | `B+ / B+` |
| `MG_CURLING_J` | `data_ov006_0213c434` | `g_profile_MG_CURLING_J` | `dScMgCurling2_c` | `func_ov006_020e6bf4` | debug @ `0x020904d8`; descriptor `ov006:0x0213c434` -> factory `0x020e6bf4` -> RTTI `0x0213c4bc` | `B+ / B+` |

## WATERFALL worked trace

Tier-A observations:

- actor/process index `197` has literal debug ID `WATERFALL` at `0x0208fd08`;
- its ov002 descriptor is `0x0210947c`, currently
  `WaterfallMist_SpawnInfo`;
- the 0x1c descriptor contains factory `0x020b6ee8`, priorities `197/21`,
  flags `0x00800003`, range words `0x00060000`, `0x00300000`,
  `0x02000000`, and trailing zero;
- the factory passes `0xdc` to `_ZN7fBase_cnwEj` at `0x02043444`, calls
  `_ZN8dActor_cC2Ev` at `0x0201150c`, stores vtable address point
  `0x021094a0`, and returns the object;
- RTTI at `0x0210945c` and its type-name at `0x02109468` spell
  `daObjWaterfall_c`; and
- both recovered TU boundaries around `0x020b6e08..0x020b6f18` are high
  confidence.

Tier-B reconstruction:

```cpp
daObjWaterfall_c* daObjWaterfall_c_classInit()
{
    return new daObjWaterfall_c();
}

const SpawnInfo g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    197, 21, 0x00800003,
    0x00060000, 0x00300000, 0x02000000, 0
};
```

The source form explains the factory and profile data.  The spellings do not survive
in SM64DS and remain reconstructed.

## Overlay-multiplexed registry entries

The registry is not a simple global `actor_id -> one descriptor object` map.  The full
extractor pass resolves all 391 indices, but 10 indices have two valid allocating
interpretations because mutually exclusive overlays reuse the same virtual address.
For example, pointer `0x02112ac0` means `C1_TRAP`/`daObjC1_Trap_c` in ov010 and a
different `CT_MECHA12L` profile in ov035.  The dry-run map therefore refuses both
`C1_TRAP` renames without additional overlay-context policy.

This is not the same as the stale ov098 `MgShellSmash_SpawnInfo` alias at
`0x0213c434`.  That word points at an ordinary `dActor_c` virtual method, not an
operator-new factory, so the stricter pilot rejects it.  The ov006 interpretation is
the sole class-initializing candidate for `MG_CURLING_J`.

## Answers

1. **Is `SpawnInfo` functioning as a profile?** Yes, as the 0x1c actor-profile
   descriptor.  A separate 0x08 base/scene-profile form also exists.
2. **Is `g_profile_<ROM_ID>` consistent?** Yes across the 20-row pilot, with unique
   ROM IDs across all 391 entries.  Overlay-multiplexed pointers still require
   context before applying a rename.
3. **Are profile and class names separate namespaces?** Yes, directly and repeatedly.
4. **Can the original type be called `ActorProfile`?** Not as a recovered SM64DS
   spelling.  Documentation should say “actor/process profile descriptor” and retain
   `SpawnInfo` where the 0x1c project type is required.

No profile rename was applied by this pilot.
