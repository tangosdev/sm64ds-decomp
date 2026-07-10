# Actor class naming from the ROM's spawn table

How 2,241 symbols across 68 modules got real names (tools/actor_names.py),
and where to look things up when drafting.

## Method

The EU arm9 statically links ACTOR_SPAWN_TABLE at 0x02090864: 391 SpawnInfo
pointers, one per actor ID (the runtime holder 0x020A4BB8 is assigned the
static table by the init at 0x0201A128; the widely-quoted 0x02006590 is the
US-region address and reads as zeros here). SpawnInfo layout (credit:
SplattyDS/DynamicAllocationDecomp, see CREDITS.md):

    +0 spawnFunc   +4 s16 behavPriority   +6 s16 renderPriority
    +8 flags       +c rangeOffsetY  +10 range  +14 drawDist  +18 unk

- actor ID -> name: DynamicAllocationDecomp include/List/ActorList.h (391 ids)
- spawn func: allocates the instance (mov r0,#size before the first bl) and
  stores the class vtable (last pool literal stored to [r4])
- vtable slot order = ActorBase virtual order, validated empirically on ov056
  BigMovingIceBlock (slot 6 Behavior == the byte-matched path follower at
  0x021112ac; slots 16/17 are the D1/D0 destructor pair):

    0 InitResources  1 BeforeInitResources  2 AfterInitResources(u32)
    3 CleanupResources  4 Before~  5 After~(u32)
    6 Behavior  7 BeforeBehavior  8 AfterBehavior(u32)
    9 Render  10 BeforeRender  11 AfterRender(u32)
    12 OnPendingDestroy  13 Virtual34(u32,u32)  14 Virtual38(u32,u32)
    15 OnHeapCreated  16 dtor D1  17 dtor D0

Slots pointing into arm9 are inherited defaults and stay unnamed. Functions
shared between sibling classes (identical-code folding) are claimed by the
lowest actor id; see symbols/actor_renames_report.txt for the audit trail.

Artifacts: symbols/actor_renames.tsv (what was renamed and why),
symbols/overlay_actors.md (which actors live in which overlay).

After pulling the renames, sync your local gitignored ledgers once:
run `python tools/actor_names.py --apply` (idempotent) or re-derive them.

## Overlay = actor group

Every small overlay is an actor bundle (ov022 = the Lethal Lava Land
platforms, ov056 = BigMovingIceBlock + ice slide, ...). Before drafting in an
overlay, read symbols/overlay_actors.md to know what the code IS, and check
reference/DynamicAllocationDecomp/include/Actors/<Class>.h for the struct
layout with field offsets - the alloc size in <Class>_Spawn should match the
header's size comments.

## Cross-reference sources for drafting semantics

- reference/DynamicAllocationDecomp/include/Actors/*.h - 182 actor structs
  with field offsets, enums, virtual sets. First stop for any actor overlay.
- decomp-refs/sm64 (n64decomp/sm64, clone locally): SM64DS reimplements the
  N64 game's design. When a target is identifiable object logic, read the N64
  behavior first: src/game/behaviors/<name>.inc.c (226 files, descriptive
  names), behavior index in data/behavior_data.c. Field vocabulary in
  include/object_fields.h (oPosX/oAction/oTimer...), Mario actions in
  include/sm64.h. Physics constants: src/game/mario_step.c.
- decomp-refs/nsmb (NSMB-Decomp/nsmb): same engine lineage and toolchain
  family (mwccarm 1.2 sp3, -O4,p). src/objectid.hpp annotates scene ids with
  their SM64DS names; src/Bases/* mirrors our Actor hierarchy; their
  Vec.hpp notes on member-init order affecting LDR/STR order match our
  codegen notes. No SDK source there by policy - names only.
- The game's own scene name registry follows the spawn table in arm9 data
  (BOOT/TITLE/STAGE/STAR_SELECT... strings) - internal EAD names, not yet
  harvested.
