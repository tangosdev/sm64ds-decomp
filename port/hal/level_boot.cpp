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
#include "dsstate_seg.h"
#include "hal/comms_seam.h"   /* run mg16 lane MP3: port::vs_player_count() */

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
     - ROCK_PILLAR (58): DOWNGRADED 2026-08-15 (run link60, lane L1) to NOT
       REPRODUCIBLE ON THIS TREE. What follows is the original entry, then why
       it no longer stands. Do not route new work off the original text.

       AS WRITTEN AT GATE 188: "faults c0000005 in ModelComponents::Render the
       first on-screen frame it draws (measured f17 when Mario spawns adjacent),
       NOT the collider and NOT the particle path (SM64DS_NO_FX_RENDER=1 still
       crashes; SM64DS_RP_NORENDER moves the fault elsewhere in the same
       sequence). Its render (func_ov016_02112b28) is the identical slot-5 Model
       dispatch that SHIP_UP renders through cleanly, so the difference is the
       MODEL FILE: its bmd (fs id 1173) loads but its Model at +0xd4 reaches
       Model::Render with a bad/unbuilt ModelComponents. A model-BUILD issue for
       RockPillar's bmd, not a mount or seating one -- deferred to the
       model-loader lane."

       THE MODEL-LOADER LANE TOOK IT AND FOUND NO FAULT TO FIX.

       Negative result, A/B'd across two independently built trees (with and
       without that lane's port/ntr/io.cpp GXSTAT change, which is a no-op here
       -- a traced level 8 reads GXSTAT=06000000 on all 200 of its display-list
       submits):
           SM64DS_LEVEL=8 SM64DS_SPAWN=-1371,-2800,-4586
           SM64DS_WINDOW_SELFTEST=900 SM64DS_FAULTS_FATAL=1
       exits 0 both ways, 900 frames, at a bit-identical end position
       (-5611966,-11866368,-18782398). That spawn puts Mario on the first of the
       SIX pillars the census reports ("+ 58 x6 ROCK_PILLAR"; the six positions
       are printed by the [pos] line).

       AND THE MODEL IS FULLY BUILT, which is the specific claim the original
       entry made and got wrong. SM64DS_ACTOR_PROBE=1 on that run:
           [actor] ROCK_PILLAR  model 30038644 file 30065208
                   transforms 300651D8 mat.t (-172,-363,-574) scene
       Non-null file AND non-null transforms, which by this probe's own contract
       (port/hal/actor_classes.cpp: "a null file has a load problem", "a null
       [transforms] is a load that did not finish") is a completed load. The
       probe fires on a class's FIRST Render, so the line also proves the class
       reaches its Render rather than being culled ahead of it.

       WHAT ACTUALLY EXPLAINS THE ORIGINAL FAULT: the SIG-RP heap trample, fixed
       in f5d936503 and d241d27bb. Both DESCEND from a9cb3e5ed, the gate-188
       commit that wrote this entry, and both are in this tree. RockPillar's
       teardown was releasing gameHeapPtr and SignPost's slot through the G0/G1
       linker collision and corrupting the ExpandingHeap free list; d241d27bb's
       own message unifies three crash signatures under it. A c0000005 surfacing
       in ModelComponents::Render on a trampled heap needs no model-build defect
       to explain it, and this entry was written before that fix landed.

       ALSO CORRECTED: SM64DS_RP_NORENDER was never implemented. It appears
       nowhere in the tree except the sentence above, so the "moves the fault
       elsewhere" observation has no mechanism behind it and is not evidence.
       (SM64DS_NO_FX_RENDER is real -- hal/particle_bridges.cpp.)

       RETIREMENT NOT CLAIMED, and the reason is specific: nobody has confirmed
       the pillar actually RASTERIZES. Reaching Render with a built Model is not
       the same as pixels on screen, and this lane did not look at a frame. So
       this stays on the books as "not reproducible", not "fixed". RE-OPEN IT
       WITH EVIDENCE AND COORDINATES -- a spawn, a frame number and a dump --
       rather than by citing the original text above. Idle-far and the other
       five movers were clean at gate 188 and remain so.
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
       (193, x1). Several also reach ov002 and carry G0/G1, so the mount is only
       the first part.
       (run linkw wave 6, lane w6-f: FLAMETHROWER (318, x5 here) was in this
       list and no longer belongs in it -- ov095 has been mounted per symbol
       since gate 83, so the class was never overlay-blocked; what blocked it
       was the undecompiled Behavior, and that is now a host copy. Hosted, and
       level 14's census reads 180/29 where it read 175/34.)

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
   blocked on undecompiled bodies or an unmounted ov064, none a free share.
   Most of the six have landed since, the last of them FLAMETHROWER (318, x3
   here) in run linkw wave 6, lane w6-f: the undecompiled body that blocked it
   was its Behavior, and that is now the host copy
   port/unmatched/Flamethrower_Behavior.c. Measured on that lane, level 15's
   census goes 50 spawned / 6 skipped -> 53 / 3, and the whole remaining skip
   list is ids 31 (x2) and 84 (x1). */
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
   or individually blocked (PAINTING, PATH_LIFT). Corrections to that commit's
   reading, re-derived while hosting:
     - FLAMETHROWER (318, ov095) was misfiled as a fill+row+closure candidate;
       its Behavior (0x021368f0, 0x470 bytes) is NOT decompiled, so it is
       blocked on the class body like level 14's lava cast, not on a fill.
       RETIRED by run linkw wave 6, lane w6-f. The reading above is still the
       right one -- the Behavior is genuinely undecompiled and no matched TU of
       that name exists -- but "blocked" is no longer the outcome: the method is
       hosted as a per-instruction transcription of the ROM listing
       (port/unmatched/Flamethrower_Behavior.c), the other four methods are
       matched src in slice_w6f.txt, and the fill is
       hal/actor_classes_flamethrower.cpp. FLAMETHROWER is registered and
       spawns on all four levels that place it (13 x3, 14 x5, 15 x3, 37 x3);
       level 13's census went 183/5 -> 186/2 on the lane's own builds. The
       skip list here is now ids 31 (x1) and 262 (x1).
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

/* ov045 = LEVEL 37, Bowser in the Fire Sea (data/stage/koopa2_map), course 16 --
   the first level hosted outside the 1..15 block, and the one the collision
   family's ExtendingMeshCollider needs (port/slice_w1l4.txt).
   data_020758c8[37] = 45, LVL_Overlay data_02092208[37] = 0x021124d4, OV0
   handles at +8 (bmd 0x0782/kcl 0x077a/icg 0x0783/icl 0x0784) resolve directly
   through handles.tsv to koopa2_map's all.bmd/kcl/icg/icl; SUBLEVEL_LEVEL_TABLE
   [37] = 16, subCount 1, a 33715-byte KCL. Mounted --whole; own_sinits 0.

   Both arm9 tables are WORD tables (stride 4). The recipe above and the level
   comments in CMakeLists say "data_020758c8[level]" as if it were bytes; read
   that way it gets all fifteen known levels wrong, so the read here is the *4
   one port_level_ds_overlay/port_level_overlay_id already do.

   The handles were read out of extracted/overlays/overlay_0045.bin: ov045 is
   compressed:true, so the dsd export is the compressed image and its halfwords
   are not handles at all. */
void port_ov045_patch(void);
void *port_ov045_at(unsigned ds);
extern unsigned char port_ov045_image[];
extern const unsigned port_ov045_ds_base, port_ov045_ds_end;

/* ov046 = LEVEL 38, the Bowser in the Fire Sea BATTLE ARENA
   (data/stage/koopa2_boss), course 16 -- the first boss arena the port mounts,
   the second half of the level-37 course. Read from the ROM like the rest, and
   the whole pipeline re-validated against all sixteen mounted rows before
   anything about ov046 was believed (every known id/overlay/LVL_Overlay
   reproduces exactly):
     data_020758c8[38] = 46 (WORD table, the ov045 correction), LVL_Overlay
     data_02092208[38] = 0x02111560, OV0 handles at LVL_Overlay+8 (bmd 0x0777 /
     kcl 0x0775 / icg 0x0778 / icl 0x0779) resolve DIRECTLY through
     build/assets/handles.tsv -- delta 0, the mapping levels 8..15 and 37 use --
     to koopa2_boss's koopa2_boss_all.bmd (4908) / koopa2_boss.kcl (2561) /
     koopa2_boss_icg.bin (17580) / koopa2_boss_icl.bin (512). Handles read from
     extracted/overlays/overlay_0046.bin, the image ovdata mounts, never the
     dsd export. SUBLEVEL_LEVEL_TABLE[38] (0x02075298, byte table) = 0x10 =
     course 16, subCount 1, flags 0 -- an ordinary one-area arena, no sublevel
     tricks.

   ITS OBJECT-OVERLAY SET IS ov060 + ov089, BY THE ARM9'S OWN SPECIAL CASE.
   LoadOrUnloadObjectOverlays (src/_Z26LoadOrUnloadObjectOverlaysPFviEi.cpp)
   short-circuits idx 0x24/0x26/0x28 (levels 36/38/40, the three koopaN_boss
   arenas): each loads ov060 unconditionally and RETURNS EARLY, skipping even
   the ov098/ov102 tail every other level loads -- but the selector loop runs
   BEFORE that early return, and its row [0,0,0,0,1,0,0] ALSO loads ov089
   (KEY 282, LAST_STAR 283, the boss-reward spawns; load-bearing for whoever
   wires the KEY drop). So the arena's own cast lives in ov060
   (Bowser/BowserTail/BowserFire/BowserShockwaves/SpikeBomb and the arena
   platform classes), and its census skips that cast by name until the ov060
   pack is hosted. Mounted --whole like the rest; own_sinits 0. */
void port_ov046_patch(void);
void *port_ov046_at(unsigned ds);
extern unsigned char port_ov046_image[];
extern const unsigned port_ov046_ds_base, port_ov046_ds_end;

/* ov044 = LEVEL 36, the Bowser in the Dark World BATTLE ARENA
   (data/stage/koopa1_boss), course 15 -- the SECOND of the three koopaN_boss
   arenas the port mounts, and ov048 below is the third. Everything re-derived
   from extracted/arm9_dec.bin (base 0x02004000) and
   extracted/overlays/overlay_0044.bin on this lane; the level-38 row above is
   the model and it reproduces byte for byte from the same reads:

     1. data_020758c8[36] -- the arm9 level->overlay map, a WORD table (stride
        4, the ov045 correction; byte stride gets every level wrong). The read
        is 0x020758c8 + 36*4 = 0x02075958, raw word 0x0000002c = overlay 44.
        IT IS level+8, AND THAT IS A READ, NOT AN ASSUMPTION: all 52 words were
        dumped on this lane and the table is contiguous 8..59 with no
        exceptions, so the +8 is a property the table HAS, not a rule applied
        to it. (Same read gives [38] = 0x2e = 46 and [40] = 0x30 = 48, which
        reproduces the landed level-38 row.)

     2. LVL_Overlay = data_02092208[36], also a word table: 0x02092208 + 36*4 =
        0x02092298, raw word 0x021113b0. Re-derived for all 52 levels on this
        lane and checked against every one of the seventeen rows already in the
        table below -- 17/17 reproduce exactly, which is what makes the two new
        reads trustworthy.

     3. The four OV0 asset handles at LVL_Overlay+8, read out of
        extracted/overlays/overlay_0044.bin (image offset 0x210; ov044 is
        compressed:true in overlays.yaml, so the dsd export is the COMPRESSED
        image and its halfwords are noise). Raw record bytes at 0x021113b0:
          cc 13 11 02 | 74 13 11 02 | 6b 07 | 69 07 | 6c 07 | 6d 07 | ...
        clps 0x021113cc, objTable 0x02111374, bmd 0x076b / kcl 0x0769 /
        icg 0x076c / icl 0x076d. RESOLVED THROUGH build/assets/handles.tsv,
        not arithmetic -- delta 0, the mapping levels 8..15 / 37 / 38 use:
          1899 -> data/stage/koopa1_boss/koopa1_boss_all.bmd  (4411 bytes)
          1897 -> data/stage/koopa1_boss/koopa1_boss.kcl      (1624)
          1900 -> data/stage/koopa1_boss/koopa1_boss_icg.bin  (13728)
          1901 -> data/stage/koopa1_boss/koopa1_boss_icl.bin  (512)
        subTables 0x0211137c, subCount 1, flags 0 -- one area, no sublevel
        tricks, the smallest KCL of any mounted level.

     4. SUBLEVEL_LEVEL_TABLE (arm9 0x02075298, a BYTE table): 0x02075298 + 36 =
        0x020752bc, raw 0x0f = course 15. koopa1 is the ROM's own name for the
        first Bowser stage, and [35] is 0x0f too -- level 35 is the approach,
        36 the arena, exactly the 37/38 pairing one course up.

   ITS CAST is ov060 + ov089, by the same arm9 special case the level-38 row
   documents (LoadOrUnloadObjectOverlays short-circuits idx 0x24/0x26/0x28 --
   levels 36/38/40 -- to ov060 with an early return, after the selector loop has
   already pulled ov089 in). Read off the level's own object table on this lane,
   the arena places THREE ids: 279 BOWSER x1 and 284 SPIKE_BOMB x8 (both landed
   in wave 6, lane w6-A) and 301 x16 -- which is NOT an ov060 class at all. The
   arm9 spawn table 0x02090864 + 301*4 = 0x02090d18 holds 0x021115e0, a
   LEVEL-OVERLAY address, and at that address in overlay_0044.bin the record's
   +0 word is 0x021112dc (inside ov044's own 0x021111a0..0x02111680 window) and
   its +4 halfword is 0x012d = 301. Both attribution tests pass on ov044, so
   the sixteen orange balls ringing the arena are ov044's own class -- hosted
   this lane in hal/actor_classes_ov044.cpp. Mounted --whole like the rest, and
   ALSO per symbol (port/ov044_syms.txt) because that class reaches its
   SpawnInfo and its two bss cells by name; own_sinits stays 0 (the ov044 sinit
   runs from the registry fill, the ov045/ov060 lane-ownership shape). */
void port_ov044_patch(void);
void *port_ov044_at(unsigned ds);
extern unsigned char port_ov044_image[];
extern const unsigned port_ov044_ds_base, port_ov044_ds_end;

/* ov048 = LEVEL 40, the Bowser in the Sky BATTLE ARENA
   (data/stage/koopa3_boss), course 17 -- the LAST of the three koopaN_boss
   arenas, and with it every boss arena in the game is mounted. Same four reads,
   same discipline:

     1. data_020758c8[40] = word at 0x020758c8 + 40*4 = 0x02075968, raw
        0x00000030 = overlay 48. level+8 again, from the same 52-word dump.

     2. data_02092208[40] = word at 0x02092208 + 40*4 = 0x020922a8, raw
        0x02111624. (0x02111624 is ALSO the address of a dsd data label inside
        ov044 -- every level overlay is linked at 0x021111a0, so the windows
        collide by construction. The word is ov048's LVL_Overlay because it is
        what ov048's row of the table says; port_level_mounts_install asserts
        each row against port_level_ds_overlay and would say so loudly if not.)

     3. LVL_Overlay+8 out of extracted/overlays/overlay_0048.bin (image offset
        0x484; compressed:true, so again NOT the dsd export). Raw at 0x02111624:
          80 17 11 02 | ac 15 11 02 | 87 07 | 85 07 | 88 07 | 89 07 | ...
        clps 0x02111780, objTable 0x021115ac, bmd 0x0787 / kcl 0x0785 /
        icg 0x0788 / icl 0x0789 -> through build/assets/handles.tsv (delta 0):
          1927 -> data/stage/koopa3_boss/koopa3_boss_all.bmd  (7228 bytes)
          1925 -> data/stage/koopa3_boss/koopa3_boss.kcl      (1889)
          1928 -> data/stage/koopa3_boss/koopa3_boss_icg.bin  (15028)
          1929 -> data/stage/koopa3_boss/koopa3_boss_icl.bin  (512)
        subTables 0x021115c8, subCount 1, flags 0.

     4. SUBLEVEL_LEVEL_TABLE[40] = byte at 0x020752c0 = 0x11 = course 17.
        [39] is 0x11 as well -- the approach/arena pair, koopa3.

   ITS CAST NEEDS NO NEW CLASS. The level's object table places exactly three
   ids: 167 BOWSER_SKY_PLATFORM x10, 279 BOWSER x1, 284 SPIKE_BOMB x5 -- all
   three ov060, all three registered by wave 6 lane w6-A, and 167's registry
   comment already says in as many words that it is "level 40's floor (not
   placed in 38)". So level 40 is a pure mount, and no slice.

   ONE OF THE THREE IS DECLINED HERE, and the reason is a HOST BOOT-ORDER limit
   in a file this lane may not touch. Level 40 is the only level of the
   fifty-two that places id 167, so wave 6 registered a class that had never
   actually spawned; mounting the arena ran it for the first time and it faulted
   under FAULTS_FATAL. daKpa3Bg_c::InitResources (src/func_ov060_021182b0.cpp)
   calls CopyTexPalFromLevelModel, whose first line loads through
   data_0209f320 -- the Stage's ModelComponents pointer, written ONLY by
   Stage::LoadModel. The ROM seats it first (Stage::InitResources :361 LoadModel,
   :363 LoadClsnAndObjects); port/tests/walk_window.cpp does the two the other
   way round (:2099 the boot, :2265 LoadModel), so the pointer is still null when
   the object pass runs. port_stage_a_boot cannot compensate: a second
   Stage::LoadModel is not idempotent (Model::UpdateFileOffsets rebases the BMD's
   offsets in place and the harness's own call would run it twice, corrupting the
   level model on every level). The whole derivation, with the fault dump, is in
   port_host_abi_blocked (hal/actor_registry.cpp). Level 40 therefore boots and
   exits 0 with the ten platforms named in the census as skipped rather than
   faked. Mounted --whole; own_sinits 0, and no per-symbol half (nothing in ov048
   is reached by name). */
void port_ov048_patch(void);
void *port_ov048_at(unsigned ds);
extern unsigned char port_ov048_image[];
extern const unsigned port_ov048_ds_base, port_ov048_ds_end;

/* ---- run linkw wave 8, lane w8-mounts: SIXTEEN LEVELS AT ONCE -------------
   Every remaining data/stage level the ROM has a row for was derived and
   attempted; sixteen of them boot and are here. The seven that do not are
   named with their measured cause at the end of this block, and every one of
   the seven is blocked BEHIND the mount, not by it.
   The recipe below port_level_table[] is unchanged and this is nothing but it,
   applied sixteen times; what is new is only that the derivation was
   mechanised (one read per field, no numbering assumed) and that the whole
   sweep of level ids 0..51 was measured rather than the interesting-looking
   ones picked.

   LEVELS 36 AND 40 ARE NOT IN THIS BLOCK'S TABLES because they landed a wave
   earlier, in the two ov044/ov048 blocks directly above -- this lane derived
   them independently and deliberately held them for that lane, and the two
   derivations agree. They ARE in port_level_table[] below, at indices 17 and
   18, ahead of this block's sixteen. Read this block's sweep as covering the
   other fifty ids.

   HOW EVERY FIELD WAS READ, and how the reader was proved before it was
   trusted. This is the general form of the same four reads the ov044 and
   ov048 blocks above spell out per level. Each row is four reads out of
   extracted/arm9_dec.bin (base 0x02004000) and the level's own overlay image,
   plus one lookup:

     data_020758c8[level]   the level's own overlay id.  A WORD table; the 52
                            words read 8..59, so ov = level + 8 holds for every
                            id, but it is read, never assumed.
     data_02092208[level]   the LVL_Overlay DS address. It points INSIDE the
                            level overlay, not into arm9, so the record is read
                            out of extracted/overlays/overlay_NNNN.bin at
                            overlays.yaml's base_address (0x021111a0 for every
                            level overlay) -- the raw unpack image, never the
                            dsd export.
     LVL_Overlay+0x08..0x0e bmd/kcl/icg/icl handles. They are the 'handle'
                            column of build/assets/handles.tsv DIRECTLY, delta
                            0 (the "+232" in some older mount notes is the
                            handle-to-file_id gap, a different number; the tsv
                            carries both columns and the handle one is the key).
     0x02075298[level]      SUBLEVEL_LEVEL_TABLE, the course number.

   VALIDATED BEFORE IT WAS BELIEVED, by both lanes independently and in two
   different shapes -- kept together here because they check different things:
     - this lane's reader reproduced NINE already-mounted rows field for field
       first -- levels 1, 4, 5, 6, 9, 10, 12, 13, 37 -- each one's overlay id,
       LVL_Overlay address, FOUR HANDLES and course matching the row already in
       the table below and the evidence already in its CMake note (level 9's
       bmd 1894 / kcl 1892 / icg 1895 / icl 1896 among them). Nine rows, all
       five fields.
     - the ov044/ov048 lane dumped all 52 words of data_02092208 and checked
       the LVL_Overlay column against every one of the seventeen rows then
       mounted, 17/17 exact. Seventeen rows, one field.
   Neither subsumes the other and they agree everywhere they overlap, so the
   reader is proved both across the whole table and across every field. Only
   then were new ids derived.

   THE TEN IDS THAT ARE NOT STAGES: 0, 29, 30, 32, 34, 41, 42, 43, 46 and 51
   have LVL_Overlay records whose four handles do not resolve to one
   data/stage directory (they read MG/ minigame art or nothing), and courses
   21..28 and 255. They are measured and declined, not forced.

   THE SHARED-WINDOW QUESTION, measured the way the ov045/ov046 notes ask for.
   All sixteen overlays load at the same DS base as every level overlay already
   mounted, and ovdata.py --cross DROPS a target that lands in more than one
   mounted window -- so a new mount can in principle flip an already-resolved
   pointer to raw, which is a regression in an ALREADY-MOUNTED level.

   READ THE BASELINES BEFORE THE NUMBERS. The two lanes measured this against
   DIFFERENT trees, neither of which is the merged one, so the counts below are
   not comparable to each other and neither is the merged tree's:
     - this lane, against a tree with the seventeen pre-ov044 rows: running the
       cross pass over the old maps and then over old + new, 156 rebased / 4620
       raw / 888 contested across 54 mounts becomes 202 / 5875 / 917 across 70.
       The 46 new patches are all the new overlays' own, and THE SET DIFFERENCE
       THE OTHER WAY IS EMPTY -- not one baseline patch was lost. (The same
       measurement over all 23 derived overlays, before the seven were dropped,
       was 217 / 6647 / 917 across 77 and also lost nothing, so none of the
       seven is blocked by the shared window either.)
     - the ov044/ov048 lane, against the same seventeen-row tree, did not run
       the counts but bounded its two by footprint instead: ov044 is
       0x021111a0..0x021116a0 and ov048 0x021111a0..0x02111900, both strictly
       inside windows ov009 (to 0x02113ee0) and ov016 (to 0x02114ea0) already
       contest, so neither can flip a resolved target raw.
   What carries across both is the SHAPE of the result, which is the part that
   answers the question: no mount on either side removed a baseline patch.

   THE MERGED TREE, measured on the integration lane rather than inferred from
   either lane's numbers: 208 rebased / 5944 raw / 940 contested across 74
   mounts. It is not 202 + anything, because the merge also brought in ov044
   and ov048 as whole mounts, ov044 again per symbol, and ov022 per symbol.
   The +6 over this lane's 202 is ov022's, which the build log attributes to
   ov022 by name; the two koopaN_boss arenas add no rebased pointer of their
   own, exactly as the ov044/ov048 footprint argument above predicts. Every
   count moved UP and none moved down, which is the shape both lanes measured.

   ov030 is the one that needed asking: at 19168 bytes it is the LARGEST level
   overlay the port mounts and it extends the shared window to 0x02115ce0, past
   every prior mount's end -- including past both koopaN_boss arenas above,
   which are the two smallest. That extension is what this lane's measurement
   above covers, and it cost nothing.

   EACH ROW'S EVIDENCE, one line per level: the ROM reads, then the object
   overlays LoadOrUnloadObjectOverlays actually loads for that id.

     lvl  ov     LVL_Overlay   crs sub flg  stage            bmd /kcl /icg /icl
     16  ov024  0x021129b4   7  1  0x00  desert_land      0713/0711/0714/0715  NOT LANDED
     17  ov025  0x02112bec   7  2  0x00  desert_py        071b/0716/071c/071d
     18  ov026  0x02112f58   8  2  0x00  water_land       07f5/07f3/07f6/07f7
     19  ov027  0x021130cc   9  1  0x00  snow_land        07ba/07b8/07bb/07bc
     20  ov028  0x021113d0   9  1  0x00  snow_kama        07b5/07b3/07b6/07b7
     21  ov029  0x02112fd0  10  2  0x00  water_city       07f0/07ed/07f1/07f2  NOT LANDED
     22  ov030  0x02114ea8  11  1  0x00  high_mt          0752/0750/0753/0754
     23  ov031  0x0211155c  11  1  0x00  high_slider      0757/0755/0758/0759
     24  ov032  0x02112f7c  12  1  0x00  tibi_deka_d      07de/07dc/07df/07e0
     25  ov033  0x02111be0  12  1  0x00  tibi_deka_t      07ea/07e8/07eb/07ec  NOT LANDED
     26  ov034  0x021140f8  12  1  0x01  tibi_deka_in     07e5/07e1/07e6/07e7
     27  ov035  0x021120bc  13  1  0x00  clock_tower      070e/070c/070f/0710  NOT LANDED
     28  ov036  0x02112a6c  14  1  0x01  rainbow_cruise   07ab/07a9/07ac/07ad  NOT LANDED
     31  ov039  0x02111438  23  1  0x00  habatake         074d/074b/074e/074f  NOT LANDED
     33  ov041  0x0211192c  25  1  0x00  metal_switch     07a1/079f/07a2/07a3  NOT LANDED
     35  ov043  0x02111b44  15  1  0x00  koopa1_map       0772/076f/0773/0774
     39  ov047  0x02111a58  17  1  0x00  koopa3_map       0792/078a/0793/0794
     44  ov052  0x02111cc8  18  1  0x00  ex_m_map         0733/0731/0734/0735
     45  ov053  0x021124f8  18  1  0x01  ex_mario         072e/072c/072f/0730
     47  ov055  0x02111994  19  1  0x00  ex_luigi         0720/071e/0721/0722
     48  ov056  0x02112c4c  20  1  0x00  ex_w_map         073d/073b/073e/073f
     49  ov057  0x02111520  20  1  0x00  ex_wario         0738/0736/0739/073a
     50  ov058  0x02111768  29  3  0x00  playroom         07a6/07a4/07a7/07a8

   THE OBJECT OVERLAYS, read from LoadOrUnloadObjectOverlays rather than
   guessed. The arm9 walks seven selector columns -- data_02075998[level][i]
   picks data_02075804[i][v] -- and only THEN takes either the 0x24/0x26/0x28
   short-circuit (ov060, early return, no ov098/ov102) or the ordinary
   ov098 + ov102 tail. Both halves run for the short-circuit ids, so levels 36,
   38 and 40 load ov089 AS WELL AS ov060.

   BOTH LANES MEASURED THIS INDEPENDENTLY AND AGREE, which is worth saying
   because it is the one mechanism the two blocks share: the ov044 block above
   reaches the same conclusion for level 36 from that level's own object table,
   and the ov046 block reaches it for level 38 from the selector row
   [0,0,0,0,1,0,0]. It is also a trap with a history -- an earlier lane named
   ov060 alone for level 38, missing ov089 and the boss-reward spawns (KEY 282,
   LAST_STAR 283) with it. That was caught in review and corrected before
   either of these lanes branched, so the level-38 block above already reads
   correctly; the note survives here because the short-circuit still LOOKS like
   it replaces the selector loop and does not.

   The reader reproduces level 3's already documented roster (ov063 ov085 ov089
   ov098 ov100 ov102) exactly. For the
   23 ids derived, with the packs this build does NOT mount called out --
   those unmounted packs are exactly why these levels' skip lists are long, and
   they are the worklist the cast lanes take from here. The seven marked NOT
   LANDED are derived and proven-mountable but held back; see the block after
   this list:

     16 ov062 ov070 ov080 ov084 ov092 ov096 ov098 ov102   NOT LANDED  [unmounted ov092 ov096]
     17 ov066 ov070 ov084 ov091 ov098 ov102              [unmounted ov066]
     18 ov064 ov070 ov079 ov084 ov090 ov094 ov098 ov100 ov102  [unmounted ov090]
     19 ov064 ov070 ov081 ov084 ov094 ov098 ov102        [all mounted]
     20 ov081 ov084 ov098 ov102                          [all mounted]
     21 ov062 ov070 ov077 ov084 ov090 ov098 ov102   NOT LANDED  [unmounted ov077 ov090]
     22 ov062 ov070 ov080 ov084 ov091 ov094 ov098 ov100 ov102  [all mounted]
     23 ov098 ov102                                      [all mounted]
     24 ov062 ov070 ov077 ov084 ov091 ov094 ov098 ov100 ov102  [unmounted ov077]
     25 ov062 ov070 ov084 ov094 ov098 ov100 ov102   NOT LANDED  [all mounted]
     26 ov070 ov084 ov098 ov102                          [all mounted]
     27 ov065 ov070 ov077 ov084 ov091 ov098 ov102   NOT LANDED  [unmounted ov077]
     28 ov062 ov070 ov077 ov084 ov091 ov095 ov098 ov102   NOT LANDED  [unmounted ov077]
     31 ov084 ov098 ov102   NOT LANDED  [all mounted]
     33 ov065 ov084 ov098 ov102   NOT LANDED  [all mounted]
     35 ov070 ov084 ov091 ov095 ov098 ov102              [all mounted]
     39 ov062 ov070 ov079 ov084 ov091 ov095 ov098 ov102  [all mounted]
     44 ov064 ov077 ov084 ov091 ov098 ov102              [unmounted ov077]
     45 ov067 ov074 ov084 ov089 ov098 ov102              [unmounted ov067 ov074]
     47 ov063 ov085 ov089 ov098 ov102                    [all mounted]
     48 ov064 ov081 ov084 ov091 ov098 ov102              [all mounted]
     49 ov064 ov073 ov089 ov098 ov102                    [unmounted ov073]
     50 ov080 ov085 ov089 ov098 ov100 ov102              [all mounted]

   THE SEVEN MARKED "NOT LANDED", and what actually blocks each. Every one of
   the seven mounts CORRECTLY -- each loads its stage, spawns its cast and
   prints a census. What kills them is downstream of the mount, in a class this
   build already registers, so none of them is mount work and none is in this
   lane. Isolated with SM64DS_SKIP_CLASS, which is the measurement rather than
   the fix:

     FLAME_CHOMP (id 270) faults in its Behavior -- port_actor_tick ->
     func_02043fdc+0x32 -> a call through a null slot (fault at +0xffc00000
     accessing 0). It blocks levels 16, 21, 25, 27 and 28. Each of the five
     boots and runs 300 frames clean with SM64DS_SKIP_CLASS=FLAME_CHOMP, which
     is what says the mount is right and the class is not.

     AMP (id 266) faults the same way, but only on levels 27 and 28. Level 21
     spawns four of them and ticks them fine, so it is DATA-dependent, not a
     dead vtable slot -- the two levels that fault differ from the one that
     does not in the actors' own object-table parameters. Worth knowing before
     someone "fixes" the class by filling a slot that is already filled.

     SNUFIT (id 236) faults in RENDER rather than tick -- port_actor_render ->
     Model::Virtual10+0xc, accessing 0. It blocks level 33 alone, which runs
     300 frames clean with SM64DS_SKIP_CLASS=SNUFIT.

     Level 31 (habatake) is the one with no class to name. It faults on FRAME
     ZERO inside the PLAYER's own render -- hal_render_player_world ->
     ModelAnim::Virtual18 -> ModelRenderFace::Render -> ModelComponents::Render
     -> func_02045074 -> func_02044b30+0x25c -> func_0204488c+0x10a, accessing
     0x134, with no actor in the walker at all. Its whole cast is furniture
     (QUESTION_BLOCK, STAR_MARKER, BOB_OMB_BUDDY, COIN, RED_COIN, CAMERA, HUD,
     MINIMAP) and the level's own geometry loads. The player enters that level
     with anim len 245760, an order of magnitude above the other levels' entry
     anims, which is where to start. A player-model gate, not a mount one.

   An unmounted pack costs the level its classes, not its boot: the pre-spawn
   gate turns them away BY NAME and the census prints them, which is what
   makes each new level's skip list a usable worklist. All sixteen are mounted
   --whole and own_sinits stays 0; none of them gets a per-symbol mount here,
   because filling their casts is a different lane. */

void port_ov025_patch(void);
void *port_ov025_at(unsigned ds);
extern unsigned char port_ov025_image[];
extern const unsigned port_ov025_ds_base, port_ov025_ds_end;

void port_ov026_patch(void);
void *port_ov026_at(unsigned ds);
extern unsigned char port_ov026_image[];
extern const unsigned port_ov026_ds_base, port_ov026_ds_end;

void port_ov027_patch(void);
void *port_ov027_at(unsigned ds);
extern unsigned char port_ov027_image[];
extern const unsigned port_ov027_ds_base, port_ov027_ds_end;

void port_ov028_patch(void);
void *port_ov028_at(unsigned ds);
extern unsigned char port_ov028_image[];
extern const unsigned port_ov028_ds_base, port_ov028_ds_end;


void port_ov030_patch(void);
void *port_ov030_at(unsigned ds);
extern unsigned char port_ov030_image[];
extern const unsigned port_ov030_ds_base, port_ov030_ds_end;

void port_ov031_patch(void);
void *port_ov031_at(unsigned ds);
extern unsigned char port_ov031_image[];
extern const unsigned port_ov031_ds_base, port_ov031_ds_end;

void port_ov032_patch(void);
void *port_ov032_at(unsigned ds);
extern unsigned char port_ov032_image[];
extern const unsigned port_ov032_ds_base, port_ov032_ds_end;


void port_ov034_patch(void);
void *port_ov034_at(unsigned ds);
extern unsigned char port_ov034_image[];
extern const unsigned port_ov034_ds_base, port_ov034_ds_end;

/* run linkw wave 17 (lane w17): level 27, Tick Tock Clock. */
void port_ov035_patch(void);
void *port_ov035_at(unsigned ds);
extern unsigned char port_ov035_image[];
extern const unsigned port_ov035_ds_base, port_ov035_ds_end;


void port_ov043_patch(void);
void *port_ov043_at(unsigned ds);
extern unsigned char port_ov043_image[];
extern const unsigned port_ov043_ds_base, port_ov043_ds_end;

void port_ov047_patch(void);
void *port_ov047_at(unsigned ds);
extern unsigned char port_ov047_image[];
extern const unsigned port_ov047_ds_base, port_ov047_ds_end;

void port_ov052_patch(void);
void *port_ov052_at(unsigned ds);
extern unsigned char port_ov052_image[];
extern const unsigned port_ov052_ds_base, port_ov052_ds_end;

void port_ov053_patch(void);
void *port_ov053_at(unsigned ds);
extern unsigned char port_ov053_image[];
extern const unsigned port_ov053_ds_base, port_ov053_ds_end;

void port_ov055_patch(void);
void *port_ov055_at(unsigned ds);
extern unsigned char port_ov055_image[];
extern const unsigned port_ov055_ds_base, port_ov055_ds_end;

void port_ov056_patch(void);
void *port_ov056_at(unsigned ds);
extern unsigned char port_ov056_image[];
extern const unsigned port_ov056_ds_base, port_ov056_ds_end;

void port_ov057_patch(void);
void *port_ov057_at(unsigned ds);
extern unsigned char port_ov057_image[];
extern const unsigned port_ov057_ds_base, port_ov057_ds_end;

void port_ov058_patch(void);
void *port_ov058_at(unsigned ds);
extern unsigned char port_ov058_image[];
extern const unsigned port_ov058_ds_base, port_ov058_ds_end;

/* run linkw wave 21 (lane w21): the last six stage ids -- 16, 21, 25, 28, 31
   and 33. See the table block below. */
void port_ov024_patch(void);
void *port_ov024_at(unsigned ds);
extern unsigned char port_ov024_image[];
extern const unsigned port_ov024_ds_base, port_ov024_ds_end;

void port_ov029_patch(void);
void *port_ov029_at(unsigned ds);
extern unsigned char port_ov029_image[];
extern const unsigned port_ov029_ds_base, port_ov029_ds_end;

void port_ov033_patch(void);
void *port_ov033_at(unsigned ds);
extern unsigned char port_ov033_image[];
extern const unsigned port_ov033_ds_base, port_ov033_ds_end;

void port_ov036_patch(void);
void *port_ov036_at(unsigned ds);
extern unsigned char port_ov036_image[];
extern const unsigned port_ov036_ds_base, port_ov036_ds_end;

void port_ov039_patch(void);
void *port_ov039_at(unsigned ds);
extern unsigned char port_ov039_image[];
extern const unsigned port_ov039_ds_base, port_ov039_ds_end;

void port_ov041_patch(void);
void *port_ov041_at(unsigned ds);
extern unsigned char port_ov041_image[];
extern const unsigned port_ov041_ds_base, port_ov041_ds_end;

/* run linkw wave C (lane cat-levels): five of the ten ids wave 8 declined as
   "not stages" -- 0, 30, 32, 34 and 46. See the table block below. */
void port_ov008_patch(void);
void *port_ov008_at(unsigned ds);
extern unsigned char port_ov008_image[];
extern const unsigned port_ov008_ds_base, port_ov008_ds_end;

void port_ov038_patch(void);
void *port_ov038_at(unsigned ds);
extern unsigned char port_ov038_image[];
extern const unsigned port_ov038_ds_base, port_ov038_ds_end;

void port_ov040_patch(void);
void *port_ov040_at(unsigned ds);
extern unsigned char port_ov040_image[];
extern const unsigned port_ov040_ds_base, port_ov040_ds_end;

void port_ov042_patch(void);
void *port_ov042_at(unsigned ds);
extern unsigned char port_ov042_image[];
extern const unsigned port_ov042_ds_base, port_ov042_ds_end;

void port_ov054_patch(void);
void *port_ov054_at(unsigned ds);
extern unsigned char port_ov054_image[];
extern const unsigned port_ov054_ds_base, port_ov054_ds_end;

/* VS wiring lane: the four VS battle maps, the ROM's own list at
   data_ov075_0211c6ec = levels {51, 43, 29, 42} (port/slice_vs.txt section
   4). The "NOT a stage" verdict the wave-C block gave these ids is hereby
   CORRECTED for exactly these four: they carry name handles past the ROM's
   course-name space because VS maps have no course name or star select, not
   because no course is behind them. LVL_Overlay addresses read from the
   ROM's own data_02092208 (file offsets shifted by the 0x4000 ITCM block,
   with data_020758c8[3]=11, [38]=46 and data_02092208[4]=0x02111c54 as the
   three controls). */
void port_ov059_patch(void);
void *port_ov059_at(unsigned ds);
extern unsigned char port_ov059_image[];
extern const unsigned port_ov059_ds_base, port_ov059_ds_end;
void port_ov051_patch(void);
void *port_ov051_at(unsigned ds);
extern unsigned char port_ov051_image[];
extern const unsigned port_ov051_ds_base, port_ov051_ds_end;
void port_ov037_patch(void);
void *port_ov037_at(unsigned ds);
extern unsigned char port_ov037_image[];
extern const unsigned port_ov037_ds_base, port_ov037_ds_end;
void port_ov050_patch(void);
void *port_ov050_at(unsigned ds);
extern unsigned char port_ov050_image[];
extern const unsigned port_ov050_ds_base, port_ov050_ds_end;
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
    {37, "Bowser in the Fire Sea (koopa2_map, course 16)", "ov045", 0x021124d4,
     port_ov045_patch, port_ov045_at,
     &port_ov045_ds_base, &port_ov045_ds_end, 0},
    {38, "Bowser fight arena (koopa2_boss, course 16)", "ov046", 0x02111560,
     port_ov046_patch, port_ov046_at,
     &port_ov046_ds_base, &port_ov046_ds_end, 0},
    {36, "Bowser in the Dark World arena (koopa1_boss, course 15)", "ov044",
     0x021113b0, port_ov044_patch, port_ov044_at,
     &port_ov044_ds_base, &port_ov044_ds_end, 0},
    {40, "Bowser in the Sky arena (koopa3_boss, course 17)", "ov048",
     0x02111624, port_ov048_patch, port_ov048_at,
     &port_ov048_ds_base, &port_ov048_ds_end, 0},
    /* run linkw wave 8, lane w8-mounts -- the sixteen rows the block above
       derives AND boots, table indices 19..34. Order is level id ascending
       WITHIN this block; the table as a whole is append-by-wave, not sorted
       (levels 36 and 40 above landed a wave earlier and keep 17 and 18). The
       table is searched by id, not indexed by it, so the order is only for
       reading -- but port_level_mount_fns[] IS positional, so the thunks below
       are numbered off these positions and not off the lane's original ones. */
    {17, "Shifting Sand pyramid (desert_py, course 7)", "ov025", 0x02112bec,
     port_ov025_patch, port_ov025_at,
     &port_ov025_ds_base, &port_ov025_ds_end, 0},
    {18, "Dire Dire Docks (water_land, course 8)", "ov026", 0x02112f58,
     port_ov026_patch, port_ov026_at,
     &port_ov026_ds_base, &port_ov026_ds_end, 0},
    {19, "Snowman's Land (snow_land, course 9)", "ov027", 0x021130cc,
     port_ov027_patch, port_ov027_at,
     &port_ov027_ds_base, &port_ov027_ds_end, 0},
    {20, "Snowman's Land igloo (snow_kama, course 9)", "ov028", 0x021113d0,
     port_ov028_patch, port_ov028_at,
     &port_ov028_ds_base, &port_ov028_ds_end, 0},
    {22, "Tall Tall Mountain (high_mt, course 11)", "ov030", 0x02114ea8,
     port_ov030_patch, port_ov030_at,
     &port_ov030_ds_base, &port_ov030_ds_end, 0},
    {23, "Tall Tall Mountain slide (high_slider, course 11)", "ov031", 0x0211155c,
     port_ov031_patch, port_ov031_at,
     &port_ov031_ds_base, &port_ov031_ds_end, 0},
    {24, "Tiny-Huge Island, huge (tibi_deka_d, course 12)", "ov032", 0x02112f7c,
     port_ov032_patch, port_ov032_at,
     &port_ov032_ds_base, &port_ov032_ds_end, 0},
    {26, "Tiny-Huge Island cave (tibi_deka_in, course 12)", "ov034", 0x021140f8,
     port_ov034_patch, port_ov034_at,
     &port_ov034_ds_base, &port_ov034_ds_end, 0},
    {35, "Bowser in the Dark World (koopa1_map, course 15)", "ov043", 0x02111b44,
     port_ov043_patch, port_ov043_at,
     &port_ov043_ds_base, &port_ov043_ds_end, 0},
    {39, "Bowser in the Sky (koopa3_map, course 17)", "ov047", 0x02111a58,
     port_ov047_patch, port_ov047_at,
     &port_ov047_ds_base, &port_ov047_ds_end, 0},
    {44, "Mario's key course (ex_m_map, course 18)", "ov052", 0x02111cc8,
     port_ov052_patch, port_ov052_at,
     &port_ov052_ds_base, &port_ov052_ds_end, 0},
    {45, "Mario's key arena (ex_mario, course 18)", "ov053", 0x021124f8,
     port_ov053_patch, port_ov053_at,
     &port_ov053_ds_base, &port_ov053_ds_end, 0},
    {47, "Luigi's key arena (ex_luigi, course 19)", "ov055", 0x02111994,
     port_ov055_patch, port_ov055_at,
     &port_ov055_ds_base, &port_ov055_ds_end, 0},
    {48, "Wario's key course (ex_w_map, course 20)", "ov056", 0x02112c4c,
     port_ov056_patch, port_ov056_at,
     &port_ov056_ds_base, &port_ov056_ds_end, 0},
    {49, "Wario's key arena (ex_wario, course 20)", "ov057", 0x02111520,
     port_ov057_patch, port_ov057_at,
     &port_ov057_ds_base, &port_ov057_ds_end, 0},
    {50, "Rec Room (playroom, course 29)", "ov058", 0x02111768,
     port_ov058_patch, port_ov058_at,
     &port_ov058_ds_base, &port_ov058_ds_end, 0},
    /* run linkw wave 17 (lane w17): the thirty-sixth mount, and the first of
       the seven "NOT LANDED" ids above to land. Every field is read from the
       ROM, and the derivation was validated against rows 17, 22 and 26 first
       (all three reproduce their existing lvl_overlay, course, subCount, flags
       and all four handle ids exactly):
         data_020758c8[27]      = 35            -> ov035
         data_02092208[27]      = 0x021120bc    -> the LVL_Overlay
         SUBLEVEL_LEVEL_TABLE[27] (0x02075298)  = 0x0d = course 13
         LVL_Overlay+0x08..0x0e = 070e/070c/070f/0710  bmd/kcl/icg/icl
         LVL_Overlay+0x14/0x15  = subCount 1, flags 0x00
       Object overlays: ov065 ov070 ov077 ov084 ov091 ov098 ov102, of which
       only ov077 is unmounted -- its classes are skipped by name at the
       pre-spawn gate and the census names them. own_sinits 0. */
    {27, "Tick Tock Clock (clock_tower, course 13)", "ov035", 0x021120bc,
     port_ov035_patch, port_ov035_at,
     &port_ov035_ds_base, &port_ov035_ds_end, 0},
    /* run linkw wave 21 (lane w21): FIVE of the six ids the wave-8 block
       marked NOT LANDED and level 27 did not take. With them, forty-one of the
       ROM's forty-two stage ids are mounted; level 31 is the only one left and
       it is not mount work (below).

       Appended at the END, indices 36..40, for the reason the merge note above
       spells out: a row inserted mid-table renumbers every thunk below it and
       NOTHING in the build says so. Level 27 keeps index 35 and its thunk keeps
       lane w19's ov035/Ttc seat call, untouched by this lane. That trap now has
       a guard rather than a convention -- port/tools/mount_pairing_guard.py
       parses this file and asserts mount_fns[i] mounts row i for every i, and
       was proved on a deliberately mis-inserted copy before it was trusted.

       Derived by this lane from the ROM, not copied off the wave-8 block, and
       the reader proved against all thirty-six then-mounted rows before a new
       one was written (see the CMake block). Per row, the five reads:
         data_020758c8[id] -> ov, data_02092208[id] -> the LVL_Overlay,
         SUBLEVEL_LEVEL_TABLE[id] -> course, +0x08..0x0e -> the four handles,
         +0x14/+0x15 -> subCount/flags.

         16 ov024 0x021129b4 crs  7 sub 1 flg 0x00 desert_land    0713/0711/0714/0715
         21 ov029 0x02112fd0 crs 10 sub 2 flg 0x00 water_city     07f0/07ed/07f1/07f2
         25 ov033 0x02111be0 crs 12 sub 1 flg 0x00 tibi_deka_t    07ea/07e8/07eb/07ec
         28 ov036 0x02112a6c crs 14 sub 1 flg 0x01 rainbow_cruise 07ab/07a9/07ac/07ad
         33 ov041 0x0211192c crs 25 sub 1 flg 0x00 metal_switch   07a1/079f/07a2/07a3

       WHAT FREED THE FOUR, since no mount field changed: FLAME_CHOMP (270) and
       AMP (266) were the wave-8 blockers on 16, 21, 25 and 28, and both are
       fixed on this tip. All four now boot bare -- SM64DS_LEVEL=<id>
       SM64DS_FAULTS_FATAL=1 SM64DS_WINDOW_SELFTEST=300, rc 0, no skip list.

       LEVEL 33 IS THE ONE THAT STILL NEEDS A SKIP, and it is landed anyway
       because the mount is proven and the blocker is not this lane's. Bare it
       faults rc 139 at +0x0003c75c accessing 0, walker node actor id 0xec =
       236 = SNUFIT, in RENDER; with SM64DS_SKIP_CLASS=SNUFIT it runs 300
       frames clean, rc 0, census 76 spawned / 18 classes. That is the wave-8
       measurement reproduced exactly on this tip, so SNUFIT is unfixed here.
       port/tools/battery.py runs level 33 WITH that skip and says why, so the
       mount is covered rather than silently untested; delete the skip there
       when SNUFIT lands and the level goes green bare.

       LEVEL 31 (habatake, ov039 0x02111438, crs 23, sub 1, flg 0x00, handles
       074d/074b/074e/074f) IS DERIVED AND DELIBERATELY NOT HERE. Its mount is
       sound and measured so: the boot prints its own LVL_Overlay with bmd 1869
       / kcl 1867 -- 0x074d / 0x074b, the ROM's own words -- parses a 5-kind
       objTable and a 1-entry sublevel table, and loads the stage. What kills
       it is downstream of every actor: rc 139 at +0x000d9bda accessing 0x134
       on FRAME ZERO with NO actor in the walker, and it still faults with all
       seven of its non-player classes skipped
       (STAR_MARKER,BOB_OMB_BUDDY,COIN,RED_COIN,CAMERA,HUD,MINIMAP), which is
       what says it is the player's own render and not a cast problem. The
       player enters habatake mid-animation -- anim(len=245760 fl=1 cur=21.0),
       i.e. frame 21 of a 60-frame entry, against len=40960 fl=0 cur=1.0 on
       levels 13, 22 and 27 -- which fits the level (habatake is the Wing Cap
       tower, entered flying) and is where a player-model lane should start.
       A row for it is one line here, one thunk and one CMake token; nothing
       needs re-deriving.

       own_sinits stays 0 for all five; none gets a per-symbol mount here. */
    {16, "Shifting Sand Land (desert_land, course 7)", "ov024", 0x021129b4,
     port_ov024_patch, port_ov024_at,
     &port_ov024_ds_base, &port_ov024_ds_end, 0},
    {21, "Dire Dire Docks city (water_city, course 10)", "ov029", 0x02112fd0,
     port_ov029_patch, port_ov029_at,
     &port_ov029_ds_base, &port_ov029_ds_end, 0},
    {25, "Tiny-Huge Island, tiny (tibi_deka_t, course 12)", "ov033", 0x02111be0,
     port_ov033_patch, port_ov033_at,
     &port_ov033_ds_base, &port_ov033_ds_end, 0},
    {28, "Rainbow Cruise (rainbow_cruise, course 14)", "ov036", 0x02112a6c,
     port_ov036_patch, port_ov036_at,
     &port_ov036_ds_base, &port_ov036_ds_end, 0},
    {33, "Metal Cap switch course (metal_switch, course 25)", "ov041",
     0x0211192c, port_ov041_patch, port_ov041_at,
     &port_ov041_ds_base, &port_ov041_ds_end, 0},
    /* run linkw wave C (lane cat-levels): FIVE OF THE TEN IDS THE WAVE-8 BLOCK
       ABOVE DECLINED AS "NOT STAGES". They are stages, and the block's own test
       says so when it is re-run.

       Appended at the END, indices 41..45, for the reason w21's block gives:
       a row inserted mid-table renumbers every thunk below it. That is now
       checked rather than remembered -- port/tools/mount_pairing_guard.py
       parses this file and asserts mount_fns[i] mounts row i.

       WHAT THE WAVE-8 READING ACTUALLY CAUGHT. Its words are "LVL_Overlay
       records whose four handles do not resolve to one data/stage directory
       (they read MG/ minigame art or nothing), and courses 21..28 and 255".
       Re-run on this tree against build/assets/handles.tsv at delta 0, id by
       id, that splits in two and the smaller half is the one it describes:

         GENUINELY NOT STAGES, five:
           29, 42, 43, 51  name handles 0x9002/0x9402/0x8c02/0x8802, past the
                           end of the ROM's handle space -- they resolve to
                           nothing at all.
           41 test_map_b   bmd 0x07db is 356 bytes and kcl 0x07da is 145, and
                           its icg/icl handles are BOTH 0. Handle 0 is
                           MG/casino_back.bmd, so this one id is the whole of
                           "reads MG/ minigame art" -- and it reads that way
                           because its handles are NULL, not because the record
                           points at a minigame.

         STAGES, five, every one a full four-file set at delta 0:
            0 ov008 test_map       18916/16386/7784/512 bytes
           30 ov038 suisou         6262/5231/13320/512
           32 ov040 horisoko       14622/16782/11304/512
           34 ov042 rainbow_mario  34011/51896/10048/512
           46 ov054 ex_l_map       27922/18362/10344/512

       THE COURSE-BAND HALF OF THE ARGUMENT WAS ALREADY DEAD when this lane
       found it: courses 21..28 are the secret stages, and w21 mounted level 33
       metal_switch at COURSE 25 out of that same band, four rows above here.
       30 suisou is the Secret Aquarium, 32 horisoko is Vanish Cap Under the
       Moat, 34 rainbow_mario is Wing Mario Over the Rainbow -- 22, 24 and 26
       of that band, siblings of 33 and of 31 habatake at 23. Course 255 is one
       id, test_map, and it is a stage with a stage's files.

       46 IS NOT EVEN IN THE BAND. It is course 19, and course 19's other half,
       47 ex_luigi, has been mounted since wave 8. The key courses run
       {course map, arena} per character -- 44 ex_m_map + 45 ex_mario at 18, 48
       ex_w_map + 49 ex_wario at 20 -- and 46 ex_l_map is Luigi's course map,
       the one missing corner of that grid.

       Fields read exactly as the recipe above says, with the reader proved
       first against ALL FORTY-ONE rows already in this table, every field:
       overlay id, LVL_Overlay address, course, subCount, flags and all four
       handles, 41/41 with no disagreement. Only then were these five written.

         lvl  ov     LVL_Overlay  crs sub flg  stage
           0  ov008  0x02111210  255  1  0x01  test_map
          30  ov038  0x02111218   22  1  0x00  suisou
          32  ov040  0x02111228   24  1  0x01  horisoko
          34  ov042  0x02111218   26  1  0x00  rainbow_mario
          46  ov054  0x021112b0   19  5  0x00  ex_l_map

       LEVEL 0 IS CUT CONTENT, and it is mounted for the same reason as the
       rest: it is in the ROM's own level table with a complete stage behind it.
       Course 255 is the ROM's own "no course", which is what a test map would
       carry.

       Levels 30 and 34 share an LVL_Overlay ADDRESS, 0x02111218. That is not a
       collision: every level overlay loads at the same DS base 0x021111a0, so
       two different overlays can put their records at the same offset. The rows
       differ in the overlay they name, and port_level_mounts_install asserts
       each row's address against data_02092208[id] on the level's own image.

       own_sinits stays 0 for all five; none gets a per-symbol mount here. */
    {0, "developer test map (test_map, no course)", "ov008", 0x02111210,
     port_ov008_patch, port_ov008_at,
     &port_ov008_ds_base, &port_ov008_ds_end, 0},
    {30, "The Secret Aquarium (suisou, course 22)", "ov038", 0x02111218,
     port_ov038_patch, port_ov038_at,
     &port_ov038_ds_base, &port_ov038_ds_end, 0},
    {32, "Vanish Cap Under the Moat (horisoko, course 24)", "ov040",
     0x02111228, port_ov040_patch, port_ov040_at,
     &port_ov040_ds_base, &port_ov040_ds_end, 0},
    {34, "Wing Mario Over the Rainbow (rainbow_mario, course 26)", "ov042",
     0x02111218, port_ov042_patch, port_ov042_at,
     &port_ov042_ds_base, &port_ov042_ds_end, 0},
    {46, "Luigi's key course (ex_l_map, course 19)", "ov054", 0x021112b0,
     port_ov054_patch, port_ov054_at,
     &port_ov054_ds_base, &port_ov054_ds_end, 0},
    /* VS wiring lane: the ROM's four VS battle maps, list order (map 1..4 =
       levels 51, 43, 29, 42). Named by list position until an asset-derived
       name lands; the id is what a bug report quotes. */
    {51, "VS map 1 (level 51)", "ov059", 0x02111548,
     port_ov059_patch, port_ov059_at,
     &port_ov059_ds_base, &port_ov059_ds_end, 0},
    {43, "VS map 2 (level 43)", "ov051", 0x0211150c,
     port_ov051_patch, port_ov051_at,
     &port_ov051_ds_base, &port_ov051_ds_end, 0},
    {29, "VS map 3 (level 29)", "ov037", 0x021111ec,
     port_ov037_patch, port_ov037_at,
     &port_ov037_ds_base, &port_ov037_ds_end, 0},
    {42, "VS map 4 (level 42)", "ov050", 0x02111204,
     port_ov050_patch, port_ov050_at,
     &port_ov050_ds_base, &port_ov050_ds_end, 0},
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
/* CAPTURED: it names which level the WORLD is, so it has to move with the
   world a restore puts back. -1 stays the "not set" sentinel either way. */
DSSTATE_BEGIN
static int g_boot_target = -1;
DSSTATE_END

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

/* STATIC_ROCK (id 61, x33 -- JRB's biggest class) reads its CLPS collision block
   out of the ov102 factory table data_ov102_0214e190[idx*0xc]; idx 0 is
   STATIC_ROCK and that word points at DS 0x02113ccc, a "CLPS" record inside the
   loaded level overlay. That overlay base (0x021111a0) is shared by eleven level
   overlays, so the pointer is ambiguous cross-mount and ovdata.py --cross keeps
   it RAW rather than guess a level (see port/tools/ovdata.py, "AN AMBIGUOUS DS
   RANGE IS DROPPED, NOT GUESSED AT"). Left raw, MeshCollider::GetSurfaceInfo ->
   func_020381cc dereferences a DS address the host never mapped -- the SIG-CLSN
   crash. STATIC_ROCK only spawns in Jolly Roger Bay, so the level that owns that
   block is always ov016: rebase the one word to ov016's mounted copy right after
   ov016's whole-mount patch runs, per JRB mount, the ov089_keymodels_fixup
   pattern. The other two idx-0 table words (0214e188/0214e18c, model + KCL
   fileptrs) target ov102 itself and are rebased by the normal per-mount pass. */
extern "C" unsigned char data_ov102_0214e190[];
static void port_jrb_staticrock_clps_seat(void)
{
    unsigned *w = (unsigned *)(data_ov102_0214e190 + 0);
    if (*w == 0x02113ccc)
        *w = (unsigned)(size_t)port_ov016_at(0x02113ccc);
    else if (*w < 0x02000000u || *w >= 0x02400000u)
        return;   /* already host-rebased (idempotent re-entry) */
    else {
        std::fprintf(stderr, "FATAL: STATIC_ROCK CLPS seat: data_ov102_"
                     "0214e190[0] holds %08x, ROM says 02113ccc -- WRONG "
                     "BYTES\n", *w);
        std::abort();
    }
}

/* IDEMPOTENT PER LEVEL, and gate 31 is why. d->patch() rewrites the overlay
   image's own pointers in place, which is not something that can be done
   twice: a second pass would rebase already-rebased words. The cache was an
   optimisation while a run only ever mounted one level. With the handoff
   calling the mount again on every entry it is load-bearing, and it needs one
   slot PER LEVEL rather than one slot overall: a session that goes 1 -> 6 -> 1
   must get the first mount of each back rather than a re-patch of either. Any
   mount registered with port_level_mount_register owes the same guarantee. */
extern "C" void port_sqrt_selftest(void);   /* hal/scene_boot.cpp */
extern "C" void port_stage_mod_apply(int level_id, void *lvl);  /* hal/stage_mods.cpp */

/* CAPTURED, and this is the whole of the cross-level reload crash.
 *
 * The cache above describes ONE THING: whether this level's overlay image has
 * been patched. The image IS .dsstate -- tools/ovdata.py routes every mounted
 * byte and every synthetic gap block into the captured section on purpose --
 * and d->patch() writes into it. So a save-state restore rolls the IMAGE back
 * and, while this array was a plain host static, left the CACHE alone. The two
 * descriptions of the same fact then disagreed, in the one direction that is
 * fatal: the cache says "patched", the bytes say "raw".
 *
 * WHAT THAT COST, measured rather than argued (RELOAD2's four-arm A/B, all
 * from one binary, restore held fixed and only the warp destination varied):
 *
 *   warp to the level THIS PROCESS booted, after restoring a state whose
 *   session never mounted it   -> cache HIT on a rolled-back image
 *                                 FAULT c0000005 accessing 000000cc
 *   warp to a level neither side ever mounted -> cache miss, real patch, clean
 *   warp to a level the SAVED session mounted but this process did not
 *                              -> cache miss on an already-patched image, and
 *                                 clean (the generated passes are pure
 *                                 absolute assignment, so a second one writes
 *                                 the same words -- checked, not assumed)
 *   no restore, the same self-warp (control)  -> clean, two object loads
 *
 * The fault is the minimap's. Reading a raw DS pointer out of a rolled-back
 * overlay does not fault, because ntr/io.cpp reserves the DS address space
 * zero-filled -- so LVL_Overlay's objTable and sub-table words read as NULL,
 * Stage::LoadClsnAndObjects loads NO objects (SM64DS_TRACE_LOADERS prints not
 * one line for that boot), LoadEntranceObjects therefore never spawns a
 * Player, data_0209f394[data_0209f250] stays 0, and Minimap::InitResources'
 * GetMinimapID(data_0209f394[data_0209f250], -1) reads obj->+0xcc through it.
 * Every symptom is downstream of the one silent skipped patch.
 *
 * ROLL THE FLAG WITH THE FACT, rather than clearing it or re-running the pass.
 * Clearing the cache after a restore would force a re-patch of the image the
 * restore just handed back ALREADY patched. Re-running every pass unguarded
 * relies on all of them being idempotent, which is not a property the mount
 * registry can promise for a mount nobody has written yet. Bracketing is the
 * one option under which the flag and the bytes it describes cannot disagree,
 * whichever way the restore moves them -- which is the same argument, in the
 * same words, that hal/lk7_persist.cpp makes for the host file-handle table.
 *
 * THE RULE THIS AND EVERY RULING LIKE IT COMES FROM, written once here because
 * it has been everywhere implied and nowhere stated: A ONE-SHOT GUARD BELONGS
 * ON THE SAME SIDE OF THE CAPTURED SECTION AS THE WORK IT GUARDS. Same side and
 * the two roll back together and can never disagree, whichever way a restore
 * moves them; opposite sides and a restore moves one without the other, which
 * is the defect above in its general form. That is the whole question each
 * guard is asked and the only one: this array and the eighteen level-path
 * bring-ups host their subjects in .dsstate and are bracketed with them, while
 * port_level_mounts_install below guards the host mount REGISTRY and stays
 * outside it. Same shape, opposite answers, one test.
 *
 * AND THE RULE ERRS SAFE ONLY BECAUSE OF A SECOND FACT, which is checked per
 * pass rather than assumed: every bracketed pass is RE-RUNNABLE. pack_check,
 * syms_patch, the __sinit_* bodies and the vtable fills are idempotent on their
 * own image, they allocate nothing on the host and they register nothing with
 * the host, so the worst a bracket placed wrongly can cost is a redundant
 * re-patch -- never a doubled effect. A pass that gains a host allocation or a
 * host registration leaves that set in principle and has to be ruled on its own
 * terms.
 *
 * The four scene-path passes are host-side for a different and simpler reason,
 * and theirs is the one that governs: a SCENE run cannot meet a save state at
 * all, because main() hands the whole process to port_scene_run before the
 * level bring-up and therefore before lk7_persist_read and before the frame
 * loop that owns F8/F9. Bracketing them would spend .dsstate bytes insuring
 * against a shape that cannot occur. The argument is written at the site, in
 * hal/scene_mg_menu.cpp. port_scene_fill_mgm also registers an atexit handler,
 * but that is a second remark there rather than the ruling, and its own `armed`
 * static latches it separately from `done` -- so bracketing `done` would not
 * have re-registered it either.
 *
 * The cells hold HOST addresses into the mounted images, which are image
 * addresses and not heap or stack, so lk6's cross-process landmine scan
 * ([ss-scan]) is untouched by this: it counts words pointing at THIS process's
 * heap or stack and these are neither. */
DSSTATE_BEGIN
static void *g_level_mounted[PORT_LEVEL_COUNT];
DSSTATE_END

/* The fix-off half of the A/B, so one binary answers both arms.
 * SM64DS_SS_NO_ROLLGUARD=1 makes lk6's load put these host-side bytes back
 * after the section copy, which is exactly the pre-fix behaviour: the flags
 * survive a restore that rolls their subject back. Host storage by
 * construction -- outside the bracket, so the stash itself never rolls. */
static void *g_level_mounted_stash[PORT_LEVEL_COUNT];

extern "C" void port_mount_cache_stash(void)
{
    std::memcpy(g_level_mounted_stash, g_level_mounted,
                sizeof g_level_mounted);
}

extern "C" void port_mount_cache_unstash(void)
{
    std::memcpy(g_level_mounted, g_level_mounted_stash,
                sizeof g_level_mounted);
}

/* SM64DS_TRACE_MOUNT=1: one line per mount call saying whether the cache
   answered or the patch pass ran. This is the instrument the cross-level reload
   crash needed and did not have -- with the cache and the image described in
   two places, "did the patch run for this entry" was only ever inferable from
   the absence of [load] lines three layers downstream. */
static int port_mount_trace(void)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_TRACE_MOUNT") != 0;
    return on;
}

static void *port_level_mount_at(int idx)
{
    void **mounted = g_level_mounted;
    if (mounted[idx]) {
        if (port_mount_trace())
            std::fprintf(stderr, "[mount] level %d: CACHE HIT %p (the patch "
                         "pass does not run)\n", port_level_table[idx].id,
                         mounted[idx]);
        return mounted[idx];
    }
    if (port_mount_trace())
        std::fprintf(stderr, "[mount] level %d: PATCHING (%s)\n",
                     port_level_table[idx].id, port_level_table[idx].overlay);
    /* Run mg15 lane SQRT. The square-root self-test is idempotent and default
       off. It is called here as well as from port_scene_begin because a LEVEL
       run never enters the scene path, and the level arm of that lane's
       regression came back with no self-test line -- which reads exactly like a
       self-test that passed. The in-level engine is where func_02053274's two
       Player-state callers live, so the level arm is the one that matters most. */
    port_sqrt_selftest();
    const PortLevelDesc *d = &port_level_table[idx];
    d->patch();
    /* JRB owns STATIC_ROCK's cross-mount CLPS pointer; seat it now that ov016's
       image is mounted and port_ov016_at can resolve the DS address to host. */
    if (d->patch == port_ov016_patch)
        port_jrb_staticrock_clps_seat();
    void *lvl = d->at(d->lvl_overlay);
    if (!lvl) {
        std::fprintf(stderr, "FATAL: %s mount: 0x%08x outside the overlay "
                     "[0x%08x, 0x%08x)\n", d->overlay, d->lvl_overlay,
                     *d->ds_base, *d->ds_end);
        std::abort();
    }
    mounted[idx] = lvl;
    /* THE LEVEL-EDITS MOD, hal/stage_mods.cpp, and this is the only line of it
       that lives here. The window is exactly this wide: after d->patch(), so
       the LVL_Overlay's objTable and sub-table words are host pointers rather
       than DS addresses, and before anything has read them -- the object
       tables are walked later and once, by Stage::LoadClsnAndObjects in
       matched src. Off (no SM64DS_STAGE_MOD) it reads one env var and
       returns. */
    port_stage_mod_apply(d->id, lvl);
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
/* Level 37's row is table index 15. Named by level, the port_mount_row_lvl14
   convention, so a sibling stream appending its own level does not collide. */
static void *port_mount_row_lvl37(void) { return port_level_mount_at(15); }
/* Level 38's row is table index 16, the same append-only convention. */
static void *port_mount_row_lvl38(void) { return port_level_mount_at(16); }
/* Levels 36 and 40 are table indices 17 and 18 -- the other two koopaN_boss
   arenas. Named by level, the same append-only convention, so a sibling stream
   appending its own level against this base does not collide. */
static void *port_mount_row_lvl36(void) { return port_level_mount_at(17); }
static void *port_mount_row_lvl40(void) { return port_level_mount_at(18); }
/* run linkw wave 8, lane w8-mounts: the sixteen new rows, table indices
   19..34 in the same order the table lists them. Named by LEVEL like the three
   above rather than by index, so a merge that renumbers the table does not
   silently point a thunk at a different level -- the index is in exactly one
   place per row and the name says which row it is meant to be.

   THE RENUMBER THIS MERGE ACTUALLY NEEDED, recorded because the naming
   convention above is what caught it. The lane wrote these sixteen as indices
   17..32 against a base where level 38 was the last row; levels 36 and 40
   landed on the tip first and took 17 and 18, so every one of the sixteen
   moved up by two. Nothing in the build would have said so: the indices are
   in range either way, port_level_mounts_install pairs port_level_table[i]
   with port_level_mount_fns[i] positionally, and its only assertion compares
   the ROW's lvl_overlay against the ROM -- never the thunk. Left at 17..32
   these would have registered level 17's id against level 36's mount and run
   the whole table two rows out of step. */
static void *port_mount_row_lvl17(void) { return port_level_mount_at(19); }
static void *port_mount_row_lvl18(void) { return port_level_mount_at(20); }
static void *port_mount_row_lvl19(void) { return port_level_mount_at(21); }
static void *port_mount_row_lvl20(void) { return port_level_mount_at(22); }
static void *port_mount_row_lvl22(void) { return port_level_mount_at(23); }
static void *port_mount_row_lvl23(void) { return port_level_mount_at(24); }
static void *port_mount_row_lvl24(void) { return port_level_mount_at(25); }
static void *port_mount_row_lvl26(void) { return port_level_mount_at(26); }
static void *port_mount_row_lvl35(void) { return port_level_mount_at(27); }
static void *port_mount_row_lvl39(void) { return port_level_mount_at(28); }
static void *port_mount_row_lvl44(void) { return port_level_mount_at(29); }
static void *port_mount_row_lvl45(void) { return port_level_mount_at(30); }
static void *port_mount_row_lvl47(void) { return port_level_mount_at(31); }
static void *port_mount_row_lvl48(void) { return port_level_mount_at(32); }
static void *port_mount_row_lvl49(void) { return port_level_mount_at(33); }
static void *port_mount_row_lvl50(void) { return port_level_mount_at(34); }
/* run linkw wave 17 (lane w17): level 27, appended at the END of the table
   on purpose -- a new row inserted mid-table renumbers every thunk below it
   and nothing in the build would say so (the merge note above). */
/* run linkw wave 18 (lane w18): level 27 is the only level the ov065 Ttc
   classes can run on, which is the disambiguation ovdata.py's cross pass is
   missing -- it drops these targets because each lands inside EIGHTEEN
   mounted overlay windows. Seating from here closes ov065's eight
   level-window reads onto ov035's own copy and verifies the storage each
   one now names. See hal/ttc_level_data_seat.cpp and port/ov035_syms.txt. */
extern "C" void port_ttc_level_data_seat(void);
static void *port_mount_row_lvl27(void)
{
    void *p = port_level_mount_at(35);
    port_ttc_level_data_seat();
    return p;
}
/* run linkw wave 21 (lane w21): five more stage ids, table indices 36..40 in
   the order the table lists them. Named by level like every thunk since wave 8,
   so a merge that renumbers the table cannot silently point one of these at
   another level's row -- and port/tools/mount_pairing_guard.py now checks that
   by parsing this file rather than leaving it to the eye. Index 35 and
   port_mount_row_lvl27 above are untouched: lane w19's ov035/Ttc seat call
   hangs off that thunk. */
static void *port_mount_row_lvl16(void) { return port_level_mount_at(36); }
static void *port_mount_row_lvl21(void) { return port_level_mount_at(37); }
static void *port_mount_row_lvl25(void) { return port_level_mount_at(38); }
static void *port_mount_row_lvl28(void) { return port_level_mount_at(39); }
static void *port_mount_row_lvl33(void) { return port_level_mount_at(40); }
/* run linkw wave C (lane cat-levels): the five recovered stages, table indices
   41..45 in the order the table lists them. Named by level like every thunk
   since wave 8, and port/tools/mount_pairing_guard.py checks the pairing. */
static void *port_mount_row_lvl0(void) { return port_level_mount_at(41); }
static void *port_mount_row_lvl30(void) { return port_level_mount_at(42); }
static void *port_mount_row_lvl32(void) { return port_level_mount_at(43); }
static void *port_mount_row_lvl34(void) { return port_level_mount_at(44); }
static void *port_mount_row_lvl46(void) { return port_level_mount_at(45); }
/* VS wiring lane: the four VS battle maps, table rows appended at the END
   (the merge-note rule above). Positional: row N takes fns[N].

   AND THAT IS THE ONE RULE THESE FOUR BROKE, fixed here (run rel0215, lane
   gatefix). They shipped mounting 35, 36, 37 and 38 -- the rows of levels 27,
   16, 21 and 25 (Tick Tock Clock ov035, Shifting Sand Land ov024, Dire Dire
   Docks city ov029, Tiny-Huge tiny ov033), each of which already has its own
   thunk above. Their own rows are 46..49.

   NOT A MERGE RENUMBER, which is the failure the comment block above predicts
   and the one a reader would assume. Checked against the parent of the commit
   that added them (110188b9a^): the four VS rows had ALREADY landed at 46..49
   in an earlier commit, so the table these were written against is the table
   they are in. The indices were simply wrong when typed, and the comment above
   them stated the rule they broke.

   WHAT IT COST, so the next reader does not have to re-derive it.
   port_level_mounts_install pairs table[i] with mount_fns[i] positionally, so
   the mount REGISTRY (hal/level_change.cpp) held, for VS level 51, a function
   that mounts Tick Tock Clock and runs port_ttc_level_data_seat() with it. The
   registry's reader is port_level_overlay(level), and its live caller is
   port_level_capture_kcl, which resolves the OUTGOING level's LVL_Overlay
   during a level change: leaving a VS map would have mounted an unrelated
   overlay mid-teardown and then looked up that overlay's KCL handle instead of
   the VS map's. It stayed invisible because the VS BOOT itself never reads the
   registry -- SM64DS_VS_MAP goes through port_level_set_target and
   port_level_desc(), which finds the row by id -- so all four maps booted
   clean over a registry that was wrong. port/tools/mount_pairing_guard.py is
   what named it; the guard's model of the table was right and the code was
   wrong. */
static void *port_mount_row_lvl51(void) { return port_level_mount_at(46); }
static void *port_mount_row_lvl43(void) { return port_level_mount_at(47); }
static void *port_mount_row_lvl29vs(void) { return port_level_mount_at(48); }
static void *port_mount_row_lvl42vs(void) { return port_level_mount_at(49); }

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
    port_mount_row_lvl37,
    port_mount_row_lvl38,
    port_mount_row_lvl36,
    port_mount_row_lvl40,
    port_mount_row_lvl17,
    port_mount_row_lvl18,
    port_mount_row_lvl19,
    port_mount_row_lvl20,
    port_mount_row_lvl22,
    port_mount_row_lvl23,
    port_mount_row_lvl24,
    port_mount_row_lvl26,
    port_mount_row_lvl35,
    port_mount_row_lvl39,
    port_mount_row_lvl44,
    port_mount_row_lvl45,
    port_mount_row_lvl47,
    port_mount_row_lvl48,
    port_mount_row_lvl49,
    port_mount_row_lvl50,
    port_mount_row_lvl27,
    port_mount_row_lvl16,
    port_mount_row_lvl21,
    port_mount_row_lvl25,
    port_mount_row_lvl28,
    port_mount_row_lvl33,
    port_mount_row_lvl0,
    port_mount_row_lvl30,
    port_mount_row_lvl32,
    port_mount_row_lvl34,
    port_mount_row_lvl46,
    port_mount_row_lvl51,
    port_mount_row_lvl43,
    port_mount_row_lvl29vs,
    port_mount_row_lvl42vs,
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
   array pointer at +4.

   CAPTURED (DSSTATE_BEGIN/END): this is a description of the CURRENT LEVEL,
   not of the session. A save-state restore replaces the world, and these two
   words have to go with it or the warp list offers the previous level's
   entrances against the restored level's overlay. See the block comment on
   the file-handle table below for the whole argument. */
DSSTATE_BEGIN
static const unsigned char *g_entrance_entries;
static int g_entrance_count;
DSSTATE_END

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
/* run mg16 lane MP3: the spawn loop's own inputs, declared here because the
   file's main extern block for them is a thousand lines further down. */
extern "C" {
extern unsigned char data_0209f21c;
extern unsigned char data_0209f250;
extern int data_0209fc5c[];
extern unsigned char data_02092128[];
extern unsigned char data_0209caa0[];
extern unsigned char data_0209f2d8;      /* game mode: 0 single file, 1 VS, 2 script */
extern unsigned short data_ov002_0210cbf4[];
extern void *data_0209f394[];
}

extern "C" void port_vs_spawn_extra_players(void *tbl, unsigned p3);

/* THE PORT'S OWN PER-SLOT CHARACTER CHOICE. See the note at the a2 seat: the
   ROM's data_02092128 is rewritten during the level boot, so the choice the
   port makes for the extra players cannot live there. Four slots, defaulting
   to slot-index order (0 Mario, 1 Luigi, 2 Wario, 3 Yoshi), which is the order
   the VS menu offers and what makes two players tellable apart in a capture. */
static unsigned char g_vs_character[4] = {0, 1, 2, 3};
extern "C" void port_vs_set_character(int slot, int ch) {
    /* & 7, not & 3: src/_ZN6Player13InitResourcesEv.cpp:76 reads the
       character as `b & 7`, so the field is THREE bits. Masking to two
       costs nothing for the four characters the game ships and would
       silently fold any value above 3 onto a different character, which
       is exactly the kind of narrowing that survives review because the
       test data never exercises it. Match the ROM's width. */
    if (slot >= 0 && slot < 4) g_vs_character[slot] = (unsigned char)(ch & 7);
}
extern "C" int port_vs_character(int slot) {
    return (slot >= 0 && slot < 4) ? (int)g_vs_character[slot] : 0;
}

static void port_load1(void *t, int a, unsigned b)
{
    port_loader_enter(1, t);
    /* run mg16 lane MP3: the player count AS THE SPAWN LOOP SEES IT, which is
       the only reading that decides how many Player actors exist. The count is
       seated in two places (hal/level_boot.cpp's a2 seat and
       tests/walk_window.cpp), and a seat that lands after this call is a seat
       that did nothing -- which is exactly the bug this line was added to
       settle, so it prints the value at the door rather than at frame time. */
    if (std::getenv("SM64DS_VS_PROBE"))
        std::fprintf(stderr,
                     "[vs] LoadEntranceObjects: count=%d p3=%u live=%d,%d "
                     "chars=%d,%d\n",
                     (int)data_0209f21c, b, data_0209fc5c[0], data_0209fc5c[1],
                     (int)data_02092128[0], (int)data_02092128[1]);
    /* THE LOCAL SLOT IS HELD AT 0 ACROSS THE ENTRANCE LOAD, and restored the
       moment every slot has a body. Run mg16 lane MP3, field failure.

       Camera::InitResources runs INSIDE the loop below and reads
       data_0209f394[data_0209f250] -- the local player's actor. On the child
       that index is 1, and slot 1 has no body yet: the ROM's loop takes its
       player starts from consecutive entrance records and this level has only
       one, which is why port_vs_spawn_extra_players exists at all. So the
       camera dereferenced a null and the child died at
       Camera::InitResources+0x98 reading 0x000000cc, every boot.

       The fill-in cannot simply move earlier: the loop ASSIGNS
       data_0209f394[i] for every slot it visits, so anything pre-spawned is
       overwritten a moment later.

       Holding the index at 0 for the duration is the small, honest answer.
       Slot 0 always has a body -- it is the one start every level does have --
       so the camera initialises against a real actor exactly as it does in
       single player, and the true slot is restored before any frame runs. The
       camera reads the index per frame, so it follows the right player from
       the first tick; nothing downstream sees the temporary value. */
    const unsigned char saved_local = data_0209f250;
    const unsigned char saved_count = data_0209f21c;
    data_0209f250 = 0;
    /* AND THE COUNT IS HELD AT 1 ACROSS THE LOAD -- the orphan-shadow fix.
       Run mg16 lane MP3, field failure 3.

       The ROM's loop spawns player i from entrance record p3 + i, and this
       level has ONE player start; record 1 is a different door. For i=1 it
       therefore called Actor::Spawn on whatever that record names, got
       something back that was not a usable player, and discarded the pointer
       (data_0209f394[1] measured NULL) -- but THE ACTOR IT CREATED IS ALREADY
       LINKED INTO THE PROCESSING LIST. It ticks, its Behavior registers a
       ShadowModel node, and nothing ever draws a body for it, because the
       render loop draws data_0209f394[] and that slot points at the player
       port_vs_spawn_extra_players makes instead.

       An actor that casts a shadow and has no body is exactly the owner's
       "a shadow as if a third thing should be there", and it is attached to
       the remote player because that is the slot whose start was missing.

       MEASURED, with SM64DS_SHADOW_TRIS: one player draws 88 shadow triangles
       (64 player + 24 scenery), so two players should draw 152. It drew 216 --
       one whole extra player-sized caster.

       Holding the count at 1 makes the ROM's loop spawn exactly the starts the
       level really has, and port_vs_spawn_extra_players then supplies the rest
       from player 0's record. Same shape as the local-index hold above, and it
       retires itself the same way: on a real VS arena whose table carries four
       consecutive starts, the count should NOT be held, because the loop is
       then right.

       ##################################################################
       #  IT IS NOT KEYED ON THAT. IT HOLDS ON EVERY MULTIPLAYER BOOT.  #
       ##################################################################

       AND THAT IS A KNOWN, FILED LIMITATION rather than an oversight. Two
       versions of a condition were tried and neither works:

         `port_entrance_count() > 0` is the level's TOTAL record count -- 14 on
         castle grounds -- so it is true wherever an entrance table exists and
         the condition collapsed to "multiplayer is on". It only ever LOOKED
         conditional.

         Comparing entrance records by CLASS (does record p3+1 name the same
         actor as p3?) is the question this comment wants to ask, and it does
         not answer it: MEASURED on castle grounds, records p3 and p3+1 carry
         the SAME raw, so the predicate returned true, the hold stopped
         happening, and the orphan caster came straight back -- rungS went from
         152 shadow triangles to 216 in the run that tried it. Class equality
         does not distinguish a player start from a different door on this
         level, so it cannot be the test.

       What actually distinguishes them is whether Actor::Spawn PRODUCES a
       usable player from the record, and that is not knowable until the loop
       has already run and made the orphan.

       THE ARENA-DAY CONSEQUENCE, filed in port/ov002_frontier.txt: on a real VS
       arena whose table carries four consecutive player starts, this hold will
       ignore them and supply three copies of record 0 instead -- every player
       spawning on top of the first. Correct on every level the port loads
       today, wrong the day an arena mounts, and written down where the arena
       lane will find it rather than left as a surprise. */
    const int want_players = (int)data_0209f21c;
    if (want_players > 1)
        data_0209f21c = 1;
    _Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij(t, a, b);
    data_0209f21c = saved_count;
    port_vs_spawn_extra_players(t, b);
    data_0209f250 = saved_local;
    if (saved_local != 0 && std::getenv("SM64DS_VS_PROBE"))
        std::fprintf(stderr,
                     "[vs] local slot restored to %d after the entrance load "
                     "(held at 0 so Camera::InitResources had a body to "
                     "follow)\n", (int)saved_local);
}
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
DSSTATE_BEGIN
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
DSSTATE_END
}  /* extern "C" */

/* Two loaders define plain C++ names (their TUs never wrapped the definition
   in extern "C"); the table above wants the Itanium name every other caller
   uses. */
#pragma comment(linker, "/alternatename:__Z15LoadDoorObjectsRN11LVL_Overlay11ObjSubTableEij=?LoadDoorObjects@@YAXAAUObjSubTable@LVL_Overlay@@HI@Z")
#pragma comment(linker, "/alternatename:__Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij=?LoadStarCameraObjects@@YAXAAUObjSubTable@LVL_Overlay@@HI@Z")

// ---- LoadFile(handle) ------------------------------------------------------
//
// THE ROM CONTRACT IS ALLOCATE-FRESH-PER-CALL, CALLER FREES, and it is read
// off the ROM's own matched TUs rather than off a description of them.
// src/LoadFile.c is `func_0201818c(handle, 1)`; every path through
// src/func_0201818c.c ends in a Memory::Allocate followed by a CpuCopy8 or a
// DecompressLZ16 into the block it just allocated. There is no table, no
// dedupe and no refcount at this level. The refcounting the paragraph here
// used to claim belongs to SharedFilePtr::LoadFile, a different function with
// a different contract.
//
// The callers say the same thing. FIFTY-FIVE TUs under src/ call the free
// function -- a further six only DECLARE a member LoadFile inside a shadow
// class and are not callers at all -- and FORTY-SIX of the fifty-five also
// call Deallocate. src/func_ov006_020e3250.c frees on the line after the
// copy: it hands the block to func_020563d4, which uploads 0x800 bytes of it
// to the BG2 screen base, and Deallocates on the very next line. That is
// exactly why a re-request of its handle finds a block the ROM has already
// returned. The nine that do not are each a documented keep rather than a
// counterexample:
//
//   six ov075 TUs (_021173a8, _02117918, _02117d80, _02118378, _02118f38,
//     _0211944c) hand the pointer to func_ov075_02116030, which stores it
//     only when its slot is empty, so a repeat there leaks on the DS too;
//   func_ov004_020b2cb8, dScMgBase_c's per-language file table, keeps all
//     twenty-nine pointers live in data_ov004_020bf560 -- the ROM's own
//     working set, which is what the slot count below is sized against;
//   Stage::LoadClsnAndObjects keeps the level KCL;
//   LoadMessageBankForLanguage keeps the message bank.
//
// The last two are what the table below outlives a call for.
//
// The port's file seam is one level up, at SharedFilePtr (hal/fs.cpp), so this
// expresses that contract there: construct the handle's SharedFilePtr, Load it
// -- an allocation plus a memcpy out of hal/fs.cpp's own master copy, which is
// where the cost the host actually cares about was already removed -- and hand
// the block back.
//
// WHAT THE TABLE IS, NOW THAT IT IS NOT A CACHE. It records the LAST block
// handed out per handle. Two things in this file read it: the per-level
// teardown, which drops the rows, and port_level_capture_kcl, which needs the
// outgoing level's KCL block to hand back late. Reusing a handle's row rather
// than appending one keeps it bounded by the number of DISTINCT handles a boot
// asks for, which is what its size is argued against below.
//
// THE SINGLE-CONSUMER RULE, and it is a RULE rather than an observation. This
// comes from the review of the lane that made the change, which went looking
// for the failure the shape invites and found it: a row can be STALE, because
// the caller owns its block and is free to Deallocate it, and the review
// measured three handles landing on ONE address inside a single boot. That is
// harmless today for a reason that is structural and not lucky --
// port_level_capture_kcl is the ONLY consumer that DEREFERENCES a row's
// filePtr to make a lifetime decision; its handle is held live by a verified
// keeper (Stage::LoadClsnAndObjects, which never frees the KCL); and any
// competing loader of that same handle drives the row's load count to two,
// at which point the capture declines rather than guessing.
//
// NOTHING IN THE BUILD ENFORCES THAT. So, for whoever adds the next consumer:
// ANY NEW READER THAT DEREFERENCES A ROW'S filePtr MUST RE-ARGUE THE
// STALENESS CASE HERE BEFORE IT IS ADDED. Reading fileID or the persistent
// mark is free, because neither can dangle. Reading filePtr is not.
//
// AND THE CACHE WAS SERVING NOTHING. Measured on the build that still had it,
// with SM64DS_LOADFILE_AUDIT=1 over all 46 mounted levels at 300 frames each:
// 510 LoadFile calls, ZERO of them answered out of the table. Every repeat
// that looked like one in a per-run count turned out to have a level teardown
// between the two requests, which drops the table, so the second call was a
// fresh boot's first call. The dedupe's whole realised value on the level
// battery was nil, and its one firing anywhere was the corrupt one below.
//
// IT USED TO BE A CACHE, AND THAT WAS THE BUG. A repeat request returned the
// block from the first request, which the ROM's caller had freed in between --
// so the second caller was handed memory that by then belonged to whatever
// loaded next, and then freed it a second time. Measured on scene 374 with
// SM64DS_LOADFILE_AUDIT=1, before the repair: of the forty-one calls a curling
// boot makes, forty handed back the handle's own bytes and one did not. The
// one is dScMgCurling_c::InitResources' last act, func_ov006_020e3250, asking
// for handle 0x30 a second time to upload 0x800 bytes of rink tilemap: the
// block it got was 1248 bytes -- handle 0xc7's size, not 0x30's 2048 -- and
// 1242 of the 1248 compared bytes disagreed. The BG2 screen base then held a
// compressed sprite sheet read as 1024 screen entries.
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
/* SIXTEEN UNTIL run link60 lane MG2, WHICH IS A LEVEL'S NUMBER AND NOT A
   SCENE'S. Sixteen covers what a level boot asks for -- its KCL and its object
   files -- and the note further down this file sizes it against that. A
   MINIGAME asks for more in one function: func_ov004_020b2cb8, dScMgBase_c's
   per-language file table, calls LoadFile TWENTY-NINE times and keeps all
   twenty-nine pointers live in data_ov004_020bf560, so it is not a leak the
   release path can absorb, it is the ROM's own working set. At sixteen the
   first minigame boot aborted on "out of host file slots" partway through that
   loop. Sixty-four is twenty-nine plus room for the rest of a scene's boot; the
   per-level release below is unchanged and still runs, and the cost is one
   PortSharedFilePtr per unused slot. */
/* ---- CAPTURED, and why -----------------------------------------------------

   This table is the port's stand-in for func_0201818c's file cache. On the DS
   that cache is DS main RAM, so a save state captures it with everything else
   and a restore puts it back. The port hosts it as ordinary C storage instead,
   which put it OUTSIDE all three things a restore rolls back -- and a table
   that says which arena block holds which file is a description of the WORLD,
   not of the session. hal/dsstate_seg.h draws that line per symbol, not per
   file, and names ntr/runtime.cpp's two genuine DS globals as the precedent
   for bracketing part of an otherwise host-only file.

   Measured before this moved (run mg15, lane RELOAD): walk through a door into
   sub-area 32, save, quit, launch again into the castle grounds and let the
   disk state load at boot. The world comes back as sub-area 32 with its six
   file rows -- and this table still held the castle grounds' TEN, with slot 1
   naming handle 1943 at arena 30057464 while the restored world has handle
   1884 at that same address. The next Release frees a block by the wrong
   handle and the next load hands a consumer another file's bytes. Bracketing
   the three arrays is what makes the two descriptions incapable of
   disagreeing. */
enum { PORT_LOADFILE_SLOTS = 64 };
DSSTATE_BEGIN
static PortSharedFilePtr g_loadfile_slot[PORT_LOADFILE_SLOTS];
static int g_loadfile_used;
/* How many times this handle has been LOADED since the last teardown. It is
   not refcounting and it is not a lifetime: it is the row's honesty flag. A
   row holds the LAST block handed out for its handle, so once a handle has
   been loaded twice the row can no longer say which of those blocks a given
   consumer is holding -- and port_level_capture_kcl, the one place that frees
   a block off this table, must decline rather than guess. Saturates; a
   separate array because PortSharedFilePtr has to stay the ROM struct that
   SharedFilePtr::Construct and ::Release write through. */
static unsigned char g_loadfile_loads[PORT_LOADFILE_SLOTS];
DSSTATE_END

/* ---- the instruments, env-gated and off by default -------------------------

   SM64DS_LOADFILE_TRACE=1  one line per call: the handle, the block handed
     back, the row it is recorded in, whether the call loaded or was served
     from the row, and the game heap's free bytes afterwards.

   SM64DS_LOADFILE_AUDIT=1  the question the trace cannot answer, and the one
     the contract turns on: does the block handed back still hold the handle's
     OWN bytes? A second, independent copy is loaded through the same seam,
     compared, and returned to the allocator. DIFFER names the first byte that
     disagrees and how many of the compared bytes do. The audit allocates and
     frees around every load, so an audited run's heap layout is not a plain
     run's -- it is a measurement mode and no frame taken under it is
     comparable with one taken without it.

   SM64DS_LOADFILE_BG2=<handle>  at exit, compare that handle's bytes against
     the BG2 screen base. The destination is not guessed: G2::GetBG2ScrPtr is
     the ROM's own answer, and func_020563d4 -- the tilemap uploader every
     2D background goes through -- copies to exactly that pointer plus an
     offset. So this answers "did the file reach the screen base" in bytes,
     without reading a pixel or duplicating the base arithmetic.

   The allocator's node header carries the block's user size at userPtr-0xc
   (the same word port_level_free_captured_kcl reads, beside the 0x5544 used
   marker at -0x10), which is what bounds every comparison below. */
void _ZN13SharedFilePtr7ReleaseEv(PortSharedFilePtr *self);
void *_ZN2G212GetBG2ScrPtrEv(void);
unsigned port_level_heap_free_bytes(void);      /* hal/level_change.cpp */

static int port_loadfile_env_on(const char *name)
{
    const char *e = std::getenv(name);
    return e && *e && *e != '0';
}

static int port_loadfile_trace_on(void)
{
    static int v = -1;
    if (v < 0) v = port_loadfile_env_on("SM64DS_LOADFILE_TRACE") ||
                   port_loadfile_env_on("SM64DS_LOADFILE_AUDIT");
    return v;
}

static int port_loadfile_audit_on(void)
{
    static int v = -1;
    if (v < 0) v = port_loadfile_env_on("SM64DS_LOADFILE_AUDIT");
    return v;
}

static unsigned port_loadfile_node_size(const char *p)
{
    if (!p)
        return 0;
    return *(const unsigned short *)(p - 0x10) == 0x5544
               ? *(const unsigned *)(p - 0xc) : 0;
}

/* Load a second, independent copy of `handle` and compare it with `given`.
   Frees the copy before returning, so the audit adds no lifetime of its own
   to the one it is measuring.

   THE SNAPSHOT IS NOT TIDINESS. The block under audit may already be free --
   that is the whole hypothesis -- and the allocator is entitled to hand the
   probe's own Allocate the very same address. It would then write the
   handle's bytes over the evidence and the comparison would come back SAME
   for the one case it exists to catch. So `given` is copied to a host buffer,
   outside the game heap, BEFORE anything else is allocated. */
static void port_loadfile_audit(int handle, const char *given)
{
    const unsigned gn = port_loadfile_node_size(given);
    if (!gn) {
        std::fprintf(stderr, "[loadfile] %#x AUDIT: %p carries no live node "
                     "header; nothing compared\n", handle,
                     (const void *)given);
        return;
    }
    char *const snap = (char *)std::malloc(gn);
    if (!snap)
        return;
    std::memcpy(snap, given, gn);

    PortSharedFilePtr probe;
    _ZN13SharedFilePtr9ConstructEj(&probe, (unsigned)handle);
    _ZN13SharedFilePtr8LoadFileEv(&probe);
    if (!probe.filePtr) {
        std::fprintf(stderr, "[loadfile] %#x AUDIT: the probe load returned "
                     "nothing; nothing compared\n", handle);
        std::free(snap);
        return;
    }
    const unsigned pn = port_loadfile_node_size(probe.filePtr);
    const unsigned n = pn < gn ? pn : gn;
    unsigned first = ~0u, differ = 0;
    for (unsigned i = 0; i < n; ++i)
        if (snap[i] != probe.filePtr[i]) {
            if (first == ~0u) first = i;
            ++differ;
        }
    if (!differ && pn == gn)
        std::fprintf(stderr, "[loadfile] %#x AUDIT SAME over %u bytes\n",
                     handle, n);
    else
        std::fprintf(stderr, "[loadfile] %#x AUDIT DIFFER: %u of %u compared "
                     "bytes, first at %#x (given %p size %u, probe %p size "
                     "%u%s)\n", handle, differ, n, first, (const void *)given,
                     gn, (void *)probe.filePtr, pn,
                     probe.filePtr == given ? ", SAME ADDRESS: the block was "
                                              "free" : "");
    _ZN13SharedFilePtr7ReleaseEv(&probe);
    std::free(snap);
}

static void port_loadfile_report(int handle, const char *p, int row,
                                 const char *how)
{
    if (port_loadfile_trace_on())
        std::fprintf(stderr, "[loadfile] %#x -> %p row %d %s, heap free %u\n",
                     handle, (const void *)p, row, how,
                     port_level_heap_free_bytes());
    if (port_loadfile_audit_on())
        port_loadfile_audit(handle, p);
}

/* SM64DS_LOADFILE_BG2=<handle>: at exit, is the BG2 screen base holding that
   handle's file? Registered on the first LoadFile so the check only runs on a
   process that loaded something. */
static int g_bg2_check_handle = -1;

static void port_loadfile_bg2_check(void)
{
    const int handle = g_bg2_check_handle;
    const char *scr = (const char *)_ZN2G212GetBG2ScrPtrEv();
    if (handle < 0 || !scr) {
        std::fprintf(stderr, "[loadfile] BG2 CHECK: no screen base\n");
        return;
    }
    PortSharedFilePtr probe;
    _ZN13SharedFilePtr9ConstructEj(&probe, (unsigned)handle);
    _ZN13SharedFilePtr8LoadFileEv(&probe);
    if (!probe.filePtr) {
        std::fprintf(stderr, "[loadfile] BG2 CHECK %#x: the file did not "
                     "load\n", handle);
        return;
    }
    const unsigned n = port_loadfile_node_size(probe.filePtr);
    unsigned first = ~0u, differ = 0;
    for (unsigned i = 0; i < n; ++i)
        if (scr[i] != probe.filePtr[i]) {
            if (first == ~0u) first = i;
            ++differ;
        }
    if (!differ)
        std::fprintf(stderr, "[loadfile] BG2 CHECK %#x at %p: EXACT over %u "
                     "bytes\n", handle, (const void *)scr, n);
    else
        std::fprintf(stderr, "[loadfile] BG2 CHECK %#x at %p: DIFFER, %u of "
                     "%u bytes, first at %#x\n", handle, (const void *)scr,
                     differ, n, first);
    _ZN13SharedFilePtr7ReleaseEv(&probe);
}

static void port_loadfile_bg2_arm(void)
{
    static int armed;
    if (armed)
        return;
    armed = 1;
    const char *e = std::getenv("SM64DS_LOADFILE_BG2");
    if (!e || !*e)
        return;
    g_bg2_check_handle = (int)std::strtol(e, 0, 0);
    std::atexit(port_loadfile_bg2_check);
}

/* PORT_HOST_ABI: src is func_0201818c(handle,1), the DS card archive loader;
   the port's file seam is one level up at SharedFilePtr, so this expresses the
   same contract there rather than driving card hardware. */
void *LoadFile(int handle)
{
    enum { SLOTS = PORT_LOADFILE_SLOTS };
    PortSharedFilePtr *const slot = g_loadfile_slot;
    int &used = g_loadfile_used;
    port_loadfile_bg2_arm();
    int row = 0;
    while (row < used &&
           !(slot[row].fileID && (int)slot[row].fileID == handle))
        ++row;
    if (row == used) {
        if (used >= SLOTS) {
            std::fprintf(stderr, "FATAL: LoadFile: out of host file slots\n");
            std::abort();
        }
        ++used;
    }
    PortSharedFilePtr *s = &slot[row];
    /* The persistent mark is the row's, not the block's: it says this handle's
       image survives a level teardown, and re-loading the handle does not
       change that.

       Construct does NOT actually clear it, and the earlier claim here that it
       zeroes the whole struct was wrong: src/func_02017e0c.c writes bytes 0..2
       and 4..7 and never touches byte 3, which is where pad lives. So this
       save and restore is defensive and carries nothing today. It is kept
       because the row's persistence should not rest on which bytes a matched
       ROM function happens to cover: a Construct that grew to clear byte 3
       would otherwise unpin the message bank with no other symptom. */
    const unsigned char persistent = s->pad;
    /* Construct clears filePtr and numRefs (src/func_02017e0c.c), which is
       what makes a reused row LOAD again: SharedFilePtr::LoadFile only calls
       Load when numRefs is zero, and would otherwise hand back the pointer
       the row already held -- the cache this shape exists to remove. */
    _ZN13SharedFilePtr9ConstructEj(s, (unsigned)handle);
    _ZN13SharedFilePtr8LoadFileEv(s);
    if (!s->filePtr) {
        std::fprintf(stderr, "FATAL: LoadFile(%d): no bytes\n", handle);
        std::abort();
    }
    /* Construct rewrites fileID from the ov0 handle to the FAT file id, so
       the row key matches only when both agree; keep the handle. */
    s->fileID = (unsigned short)handle;
    s->pad = persistent;
    if (g_loadfile_loads[row] < 0xff)
        ++g_loadfile_loads[row];
    port_loadfile_report(handle, s->filePtr, row, "LOADED");
    return s->filePtr;
}

/* Pin the slot behind `handle` as PERSISTENT, so the per-level teardown
   (port_level_reset_host) leaves it loaded across level changes. The message
   bank is loaded once at game boot through this same table (message_boot.cpp),
   not per level, and its section pointers stay pinned in globals for the whole
   run; freeing its image on the first level change would leave the message
   system reading a freed block. Everything else in the table is a level's own
   file, dropped on the change (and the KCL freed). `pad` carries the flag (it
   is otherwise unused, and the ROM's SharedFilePtr has nothing there either). */
void port_loadfile_pin_persistent(int handle)
{
    for (int i = 0; i < g_loadfile_used; ++i)
        if ((int)g_loadfile_slot[i].fileID == handle) {
            g_loadfile_slot[i].pad = 1;
            return;
        }
}

/* PROOF-OF-FIX (temporary, minigame-entry lane): free the non-persistent
   LoadFile slots so a scene transition starts with an empty table, the same
   drop-not-release discipline port_level_reset_host uses. */
extern "C" void port_loadfile_reset_scene(void)
{
    int keep = 0;
    for (int i = 0; i < g_loadfile_used; ++i) {
        if (g_loadfile_slot[i].pad) {
            if (keep != i) {
                g_loadfile_slot[keep] = g_loadfile_slot[i];
                g_loadfile_loads[keep] = g_loadfile_loads[i];
            }
            ++keep;
        }
    }
    for (int i = keep; i < g_loadfile_used; ++i) {
        g_loadfile_slot[i].fileID = 0;
        g_loadfile_slot[i].numRefs = 0;
        g_loadfile_slot[i].filePtr = 0;
        g_loadfile_slot[i].pad = 0;
        g_loadfile_loads[i] = 0;
    }
    g_loadfile_used = keep;
}

/* Method faces: the three MeshCollider helpers the boot calls by their
   Itanium names while their definitions are real MSVC members. */
void _ZN12MeshCollider17UpdateFileOffsetsER8KCL_File(void *file)
{ MeshCollider::UpdateFileOffsets(*(KCL_File *)file); }
int _ZNK12MeshCollider16GetOctreeOriginYEv(const void *self)
{ return ((const MeshCollider *)self)->MeshCollider::GetOctreeOriginY(); }
int _ZNK12MeshCollider13GetUnkOctreeYEv(const void *self)
{ return ((const MeshCollider *)self)->MeshCollider::GetUnkOctreeY(); }

// ---- the globals the sub-loaders store through -----------------------------
//
// Every "Load<Kind>Objects" that is not a spawner is a two-word veneer:
// store the table pointer in one global, the count in another. Storage only;
// the consumers (minimap, fog, teleport) are Stage B and C.
DSSTATE_BEGIN
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
DSSTATE_END
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
#define SAVEBLK(sec, name, size) \
    __pragma(section(sec, read, write))                          \
    extern "C" __declspec(allocate(sec)) __declspec(align(2))    \
    unsigned char name[size] = {0}

SAVEBLK(".dsstate$savblk0000", data_0209caa0, 0x14);
SAVEBLK(".dsstate$savblk0001", data_0209cab4, 0x1e);
SAVEBLK(".dsstate$savblk0002", data_0209cad2, 0x12);
SAVEBLK(".dsstate$savblk0003", data_0209cae4, 0x10);
/* AND THE FIFTH. The paragraph above has said "split five ways" since it was
   written while the macro ran out at four, and run link60 Stage 5 lane SV1
   added this row because the change it landed turns the gap from a read into
   a WRITE. config/arm9/symbols.txt runs caa0, cab4, cad2, cae4, caf4, then
   data_0209cdcc, so caf4's ROM span is 728 bytes and the whole save object is
   one 0x32c run of five symbols.
   WHAT NEEDED IT. SaveData::SetDefaultValuesMg clears 0x2e4 bytes through the
   pointer its only caller hands it, and that pointer is data_0209cae4, which
   is caa0+0x44. It therefore needs caa0+0x44 .. caa0+0x328 to be real
   storage. With the group ending at caa0+0x54 the other 0x2d4 bytes landed on
   data_020a0de8's four touch bytes, on _dsstate_hi, and then past the end of
   the captured span; measured in this tree's map before the fix, cae4 sat at
   00c9a2c8 and _dsstate_hi at 00c9a2e8. Nothing would have faulted, which is
   the bad news rather than the good: that memory is mapped and the corruption
   would have been silent.
   IT WAS ALREADY WRONG AS A READ, before any write reached it.
   src/func_ov007_020cc600.c ends every call with
   func_ov007_020cc168(data_0209caa0[0x328]), the byte at caa0+0x328, and the
   title screen dispatches that function twice per boot. That read was 0x2c4
   bytes past _dsstate_hi until this line existed.
   THE STORAGE MOVED HERE from hal/scene_mg_faces.cpp, which had it at the
   right 728 bytes and could not make it contiguous from another translation
   unit; it was 0x84574 bytes away in the image. Both files are in the same
   three targets, so nothing changes about which builds have it. */
SAVEBLK(".dsstate$savblk0004", data_0209caf4, 728);

#undef SAVEBLK

// ---- the six the Player::InitResources retirement handed back --------------
//
// port/unmatched/Player_InitResources.cpp used to be the definition site for
// these, because a hand-written host copy is free to own storage. The matched
// source hosts itself now (HOSTABI_RETIRE_SYMS), and it goes through
// --extern-data, which is the port's standing claim that engine BSS is the
// HAL's -- so the storage comes here rather than staying in a src TU.
//
// EVERY ONE IS FOUR BYTES, taken from config/arm9/symbols.txt by next-symbol
// delta, not from the width the one function that used them happened to
// declare. The retired copy spelled three of them `u8` and one `s8`; the
// ROM's own symbol grid puts the next symbol four bytes on in every case
// (0209f200/0209f204, 0209f254/0209f258, 0209f2fc/0209f300,
// 02092114/02092118), so a one-byte host object left three bytes of a ROM
// global sitting on whatever the linker put next. Same reasoning and same
// spelling as the sub-loader block above.
//
// data_02092114 and data_0209212c are kind:data(any) in the symbol table
// rather than bss, and they are zero-initialised here because that is exactly
// what the retired host copy did -- this is a move, not a correction.
// data_02092114 reads as a queued character swap where -1 means "none"
// (hal/star_flow.cpp and hal/player_bridges.cpp both say so), so a host zero
// is a queued swap to character 0 at boot. That is the data_0209211c trap the
// retired file's own comment describes, one symbol over, and it was already
// live before this move; it wants its ROM initial value re-derived rather
// than a guess folded into a retirement commit.
DSSTATE_BEGIN
extern "C" {
unsigned char data_0209f200[4];   /* level-enter mode latch         */
unsigned char data_0209f254[4];   /* the star / silver-star request */
/* THE LATCHED ENTRY REASON, 1 fresh / 2 death -- Stage::InitResources' copy of
   data_0209f26c (src/_ZN5Stage13InitResourcesEv.cpp:201), not a cap check. The
   old comment here said "the 'lost the cap' check gate", which is wrong and cost
   a triage pass: it is what the Player's level-enter step reads to restore
   health on re-entry (src/func_ov002_020c75f0.c:29) and what HUD::InitResources
   reads to pick the meter state. The boot latches it; see the seat below. */
unsigned char data_0209f2fc[4];
signed char   data_02092114[4];   /* queued character swap, -1 none */
int           data_0209212c;      /* the spawn probe's Y            */
int           data_0209fc48;      /* the running cutscene, 0 = none */
}
DSSTATE_END

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
/* why we are ENTERING: 1 fresh, 2 death. SetNextLevel (src/SetNextLevel.c:54)
   writes it from KillPlayer's / HitDeathPlane's argument; the boot latches it
   into data_0209f2fc the way Stage::InitResources:201 does. Declared the same
   way hal/level_change.cpp:118 declares it -- storage is auto_bss.cpp's wider
   host allocation and the ROM reads it as a byte. */
extern unsigned char data_0209f26c;
extern int data_0209f264[];          /* current entrance */
extern int data_0209f220[];          /* current star filter */
extern unsigned char *data_0209f344; /* VS star-order pointer (host: bob_enemy_bridges.cpp) */
/* data_0209212c (world Y max) is DEFINED above, in the retirement block. Do
   not re-declare it here: a second declaration outside the DSSTATE pragma
   region moves the symbol back to the default segment and the save state
   stops capturing it. See the note in hal/dsstate_seg.h. */
extern int data_020a0d84[];          /* path table base (auto_bss) */
extern int data_020a0d88[];          /* path node base (auto_bss) */

/* Loader indices, for the suppression masks. */
enum {
    LOADER_ENTRANCE = 1,
    LOADER_DOOR = 9,
    LOADER_EXIT = 10,
};

/* NOT CAPTURED, and this one is WRITE-ONLY: port_stage_boot_body assigns it and
   nothing in the tree reads it (grep -rn g_stage_mc port/ finds the definition
   and the one assignment, nothing else). RELOADRV's reverse scan named it
   because it holds an arena address, which it does -- and a stale arena address
   nothing dereferences is not a hazard, it is dead storage. Left in place
   rather than deleted because that is a separate change from this lane's, and
   named here so the next reverse scan does not have to re-derive the answer. */
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

/* ---- [lvl-perf]: what a level entry costs ---------------------------------
   Four QPC-bracketed spans, accumulated across one entry and printed as ONE
   stderr line (so it lands in the playlog) by port_lvlperf_emit:

       [lvl-perf] teardown=Xms boot=Yms census=Zms print=Wms

   teardown is the level-change pre-boot span (level_change.cpp, zero on a
   direct boot), boot is port_stage_a_boot below, census is port_actor_census,
   and print is the boot-dump probe block (port_level_probe +
   port_stage_a_probe) -- timed apart from the boot proper so time spent
   INSIDE printf is its own number. That split is the point: the entry stall
   this line was built to watch was stdout itself (an unbuffered console
   costs 2-6ms PER LINE and a level entry prints ~248 of them; the setvbuf
   note in walk_window.cpp has the measurements). A handful of QPC reads per
   level entry, nothing per frame. QPC hand-declared so this file stays out
   of windows.h. */
extern "C" __declspec(dllimport) int __stdcall
QueryPerformanceCounter(long long *);
extern "C" __declspec(dllimport) int __stdcall
QueryPerformanceFrequency(long long *);

static double g_lvlperf_ms[4];   /* teardown, boot, census, print */

extern "C" double port_lvlperf_now(void)
{
    static long long freq;
    long long c;
    if (!freq)
        QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&c);
    return c * 1000.0 / freq;
}

