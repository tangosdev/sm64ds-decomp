# DTOR-PAIRS-C: the flat-C destructor pairs of seated classes, seated from src

Lane 8 of `C:/tmp/link-scope/status/LINKSCOPE.md`. Branch `port/dtor-pairs-c`,
rebased onto port-mount-noseat-cluster 3d048eb4e. Nothing under src/ or
include/ is touched; every change is in port/.

## What the lane does

A seated class whose vtable slot 16 (D1, the complete destructor) or 17 (D0,
the deleting destructor) pointed at a host thunk, a trap (`ac_trap17`,
`bw_trap17`, `wf_fill_shared`'s slot 17) or a host copy of the teardown chain,
while the class's own matched flat-C body sat unreferenced in src/. Each slot
now takes the matched body behind the ecx->arg adapter (the
`lk2_platform_dtor_seat.cpp` shape); the host copies are deleted; the TU rides
the slice its sibling D1/Init already rides. Every body byte-matches under
2004/b56, and for every one the ROM's word 16/17 of the class's own table
relocates to that body (`config/arm9/overlays/*/relocs.txt`, quoted in the
fill comments).

Placeholder names (`VT`, `VT0`, `VT1` from decl_common.h, `G0`) are settled per
source by reading the relocation of the store off the ROM and compiling that
one TU with a `COMPILE_DEFINITIONS` rename in port/CMakeLists.txt (the
BookShotSpawner / BrickBlock / Mecha03 shape). No shared dummy is relied on.

## Seated (34 bodies, 6 root-table bodies besides)

Group 1, `hal/actor_classes.cpp` slot-17 traps (af978750b): Bird, MetalNet,
Flag, Cannon, Butterfly, Fish, Trap, QuestionBlock D0.

Group 2, `hal/actor_classes_bob_world.cpp` slot-17 traps (c82557a06):
StarSwitch, CannonHatch, BrickBlock (VT=_ZTV10BrickBlock), FortressTower,
WaterBomb, RollingIronBall, ArrowSignRight D0.

Group 2b, five level overlays (fb4db508c): Whomp's Fortress MovingBarSmall,
TowerStep, PoleBillboard, KnockDownPlank D0; Jolly Roger Bay ShipUp,
SlidingBox and FloatOnWaterPlatformJrb D1+D0; IceBlock D1+D0; BasementWater
D1+D0 (the SwitchPillar-spelled src files); the clock hand's D0. Seven host
copies of the teardown chain deleted.

Group 3, the intro object and the bullies (165a8bd70): CutsceneObject D0
(VT=_ZTV14CutsceneObject, reloc 0x020f1fc4 -> 0x0210bd60); Bully D1+D0 and
BigBully D1+D0 (VT1=data_ov064_0211b768, all four second stores relocate to
ov064 0x0211b768). Two host copies deleted; BigBully stops borrowing Bully's
thunks.

Group 4, the castle water (dcfc9da7b): CastleWater D0
(VT0=_ZTV14daObjMcWater_c;VT1=_ZTV10dBgActor_c, relocs 0x02111ab4 ->
0x02113a18 and 0x02111ab8 -> 0x0210ae38). The last castle-grounds slot-17
trap.

Group 5, the three root tables (hal/dtor_seats_base.cpp): ActorBase,
ActorDerived and View D1+D0 into words 16/17 of data_02099edc, data_0208e4b8
and _ZTV4View, which no fill ever wrote. Nothing dispatches them; the seat is
the ROM's word and the reference edge. Called from port_stage_a2_seat beside
hal_seat_model_family_dtors.

## Skipped, with the wall

- InvisiblePole D0 (`_ZN13InvisiblePoleD0Ev`, ov002 0x020b076c): it stores
  `_ZTV13InvisiblePole` (0x0210853c), which is NOT the pole's table. The
  pole's factory installs data_ov002_02108480 (RTTI daBar_c; its D1/D0 are
  func_ov002_020b05d0/0600 and are already seated). 0x0210853c is installed
  by CameraTag_Spawn (reloc from:0x020b07f4), the gate-20 name shift, and
  CAMERA_TAG is refused in LINKSCOPE (no edge, no host table). Belongs to
  whichever lane seats CameraTag.
- ShadowModel D0 and ModelBase D1: the documented MSVC destructor-fold
  refusals (hal/model_dtor_seat.cpp, hal/w2_dtor_heads.cpp section 2). The
  folded slot holds the other half of the pair and there is no index for
  the second body.
- Nothing here belongs to the seat-ov002 lane (YoshiEgg, Fireball,
  BowserShockwaves, Bubble) or to the shadow-A lane's LINKSCOPE lane-5 rows;
  none of those classes has a flat-C D1/D0 pointed at a host thunk, so no
  pair was skipped on ownership grounds.

## Proofs

- Build: port/build-port.cmd per group (all four guards ahead of configure:
  closestplayer_guard, inferred_stub_guard "no new guessed vtable body
  seated", closure/facegen/mapdiff/vtablerows selftests, tailjump_guard
  forms OK), every target linked.
- Linkage (port/tools/linkage.py, walk_window.map): 9284 (82.0%) on the
  shared tree at 3d048eb4e, 9324 (82.3%) at this tip. +40 = the 34 pair
  bodies plus the 6 root-table bodies. Shadows unchanged (18 + 12 MSVC-name).
- Teardown under faults: walk_window.exe, SM64DS_LEVEL=1 SM64DS_FAULTS_FATAL=1
  SM64DS_WINDOW_SELFTEST=600 SM64DS_WARP_SEQ=6@100,7@200,8@300,15@400,1@500.
  Five level changes (castle grounds -> BOB -> Whomp's -> JRB -> LLL fire_mt
  -> castle grounds), every one through port_level_teardown dispatching slot
  16 on every live actor (51, 185, 174, 208, 56 actors torn down), rc 0, no
  fault, no declined teardown. That covers the castle-grounds four, the
  bob_world seven, the Whomp's / JRB / basement pairs and the bullies.
- vtspan --seats over dumpbin /disasm:nobytes of the 208 walk_window hal
  objects: 1669 fills checked, no wrong-body seats. --sweep rc 0. --fills
  rc 1 with byte-identical output to the shared tree (the two unresolved
  scene_vs_menu fills), pre-existing.
- Battery: `python port/tools/battery.py --skip-build` at 21b843956, ALL GREEN
  (smokes, every mounted level and hosted scene under FAULTS_FATAL=1, default
  boot, linkage 9324, ptr_audit, shipping config liveness). Standing skips
  unchanged: level 27 TTC_MOVING_BEAM, level 45 GOOMBOSS.
