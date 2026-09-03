# DTOR-FACES-CPP: the real-C++ destructor TUs of seated classes, linked from src

Lane 10 of `C:/tmp/link-scope/status/LINKSCOPE.md`. Branch `port/dtor-faces-cpp`
off 5600924cb. Nothing under src/ or include/ is touched; every change is in
port/ and this file.

## The wall, and the mechanism

Sixty-odd seated classes have a D1 in src/ that is the shadow-class C++ form
(`struct Actor { char pad[..]; virtual ~Actor(); }; struct Coin : Actor
{ CommonModel m0; ...; virtual ~Coin(); }; Coin::~Coin() {}`). mwcc turns that
into the byte-matched ROM body; MSVC turns it into `??1Coin@@UAE@XZ`, whose
synthesised member chain calls `??1CommonModel@@QAE@XZ`, `??1ShadowModel@@QAE@XZ`,
`??1MovingCylinderClsn@@QAE@XZ`, `??1WithMeshClsn@@QAE@XZ` and `??1Actor@@UAE@XZ`,
decorated __thiscall names nothing in the port defined (the bodies exist under
their ROM C names). Every fill so far answered with a transcription of the
ROM chain in a static thunk and left the matched TU out of the link.

The mechanism (`port/hal/dtor_faces_cpp.cpp`, `.h`, `port/slice_dtorfaces.txt`):

1. FACES. One real __thiscall definition per decorated shadow-class
   destructor, declared exactly as the src TUs declare it, forwarding `this`
   to the C body. Which body is read off the ROM relocations of all 67
   class-form D1 TUs: every base call is 0x020112c8 `_ZN5ActorD2Ev` (so the
   virtual-mangled `??1Actor@@UAE@XZ` forwards to D2, beside the seam's
   non-virtual `??1Actor@@QAE@XZ`); every member call is a plain `bl` to that
   member class's D1. Seven new faces here (Actor U; MovingCylinderClsn,
   MovingCylinderClsnWithPos, ShadowModel, WithMeshClsn, CommonModel,
   ExtendingMeshCollider Q); the other four the TUs ask for (Model, ModelAnim,
   TextureSequence, TextureTransformer) already exist in
   `unmatched/Mg3DEsp_Faces.cpp` and are linked in the same four targets.
   The set was measured, not predicted: all 64 candidate TUs compiled under
   the port's MSVC flags and their UNDEF `??1` symbols collected; the eleven
   names above are the whole list.
2. ADAPTERS. `hal_cppd1_<Class>`, the vtable-law __fastcall thunk whose body
   is the qualified call `((Class *)s)->Class::~Class()`, a direct call into
   the matched body. The fill's slot-16 line points at it; the transcribed
   static thunk is deleted.

Run-time difference, stated: the MSVC body stores its own `??_7Class@@6B@`
into the vptr where the ROM body stored the class table; nothing dispatches
in that window and Actor's D2 stores `_ZTV5Actor` as its first statement.
The member chain is the same calls at the same offsets.

## Scope check against the live tree

Per the coordinator's rule, every class was checked on THIS tree's own
walk_window.map (5600924cb, built in the worktree) before seating: none of
the 67 `??1<Class>@@UAE@XZ` symbols is in the map, and every slot-16 write
for the seated classes below was read off the fill source (a transcribed
thunk, a shared Actor-only thunk, or a host copy).

## Seated

Group 1, the proof: Coin (`hal/actor_classes_bob_world.cpp`, ROM 0x020b0f54,
ov002). Chain WithMeshClsn 0x1ac, MovingCylinderClsn 0x178, ShadowModel 0x150,
CommonModel 0x114 and 0xd8, Actor D2; MSVC's ??1Coin@@UAE@XZ disassembled
from the built object emits exactly those offsets. Linkage 9403 -> 9404.
Teardown selftest (walk_window, SM64DS_LEVEL=1 SM64DS_FAULTS_FATAL=1
SM64DS_WINDOW_SELFTEST=600 SM64DS_WARP_SEQ=6@100,7@200,8@300,15@400,1@500):
rc 0, 51 / 185 / 174 / 208 actors torn down on the four warps, no fault,
no declined teardown. vtspan --seats over dumpbin /disasm:nobytes of the
946 hal objects: 1689 checked, no wrong-body seats; --sweep rc 0.

Group 2, `hal/actor_classes.cpp` (a2de7dc2d): StarMarker (the host copy
`unmatched/StarMarker_D1.cpp` deleted), Bird, Butterfly, Fish, Door (the ROM
word 16 of `_ZTV4Door`, the table `hal_fill_star_door_vtable` binds),
PeachPainting, Flag, Cannon, and the three Actor-only bodies VirtualDoor,
PoppingLavaBubbles, AmbientSoundEffects whose slot was the shared
`ac_d1_actor_only` thunk (now deleted). 9404 -> 9415.