extern "C" void port_lvlperf_note(int span, double ms)
{
    if (span >= 0 && span < 4)
        g_lvlperf_ms[span] += ms;
}

extern "C" void port_lvlperf_emit(void)
{
    std::fprintf(stderr,
                 "[lvl-perf] teardown=%.1fms boot=%.1fms census=%.1fms "
                 "print=%.1fms\n", g_lvlperf_ms[0], g_lvlperf_ms[1],
                 g_lvlperf_ms[2], g_lvlperf_ms[3]);
    g_lvlperf_ms[0] = g_lvlperf_ms[1] = g_lvlperf_ms[2] = g_lvlperf_ms[3] = 0;
}

/* SM64DS_MM_STALE=1 probe; defined beside port_level_reset_host below. */
static void port_minimap_stale_probe(const char *when);

/* ---- THE BOOT IS DISPATCHED NOW, NOT CALLED (run link60, lane L4) ---------
 *
 * port_stage_a_boot used to BE the body below. It is now the seam that hands
 * the body to _ZTV5Stage slot 0 and lets the ROM's own init Process dispatch
 * it: hal/stage_bridges.cpp's port_stage_lifecycle_boot calls func_020433b8 on
 * the first entry, which is the exact call func_02043098 makes for every other
 * actor class, and dispatches slot 0 through the table on later ones.
 *
 * WHAT THAT CHANGES, and it is worth being exact because nothing here is a
 * linkage move. The body is the same body; it is the same host subset of
 * Stage::InitResources it was before, for the reason
 * port/stage_lifecycle_map.txt section 4 gives. What is new is that slot 1
 * (Scene::ResetFadersAndSound, through the Stage's own veneer target) and
 * slot 2 (ActorDerived::AfterInitResources, through the face) now RUN, matched
 * code doing the ROM's work at the ROM's point in the boot -- and that the
 * Stage reaches the behaviour and render lists its SpawnInfo has always
 * described, which is what retires the pause-bit stand-in in
 * hal/stage_bridges.cpp.
 *
 * The two arguments ride in a stash rather than through the dispatch, because
 * the ROM's init Process dispatches int(void) and has nowhere to put them. */
