// The real level boot, host side.
//
// Everything the game needs to load the castle grounds for real -- the level
// overlay, the collision file, the object tables -- instead of the harness's
// hand-staged KCL and invented spawn point. Nothing here is behaviour:
// Stage::LoadClsnAndObjects and its fifteen sub-loaders are the matched src
// files (slice_gate14.txt), and this is the seam they need.
//
// ---- the overlay mount ----------------------------------------------------
//
// A DS overlay is linked at a fixed base and loaded there unrelocated, so the
// ROM image already carries absolute pointers -- the object tables, the CLPS
// block, the path nodes. Mounting it on the host is therefore two steps:
// copy the whole image into one host array (ovdata.py --whole; per-symbol
// arrays break every walk that steps past the symbol dsd happened to name),
// then rewrite the words the delink table says point back inside it.
//
// port_ovNNN_at() turns a DS address into the host address of the same byte,
// which is how every constant below is spelled.
//
// ---- WHICH overlay, and WHICH level -- the evidence ------------------------
//
// Three facts identify a level, and all three come out of the ROM rather than
// out of the overlay numbering.
//
// 1. LoadLevelOverlays(level) is `LoadOverlay(data_020758c8[level])`, so the
//    arm9 table at 0x020758c8 IS the level-to-overlay map. Read out of the
//    decompressed arm9 it is 52 words, and it happens to be contiguous:
//    level N -> overlay 8+N, level 1 -> ov009. That is a MEASUREMENT, not the
//    assumption -- the map is what the table says, and the table is read.
//
// 2. Which level a given overlay is comes from its own LVL_Overlay: the four
//    halfwords at +0x08 are the bmd/kcl/icg/icl OV0 HANDLES, and a handle is
//    resolved through build/assets/handles.tsv -- the ROM's own handle table,
//    which is what func_02018a24 reads and what hal/fs.cpp already loads to
//    open any file at all.
//
//    RESOLVE THE HANDLE, DO NOT ARITHMETIC IT. The handle-to-FAT relation on
//    the stage folders happens to be a constant subtraction, and deriving that
//    constant from one assumed pair got both levels below wrong: ov009 read
//    "main_garden" and ov015 read "bombhei_map", which put Bob-omb Battlefield
//    on level 7. It is not. Through the real table ov009 is main_castle (the
//    castle grounds, which is what the port has been booting all along) and
//    ov015 is battan_king_map -- Whomp's Fortress, battan king being the Whomp
//    King. Bob-omb Battlefield's bombhei_map is ov014's, so level 6.
//
// 3. Cross-checked twice, and both checks are decisive on their own.
//
//    SUBLEVEL_LEVEL_TABLE (arm9 0x02075298) maps a level to its COURSE number.
//    Entries 1..5 are all 29, the castle and its floors; entry 6 is course 0
//    and entry 7 is course 1. Course 0 is the first course, and the whole
//    table walks in course order from there -- 8 and 9 both course 2 (Jolly
//    Roger Bay and its ship), 10 and 11 both course 3 (Cool Cool Mountain and
//    its slide).
//
//    The per-level OBJECT overlay table LoadOrUnloadObjectOverlays walks
//    (data_02075998 selectors into data_02075804) gives level 6
//    ov062/069/078/084/091/095/100. ov078 is KingBobOmb, and level 6 is the
//    ONLY level in all 52 that loads it. The rest of the roster agrees:
//    Koopa the Quick and the Koopa's flag (ov062), Bob-omb Buddy and the
//    Goombas (ov084), the Stump and the sliding platform (ov091), SeesawBob,
//    the seesaw bridge (ov095), and the Chain Chomp (ov100).
//
// So: castle grounds = level 1, ov009, LVL_Overlay 0x02112bdc.
//     Bob-omb Battlefield = level 6, ov014, LVL_Overlay 0x02113434.
//
// Every level overlay is linked at the same base (0x021111a0) because the DS
// only ever holds one. On the host each is its own array with its own
// port_ovNNN_at(), so several can be mounted at once and the table below
// picks the one the boot walks.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "MeshCollider.h"

