# Actor class naming from the ROM's spawn table

How **2,241** symbols across **68** modules got real names [tools/actor_names.py](../tools/actor_names.py),
and where to look things up when drafting.

## Method

The EU arm9 statically links `ACTOR_SPAWN_TABLE` at `0x02090864: 391 SpawnInfo`
pointers, one per *actor ID* (the runtime holder **0x020A4BB8** is assigned the
static table by the init at **0x0201A128**; the widely-quoted **0x02006590** is the
US-region address and reads as zeros here). `SpawnInfo` layout (credit:
**SplattyDS/DynamicAllocationDecomp**, see [CREDITS.md](../CREDITS.md)):
```c
    +0 spawnFunc   +4 s16 behavPriority   +6 s16 renderPriority
    +8 flags       +c rangeOffsetY  +10 range  +14 drawDist  +18 unk
```
- **actor ID** -> name: **DynamicAllocationDecomp** `include/List/ActorList.h` (**391** ids)
- *spawn func*: allocates the instance (`mov r0,#size before the first bl`) and
  stores the *class vtable* (last pool literal stored to [r4])
- **vtable slot order** = `ActorBase` virtual order, validated empirically on `ov056`
  *BigMovingIceBlock* (slot `6 Behavior` == the byte-matched path follower at
  **0x021112ac**; slots `16/17` are the` D1/D0` **destructor** pair):
```c
    0 InitResources  1 BeforeInitResources  2 AfterInitResources(u32)
    3 CleanupResources  4 Before~  5 After~(u32)
    6 Behavior  7 BeforeBehavior  8 AfterBehavior(u32)
    9 Render  10 BeforeRender  11 AfterRender(u32)
    12 OnPendingDestroy  13 Virtual34(u32,u32)  14 Virtual38(u32,u32)
    15 OnHeapCreated  16 dtor D1  17 dtor D0
```
Slots pointing into arm9 are inherited defaults and stay unnamed. Functions
shared between sibling classes (identical-code folding) are claimed by the
lowest **actor id**; see [symbols/actor_renames_report.txt](../symbols/actor_renames_report.txt) for the audit trail.

Artifacts: [symbols/actor_renames.tsv](../symbols/actor_renames.tsv) (what was renamed and why),
[symbols/overlay_actors.md](../symbols/overlay_actors.md) (which actors live in which overlay).

After pulling the renames, sync your local gitignored ledgers once:
run `python tools/actor_names.py --apply` (idempotent) or re-derive them.

## Overlay = actor group

Every small overlay is an *actor bundle* (`ov022` = the **Lethal Lava Land**
platforms, `ov056` = *BigMovingIceBlock* + *ice slide*, ...). Before drafting in an
overlay, read [symbols/overlay_actors.md](../symbols/overlay_actors.md) to know what the code IS, and check
`reference/DynamicAllocationDecomp/include/Actors/<Class>.h` for the struct
layout with field offsets - the alloc size in `<Class>_Spawn` should match the
header's size comments.

## Cross-reference sources for drafting semantics

- `reference/DynamicAllocationDecomp/include/Actors/*.h` - **182** actor structs
  with field offsets, enums, virtual sets. First stop for any actor overlay.
---
- *decomp-refs/sm64* ([**n64decomp/sm64**](https://github.com/n64decomp/sm64), clone locally): SM64DS reimplements the
  N64 game's design. When a target is identifiable object logic, read the N64
  behavior first:

  1. `src/game/behaviors/<name>.inc.c` (**226** files, descriptive names);
  2. The behavior index in `data/behavior_data.c`;
  3. Field vocabulary in `include/object_fields.h` (**oPosX**/*oAction*/`oTimer`);
  4. Mario actions in `include/sm64.h`;
  5. Physics constants: `src/game/mario_step.c`.
---
- *decomp-refs/nsmb* ([**NSMB-Decomp/nsmb**](https://github.com/NSMB-Decomp/nsmb)): same engine lineage and toolchain family (`mwccarm 1.2 sp3, -O4,p`).
  
  1. `src/objectid.hpp` annotates scene ids with their SM64DS names;
  2. `src/Bases/*` mirrors our *Actor* hierarchy;
  3. Their `Vec.hpp` notes on member-init order affecting `LDR/STR` order match our codegen notes;
  4. No SDK source there by policy - names only.
---
- **Harvested.** The registry isn't separate from `ACTOR_SPAWN_TABLE` (EU
  `0x02090864`) - scenes and actors share the same 391-slot table. A parallel
  `char*` array, `ACTOR_DEBUG_NAMES` (EU `0x02090e80`, immediately after the
  spawn table), gives Nintendo's own EAD debug-build name for every slot -
  `GetActorDebugName()` ([src/GetActorDebugName.c](../src/GetActorDebugName.c), formerly `func_020233d4`) is
  the accessor, called from `ShowCrashScreen.c:66` to print the offending
  actor/scene's name on a crash. All 391 names + the `END_OF` sentinel are
  captured in [symbols/actor_debug_names.tsv](../symbols/actor_debug_names.tsv) (index, name, string address).
  Indices 0-8 are the scene cluster (BOOT/DSMT/TITLE/STAGE/STAR_SELECT/
  MINIGAME/ENTRY/RESULT/GAME_OVER), 9-359 are world actors, 360 is MULTIBOOT,
  361-390 are minigame names, 391 is the END_OF sentinel used by the `i < 0x188`
  bounds check in the accessor.

  This is **Tier-A-grade evidence** (a literal ROM string, see
  [notes/symbol-name-provenance.md](../notes/symbol-name-provenance.md)) for something Tier B explicitly couldn't
  prove: `ACTOR_SPAWN_TABLE`'s wiring proves *which vtable* a slot spawns, but
  "nothing in the binary says what \[a class\] means" - English glosses in
  [config/rom-name-glossary.json](../config/rom-name-glossary.json) and [symbols/overlay_actors.md](../symbols/overlay_actors.md) were
  community readings. This table is where the binary finally does say. It
  won't always agree word-for-word (EAD's internal codes are terser than the
  community's expanded English, e.g. raw `BC_SWITCH` vs. glossed
  `BLUE_COIN_SWITCH`). The first cross-reference is recorded in
  [ead-debug-name-crossref.md](../notes/ead-debug-name-crossref.md); it is deliberately
  partial, so check both sources before promoting another gloss.