/* NOT CAPTURED, adjudicated rather than assumed. RELOADRV's reverse scan named
   g_boot_mc as a host word holding an arena address (3003afcc, the Stage's
   level MeshCollider). It is a CALL ARGUMENT IN TRANSIT: port_stage_a_boot
   writes all three, port_stage_lifecycle_boot reads them back through
   port_stage_boot_arg_mc/_spawn on the same call, and port_stage_boot_set_result
   fills the third before the same call returns. Nothing reads any of them
   outside one boot, and no restore lands inside a boot -- the disk read runs
   after the boot completes and F9 runs in the frame loop -- so what a restore
   would roll back is a value that is dead until the next boot overwrites it. */
static void *g_boot_mc;
static int   g_boot_spawn;
static void *g_boot_result;

extern "C" void *port_stage_boot_arg_mc(void)  { return g_boot_mc; }
extern "C" int   port_stage_boot_arg_spawn(void) { return g_boot_spawn; }
extern "C" void  port_stage_boot_set_result(void *o) { g_boot_result = o; }
extern "C" void  port_stage_lifecycle_boot(void);   /* hal/stage_bridges.cpp */
extern "C" void *port_stage_boot_body(void *mc, int spawn);
/* The scene root, and the level model loader the boot below now calls in the
   ROM's order. port_stage_object returns null before port_stage_create has
   run, which is the legacy no-Stage boot; _ZN5Stage9LoadModelEv is matched
   src, on slice_gate26.txt:29 and already linked. */