extern "C" {
void port_ov009_patch(void);
void *port_ov009_at(unsigned ds);
extern unsigned char port_ov009_image[];
extern const unsigned port_ov009_ds_base, port_ov009_ds_end;

void port_ov014_patch(void);
void *port_ov014_at(unsigned ds);
extern unsigned char port_ov014_image[];
extern const unsigned port_ov014_ds_base, port_ov014_ds_end;

/* ov010 = level 2, a castle interior floor (course 29). ov015 = level 7,
   Whomp's Fortress (course 1). Both mounted --whole like the two above; the
   generator emits these four symbols per overlay. Neither has its own
   per-symbol mount (own_sinits = 0), so a class that lives only in that
   overlay's data is unregistered and the pre-spawn gate skips it by name --
   the level still boots walkable. See the recipe beside port_level_table[]. */
void port_ov010_patch(void);
void *port_ov010_at(unsigned ds);
extern unsigned char port_ov010_image[];
extern const unsigned port_ov010_ds_base, port_ov010_ds_end;

void port_ov015_patch(void);
void *port_ov015_at(unsigned ds);
extern unsigned char port_ov015_image[];
extern const unsigned port_ov015_ds_base, port_ov015_ds_end;

/* ov011 = level 3, the castle garden (data/stage/main_garden), course 29. The
   level->overlay map data_020758c8[3] = 11 (it is level+8, a read not an
   assumption), the LVL_Overlay table data_02092208[3] = 0x021113ac, and the
   four OV0 handles at LVL_Overlay+8 resolve through build/assets/handles.tsv to
   main_garden's bmd/kcl/icg/icl -- the same evidence path the four above use.
   SUBLEVEL_LEVEL_TABLE[3] (0x02075298) is 0x1d = course 29, the castle family,
   an ordinary outdoor level with subCount=1 and only the standard hosted
   loaders. Mounted --whole like the four above; own_sinits stays 0. */
void port_ov011_patch(void);
void *port_ov011_at(unsigned ds);
extern unsigned char port_ov011_image[];
extern const unsigned port_ov011_ds_base, port_ov011_ds_end;

/* ov017 = level 9, the sunken pirate ship (data/stage/kaizoku_ship), course 2 --
   Jolly Roger Bay's ship sublevel. The level->overlay map data_020758c8[9] = 17
   (it is level+8, a read not an assumption), the LVL_Overlay table
   data_02092208[9] = 0x02111a78, and the four OV0 handles at LVL_Overlay+8
   (bmd 1894 / kcl 1892 / icg 1895 / icl 1896) resolve through
   build/assets/handles.tsv to kaizoku_ship's bmd/kcl/icg/icl -- the same
   evidence path the five above use. SUBLEVEL_LEVEL_TABLE[9] (0x02075298) is
   0x02 = course 2, an ordinary indoor stage with subCount=1 and only the
   standard hosted loaders. Mounted --whole like the five above; own_sinits
   stays 0.

   ITS SKIPPED CAST, and why two of the three stay skipped. Level 9's census
   names three unregistered classes. EXCLAMATION_BLOCK (21) is hosted at gate
   128 -- it shares QUESTION_BLOCK's already-filled vtable, so it cost only its
   factory. The other two each need a whole overlay's worth of new mount and
   are documented blockers, not fake-booted:

     - TREASURE_CHEST (13, ov064): fully decompiled (7 methods + two ov064
       helpers func_ov064_0211a284/_0211a734), self-contained in ov064. BLOCKED
       on a fresh per-symbol ov064 mount: ov064 is not mounted at all, and the
       class reaches its SpawnInfo (0x0211c4e8), _ZTV13TreasureChest and two
       bss SharedFilePtrs (data_ov064_0211c964/_0211c96c) by name. Those file
       pointers are constructed by ov064's ten static initialisers, so hosting
       it is the gate-64/gate-95 shape: a per-symbol ov064 mount, its sinits,
       the vtable fill and the row. A multi-part gate, deferred.

     - SHIP_WATER (62, ov017): fully decompiled, but its byte-matched
       InitResources references its own overlay's statics by the ov055/ov056
       symbol SPELLINGS (data_ov056_02111a60, data_ov055_02111a94) -- ov017,
       ov055 and ov056 share base 0x021111a0, and the decomp TU was written
       against the sibling overlays' names for the same bytes. Hosting it needs
       a per-symbol ov017 mount (ShipWater_SpawnInfo, _ZTV9ShipWater,
       data_ov017_02111c88) PLUS an alias-by-address that resolves the ov055/
       ov056 spellings onto ov017's host bytes -- the "propagate config renames
       BY ADDRESS" hazard. Its Behavior also gates on TREASURE_CHEST (waits for
       every id-13 actor to be opened), so it wants that class first. Deferred
       to a dedicated gate; the level boots and is walkable without it. */
void port_ov017_patch(void);
void *port_ov017_at(unsigned ds);
extern unsigned char port_ov017_image[];
extern const unsigned port_ov017_ds_base, port_ov017_ds_end;

/* ov016 = level 8, Jolly Roger Bay (data/stage/kaizoku_irie), course 2 -- the
   first MAIN COURSE the port mounts, a big open outdoor stage. data_020758c8[8]
   = 16, LVL_Overlay data_02092208[8] = 0x02113b90, OV0 handles at +8 (bmd
   0x0761, kcl 0x075f, icg 0x0762, icl 0x0763) resolve through handles.tsv to
   kaizoku_irie's all.bmd/kcl/icg/icl; SUBLEVEL_LEVEL_TABLE[8] = 2 = course 2.
   384 load relocs, a 25609-byte KCL. Mounted --whole; own_sinits 0.

   GATE 187: L8 boots, walks and warps in and out clean. Census 182 spawned
   (29 classes: PLAYER, COIN x61, STATIC_ROCK x33, GOOMBA x8, SIGN_POST x8,
   RED_COIN/BLUE_COIN x8, FISH x6, CAP x5, STAR_MARKER x5, POWER_STAR x4,
   INVISIBLE_POLE x4, and the rest of the furniture), 29 skipped (12 classes).
   The warp handoff tears down clean both ways in one round: L1->L8 marks 51 and
   leaves 0; L8->L6 marks 182 and leaves 0; L8->L1 marks 177 and leaves 0. An
   8-viewpoint proximity sweep (240f each, the whole x[-12000..12000]/z[same]
   footprint) and 300f/900f idle soaks all exit clean with no crash.txt and no
   exit.txt. None of L7's teardown/render/sign-drain culprits (CAP-arg,
   FIRE_PIRANHA ov084, BLUE_COIN_SWITCH G0, the TowerStep/MovingBarSmall sign
   drain, the HealingHeart slot-5) fire here: those actors are ov015's, and JRB
   spawns none of them.

   ITS SKIPPED CAST, all 12 named off the ROM spawn table + config symbols.
   Every one boots walkable with the class skipped and named in the census; each
   is a deferred multi-part gate (the gate-64/95 per-symbol mount + sinits +
   vtable fill + row shape), not fake-booted:

     ov016-resident (6 classes, need a per-symbol ov016 mount running its five
     __sinit_ov016_* to construct the SharedFilePtrs, since the whole-mount
     leaves own_sinits 0):
       - ShipDown (56), ShipUp (57): the sunken ship's rise/sink platforms.
       - RockPillar (58, x6): the rising rock pillars.
       - FloatOnWaterPlatformJrb (60): a MovingMeshCollider. Its InitResources
         installs MeshColliderBase::UpdatePosWithTransform as the BeforeClsn
         callback (func_020393d4 into the +0x124 collider) -- the EXACT
         contested-slot family c9a1731da seats (the walls-area bridge fix). It
         is the level's first rideable moving collider; hosting it is what the
         RIDE sweep needs, and riding it is the direct test that this tree's
         c9a1731da seating covers a JRB mover, not only ov015's bridge.
       - Unagi (242, x3): the eel.
       - SlidingBox (313): a sliding platform.

     ov064-resident (3 classes, shared with the ship's cast, the same ov064
     mount the level-9 note defers):
       - TreasureChest (13, x4), JetStream (245, the water current), Clam
         (315, x5).

     ov002-resident (2 classes, already-mounted overlay but each an
     alias-collision risk, not a clean free-match):
       - Seaweed (296, x4): id 296 is a DIFFERENT actor from the id-297
         HealingHeart that already reuses _ZTV7Seaweed in the port
         (actor_classes_bob_world.cpp); hosting 296 needs that binding
         disentangled first.
       - MugenBgm (351): the looping-BGM trigger.

     ov102-resident (1 class): KoopaShell (285), the ridden shell, needs the
     ov102 shell-spawn path.

   The RIDE sweep found NO active moving collider to ride: the collider registry
   after boot is slot 0 only (the level's static 645-tri KCL, owner NULL),
   because all of JRB's movers are in the skipped cast above. So the ride sweep
   is a no-op until FloatOnWaterPlatformJrb (or another mover) is hosted -- the
   contested-slot bomb cannot fire on a mover that never spawns. Flagged for the
   follow-on gate.

   GATE 188: the six ov016 MOVERS are now hosted (per-symbol ov016 mount in
   ov016_syms.txt + slice_gate188.txt + hal/actor_classes_jrb.cpp). Census is now
   193 spawned / 35 classes, 22 skipped / 7 classes. All six spawn:
   SHIP_DOWN (56) x1, SHIP_UP (57) x1, ROCK_PILLAR (58) x6,
   FLOAT_ON_WATER_PLATFORM_JRB (60) x1, UNAGI (242) x3, SLIDING_BOX (313) x1.

   THE CONFIG NAMES ARE A DECOY (the gate-178 Amilift pattern, from ov016 relocs):
   id 60 installs data_ov016_02114bcc (the daObjKi_Ita_c base, plain Platform
   defaults for Behavior/Cleanup/Render); id 313 installs the DERIVED
   _ZTV23FloatOnWaterPlatformJrb, which carries the slot-5 Render and the COUPLED
   Behavior (case 0 FindWithActorID(0x39=SHIP_UP) -> self-destruct if the ship is
   absent). The chain survives because SHIP_UP is hosted.

   RIDE SWEEP RESULT: SHIP_UP is a MovingMeshCollider; spawning Mario ON it
   (SM64DS_SPAWN=4850,1100,2250) runs 900 frames clean under SM64DS_FAULTS_FATAL=1
   -- the direct proof that gate-186's contested-slot seating (mmc_vtable.cpp)
   covers a JRB mover, not only ov015's bridge. The bomb did NOT fire. Idle soaks
   (300f/900f, Mario at the entrance) are clean too.

   TWO documented residues:
     - ROCK_PILLAR (58) faults c0000005 in ModelComponents::Render the first
       on-screen frame it draws (measured f17 when Mario spawns adjacent), NOT the
       collider and NOT the particle path (SM64DS_NO_FX_RENDER=1 still crashes;
       SM64DS_RP_NORENDER moves the fault elsewhere in the same sequence). Its
       render (func_ov016_02112b28) is the identical slot-5 Model dispatch that
       SHIP_UP renders through cleanly, so the difference is the MODEL FILE: its
       bmd (fs id 1173) loads but its Model at +0xd4 reaches Model::Render with a
       bad/unbuilt ModelComponents. A model-BUILD issue for RockPillar's bmd, not
       a mount or seating one -- deferred to the model-loader lane. Idle-far and
       the other five movers are clean; the per-actor quarantine catches it, so
       JRB stays walkable with RockPillar quarantined on approach.
     - Each ROCK_PILLAR spawns a RockTriangle child (id 59, ov102, Actor::Spawn
       (0x3b) in its InitResources) -- newly visible now that RockPillar runs its
       Init. RockTriangle (x6) is skipped (unregistered); the ov102 mount for it
       and KoopaShell (285) is the next follow-on. */
void port_ov016_patch(void);
void *port_ov016_at(unsigned ds);
extern unsigned char port_ov016_image[];
extern const unsigned port_ov016_ds_base, port_ov016_ds_end;

/* ov012 = level 4, a castle basement floor (data/stage/castle_b1), course 29.
   data_020758c8[4] = 12, LVL_Overlay data_02092208[4] = 0x02111c54, OV0 handles
   at +8 (raw 0x0615/0x061b/0x061c/0x061d, +232 the overlay handle base) resolve
   through handles.tsv to castle_b1's bmd/kcl/icg/icl; SUBLEVEL_LEVEL_TABLE[4] =
   0x1d = course 29, interior-shaped like level 2. Mounted --whole; own_sinits 0. */
void port_ov012_patch(void);
void *port_ov012_at(unsigned ds);
extern unsigned char port_ov012_image[];
extern const unsigned port_ov012_ds_base, port_ov012_ds_end;

/* ov013 = level 5, the castle second floor (data/stage/castle_2f), course 29.
   data_020758c8[5] = 13, LVL_Overlay data_02092208[5] = 0x02111844, handles at
   +8 (raw 0x060b/0x0612/0x0613/0x0614, +232) resolve to castle_2f's
   bmd/kcl/icg/icl; SUBLEVEL_LEVEL_TABLE[5] = 0x1d = course 29. Mounted --whole. */
void port_ov013_patch(void);
void *port_ov013_at(unsigned ds);
extern unsigned char port_ov013_image[];
extern const unsigned port_ov013_ds_base, port_ov013_ds_end;

/* ov018 = level 10, Cool Cool Mountain (data/stage/snow_mt), course 3 -- the
   mountain main course, a big open outdoor stage like Jolly Roger Bay. The
   level->overlay map data_020758c8[10] = 18 (it is level+8, a read not an
   assumption), the LVL_Overlay table data_02092208[10] = 0x02112f2c, and the
   four OV0 handles at LVL_Overlay+8 (raw 0x06d7/0x06d5/0x06d8/0x06d9) resolve
   through build/assets/handles.tsv -- the overlay's handle base is a constant
   +232 above the tsv index, confirmed across all nine known-good levels -- to
   snow_mt's all.bmd/kcl/icg/icl. SUBLEVEL_LEVEL_TABLE[10] (0x02075298) is
   0x03 = course 3, a 46876-byte KCL. Mounted --whole like the eight above;
   own_sinits stays 0.

   ITS SKIPPED CAST, and why the rest stay skipped. Level 10's census used to
   name eleven unregistered classes; gate 190 hosts two of them --
   ICE_SHEET (295) and POWER_STAR_CREATE (355), both ov018-RESIDENT -- on a
   new per-symbol ov018 mount (port/ov018_syms.txt, hal/actor_classes_ccm.cpp)
   riding port_actor_overlays_sinits the way ov015's does (gate 59). RED_FLAME
   (316, ov002) was ALSO hosted, at gate 175 (14h after this block was first
   written) -- it reuses BLUE_FLAME's filled vtable, the
   ExclamationBlock/QuestionBlock shape.

   GATE 191 hosts SKI_LIFT (63, x1) and MOTHER_PENGUIN (257, x1), the two
   remaining ov018-resident classes. Both blockers above are resolved: main's
   c19c90882 (#1301) fixed SkiLift::InitResources's data_ov036/ov056/ov022
   sibling-overlay spellings to their own ov018 names (carried into this
   worktree byte-identical); func_ov018_02111d28 is matched (its ShadowModel
   RadHeight call spells the by-value-Fix12 ABI signature directly). The
   eight src/_ZN7SkiLift* files turned out to be MOTHER_PENGUIN's own bodies
   under a dsd-era class-identity mislabel, not the real SkiLift's -- see
   port/slice_gate191.txt. Six remain, grouped by blocker:

     - ACTOR-OVERLAY classes needing a fresh per-symbol mount, none a free
       share: ICE_BLOCK (18, x4, ov081), MR_BLIZZARD (223, x3, ov081) and
       SPINDRIFT (312, x5, ov080/ov081) live in ov080/ov081, neither mounted;
       BABY_PENGUIN (256, x2, ov070/ov072) and id 272 (SNOWMAN, x1,
       ov070/072/074) live in ov070's set, unmounted; HOOT_THE_OWL (234, x1,
       ov006/ov094) lives in ov006/ov094, unmounted. Each is the
       gate-64/gate-83 shape (a new per-symbol overlay mount, its sinits, the
       vtable fill and a row).

   All six are turned away by the pre-spawn gate by name and named in the
   census, so the boot is honest about what did not spawn.

   GATE 190 FIX ROUND, a play-time discovery the boot census could not see:
   hosting POWER_STAR_CREATE spawns a fifth, free-flying POWER_STAR whose
   state body func_ov002_020e7e24 retries Actor::SpawnSoundObj(6) every frame
   until the spawn SUCCEEDS -- so SOUND_OBJECT (359, ov002, fully matched) had
   to be hosted with it (registry row + PMF cell seat + Behavior host copy,
   hal/actor_classes_ccm.cpp + port/unmatched/SoundObject_Behavior.cpp). The
   end-of-run census is the witness for this class, not the boot one. */
void port_ov018_patch(void);
void *port_ov018_at(unsigned ds);
extern unsigned char port_ov018_image[];
extern const unsigned port_ov018_ds_base, port_ov018_ds_end;

/* ov019 = level 11, Cool Cool Mountain's slide (data/stage/snow_slider), course
   3. data_020758c8[11] = 19, LVL_Overlay data_02092208[11] = 0x02112c68, OV0
   handles at +8 (bmd 0x07c4/kcl 0x07c2/icg 0x07c5/icl 0x07c6) resolve through
   handles.tsv to snow_slider's all.bmd/kcl/icg/icl; SUBLEVEL_LEVEL_TABLE[11] =
   3 = course 3. A 57805-byte KCL. Mounted --whole; own_sinits 0.

   GATE 189, the proof pass (post-188 build): boots with the FULL placed cast,
   84 spawned \ 12 classes, 0 skipped -- the first course to boot with no
   skip census at all. 900f soaks are deterministic (two runs byte-identical
   on stdout) and clean under SM64DS_FAULTS_FATAL=1; proximity soaks at the
   ICE_SLIDE_MANAGER, the DOOR, and the far COIN cluster all exit 0 with no
   crash artifacts. Two of those proximity runs are cross-level witnesses:
   walking into the DOOR fires a real handoff (11 -> 10 entrance 4, clean
   heap teardown), and the coin-cluster spawn point rides the death/OOB exit
   (11 -> 1 entrance 13 reason 2) into a fully-hosted castle grounds.
   GATE 190: the one PLAY-TIME gap the boot census could not see is closed --
   nearing the slide used to spawn two ONE_UP_LOGO (331, 0x14b) that skipped
   as unregistered; OneUpLogo is now hosted (its registry row joined the
   level-10 CCM slice gate, ov002's already-mounted per-symbol window --
   OneUpLogo_Spawn 0x020f1170, _ZTV9OneUpLogo 0x0210b1ac). Both now spawn. */
void port_ov019_patch(void);
void *port_ov019_at(unsigned ds);
extern unsigned char port_ov019_image[];
extern const unsigned port_ov019_ds_base, port_ov019_ds_end;

/* ov020 = level 12, Big Boo's Haunt (data/stage/teresa_house), course 4.
   data_020758c8[12] = 20, LVL_Overlay data_02092208[12] = 0x021138fc, OV0
   handles at +8 (bmd 0x07d2/kcl 0x07cc/icg 0x07d3/icl 0x07d4) resolve directly
   through handles.tsv to teresa_house's all.bmd/kcl/icg/icl;
   SUBLEVEL_LEVEL_TABLE[12] = 4 = course 4, subCount 7. Mounted --whole. */
void port_ov020_patch(void);
void *port_ov020_at(unsigned ds);
extern unsigned char port_ov020_image[];
extern const unsigned port_ov020_ds_base, port_ov020_ds_end;

/* ov022 = level 14, Lethal Lava Land (data/stage/fire_land), course 6 -- the
   first COURSE-6 main course the port mounts. data_020758c8[14] = 22 (level+8,
   a read not an assumption), LVL_Overlay data_02092208[14] = 0x02113228, the
   four OV0 handles at LVL_Overlay+8 (bmd 0x0742 / kcl 0x0740 / icg 0x0743 /
   icl 0x0744) resolve DIRECTLY through build/assets/handles.tsv -- the ov0
   handle in the overlay image is the tsv handle, no arithmetic, the delta-0
   mapping the known-good levels use -- to fire_land's all.bmd/kcl/icg/icl.
   The handles are read from the RAW ndspy overlay image
   (extracted/overlays/overlay_0022.bin), the same image ovdata.py mounts; the
   dsd export is stale against config re-addressings, which is why an early
   read of the dsd copy gave the wrong words. SUBLEVEL_LEVEL_TABLE[14]
   (0x02075298) is 0x06 = course 6, a main course with subCount 1. Mounted
   --whole like the rest; own_sinits stays 0.

   ITS SKIPPED CAST, and why every class stays skipped for now. Level 14's
   census names 22 unregistered classes; each id resolves through
   ACTOR_SPAWN_TABLE (arm9 0x02090864 + id*4) to a SpawnInfo, and the level's
   object overlays are ov064/ov071/ov080/ov084/ov095 (from data_02075998[14]
   selecting into data_02075804 -- the same LoadOrUnloadObjectOverlays walk the
   level-6 note beside port_level_table[] describes; ov084/ov095 are already
   mounted for other levels, ov064/ov071/ov080 are not). None of the 22 is a
   free share of an already-hosted vtable. Grouped by blocker:

     - ov022-RESIDENT (SpawnInfo in this level overlay) with NO decompiled
       methods, only a Spawn factory: VOLCANO_RING (71, x1), LAVA_BRIDGE
       (73, x2), FLOAT_ON_LAVA_PLATFORM (74, x1), LAVA_SEESAW (77, x1),
       LAVA_PLANK (82, x3). Blocked on the class bodies not being decompiled.

     - ov022-RESIDENT, fully decompiled, blocked on an ov022 CODE SLICE: only
       ROLLING_LOG_LLL (70, x1) is self-contained in overlay terms (its
       InitResources/Behavior/Cleanup/D0 reach no other overlay's data). It is
       still blocked: its Behavior dispatches a state closure through a
       pointer-to-member table at +0x108 that InitResources seats with
       func_ov022_02112790(this, &data_ov022_02114690), and that table lives in
       ov022 BSS (0x02114690 is past the image end 0x02114500) built by ov022's
       own static initialisers. None of ov022's 46 decompiled func_ov022_* nor
       its __sinit_ov022_* are in any build slice -- the overlay is mounted
       --whole as DATA only, so the pmf words are DS code pointers no slice
       patches to host addresses, and the first Behavior tick would call
       through them. Hosting it is an ov022 code-slice + sinit + pmf-repoint
       gate, deferred.

     - ov022-RESIDENT but routing collision through ov064's CLPS block, so
       BLOCKED on an ov064 mount: ROTATING_PLATFORM_LLL (80, x1) and
       FLOATING_FLOOR_LLL_BIG (75, x1) both pass data_ov064_0211bb0c/ba6c as the
       CLPS_Block to MovingMeshCollider::SetFile, and both spell CleanupResources
       through the per-TU G0/G1 placeholders (the IceSheet wall, level 10's
       note). FLOATING_FLOOR_LLL_SMALL (76, x3) reaches data_ov036/ov002 and
       FALL_BLOCK_LLL (83, x9) calls func_ov080_* into ov080. Each needs a fresh
       overlay mount plus per-TU G0/G1, deferred.

     - HOME IN AN UNMOUNTED OBJECT OVERLAY, needing a fresh per-symbol mount
       (the gate-64 shape -- a new overlay mount, its sinits, the vtable fill and
       a row): ov064 holds METAL_NET_LIFT (69, x2), TILTING_PLATFORM_LLL
       (72, x4), BOWSER_PUZZLE_PIECE (78, x14), BOWSER_PUZZLE_MANAGER (79, x1),
       ROTATING_FIREBAR (81, x1), LAVA_BUBBLE (214, x7), BULLY (215, x5),
       BIG_BULLY (216, x2); ov071 holds MR_I (262, x2); ov080 holds CRAZED_CRATE
       (193, x1); ov095 holds FLAMETHROWER (318, x5). Several also reach ov002
       and carry G0/G1, so the mount is only the first part.

     - ARM9/ov000-adjacent, partially decompiled: POPPING_LAVA_BUBBLES (196, x1)
       -- only Init/Behavior/D0 are decompiled and its spawnFunc is in ov000, a
       base overlay; deferred.

   All 22 are turned away by the pre-spawn gate by name and named in the census,
   so the boot is honest about what did not spawn. None is a clean single-class
   host: the tractable-looking one (RollingLogLll) needs an ov022 code slice the
   port does not yet stand up, and the rest need overlay mounts or the per-TU
   G0/G1 fix. Deferred as multi-part gates; the level boots and is walkable. */
void port_ov022_patch(void);
void *port_ov022_at(unsigned ds);
extern unsigned char port_ov022_image[];
extern const unsigned port_ov022_ds_base, port_ov022_ds_end;

/* ov023 = level 15, Lethal Lava Land's fire_mt sublevel, course 6.
   data_020758c8[15] = 23, LVL_Overlay data_02092208[15] = 0x02111b88, OV0
   handles at +8 (bmd 0x0748/kcl 0x0745/icg 0x0749/icl 0x074a) resolve directly
   through handles.tsv to fire_mt's all.bmd/kcl/icg/icl (read the handles from
   extracted/overlays/overlay_0023.bin, NOT the stale dsd export whose halfwords
   drift by 232). SUBLEVEL_LEVEL_TABLE[15] = 6. Mounted --whole; own_sinits 0.
   Its six-class skipped cast (Squasher, PoppingLavaBubbles, Flamethrower,
   LavaBubble, Bully, PathLift) is documented in the port-w9-lvl15 commit; all
   blocked on undecompiled bodies or an unmounted ov064, none a free share. */
void port_ov023_patch(void);
void *port_ov023_at(unsigned ds);
extern unsigned char port_ov023_image[];
extern const unsigned port_ov023_ds_base, port_ov023_ds_end;

/* ov021 = level 13, Hazy Maze Cave (data/stage/cave), course 5.
   data_020758c8[13] = 21, LVL_Overlay data_02092208[13] = 0x021138c4, OV0
   handles at +8 (bmd 0x0709/kcl 0x0706/icg 0x070a/icl 0x070b) resolve directly
   through handles.tsv to cave's cave_all.bmd/kcl/icg/icl (61224-byte KCL, the
   largest mounted); SUBLEVEL_LEVEL_TABLE[13] = 5, subCount 8. Mounted --whole;
   own_sinits 0. The PAINTING (307) is gated on this level too, the levels-4/5
   PMF host-ABI skip; its other skips need ov065/ov071 mounts (see the
   port-w9-lvl13 commit).

   GATES 172-175 HOSTED FOUR of that commit's skipped classes: CRATE (194,
   ov098) x1, UP_DOWN_LIFT_HMC (33, ov095) x4, MONTY_MOLE (310, ov080) x3 and
   RED_FLAME (316, ov002, sharing BLUE_FLAME 317's fresh fill) x17 -- census
   158 spawned / 30 classes, 29 skipped / 11 classes, from 133/26 and 54/15.
   The rest of the skip list is mount-gated (ov065/ov071, the ov060 window)
   or individually blocked (PAINTING, PATH_LIFT, FLAMETHROWER). Corrections
   to that commit's reading, re-derived while hosting:
     - FLAMETHROWER (318, ov095) was misfiled as a fill+row+closure candidate;
       its Behavior (0x021368f0, 0x470 bytes) is NOT decompiled, so it is
       blocked on the class body like level 14's lava cast, not on a fill.
     - UP_DOWN_LIFT_HMC (33) is real but not free: _ZTV13UpDownLiftBbh
       (0x02137628, 32-slot Platform shape) has NO host fill yet -- SEESAW_BOB's
       gate-83 fill is the DIFFERENT _ZTV9SeesawBob -- and UpDownLiftBbh's
       Behavior dispatches data_ov095_02137910 through an incomplete-class PMF,
       so it needs a host-copy Behavior + seat (the WHOMP shape) on top of the
       fill. All six class methods are matched src.
     - MONTY_MOLE (310, ov080) closure is fully matched (16-slot vtable at
       0x021280b0) but its Behavior PMF-dispatches data_ov080_02128438, seated
       by __sinit_ov080_021278c0 from DS-address statics: a Crate-shaped
       source-side seat gate. */
void port_ov021_patch(void);
void *port_ov021_at(unsigned ds);
extern unsigned char port_ov021_image[];
extern const unsigned port_ov021_ds_base, port_ov021_ds_end;
}