Group 3, the ov060/ov070/ov071/ov072 enemies: BowserTail, Bowser,
BowserSkyPlatform (the spike-bomb table), Amp (whose transcription had
dropped the ModelAnim member the ROM body tears down), FlameChomp,
FlameChompFire, MrI, MrI_Projectile, Scuttlebug, BabyPenguin, SnowmanBody,
SnowmanHead. 9415 -> 9427.

Group 4, the small-cast level overlays and ov002 tags (ab90ac188):
PyramidTag, CrazedCrate, BookShotSpawner, WingFeather, InvisibleSecret (the
vspopup number table `_ZTV15InvisibleSecret`; bob_world's "invisible secret"
fill is a different class), EnemySpawner, EnemySwitchTag, HauntedChair,
PrincessPeach, Toad, RacingPenguin, IceSlideManager, RollingLogLll,
RollingLogTtm, TreasureChest, Clam, CutsceneObject. 9427 -> 9444.

Group 5, the rest of the seated cast: MegaMushroomCreateTag,
ClockPaintingHandShort, SlideDecorationSilverStar, RotatingPlatformRr,
OrangeBallBillboard, Pokey, Tornado, Spiny, Lakitu, Moneybag, MugenBgm,
PushBlock, PoleLift, BlueFlame, KoopaFlag, BobOmbBuddy, MontyMole, OneUpLogo,
SkiLift (the ROM word of `_ZTV7SkiLift`, which the port fills as
MotherPenguin's table), Seaweed (`_ZTV7Seaweed`, HealingHeart's table),
BrickBlock, and ArrowLift (`_ZTV9ArrowLift`, filled as WaterDiamond's table;
its slot 16 was `ov29_trap16`, the one trap this lane replaces). Every
name-shifted seat was settled by `vtspan.py <symbol>` reading the ROM word
16 of the table the port storage stands for, and by the transcribed thunk's
chain agreeing with the D1 TU's ROM relocations. All 22 D1 TUs link (each
`_ZN..D1Ev` symbol present in walk_window.map); linked total 9467 (83.6%).
The branch was then rebased onto the cons base da58b6db0 (HUD-INITRES); the
CMakeLists slice and the gate-27 hostgen block are in far-apart regions and
the rebase took clean, followed by a full clean rebuild.

## Skipped, with the wall

- Trap (`_ZN4TrapD1Ev`, ov010): `hal/actor_classes.cpp`'s `tr_d1` ruling
  stands. TRAP (0x24) and LIGHT_BEAM (0x25) share the table with different
  layouts and the ROM chain (Model 0xd4, MovingCylinderClsnWithPos 0x124)
  fits only LIGHT_BEAM; the thunk switches on the id. Seating the matched
  body would run that chain on a 944-byte TRAP.
- InvisiblePole (`_ZN13InvisiblePoleD1Ev`): stores `_ZTV13InvisiblePole`,
  which is CameraTag's table, not the pole's (DTORPAIRS.md, same wall for
  its D0). CameraTag itself: no table in the port and no live edge.
- Cloud (`_ZN5CloudD1Ev`): ov039 is not mounted (LINKSCOPE lane 12).

## Proofs

Off tip cb75b8215, rebased onto cons base da58b6db0 and fully clean-rebuilt
(build/port wiped, all four targets and every smoke relinked from scratch,
all build-port.cmd guards green).

- Teardown under faults (the DTORPAIRS warp): walk_window.exe,
  `SM64DS_LEVEL=1 SM64DS_FAULTS_FATAL=1 SM64DS_WINDOW_SELFTEST=600
  SM64DS_WARP_SEQ=6@100,7@200,8@300,15@400,1@500`. rc 0, five warps
  (castle grounds -> 6 -> 7 -> 8 -> 15 -> castle grounds) with 51 / 185 /
  174 / 208 / 56 live actors torn down through `port_level_teardown`
  dispatching slot 16 on each, no fault, no declined teardown. The per-warp
  counts are non-vacuous, so the seated faces are exercised on real live
  actors, not a motionless census.
- vtspan.py --seats over `dumpbin /disasm:nobytes` of 950 hal objects
  (`build/port/_abicheck_disasm`): 1689 shared-arm9-body seats checked, NO
  wrong-body seats; the 22 group-5 adapters are class-body seats and each
  fill site points at its `hal_cppd1_<Class>` (source diff), no unseated
  regression.
- Linkage 9467 (83.6%); all 22 group-5 `_ZN..D1Ev` symbols present in
  walk_window.map; ptr_audit 0 unhosted code pointers.
- battery.py ALL GREEN: build, 20/20 smokes, every mounted level selftest
  under FAULTS_FATAL (the two skips -- level 27 TTC_MOVING_BEAM, level 45
  GOOMBOSS -- are decomp-owned unmatched bodies, not this lane), all 34
  scene selftests, default boot to title, shipcfg (PORT_ROM_CLEAN static
  CRT) build + selftest rc 0.
- Scope: no src/ or include/ change vs da58b6db0; the whole lane is port/
  (39 files) and this status file. The src D1 TUs were already matched
  decomp on the base; this lane only enrolls them into the port link and
  rewires slot 16.