extern "C" void *port_stage_object(void);
extern "C" void _ZN5Stage9LoadModelEv(char *self);

void *port_stage_a_boot(void *mc, int spawn)
{
    g_boot_mc = mc;
    g_boot_spawn = spawn;
    g_boot_result = 0;
    port_stage_lifecycle_boot();
    return g_boot_result;
}

/* ---- THE INTRO SEAM -------------------------------------------------------
   ONE decision point for "does this entry play the ROM's opening", and ONE
   place to suppress it. Nothing else in the port may test the intro bit to
   decide this; scattering the condition is what this block exists to prevent.

   THE ROM'S OWN RULE, from matched source
   (src/_ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider.cpp:76-98):

       intro = (data_0209f2d8 == 0)                 // game mode 0 = single file
            && ((data_0209caa0[2] & 0x80) == 0)     // flags2 bit 7: not seen yet
            && (ContinueKuppaScriptIfNecessary() == 0);
       ... Spawn(0x14f MINIMAP) always; Spawn(0x14e HUD) only when !intro ...
       if (intro) StartIntroCutscene();             // Sound::LoadInitialGroup(0x25)
                                                    // + RunKuppaScript(&data_020890a0)

   A FRESH SAVE LEGITIMATELY HAS THAT BIT CLEAR. SaveData::SetDefaultValues
   (0x02013dc4) memsets the 0x44-byte record and leaves flags2 == 0x8 (bit 3,
   Yoshi unlocked) -- bit 0x80 clear. An empty slot reaches that through
   SaveData::ReadFileData when the cart read fails. So the ROM needs no extra
   state to ask for the opening: the absence of the bit IS the request.

   WHY THE PORT USED TO SET IT UNCONDITIONALLY, and why that is still mostly
   right: the same bit is what the whole bottom screen renders through
   (HUD::Behavior and HUD::Render both open with
   `if ((data_0209caa0[2] & 0x80) == 0) return 1;`), so a level walked around
   with the bit clear has a correctly-constructed HUD that draws nothing. That
   is the state the real game is in ONLY during the opening. The fix is not to
   stop setting the bit -- it is to stop setting it for the ONE entry the ROM
   would have played the opening on, and let the ROM's own machinery set it
   when the opening ends.

   WHO SETS IT WHEN THE OPENING ENDS: the ROM does, and it is hosted.
   src/func_ov085_0212d5dc.cpp:49-51 -- LakituBro's last opening state, after
   the camera flight settles -- hands control back and does
   `data_0209caa0[2] |= 0x80;`. The port writes nothing there.

   WHY THIS IS ARMED RATHER THAN DERIVED FROM THE BIT ALONE: every level
   selftest in the battery boots level 1 directly, and the ROM's gate has no
   level check (an agent's disassembly pass confirmed this: any level loaded
   with mode 0 and the bit clear starts the opening). Deriving from the bit
   alone would fire the opening in all of them. The arm is set only by
   port_level_entry_latch (hal/level_change.cpp), which only the title bridge
   calls -- so the default boot cannot reach it. */