/* LVL_Overlay, the fields the boot uses. */
struct PortLvlOverlay {
    unsigned char *clps;         /* 0x00 */
    unsigned char *objTable;     /* 0x04 */
    unsigned short bmdFileId;    /* 0x08 */
    unsigned short kclFileId;    /* 0x0a */
    unsigned short icgFileId;    /* 0x0c */
    unsigned short iclFileId;    /* 0x0e */
    unsigned char *subTables;    /* 0x10, stride 0xc */
    unsigned char subCount;      /* 0x14 */
    unsigned char flags;         /* 0x15 */
    unsigned char pad16[2];
    unsigned int unk18;          /* 0x18 */
};

/* ---- THE RECIPE: MOUNTING LEVEL N+1 ---------------------------------------
   Adding a level is mechanical and this is the whole of it. Nothing past the
   mount is per-level work: the object tables are the level's own data driving
   matched src, and a class that this build does not register is skipped by name
   at the pre-spawn gate (hal/actor_registry.cpp) rather than faulting.

     1. Find the overlay and its LVL_Overlay address FROM THE ROM, not the
        numbering. data_020758c8[level] is the overlay id (it is level+8), and
        data_02092208[level] is the LVL_Overlay DS address. port_level_ds_overlay
        (hal/level_change.cpp) reads the second, and port_level_mounts_install
        below asserts every row against it, so a wrong address says so loudly.
        The arm9 SUBLEVEL_LEVEL_TABLE (0x02075298) gives the course number,
        which is the level's shape.

     2. Add the overlay name to PORT_LEVEL_OVERLAYS in port/CMakeLists.txt. That
        mounts it --whole and generates port_ovNNN_{patch,at,image,ds_base,
        ds_end}. Slice-manifest edits are read at CONFIGURE time, so rerun cmake.

     3. Declare those five symbols (the extern block above) and add one row here:
        {level, name, "ovNNN", <LVL_Overlay DS addr>, port_ovNNN_patch,
         port_ovNNN_at, &port_ovNNN_ds_base, &port_ovNNN_ds_end, 0}.
        own_sinits stays 0 unless the overlay ALSO gets a per-symbol mount and
        hosted static initialisers (gate 17, hal/ov009_boot.cpp) -- only the
        castle grounds has that so far.

     4. Add the mount thunk: a port_mount_row_N below and its slot in
        port_level_mount_fns[]. PORT_LEVEL_COUNT sizes the caches off the table,
        so the thunk count has to match the row count.

   That is it. The level is then reachable by SM64DS_LEVEL=<id> at boot and by
   the debug level select (SM64DS_LEVEL_SELECT=<id>, hal/level_change.cpp's
   port_title_select over the ROM's own row table). Registering the level's own
   actor classes is a separate, optional gate: without it the level boots and is
   walkable with those classes skipped, which the census names. */
struct PortLevelDesc {
    int id;
    const char *name;
    const char *overlay;
    unsigned lvl_overlay;
    void (*patch)(void);
    void *(*at)(unsigned);
    const unsigned *ds_base;
    const unsigned *ds_end;
    int own_sinits;
};

static const PortLevelDesc port_level_table[] = {
    {1, "castle grounds (main_castle)", "ov009", 0x02112bdc,
     port_ov009_patch, port_ov009_at,
     &port_ov009_ds_base, &port_ov009_ds_end, 1},
    {6, "Bob-omb Battlefield (bombhei_map)", "ov014", 0x02113434,
     port_ov014_patch, port_ov014_at,
     &port_ov014_ds_base, &port_ov014_ds_end, 0},
    {2, "castle interior floor (course 29)", "ov010", 0x0211229c,
     port_ov010_patch, port_ov010_at,
     &port_ov010_ds_base, &port_ov010_ds_end, 0},
    {7, "Whomp's Fortress (course 1)", "ov015", 0x02113518,
     port_ov015_patch, port_ov015_at,
     &port_ov015_ds_base, &port_ov015_ds_end, 0},
    {3, "castle garden (main_garden, course 29)", "ov011", 0x021113ac,
     port_ov011_patch, port_ov011_at,
     &port_ov011_ds_base, &port_ov011_ds_end, 0},
    {9, "sunken ship (kaizoku_ship, course 2)", "ov017", 0x02111a78,
     port_ov017_patch, port_ov017_at,
     &port_ov017_ds_base, &port_ov017_ds_end, 0},
    {8, "Jolly Roger Bay (kaizoku_irie, course 2)", "ov016", 0x02113b90,
     port_ov016_patch, port_ov016_at,
     &port_ov016_ds_base, &port_ov016_ds_end, 0},
    {4, "castle basement (castle_b1, course 29)", "ov012", 0x02111c54,
     port_ov012_patch, port_ov012_at,
     &port_ov012_ds_base, &port_ov012_ds_end, 0},
    {5, "castle second floor (castle_2f, course 29)", "ov013", 0x02111844,
     port_ov013_patch, port_ov013_at,
     &port_ov013_ds_base, &port_ov013_ds_end, 0},
    {10, "Cool Cool Mountain (snow_mt, course 3)", "ov018", 0x02112f2c,
     port_ov018_patch, port_ov018_at,
     &port_ov018_ds_base, &port_ov018_ds_end, 0},
    {11, "Cool Cool Mountain slide (snow_slider, course 3)", "ov019", 0x02112c68,
     port_ov019_patch, port_ov019_at,
     &port_ov019_ds_base, &port_ov019_ds_end, 0},
    {12, "Big Boo's Haunt (teresa_house, course 4)", "ov020", 0x021138fc,
     port_ov020_patch, port_ov020_at,
     &port_ov020_ds_base, &port_ov020_ds_end, 0},
    {14, "Lethal Lava Land (fire_land, course 6)", "ov022", 0x02113228,
     port_ov022_patch, port_ov022_at,
     &port_ov022_ds_base, &port_ov022_ds_end, 0},
    {15, "Lethal Lava Land (fire_mt, course 6)", "ov023", 0x02111b88,
     port_ov023_patch, port_ov023_at,
     &port_ov023_ds_base, &port_ov023_ds_end, 0},
    {13, "Hazy Maze Cave (cave, course 5)", "ov021", 0x021138c4,
     port_ov021_patch, port_ov021_at,
     &port_ov021_ds_base, &port_ov021_ds_end, 0},
};

enum { PORT_LEVEL_COUNT = sizeof port_level_table / sizeof port_level_table[0] };

/* THE BOOT TARGET, and why it is not the env any more.
   The port booted whatever SM64DS_LEVEL named and cached it once, which was
   right while a run only ever entered one level. The moment the handoff
   (hal/level_change.cpp) started warping BETWEEN levels it was wrong in the
   worst way: the handoff latched the new level into data_0209f2f8 and called
   port_stage_a_boot, but the mount below still resolved to the env-cached desc,
   so a warp to Bob-omb Battlefield re-booted the castle grounds -- the census
   came back the castle's, doubled, and the [lvl] line said "level 1 up" after a
   select of level 6. Stage::InitResources has no such bug: it mounts
   data_02092208[data_0209f2f8], the LVL_Overlay for the CURRENT level.

   So the boot has an explicit target: the level id whoever is about to boot
   wants. The handoff sets it to the level it latched (port_level_set_target,
   called by hal/level_change.cpp), and the direct boot seeds it from
   SM64DS_LEVEL through port_level_boot_target before the first mount. -1 means
   "not set", the direct-boot case, and falls back to the env. This is a
   separate word from data_0209f2f8 on purpose: data_0209f2f8 is bss and reads 0
   (a valid level id) before any boot, so it cannot double as the sentinel. */
static int g_boot_target = -1;

static int port_level_env_want(void)
{
    static int want = -2;
    if (want != -2)
        return want;
    const char *e = std::getenv("SM64DS_LEVEL");
    want = e ? std::atoi(e) : 1;
    return want;
}

/* The desc for a given level id, or null if the port cannot mount it. Never
   aborts: a level with no row is a real answer the handoff declines with a
   message, and the direct-boot resolver below turns null into the abort. */
static const PortLevelDesc *port_level_desc_for(int id)
{
    for (int i = 0; i < PORT_LEVEL_COUNT; ++i)
        if (port_level_table[i].id == id)
            return &port_level_table[i];
    return 0;
}

/* The level this boot is for: the explicit target when one is set, the env
   otherwise. An id the port cannot mount is named along with the ones it can
   rather than silently falling back -- a quiet fallback to the castle grounds
   would read as "Bob-omb Battlefield boots" when it did not. */
static const PortLevelDesc *port_level_desc(void)
{
    int want = (g_boot_target >= 0) ? g_boot_target : port_level_env_want();
    const PortLevelDesc *d = port_level_desc_for(want);
    if (d)
        return d;
    std::fprintf(stderr, "FATAL: level %d is not a hosted level. Hosted:", want);
    for (int i = 0; i < PORT_LEVEL_COUNT; ++i)
        std::fprintf(stderr, " %d (%s)", port_level_table[i].id,
                     port_level_table[i].name);
    std::fprintf(stderr, "\n");
    std::abort();
    return 0;
}

/* The handoff's own hook: point the boot at the level it just latched. Called
   by hal/level_change.cpp after port_level_latch, before port_stage_a_boot, so
   the mount and every desc read below resolve to the WARPED-TO level rather
   than the env-cached one. */
extern "C" void port_level_set_target(int level)
{
    g_boot_target = level;
}

/* Seed the target from the env for the DIRECT boot, before the first mount.
   The handoff does not call this: it sets the target itself. Only writes when
   nothing has set a target yet, so a warp's target is never clobbered. Returns
   the level it settled on. */
extern "C" int port_level_boot_target(void)
{
    if (g_boot_target < 0) {
        const PortLevelDesc *d = port_level_desc();   /* aborts on a bad env */
        g_boot_target = d->id;
        std::printf("[level] %d = %s, %s\n", d->id, d->name, d->overlay);
    }
    return g_boot_target;
}

extern "C" int port_level_id(void) { return port_level_desc()->id; }
extern "C" const char *port_level_name(void) { return port_level_desc()->name; }
extern "C" int port_level_count(void) { return PORT_LEVEL_COUNT; }

extern "C" int port_level_nth(int i, int *id, const char **name)
{
    if (i < 0 || i >= PORT_LEVEL_COUNT) return 0;
    if (id) *id = port_level_table[i].id;
    if (name) *name = port_level_table[i].name;
    return 1;
}

/* Whether the CURRENT level's overlay has its own hosted sinits (gate 17). */
extern "C" int port_level_has_own_sinits(void)
{ return port_level_desc()->own_sinits; }

/* hal/ov009_boot.cpp -- runs ov009's four static initialisers. Forward-declared
   here because port_stage_a_boot (below) calls it on every level-1 boot, before
   the block near the file's end that also declares it for a2_seat's use. */
extern "C" void port_ov009_sinits(void);