static int g_intro_armed;

/* THE SUPPRESSION SEAM. The one place that says "not this time".
   The direct-entry round gates HERE -- adding its own test to this function --
   rather than adding a condition anywhere else.

   IT DEFAULTS TO SUPPRESSED, and that is a measurement, not caution. With the
   seam armed the ROM's gate DOES fire -- proven, because the HUD (actor 0x14e)
   stops being spawned, which is the ROM's own intro branch and nothing else in
   the port can produce it -- and the opening then reaches four gaps in the port
   that are outside this seam's reach. Measured on
   out/intro-cutscene/P2_intro_on.log:

     1. actor 0x160 CutsceneObject is NOT REGISTERED. The log says
        "[spawn-declined] actor 0x160 not registered". That one class IS the
        cutscene cast: kuppa cmd 0x17 is the Peach letter (peach_letter_US.bmd),
        0x12 the Lakitu flight (c_jugem.bmd), 0x13 Peach, 0x14/0x15/0x16 the
        three warp pipes, 0x19 the cloud. Needs a class seat in
        hal/actor_classes*.cpp -- not this lane's files.

     2. src/func_ov002_020bd600.c is MATCHED but offered to no slice, so it is
        absent from the binary. It is reached through the ov002 state descriptor
        at 0x0210a14c -- a Player state the opening drives into -- and the run
        dies calling its DS address: "FAULT code c0000005 ... accessing
        020bd600", eax holding the unrelocated pointer.

     3. src/__sinit_02073e6c.c is in no slice either, so it never runs. It is
        what patches the script blobs' unaligned pointer fields at boot (392
        strb). VERIFIED in the ROM image: data_020890a0+0x103 (the cmd-6 chain
        arg) and the camera waypoint slots at +0x62/+0x66/+0x8a/+0x8e are all
        ZERO on disc. Without that sinit the opening plays its first script to
        frame 400 and stops -- no Lakitu flight, no arrival, and no closing
        LoadLevelNoReturn.

     4. src/func_ov085_0212d5dc.cpp -- LakituBro's last opening state, the one
        that hands control back and sets the intro-seen bit -- calls
        func_ov002_020c3e8c() with no argument, relying on the receiver riding
        ARM r0. The tree already knows: the pair is frozen in
        port/tools/aritycheck_plainfunc_baseline.txt. Four sibling LakituBro
        bodies already have host copies for exactly this; this one does not,
        because nothing reached it until the gate started firing.

   So the honest state is: the DECISION is solved and hosted, the CAST is not.
   Flipping this default is a one-line change once those four close, and the
   run above is the worklist. SM64DS_INTRO=1 opts in today for that work.

   ---- THE FLIP HAPPENED, AND THE FOUR CLOSED ------------------------------

   All four are closed and the opening runs end to end. The evidence is one
   process in runs/rel0215/out/gatefix/after/intro.log:

     122066  [intro] the opening is ARMED for this entry
     122092  [intro] a cutscene script is running (007A23A0)
     134543  [intro] boot: flags2 bit7 0 | pending 007a24e8 | running 007a42d8
     137526  [intro] flags2 bit 7 0 -> 1

   That last line is the whole proof. Nothing in the port writes that bit --
   src/func_ov085_0212d5dc.cpp:51 does, and it is LakituBro's LAST opening
   state, the one that hands control back after the camera flight settles. The
   bit going 0 -> 1 means the opening played to its own end through the ROM's
   own script, reloaded through its own cmd-0x0b pending word, and gave the
   player the game. Gap 1 (actor 0x160) is seated, gap 2 and gap 4 are in
   slices, and gap 3 is called by hand out of port_intro_seat_scripts below.

   SO THE DEFAULT INVERTS: the opening PLAYS unless something says not this
   time. It is still the ONE suppression seam and it still gates here rather
   than anywhere else. Two names say not this time and they are the same
   answer: SM64DS_SKIP_INTRO=1, the owner's "option to skip opening cutscene",
   and SM64DS_INTRO=0, which is the name the tree already documents. Both are
   resolved in hal/title_entry.cpp's port_boot_skip_intro so there is one
   reader of both spellings.

   NOTHING ELSE MOVES, and this is the half worth being precise about, because
   a default that fires an opening in forty-six level selftests would be a
   disaster and it cannot happen here. This function is only ever consulted
   from port_intro_wants_play below, which returns 0 before reaching it unless
   g_intro_armed is set, and g_intro_armed is set by exactly one caller --
   port_level_entry_latch in hal/level_change.cpp, which only a title crossing
   and the VS start reach. A direct SM64DS_LEVEL boot never arms, so it never
   asks. The VS start does arm, and is then refused by the ROM's own first
   precondition below (data_0209f2d8 != 0: PrepareVsMode writes mode 1), which
   is measured rather than reasoned in this lane's proof runs. */
extern "C" int port_boot_skip_intro(void);   /* hal/title_entry.cpp */

extern "C" int port_intro_suppressed(void)
{
    return port_boot_skip_intro();
}

/* Armed by the title bridge's own latch, for the next level boot only. */
extern "C" void port_intro_arm_for_entry(void)
{
    g_intro_armed = 1;
}

/* The ROM's own static initialiser for the script blobs. It writes the chain
   arguments and the argument-blob pointers into the four opening scripts, byte
   by byte because those fields are unaligned, and on the host it writes HOST
   addresses. The scripts ship with those fields ZERO, so without it the opening
   dereferences nulls (measured: a c0000005 inside SharedFilePtr::Release).

   Called from HERE, not from the boot, on purpose. The ROM runs it
   unconditionally at startup and nothing else in the port reads what it
   patches, so running it unconditionally would also be correct -- but this lane
   promised the default path stays byte-identical, and the only path that needs
   these pointers is the one about to play the opening. Once per process; the
   precedent for calling an arm9 sinit by hand is hal/fdr_arm9_fader_seat.cpp,
   which does the same for __sinit_02074f80. */
extern "C" void __sinit_02073e6c(void);
extern "C" void port_intro_seat_dcc(void);   /* hal/intro_dcc_blob.cpp */
/* The PENDING script word. ProcessKuppaScript's cmd 0x0b stores it next to the
   closing LoadLevelNoReturn, and ContinueKuppaScriptIfNecessary reads it on the
   next boot -- which is the mechanism that brings the HUD back. Declared here
   only to be READ by the boot assertion below; data_0209fc48 is defined further
   up this file. */
extern "C" int data_0209fc4c;
extern "C" {
extern unsigned data_ov085_02130744[];
/* declared again here: the file's own declaration is a thousand lines below
   this seam, and the seam is the first thing in the boot that needs it */
void port_actor_overlays_sinits(void);
}

/* The fs-floor watchpoint. data_ov085_02130744 is a SharedFilePtr
   { u16 fileID; u8 numRefs; void *filePtr; }; this reports the first two so a
   run can say WHERE the fileID stops being 291. Inert unless SM64DS_INTRO_WATCH
   is set, so it costs the default path nothing. */
extern "C" void port_intro_watch(const char *where)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_INTRO_WATCH") ? 1 : 0;
    if (!on)
        return;
    const unsigned char *p = (const unsigned char *)data_ov085_02130744;
    std::fprintf(stderr, "  [watch] %-28s data_ov085_02130744 fileID %u "
                 "numRefs %u filePtr %p\n", where,
                 (unsigned)(p[0] | (p[1] << 8)), (unsigned)p[2],
                 *(void *const *)(p + 4));
}

static void port_intro_seat_scripts(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    /* Seat the one code pointer ptr_audit flagged BEFORE the sinit writes into
       the same blob, so nothing can read a DS address in between. */
    port_intro_seat_dcc();
    __sinit_02073e6c();

    /* THE ov085 FILE POINTERS. The opening's cast loads its models through
       SharedFilePtrs that live in ov085 BSS and are Constructed by
       __sinit_ov085_0212fa40 (data_ov085_02130744 gets file id 0x2d8 at its
       line 65). Those sinits run from port_actor_overlays_sinits(), which the
       port calls out of port_a2_seat_body -- and the title-entry crossing made
       that seat SINGLE-RUN (see the title-entry commit: it "ran twice and is
       not idempotent"). So on this path the construction may already have
       happened for the TITLE and not for the adventure, which reads as
       "fs fileID 0 not in catalog (fileptr 02130744)" from the port's own fs.
       The id is fine and the asset is present -- 0x2d8 is
       data/normal_obj/obj_pushblock/obj_pushblock.bmd in build/assets/files.tsv
       -- so this is an unconstructed handle, not a missing file.
       port_actor_overlays_sinits() carries its own done-guard, so calling it
       here is a no-op when it has already run and the fix when it has not.
       The probe reports which of those it was rather than leaving it to
       inference. */
    {
        const unsigned before = data_ov085_02130744[0];
        port_actor_overlays_sinits();
        const unsigned after = data_ov085_02130744[0];
        std::fprintf(stderr, "  [intro] ov085 file pointers: data_ov085_02130744"
                     " fileID %u -> %u (%s)\n", before, after,
                     after == before ? "unchanged -- the sinits had already run"
                                     : "constructed by this call");
        port_intro_watch("at the intro seam");
    }
}

/* The decision, consumed once by the boot that follows the crossing. */
extern "C" int port_intro_wants_play(void)
{
    /* THE OPENING'S THREE ROM WORDS, ON EVERY BOOT. This is where the completion
       bar is asserted rather than eyeballed, because all three live here and
       all three are the ROM's own:

         flags2 bit 7   data_0209caa0[2] & 0x80 -- "the opening has been seen".
                        Clear on the boot that plays it; SET afterwards, and set
                        by src/func_ov085_0212d5dc.cpp:51 (LakituBro's last
                        opening state), never by the port.
         pending        data_0209fc4c -- what ProcessKuppaScript's cmd 0x0b
                        stored alongside the closing LoadLevelNoReturn. Non-zero
                        on the RELOAD boot is what makes
                        ContinueKuppaScriptIfNecessary return 1, which makes the
                        gate's `intro` false, which is HOW THE HUD COMES BACK.
         running        data_0209fc48 -- the script actually executing.

       Printed before the armed check so the reload boot reports too; that boot
       is not armed and would otherwise return on the first line.
       Inert unless SM64DS_INTRO_WATCH. */
    {
        static int on = -1;
        if (on < 0)
            on = std::getenv("SM64DS_INTRO_WATCH") ? 1 : 0;
        if (on)
            std::fprintf(stderr,
                         "  [intro] boot: flags2 bit7 %d | pending %08x | "
                         "running %08x | armed %d\n",
                         (data_0209caa0[8] & 0x80) ? 1 : 0,
                         (unsigned)data_0209fc4c, (unsigned)data_0209fc48,
                         g_intro_armed);
    }
    if (!g_intro_armed)
        return 0;
    g_intro_armed = 0;                  /* one-shot: this boot, not the next */
    if (port_intro_suppressed())
        return 0;
    /* The ROM's own two preconditions, re-read rather than assumed. The third
       (no script already running) is LoadClsnAndObjects' own and is left to it. */
    if (data_0209f2d8 != 0)
        return 0;
    if (data_0209caa0[8] & 0x80)
        return 0;                       /* a used file: the opening is done */
    port_intro_seat_scripts();          /* the chain pointers, before it starts */
    return 1;
}

extern "C" void *port_stage_boot_body(void *mc, int spawn)
{
    const double lvlperf_t0 = port_lvlperf_now();
    g_stage_mc = mc;
    port_minimap_stale_probe("boot entry");
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

    /* THE ENTRY REASON, LATCHED, where Stage::InitResources latches it:
       src/_ZN5Stage13InitResourcesEv.cpp:201, `data_0209f2fc = data_0209f26c`,
       immediately before the same function replaces data_0209f2f8 with the
       level being entered -- which is the line directly below. data_0209f26c is
       "why we are entering" (1 fresh, 2 death), written by SetNextLevel
       (src/SetNextLevel.c:54) from KillPlayer's and HitDeathPlane's argument;
       data_0209f2fc is the copy every consumer downstream of the boot reads.

       WITHOUT THIS THE PORT CANNOT LEAVE A DEATH. The ROM restores the player
       on re-entry from the Player level-enter state itself,
       src/func_ov002_020c75f0.c:29 --

           if (data_0209f2fc == 1 || data_0209f2fc == 2)
               Player::Heal(c, 0x880);

       -- and GiveHealth clamps at 0x880, so that is an unconditional restore to
       full health. That step is linked (slice_gate14.txt) and dispatched
       (hal/player_states.inc:297); it has been declining every entry because its
       gate read a byte nobody set. The same file plays the death-arrival voice
       clip on == 2 (line 68), and HUD::InitResources reads the same byte to pick
       the meter state (port/unmatched/HUD_InitResources.cpp:288): == 2 seats
       data_ov002_02111178 = 1, which is what lets HUD::UpdateHealthMeter walk
       state 1 -> 2 -> 3 once health is back to 8, and state 3 is the ONLY
       GiveLives(-1) call site in the game (src/_ZN3HUD17UpdateHealthMeterEv.cpp
       :117). So health restore and life loss are the same seat, and this is it.

       NOT SetPlayerGlobals. That function seats lives to 4 and health to 0x880
       for all four players. In the ROM it has three callers -- StartFile,
       PrepareVsMode and the ov003 title-confirm path func_ov003_020ad814 --
       and the port adds two more, hal/star_flow.cpp:152 (inside
       seat_player_globals, which is port_course_seat's whole body) and
       hal/level_change.cpp:1415 (the port's copy of that same ov003 path).

       Be precise about what those callers are, because an earlier version of
       this note said "none of them a level entry" and that is wrong: StartFile
       and func_ov003_020ad814 both call LoadLevelNoReturn immediately before
       SetPlayerGlobals, so they ARE first-entry paths. (PrepareVsMode is not
       one at all -- it is VS setup and loads no level.) What is true, and is
       the only thing this seat needs, is that NONE OF THEM IS ON THE PER-ENTRY
       OR RESPAWN PATH: they run when a file is started or a mode is prepared,
       never when an already-running file re-enters a level. That is why
       port_course_seat is correctly a once-per-session one-shot, and why
       running it per entry would restore lives to 4 on every respawn and make
       game over unreachable a second way.

       WHY THIS IS UNCONDITIONAL HERE. The ROM's latch is not unconditional in
       its own frame: it sits inside InitResources' once-per-init guard,
       `if (*(s32*)(thiz + 0x9c4) == 0)`. Writing it unguarded here is
       equivalent only because this boot body IS the port's init slot -- it is
       the code that runs where that guarded block runs, once per entry. If the
       boot ever grows a second, non-init caller, this needs the guard back.
       (The A1 geometry regression has no HUD and no Player, so nothing there
       reads the byte either way.) Listed as missing, with its live readers
       named, in port/stage_lifecycle_map.txt:258.

       ONLY THE FIRST OF THE ROM'S THREE STATEMENTS IS PORTED HERE. The ROM
       site is:

           data_0209f2fc = data_0209f26c;
           if (data_0209f2fc == 1) {
               data_02092124 = data_0209f2f8;   <- the level being LEFT
               data_02092118 = -1;
           }

       The `== 1` half is deliberately NOT ported. It is harmless today:
       data_02092124 and data_02092118 are romdata-hosted and the port neither
       writes nor reads them (stage_lifecycle_map.txt:1223), and the image ships
       6 and -1 -- 6 being exactly what StartFile writes -- so the values are
       already the ones a fresh file would have. What the port gives up is that
       data_02092124 is now PERMANENTLY PINNED at 6 instead of tracking the
       level you came from on a fresh entry. Its ROM readers are Stage::Render,
       Stage::LC_Render and Stage::LC_Update, all of which only ask
       SublevelToLevel(it) >= 0xf, so a pinned 6 reads as "not a boss course"
       forever.

       DO NOT COMPLETE THIS AS A TIDY-UP. Writing the missing two lines is not
       two lines of consequence: a data_02092124 that tracks the previous level
       arms func_ov002_020c7cbc -> LoadKeyModels -> the actor 0x11a spawn ->
       func_ov089_0213115c, which has no null check and has never executed on
       the port. That wants its own lane with a fault-fatal run, not a commit
       that is nominally about a comment. */
    data_0209f2fc[0] = data_0209f26c;

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
       see the pre-intro cloud backdrop the bottom screen shows without it.

       AND THE ONE ENTRY THAT WANTS IT CLEAR. port_intro_wants_play() (the seam
       above port_stage_boot_body) is true only for a title-bridge crossing into
       a fresh file, and only with SM64DS_INTRO=1 -- see the seam for the four
       measured gaps that keep it opt-in. On that one boot the bit is left
       ALONE: LoadClsnAndObjects below then takes its own intro branch, declines to
       spawn the HUD exactly as the ROM does, and calls StartIntroCutscene. The
       bit gets set by the ROM's own hand at the end of the flight
       (src/func_ov085_0212d5dc.cpp:51), and the HUD comes up on the next boot.
       LakituBro::InitResources reads the same bit to choose his intro state
       chain, and it runs inside this object pass, so it has to still be clear
       here rather than restored afterwards. */
    const int play_intro = port_intro_wants_play();
    /* ...and the boot that CONTINUES an opening. ProcessKuppaScript's cmd 0x0b
       parked the next script in data_0209fc4c next to the closing
       LoadLevelNoReturn; ContinueKuppaScriptIfNecessary (inside
       LoadClsnAndObjects below) is what consumes it. Non-zero HERE means this
       boot is the opening's second half, and the bit has to stay clear so
       LakituBro::InitResources picks the opening chain data_ov085_02130790 and
       the ROM's own func_ov085_0212d5dc:51 writes it. */
    const int continuing = (data_0209fc4c != 0);
    unsigned char intro_seen = (unsigned char)(data_0209caa0[8] & 0x80);
    if (!play_intro && !continuing)
        data_0209caa0[8] |= 0x80;   /* word 2 bit 7: the intro has played */
    else
        std::fprintf(stderr, "[intro] the opening is ARMED for this entry: "
                     "flags2 bit 7 left clear, the ROM's own gate decides\n");

    /* ---- THE LEVEL MODEL, WHERE THE ROM LOADS IT (run link60, lane SL0) ---
       Stage::InitResources calls Stage::LoadModel at its line 361 and
       Stage::LoadClsnAndObjects at 363, in that order. The port had them the
       other way round: the boot ran the whole object pass and
       port/tests/walk_window.cpp called Stage::LoadModel afterwards. Moving
       the call here is the ROM's order restored, and it is one line.

       WHAT IT UNBLOCKS. Stage::LoadModel is the only writer of data_0209f320,
       the Stage's ModelComponents pointer. daKpa3Bg_c::InitResources
       (src/func_ov060_021182b0.cpp) reaches it through
       CopyTexPalFromLevelModel on its first line, so with the load happening
       after the object pass the pointer was still null when the object pass
       ran and actor id 167 faulted under FAULTS_FATAL. Level 40 is the only
       level of the fifty-two that places 167, and it places ten of them;
       hal/actor_registry.cpp:229-241 declined all ten for exactly this
       reason and named this fix.

       WHY IT HAD TO MOVE RATHER THAN BE ADDED. hal/level_boot.cpp's own
       LoadFile(handle) above WAS a per-level cache, and on a repeat request
       for a handle it already held it returned THE SAME filePtr.
       func_02016ff4 (port/unmatched/func_02016ff4_hostcopy.cpp) then calls
       Model::UpdateFileOffsets unconditionally, and that rebases the BMD's
       file-relative offsets IN PLACE. So two Stage::LoadModel calls inside
       one level would add the base twice and send every pointer in the level
       model out of the file. That is why this is a MOVE and both of
       walk_window's calls go: after it there is exactly one rebase per loaded
       buffer, and the per-level teardown (port_level_reset_host) drops the
       table between levels so the next boot rebases a fresh one.

       THAT SHARING IS GONE (run link60 lane LF1). LoadFile now loads a fresh
       block on every call, which is the ROM's own contract, so a second
       Stage::LoadModel would rebase a second buffer once rather than one
       buffer twice. The MOVE still stands on its own reason -- the ROM runs
       LoadModel before LoadClsnAndObjects and data_0209f320 has to be seated
       before the object pass -- and the hazard this paragraph named is now
       closed by the loader as well as by construction.

       THE GENERAL FIX IS NOT HERE AND IS NOT THIS LANE'S FILE. Keying the
       rebase so a second pass cannot fire whatever the caller does belongs at
       func_02016ff4, the single call site of UpdateFileOffsets on this path,
       in port/unmatched/. This lane closes the hazard by construction for the
       level model and does not touch that file. Note also that the fs.cpp
       cache is NOT the hazard: fs_hand_out memcpys pristine bytes into a
       fresh allocation on every call, so the cache alone never hands back a
       rebased buffer. It is this file's handle table that shares one. */
    {
        void *st = port_stage_object();
        if (st)
            _ZN5Stage9LoadModelEv((char *)st);
    }

    /* WATCHPOINT-EQUIVALENT for the fs floor (run lvled, lane intro-cutscene).
       data_ov085_02130744 reads fileID 291 at the intro seam and 0 by the time
       SharedFilePtr::Load sees it. Release provably cannot zero fileID
       (func_02017c24 clears only the +0x04 buffer pointer), so a WRITE does it.
       These two samples bracket the object pass: if 291 survives the first and
       not the second, the writer is inside LoadClsnAndObjects; if the first
       already reads 0, it is in the boot above. Inert unless SM64DS_INTRO_WATCH
       is set. */
    port_intro_watch("before LoadClsnAndObjects");
    _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider(o, 0, mc);
    port_intro_watch("after LoadClsnAndObjects");
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
    port_minimap_stale_probe("boot done");
    port_lvlperf_note(1, port_lvlperf_now() - lvlperf_t0);
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
void hal_fill_meshcolliderbase_vtable(void);
void hal_seat_meshcollider_dtor(void);
void hal_seat_expandingheap_vmax(void);
void hal_seat_platform_dtors(void);
void hal_fill_enemy_base_vtable(void);
void hal_seat_expandingheap_dtors(void);
void hal_seat_model_family_dtors(void);
void hal_seat_solidheap(void);
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
unsigned char data_ov002_0210a83c[];
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
extern "C" int _ZN9ActorBase19BeforeInitResourcesEv(void *self)
{ return ((ActorBase *)self)->ActorBase::BeforeInitResources() ? 1 : 0; }


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

/* ---- THE PLAYER'S TABLE IS THIRTY-ONE SLOTS, AND ONLY TWENTY WERE SEEDED --
   data_ov002_0210a83c is _ZTV6Player, and it comes from the ov002 per-symbol
   mount -- so slots 20..30 held the ROM's own DS addresses. Real storage, real
   contents, and on host those resolve outside .text. The seed loop below
   stopped at 20, so nothing ever overwrote them, and dispatching slot 25 on
   the Player jumped to a DS address: exit -1073741819.

   This table is invisible to a DECLARATION sweep twice over. It carries a
   plain data name rather than a _ZTV name, the same blind spot that hid
   BILL_BLASTER's table (ov079 0x02127fb8); and there is no array declaration
   here at all, because the width lives in a loop bound. Only comparing what a
   fill WRITES against the ROM span finds it, which is what
   port/tools/vtspan.py --fills does.

   Slots 20..30 are Actor's own interaction list, the same addresses every
   other Actor table carries, and every body is already in the build. Slot 19
   is Actor::OnTurnIntoEgg (arm9 0x02010154, slice_gate50) and was trapped for
   want of seating rather than for a reason.

   THERE IS NO SLOT 31. The Player is a plain Actor, not a Platform, and dsd's
   next symbol (data_ov002_0210a8b8) sits exactly 31 words along -- writing one
   would run off the end of the mounted symbol.

   Reachability of the tail is UNPROVEN and is not claimed here: the wall and
   floor dispatchers run on the player's own collider and look up what the
   player is TOUCHING, so the Player cannot be its own target through them.
   Something else would have to name the Player through a collision result.
   The ROM says what belongs in these slots either way. */
extern "C" {
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);       /* 19 */
int  _ZN5Actor9Virtual50Ev(void *self);                            /* 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);          /* 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);              /* 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);              /* 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);                  /* 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);                  /* 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27 */
/* 28 is declared but deliberately NOT forwarded -- see the slot-28 note in
   hal_fill_player_vtable. Kept so the list reads as the ROM's own. */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);                    /* 29 */
/* 18 is the Player's OWN body (ov002 0x020e69b8), not Actor's: a real MSVC
   method TU (src/_ZN6Player13OnYoshiTryEatEv.cpp, slice_gate16) whose whole
   body is `return 1` -- Yoshi cannot eat the Player. Flat name bridged to the
   method the TU emits; the mangling was read out of the compiled obj. */
int  _ZN6Player13OnYoshiTryEatEv(void *self);                      /* 18 */
}
#pragma comment(linker, "/alternatename:__ZN6Player13OnYoshiTryEatEv=?OnYoshiTryEat@Player@@UAEHXZ")
/* 19 and 21..28 take their argument PUSHED by the __thiscall caller, so each
   thunk needs the dummy edx AND the named parameter or the caller's frame runs
   short. */
static int __fastcall ps_yoshi18(void *s, void *)
{ return _ZN6Player13OnYoshiTryEatEv(s); }
static int __fastcall ps_egg19(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ps_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ps_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ps_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ps_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ps_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ps_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ps_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ps_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ps_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* SLOT 28 ONLY, and the distinction matters to a player. ps_trap below
   abort()s, which takes the process down. The actor tables instead RAISE,
   through port_actor_slot_decline (port/unmatched/func_02043fdc_hostcopy.cpp), and
   port_dispatch_guarded's __except catches that: the actor freezes, a dump is
   filed, the level keeps running. Slot 28 wants the second behaviour, because
   an abort here would be a REGRESSION against what players already survive.

   Scoped to this one seat on purpose. Every other trapping Player slot has
   aborted all along, and converting this file's whole trap convention is a
   broad behaviour change that has no business riding a hotfix. */
extern "C" void port_actor_slot_decline(const char *what);
static int __fastcall ps_decline28(void *, void *)
{
    std::fprintf(stderr,
                 "UNHOSTED: Player vtable slot 28 (OnHitFromUnderneath) is not "
                 "hosted -- two call sites, two conventions\n");
    port_actor_slot_decline("unhosted vtable slot 28 on the Player");
    return 0;
}

#define HAL_PLAYER_SLOTS 31
static const char *const hal_player_slot_name[HAL_PLAYER_SLOTS] = {
    "InitResources", "BeforeInitResources", "AfterInitResources",
    "CleanupResources", "BeforeCleanupResources", "AfterCleanupResources",
    "Behavior", "BeforeBehavior", "AfterBehavior",
    "Render", "BeforeRender", "AfterRender",
    "OnPendingDestroy", "Virtual34", "Virtual38", "OnHeapCreated",
    "~Player (D1)", "~Player (D0)", "OnYoshiTryEat", "OnTurnIntoEgg",
    "Virtual50", "OnGroundPounded", "OnAttacked1", "OnAttacked2",
    "OnKicked", "OnPushed", "OnHitByCannonBlastedChar", "OnHitByMegaChar",
    "OnHitFromUnderneath", "OnAimedAtWithEgg", "OnAimedAtWithEggReturnVec"};
/* hal_player_trap_slot HAD THE SAME DEFECT hal/stage_bridges.cpp's did, and it
   is fixed here for the same reason: it was declared, read, and written
   nowhere in the tree, so one ps_trap in every slot meant every unhosted
   Player dispatch aborted as "slot 0 (InitResources)" whatever really fired.
   On the Stage that cost the 2026-08-16 exit-course report a session spent on
   a slot that was seated all along. The bounds check below is already correct
   -- unlike the Stage's `& 19` -- so only the recording half needs fixing.
   One thunk per slot, the hal/sub_actors.cpp sa_trap13 shape. */
static int hal_player_trap_slot = -1;
static int __fastcall ps_trap(void *, void *)
{
    std::fprintf(stderr, "FATAL: Player vtable slot %d (%s) is not hosted\n",
                 hal_player_trap_slot,
                 (unsigned)hal_player_trap_slot < HAL_PLAYER_SLOTS
                     ? hal_player_slot_name[hal_player_trap_slot]
                     : "the trap did not record its slot -- fix the thunk");
    std::abort();
    return 0;
}

#define HAL_PLAYER_TRAP(n)                                                   \
    static int __fastcall ps_trap##n(void *s, void *d)                       \
    { hal_player_trap_slot = (n); return ps_trap(s, d); }
HAL_PLAYER_TRAP(0)  HAL_PLAYER_TRAP(1)  HAL_PLAYER_TRAP(2)  HAL_PLAYER_TRAP(3)
HAL_PLAYER_TRAP(4)  HAL_PLAYER_TRAP(5)  HAL_PLAYER_TRAP(6)  HAL_PLAYER_TRAP(7)
HAL_PLAYER_TRAP(8)  HAL_PLAYER_TRAP(9)  HAL_PLAYER_TRAP(10) HAL_PLAYER_TRAP(11)
HAL_PLAYER_TRAP(12) HAL_PLAYER_TRAP(13) HAL_PLAYER_TRAP(14) HAL_PLAYER_TRAP(15)
HAL_PLAYER_TRAP(16) HAL_PLAYER_TRAP(17) HAL_PLAYER_TRAP(18) HAL_PLAYER_TRAP(19)
HAL_PLAYER_TRAP(20) HAL_PLAYER_TRAP(21) HAL_PLAYER_TRAP(22) HAL_PLAYER_TRAP(23)
HAL_PLAYER_TRAP(24) HAL_PLAYER_TRAP(25) HAL_PLAYER_TRAP(26) HAL_PLAYER_TRAP(27)
HAL_PLAYER_TRAP(28) HAL_PLAYER_TRAP(29) HAL_PLAYER_TRAP(30)
#undef HAL_PLAYER_TRAP

static void *const hal_player_trap_thunk[HAL_PLAYER_SLOTS] = {
    (void *)ps_trap0,  (void *)ps_trap1,  (void *)ps_trap2,  (void *)ps_trap3,
    (void *)ps_trap4,  (void *)ps_trap5,  (void *)ps_trap6,  (void *)ps_trap7,
    (void *)ps_trap8,  (void *)ps_trap9,  (void *)ps_trap10, (void *)ps_trap11,
    (void *)ps_trap12, (void *)ps_trap13, (void *)ps_trap14, (void *)ps_trap15,
    (void *)ps_trap16, (void *)ps_trap17, (void *)ps_trap18, (void *)ps_trap19,
    (void *)ps_trap20, (void *)ps_trap21, (void *)ps_trap22, (void *)ps_trap23,
    (void *)ps_trap24, (void *)ps_trap25, (void *)ps_trap26, (void *)ps_trap27,
    (void *)ps_trap28, (void *)ps_trap29, (void *)ps_trap30};

extern "C" void hal_fill_player_vtable(void)
{
    void **vt = (void **)data_ov002_0210a83c;
    for (int i = 0; i < HAL_PLAYER_SLOTS; ++i)
        vt[i] = hal_player_trap_thunk[i];
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
    /* 18..29, the ROM's own contents. 17 (D0) keeps the trap on purpose, per
       the note above. 18 is the Player's OWN OnYoshiTryEat (ov002 0x020e69b8),
       seated now that its TU rides slice_gate16 -- while it was in no slice
       the slot trapped, since forwarding to Actor's would run the wrong code
       rather than less code. 30 keeps the trap -- its ROM body returns a
       Vector3 by value and the sret contract is unproved, the reading every
       other table in the port takes. */
    vt[18] = (void *)ps_yoshi18;
    vt[19] = (void *)ps_egg19;
    vt[20] = (void *)ps_v50;
    vt[21] = (void *)ps_pounded;
    vt[22] = (void *)ps_atk1;
    vt[23] = (void *)ps_atk2;
    vt[24] = (void *)ps_kicked;
    vt[25] = (void *)ps_pushed;
    vt[26] = (void *)ps_cannon;
    vt[27] = (void *)ps_mega;
    /* SLOT 28 DECLINES, and it is the one slot here that cannot be seated at
       all with a single thunk. Actor slot 28 has TWO call sites in this binary
       with INCOMPATIBLE conventions:

         thiscall  func_ov002_020eeca8+0x44   call [reg+0x70]
                   -- `this` in ecx, the argument pushed, CALLEE pops
         cdecl     func_ov002_020cef84+0x23f  mov eax,[reg+0x70]; call eax;
                                              add esp,8
                   -- (self, a) both pushed, CALLER pops 8

       A __fastcall thunk that forwards Actor::OnHitFromUnderneath emits
       `ret 4`. That satisfies the thiscall site and BREAKS the cdecl one: the
       caller pushes 8, the callee pops 4, the caller pops 8, and esp ends four
       bytes high. It also reads `this` from ecx while the cdecl site passed it
       on the stack, so it gets the wrong argument as well. No single word in
       this slot satisfies both conventions.

       That trade is bad in the direction that matters. An unseated slot is a
       null/DS-address call: loud, named in a dump, and caught. A four-byte
       stack imbalance escapes the quarantine net entirely and fails later with
       nothing attached to it. So this declines by name until the call is
       modelled uniformly -- strictly better than either failure.

       FOLLOW-UP, not done here and wanting its own review: a host copy of
       func_ov002_020cef84 in port/unmatched modelling that call as a C++
       virtual, so both sites become thiscall and the slot can hold the real
       body. That is the port's established pattern and leaves src untouched.

       It DECLINES rather than TRAPS. ps_trap abort()s, and an abort here
       would be worse for a player than the unseated slot they have today: an
       unseated slot is an execute-at-null that the quarantine net catches, so
       the level survives with a dump. ps_decline28 raises the same catchable
       fault the actor tables raise, so this slot is named and attributed like
       a trap AND survivable like the quarantine. Best of the three outcomes,
       and the only one that is not a regression against the live build.

       Derived from binary arithmetic on the cons build rather than from a
       repro, and recorded that way on purpose. */
    vt[28] = (void *)ps_decline28;
    vt[29] = (void *)ps_aimed;
}

/* The per-frame tick the ROM's processing list runs on every actor:
   func_02043288 = ActorBase::Process over slots 7/6/8. Actor::BeforeBehavior
   is the half the harness never had -- it is what copies pos into PREV POS,
   and prev pos is the start of every line WithMeshClsn's continuous update
   casts. Driving Behavior bare left prev at the constructor's zero, so the
   first frame at the gate swept a segment from the world origin. */
/* THE INTRO-SEEN BIT, EDGE-TRIGGERED. The completion bar for the opening asks
   that flags2 bit 7 ends SET and that the port never sets it -- the write is
   src/func_ov085_0212d5dc.cpp:51, LakituBro's last opening state, right after
   it hands control back to the player. A boot-time read cannot show that: on
   the reload boot the bit is still clear (measured), because the flight's
   ending state runs during the level that follows. So this reports the EDGE,
   once, from a place that ticks. Inert unless SM64DS_INTRO_WATCH. */
extern "C" void port_intro_bit_edge(void)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_INTRO_WATCH") ? 1 : 0;
    if (!on)
        return;
    static int last = -1;
    const int now = (data_0209caa0[8] & 0x80) ? 1 : 0;
    if (now != last) {
        std::fprintf(stderr, "  [intro] flags2 bit 7 %d -> %d\n", last, now);
        last = now;
    }
}

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

/* ===========================================================================
 * THE SECOND PLAYER'S SPAWN POINT, and why the port has to supply one.
 *
 * _Z19LoadEntranceObjects... spawns player i from ENTRANCE RECORD p3 + i --
 * its `e++` at the bottom of the loop -- so a level whose entrance table
 * carries four consecutive player starts spawns four players and one that does
 * not, does not. SM64DS's VS arenas have those consecutive records. Castle
 * grounds does not: record p3+1 is entrance 1 of 14, a different door, and
 * Actor::Spawn makes whatever that record names, which is not a Player.
 *
 * MEASURED, not assumed, and this is the shape the bug presents in:
 *     [vs] LoadEntranceObjects: count=2 p3=0 live=1,1 chars=0,1
 *     [vs] f0 count=2 me=0 live=1,1,0,0
 *     [vs] f0 slot1 actor=NULL
 * Every input the loop reads is correct and the slot is still empty, because
 * the thing that is wrong is the DATA the loop reads, not the loop.
 *
 * So the port supplies the missing starts, from player 0's OWN record, and
 * says so rather than pretending the level had them. Everything else about
 * each spawn is the ROM's: the same Actor::Spawn, the same class id out of
 * data_ov002_0210cbf4, the same flag packing including `(i << 6)` -- which is
 * what Player::InitResources unpacks into mPlayerNo, so the actor knows which
 * comms slot it is -- and the same area and rotation.
 *
 * THEY ARE PLACED DELIBERATELY OVERLAPPING, 40.0 units apart on x against body
 * cylinders of radius 40.0 each. Two cylinders 40 apart with a combined radius
 * of 80 overlap by 40, so CylinderClsn::Process's symmetric branch has real
 * work to do on the very first frame and BOTH positions move. That makes the
 * collision proof decisive on frame 1 rather than dependent on driving two
 * characters together, and it is a legitimate arena start: VS arenas spawn
 * players close and the ROM's own solver is what separates them.
 *
 * RETIREMENT: when ov075 (the VS menu) is mounted and a real VS arena is
 * loaded, its entrance table carries the consecutive starts and this function
 * finds every slot already filled and does nothing. It is a stand-in for
 * missing level data, not a replacement for the ROM's spawn path.
 * =========================================================================== */
extern "C" void port_vs_spawn_extra_players(void *tbl, unsigned p3)
{
    struct Entry { unsigned short raw; short x, y, z; short rx, ry, rz;
                   unsigned short param; };
    struct ObjSubTable { unsigned char pad0, count, pad2[2]; Entry *entries; };

    ObjSubTable *t = (ObjSubTable *)tbl;
    const int n = (int)data_0209f21c;
    if (n < 2 || !t || !t->entries) return;
    if ((unsigned)t->count <= p3) return;

    Entry *base = t->entries + p3;      /* player 0's own record */
    const unsigned sl = (unsigned)((base->param >> 7) & 0xf);
    const int area = (int)(signed char)(base->param & 7);

    for (int i = 1; i < n && i < 4; ++i) {
        if (data_0209f394[i]) continue;             /* the level had a start */
        if (data_0209fc5c[i] == 0) continue;        /* slot not live */

        int pos[3];
        pos[0] = (base->x << 12) + i * (40 << 12);  /* 40.0 units apart: OVERLAP */
        pos[1] = base->y << 12;
        pos[2] = base->z << 12;

        /* THE CHARACTER IS FLAG BITS 0..2, NOT BITS 3..5, and getting that
           backwards is why both players came up Mario in every capture this
           lane banked -- the contradiction the review caught between this
           function's own comment and MP3.md's gap note. Re-derived from
           src/_ZN6Player13InitResourcesEv.cpp:71-77 rather than from either
           claim:

               a = *(u32 *)(c + 8);                 the spawn flags
               *(u8 *)(c + 0x6d8) = (a >> 6) & 3;   mPlayerNo   <- (i << 6)
               sub                = (a >> 3) & 7;               <- f1
               *(u8 *)(c + 0x6d9) =  a       & 7;   CHARACTER   <- f2

           So the byte the model is chosen from is f2, which in the ROM's own
           loop is data_0209caa0[0x41] -- the SAVE FILE's character, the same
           for every slot. data_02092128[i] lands at +0x6da as `sub`, and :83-87
           then writes data_02092128[playerNo] BACK from the character whenever
           the two disagree, which is the table rewrite measured mid-boot.

           So both halves carry the port's choice: f2 so the actor comes up as
           that character, f1 so `sub` agrees with it and the ROM's own
           reconciliation has nothing to correct. The ROM's table is re-seated
           to match for anything downstream that reads it. */
        const unsigned ch = (unsigned)port_vs_character(i);
        const unsigned f2 = ch;
        const unsigned f1 = ch;
        data_02092128[i] = (unsigned char)ch;
        const unsigned flags = f2 | (f1 << 3) | ((unsigned)i << 6) | (sl << 8);

        void *a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            data_ov002_0210cbf4[base->raw], flags,
            (const PortVec3 *)pos, (const PortVec3_16 *)&base->rx, area, -1);
        data_0209f394[i] = a;
        std::fprintf(stderr,
                     "[vs] port-supplied start for slot %d: actor=%p "
                     "char=%u pos=(%d,%d,%d) (the level's entrance table has "
                     "no player start at record %u)\n",
                     i, a, f1, pos[0], pos[1], pos[2], p3 + i);
    }
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

/* The actor's own area: mAreaId, the s8 at Actor+0xcc (include/Actor.h),
   seated by the Actor constructor from data_0209b44c -- the byte
   func_02010e78 writes from Actor::Spawn's area argument. NOT the byte at
   +0x10: the ActorBase constructor never writes +0x10 (its stores are
   0x00/0x04/0x08/0x0c/0x12/0x13), AfterInitResources keeps a boolean there,
   and this path read that boolean as the area for a while -- masked because
   area 0 is the common case and the boolean usually reads 0.
   hal/editor_channel.cpp's OFF_ACTOR_AREA is the same byte, and the ROM's
   own GetMinimapID reads obj->+0xcc as this area index -- see the minimap
   prose further down this file. */