/* IDEMPOTENT PER LEVEL, and gate 31 is why. d->patch() rewrites the overlay
   image's own pointers in place, which is not something that can be done
   twice: a second pass would rebase already-rebased words. The cache was an
   optimisation while a run only ever mounted one level. With the handoff
   calling the mount again on every entry it is load-bearing, and it needs one
   slot PER LEVEL rather than one slot overall: a session that goes 1 -> 6 -> 1
   must get the first mount of each back rather than a re-patch of either. Any
   mount registered with port_level_mount_register owes the same guarantee. */
static void *port_level_mount_at(int idx)
{
    static void *mounted[PORT_LEVEL_COUNT];
    if (mounted[idx])
        return mounted[idx];
    const PortLevelDesc *d = &port_level_table[idx];
    d->patch();
    void *lvl = d->at(d->lvl_overlay);
    if (!lvl) {
        std::fprintf(stderr, "FATAL: %s mount: 0x%08x outside the overlay "
                     "[0x%08x, 0x%08x)\n", d->overlay, d->lvl_overlay,
                     *d->ds_base, *d->ds_end);
        std::abort();
    }
    mounted[idx] = lvl;
    return lvl;
}

extern "C" void *port_level_mount(void)
{
    return port_level_mount_at((int)(port_level_desc() - port_level_table));
}

/* The handoff registry in hal/level_change.cpp wants a nullary mount per level,
   so every table row gets a thunk. A third level is a third line here beside
   its row, which is the price of the registry not knowing about the table. */
static void *port_mount_row_0(void) { return port_level_mount_at(0); }
static void *port_mount_row_1(void) { return port_level_mount_at(1); }
static void *port_mount_row_2(void) { return port_level_mount_at(2); }
static void *port_mount_row_3(void) { return port_level_mount_at(3); }
static void *port_mount_row_4(void) { return port_level_mount_at(4); }
static void *port_mount_row_5(void) { return port_level_mount_at(5); }
static void *port_mount_row_6(void) { return port_level_mount_at(6); }
static void *port_mount_row_7(void) { return port_level_mount_at(7); }
static void *port_mount_row_8(void) { return port_level_mount_at(8); }
static void *port_mount_row_9(void) { return port_level_mount_at(9); }
static void *port_mount_row_10(void) { return port_level_mount_at(10); }
static void *port_mount_row_11(void) { return port_level_mount_at(11); }
/* Level 14's row is table index 12. Named by level rather than index and kept
   append-only so a sibling stream adding a level against the same base does not
   collide; a reviewer renumbers to port_mount_row_12 at merge. */
static void *port_mount_row_lvl14(void) { return port_level_mount_at(12); }
static void *port_mount_row_13(void) { return port_level_mount_at(13); }
static void *port_mount_row_14(void) { return port_level_mount_at(14); }
static void *(*const port_level_mount_fns[PORT_LEVEL_COUNT])(void) = {
    port_mount_row_0, port_mount_row_1, port_mount_row_2, port_mount_row_3,
    port_mount_row_4,
    port_mount_row_5,
    port_mount_row_6,
    port_mount_row_7,
    port_mount_row_8,
    port_mount_row_9,
    port_mount_row_10,
    port_mount_row_11,
    port_mount_row_lvl14,
    port_mount_row_13,
    port_mount_row_14,
};

// ---- the loader dispatch table ---------------------------------------------
//
// LoadObjects indexes data_ov002_0210cbb8 with `kind & 0x1f` and the ROM's
// table is FIFTEEN entries long: indices 15..31 read whatever data follows,
// which in ov002 is the actor-id table at 0x0210cbf4. Real level data never
// carries an index past 14, so the overrun is unreachable rather than a bug,
// but the host copy closes it anyway -- the tail is null and LoadObjects
// already skips null entries.
//
// The table is hand-built rather than ovdata-mounted for the obvious reason:
// its fifteen words are ov002 CODE addresses, meaningless on the host.
/* SM64DS_TRACE_LOADERS=1 names each loader as it runs, which is the only
   window into the boot: everything inside it is matched src.

   This hook also used to write the level collider's file<->world scale pair
   (0x40000 / 0x40 into MeshCollider+0x2c / +0x38) before anything raycast
   against it. The walks read those words as their unit conversion, and
   SetFile's 1.0 made every ray miss. They read the ROM's own >>6 now --
   see port/unmatched/MeshCollider_DetectClsn_Sphere.cpp, BASIS CONVENTION --
   so the pair is nobody's business and +0x2c is back to being what the ROM
   put there, the Y of the collider's up axis. */
/* The level's own entrance sub-table, kept as the boot walks past it, so the
   debug menu's warp list is the LEVEL'S entrances rather than a hand-written
   list of coordinates that goes stale the moment anyone changes levels. The
   record is LoadEntranceObjects' own `struct Entry`: raw id, s16 x/y/z, a
   Vector3_16 rotation, and a param whose low three bits are the area.
   Sixteen bytes, and the sub-table header carries the count at +1 and the
   array pointer at +4. */
static const unsigned char *g_entrance_entries;
static int g_entrance_count;

extern "C" int port_entrance_count(void) { return g_entrance_count; }

extern "C" int port_entrance_record(int i, int *x, int *y, int *z, int *yaw)
{
    const unsigned char *e;
    if (!g_entrance_entries || i < 0 || i >= g_entrance_count) return 0;
    e = g_entrance_entries + (size_t)i * 16;
    if (x) *x = *(const short *)(e + 2);
    if (y) *y = *(const short *)(e + 4);
    if (z) *z = *(const short *)(e + 6);
    if (yaw) *yaw = *(const short *)(e + 0xa);
    return 1;
}

extern "C" void port_loader_enter(int idx, const void *tbl)
{
    static int on = -1;
    if (idx == 1) {                  /* LOADER_ENTRANCE, declared below */
        g_entrance_count = ((const unsigned char *)tbl)[1];
        g_entrance_entries =
            *(const unsigned char *const *)((const char *)tbl + 4);
    }
    if (on < 0) on = std::getenv("SM64DS_TRACE_LOADERS") != 0;
    if (on)
        std::printf("  [load] %2d count %u entries %p\n", idx,
                    ((const unsigned char *)tbl)[1],
                    *(const void *const *)((const char *)tbl + 4));
}

extern "C" {
void _Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z19LoadPathNodeObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z15LoadPathObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z15LoadViewObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z17LoadSimpleObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z25LoadTeleportSourceObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z23LoadTeleportDestObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z14LoadFogObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z15LoadDoorObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z15LoadExitObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z22LoadMinimapTileObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z23LoadMinimapScaleObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z23LoadUnusedType13ObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);
void _Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij(void *, int, unsigned);

}  /* extern "C" */