static int port_actor_area(const void *actor)
{
    return *(const signed char *)((const char *)actor + 0xcc);
}

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
    /* Regression assertion for the +0x10/+0xcc confusion above: the area this
       call asked for must be the one the constructor seated. The report line
       below prints the READBACK, not the argument, so "area N" in the log is
       the actor's own field -- spawn one at a nonzero area and the log has to
       say so or this fires. */
    if (a && port_actor_area(a) != (int)(signed char)area)
        std::fprintf(stderr, "  [dbgspawn] REGRESSION: asked for area %d and "
                     "the spawned actor's mAreaId (+0xcc) reads %d -- some "
                     "part of the spawn path is on the wrong byte again\n",
                     area, port_actor_area(a));
    std::printf("[dbgspawn] actor %u (%s) param 0x%x at (%d, %d, %d) yaw %04x "
                "area %d -> %p\n", id, port_actor_class_name(id), param,
                x >> 12, y >> 12, z >> 12, (unsigned short)yaw,
                a ? port_actor_area(a) : area, a);
    return a;
}

/* The common case: at the local player, facing the way he faces, in his area.
   Player pos is +0x5c..0x64 and his facing yaw is +0x8e; the area is his own
   mAreaId, read through port_actor_area above (it used to be the +0x10
   boolean -- see that comment). `has_area` overrides his area with `area`,
   which is what the @<area> spelling of SM64DS_SPAWN_ACTOR feeds through.
   Falls back to the world origin when no player exists yet, so an early call
   still reaches the registry gate rather than dereferencing null. */
static void *port_debug_spawn_area(unsigned id, unsigned param,
                                   int area, int has_area)
{
    const char *p = (const char *)data_0209f394[0];
    if (!p) {
        std::fprintf(stderr, "  [dbgspawn] no player yet, spawning actor %u "
                     "at the origin\n", id);
        return port_debug_spawn_at(id, param, 0, 0, 0, 0,
                                   has_area ? area : 0);
    }
    return port_debug_spawn_at(id, param, *(const int *)(p + 0x5c),
                               *(const int *)(p + 0x60),
                               *(const int *)(p + 0x64),
                               *(const short *)(p + 0x8e),
                               has_area ? area : port_actor_area(p));
}

extern "C" void *port_debug_spawn(unsigned id, unsigned param)
{
    return port_debug_spawn_area(id, param, 0, 0);
}

/* SM64DS_SPAWN_ACTOR=<id>[:<param>][@<area>][,<id>[:<param>][@<area>]...]
   fires the same hook once, right after the boot, so a class can be exercised
   from the command line with no rebuild. Ids are decimal or 0x-prefixed. An
   explicit @<area> spawns at the player's spot but seats that area instead of
   his own -- the headless probe for the mAreaId read: spawn one @2 and the
   [dbgspawn] line must report area 2 (it prints the spawned actor's readback,
   and port_debug_spawn_at cries REGRESSION on a mismatch). */