static void port_load0(void *t, int a, unsigned b)
{ port_loader_enter(0, t); _Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load1(void *t, int a, unsigned b)
{ port_loader_enter(1, t); _Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load2(void *t, int a, unsigned b)
{ port_loader_enter(2, t); _Z19LoadPathNodeObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load3(void *t, int a, unsigned b)
{ port_loader_enter(3, t); _Z15LoadPathObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load4(void *t, int a, unsigned b)
{ port_loader_enter(4, t); _Z15LoadViewObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load5(void *t, int a, unsigned b)
{ port_loader_enter(5, t); _Z17LoadSimpleObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load6(void *t, int a, unsigned b)
{ port_loader_enter(6, t); _Z25LoadTeleportSourceObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load7(void *t, int a, unsigned b)
{ port_loader_enter(7, t); _Z23LoadTeleportDestObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load8(void *t, int a, unsigned b)
{ port_loader_enter(8, t); _Z14LoadFogObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load9(void *t, int a, unsigned b)
{ port_loader_enter(9, t); _Z15LoadDoorObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load10(void *t, int a, unsigned b)
{ port_loader_enter(10, t); _Z15LoadExitObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load11(void *t, int a, unsigned b)
{ port_loader_enter(11, t); _Z22LoadMinimapTileObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load12(void *t, int a, unsigned b)
{ port_loader_enter(12, t); _Z23LoadMinimapScaleObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load13(void *t, int a, unsigned b)
{ port_loader_enter(13, t); _Z23LoadUnusedType13ObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }
static void port_load14(void *t, int a, unsigned b)
{ port_loader_enter(14, t); _Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b); }

extern "C" {
typedef void (*PortObjLoader)(void *, int, unsigned);
PortObjLoader data_ov002_0210cbb8[32] = {
    port_load0,        /*  0 */
    port_load1,        /*  1 */
    port_load2,        /*  2 */
    port_load3,            /*  3 */
    port_load4,            /*  4 */
    port_load5,          /*  5 */
    port_load6,  /*  6 */
    port_load7,    /*  7 */
    port_load8,             /*  8 */
    port_load9,            /*  9 */
    port_load10,            /* 10 */
    port_load11,     /* 11 */
    port_load12,    /* 12 */
    port_load13,    /* 13 */
    port_load14,      /* 14 */
    /* 15..31: the ROM's overrun, made explicit */
};
}  /* extern "C" */

/* Two loaders define plain C++ names (their TUs never wrapped the definition
   in extern "C"); the table above wants the Itanium name every other caller
   uses. */
#pragma comment(linker, "/alternatename:__Z15LoadDoorObjectsRN11LVL_Overlay11ObjSubTableEij=?LoadDoorObjects@@YAXAAUObjSubTable@LVL_Overlay@@HI@Z")
#pragma comment(linker, "/alternatename:__Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij=?LoadStarCameraObjects@@YAXAAUObjSubTable@LVL_Overlay@@HI@Z")

// ---- LoadFile(handle) ------------------------------------------------------
//
// The ROM's LoadFile is func_0201818c(handle, 1), the archive loader's
// refcounted entry point. The port's file seam is one level up, at
// SharedFilePtr (hal/fs.cpp), so this is the same contract expressed there:
// one persistent SharedFilePtr per handle, loaded once, pointer returned.
//
// It deliberately does NOT run MeshCollider::UpdateFileOffsets, which is what
// makes it different from MeshCollider::LoadFile. The caller here is
// Stage::LoadClsnAndObjects, and its very next line is the fixup. Doing it in
// both places rebases the four header words twice, and the fixup is
// `ptr = &file + (int)ptr` -- not idempotent, so the second pass sends the
// positions array off into whatever follows the file.
extern "C" {
struct PortSharedFilePtr {
    unsigned short fileID;
    unsigned char numRefs;
    unsigned char pad;
    char *filePtr;
};
struct PortSharedFilePtr *_ZN13SharedFilePtr9ConstructEj(struct PortSharedFilePtr *,
                                                         unsigned);
void _ZN13SharedFilePtr8LoadFileEv(struct PortSharedFilePtr *);

/* The handle table is per-LEVEL, not per-run: the KCL and the object files a
   level's boot loads through it are that level's. Gate 31 releases them on a
   level change (port_level_reset_host below), which is why the storage is
   file-scope now rather than function-static. */
enum { PORT_LOADFILE_SLOTS = 16 };
static PortSharedFilePtr g_loadfile_slot[PORT_LOADFILE_SLOTS];
static int g_loadfile_used;

/* PORT_HOST_ABI: src is func_0201818c(handle,1), the DS card archive loader;
   the port's file seam is one level up at SharedFilePtr, so this expresses the
   same contract there rather than driving card hardware. */
void *LoadFile(int handle)
{
    enum { SLOTS = PORT_LOADFILE_SLOTS };
    PortSharedFilePtr *const slot = g_loadfile_slot;
    int &used = g_loadfile_used;
    for (int i = 0; i < used; ++i)
        if (slot[i].fileID && slot[i].filePtr &&
            (int)slot[i].fileID == handle)
            return slot[i].filePtr;
    if (used >= SLOTS) {
        std::fprintf(stderr, "FATAL: LoadFile: out of host file slots\n");
        std::abort();
    }
    PortSharedFilePtr *s = &slot[used];
    _ZN13SharedFilePtr9ConstructEj(s, (unsigned)handle);
    _ZN13SharedFilePtr8LoadFileEv(s);
    if (!s->filePtr) {
        std::fprintf(stderr, "FATAL: LoadFile(%d): no bytes\n", handle);
        std::abort();
    }
    /* Construct rewrites fileID from the ov0 handle to the FAT file id, so
       the cache key above matches only when both agree; keep the handle. */
    ++used;
    s->fileID = (unsigned short)handle;
    return s->filePtr;
}

/* Method faces: the three MeshCollider helpers the boot calls by their
   Itanium names while their definitions are real MSVC members. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12MeshCollider17UpdateFileOffsetsER8KCL_File(void *file)
{ MeshCollider::UpdateFileOffsets(*(KCL_File *)file); }
#else
void _ZN12MeshCollider17UpdateFileOffsetsER8KCL_File(void *file);  /* Linux: real symbol from src/_ZN12MeshCollider17UpdateFileOffsetsER8KCL_File */
#endif /* _WIN32 */
int _ZNK12MeshCollider16GetOctreeOriginYEv(const void *self)
{ return ((const MeshCollider *)self)->MeshCollider::GetOctreeOriginY(); }
int _ZNK12MeshCollider13GetUnkOctreeYEv(const void *self)
{ return ((const MeshCollider *)self)->MeshCollider::GetUnkOctreeY(); }

// ---- the globals the sub-loaders store through -----------------------------
//
// Every "Load<Kind>Objects" that is not a spawner is a two-word veneer:
// store the table pointer in one global, the count in another. Storage only;
// the consumers (minimap, fog, teleport) are Stage B and C.
short data_ov002_0211118c;   /* the per-level spawn counter, ov002 bss */
int data_02092138;           /* world Y min (func_0202a850) */
int data_020a0d8c[4];        /* path count */
int data_0209f31c[4];        unsigned char data_0209f258[4];   /* fog */
int data_0209f328[4];        unsigned char data_0209f214[4];   /* entrances */
int data_0209f334[4];        unsigned char data_0209f2e8[4];   /* minimap tiles */
int data_0209f348[4];        unsigned char data_0209f25c[4];   /* minimap scale */
unsigned char data_0209f2d0[4];                                /* teleport dest
                                                                  count; the
                                                                  pointer
                                                                  data_0209f330
                                                                  is auto_bss */
int data_0209f338[4];        /* the unused type-13 word */
/* the CURRENT LVL_Overlay: storage is hal/actor_vtables.cpp, parked on a
   zeroed block for the no-level case; the boot points it at the real one */
extern unsigned char *data_0209f340;
}  /* extern "C" */

// ---- the save block, contiguous --------------------------------------------
//
// LoadEntranceObjects reads data_0209caa0[0x41]. The symbol dsd named
// data_0209caa0 is 0x14 bytes; byte 0x41 lands inside data_0209cad2, two
// symbols further on. That is the ordinary decomp shape -- one save-file
// struct the delink split five ways at the boundaries code happened to
// reference -- and separate host arrays make the read land on whatever the
// linker put next.
//
// Grouped sections put them back in ROM order, the mechanism romdata.py uses
// for the camera-mode table. Every delta here equals the symbol's own size
// and all four are even, so align(2) packs with no interior padding.
#include "port_msvc_compat.h"
#define SAVEBLK(sec, name, size) \
    extern "C" PORT_GROUPED_DECL_A(sec, ".savblk", 2) unsigned char name[size] = {0}

SAVEBLK(".savblk$0000", data_0209caa0, 0x14);
SAVEBLK(".savblk$0001", data_0209cab4, 0x1e);
SAVEBLK(".savblk$0002", data_0209cad2, 0x12);
SAVEBLK(".savblk$0003", data_0209cae4, 0x10);

#undef SAVEBLK

// ---- Stage A ---------------------------------------------------------------
//
// A1 runs the real boot with every spawner switched off, so what it proves is
// exactly the geometry: the KCL comes from the level's own kclFileId, the
// CLPS block is the level's own (not a zeroed stand-in), the path table and
// its 220 nodes are seated, and the world-Y bounds come out of the octree.
//
// Suppression is a write into the HOST copy of the overlay -- counts set to
// zero -- rather than a branch in the loader, because the loader is matched
// src and stays untouched. subCount = 0 removes the whole sub-table, which is
// where the 89 Standard/Simple objects live.
static void port_stage_suppress(PortLvlOverlay *o, unsigned kind_mask,
                                int drop_subtables)
{
    if (drop_subtables)
        o->subCount = 0;
    unsigned n = *(unsigned short *)o->objTable;
    unsigned char *e = *(unsigned char **)(o->objTable + 4);
    for (unsigned i = 0; i < n; ++i, e += 8)
        if (kind_mask & (1u << (e[0] & 0x1f)))
            e[1] = 0;
}

extern "C" {
void _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider(void *ovl,
                                                                  unsigned p,
                                                                  void *mc);
extern signed char data_0209f2f8;    /* current level */
extern int data_0209f264[];          /* current entrance */
extern int data_0209f220[];          /* current star filter */
extern unsigned char *data_0209f344; /* VS star-order pointer (host: bob_enemy_bridges.cpp) */
extern int data_0209212c;            /* world Y max */
extern int data_020a0d84[];          /* path table base (auto_bss) */
extern int data_020a0d88[];          /* path node base (auto_bss) */

/* Loader indices, for the suppression masks. */
enum {
    LOADER_ENTRANCE = 1,
    LOADER_DOOR = 9,
    LOADER_EXIT = 10,
};

static void *g_stage_mc;

extern "C" void port_scene_canary(const char *where);

/* `spawn` selects the stage: 0 = A1, the same boot with every spawner
   switched off (the geometry regression); 1 = the level's own object load. */
extern "C" void port_particle_boot(void);   /* hal/particle_bridges.cpp */
extern "C" void port_boot_course_sound(int level);   /* hal/star_flow.cpp:
                                            the InitResources sound-row block */

/* THE MESSAGE BOX DATA LAYER IS HOSTED now (hal/message_boot.cpp). func_0201f32c
   opens a message and its first line is

       if (*(u16 *)((char *)data_0209d70c + 8) <= (u16)arg0) return;

   which is the ROM's own bounds check against the message count. data_0209d70c
   now points at the real loaded bank header, so the count reads 711 and the
   matched body runs. The seat is done through port_message_archive_seat, which
   loads the bank once and is idempotent, so the early boot call and the a2 seat
   both reach it and only the first does work. */
extern "C" int data_0209d70c[];   /* hal/auto_bss.cpp */
extern "C" void port_message_archive_seat(void);

void *port_stage_a_boot(void *mc, int spawn)
{
    g_stage_mc = mc;
    /* Defensive: clear the quarantine freeze set on the LOAD side too. The
       teardown path (level_change.cpp) already resets it, but a future exit
       path that bypasses teardown would otherwise carry stale frozen actor
       POINTERS into this level, and a new actor reusing one of those heap
       slots would be wrongly skipped. Resetting here makes "no stale set
       survives into a level" true regardless of how the last one ended.
       Idempotent, no-op on today's paths. */
    {
        extern void port_quarantine_reset(void);
        port_quarantine_reset();
    }
    /* Settle which level this boot is for BEFORE the mount reads it. The direct
       boot seeds the target from SM64DS_LEVEL here; the handoff has already set
       it to the latched level (port_level_set_target), so this is a no-op on
       the warp path. Either way the mount below resolves to the right overlay
       -- which is the whole fix for the warp booting the wrong level. */
    port_level_boot_target();
    /* fx wrote this against the ov009-only mount; the lvl stream made the
       mount parameterised, and the bank load wants to happen before any level
       logic can open a text box, so it rides the new call */
    port_message_archive_seat();
    PortLvlOverlay *o = (PortLvlOverlay *)port_level_mount();

    /* STAGE B: THE TABLES ARE BACK ON. Stage A1 zeroed the Entrance, Door and
       Exit counts in the host copy of the overlay and dropped the sub-table
       -- the 89 Standard/Simple objects -- so that what it proved was exactly
       the geometry. All of it loads now. Doors and Exits spawn actors like
       everything else and meet the same registry gate; there is no reason for
       the loader to be the thing that stops them. */
    if (!spawn)
        port_stage_suppress(o, (1u << LOADER_ENTRANCE) | (1u << LOADER_DOOR) |
                                   (1u << LOADER_EXIT), 1);

    data_0209f2f8 = (signed char)port_level_id();
    /* TEMPORARY probe (hal/cannon_probe.cpp): SM64DS_CANNONS_OPEN=1 sets the
       current level's cannon-unlock save bit through the matched setter, the
       stand-in for Bob-omb Buddy's post-dialogue OpenCannonInCurLevel() until
       the real talk flow drives it. Must run after data_0209f2f8 is set. */
    {
        extern void port_cannons_open_probe(void);
        if (spawn)
            port_cannons_open_probe();
    }
    /* Entrance 0 is the level's first entrance record, which is where the
       game puts you arriving from outside: the castle gate on the grounds,
       the warp-pipe pad on Bob-omb Battlefield. SM64DS_ENTRANCE picks another
       one; port_entrance_count() says how many the level has. */
    {
        const char *en = std::getenv("SM64DS_ENTRANCE");
        data_0209f264[0] = en ? std::atoi(en) : 0;
    }
    /* Star filter: the sub-table's group byte (kind >> 5) loads when it is 0
       or equal to this. ADVENTURE is 1, which is grp0 + grp1; SM64DS_STAR_FILTER
       is the knob that reads the other halves back (0 = grp0 alone). */
    {
        const char *sf = std::getenv("SM64DS_STAR_FILTER");
        data_0209f220[0] = sf ? std::atoi(sf) : 1;
    }
    /* data_0209f344: the VS star-order pointer Stage::InitResources:427 seats to
       &VS_STAR_SPAWN_ORDERS[func_0203dad4() % 6]. The port hand-rolls the boot
       and skips InitResources, so without this the pointer stays NULL and the
       star-progress index data_0209f344[data_0209f208] faults (StarMarker::
       Behavior +0x2e, SIG-2, WF star mission 2; plus the ov002/ov084 checks).
       func_0203dad4() is just `return data_020a1040` (src/func_0203dad4.c), and
       that gate-36 TU is not in the walk_window link set, so the row index is
       computed from data_020a1040 directly -- the identical value the ROM's
       expression evaluates to. data_020a1040 is the VS local-comms record
       (hosted in hal/camera_bridges.cpp), zero in single player, so the row is
       0; column 0 of every row is 0 and data_0209f208 is 0 in single player, so
       the "is this my star" compare is inert for every real adventure marker --
       the same result the DS produces. Host + table are in
       hal/bob_enemy_bridges.cpp. */
    {
        extern unsigned char VS_STAR_SPAWN_ORDERS[6][0xC];
        extern int data_020a1040;
        data_0209f344 = &VS_STAR_SPAWN_ORDERS[(unsigned)data_020a1040 % 6][0];
    }
    data_0209f340 = (unsigned char *)o;

    /* THE LEVEL OVERLAY'S OWN STATIC INITIALISERS, on EVERY boot, where the DS
       runs them: after the overlay is mounted and the level is current, before
       LoadClsnAndObjects spawns anything. Every SharedFilePtr the level's own
       actors load through is Construct'd there (ov009: the Flag's model/anim
       pair, data_ov009_02113eb8/eb0), so an actor that spawns before the sinit
       has run reads a zeroed SharedFilePtr and Model::LoadFile faults on fileID
       0. That is exactly what an out-of-bounds death did: HitDeathPlane sends
       the player back to the castle grounds (level 1), the boot mounts ov009 and
       LoadClsnAndObjects spawns a Flag -- but the ov009 sinits had only ever run
       at process start (port_stage_a2_seat), so a session that BOOTED into a
       different level (e.g. Whomp's Fortress) and then fell out never ran them.
       Riding the boot puts them on every entry, the death re-entry included, the
       way the ROM does; port_ov009_sinits is idempotent, so the fresh-boot
       a2_seat call and this one do not double-construct. Guarded to the booted
       level's own overlay, so the castle grounds' file pointers are not seated
       under a level that never reads them. */
    if (spawn && port_level_has_own_sinits())
        port_ov009_sinits();

    /* THE SOUND ROW, where Stage::InitResources seats it: after the overlay is
       up and the level is current, before LoadClsnAndObjects. This is the block
       InitResources runs through GetSoundGroupID / Sound::LoadGroupAndSetBank /
       Sound::LoadAndSetMusic_Layer1, hosted in hal/star_flow.cpp so the one
       func_0203d974==1 seam is compensated in one place. It used to be a
       separate gate-35 seat (port_course_seat) that ran once per process and
       so never re-seated across a warp; riding the boot puts it on EVERY entry,
       the warp included, the way the ROM does. Only on a real spawn boot: the
       A1 geometry regression has no course. */
    if (spawn)
        port_boot_course_sound((int)data_0209f2f8);

    /* ONE BIT, TWO JOBS, and they pull opposite ways on a port with no
       sound engine.
       LoadClsnAndObjects' last decision is the intro cutscene: mode 0 plus
       bit 7 of data_0209caa0[2] clear (= the intro has not played) runs
       StartIntroCutscene, which loads a sound group and, three calls down,
       reads the DS console-type word at 0x027ffc40. So the bit has to be
       SET across the boot.
       The same bit is the one Player::InitResources tests to decide whether
       to load the character's voice bank -- the identical unhosted sound
       path. So it has to be CLEAR when the Player initialises.
       Scoping it to the boot satisfied both while the Player was still the
       harness's (Stage A1).

       IT STAYS SET NOW, and that is the third job the same bit does: it is
       what the whole bottom screen renders through. HUD::Behavior and
       HUD::Render both open on

           if ((data_0209caa0[2] & 0x80) == 0) return 1;

       -- the adventure-mode branch draws the health meter, the coins, the
       stars, the timer and the camera buttons, and with the bit clear it
       returns before any of them. Clearing it after the boot left a hosted,
       ticking, correctly-constructed HUD that drew nothing at all.

       Leaving it set is the state the real game is in during gameplay: the
       intro HAS played by the time a level is being walked around in. The
       restore was only ever protecting Player::InitResources' voice-bank
       load, and the Player initialises INSIDE the boot -- while the bit is
       set either way -- so the restore was not protecting anything by the
       time the entrance started spawning him.

       SM64DS_INTRO_UNSEEN=1 puts the old behaviour back, which is also how to
       see the pre-intro cloud backdrop the bottom screen shows without it. */
    unsigned char intro_seen = (unsigned char)(data_0209caa0[8] & 0x80);
    data_0209caa0[8] |= 0x80;   /* word 2 bit 7: the intro has played */
    _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider(o, 0, mc);
    port_scene_canary("after LoadClsnAndObjects");
    if (!intro_seen && std::getenv("SM64DS_INTRO_UNSEEN"))
        data_0209caa0[8] &= ~0x80;

    /* RISK 1 IS CLOSED, and not by writing anything here. The real SetFile
       leaves the collider's file<->world vectors at 1.0, which on the ROM is
       right: its ITCM octree walk bakes the <<6 into its own vertex and origin
       loads. The port's transcription used to consume those two words as the
       conversion instead, so the boot had to overwrite them for the level --
       and could not for anything else, which is what hid the moat water from
       every ray in the game. The walks do the ROM's shift now
       (port/unmatched/MeshCollider_DetectClsn_Sphere.cpp, BASIS CONVENTION)
       and SetFile's own values stand. */

    /* THE PARTICLE SUBSYSTEM, at the point Stage::InitResources brings it up.
       Its own line on the ROM is the second-to-last thing InitResources does
       (0x0202d3dc, right after LoadSkybox):

           Particle::SysTracker::Initialise((char *)thiz + 0x50);

       which is here because the archive's textures are uploaded into VRAM
       banks the level has already claimed, so it cannot run before the loads
       above. Everything it needs is up by now. */
    port_particle_boot();
    return o;
}

// ---- Stage A2/B: the seam the registry needs --------------------------------
//
// The registry itself is hal/actor_registry.cpp: the class table, the
// pre-spawn gate at data_020a4b58, and the five processing-list callbacks.
// What is left here is the engine state the spawn spine reads before any of
// that matters -- the scene root, the player slots, the camera's boot inputs.
extern "C" {
int hal_camera_check_layout(void);
void port_ptr_tables_check(void);
void hal_fill_camera_vtable(void);
void hal_camera_slots_harness_owned(void);
void port_actor_registry_install(void);
void port_actor_lists_seat(void);
void hal_fill_moving_mesh_collider_vtable(void);
void port_ov009_sinits(void);
void port_actor_overlays_sinits(void);
extern void *data_0209f318;
extern int data_0209f5c0[];
extern int data_020a4b6c[];           /* the scene tree */
}

/* ---- the scene root -------------------------------------------------------
   func_02042ffc refuses to spawn anything under a null parent, and the
   ActorBase constructor links the new actor's SceneNode (+0x14) under the
   parent's. data_0209f5c0 is that parent, and on the ROM it is the STAGE
   ACTOR.

   IT IS THE STAGE ACTOR HERE TOO NOW (gate 24). What used to sit in this seat
   was a 0x50-byte ActorBase-shaped block -- a zeroed SceneNode with its actor
   back-pointer aimed at itself, which is what ActorBase::SceneNode::Reset plus
   the ctor's `+0x24 = this` produce, and enough for the spawn spine to link
   under. The real object is built by Stage::Stage (matched src, slice_gate24)
   and seated by port_stage_create in hal/stage_bridges.cpp; the tree head is
   no longer written by hand either, because the ActorBase constructor's own
   no-parent branch does it. */
extern "C" void *port_stage_create(void);
extern "C" void *port_stage_object(void);

/* ---- the Player vtable ----------------------------------------------------
   Spawning through func_02043098 ends in func_020433b8 -> the init Process,
   which dispatches BeforeInitResources / InitResources / AfterInitResources
   through the object's vptr. The Player's vptr is data_ov002_0210a83c, real
   ov002 data carrying ov002 CODE addresses -- fine to mount, impossible to
   call. The slots the port can service are overwritten with host thunks in
   place (the ovdata contract: callers patch code pointers at runtime); the
   rest trap by name rather than jumping into the overlay image. */
extern "C" {
/* defined by the ov002 whole-image mount (build/host-src/ov002_data.c); this is
   a DECLARATION. MSVC accepts the unsized tentative form; GCC needs the explicit
   `extern` on an incomplete array. */
extern unsigned char data_ov002_0210a83c[];
int _ZN6Player13InitResourcesEv(void *self);
int _ZN5Actor19BeforeInitResourcesEv(void *self);
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned r);
int _ZN5Actor14BeforeBehaviorEv(void *self);
int hal_player_behavior(void *self);
int func_02043288(void *self);         /* port/unmatched: the behaviour Process */
}

/* Method faces for the init chain. Everything the spawn spine touches is
   reached by its Itanium name from a .c TU, i.e. cdecl, while these three
   definitions are real MSVC __thiscall methods -- a linker alias would hand
   the body an ecx that never held `this`. */
#include "ActorBase.h"
#include "Actor.h"
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN9ActorBase19BeforeInitResourcesEv(void *self)
{ return ((ActorBase *)self)->ActorBase::BeforeInitResources() ? 1 : 0; }
#else
extern "C" int _ZN9ActorBase19BeforeInitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN9ActorBase19BeforeInitResourcesEv */
#endif /* _WIN32 */


static int __fastcall ps_init(void *s, void *)
{
    /* Bit 7 of the save block's word 2 says the intro has played, and
       Player::InitResources reads it to decide whether to load the
       character's voice bank -- unhosted sound, the same engine the intro
       cutscene reaches. The boot needs the bit set (see port_stage_a_boot);
       the Player needs it clear. Scoped to the one call that cares. */
    unsigned char saved = data_0209caa0[8];
    data_0209caa0[8] &= ~0x80;
    int r = _ZN6Player13InitResourcesEv(s);
    data_0209caa0[8] = saved;
    return r;
}
static int __fastcall ps_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ps_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ps_behavior(void *s, void *)
{ return hal_player_behavior(s); }
/* Slots 7 and 8, read out of ov002's own _ZTV6Player at 0x0210a83c with its
   relocation table applied: 0x02010fd4 = Actor::BeforeBehavior and 0x02010fc8
   = Actor::AfterBehavior. The second is a `ldr ip,[pc]; bx ip' veneer onto
   ActorBase::AfterBehavior (0x02043af8), so the thunk calls the target
   directly -- a host forward through the veneer's own C face would drop the
   argument the ARM tail call rides through in r0/r1. */
static int __fastcall ps_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ps_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
/* Slots 9/10/11. The render bucket (processing list 5) now dispatches every
   actor's Render through its vtable, and the Player is on that list like
   everything else -- so slot 9 can no longer be a trap. It is a no-op that
   reports success instead: Player::Render is the ROM's whole model/shadow/
   particle chain and only its body walk is hosted, so the harness still draws
   him itself (hal_render_player_world) right after the bucket. The two hooks
   around it are the game's own. */
extern "C" int _ZN5Actor12BeforeRenderEv(void *self);
static int __fastcall ps_render(void *, void *) { return 1; }
static int __fastcall ps_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ps_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }

/* ---- the DESTROY slots (gate 31) ------------------------------------------
   A level change destroys every actor the previous level spawned, the Player
   among them, and the destroy path is the ROM's own: the cleanup Process runs
   slots 4/3/5 and ActorBase::AfterCleanupResources then dispatches slot 16.
   Four of those five trapped, so the first level change aborted inside the
   Player rather than tearing him down.

   Slots 4 and 5 are the shared Actor/ActorBase bodies every other class uses
   (hal/actor_classes.cpp's ac_bclean / ac_aclean); 3 and 12 and 16 are the
   Player's own matched src. Slot 17 (D0, the deleting form) stays trapped on
   purpose: the ROM's teardown never dispatches it -- AfterCleanupResources
   calls slot 16 and does the Memory::Deallocate itself -- so a call landing
   there means something reached the Player through `delete`, which is a bug
   worth an abort rather than a double free. */
extern "C" {
/* Faces, in hal/method_faces.cpp: both definitions are real methods. */
int _ZN6Player16CleanupResourcesEv(void *self);
void _ZN6Player16OnPendingDestroyEv(void *self);
void *_ZN6PlayerD2Ev(void *self);
}
static int __fastcall ps_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ps_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ps_clean(void *s, void *)
{ return _ZN6Player16CleanupResourcesEv(s); }
static void __fastcall ps_pdes(void *s, void *)
{ _ZN6Player16OnPendingDestroyEv(s); }
/* D1 is the complete-object destructor the ROM's slot 16 holds. The Player
   has no virtual bases, so D1 and D2 are the same body and mwcc emits one;
   MSVC's D2 spelling is what src/_ZN6PlayerD2Ev.cpp defines. It must NOT
   deallocate -- the caller does that one line later. */
static int __fastcall ps_d1(void *s, void *)
{ return (int)(size_t)_ZN6PlayerD2Ev(s); }

static const char *const hal_player_slot_name[20] = {
    "InitResources", "BeforeInitResources", "AfterInitResources",
    "CleanupResources", "BeforeCleanupResources", "AfterCleanupResources",
    "Behavior", "BeforeBehavior", "AfterBehavior",
    "Render", "BeforeRender", "AfterRender",
    "OnPendingDestroy", "Virtual34", "Virtual38", "OnHeapCreated",
    "~Player (D1)", "~Player (D0)", "OnYoshiTryEat", "OnTurnIntoEgg"};
static int hal_player_trap_slot;
static int __fastcall ps_trap(void *, void *)
{
    std::fprintf(stderr, "FATAL: Player vtable slot %d (%s) is not hosted\n",
                 hal_player_trap_slot,
                 hal_player_slot_name[hal_player_trap_slot & 19]);
    std::abort();
    return 0;
}

extern "C" void hal_fill_player_vtable(void)
{
    void **vt = (void **)data_ov002_0210a83c;
    for (int i = 0; i < 20; ++i)
        vt[i] = (void *)ps_trap;
    vt[0] = (void *)ps_init;
    vt[1] = (void *)ps_binit;
    vt[2] = (void *)ps_ainit;
    vt[6] = (void *)ps_behavior;
    vt[7] = (void *)ps_bbeh;
    vt[8] = (void *)ps_abeh;
    vt[9] = (void *)ps_render;
    vt[10] = (void *)ps_bren;
    vt[11] = (void *)ps_aren;
    vt[3] = (void *)ps_clean;
    vt[4] = (void *)ps_bclean;
    vt[5] = (void *)ps_aclean;
    vt[12] = (void *)ps_pdes;
    vt[16] = (void *)ps_d1;
}

/* The per-frame tick the ROM's processing list runs on every actor:
   func_02043288 = ActorBase::Process over slots 7/6/8. Actor::BeforeBehavior
   is the half the harness never had -- it is what copies pos into PREV POS,
   and prev pos is the start of every line WithMeshClsn's continuous update
   casts. Driving Behavior bare left prev at the constructor's zero, so the
   first frame at the gate swept a segment from the world origin. */
extern "C" int hal_player_process(void *self)
{ return func_02043288(self); }

// ---- the direct actor-spawn hook -------------------------------------------
//
// Put one actor of a given class into the running level, on demand, without a
// level that happens to name it. Written for the two streams bringing actor
// classes up: a class can be exercised the moment its registry row lands,
// against whatever level is booted, instead of waiting for a level whose own
// object table calls for it.
//
// It is the LEVEL'S OWN SPAWN PATH and nothing else. LoadStandardObjects'
// whole body is a loop of
//
//     Actor::Spawn(actorIDTable[e->raw], e->param, &pos, &rot, area, seq)
//
// so this calls exactly that, with the same per-level sequence counter
// (data_ov002_0211118c, post-incremented like the loader does) and the same
// area the caller asks for. Everything downstream is untouched: the pre-spawn
// gate in hal/actor_registry.cpp still turns away an unregistered class and
// names it, the ActorBase constructor still reads its two list priorities out
// of the ROM SpawnInfo, and the actor lands on the same five processing lists
// as one the boot spawned.
//
// The id is the ACTOR id (the registry's column, what
// port_actor_class_name() answers to), not the raw object-table id that
// data_ov002_0210cbf4 translates. That is the id the class rows are written
// in, so it is the id to debug in.
extern "C" {
struct PortVec3 { int x, y, z; };
struct PortVec3_16 { short x, y, z; };
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned actorID,
                                                   unsigned param1,
                                                   const PortVec3 *pos,
                                                   const PortVec3_16 *rot,
                                                   int areaID,
                                                   int deathTableID);
extern void *data_0209f394[];          /* the local players, [0] is ours */
const char *port_actor_class_name(unsigned id);
}

/* ---- classes that belong to a LEVEL overlay -------------------------------
   Four of the registry's rows are ov009's, and their SharedFilePtrs are
   constructed by ov009's own static initialisers -- which run only when ov009
   is the mounted level overlay, the way the DS runs them. The level boot is
   safe either way, because no other level's object table names these ids. The
   debug hook is not: asking for one on another level reached an unconstructed
   SharedFilePtr and died as "fs fileID 0 not in catalog", four layers down
   from the thing that was actually wrong.

   So the hook names it instead. This is a list of ids, not a mechanism: the
   registry is where a class declares which overlay owns it, and when it does,
   this reads it from there. */
static const struct { unsigned id; int level; const char *what; }
port_level_owned_class[] = {
    {338, 1, "CASTLE_WATER (ov009)"},
    {339, 1, "METAL_NET (ov009)"},
    {342, 1, "FLAG (ov009)"},
    {343, 1, "BIRD (ov009)"},
};

/* Spawn `id` at an explicit world position (Fix12i, i.e. units << 12) facing
   `yaw`. Returns the ActorBase* the spine built, or 0 when the registry gate
   turned the class away -- which it reports itself, on stdout. */
extern "C" void *port_debug_spawn_at(unsigned id, unsigned param,
                                     int x, int y, int z, int yaw, int area)
{
    for (unsigned i = 0; i < sizeof port_level_owned_class /
                             sizeof port_level_owned_class[0]; ++i)
        if (port_level_owned_class[i].id == id &&
            port_level_owned_class[i].level != port_level_id()) {
            std::fprintf(stderr, "  [dbgspawn] actor %u is %s and level %d is "
                         "booted, so its overlay's static initialisers never "
                         "ran -- refusing rather than spawning it onto "
                         "unconstructed file pointers\n", id,
                         port_level_owned_class[i].what, port_level_id());
            return 0;
        }

    PortVec3 pos;
    PortVec3_16 rot;
    void *a;
    short seq;

    pos.x = x; pos.y = y; pos.z = z;
    rot.x = 0; rot.y = (short)yaw; rot.z = 0;
    seq = data_ov002_0211118c;
    data_ov002_0211118c = (short)(seq + 1);
    a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(id, param, &pos, &rot,
                                                     area, seq);
    std::printf("[dbgspawn] actor %u (%s) param 0x%x at (%d, %d, %d) yaw %04x "
                "area %d -> %p\n", id, port_actor_class_name(id), param,
                x >> 12, y >> 12, z >> 12, (unsigned short)yaw, area, a);
    return a;
}

/* The common case: at the local player, facing the way he faces, in his area.
   Player pos is +0x5c..0x64 and his facing yaw is +0x8e; area is the byte the
   ActorBase constructor kept at +0x10. Falls back to the world origin when no
   player exists yet, so an early call still reaches the registry gate rather
   than dereferencing null. */
extern "C" void *port_debug_spawn(unsigned id, unsigned param)
{
    const char *p = (const char *)data_0209f394[0];
    if (!p) {
        std::fprintf(stderr, "  [dbgspawn] no player yet, spawning actor %u "
                     "at the origin\n", id);
        return port_debug_spawn_at(id, param, 0, 0, 0, 0, 0);
    }
    return port_debug_spawn_at(id, param, *(const int *)(p + 0x5c),
                               *(const int *)(p + 0x60),
                               *(const int *)(p + 0x64),
                               *(const short *)(p + 0x8e),
                               *(const unsigned char *)(p + 0x10));
}

/* SM64DS_SPAWN_ACTOR=<id>[:<param>][,<id>[:<param>]...] fires the same hook
   once, right after the boot, so a class can be exercised from the command
   line with no rebuild. Ids are decimal or 0x-prefixed. */
extern "C" void port_debug_spawn_env(void)
{
    const char *s = std::getenv("SM64DS_SPAWN_ACTOR");
    if (!s)
        return;
    while (*s) {
        char *end;
        unsigned id = (unsigned)std::strtoul(s, &end, 0);
        unsigned param = 0;
        if (end == s) {
            std::fprintf(stderr, "  [dbgspawn] SM64DS_SPAWN_ACTOR: cannot read "
                         "an id at \"%s\"\n", s);
            return;
        }
        s = end;
        if (*s == ':')
            param = (unsigned)std::strtoul(s + 1, (char **)&s, 0);
        port_debug_spawn(id, param);
        if (*s == ',')
            ++s;
        else
            break;
    }
}

/* ---- the entrance-driven boot ---------------------------------------------
   Seats everything LoadEntranceObjects reads, then runs the same boot with
   the Entrance table left switched on. The Player and the Camera come out of
   the entrance record: position, rotation, area, entrance id and entrance
   type, all of it the level's own. */
extern "C" {
extern unsigned char data_0209f21c;    /* controller count */
extern unsigned char data_0209f250;    /* local player index */
extern int data_0209fc5c[];            /* per-player "this slot is live" */
extern unsigned char data_02092128[];  /* per-player character */
extern signed char data_02092120;      /* currently shown area, -1 = none */
extern int data_0209f32c[];            /* water level */
extern int data_0209fc48;              /* the running cutscene, 0 = none */
extern int data_0209f20c[], data_0209f294[], data_0209f2c4[], data_0209b454[];
extern int data_0209ee90[];            /* +0x44 is the projection's W scale */
extern int data_0209d70c[];            /* the message archive header pointer */
}

// ---- the message archive: the REAL bank, loaded ----------------------------
//
// Bob-omb Battlefield is the first level the port boots whose own logic opens
// a TEXT BOX. func_ov002_020c44c4 is the Player's one-shot level-intro check;
// its switch is on data_0209f2f8, the current level, and `case 7: r4val = 8` is
// Bob-omb Battlefield's tutorial message -- fired when
// SaveData::CountStarsCollectedInLevel comes back zero, which on a port with
// a zeroed save block it always does. That runs the message state machine in
// func_ov002_020c4188, whose case 2 calls func_0201f32c, whose first line is
//
//     if (*(u16 *)((char *)data_0209d70c + 8) <= (u16)arg0) return;
//
// data_0209d70c is the pointer to the loaded message archive's header. This
// seat now loads the REAL one: hal/message_boot.cpp's port_message_bank_load
// runs the matched loader+parser logic (LoadFile(0x0431) ->
// data/message/msg_data_eng.bin, decompressed by the fs layer; sections pinned)
// so data_0209d70c[+8] reads the real messageCount (711) and func_0201f32c's
// matched body runs for every valid id instead of early-returning.
extern "C" int port_message_bank_load(void); /* hal/message_boot.cpp */

extern "C" void port_message_archive_seat(void)
{
    if (data_0209d70c[0])
        return;
    if (!port_message_bank_load())
        std::fprintf(stderr, "[msg] message bank did not load; text boxes will "
                     "be declined by the ROM's own bounds check\n");
}

// ---- the twelve shared models Stage::InitResources preloads ----------------
//
// Stage::InitResources' own line, between LoadGraphics2D and LoadModel:
//
//     for (i = 0; i < 0xC; i++) Model::LoadFile(data_020756f0[i]);
//
// Twelve SharedFilePtrs in ov002 -- the coin, the mushroom, the shared pickup
// models -- loaded once at level boot so the classes that use them can read
// SharedFilePtr::filePtr straight out without loading anything themselves.
// Several do exactly that: OneUpMushroom::InitResources reads
// data_ov002_0210d9b8.filePtr for mushroom types 11 and 12, Coin does the same
// for its first two kinds, and neither has a LoadFile in front of it. The ROM
// can afford that because this loop already ran.
//
// The port never carried the loop over, because the castle grounds happens to
// name no object that takes the direct-read path. Bob-omb Battlefield names
// eight type-11 mushrooms, and the first one walked a null BMD_File into
// Model::AddToCommonModelDataArr, which takes a REFERENCE and hands it to
// LoadTexAndPal -- a fault on hardware just as much as on the host.
//
// It is spelled by NAME rather than by mounting data_020756f0 itself. That
// table is arm9 data holding twelve ov002 ADDRESSES, and on the host ov002's
// symbols are separate arrays; mounting the words would hand Model::LoadFile
// twelve DS addresses. The names are the same twelve targets, read out of the
// arm9 relocation table, in the ROM's own order.
extern "C" {
void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern unsigned char data_ov002_0210da48[], data_ov002_0210d9b8[],
    data_ov002_0210da50[], data_ov002_0210d9f8[], data_ov002_0210da40[],
    data_ov002_0210d9a0[], data_ov002_0210d9c0[], data_ov002_0210e7d8[],
    data_ov002_0210e3a0[], data_ov002_0211094c[], data_ov002_0211095c[],
    data_ov002_0210d9a8[];
}

extern "C" void port_stage_preload_shared_models(void)
{
    static void *const tbl[12] = {
        data_ov002_0210da48, data_ov002_0210d9b8, data_ov002_0210da50,
        data_ov002_0210d9f8, data_ov002_0210da40, data_ov002_0210d9a0,
        data_ov002_0210d9c0, data_ov002_0210e7d8, data_ov002_0210e3a0,
        data_ov002_0211094c, data_ov002_0211095c, data_ov002_0210d9a8,
    };
    static int done;
    int loaded = 0;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 12; ++i) {
        _ZN5Model8LoadFileER13SharedFilePtr(tbl[i]);
        /* SharedFilePtr is {u16 fileID; u8 numRefs; u8 pad; char *filePtr} */
        if (*(void *const *)((const char *)tbl[i] + 4))
            ++loaded;
        else
            std::fprintf(stderr, "  [preload] shared model %d (handle %u) did "
                         "not load\n", i, *(const unsigned short *)tbl[i]);
    }
    std::printf("[preload] %d/12 shared models seated\n", loaded);
}

extern "C" void port_level_mounts_install(void);

extern "C" void port_stage_a2_seat(void)
{
    port_message_archive_seat();
    port_stage_preload_shared_models();

    /* Which levels this build can mount. Registered before anything can ask,
       which is here rather than in main: the handoff seam is boot state like
       the registry and the sinits below it. */
    port_level_mounts_install();

    /* the scene tree root the spawn spine links under -- the real Stage.
       Constructing it IS the seating: Stage::Stage runs with data_020a4b6c[0]
       still null, so func_0203b438 takes its no-parent branch and writes the
       Stage's own SceneNode into the tree head, which is how the ROM's tree
       gets its root. port_stage_create asserts that it did. */
    {
        void *stage = port_stage_create();
        data_0209f5c0[0] = (int)(size_t)stage;
    }

    /* one local player, index 0, playing Mario, with the slot marked live so
       LoadEntranceObjects keeps the pointer it spawns */
    data_0209f21c = 1;
    data_0209f250 = 0;
    data_0209fc5c[0] = 1;
    data_02092128[0] = 0;
    /* data_0209caa0[0x41], which is byte 0xf of data_0209cad2 -- the third
       symbol of the run. Spelled at its owner rather than as an index past
       the first symbol's declared 0x14 bytes, which MSVC turns into a
       compile-time range check and a fast-fail. */
    if (data_0209cab4 - data_0209caa0 != 0x14 ||
        data_0209cad2 - data_0209caa0 != 0x32 ||
        data_0209cae4 - data_0209caa0 != 0x44)
        std::fprintf(stderr, "  [a2] SAVE BLOCK NOT CONTIGUOUS: +%d +%d +%d\n",
                     (int)(data_0209cab4 - data_0209caa0),
                     (int)(data_0209cad2 - data_0209caa0),
                     (int)(data_0209cae4 - data_0209caa0));
    data_0209cad2[0x41 - 0x32] = 0;

    /* Engine state the CAMERA's own boot reads, which under the entrance
       path runs inside LoadClsnAndObjects rather than after it. The harness
       used to stage this next to its hand-built camera; the same values, one
       step earlier. data_0209ee90[0x44/4] is the one that shows: it is the W
       scale Camera::Render hands PerspectiveW_, and at 0 the projection
       collapses and the frame comes out empty. */
    data_02092120 = -1;                 /* no area shown -> ChangeArea skips */
    data_0209f32c[0] = 0;               /* water level */
    data_0209fc48 = 0;                  /* not in a cutscene */
    data_0209f20c[0] = data_0209f294[0] = data_0209f2c4[0] = 0;
    data_0209b454[0] = 0;
    data_0209ee90[0x44 / 4] = 0x1000;

    /* The actor classes bring the first MOVING colliders onto the level's own
       collider list, so their vtable's own overrides go in before anything can
       spawn. hal/clsn_vtable.cpp has already seeded it with MeshCollider's. */
    hal_fill_moving_mesh_collider_vtable();

    /* The LEVEL overlay's own static initialisers, where the DS runs them:
       after the overlay is mounted and before anything spawns. Every
       SharedFilePtr the level's own actors load through is constructed there,
       and so are the three Vector3 arrays the moat spawns its bubbles along.
       Only ov009 has this hosted (gate 17), and on the DS a level overlay's
       sinits run only when THAT overlay is the loaded one -- running the
       castle grounds' under another level would seat its file pointers and
       its Bird state table over bytes that level never reads. */
    if (port_level_has_own_sinits())
        port_ov009_sinits();

    /* and the ACTOR overlays' -- ov085 and up, the overlays that exist for a
       handful of classes several levels share (hal/actor_overlays.cpp). Same
       place in the boot for the same reason. */
    port_actor_overlays_sinits();

    /* the five processing-list callbacks, then the class table and the gate */
    port_actor_lists_seat();
    /* The two hosted ROM pointer tables, before anything can dispatch through
       one. Both are reached from paths that only open up mid-session -- the
       star-get cutscene and the backup-media engine -- so a slot that has
       silently gone back to holding a DS address has to be caught here, on
       frame 0, and not ten thousand frames in. hal/ptr_tables.cpp. */
    port_ptr_tables_check();
    if (!hal_camera_check_layout())
        std::fprintf(stderr, "  [cam] LAYOUT CHECK FAILED\n");
    hal_fill_camera_vtable();
    hal_camera_slots_harness_owned();
    port_actor_registry_install();
    std::printf("[a2] scene root %p\n", port_stage_object());
}

/* ---- the path-binding bounds assert --------------------------------------
   RETIRED AS A WORKAROUND, KEPT AS AN ASSERT. It was written because the
   floor ClsnResult was arriving corrupt -- the ClsnResult MSVC-alignment skew
   fixed since -- and a corrupt record read back as path 0, which has seven
   nodes. func_ov002_020c0108 reads a bound path's nodes into a THREE-element
   stack array, so seven is 84 bytes into a 36-byte frame.

   With real records the port now produces exactly what the level produces.
   Of castle grounds' 22 CLPS entries twenty name path 0xff; only 16 and 17
   name a path at all, 5 and 3, and both of those have exactly two nodes.
   Measured: standing on an attribute-17 triangle binds path 3 and an
   attribute-16 one binds path 5, and nothing else ever binds. Every longer
   path in the level is for actors, which read them through PathPtr with
   their own storage.

   So this no longer rewrites anything on the level the port boots. What is
   left is a bounds check in front of a fixed-size ROM stack array, which
   costs two loads a frame and turns a future regression into one line of
   output instead of a smashed frame. */
extern "C" int port_stage_path_guard(void *player)
{
    char *c = (char *)player;
    unsigned id = *(unsigned *)(c + 0x670);
    if (id == 0xff)
        return 0;
    const unsigned char *tbl = (const unsigned char *)(size_t)data_020a0d84[0];
    int count = data_020a0d8c[0];
    if (tbl && (int)id < count && tbl[id * 6 + 2] <= 3)
        return 0;
    {
        static unsigned said;
        if (id < 32 && !(said & (1u << id))) {
            said |= 1u << id;
            std::fprintf(stderr, "  [path] ASSERT: binding %u has %d nodes and "
                         "the node walk holds 3 -- the level cannot produce "
                         "this, so the floor record is wrong\n", id,
                         tbl && (int)id < count ? tbl[id * 6 + 2] : -1);
        }
    }
    *(unsigned *)(c + 0x670) = 0xff;
    return 1;
}

/* ---- probes --------------------------------------------------------------
   The boot is a pointer rewrite over Nintendo bytes followed by matched code
   walking it, so what matters is what the game ends up reading. */
void port_level_probe(void)
{
    const PortLvlOverlay *o = (const PortLvlOverlay *)port_level_mount();
    const PortLevelDesc *d = port_level_desc();
    const char *ov = d->overlay;
    unsigned char *img = (unsigned char *)d->at(*d->ds_base);
    std::printf("[%s] level %d = %s\n", ov, d->id, d->name);
    std::printf("[%s] image %p .. %p (DS 0x%08x .. 0x%08x)\n", ov,
                (void *)img, (void *)(img + (*d->ds_end - *d->ds_base)),
                *d->ds_base, *d->ds_end);
    std::printf("[%s] LVL_Overlay: clps %p objTable %p bmd %u kcl %u "
                "subTables %p subCount %u flags %02x\n", ov,
                (void *)o->clps, (void *)o->objTable, o->bmdFileId,
                o->kclFileId, (void *)o->subTables, o->subCount, o->flags);
    unsigned n = *(const unsigned short *)o->objTable;
    const unsigned char *e = *(const unsigned char *const *)(o->objTable + 4);
    std::printf("[%s] objTable: %u kinds at %p\n", ov, n, (const void *)e);
    for (unsigned i = 0; i < n; ++i, e += 8)
        std::printf("        kind 0x%02x (grp %d idx %2d) count %3u entries %p\n",
                    e[0], (e[0] >> 5) & 7, e[0] & 0x1f, e[1],
                    *(const void *const *)(e + 4));
    for (unsigned s = 0; s < o->subCount; ++s) {
        const unsigned char *t =
            *(const unsigned char *const *)(o->subTables + s * 0xc);
        if (!t)
            continue;
        unsigned m = *(const unsigned short *)t;
        const unsigned char *se = *(const unsigned char *const *)(t + 4);
        std::printf("[%s] sub[%u] table %p: %u kinds\n", ov, s,
                    (const void *)t, m);
        for (unsigned i = 0; i < m; ++i, se += 8)
            std::printf("        kind 0x%02x (grp %d idx %2d) count %3u "
                        "entries %p\n", se[0], (se[0] >> 5) & 7, se[0] & 0x1f,
                        se[1], *(const void *const *)(se + 4));
    }
}

void _ZN7PathPtr6FromIDEj(void *self, unsigned id);
unsigned _ZNK7PathPtr8NumNodesEv(const void *self);
void _ZNK7PathPtr7GetNodeER7Vector3j(const void *self, int *out, unsigned idx);

void port_stage_a_probe(void *mc_)
{
    MeshCollider *mc = (MeshCollider *)mc_;
    const PortLvlOverlay *o = (const PortLvlOverlay *)port_level_mount();

    /* CLPS: "CLPS" magic, u16 entry size, u16 count, then the records --
       byte 0 the surface type, byte 4 the path id (0xff = none). */
    const unsigned char *clps = o->clps;
    unsigned esize = *(const unsigned short *)(clps + 4);
    unsigned ecount = *(const unsigned short *)(clps + 6);
    std::printf("[clsn] clps %p magic %.4s entrySize %u count %u\n",
                (const void *)clps, (const char *)clps, esize, ecount);
    for (unsigned i = 0; i < ecount; ++i) {
        const unsigned char *r = clps + 8 + i * esize;
        std::printf("        [%2u] type %02x path %02x  %02x %02x %02x %02x "
                    "%02x %02x\n", i, r[0], r[4], r[1], r[2], r[3], r[5],
                    r[6], r[7]);
    }

    /* The KCL the boot loaded, and the surface types the walk will resolve
       through the block above. The triangle array runs from tris up to the
       octree the header's fourth word points at. */
    {
        const KCL_File *f = mc->kclFile;
        long tricount = ((const char *)f->unk_0c - (const char *)f->tris) / 16;
        std::printf("[clsn] kclFile %p positions %p normals %p tris %p "
                    "octree %p (%ld triangles)\n",
                    (const void *)f, (const void *)f->positions,
                    (const void *)f->normals, (const void *)f->tris,
                    (const void *)f->unk_0c, tricount);
        int seen[256];
        std::memset(seen, 0, sizeof seen);
        int distinct = 0;
        if (tricount < 0 || tricount > 65536)
            tricount = 256;
        for (long t = 1; t <= tricount; ++t) {
            unsigned a = f->tris[t].attribute & 0xff;
            if (a < 256 && !seen[a]) { seen[a] = 1; ++distinct; }
        }
        std::printf("[clsn] %ld triangles: %d distinct surface types (",
                    tricount, distinct);
        for (int a = 0; a < 256; ++a)
            if (seen[a]) std::printf(" %d", a);
        std::printf(" )\n");
        std::printf("[clsn] world Y bounds: min %d (%.1f) max %d (%.1f)\n",
                    data_02092138, data_02092138 / 4096.0f,
                    data_0209212c, data_0209212c / 4096.0f);
        /* THE COLLISION'S OWN WORLD EXTENT, straight off the vertex array
           (positions run up to the normals table; each is a 12-byte s32
           vector stored at 1/64 of a Fix12i, hence the <<6). This is the
           number the level model's render scale is checked against: the
           two describe the same terrain, so their world bounds must be the
           same bounds. */
        {
            const s32 (*p)[3] = f->positions;
            long n = ((const char *)f->normals - (const char *)f->positions) /
                     12;
            if (n > 0 && n < 65536) {
                int lo[3] = {1 << 30, 1 << 30, 1 << 30};
                int hi[3] = {-(1 << 30), -(1 << 30), -(1 << 30)};
                for (long i = 0; i < n; ++i)
                    for (int k = 0; k < 3; ++k) {
                        int v = p[i][k] << 6;
                        if (v < lo[k]) lo[k] = v;
                        if (v > hi[k]) hi[k] = v;
                    }
                std::printf("[clsn] %ld vertices, world bounds x[%.1f..%.1f] "
                            "y[%.1f..%.1f] z[%.1f..%.1f]\n", n,
                            lo[0] / 4096.0f, hi[0] / 4096.0f, lo[1] / 4096.0f,
                            hi[1] / 4096.0f, lo[2] / 4096.0f, hi[2] / 4096.0f);
            }
        }
        std::printf("[clsn] collider pair: 0x%x / 0x%x\n",
                    *(int *)((char *)mc_ + 0x2c), *(int *)((char *)mc_ + 0x38));
    }

    /* Paths: the ones the level's OWN CLPS entries bind to.
       The probe ids used to be the literals 5 and 3, which are the two the
       castle grounds' entries 16 and 17 name. A level with no path table at
       all -- Bob-omb Battlefield loads none -- sent PathPtr::FromID through a
       null base and faulted inside the probe, with the boot itself already
       finished and correct. Read the bindings out of the CLPS block instead,
       so the probe reports whatever the booted level actually has. */
    std::printf("[path] table %p count %d nodes %p\n",
                (void *)(size_t)data_020a0d84[0], data_020a0d8c[0],
                (void *)(size_t)data_020a0d88[0]);
    if (!data_020a0d84[0] || data_020a0d8c[0] <= 0) {
        std::printf("[path] this level binds no paths\n");
        return;
    }
    {
        unsigned seen = 0;                /* one bit per id already probed */
        int probed = 0;
        for (unsigned i = 0; i < ecount && probed < 4; ++i) {
            unsigned id = clps[8 + i * esize + 4];
            if (id == 0xff || id >= 32 || (seen & (1u << id)))
                continue;
            if ((int)id >= data_020a0d8c[0]) {
                std::printf("[path] CLPS entry %u binds path %u, past the "
                            "table's %d\n", i, id, data_020a0d8c[0]);
                continue;
            }
            seen |= 1u << id;
            ++probed;
            int path[2] = {0, 0};
            _ZN7PathPtr6FromIDEj(path, id);
            unsigned nodes = _ZNK7PathPtr8NumNodesEv(path);
            std::printf("[path] CLPS %u -> FromID(%u) -> rec %p firstNode %u "
                        "count %u\n", i, id, (void *)(size_t)path[0],
                        *(unsigned short *)(size_t)path[0], nodes);
            for (unsigned k = 0; k < nodes && k < 4; ++k) {
                int v[3];
                _ZNK7PathPtr7GetNodeER7Vector3j(path, v, k);
                std::printf("        node %u = (%.0f, %.0f, %.0f)\n", k,
                            v[0] / 4096.0f, v[1] / 4096.0f, v[2] / 4096.0f);
            }
        }
        if (!probed)
            std::printf("[path] no CLPS entry on this level binds a path\n");
    }
}
}  /* extern "C" */

/* ---- the BMD shrink-to-fit switch ----------------------------------------
   Model::LoadAndSetFile's last step is func_02017060: reallocate the loaded
   BMD down to the size its own header reports, once SetFile has parsed it.
   The port's other model path already declines it -- hal/cxxname_bridge.cpp's
   expansion of Model::LoadFile carries the note "Reallocate is a DS heap
   shrink, no-op here" -- and this is the same decision spelled where the
   caller can see it. SM64DS_MODEL_SHRINK=1 turns it back on for the A/B. */
extern "C" int port_model_shrink_enabled(void)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_MODEL_SHRINK") != 0;
    return on;
}

// ---- gate 31: what the level change has to undo -----------------------------
//
// A level change tears the actors down through the game's own path (see
// hal/level_change.cpp). What that path cannot reach is the host storage the
// port's boot staged BESIDE the game -- storage the game never allocated and
// so never frees. Two things are in that class here, and they are the two
// this file owns.
//
//  1. THE HANDLE TABLE. LoadFile is the port's stand-in for func_0201818c and
//     it caches one persistent SharedFilePtr per handle. The handles a boot
//     asks for are the LEVEL's: its KCL, its object files. Carrying them into
//     the next level would hold the old level's files loaded forever and, at
//     sixteen slots, run the table out on the third or fourth change with
//     "out of host file slots". Release is the ROM's own refcount drop
//     (SharedFilePtr::Release -> func_02017c24 when the last reference goes),
//     so the file image goes back to the game heap the same way it would on
//     the DS.
//
//  2. THE ENTRANCE CACHE. g_entrance_entries points into the CURRENT level
//     overlay's own bytes. After a change it points into the previous
//     level's, which is exactly the stale-pointer shape a transition
//     produces; the debug menu's warp list reads it.
//
// The sub-loader globals below it (path counts, fog, minimap, teleport) are
// all rewritten by the next LoadClsnAndObjects before anything reads them, so
// they are not strictly reset work. They are zeroed anyway: a level that
// happens not to carry one of those tables would otherwise read the previous
// level's count against the new level's pointer, and that is a fault rather
// than a wrong number.
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(struct PortSharedFilePtr *self);
void _ZN5Stage18ResetMeshCollidersEv(void);
int port_level_mount_register(int level, void *(*fn)(void));
unsigned port_level_ds_overlay(int level);
void port_actor_census_reset(void);      /* hal/actor_registry.cpp */
}

extern "C" void port_level_reset_host(void)
{
    /* THE SLOTS ARE DROPPED, NOT RELEASED, and that is a measured decision
       rather than a shortcut. SharedFilePtr::Release ends in func_02017c24,
       which hands filePtr back with Memory::Deallocate. Doing that here
       faulted inside ExpandingHeapAllocator::UnlinkNode on the level's third
       handle (1944, the level's icg/icl pair) every time -- the block was
       already off the heap, so something else on the level owns that image
       and gives it back during its own cleanup.

       Which owner is the open question and it is worth answering, because
       until it is, the images behind these handles are what a level change
       leaks. The measurement is in the [lvl] line the change prints: heap
       free before, after the teardown, and after the next level is up. On the
       castle grounds re-entering itself the net is the number to watch, and
       SM64DS_TRACE_LEVEL=1 names every slot it drops. A double free is a
       corrupted heap two transitions later with no trail; a leak is a number
       that goes down by a known amount. This takes the second one on purpose
       until the ownership is settled. */
    const int trace = std::getenv("SM64DS_TRACE_LEVEL") != 0;
    for (int i = 0; i < g_loadfile_used; ++i) {
        if (trace)
            std::printf("  [lvl] dropping file slot %d: handle %u refs %u "
                        "ptr %p\n", i, g_loadfile_slot[i].fileID,
                        g_loadfile_slot[i].numRefs,
                        (void *)g_loadfile_slot[i].filePtr);
        g_loadfile_slot[i].fileID = 0;
        g_loadfile_slot[i].numRefs = 0;
        g_loadfile_slot[i].filePtr = 0;
    }
    g_loadfile_used = 0;

    g_entrance_entries = 0;
    g_entrance_count = 0;

    /* the census counters, so the warped-into level reports what IT spawned
       rather than the sum with the level it replaced (hal/actor_registry.cpp) */
    port_actor_census_reset();

    data_ov002_0211118c = 0;
    data_020a0d8c[0] = 0;
    data_0209f31c[0] = 0;  data_0209f258[0] = 0;
    data_0209f328[0] = 0;  data_0209f214[0] = 0;
    data_0209f334[0] = 0;  data_0209f2e8[0] = 0;
    data_0209f348[0] = 0;  data_0209f25c[0] = 0;
    data_0209f2d0[0] = 0;
    data_0209f338[0] = 0;
    data_020a0d84[0] = 0;
    data_020a0d88[0] = 0;
}

// ---- the Stage, between two levels -----------------------------------------
//
// The Stage object outlives the level (hal/level_change.cpp says why), so its
// two level-owned sub-objects have to be put back to their pre-boot state by
// hand.
//
//   +0x91c  the level MeshCollider. Stage::LoadClsnAndObjects SetFiles it and
//           registers it, so what the next boot needs is the registry empty.
//           Stage::ResetMeshColliders is the ROM's own zeroing of exactly that
//           table (data_020a0c80, 0x18 entries) and it is what
//           Stage::CleanupResources calls for the same reason.
//
//   +0x86c  the level Model. Stage::LoadModel overwrites it wholesale, so the
//           only thing owed is the old BMD -- Model::~Model frees the block at
//           +0x4c and ModelBase::~ModelBase the rest. The object is
//           re-constructed straight after so the next LoadModel writes into a
//           Model rather than into a corpse.
//
// The SKYBOX at +0x9bc is a Model the Stage NEWS off the game heap, and
// Stage::LoadSkybox news another one every time it runs. Deleting it here is
// what keeps a level change from leaking one skybox model per transition.
//
// WHY THIS IS NOT Stage::CleanupResources, checked against the src. The full
// teardown (src/_ZN5Stage16CleanupResourcesEv.cpp) is host-hostile in three
// places the port has no answer for: Scene::SetAndStopColorFader (the COLOR
// fader the title path already routes around -- data_0209f5e8 is a null host
// slot), func_02073244 over the FaderWipe array (the wipe subsystem the port
// stages separately), and UnloadLevelOverlays / UnloadArchive (the NARC
// archive path the port's fs seam replaces). Its Model::LoadAndSetFile
// (src) also does NOT free the old BMD -- it overwrites modelFile and calls
// SetFile -- so the D2/C1 reseat below is load-bearing, not belt-and-braces:
// without it the previous level's BMD and ModelComponents leak and the render
// walk reads stale components. So this is the level-owned-subobject SUBSET of
// CleanupResources that is host-safe, and it stays until the boot hosts the
// whole InitResources/CleanupResources pair with the archive, VRAM-bank and
// fader subsystems fed (see the report's "what remains").
extern "C" {
void *_ZN5ModelD2Ev(void *self);
void *_ZN5ModelC1Ev(void *self);
void _ZN6Memory10DeallocateEPv(void *p);
}

extern "C" void port_level_stage_reseat(void *stagev)
{
    char *stage = (char *)stagev;

    _ZN5Stage18ResetMeshCollidersEv();

    /* the level model, in place */
    _ZN5ModelD2Ev(stage + 0x86c);
    _ZN5ModelC1Ev(stage + 0x86c);

    /* the skybox, which is a pointer rather than a member */
    void **sky = (void **)(stage + 0x9bc);
    if (*sky) {
        _ZN5ModelD2Ev(*sky);
        _ZN6Memory10DeallocateEPv(*sky);
        *sky = 0;
    }
}

// ---- the mounts, registered against the seam --------------------------------
/* Every level this build can mount, handed to the handoff registry with the
   ROM's own table as the check. Gate 30 owns which levels exist (the table at
   the head of this file) and gate 31 owns changing between them, so this is
   the one place the two meet: a level added to the table registers itself here
   without the registry ever learning what a PortLevelDesc is.

   The disagreement line matters more than it looks. data_02092208 is the ROM's
   level -> LVL_Overlay map, so a row whose hand-entered address does not match
   it is a row pointing at the wrong overlay, which reads downstream as a level
   that loads and renders nonsense rather than as a mount failure. Say it, do
   not abort: the table is still the thing the boot uses, and a loud mismatch
   is more useful than a dead run. */
extern "C" void port_level_mounts_install(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < PORT_LEVEL_COUNT; ++i) {
        const PortLevelDesc *d = &port_level_table[i];
        const unsigned ds = port_level_ds_overlay(d->id);
        if (ds != d->lvl_overlay)
            std::fprintf(stderr, "  [lvl] LEVEL %d DISAGREES: the ROM's table "
                         "says 0x%08x, this file mounts 0x%08x\n",
                         d->id, ds, d->lvl_overlay);
        port_level_mount_register(d->id, port_level_mount_fns[i]);
    }
}