extern "C" void port_debug_spawn_env(void)
{
    const char *s = std::getenv("SM64DS_SPAWN_ACTOR");
    if (!s)
        return;
    while (*s) {
        char *end;
        unsigned id = (unsigned)std::strtoul(s, &end, 0);
        unsigned param = 0;
        int area = 0, has_area = 0;
        if (end == s) {
            std::fprintf(stderr, "  [dbgspawn] SM64DS_SPAWN_ACTOR: cannot read "
                         "an id at \"%s\"\n", s);
            return;
        }
        s = end;
        if (*s == ':')
            param = (unsigned)std::strtoul(s + 1, (char **)&s, 0);
        if (*s == '@') {
            area = (int)std::strtol(s + 1, (char **)&s, 0);
            has_area = 1;
        }
        port_debug_spawn_area(id, param, area, has_area);
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
extern int data_0208ee44;              /* vblanks per game tick */
extern unsigned char data_0209f21c;    /* controller count */
extern unsigned char data_0209f250;    /* local player index */
extern int data_0209fc5c[];            /* per-player "this slot is live" */
extern unsigned char data_02092128[];  /* per-player character */
/* run mg16 lane MP3: the per-player input gate Player::Behavior tests before it
   will point data_020a0e40 at its own slot. Hosted by hal/auto_bss.cpp. */
extern int data_0209fc68[];
/* run mg16 lane MP3: the ROM's own "which comms slot am I", linked since MP1
   and never called until now. src/func_0203da9c.c is `return data_020a0f10`. */
int func_0203da9c(void);
/* and the ROM's own player-count setter: data_0208a0e0 = n plus func_020308d0.
   Stage::InitResources:153 copies data_0208a0e0 into data_0209f21c, so this is
   the write that actually survives into the spawn loop. */
void SetNumPlayers(int n);
/* run mg16 lane MP3: the port's own per-slot character choice, defined above
   port_load1. It exists because the ROM rewrites data_02092128 during the boot. */
void port_vs_set_character(int slot, int ch);
int  port_vs_character(int slot);
extern signed char data_02092120;      /* currently shown area, -1 = none */
extern int data_0209f32c[];            /* water level */
/* data_0209fc48 (the running cutscene) is DEFINED above, in the retirement
   block, and must not be re-declared here -- see the note at the other
   removed declaration and in hal/dsstate_seg.h. */
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

/* CAPTURED, and it is g_level_mounted's argument again with a different
   payload. What this flag says is "the twelve preloads have run", and what that
   pass writes is the twelve SharedFilePtr records themselves: Model::LoadFile
   fills fileID, numRefs and filePtr in each. They are hosted ov002 globals, so
   they are .dsstate content and a restore rolls them back. A host static does
   not roll back with them, and the two then disagree in the fatal direction --
   the flag says done, the records read unloaded, the pass never runs again, and
   the first type-11 mushroom walks the null BMD_File described at the top of
   this block into Model::AddToCommonModelDataArr.

   AND THE DISAGREEMENT IS REACHABLE, which is worth stating because "loaded"
   looks like a one-way latch and is not: SharedFilePtr::Release decrements
   numRefs and frees at zero. Counted over the tree rather than eyeballed, and
   the count is uneven: FOUR of the twelve are released anywhere at all --
   0210da40, 0210d9a0, 0210d9c0 and 0210d9a8 -- by fourteen classes
   (BowserPuzzlePiece, Coin, Dorrie, InvisibleSecret, Klepto, MantaRay, Player,
   QuestionBlock, RollingLogTtm, SnowmanBreath, StarMarker, Stump, Toad,
   TreasureChest) plus one free function, func_ov002_020f069c; every site but
   that last is a CleanupResources body. The other EIGHT are released nowhere,
   so once the seat has run they stay loaded for the life of the process. It is
   the four that make a disagreement reachable: one of those can be back to
   unloaded inside a session, so a state can hold it either way round.

   THE SWEEP MISSED THIS ONE and the reason generalises: RELOAD2's sweep
   enumerated callers of port_ovNNN_syms_patch and __sinit_ovNNN_*, and this
   pass calls neither. Its callee is Model::LoadFile. A search keyed to the two
   patch-pass names cannot see a one-shot guard whose payload is a LOADER, so
   the family is wider than that pattern and the next audit should be keyed on
   which side of the section the payload lands on, not on the callee's name.

   THE RE-RUN IS SAFE, read out of the callees rather than assumed.
   SharedFilePtr::LoadFile calls Load() only when numRefs is 0, and
   Model::LoadFile does UpdateFileOffsets + AddToCommonModelDataArr +
   ReallocateModelFile only when numRefs comes back 1, so the expensive half is
   the first load and nothing else. The storage does not leak either: Load()
   carves through Memory::Allocate, which comes out of the root heap arena, and
   hal/lk6_savestate.cpp captures that arena AND its carve cursor
   (Slot.arena/Slot.arena_cursor) and puts both back -- so a rollback un-carves
   the block in the same motion that rolls the record back. Bracketed, flag and
   records move together, so the one case that would drift a refcount, a re-run
   over a record that is still loaded, cannot arise from a restore at all.

   NOT IN THE SM64DS_SS_NO_ROLLGUARD STASH, and that is deliberate: it matches
   the eighteen rather than the two. port_rollguard_stash below carries
   g_level_mounted and ov009's sinit flag, the pair RELOAD2's A/B actually
   measured; the bracketed level-path guards are not in it. Adding this one
   would WIDEN the knob rather than keep it the before/after switch the soak
   reads, so the knob's scope stays what it was.

   tbl stays host-side on purpose: it is const, nothing writes it, and it holds
   host addresses that are fixed for the process lifetime. */
DSSTATE_BEGIN
static int g_preload_shared_models_done;
DSSTATE_END

extern "C" void port_stage_preload_shared_models(void)
{
    static void *const tbl[12] = {
        data_ov002_0210da48, data_ov002_0210d9b8, data_ov002_0210da50,
        data_ov002_0210d9f8, data_ov002_0210da40, data_ov002_0210d9a0,
        data_ov002_0210d9c0, data_ov002_0210e7d8, data_ov002_0210e3a0,
        data_ov002_0211094c, data_ov002_0211095c, data_ov002_0210d9a8,
    };
    int loaded = 0;
    if (g_preload_shared_models_done)
        return;
    g_preload_shared_models_done = 1;
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
extern "C" void port_scene_registry_install(void);   /* hal/scene_boot.cpp */

/* THE SEAT, WITH ONE PARAMETER: does this run get a Stage?
   A LEVEL run does. A SCENE run does not, and that is the only difference
   between the two boots at this level of the stack. On the DS the Stage IS the
   level scene -- ACTOR_SPAWN_TABLE[3] is its record and GetSceneOverlayID sends
   ids 3/6/7 to ov002 -- so a run that is entering the star select (id 4) or the
   level select (id 2) must not construct one: the scene the spine is about to
   spawn is what becomes the tree root, exactly the way the Stage does on a
   level, through the same no-parent branch in func_0203b438.
   Everything else here is bring-up both modes need, so both take it. */
/* THE STAGE HALF, lifted out so it can be reached on its own. Called from the
   body below in its original position on a first call, and on its own when the
   title-entry bridge asks for a Stage after a scene run already took the seat.
   Its own guard is in port_stage_create, which refuses to build a second one. */
static void port_a2_seat_stage(void)
{
    /* the scene tree root the spawn spine links under -- the real Stage.
       Constructing it IS the seating: Stage::Stage runs with data_020a4b6c[0]
       still null, so func_0203b438 takes its no-parent branch and writes the
       Stage's own SceneNode into the tree head, which is how the ROM's tree
       gets its root. port_stage_create asserts that it did. */
    void *stage = port_stage_create();
    /* Scene::ResetFadersAndSound OWNS THIS LINE NOW (_ZTV5Stage slot 1,
       run link60 lane L4): its first statement is `data_0209f5c0 = self`
       and it runs on every level entry through the init Process. This seat
       stays because the port can spawn before the first level boot -- the
       scene registry is installed here too -- and func_02042ffc refuses to
       spawn anything under a null parent. It is a bootstrap for the window
       between process start and the first boot, not a per-entry stand-in. */
    data_0209f5c0[0] = (int)(size_t)stage;
}

/* ---- THE SEAT RUNS ITS SHARED HALF ONCE PER PROCESS (run lvled) ------------
 *
 * Until the title-entry bridge existed, exactly one of the two callers ran, one
 * time: a level run took port_stage_a2_seat and a scene run took
 * port_scene_a2_seat, and neither could follow the other. So "once" was a
 * property of the call sites and nothing here had to hold it.
 *
 * SM64DS_TITLE_ENTRY=1 makes a run take BOTH, in order: the title comes up on
 * the scene path, a save file is picked, and tests/walk_window.cpp falls
 * through to its own level boot in the same process. That second call used to
 * abort, and the abort was correct rather than incidental:
 *
 *     FATAL: SignPost state 0 Init: the sinit left 004308b0, the ROM's own
 *     table says 020bba24 -- WRONG BYTES
 *
 * port_actor_registry_install's vtable fills carry VERIFY-THEN-REWRITE seats --
 * port_sign_post_states_seat is the one that fired, and it is not alone -- which
 * read the DS address a static initialiser left, assert it, and overwrite it
 * with the host body. Run a second time they find their own host address where
 * they expect the ROM's and refuse. THAT REFUSAL IS A REAL DRIFT DETECTOR and
 * hal/actor_classes.inc's wave-10 note is the tree already saying so in print:
 * "A sibling's fill may only be reused from another row when it is IDEMPOTENT;
 * one carrying a seat is not." Blunting those guards to allow a second pass
 * would trade a live check for a convenience.
 *
 * So the seat is not made idempotent -- it is made SINGLE. This is a bring-up,
 * it belongs to the process rather than to the mode, and running it once is
 * what both call sites already believed they were getting. For every path that
 * existed before this lane the guard is a no-op: one caller, one call, one run.
 * ------------------------------------------------------------------------- */
static void port_a2_seat_body(int make_stage)
{
    static int seat_done;
    static int stage_done;

    if (seat_done) {
        /* A SECOND CALL, which today only the title-entry bridge can produce.
           The shared bring-up is already standing; what a level still needs and
           a scene run did not build is the Stage. */
        if (make_stage && !stage_done) {
            stage_done = 1;
            port_a2_seat_stage();
            std::printf("[a2] scene root %p (Stage seated after a scene run; "
                        "the shared bring-up was already done)\n",
                        port_stage_object());
        }
        return;
    }
    seat_done = 1;
    if (make_stage)
        stage_done = 1;

    port_message_archive_seat();
    port_stage_preload_shared_models();

    /* Which levels this build can mount. Registered before anything can ask,
       which is here rather than in main: the handoff seam is boot state like
       the registry and the sinits below it. */
    port_level_mounts_install();

    if (make_stage)
        port_a2_seat_stage();

    /* ---- HOW MANY PLAYERS, AND WHICH ONE AM I (run mg16, lane MP3) ---------
     *
     * These four lines used to be four constants: one player, index 0, Mario,
     * slot 0 live. That is what a single-player port needs and it is ALSO the
     * only thing standing between this build and two players in one world --
     * which is worth stating plainly, because the multiplayer work looked from
     * the outside like it needed a remote-player actor and a spawn path, and it
     * needs neither.
     *
     * THE ROM HAS NO REMOTE-PLAYER CONCEPT. It spawns ONE Player actor PER
     * COMMS SLOT: _Z19LoadEntranceObjects...'s loop runs `i < data_0209f21c`,
     * packs the slot into the spawn flags as `(i << 6)`, and Player::
     * InitResources unpacks it into mPlayerNo at +0x6d8. Player::Behavior then
     * sets data_020a0e40 = mPlayerNo for the length of its own tick, so every
     * downstream `data_020a0e58[data_020a0e40 * 4]` read resolves to THAT
     * player's pad. Remote input is that one assignment, and it is the ROM's.
     *
     * data_0209f21c == 1 is therefore the whole blocker: the spawn loop runs
     * once, Stage::CheckInput fills only Ctrl[0], and CylinderClsn::Process's
     * pairwise walk has a one-element list so two players could never touch
     * each other either.
     *
     * WHAT DECIDES THE COUNT IS THE SESSION, not a knob. port::vs_player_count()
     * reads the seam: with no transport installed it answers 1 and these lines
     * are byte-for-byte what they were, which is what keeps every single-player
     * baseline in this tree valid. With a transport connected it answers the
     * live player count the ROM's own conductor negotiated.
     *
     * data_0209f250 stops being a constant too. The ROM's own spelling is
     * Stage::InitResources:154, `data_0209f250 = func_0203da9c()`, and
     * func_0203da9c returns data_020a0f10 -- MY COMMS SLOT. Hardcoding it to 0
     * meant both instances believed they were player 0, so both would have
     * driven the same actor and neither would have driven the other.
     * func_0203da9c has been linked and never called for the whole life of this
     * port; this is the call site it was waiting for. */
    /* THE SESSION COMES FIRST, THEN THE WORLD. Run mg16 lane MP3, field
       failure. Everything below asks the seam who I am and how many of us
       there are, and neither answer exists until the link is up:
       data_020a0f10 is written by src/func_0203ea5c.c:252, which runs only
       once a round has completed. Seating a world before that read 0 on BOTH
       consoles, so both believed they were player 0 and the child drove the
       host's character. On the DS the menu joins before anything loads a
       level; this is that ordering, restored. No-op with no transport. */
    port::comms_wait_for_session(600);

    const int vs_players = port::vs_player_count();
    /* ---- THE PER-PLAYER INPUT GATE (run mg16, lane MP3) -------------------
     *
     * Without this, two Player actors exist and BOTH READ PLAYER 0'S PAD, so
     * the second one mirrors the first exactly and no input from anywhere can
     * ever reach it. src/_ZN6Player8BehaviorEv.cpp:111-122 is the gate:
     *
     *     if (data_0209fc68 == 0) {
     *         ... if (data_0209fc48 == 0) { data_020a0e40 = 0; goto skip; }
     *     }
     *     data_020a0e40 = mPlayerNo;
     *
     * data_020a0e40 is "which player is being ticked", and every downstream
     * data_020a0e58[data_020a0e40 * 4] read resolves through it. With the gate
     * clear it is pinned to 0 for every actor; with it set each Player reads
     * its OWN slot, which is the entire remote-input mechanism.
     *
     * SEATED DIRECTLY RATHER THAN THROUGH THE ROM'S SETTERS, and the reasoning
     * is proportionality. The ROM writes this from six session functions
     * around 0x02030xxx (~0x420 bytes) that are matched but unlinked, and every
     * one of them is about entering a multiplayer SESSION -- menus, mode
     * selection, the things ov075 drives. Linking six functions to set one flag
     * would pull that whole surface in for a byte, and this lane's own rule has
     * been to move the ROM's code in when the ROM's code is what runs. Nothing
     * here runs: the flag is a precondition, and the port is standing in for
     * the menu that would set it, exactly as it does for the role byte in
     * hal/comms_conductor.cpp. When ov075 lands, its setters own this and this
     * line goes.
     *
     * ONE PLAYER LEAVES IT CLEAR, so the single-player path is untouched and
     * data_020a0e40 stays pinned at 0 exactly as it always was. */
    data_0209fc68[0] = (vs_players > 1) ? 1 : 0;
    /* THROUGH THE ROM'S OWN SETTER, and the first version of this did not, which
       is worth recording because it looked like it worked. Writing
       data_0209f21c here seats it -- and then Stage::InitResources:153 runs and
       does `data_0209f21c = data_0208a0e0`, putting it straight back to 1. The
       count the game actually uses lives in data_0208a0e0, and SetNumPlayers is
       the ROM's own way to write it (it also calls func_020308d0, which this
       must not skip). Measured: the boot log said "2 players" while the probe
       reported one actor on all 300 frames. data_0209f21c is still seated too,
       for the window before InitResources runs. */
    SetNumPlayers(vs_players);
    data_0209f21c = (unsigned char)vs_players;
    data_0209f250 = (unsigned char)func_0203da9c();
    for (int i = 0; i < vs_players; ++i) {
        data_0209fc5c[i] = 1;          /* this slot is live */
        data_02092128[i] = (unsigned char)i;   /* character: see below */
        port_vs_set_character(i, i);   /* and the port's own copy: see below */
    }
    /* THE PORT KEEPS ITS OWN COPY OF THE CHARACTER CHOICE, and that is not
     * belt-and-braces -- data_02092128 IS REWRITTEN DURING THE LEVEL BOOT.
     * Measured: the loader witness prints chars=0,1 on the way into
     * LoadEntranceObjects and port_vs_spawn_extra_players, which runs
     * immediately after it, reads 0 for slot 1. Something inside the spawn
     * (Player::InitResources seats the array from the save block) puts player
     * 0's character back across every slot.
     *
     * That is the ROM's table and the ROM's business. What is NOT the ROM's is
     * the choice for the EXTRA players, because the port is standing in for the
     * VS menu that would make it, so the port holds that decision in its own
     * storage instead of leaving it in a table the ROM owns and overwrites. */
    /* CHARACTER PER SLOT, and it is a placeholder with a reason rather than a
     * decision. The ROM picks these in the VS menu (ov075), which is not
     * mounted yet, so slot i gets character i -- 0 Mario, 1 Luigi, 2 Wario,
     * 3 Yoshi -- which is the character ORDER the VS menu offers and makes the
     * two players visually distinguishable in a proof capture, which is the
     * point at this stage. When ov075 lands, its own selection replaces this. */
    if (vs_players > 1)
        std::fprintf(stderr,
                     "  [a2] VS: %d players, I am slot %d, characters",
                     vs_players, (int)data_0209f250);
    if (vs_players > 1) {
        for (int i = 0; i < vs_players; ++i)
            std::fprintf(stderr, " %d", (int)data_02092128[i]);
        std::fprintf(stderr, "\n");
    }
    /* data_0209caa0[0x41], which is byte 0xf of data_0209cad2 -- the third
       symbol of the run. Spelled at its owner rather than as an index past
       the first symbol's declared 0x14 bytes, which MSVC turns into a
       compile-time range check and a fast-fail. */
    if (data_0209cab4 - data_0209caa0 != 0x14 ||
        data_0209cad2 - data_0209caa0 != 0x32 ||
        data_0209cae4 - data_0209caa0 != 0x44 ||
        data_0209caf4 - data_0209caa0 != 0x54)
        std::fprintf(stderr,
                     "  [a2] SAVE BLOCK NOT CONTIGUOUS: +%d +%d +%d +%d\n",
                     (int)(data_0209cab4 - data_0209caa0),
                     (int)(data_0209cad2 - data_0209caa0),
                     (int)(data_0209cae4 - data_0209caa0),
                     (int)(data_0209caf4 - data_0209caa0));
    data_0209cad2[0x41 - 0x32] = 0;

    /* THE FRAME DIVIDER, src/_ZN5Stage13InitResourcesEv.cpp:362. One of the
       twenty InitResources global writes section 2d of port/stage_lifecycle_map.txt
       lists as ABSENT from the port, and one of the five it lists as having a
       live reader. The word is vblanks-per-game-tick: IRQ::VBlankHandler only
       wakes the main thread once its vblank count reaches it, so 2 is the 3D
       levels' 30fps and 1 is the minigames' 60. The ROM IMAGE'S STATIC VALUE
       IS 1, so a level that never ran this line was sitting on a minigame's
       divider for the whole session.

       Two things read it and both were wrong without this.
       hal/message_pump.cpp:256 uses it as the STEP a message window's close
       timer counts down by (`data_0209d67c -= data_0208ee44`), so on a level
       every message hung on screen for twice its ROM duration. And the host
       frame pacer in tests/walk_window.cpp now takes the loop's budget from
       it, which is what surfaced this: the level loop asked for the divider
       and got a minigame's answer. */
    data_0208ee44 = 2;

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

    /* Batch-2 linkage seat: the MeshColliderBase base table's own matched
       bodies. C2 installs the vptr; this fill names the bodies so they link.
       Present only in the slice_gate16 targets (walk_window family). */
    hal_fill_meshcolliderbase_vtable();

    /* Batch-3 linkage seat: the concrete MeshCollider's own deleting dtor (D0)
       into slot 0 of _ZTV12MeshCollider, and D1 kept referenced. walk_window
       family only; the gate-8/9 smoke targets keep the trap (they never delete
       the level collider). */
    hal_seat_meshcollider_dtor();

    /* Lane-lk1 linkage seat: ExpandingHeap's VMax pair into slots 10/11 of
       _ZTV13ExpandingHeap. walk_window family only; the pair's allocator
       callee needs gate 4b's cstd::abs, which the minimal root-heap targets
       do not link, so those keep the traps (no caller evidence anywhere). */
    hal_seat_expandingheap_vmax();

    /* Linkage seat: Platform's own destructor pair into slots 16/17 of the
       hosted base table (both storage names), the bodies the ROM's table at
       ov002 0x0210ae38 carries there. walk_window family only, next to the
       collider seats above; the base table is only installed mid-teardown and
       nothing dispatches through it, so the seat is the ROM's contents where
       the trap prefill stood. hal/lk2_platform_dtor_seat.cpp. */
    hal_seat_platform_dtors();

    /* Linkage seat: the Enemy base table (data_ov002_021081e4), same reading
       as the Platform base pair -- installed only mid-teardown, never
       dispatched through; the fill gives it the shared half plus the class's
       own destructor pair, the ROM's contents. hal/actor_classes.cpp. */
    hal_fill_enemy_base_vtable();

    /* Lane-lk4 linkage seat: ExpandingHeap's dtor chain (D1/D0/VDestroy) and
       its last two allocator forwarders (VDeallocateAll/VMemoryLeft) into
       slots 0/1/2/5/12, closing the class. walk_window family only; the D0
       closure needs Memory::operator_delete2 (gate 16), which the minimal
       root-heap targets do not link, so those keep the traps. */
    hal_seat_expandingheap_dtors();

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

    /* Lane-lk4 linkage seat: the model family's own deleting dtors (D0) into
       slot 0 of the five hosted primary tables. AFTER the registry install on
       purpose: King Bob-omb's registry fill rewrites the BlendModelAnim slot
       with its no-op, and this seat has to win every level boot. walk_window
       family only (hal/model_dtor_seat.cpp rides the same targets as the
       MeshCollider dtor seat). */
    hal_seat_model_family_dtors();

    /* Lane-lk4 linkage seat: the solid-heap face. SolidHeap's own sixteen
       slot table filled with its matched bodies, and ActorBase::Virtual34/38
       (the per-instance heap hooks) into slots 13/14 of _ZTV5Actor, after
       the registry install's STAR_CAMERA fill trapped them. walk_window
       family only (hal/lk4_solidheap_seat.cpp). */
    hal_seat_solidheap();

    /* THE ov003 SCENE CLASS, seated the same way and in the same place as the
       level cast: dScStarSel_c (id 4, the star select), the ROM's own
       ACTOR_SPAWN_TABLE record with its factory word repointed and its vtable
       filled (hal/scene_boot.cpp). ONE class. ov003 carries three -- dScTitle_c
       (id 2) and dScGameOver_c (id 8) are the others -- and those two are
       derived to the same depth and NOT seated; port/slice_scene1.txt names
       what blocks them.
       ON EVERY BOOT, LEVEL RUNS INCLUDED, and that is deliberate rather than
       incidental: the arm9 spawn table really does carry that entry whichever
       occupant of the shared slot is loaded, and the port's registry is the
       host form of that table. It is also what makes the edge REAL for
       /OPT:REF -- the ov003 bodies link because the ROM's own table and the
       ROM's own vtable name them, not because anything /include:'s them.
       A level run never dispatches through it: nothing spawns actor id 4
       inside a level, and the whole 46-level battery is the measurement that
       says so. That measurement covers id 4 and nothing else, so whoever
       seats 2 or 8 owes it again. */
    port_scene_registry_install();

    if (make_stage)
        std::printf("[a2] scene root %p\n", port_stage_object());
}

extern "C" void port_stage_a2_seat(void) { port_a2_seat_body(1); }

/* The scene boot's entry (hal/scene_boot.cpp's port_scene_run, reached from
   tests/walk_window.cpp when SM64DS_SCENE is set): the same seat with no
   Stage. Kept here rather than in scene_boot.cpp so there is exactly one copy
   of the sequence and no chance of the two drifting. */
extern "C" void port_scene_a2_seat(void) { port_a2_seat_body(0); }

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
void port_stage_anims_rearm(void);       /* hal/stage_bridges.cpp: re-arm the
                                            texture-transformer reload, which
                                            keys off the level id and so cannot
                                            see a self-warp */
}

// ---- SM64DS_MM_STALE=1: what the minimap inherits across a level change -----
//
// GetMinimapID reads two independent things the ROM hands it FRESH on every
// level and the port, which keeps one Stage alive across levels, does not:
//
//   1. the level AREA TABLE at Stage+0x8bc (stride 0xc). Its +8 word is the
//      head of the per-area MINIMAP-CHANGE list that LoadSimpleObjects builds
//      through LoadMinimapChangeObject, and GetMinimapID walks that list --
//      `sub ecx,dword ptr [eax]` at GetMinimapID+0x33 is the node deref.
//   2. the three MARKER ARRAYS Minimap::Behavior hands GetMinimapID as `obj`:
//      data_0209f40c (12 star markers), data_0209f3e8 (9 stars),
//      data_0209f3a4 (8 spike bombs). Stage::InitResources zeroes all three
//      on every entry (src/_ZN5Stage13InitResourcesEv.cpp:284..300).
//
// The probe prints both at the top of the boot, BEFORE anything this level
// loads has run, so a non-zero line is by definition the previous level's.
extern "C" {
extern int data_0209f40c[];
extern unsigned char data_0209f3e8[];
extern unsigned char data_0209f3a4[];
extern void *data_0209f314;
extern void *data_0209f354;          /* the VIEW-OBJECT table pointer */
extern unsigned char data_0209f1f8;  /* and its count */
}

static void port_minimap_stale_probe(const char *when)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_MM_STALE") != 0;
    if (!on)
        return;

    const char *area = (const char *)data_0209f314;
    int heads = 0, flags = 0, anims = 0;
    if (area) {
        for (int i = 0; i < 8; ++i) {
            if (*(void **)(area + i * 0xc + 0)) ++anims;
            if (*(unsigned char *)(area + i * 0xc + 4)) ++flags;
            if (*(void **)(area + i * 0xc + 8)) ++heads;
        }
    }
    int m40c = 0, m3e8 = 0, m3a4 = 0;
    for (int i = 0; i < 12; ++i) if (data_0209f40c[i]) ++m40c;
    for (int i = 0; i < 9; ++i)  if (((void **)data_0209f3e8)[i]) ++m3e8;
    for (int i = 0; i < 8; ++i)  if (((void **)data_0209f3a4)[i]) ++m3a4;

    std::fprintf(stderr,
                 "[mm-stale] %s: area table %p anim=%d flag=%d CHANGELIST=%d | "
                 "markers f40c=%d f3e8=%d f3a4=%d | viewobj f354=%p f1f8=%d\n",
                 when, (void *)area, anims, flags, heads, m40c, m3e8, m3a4,
                 data_0209f354, (int)data_0209f1f8);
    if (area)
        for (int i = 0; i < 8; ++i)
            if (*(void **)(area + i * 0xc + 8))
                std::fprintf(stderr, "[mm-stale]   area %d change-list head "
                             "%p\n", i, *(void **)(area + i * 0xc + 8));
    std::fflush(stderr);
}

/* ---- freeing the level's KCL image (the biggest slice of the ~108KB-per-
   re-entry game-heap leak) -----------------------------------------------------
   LoadFile allocates one game-heap block per handle through fs_hand_out ->
   Memory::Allocate. Almost every image is owned by something the teardown
   already frees: an actor's cleanup frees the ones it loaded, and
   port_level_stage_reseat frees the Stage's own level Model (+0x86c) and skybox
   (+0x9bc), which frees the BMDs behind them. The one image NOTHING frees is the
   level's KCL: Stage::LoadClsnAndObjects loads it straight through this table and
   registers it into the persistent Stage's MeshCollider, and the port resets
   that registry (Stage::ResetMeshColliders) without freeing the image. So the
   castle grounds re-entering itself leaked its KCL -- main_castle.kcl, 71732
   bytes in the heap -- every cycle. That is the bulk of the drift the [lvl]
   line reported (108168 bytes total, actor census steady at 51).

   This frees EXACTLY that one image and drops the rest, because the KCL is the
   only slot whose owner is the port itself. The KCL's OV0 handle is the level's
   own datum: LVL_Overlay+0x0a (PortLvlOverlay.kclFileId), read from the ROM's
   own overlay for the level being torn down (data_0209f2f8, the current level,
   before port_level_latch advances it). Trying to free MORE than the KCL is
   what an earlier draft got wrong: releasing every still-allocated slot freed
   the level Model's and skybox's BMD images out from under the reseat that was
   about to hand them back, corrupting the allocator's free list (the next
   MemoryLeft walk faulted). Freeing precisely the one orphaned handle avoids
   guessing.

   A guard on the block's own header keeps this safe against a double free even
   so: a live ExpandingHeapAllocator block carries the used-node magic 0x5544 in
   the two bytes at userPtr-0x10 (src/ExpandingHeapAllocator AllocateNode ->
   CreateNode(...,0x5544)). If the KCL image was somehow already freed, the magic
   no longer reads 0x5544 and the release is skipped. SM64DS_TRACE_LEVEL=1
   reports what it did.

   RESOLVE EARLY, FREE LATE -- and the split is not cosmetic. The Stage's
   MeshCollider registry POINTS AT this image (LoadClsnAndObjects registered it),
   and Stage::ResetMeshColliders is what clears that registry. That reset runs in
   port_level_stage_reseat, LATER than port_level_reset_host. Freeing the image
   in reset_host, as an earlier draft did, returns the block to the allocator
   while the registry is still holding a live pointer into it: six clean
   re-entries happened not to dereference it in that window, but any future
   teardown, collision query or trace that walked the registry between reset_host
   and ResetMeshColliders would read freed memory -- a rare, unreproducible
   crash. So this is split in two:

     port_level_capture_kcl()  -- runs in port_level_reset_host, BEFORE
       port_level_latch advances data_0209f2f8. It resolves the handle from the
       level being torn down (its own LVL_Overlay, +0x0a) and COPIES the slot's
       SharedFilePtr aside. It frees nothing. The resolution must happen here,
       from the old level's overlay, because the latch is about to point
       data_0209f2f8 at the incoming level -- reading kclFileId after the latch
       would name the WRONG level's KCL.

     port_level_free_captured_kcl()  -- runs in port_level_stage_reseat, AFTER
       Stage::ResetMeshColliders() has emptied the registry. By construction the
       registry no longer references the image, so the free has no live pointer
       to invalidate. SharedFilePtr::Release operates entirely on the passed
       struct (fileID, numRefs, filePtr -> Memory::Deallocate; src), so the
       aside copy frees the right block even though reset_host has since zeroed
       the live table slot. The 0x5544 magic guard rides on the captured filePtr.

   Splitting this way keeps the ROM-shaped resolution order (handle named from
   the outgoing level) while making the free order safe by construction (image
   returned only after the last thing pointing at it is cleared). The KCL alone
   was measured stable across repeated re-entries (heap held to a fixed baseline,
   no fault, census 51); the rest of the table is dropped as before, its images
   the owners' to free. */
extern "C" void *port_level_overlay(int level);   /* hal/level_change.cpp */

/* The KCL slot captured in port_level_reset_host (from the outgoing level) and
   freed in port_level_stage_reseat, after ResetMeshColliders clears the registry
   that points at the image. A copy, not an index: reset_host zeroes the live
   table slot before the free runs, but SharedFilePtr::Release only needs the
   struct's own fields, so the aside copy frees the right block. */
/* CAPTURED for the same reason as the handle table above: it is a row COPIED
   out of that table, so a restore that rolled the table back and left this
   behind would be the same disagreement one indirection further along. */
/* THE VALIDITY FLAG RIDES WITH THE ROW IT VALIDATES. Half a description is
   what this whole family of bugs is made of: a restore that rolled the slot
   copy back and left the flag saying "there is one" would be the file-handle
   disagreement again, one indirection further along. Practically inert -- the
   flag is only non-zero BETWEEN port_level_reset_host and
   port_level_stage_reseat, a window inside one level change that no restore
   can land in -- but a pair that cannot disagree needs no such argument, and
   the argument is what would have to be re-checked the next time the window
   moves. Four bytes. */
DSSTATE_BEGIN
static PortSharedFilePtr g_pending_kcl;
static int g_have_pending_kcl;
DSSTATE_END

/* Resolve the outgoing level's KCL handle and stash its slot for a late free.
   Called from port_level_reset_host BEFORE port_level_latch, so data_0209f2f8
   still names the level being torn down. Frees nothing. */
static void port_level_capture_kcl(void)
{
    const int trace = std::getenv("SM64DS_TRACE_LEVEL") != 0;
    g_have_pending_kcl = 0;
    const int level = (int)data_0209f2f8;
    PortLvlOverlay *ov = (PortLvlOverlay *)port_level_overlay(level);
    if (!ov)
        return;
    const unsigned kcl = ov->kclFileId;
    for (int i = 0; i < g_loadfile_used; ++i) {
        if (g_loadfile_slot[i].fileID != kcl)
            continue;
        /* THE ROW MUST BE ABLE TO NAME THE BLOCK. LoadFile allocates a fresh
           one per call, the ROM's contract, so a handle loaded twice leaves
           this row holding the SECOND block while Stage::LoadClsnAndObjects'
           collider still points at the first. Freeing on that guess returns a
           block the registry does not own and keeps the one it does; leaking
           the image is the cheaper error, so decline and say so. Measured on
           this tree over all 46 mounted levels, 510 LoadFile calls: no level
           loads ANY handle twice inside one boot, let alone its own KCL. So
           the branch is a net that nothing currently takes, and not a
           workaround for something that happens. */
        if (g_loadfile_loads[i] > 1) {
            /* UNCONDITIONAL, and on stderr, unlike every other line in this
               function. The others narrate a path that is working; this one
               says a level image was just left to leak. A leak that only
               announces itself when someone has already set SM64DS_TRACE_LEVEL
               is a leak nobody finds, and the whole point of the branch is to
               be the tripwire for a case no measured level reaches. stderr
               because the flight recorder captures it in real play. */
            std::fprintf(stderr, "[lvl] KCL handle %u was loaded %u times this "
                         "level; the row cannot name the collider's block, so "
                         "the image is LEFT TO LEAK rather than freed\n",
                         kcl, (unsigned)g_loadfile_loads[i]);
            return;
        }
        g_pending_kcl = g_loadfile_slot[i];   /* copy fileID/numRefs/filePtr */
        g_have_pending_kcl = 1;
        if (trace)
            std::printf("  [lvl] captured the level's KCL for a late free: "
                        "handle %u ptr %p\n", kcl,
                        (void *)g_loadfile_slot[i].filePtr);
        return;
    }
    if (trace)
        std::printf("  [lvl] no KCL slot for handle %u to capture\n", kcl);
}

/* Free the KCL image captured by port_level_capture_kcl. Called from
   port_level_stage_reseat AFTER Stage::ResetMeshColliders() has emptied the
   registry, so nothing live points into the block being returned. */
static void port_level_free_captured_kcl(void)
{
    if (!g_have_pending_kcl)
        return;
    const int trace = std::getenv("SM64DS_TRACE_LEVEL") != 0;
    g_have_pending_kcl = 0;
    char *fp = g_pending_kcl.filePtr;
    const unsigned short mg = fp ? *(unsigned short *)(fp - 0x10) : 0;
    if (fp && mg == 0x5544) {
        if (trace)
            std::printf("  [lvl] releasing the level's KCL: handle %u ptr "
                        "%p size %u (registry cleared, nothing else owns it)\n",
                        (unsigned)g_pending_kcl.fileID, (void *)fp,
                        *(unsigned *)(fp - 0xc));
        _ZN13SharedFilePtr7ReleaseEv(&g_pending_kcl);
    } else if (trace) {
        std::printf("  [lvl] KCL handle %u already reclaimed (magic %04x); "
                    "not freeing\n", (unsigned)g_pending_kcl.fileID, mg);
    }
}

extern "C" void port_level_reset_host(void)
{
    /* Capture the outgoing level's KCL slot (the one LoadFile image the port
       itself orphans -- see port_level_capture_kcl) for a late free. It resolves
       the handle HERE, before port_level_latch advances data_0209f2f8, but the
       image is not returned to the allocator until port_level_stage_reseat has
       run Stage::ResetMeshColliders and emptied the registry that points at it. */
    port_level_capture_kcl();

    /* THE HANDLE TABLE, dropped not released. Every OTHER image here is owned by
       something the teardown frees (an actor, or the Stage's Model/skybox that
       port_level_stage_reseat hands back); releasing them here would double-free
       the owner's block. The slots are zeroed so the next level starts with an
       empty table and re-loads its own files. */
    const int trace = std::getenv("SM64DS_TRACE_LEVEL") != 0;
    int keep = 0;
    for (int i = 0; i < g_loadfile_used; ++i) {
        if (g_loadfile_slot[i].pad) {           /* persistent (message bank) */
            if (keep != i) {
                g_loadfile_slot[keep] = g_loadfile_slot[i];
                g_loadfile_loads[keep] = g_loadfile_loads[i];
            }
            ++keep;
            continue;
        }
        if (trace)
            std::printf("  [lvl] dropping file slot %d: handle %u ptr %p\n", i,
                        g_loadfile_slot[i].fileID,
                        (void *)g_loadfile_slot[i].filePtr);
    }
    for (int i = keep; i < g_loadfile_used; ++i) {
        g_loadfile_slot[i].fileID = 0;
        g_loadfile_slot[i].numRefs = 0;
        g_loadfile_slot[i].filePtr = 0;
        g_loadfile_slot[i].pad = 0;
        /* The count is cleared with the row it belongs to, and carried with a
           row that is kept. It answers "can this row still name the block a
           consumer is holding", so a count that outlived its row would decline
           the next level's KCL free for a reason from the level before it. */
        g_loadfile_loads[i] = 0;
    }
    g_loadfile_used = keep;

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

    /* THE THREE MARKER ARRAYS, and they are the ROM's own lines rather than
       port hygiene. Stage::InitResources zeroes eight things in one block
       (src/_ZN5Stage13InitResourcesEv.cpp:284..307):

           for (i = 0; i < 0xC; i++) data_0209f40c[i] = 0;
           for (i = 0; i < 9;   i++) data_0209f3e8[i] = 0;
           func_ov001_020ab2e4();
           for (i = 0; i < 8;   i++) data_0209f3a4[i] = 0;
           data_0209f1f8 = 0; data_0209f2d0 = 0; data_0209f258 = 0;
           data_0209f2e8 = 0; data_0209f25c = 0; data_0209f338 = 0;

       The port hand-rolls the boot and skips InitResources, so this function is
       where that block lands -- and it carried the five scalars and not the
       three ARRAYS. The three are the only entries in the block that hold ACTOR
       POINTERS: SetStarMarker/PowerStar::AddStarMarker file into f40c, ov001
       and func_0202a8e0 into f3e8, AddSpikeBomb into f3a4. Minimap::Behavior
       walks all three every frame; f40c and f3e8 it hands to GetMinimapID as
       `obj`, while the f3a4 loop reads o->+0xcc itself and never calls it --
       a stale pointer is dereferenced either way, just down two paths.

       GetMinimapID reads obj->+0xcc as an AREA INDEX -- a signed byte. Left
       stale, a slot still points at an actor the previous level's teardown
       destroyed, and +0xcc is whatever the freed block now holds.

       The crash dump reads the whole chain out: edx 8 at the fault, so the
       byte came back 8, and the area table is EIGHT entries (Stage+0x8bc,
       0x60 bytes), so index 8 is one past the end and lands on the level
       MeshCollider at Stage+0x91c. Its word at +8 was ffffffff, which is the
       eax the faulting instruction dereferences -- GetMinimapID+0x33,
       `sub ecx,dword ptr [eax]`, "access 00000000 at ffffffff".

       A direct boot never sees it because BSS starts zeroed, which is exactly
       why this only ever showed up on the warp path. THREE OF THE FOURTEEN
       MEASURABLE levels leave a live pointer in f40c when they are torn down
       (6, 7 and 14; level 10 is unmeasurable because warping out of it
       hard-faults for an unrelated reason), so the exposure is wider than the
       one pair that actually faults; which pairs pull the trigger depends on
       what the next level's allocator puts in the freed block.

       AND THE ROM REALLY DOES DEPEND ON THIS ZEROING, which is worth stating
       carefully because the obvious stronger claim is false. Slots ARE cleared
       on the way out in general: UntrackStar does SetStarMarker(slot, 0, 2),
       and PowerStar, Coin and QuestionBlock all call it from their cleanups.
       What has no such path is the actor that faults here -- Whomp::
       InitResources calls Actor::TrackStar to file itself in, and no Whomp
       file anywhere calls UntrackStar. Its slot is only ever emptied by the
       InitResources loop above, so a port that skips that loop keeps a Whomp
       pointer alive into the next level.

       Not carried, and why. func_ov001_020ab2e4 is in ov001, which the port
       does not mount.

       data_0209f1f8 (the view-object count) is the interesting one, and the
       first version of this comment got its reason wrong. It said the count is
       "written by LoadViewObjects on every boot before anything reads it".
       That is not a guarantee: LoadViewObjects is a SUB-TABLE loader, so it
       runs only when the level DECLARES a type-4 table, and it re-seats the
       POINTER data_0209f354 as well as the count (func_0202b0c4 writes both).
       A destination with no view objects would therefore inherit both from the
       level it replaced, and Camera::InitResources loops i < data_0209f1f8
       through GetViewObj -> &data_0209f354[i].

       Measured rather than argued: all fifteen mounted levels declare one and
       re-seat both (counts 5, 11, 3, 6, 15, 4, 7, 1, 1, 4, 1, 21, 1, 2, 2), so
       the carry is real at boot entry and overwritten by boot done on every
       pair that exists today. LATENT, not live -- and it arms itself the day
       someone mounts a level without view objects. Left out of this commit to
       keep the diff to the fault it is about; it is the ROM's own line
       (InitResources:302) and a one-line follow-up. */
    {
        extern int data_0209f40c[];
        extern unsigned char data_0209f3e8[];
        extern unsigned char data_0209f3a4[];
        for (int i = 0; i < 12; ++i) data_0209f40c[i] = 0;
        for (int i = 0; i < 9;  ++i) ((void **)data_0209f3e8)[i] = 0;
        for (int i = 0; i < 8;  ++i) ((void **)data_0209f3a4)[i] = 0;
    }

    /* NO AREA IS SHOWING YET, which is Stage::InitResources:211 and belongs on
       every level entry rather than once a process. The port ran it in
       port_stage_a2_seat, so a warp carried the PREVIOUS level's area index
       into the new level and it lands in two places:

         ChangeArea, which the new level's Camera::InitResources calls, reads
         `if (data_02092120 >= 0) HideArea(data_02092120)` -- clearing the
         showing flag of whatever area the OLD level happened to be in, in the
         NEW level's table, before ShowArea turns the right one on.

         GetMinimapID's first line, which falls back to data_02092120 whenever
         the object it is handed carries a negative area byte.

       Both read a number the new level never chose. -1 is the ROM's own value
       and it makes ChangeArea skip the hide, which is the behaviour a direct
       boot already gets. */
    {
        extern signed char data_02092120;
        data_02092120 = -1;
    }
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

    /* The registry that pointed at the outgoing level's KCL image is now empty,
       so it is safe to return that image to the allocator. port_level_reset_host
       captured it (from the level being torn down) before the latch; free it
       here, after the reset, so nothing live points into the block. */
    port_level_free_captured_kcl();

    /* THE AREA TABLE, the third level-owned member of the Stage and the one
       this function was missing. Stage+0x8bc, stride 0xc, eight entries: the
       extent is the Stage's own layout (include/Stage.h -- unk_8bc then
       pad_8bd[0x5f], the next member being the level MeshCollider at +0x91c),
       so 0x60 bytes, and the stride is the one ShowArea/HideArea/
       IsAreaShowing, LoadMinimapChangeObject and port_stage_advance_anims all
       index by. Per entry: a TextureTransformer* at +0, the "this area is
       showing" flag at +4, and at +8 the head of the per-area MINIMAP-CHANGE
       list.

       On the ROM this needs no code. The Stage is destroyed and rebuilt per
       level, so Stage::InitResources always finds the table zeroed, and
       Stage::CleanupResources sets data_0209f314 = 0 so GetMinimapID's own
       `if (table == 0) return the LVL_Overlay default` guard covers the gap in
       between. The port keeps ONE Stage alive across levels on purpose
       (hal/level_change.cpp says why), so it owes that freshness by hand --
       which is what this whole function is, and it already does exactly this
       for the level Model at +0x86c and the skybox at +0x9bc.

       THIS IS NOT THE CRASH FIX, and the ablation says so rather than the
       reasoning. Running the 7 -> 10 repro four times over the two clears in
       this commit, one knob each: with only this memset the MINIMAP still
       faults, with only the marker-array clear in port_level_reset_host it
       does not. The faulting `obj` is a stale marker, not a stale list node.

       What this one is for is the +8 word on its own terms. LoadSimpleObjects
       builds the per-area minimap-change list through LoadMinimapChangeObject
       on every boot, appending to whatever head it finds, and nothing ever
       frees a node. Carried across a change it appends the new level's nodes
       onto the previous level's, so the list grows without bound for a
       session and GetMinimapID answers out of the wrong level's entries.
       Four levels leave one behind (SM64DS_MM_STALE=1 over a boot of each:
       7 and 13 and 15 leave one, 12 leaves five).

       THE +0 SLOTS ARE NOT FREE, and an earlier draft of this comment claimed
       they were. port_stage_advance_anims (hal/stage_bridges.cpp) is the ONLY
       caller of Stage::LoadTextureTransformers in the port, and it re-loads
       only when the level id CHANGES. Zero the slots without arming it and a
       SELF-WARP -- level A to level A, where the id does not change -- leaves
       every slot null with nothing left to refill them. Measured on levels 13,
       7 and 1: one reload line in the whole run, and anim=0 after the second
       entry where the first left 2, 1 and 1. That is texture animation dead
       for the rest of the session.

       So the memset is PAIRED WITH THE ARM rather than narrowed to the +4 and
       +8 words. Narrowing would also have kept the fault fixed and the
       animation alive, but it would have carried the previous boot's
       transformer POINTERS across an entry that released and re-loaded the BTA
       behind them -- a dangling walk of the same shape as the one this commit
       is about. On the ROM the whole record is fresh, because
       Stage::InitResources calls LoadTextureTransformers on every entry (src
       line 388) into a Stage that was just constructed. Zero it all and reload
       it all is the ROM's own shape, and it also closes the self-warp
       aliasing that predates this commit -- the case the level-id guard was
       never able to see. */
    std::memset(stage + 0x8bc, 0, 0x60);
    port_stage_anims_rearm();

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
/* ---- readers for the HOST-SIDE description of the current level ------------

   A save-state restore rolls back exactly three things: the hosted DS arena,
   the .dsstate section, and the hardware content stores. It rolls back NOTHING
   ELSE -- and this file stages a second description of the same world outside
   all three. Gate 31's comment above already names that set, because a LEVEL
   CHANGE has to undo it by hand; a restore is the same transition and undoes
   none of it.

   These readers exist so the save-state census in tests/walk_window.cpp can
   print that second description beside the world's own without this file's
   statics leaving this file, and without dragging hal/lk6_savestate.cpp onto
   the link line of every target that compiles this one (smoke_player does not
   link it). */
extern "C" int port_level_host_boot_target(void) { return g_boot_target; }
extern "C" const void *port_level_host_entrances(int *count)
{
    if (count) *count = g_entrance_count;
    return g_entrance_entries;
}
extern "C" int port_level_host_file_rows(void) { return g_loadfile_used; }
extern "C" void *port_level_host_file_row(int i, unsigned *handle,
                                          unsigned *refs, int *persistent)
{
    if (i < 0 || i >= g_loadfile_used || i >= PORT_LOADFILE_SLOTS)
        return 0;
    if (handle)     *handle = g_loadfile_slot[i].fileID;
    if (refs)       *refs = g_loadfile_slot[i].numRefs;
    if (persistent) *persistent = g_loadfile_slot[i].pad;
    return g_loadfile_slot[i].filePtr;
}
/* the two halves of the path binding the port's own assert reads */
extern "C" void port_level_host_paths(void **table, int *count)
{
    if (table) *table = (void *)(size_t)data_020a0d84[0];
    if (count) *count = data_020a0d8c[0];
}

/* The two rollback-coupled one-shot guards, stashed and put back together.
   hal/lk6_savestate.cpp calls this pair only under SM64DS_SS_NO_ROLLGUARD=1,
   which is the fix-off arm of RELOAD2's A/B; see the hook's comment there and
   the argument on g_level_mounted above.

   REGISTERED BY tests/walk_window.cpp, NOT HERE, and the link set is the whole
   reason: smoke_player links this file but not hal/lk6_savestate.cpp, so a
   call to port_ss_rollguard_hook from here is an unresolved external in that
   target. walk_window is the only binary that links both, so it is the only
   place the two halves can be joined. */
extern "C" {
void port_mount_cache_stash(void);
void port_mount_cache_unstash(void);
void port_ov009_sinit_stash(void);       /* hal/ov009_boot.cpp */
void port_ov009_sinit_unstash(void);
}

extern "C" void port_rollguard_stash(void)
{
    port_mount_cache_stash();
    port_ov009_sinit_stash();
}

extern "C" void port_rollguard_unstash(void)
{
    port_mount_cache_unstash();
    port_ov009_sinit_unstash();
}

extern "C" void port_level_mounts_install(void)
{
    /* HOST-ONLY, and deliberately not bracketed: what this guards is the host
       mount REGISTRY (port_level_mount_register), which no restore touches.
       The flag that had to move into .dsstate is g_level_mounted above, whose
       subject is the overlay IMAGE. Same shape, opposite adjudication, and the
       difference is which side of the section the guarded work lands on. */
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
