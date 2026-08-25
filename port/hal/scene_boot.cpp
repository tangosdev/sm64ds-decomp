// THE SCENE BOOT: how the port enters a scene that is not a level.
//
// ---- what a scene is -------------------------------------------------------
//
// A scene is an ACTOR. There is no separate scene machinery in this game: the
// title screen, the star select, the game-over screen, every minigame and the
// level itself are all actors spawned through the same spine, and the thing
// that makes one of them a "scene" is that its class derives from Scene rather
// than from Actor, and that its actor id is one the arm9 knows to load an
// overlay for. That is the whole difference.
//
// So hosting a scene is the ORDINARY REGISTRY SHAPE this port already uses for
// a hundred actor classes: the ROM's own SpawnInfo record with its factory word
// repointed at the host factory, seated in data_020a4bb8 at the class's id,
// plus a vtable fill. Nothing in this file is a new mechanism. What is new is only
// that the ids are scene ids and that something drives the ROM's own chain.
//
// ---- the chain, end to end, every step a matched arm9 TU -------------------
//
//   Scene::SetSceneToSpawn(id, param)     0x0202e36c
//       data_02092664 = id, data_0209f5b8 = param
//   Scene::SpawnIfNecessary()             0x0202e26c
//       if data_02092660 == 0 and data_02092664 != 0x187:
//           func_02013edc(id, param, 1)   0x02013edc, a tail-call veneer
//         -> func_02042fe4(id, param, 1)  0x02042fe4
//         -> func_02043098(id, 0, param, 1)         THE SPAWN SPINE
//              data_020a4b50 = id; func_02043060(id) the pre-spawn hook;
//              func_02043180(id, 0, param, 1) the spawn context;
//              (*data_020a4bb8[id])()      the factory;
//              func_020433b8(obj)          thread it onto the init list
//       then data_02092664 = 0x187 and data_02092660 = 1, so it spawns once.
//
// Every one of those is a matched TU and all but Scene::SpawnIfNecessary were
// already in the walk_window link set before this lane; that one arrives with
// port/slice_scene1.txt. THE PORT ADDS NOTHING TO THE CHAIN. port_scene_boot
// below calls the first two functions and that is all it does.
//
// ---- the overlay load, and why the host does not run it --------------------
//
// On the DS the spine's PRE-SPAWN HOOK is what mounts the scene's overlay.
// func_0201a5cc seats data_020a4b58 = func_0201a694 and data_020a4b5c =
// func_0201a614, and func_02043060 calls the first with the pending actor id
// before the factory runs:
//
//   func_0201a694(id) -> GetSceneOverlayID(id)   ids 2/4/8 -> ov003,
//                                                1 -> ov007, 5 -> ov005,
//                                                3/6/7 -> ov002,
//                                                0x169..0x186 -> ov006
//                     -> func_0201a754(old) then func_0201a798(new)
//                        the unload-before-load discipline, and the one place
//                        ov004 is ever loaded (alongside ov006).
//
// The port's occupant of that hook slot is port_prespawn_hook (the registry
// gate, hal/actor_registry.cpp), which returns the same 2/3 the ROM's hook
// returns. That substitution is not new here and it is not a stand-in for the
// overlay load: the port has no overlay loader because every overlay it hosts
// is a static host array mounted at build time (port/tools/ovdata.py). ov003 is
// mounted exactly the way ov009 or ov016 is. There is no LoadOverlay to run and
// nothing for it to do.
//
// ---- the three classes, and the one this file seats ------------------------
//
// ov003 carries three, and the arm9 ACTOR_SPAWN_TABLE (0x02090864) names all
// three by relocation -- these are reads out of config/arm9/relocs.txt, not
// inferences:
//
//   id 2  0x0209086c -> 0x020b1380   dScTitle_c     the debug level select
//   id 4  0x02090874 -> 0x020b16b4   dScStarSel_c   the star select   <- seated
//   id 8  0x02090884 -> 0x020b1750   dScGameOver_c  the game-over screen
//
// and each record's +4 halfword carries its own id back (2, 4, 8), which is the
// registry's own cross-check and is asserted below. The record is mounted bytes
// (port/ov003_syms.txt); only the +0 factory word is repointed.
//
// ONLY THE STAR SELECT IS SEATED HERE, and what holds the other two is neither
// the mount nor the chain: eleven of their fourteen slot bodies carry the
// "recovered from vtable slot identity" marker, and three of their dtor TUs
// spell per-TU placeholders that exist in no config. Both blockers are written
// out in full at the bottom of port/slice_scene1.txt.
//
// ---- the vtable ------------------------------------------------------------
//
// All three tables are the 18-slot Scene shape and agree with _ZTV5Scene (arm9
// 0x02092680) slot for slot everywhere they do not override. Slots 1/2/4/5/
// 7/8/10/11 are Scene's own halves, 13/14/15 ActorBase's; 0/3/6/9/12/16/17 are
// the class's own. The tables are left OUT of port/ov003_syms.txt (the
// ov079/ov080/ov081 convention), because the mounted bytes would be DS code
// addresses and nothing rebases a code word; the one the port dispatches
// through is the host array below.
//
// THE SLOT CONTENTS ARE THE ROM'S. Each was read out of
// extracted/overlays/overlay_0003.bin at addr - 0x020ad660 (that image's length
// is 0x4240, which equals the OVT's ram_size for ov003, so the read is exact --
// the dsd export is a different file and is not the RAM image), cross-checked
// against config/arm9/overlays/ov003/relocs.txt. No slot here is invented and
// none is a trap: every one of the eighteen words has a body.
//
//   0  InitResources           func_ov003_020af8a0    ov003
//   1  BeforeInitResources     Scene::                0x0202e638
//   2  AfterInitResources      Scene::                0x0202e62c
//   3  CleanupResources        func_ov003_020af86c    ov003
//   4  BeforeCleanupResources  Scene::                0x0202e5f0
//   5  AfterCleanupResources   Scene::                0x0202e5d0
//   6  Behavior                func_ov003_020af038    ov003
//   7  BeforeBehavior          Scene::                0x0202e3d4
//   8  AfterBehavior           Scene::                0x0202e3c8
//   9  Render                  func_ov003_020ae6f4    ov003
//  10  BeforeRender            Scene::                0x0202e3a4
//  11  AfterRender             Scene::                0x0202e398
//  12  OnPendingDestroy        func_ov003_020ae6f0    ov003
//  13  Virtual34               ActorBase::            0x0204357c
//  14  Virtual38               ActorBase::            0x0204349c
//  15  OnHeapCreated           ActorBase::            0x02043494
//  16  D2                      func_ov003_020addfc    ov003
//  17  D0                      func_ov003_020ade54    ov003
//
// ---- and how a run gets here -----------------------------------------------
//
// SM64DS_SCENE=<id> in walk_window, mirroring SM64DS_LEVEL. The level harness
// hands the whole run over at the top of main -- after the host bring-up (the
// fixed-address reservation, the root heap, the ov002 mount's pointer pass and
// its static initialisers, the model vtable fills) and before anything level-
// shaped happens -- and port_scene_run below owns the rest of the process.
//
// IT IS A HANDOVER RATHER THAN A MODE INSIDE THE LEVEL LOOP, and the reason is
// structural rather than stylistic. walk_window's frame is written around the
// Player the entrance spawned: it reads the player pointer unguarded from the
// input block to the camera rig to the HUD. A scene has no Player, no Stage, no
// level overlay and no entrance, so there is nothing for those lines to read.
// The frame here is the same five-phase split walk_window makes and the same
// one func_02044120 makes on the DS -- phases 4/2/3 the tick, phase 5 the
// render bucket inside the host's render frame, phase 1 the scene-tree pass
// that closes it -- with the level's own passes simply absent because there is
// no level.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ntr/gx.h"
#include "ntr/ppu.h"
#include "ntr/rt.h"

#include "dsstate_seg.h"

// ---- THE LINKAGE FACES ov003's OWN SOURCES NEED -----------------------------
//
// Three shapes, all of them decomp-side spellings rather than port decisions,
// and all three closed BY ADDRESS rather than by name.
//
// 1. C-NAMED SYMBOLS DECLARED AT C++ LINKAGE. src/func_ov003_020af038.cpp (the
//    Behavior) declares nine of its globals outside an extern "C" block, so
//    MSVC mangles the references. The definitions are the port's ordinary
//    C-named hosted globals; these aliases bind the mangled spelling to them.
//    Same mechanism, same reason, as hal/cxx_aliases.cpp.
#pragma comment(linker, "/alternatename:?data_0209f5bc@@3PAUObj@@A=_data_0209f5bc")
#pragma comment(linker, "/alternatename:?data_0209f5e8@@3FA=_data_0209f5e8")
#pragma comment(linker, "/alternatename:?data_02092128@@3EA=_data_02092128")
#pragma comment(linker, "/alternatename:?data_02092114@@3EA=_data_02092114")
#pragma comment(linker, "/alternatename:?data_0209f1f0@@3EA=_data_0209f1f0")
#pragma comment(linker, "/alternatename:?data_020a0de8@@3PAURow020a0de8@@A=_data_020a0de8")
#pragma comment(linker, "/alternatename:?data_020a0de9@@3PAY03EA=_data_020a0de9")
#pragma comment(linker, "/alternatename:?data_020a0e58@@3PAGA=_data_020a0e58")
#pragma comment(linker, "/alternatename:?data_020a0e5a@@3PAGA=_data_020a0e5a")
//
// 2. NAMESPACE-QUALIFIED FUNCTIONS. Two ov003 TUs reach arm9 functions through
//    a locally-declared namespace (`namespace G2 { short *GetBG0ScrPtr(); }`,
//    `class Sound { static void UnsetPlayerVoiceGroup(); }`) rather than
//    through the mangled C name the matched TU defines.
#pragma comment(linker, "/alternatename:?GetBG0ScrPtr@G2@@YAPAFXZ=__ZN2G212GetBG0ScrPtrEv")
#pragma comment(linker, "/alternatename:?UnsetPlayerVoiceGroup@Sound@@SAXXZ=__ZN5Sound21UnsetPlayerVoiceGroupEv")
//
// 3. THE OAM SPRITE TEMPLATES, SPELLED AS FUNCTIONS. Four TUs of the star
//    select's closure name SEVENTEEN distinct ov001 sprite-template tables as
//    `extern void *func_020abXXXX[]` -- thirteen of them in
//    dScStarSel_c::Render (src/func_ov003_020ae6f4.cpp) alone. No func_020ab*
//    symbol exists in any config: the addresses are ov001 DATA, and the
//    "func_" prefix is a decomp-side guess at what lives there. Each alias
//    below binds the guess to the address's real name, read out of
//    config/arm9/overlays/ov001/symbols.txt.
//    FIVE OF THE SEVENTEEN WERE ALREADY ALIASED, in hal/sub_actors.cpp's own
//    ov001 block, because the HUD's render TUs spell them the same way -- the
//    same defect in a different overlay's source (020ab948, 020ab9c8,
//    020aba70, 020abad0, 020abad8; that block's sixth entry, 020abd88, this
//    closure does not use). Those five are not repeated here, so the twelve
//    below are exactly the spellings this lane is the first caller of. Ten of
//    the twelve addresses were not in the port's ov001 mount either and joined
//    it this lane (port/ov001_syms.txt, which also records why the address is
//    ov001's and not ov000's).
#pragma comment(linker, "/alternatename:_func_020ab938=_data_ov001_020ab938")
#pragma comment(linker, "/alternatename:_func_020ab940=_data_ov001_020ab940")
#pragma comment(linker, "/alternatename:_func_020abb18=_data_ov001_020abb18")
#pragma comment(linker, "/alternatename:_func_020abb34=_data_ov001_020abb34")
#pragma comment(linker, "/alternatename:_func_020abb54=_data_ov001_020abb54")
#pragma comment(linker, "/alternatename:_func_020abb74=_data_ov001_020abb74")
#pragma comment(linker, "/alternatename:_func_020abb94=_data_ov001_020abb94")
#pragma comment(linker, "/alternatename:_func_020abbb4=_data_ov001_020abbb4")
#pragma comment(linker, "/alternatename:_func_020abbf4=_data_ov001_020abbf4")
#pragma comment(linker, "/alternatename:_func_020abcb4=_data_ov001_020abcb4")
#pragma comment(linker, "/alternatename:_func_020abd78=_data_ov001_020abd78")
#pragma comment(linker, "/alternatename:_func_020abd80=_data_ov001_020abd80")

// ---- run link60 lane L2: the two dScDSMT_c PLACEHOLDER SPELLINGS -----------
//
// The ov007 dtor pair spells its vptr restores with the per-TU placeholder
// names the ov003 write-up named as blocker 2 for dScTitle_c. Both are
// resolved here to the address the ROM actually stores, read out of the
// literal pool of each body in extracted/overlays/overlay_0007.bin:
//
//   func_ov007_020cc028 (D2)   ldr r1,=0x021032e8 ; str -> _ZTV9dScDSMT_c
//                              ldr r0,=0x02092680 ; str -> _ZTV8dScene_c
//                              ldr r1,=0x0208e4b8 ; str -> _ZTV7dBase_c
//   func_ov007_020cc070 (D0)   the same three, spelled VT0 / VT1 / VT2, then
//                              Memory::Deallocate(this, *0x020a0eac) spelled G0
//
// 0x02092680 is _ZTV5Scene and 0x0208e4b8 is _ZTV12ActorDerived, both matched
// arm9 data symbols already in the build, and 0x021032e8 is the class's own
// table inside the ov007 mount. TWO of the six names need a face:
//
//   _ZTV9dScDSMT_c   UNIQUE to src/func_ov007_020cc028.c -- no other TU in the
//                    tree spells it -- so aliasing it to the ROM's own address
//                    is exact and cannot collide.
//   _ZTV8dScene_c    spelled by four TUs (ov003 x2, ov005, ov007), all of them
//                    Scene subclasses restoring the SAME base table, and none
//                    of the other three is in any slice today. _ZTV5Scene is
//                    the right answer for all four.
//
// The other four resolve to storage that already exists and this file adds
// nothing for them: _ZTV7dBase_c is hal/sub_actors.cpp's TRAP-FILLED 18-slot
// array, VT0/VT2 are hal/actor_vtables.cpp's shared placeholders, VT1 is
// hal/auto_bss.cpp's and G0 is hal/cxxname_bridge.cpp's. That is a KNOWN
// DIVERGENCE and not a fix: on the ROM D0's three stores put back
// 0x021032e8 / _ZTV5Scene / _ZTV12ActorDerived, and on the host they put back
// three unrelated arrays. It is the doctrine hal/actor_vtables.cpp already
// states -- "installed transiently during teardown and never dispatched" --
// and a dispatch through _ZTV7dBase_c traps rather than going quiet. Neither
// dtor is entered in any run this lane made; see port/ov007_seat.txt.
#pragma comment(linker, "/alternatename:__ZTV9dScDSMT_c=_data_ov007_021032e8")
#pragma comment(linker, "/alternatename:__ZTV8dScene_c=__ZTV5Scene")

extern "C" {

/* the spawn table and the two Scene entry points (matched arm9) */
extern void **data_020a4bb8;                     /* hal/actor_vtables.cpp */
int _ZN5Scene15SetSceneToSpawnEjj(unsigned id, unsigned param);
int _ZN5Scene16SpawnIfNecessaryEv(void);
extern unsigned short data_02092664;             /* the pending scene id */
extern signed char data_02092110;                /* the CURRENT SUBLEVEL */
extern signed char SUBLEVEL_LEVEL_TABLE[];       /* arm9 0x02075298 */
extern unsigned char data_02092660;              /* "a scene has spawned";
                                                    defined below, in .dsstate */

/* the ov005 launch row's +0x04 word, from hal/scene_mg_sound.cpp -- the param
   the Rec Room hands Scene::SetSceneToSpawn. 0 for a non-minigame scene. */
unsigned port_mg_scene_spawn_param(int scene_id);

/* the mounted SpawnInfo record (port/ov003_syms.txt) */
extern unsigned char StarSelect_SpawnInfo[];     /* dScStarSel_c  id 4 */

/* the factory (matched src, port/slice_scene1.txt) */
void *StarSelect_Spawn(void);                    /* dScStarSel_c */

/* Scene's own lifecycle halves, slots 1/2/4/5/7/8/10/11.
   Flat C names: every one of these TUs defines the mangled name at C linkage
   the way the rest of the port spells arm9 methods. */
int  _ZN5Scene19BeforeInitResourcesEv(void *self);      /* slot 1  */
int  _ZN5Scene22BeforeCleanupResourcesEv(void *self);   /* slot 4  */
void _ZN5Scene21AfterCleanupResourcesEj(void *self, unsigned a); /* slot 5 */
int  _ZN5Scene14BeforeBehaviorEv(void *self);           /* slot 7  */
int  _ZN5Scene12BeforeRenderEv(void *self);             /* slot 10 */

/* slots 2, 8, 11 and 15, through hal/scene_actor_faces.cpp. Three of the four
   are the ROM's TAIL-CALL VENEERS taken straight to their target with both
   arguments, because a (void)->(void) transcription of a veneer loses them on
   the host; that file has the derivation. */
void port_scene_after_init(void *self, unsigned a);
void port_scene_after_behavior(void *self, unsigned a);
void port_scene_after_render(void *self, unsigned a);
int  port_scene_on_heap_created(void *self);
int  port_scene_base_init(void *self);            /* ActorBase:: slot 0 */
int  port_scene_base_cleanup(void *self);         /* ActorBase:: slot 3 */

/* dScStarSel_c's own seven */
void func_ov003_020af8a0(void *self);            /* slot 0  InitResources */
int  func_ov003_020af86c(void);                  /* slot 3  CleanupResources */
int  func_ov003_020af038(void *self);            /* slot 6  Behavior */
int  func_ov003_020ae6f4(void *self);            /* slot 9  Render */
void func_ov003_020ae6f0(void);                  /* slot 12 OnPendingDestroy */
int  func_ov003_020addfc(void *self);            /* slot 16 D2 */
int *func_ov003_020ade54(void *self);            /* slot 17 D0 */

/* THE VTABLE, a host array. The name is the ROM's own data symbol, which is
   what StarSelect_Spawn writes into the object's +0 word and what the D2 and
   D0 bodies write back on the way down, so the spelling has to be exactly this
   and port/ov003_syms.txt has to leave it out of the mount. 18 words.
   The other two classes' tables (data_ov003_020b1650, data_ov003_020b179c) are
   left out of the mount as well -- they are code-pointer tables and nothing
   rebases a code word -- and are not defined here either, because nothing in
   the link set names them. */
DSSTATE_BEGIN
void *data_ov003_020b1704[18];                   /* dScStarSel_c */
DSSTATE_END

/* ---- run link60 lane L2: dScDSMT_c, the TITLE SCREEN, scene id 1 ----------
   The ov007 mount (port/ov007_syms.txt) hosts all three records this needs, so
   unlike the ov003 seat NOTHING here is a fresh host array: the SpawnInfo, the
   class vtable and the graphCallback_c sub-object vtable are the mount's own
   storage at the ROM's names and the ROM's spacing, and the fill writes host
   thunks INTO them. That is the right treatment when the table is inside a
   mounted data span -- defining a second array of the same name would be a
   duplicate symbol, and leaving the mounted one alone would leave eighteen raw
   DS code addresses live in a table the factory installs. */
extern unsigned char data_ov007_02103264[];  /* SpawnInfo, 8 bytes, +4 reads 1 */
extern unsigned char data_ov007_021032e8[];  /* _ZTV9dScDSMT_c, 88 bytes/22 words */
extern unsigned char data_ov007_021032b0[];  /* graphCallback_c, 16 bytes/4 words */
void port_ov007_pack_check(void);            /* generated by tools/ovdata.py */
void port_ov007_syms_patch(void);

int *func_ov007_020ccad0(void);              /* the factory */
int  func_ov007_020cc4c0(char *self);        /* slot 0  InitResources */
int  func_ov007_020cc45c(void);              /* slot 3  CleanupResources */
int  func_ov007_020cc2cc(char *self);        /* slot 6  Behavior */
int  func_ov007_020cc2b0(void *self);        /* slot 9  Render */
void func_ov007_020cc2ac(void);              /* slot 12 OnPendingDestroy */
int *func_ov007_020cc028(int *self);         /* slot 16 D2 */
int *func_ov007_020cc070(int *self);         /* slot 17 D0 */
/* THE MOUNT'S OTHER EIGHT RAW CODE WORDS. port/ov007_binding_diff.txt section
   3 counts eighteen pointer words the mount leaves holding DS addresses
   because ov007's .text is not mounted, and asks the wiring lane the right
   question about each: "is it dereferenced". The answer for every one of the
   eighteen is now YES, and here is the whole set with where each is handled:
     7   the class vtable's own overridden slots      scene_fill_title
     2   the graphCallback_c table's two ov007 slots  scene_fill_title
     1   the SpawnInfo's +0 factory word              port_scene_registry_install
     6   data_ov007_02103290, the graph-callback      HERE
         table InitResources hands to
         func_ov007_020b7138 as an argument
     2   data_ov007_02103254 / _02103258              HERE
   The six were found the hard way: with the vtables filled and the table left
   raw, the boot faulted at eip 0x01cccab4 accessing 0x020ccab4, which is
   func_ov007_020ccab4's DS address dispatched as a host one, three frames
   under InitResources (func_ov007_020b7138 -> func_ov007_020b68e8 ->
   func_ov007_020c3df4). All eight bodies are matched and in the slice. */
extern unsigned char data_ov007_02103290[];  /* 6 callbacks + the vtable head */
extern unsigned char data_ov007_02103254[];
extern unsigned char data_ov007_02103258[];
int  func_ov007_020ccab4(int a);
int  func_ov007_020cca98(int a);
void func_ov007_020cca80(void);
int  func_ov007_020cca74(void);
int  func_ov007_020cca68(void);
unsigned char func_ov007_020cc600(int arg);
void func_ov007_020c3e4c(void *arg);
void func_ov007_020c3e64(void *arg);
/* the graphCallback_c sub-object's own four, at +0x50 */
int  func_ov007_020cc110(void);              /* gc slot 0 */
int  func_ov007_020cc0f4(void *self);        /* gc slot 2 */
int  _ZN5Scene14GraphCallback1Ev(void *self);/* gc slot 1, matched arm9 */
int  _ZN5Scene14GraphCallback3Ev(void *self);/* gc slot 3, matched arm9 */

/* the seat, minus the Stage (hal/level_boot.cpp) */
void _ZN5Scene9SetFadersEP15FaderBrightness(void *thiz);

void port_scene_a2_seat(void);

/* the minigame RNG seed, hal/scene_mg.cpp. Gates itself on the ROM's
   IsMinigameActorID and on the run's shape; see its header there. */
void port_scene_mg_seed_rng(int id, int windowed);

/* ---- func_02019144's FIRST BEAT, the current scene's graphics block --------
 *
 * THE DEFECT THIS CLOSES, derived off the ROM's own sources and then measured.
 *
 * src/func_02019144.c is the ROM's once-per-frame display sync, called from
 * IRQ::VBlankHandler. It OPENS with the current graphics block's vtable slot 2
 * and RETURNS WITHOUT DOING ANYTHING ELSE when that slot answers 0:
 *
 *     UnkObj *p = data_0209d4a8;
 *     if (p != 0) { if (p->vt->func8(p) == 0) return; }
 *     OAM::Flush(); OAM::Load();
 *     DISPCNT_A = (DISPCNT_A & ~0x1f00) | (data_0209d45c << 8);
 *     DISPCNT_B = (DISPCNT_B & ~0x1f00) | (data_0209d454 << 8);
 *     ... the eight BG scroll registers ...
 *
 * The port has never run that function. It reproduces the TAIL in two places
 * -- hal/message_compositor.cpp publishes the engine A mask, hal/sub_screen.cpp
 * publishes the engine B one and calls OAM::Load -- and it reproduces the HEAD
 * nowhere, standing in for the Stage's slot 2 with hal/sub_actors.cpp's
 * hard-coded port_minimap_affine_update. hal/sub_actors.cpp says so in as many
 * words: "the port does not run func_02019144 ... and nothing seats
 * data_0209d4a8".
 *
 * That is exactly backwards for a scene that OVERRIDES the beat, and scene 1
 * is one. dScDSMT_c::InitResources parks its own graphCallback_c sub-object in
 * data_0209d4a8 (src/func_ov007_020cc4c0.cpp, `data_0209d4a8 = self + 0x50`),
 * the ROM's relocations put func_ov007_020cc0f4 in slot 2 of that block's
 * table (config/arm9/overlays/ov007/relocs.txt: from:0x021032b8 to:0x020cc0f4),
 * and that body returns 0. So on hardware the title screen SUPPRESSES the
 * generic tail and does the whole display sync itself, in
 * func_ov007_020b6eb4: the fade beat, the display capture, the layer setup
 * (func_ov007_020bd648, which is what writes DISPCNT bits 8-12) and its own
 * OAM upload straight to 0x07000000 / 0x07000400.
 *
 * The port ran the tail it should have skipped and skipped the head it should
 * have run, so the scene's own display work never happened AND the port's
 * publish of a zero mask erased the enables. Measured in port/ov007_seat.txt
 * section 5h.
 *
 * WHAT IS DISPATCHED, AND WHAT DELIBERATELY IS NOT. Only a block whose vtable
 * this file has SEATED with host thunks. Every other graphics block in the
 * tree still holds the ROM's raw DS code addresses -- the Stage's own table,
 * data_02092188, is hosted by nobody -- and dispatching one would branch to a
 * DS address. That is the rule vtspan.py already enforces for scene vtables
 * and it is the same rule here. With no seated block current the beat answers
 * 1 and dispatches nothing, which is the port's behaviour before this change,
 * to the byte.
 *
 * SM64DS_GRAPH_BLOCK_OFF=1 puts the old behaviour back on the same binary, so
 * a before/after is one build and one .dsstate base -- what
 * notes/port-selftest-bmp-gate.md requires before two BMPs may be compared. */
void port_graph_block_register(void *vt);
extern "C" int port_graph_block_beat(void);
extern "C" int port_graph_block_verdict(void);


/* the frame: the same calls, in the same order, that walk_window's own loop
   makes (hal/actor_registry.cpp, hal/fader_wipes.cpp, hal/sub_screen.cpp,
   hal/message_compositor.cpp) */
void port_actor_tick(void);          /* phases 4/2/3 */
void port_actor_render(void);        /* phase 5 */
void port_actor_scene_pass(void);    /* phase 1 */
void port_fader_advance(void);
void port_frame_clock_tick(void);    /* phase 6: data_020a0db0 (hal/fader_wipes.cpp) */
/* SM64DS_MG_RESULTS_PROBE (hal/scene_mg.cpp), off unless the variable is set */
void port_mg_results_probe(int frame);
void hal_sub_screen_init_hw(void *hwnd, int zoom);
void hal_sub_screen_probe(void);
void hal_sub_screen_frame_begin(void);
void hal_sub_screen_present(unsigned int *dst, int w, int h);
/* engine B's own raster, written without re-scanning; see its note */
int hal_sub_screen_write_bmp(const char *path);
/* THE STACKED LAYOUT (hal/sub_screen.cpp, run link60 lane DSL1): both DS
   screens full size, top above bottom, which is what a touchscreen game needs
   and the corner panel cannot give it. This file is where a run learns whether
   it is a minigame, so this file is what proposes the mode. */
void hal_sub_screen_set_stacked(int on);
int hal_sub_screen_stacked(void);
unsigned int *hal_sub_screen_stacked_image(const unsigned int *top);
/* the stacked image's live size, which the screen gap makes a variable */
void hal_sub_screen_stacked_size(int *w, int *h);
/* and how many of its rows are the gapless headroom above the top screen */
int hal_sub_screen_stacked_headroom(void);
/* and whether the rows between the halves are a hinge or the world's own rows,
   which is the top engine's OBJ display shift; hal/screen_gap.h carries the
   note and hal/sub_screen.cpp says why it is forwarded rather than read */
int hal_sub_screen_stacked_obj_shift(void);
int IsMinigameActorID(unsigned int id);
void port_message_composite_engine_a(void *fb);
void sdat_host_tick(void);           /* hal/sdat/consumer.cpp */
void out_set_volume_pct(int);        /* hal/sdat/out_win.cpp */
int host_settings_poll(void);        /* hal/host_settings.cpp, the live re-read */
int host_setting_volume(void);       /* same */
void port_scene_mg_seat_sound(int scene_id); /* hal/scene_mg_sound.cpp */

extern int data_020a4b6c[8];         /* the scene tree: head, callback, 0 */
extern void *data_0209f5bc;          /* the installed fader; hal/fader_wipes.cpp */

}  /* extern "C" */

DSSTATE_BEGIN
extern "C" {
/* Scene::SpawnIfNecessary's "a scene has already spawned" latch. The matched
   src/_ZN5Scene21AfterCleanupResourcesEj.cpp DEFINES this byte (a namespace-
   scope `unsigned char data_02092660;` inside extern "C", which in C++ is a
   definition, not a tentative one), and a definition inside src/ cannot be
   bracketed into .dsstate without editing the byte-verified tree. So that TU
   is a host copy for this lane -- port/unmatched/Scene_AfterCleanupResources
   .cpp, same body, the global declared extern -- and the byte lives here,
   inside the bracket, where a save state captures it.
   The arm9 BSS words the star select's own path reaches and no level path
   does. Each is sized by ROM SPAN (the next symbol's address), not by the
   width of the field the one caller happens to touch -- the undersized-global
   trap. All five are bss, so zeroed host storage reads exactly what the DS's
   own cleared BSS reads:
     data_0209d3c0  4   func_02019018's 3D-engine-enabled flag
     data_0209d464  4   func_020190b8's "3D geometry engine armed" latch
     data_0209d478  4   SetBg2Offset's Y
     data_0209d49c  4   SetBg2Offset's X
     data_020a60a8  4   GXS::BeginLoadOBJExtPltt's saved bank bits
     data_020a8048  4   NitroSDK's FS archive-list head
     data_020a804c 12   and its FSDirPos current directory

   THE LAST TWO WERE MIS-IDENTIFIED HERE and the correction is run link60 lane
   NFS's. They were read as "the Vram__Map family's LCDC cursor and its
   three-word bank record", which is not what they are: every literal-pool
   reference to either address in the whole arm9 image is inside the FS module
   (0x020a8048 at 0x0205cc7c / 0x0205cd2c / 0x0205d288, 0x020a804c at
   0x0205cc78 / 0x0205cd30 / 0x0205d870) and nothing in Vram__Map touches
   either. data_020a8048 is the head of the registered-archive list
   func_0205cc80 builds; data_020a804c is the 12-byte FSDirPos
   {FSArchive *arc, u16 own_id, u16 index, u32 pos} that func_0205d714 copies
   into every FSFile it opens by name, which is exactly why three words is
   right for it.

   ONLY THE NAMES CHANGE. Both stay here, at these sizes, in this order: the
   spans were already correct, hal/fs_names.cpp writes them through the ROM's
   own registration rather than by hand, and moving a member out of a .dsstate
   block moves the whole section base for every file in it. See
   port/nfs_names_map.txt.                                                   */
int data_0209d3c0;
int data_0209d464;
int data_0209d478;
int data_0209d49c;
int data_020a60a8;
int data_020a8048;
int data_020a804c[3];
unsigned char data_02092660;
}
DSSTATE_END

/* ---- the one PORT_HOST_ABI face this lane needs ---------------------------
   PORT_HOST_ABI: LoadArchive mounts a NARC through the DS card loader, and the
   host has no mount step to make.
   src/LoadArchive.c walks data_0208ecf4, the ROM's 13-entry archive-mount
   table of {ptr, heap, idBase, idEnd, shortName, narcPath}, and calls
   func_02018934 to pull the whole NARC into the game heap. The port resolves
   archive-interior file IDs (>= 0x8000) LAZILY instead: hal/fs.cpp's
   port_fs_archive_fill walks port_archive_map -- the host-shaped copy
   port/tools/romdata.py generates from that same ROM table -- finds the
   archive whose id range covers the request, loads its image off disk on first
   use and decodes the member. So on the host an archive is never "mounted" and
   never "not mounted"; the id resolves either way.
   That makes the ROM's mount call a no-op whose only observable is its return
   value, which is "is archive N available". On the host every archive in the
   table is available, so the answer is 1. Returning it is what lets
   LoadTextNarcs (matched, in the slice) run its real language switch and what
   lets dScStarSel_c::InitResources proceed to the LoadFile calls the fs seam
   really does serve.
   The alternative -- linking the matched TU -- would need data_0208ecf4 hosted
   as raw ROM bytes with its DS string pointers, plus func_02018934 and the
   four-TU card-loader chain under it, to produce a mount the fs seam then
   ignores. That is a fake, not a fix, so the seam is here in the open. */
extern "C" int LoadArchive(int idx)
{
    return (unsigned)idx < 13u;
}

/* ---- C-name faces for three namespaced arm9 functions ---------------------
   The cxxname_bridge pattern in reverse: these three matched TUs define their
   function INSIDE a C++ namespace (`namespace GX { void SetBankForTex(u16) }`,
   `namespace G3X { void SetFog(bool,int,int,int) }`), so they export a mangled
   name, while src/func_ov003_020af8a0.c calls them by the C name every other
   arm9 spelling in the port uses. One forwarding definition each. Their
   siblings (SetBankForTexPltt, SetBankForSubBG, SetBankForSubOBJ,
   SetBankForSubOBJExtPltt) are .c TUs and already export the C name, which is
   why only these three are here. */
namespace GX { void SetBankForTex(unsigned short); void SetBankForOBJ(unsigned short); }
namespace G3X { void SetFog(bool enable, int a, int b, int c); }
extern "C" void _ZN2GX13SetBankForTexEt(unsigned short v) { GX::SetBankForTex(v); }
extern "C" void _ZN2GX13SetBankForOBJEt(unsigned short v) { GX::SetBankForOBJ(v); }
extern "C" void _ZN3G3X6SetFogEbiii(int e, int a, int b, int c)
{ G3X::SetFog(e != 0, a, b, c); }

// ============================================================================
// run link60 lane L2: everything the ov007 slice needed that was not a TU.
// ============================================================================
//
// ---- 1. THE TITLE SCREEN'S OWN arm9 STORAGE -------------------------------
//
// Eight arm9 globals no level path has ever reached. Five of them are ONE
// LOGICAL OBJECT that dsd split into five symbols, and getting that wrong is
// the undersized-global trap in its sharpest form: dScDSMT_c::InitResources
// writes data_0209b340[0x27], which is byte +0x9c of a symbol whose delta to
// the next dsd name is ELEVEN. Five separate host globals would put that store
// 145 bytes past the end of its array.
//
// So the run 0x0209b33c .. 0x0209b3ec is hosted CONTIGUOUSLY, at the ROM's own
// spacing, using the ordered-section idiom tools/ovdata.py already uses for a
// packed mount, and port_l2_pack_check() proves the layout at run time rather
// than trusting the linker to have honoured it. Sizes are ROM deltas from
// config/arm9/symbols.txt: 4, 11, 3, 138, 20.
//
// All five are BSS, so zeroed host storage reads exactly what the DS's own
// cleared BSS reads. They sit inside .dsstate so a save state captures them.
#pragma section(".dsstate$l2_00", read, write)
#pragma section(".dsstate$l2_01", read, write)
#pragma section(".dsstate$l2_02", read, write)
#pragma section(".dsstate$l2_03", read, write)
#pragma section(".dsstate$l2_04", read, write)
extern "C" {
/* the file-select record pointer */
__declspec(allocate(".dsstate$l2_00")) __declspec(align(1))
unsigned char data_0209b33c[4];
/* the title state block. ELEVEN bytes by dsd delta, reached at +0x9c */
__declspec(allocate(".dsstate$l2_01")) __declspec(align(1))
unsigned char data_0209b340[11];
__declspec(allocate(".dsstate$l2_02")) __declspec(align(1))
unsigned char data_0209b34b[3];
__declspec(allocate(".dsstate$l2_03")) __declspec(align(1))
unsigned char data_0209b34e[138];
__declspec(allocate(".dsstate$l2_04")) __declspec(align(1))
unsigned char data_0209b3d8[20];
}

DSSTATE_BEGIN
extern "C" {
/* The three that stand alone. data_0209d4a8 is the Scene fader pointer
   InitResources parks and CleanupResources clears; data_0209d524 is the
   "object overlays are loaded" flag both InitResources and Behavior branch on;
   data_0208ee3c is arm9 .DATA, not bss, so it carries the ROM's own value --
   read at extracted/arm9_dec.bin + (addr - 0x02004000), the base
   port/tools/romdata.py documents, which is 01 00 00 00. Hosting it zeroed
   would have been a silent one-bit lie. */
/* data_0209d4a8 is NOT defined here: hal/w8a_stage_storage.cpp already hosts
   it. Only the mangled spelling is added, below. */
int data_0209d524;
unsigned char data_0208ee3c[4] = { 1, 0, 0, 0 };
}
DSSTATE_END

static void port_l2_pack_check(void)
{
    struct { const unsigned char *p; int want; const char *n; } k[] = {
        { data_0209b33c,   0, "data_0209b33c" },
        { data_0209b340,   4, "data_0209b340" },
        { data_0209b34b,  15, "data_0209b34b" },
        { data_0209b34e,  18, "data_0209b34e" },
        { data_0209b3d8, 156, "data_0209b3d8" },
    };
    for (int i = 0; i < 5; ++i)
        if (k[i].p - data_0209b33c != k[i].want)
            std::fprintf(stderr, "  [scene] L2 PACK BROKEN: %s at +%d, ROM "
                         "says +%d\n", k[i].n,
                         (int)(k[i].p - data_0209b33c), k[i].want);
}

// ---- 1b. THREE HAND-ASM PRIMITIVES, HOSTED ---------------------------------
//
// PORT_HOST_ABI: all three TUs are `asm void` blocks, which MSVC cannot parse.
// src/Matrix3x3_LoadIdentity.c, src/MultiStore32Bytes.c and src/func_02052ec8.c
// carry the tree's HAND-ASM PRIMITIVE banner -- they were assembly in the
// original and there is no C to decompile them to -- so they are transcribed
// here instead of being compiled, they are NOT in the slice, and none of the
// three counts as linked. The asm is short enough to state and to check
// against, and the whole slice was swept for the pattern so these are all of
// them rather than the ones that happened to surface.
//
//   func_02052ec8(m)   the 4x4 sibling of the one below: seven stmia bursts
//     ({r2,r3} {r1,r3} {r1,r2,r3} {r1,r3} {r1,r2,r3} {r1,r3} {r1,r2}) with
//     r2 = 0x1000 and r1 = r3 = 0, which lands 0x1000 at words 0, 5, 10 and 15
//     and zero everywhere else -- sixteen words, a 4x4 identity. The register
//     list order is what fixes the pattern: stmia stores lowest register
//     first, so {r1,r2,r3} is 0, 0x1000, 0 and not 0x1000, 0, 0.
//
//   Matrix3x3_LoadIdentity(m)   mov r2,#0x1000 / str r2,[r0,#0x20] / mov r3,#0
//                               / stmia r0!,{r2,r3} / mov r1,#0
//                               / stmia r0!,{r1,r3} / stmia r0!,{r2,r3}
//                               / stmia r0!,{r1,r3}
//     which lays down 0x1000,0,0, 0,0x1000,0, 0,0,0x1000 -- a 3x3 identity in
//     the game's 1.12 fixed point, with [8] written FIRST and the other eight
//     in four pairs. Its one caller, src/func_ov007_020c42f8.c, declares it
//     `void Matrix3x3_LoadIdentity(Mat3 *m)`, which is the shape used here.
//
//   MultiStore32Bytes(val,dst,len)  fills len BYTES from dst with the 4-byte
//     val: eight words at a time up to (len >> 5) << 5, then one word at a
//     time up to dst + len. All five call sites declare the same three
//     parameters, so there is no argument-count question here.
extern "C" void func_02052ec8(int *m)
{
    for (int i = 0; i < 16; ++i) m[i] = 0;
    m[0] = m[5] = m[10] = m[15] = 0x1000;
}
extern "C" void Matrix3x3_LoadIdentity(int *m)
{
    m[8] = 0x1000;
    m[0] = 0x1000; m[1] = 0;
    m[2] = 0;      m[3] = 0;
    m[4] = 0x1000; m[5] = 0;
    m[6] = 0;      m[7] = 0;
}
extern "C" void MultiStore32Bytes(unsigned val, int *dst, int len)
{
    for (int i = 0; i + 4 <= len; i += 4)
        *dst++ = (int)val;
}

// ---- 2. THE OVERLAY LOADER, WHICH THE HOST DOES NOT HAVE -------------------
//
// The four overlay-id symbols dScDSMT_c names. On the DS these are LINKER
// symbols whose ADDRESS is the id -- src spells the argument `(int)&overlay_100`
// -- and MSVC has no way to give a C++ global an absolute address, so on the
// host `&overlay_100` is an ordinary host address and NOT 100. Stated rather
// than papered over.
//
// It costs nothing measurable and the reason is worth writing down. The two
// consumers are both linked matched TUs and both are keyed on the id:
//   func_02017e94(id)  unload. It scans data_0209d3c4[12] for an entry whose
//                      first word equals the key and returns early when there
//                      is none. InitResources calls it twice, UNCONDITIONALLY,
//                      on the first frame -- so this one really does run, and
//                      with a host-address key it finds nothing and returns,
//                      which is also what the ROM does when the overlay is not
//                      resident.
//   LoadOverlay(id)    load, reached only from Behavior's result == 6 branch,
//                      which is a menu confirm. No idle run reaches it.
// Neither could do the real thing anyway: the port has no overlay loader,
// because every overlay it hosts is a static host array mounted at build time
// (this file's own section 2). The ids are here so the symbols resolve, and
// the day an overlay loader exists they become its first customer.
extern "C" { int overlay_64, overlay_66, overlay_100, overlay_102; }

/* THE FOUR ENTRY POINTS THEMSELVES, faced rather than linked, and this is the
   one place this lane traded linkage for honesty on purpose.
   All four have matched src TUs and all four were IN the slice for one build.
   Taking them meant taking the DS card overlay/archive loader under them --
   FS_LoadOverlay, func_02018c00, func_0203d7b8, func_0205e088, func_02017fd0,
   func_02018908, func_0205dc0c -- plus hosting data_0208ecf4, the 13-entry
   archive-mount table whose entries are DS STRING POINTERS, and data_02075998
   / data_02075804, the object-overlay id tables. Eleven TUs and three
   pointer-bearing arm9 tables, to drive a load that resolves to nothing:
   the host has no overlay loader at all, because every overlay it hosts is a
   static host array mounted at build time (section 2 of this file), and
   hal/fs.cpp resolves archive-interior file ids lazily so an archive is never
   mounted and never not mounted. That is the same trade the LoadArchive face
   above already makes, in the same direction, and it is recorded as a cost:
   ELEVEN MATCHED TUs THIS SLICE COULD HAVE COUNTED AND DID NOT.
   The observable each face has to reproduce is nothing: three return void and
   the fourth is void, none has an out-parameter, and the ROM's own answer when
   the overlay is not resident is to do nothing. */
extern "C" void LoadOverlay(int)                       {}
extern "C" void func_02017e94(int)                     {}
extern "C" void UnloadArchives(void)                   {}
extern "C" void LoadOrUnloadObjectOverlays(void (*)(int), int) {}

/* PORT_HOST_ABI: CP15::EnableDTCM sets the ARM946 control register's DTCM
   enable bit, and the host has no CP15. Its TU is a fourth `asm` block MSVC
   cannot parse (`asm { mrc p15,0,v,c1,c0,0 }` / or 0x10000 / `mcr` back), so
   it is not in the slice and does not count as linked. The ROM's observable is
   the control-register value it returns, and the one thing any caller does
   with that is hand it back to CP15::DisableDTCM to restore. Returning the bit
   set is the answer that round-trips; there is no host state to change. */
extern "C" unsigned _ZN4CP1510EnableDTCMEv(void) { return 0x10000u; }

/* THE DS BACKUP-MEDIA DRIVER, the second family this lane refused, and the
   second time for the same reason. SaveData::ReadDataFromCart and
   SaveData::SaveDataToCart are matched TUs and were both in the slice for one
   build; under them is the card/backup driver (func_0203da3c, func_0206045c,
   func_02057020, func_0205ff80, func_0205ff70, func_02057078 and the rest of
   the CARD_ chain), which the host does not have and which no amount of
   hosted arm9 data substitutes for. Their four callers -- ReadFileData,
   ReadMinigameData, EraseSaveFile and SaveFile -- ARE in the slice and do
   link, so what is faced is the two leaves and not the save logic above them.

   THE OBSERVABLE IS AN int AND ITS CONVENTION IS THE OPPOSITE OF WHAT THIS
   BLOCK USED TO SAY. Both faces returned 0, and the prose here called 0 "the
   ROM's own answer on a failed read". Run link60 Stage 5 lane MR1 traced the
   read on a live path, its review found the ROM evidence sitting unopened in
   src/, and run link60 Stage 5 lane SV1 re-derived it off BOTH matched bodies
   before changing anything:

     src/_ZN8SaveData16ReadDataFromCartEPcjj.cpp
       0  ONLY after the eight bytes read out of the record match
          data_020a4b40 AND the rotate-xor checksum over the payload equals
          the stored crc. A VALIDATED READ HAPPENED.
       1  every failure path. The FIRST statement in the body is
          `if (func_0203da3c() == 2) return 1;`, the no-media answer, and a
          host with no cart is permanently in that state.
       2  the backup copy was read but its magic did not match the primary's.

     src/_ZN8SaveData14SaveDataToCartEPcjj.cpp
       0  ONLY after BOTH copies, primary and backup, were written whole.
       1  every failure path, and its first statement is the same no-media
          test. There is no 2 on the write side.

   SO 0 IS THE SUCCESS VALUE ON BOTH, and the callers read them that way. The
   two readers are `result = ReadDataFromCart(...); if (result) {
   SetDefaultValues*(...); ... }`, so a face returning 0 told the game an
   uninitialised buffer was a validated save and SKIPPED the defaults; for
   ReadFileData that buffer is fresh Memory::operator_new2(0xcc) storage
   nothing writes. The two writers invert (`if (SaveDataToCart(...) == 0)
   return 1;`), so a face returning 0 told SaveFile and EraseSaveFile that
   bytes reached a cart this host does not have. Both directions were a lie
   and neither was the safe one.

   BOTH RETURN 1 NOW. It is the ROM's own no-media answer to both questions
   and the only value either face can honestly give.

   AND THE FILE-SELECT SENTENCE THAT USED TO CLOSE THIS BLOCK WAS WRONG IN THE
   SAME DIRECTION, which is worth spelling out because it read as a measured
   behaviour and was a derivation from the backwards premise. It said returning
   0 is "why the file select shows empty files". 0 is the value that SKIPS
   SetDefaultValues, so it is the value that leaves the file select reading
   uninitialised storage; 1 is the value that constructs the defaults. Nothing
   has rendered a file select yet, so what one will show is still unmeasured
   and this block no longer guesses. What changed is which value gets there.

   THE FLIP COULD NOT LAND ALONE, and that is the whole reason MR1 recorded it
   instead of taking it. It fires SaveData::SetDefaultValues and
   SetDefaultValuesMg, which were reached through two receiver-dropping
   /alternatename directives in section (c) below that MR1 measured, ruled
   unfired and deliberately left standing. They were unfired BECAUSE of the
   value being corrected here. Both are faced in the same commit, the write
   side is audited above rather than left for someone to find, and the
   0x2e4-byte default the Mg sibling writes needed the save block hosted at
   its real ROM width first (hal/level_boot.cpp's fifth SAVEBLK row).
   port/ov007_seat.txt section 5g carries the trace and the run. */
extern "C" int _ZN8SaveData16ReadDataFromCartEPcjj(char *, unsigned, unsigned)
{ return 1; }
extern "C" int _ZN8SaveData14SaveDataToCartEPcjj(char *, unsigned, unsigned)
{ return 1; }

// ---- 2b. FIVE MORE arm9 GLOBALS, AND ONE THAT IS A VTABLE ------------------
//
// data_0208ea6c IS NOT DATA, it is a twelve-slot vtable in arm9 .data, and it
// is on the critical path: func_02017278 (the factory's last call, which
// constructs the object's member at +0x54) writes four vptrs in construction
// order and this is the last of them, and func_02017254 (which BOTH dtors
// call) writes it back and then runs Color::D1. Its twelve words are all
// relocated code addresses -- 0x02017254, 0x02017228, 0x0201721c, 0x020171f0,
// 0x020171c8, 0x02017684, 0x02017670, 0x02017628, 0x0201761c, 0x02017610, a
// non-code word at +0x2c, 0x02017450.
//
// IT IS TRAP-FILLED, NOT ROM-FILLED, and that is a deliberate difference from
// its three siblings. data_0208eafc, data_0208eacc and data_0208eb2c -- the
// other three vptrs func_02017278 writes -- are already in the build carrying
// their raw ROM bytes, so a dispatch through any of them today jumps to a DS
// address. This one goes the other way, on hal/sub_actors.cpp's precedent: a
// dispatch lands on a named trap instead of on 0x02017254 interpreted as a
// host address. NOTHING DISPATCHED THROUGH IT IN ANY RUN THIS LANE MADE.
// Filling the twelve for real is twelve more bodies and is not this lane's.
static void l2_trap(const char *name);
static void l2_vt_trap(void) { l2_trap("data_0208ea6c vtable slot"); }
DSSTATE_BEGIN
extern "C" {
void *data_0208ea6c[12];
/* data_0208eb2c is the SAME SHAPE and the same critical path: func_02017278
   writes it as the third of the member's four vptrs, and its ten words are
   relocated code addresses too (0x020175c4, 0x02017598, 0x020174e0,
   0x020176d8, 0x02017698, ...). Trap-filled for the same reason. The other
   two the ctor writes, data_0208eafc and data_0208eacc, are already in the
   build carrying raw ROM bytes and this lane did not touch them. */
void *data_0208eb2c[10];
/* data_0208eacc, the SECOND of the member's four vptrs, joins them: 48 bytes,
   twelve words, relocated code addresses. Three of the four are now filled
   here and only data_0208eafc is still shipping raw ROM bytes, because
   nothing this lane compiled asked for it. */
void *data_0208eacc[12];
/* AND THE FOURTH, data_0208eafc, which closes the set func_02017278 writes.
   This one is NOT uniformly pointers: only three of its twelve words are
   relocated (+0 -> 0x0201786c, +4 -> 0x02017848, +44 -> 0x0208ea00) and the
   nine in between are literal zero in the ROM. So it carries its ROM bytes
   and only the three relocated words are trap-filled at seat time, which is
   the difference between hosting a vtable and hosting a table that happens to
   start with two function pointers. The +44 word points at another arm9 data
   symbol rather than at code and is left as the ROM has it. */
unsigned char data_0208eafc[48] = {
    108,120,1,2, 72,120,1,2, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,234,8,2
};
int data_020a80e4[8];                              /* bss, 32 by ROM span */
/* a 256-byte pure-data table func_020538b8 indexes. NO relocation anywhere in
   config/arm9/relocs.txt lands inside 0x02086214 .. 0x02086314, so the ROM
   bytes are the whole truth and they are carried verbatim. */
unsigned char data_02086214[256] = {
    0,0,81,0,163,0,244,0,70,1,151,1,233,1,58,2,
    139,2,220,2,45,3,126,3,207,3,32,4,112,4,193,4,
    17,5,97,5,177,5,1,6,81,6,160,6,239,6,62,7,
    141,7,220,7,42,8,120,8,198,8,20,9,97,9,174,9,
    251,9,72,10,148,10,224,10,44,11,119,11,194,11,13,12,
    87,12,161,12,235,12,52,13,125,13,198,13,15,14,86,14,
    158,14,229,14,44,15,115,15,185,15,255,15,68,16,137,16,
    206,16,18,17,86,17,153,17,220,17,31,18,97,18,163,18,
    228,18,37,19,102,19,166,19,230,19,37,20,100,20,162,20,
    224,20,30,21,91,21,152,21,213,21,17,22,76,22,136,22,
    194,22,253,22,55,23,112,23,170,23,226,23,27,24,83,24,
    138,24,193,24,248,24,46,25,100,25,154,25,207,25,4,26,
    56,26,108,26,159,26,211,26,5,27,56,27,106,27,156,27,
    205,27,254,27,46,28,94,28,142,28,190,28,237,28,27,29,
    74,29,120,29,165,29,211,29,255,29,44,30,88,30,132,30,
    176,30,219,30,6,31,48,31,90,31,132,31,174,31,215,31
};
unsigned char data_0209a624[4] = { 1, 0, 0, 0 };   /* arm9 .data, ROM value */
int data_020a637c[9];                              /* bss, 36 by ROM span */
int data_020a80cc[6];                              /* bss, 24 by ROM span */
/* data_0209caa0 is NOT defined here: hal/level_boot.cpp already hosts it as
   a 0x14 save block, and its own header records that the ROM object is wider
   than the dsd symbol. Only the mangled spelling below is added. */
}
DSSTATE_END
static void l2_fill_0208ea6c(void)
{
    for (int i = 0; i < 12; ++i) data_0208ea6c[i] = (void *)l2_vt_trap;
    for (int i = 0; i < 10; ++i) data_0208eb2c[i] = (void *)l2_vt_trap;
    for (int i = 0; i < 12; ++i) data_0208eacc[i] = (void *)l2_vt_trap;
    ((void **)data_0208eafc)[0] = (void *)l2_vt_trap;
    ((void **)data_0208eafc)[1] = (void *)l2_vt_trap;
}

// ---- 3. NINETEEN TRAPPING SITES, AND HOW THEY COUNT ------------------------
//
// WAS TWENTY-FOUR, then twenty-one, then twenty. Four ov007 traps came out
// when their matched TUs arrived, and a fifth came out WITHOUT a match: run
// link60 lane SC1 gave func_ov007_020c9688 a host transcription instead
// (port/unmatched/Ov007_OamCellBank_020c9688.cpp), which is a different kind of
// retirement and section 3a says so.
//
// COUNT THEM AS 18 L2_UNMATCHED BODIES PLUS ONE TRAP-FILLED VTABLE, because
// they are not the same kind of thing and one number hides that. The 18 are
// functions with no C anywhere in the tree, each standing where a body would
// be. The 19th is the twelve slots of data_0208ea6c (section 2b), which all
// point at one shared trap: a hosted arm9 vtable this lane chose to fill
// loudly rather than leave carrying raw DS words. One counter covers all 19
// and the run prints it, so "none of them fired" is a measurement.
//
// ---- 3a. FIFTEEN UNMATCHED ov007 BODIES, FOURTEEN OF THEM TRAPPED ----------
//
// WAS NINETEEN. Four of them are decompiled now and the traps are gone.
// func_ov007_020beeb0, func_ov007_020c7d60 and func_ov007_020cbbb0 landed on
// the decomp's main between 2026-08-05 and 2026-08-06, after this branch
// forked, and run link60's port-catchup lane brought the matched TUs across
// by address; see port/port_catchup_inventory.txt rows 1-3.
// func_ov007_020b2998 is the fourth and the newest: lane CK1 matched it on
// 2026-08-16 (main db0c4960635e, PR #1536) and lane PC2 brought it across the
// same day.
//
// Each was re-verified here against extracted/overlays/overlay_0007.bin rather
// than taken on main's word; the dsd export is stale for this overlay and a
// control run on src/func_ov007_020ba2e0.c, matched on both refs, proves it.
// PC2 re-ran that control rather than quoting it: NOMATCH against
// extracted/dsd/arm9_overlays/ov007.bin and MATCH against the raw image, in
// this worktree, on the day it wired its own TU.
//
// A FOURTH ov007 BODY CAME ACROSS IN THE SAME PASS AND IS NOT IN THIS LIST:
// func_ov007_020c49bc, which never had a trap because nothing references it
// (the note after the list explains why). It is in slice_ov007.txt now, so
// /OPT:REF still drops it, but the day a slice reaches it the matched body is
// what answers instead of an unresolved external.
//
// ov007 is 548 functions and 533 have a matched src TU. The fifteen that do
// not are inside delink blocks marked incomplete, they are called from bodies
// that ARE matched, and there is no C for them anywhere in the tree. A
// plausible hand-written body would be exactly the guess the inferred-stub
// guard exists to refuse, so fourteen of them are a TRAP that names itself once
// and returns zero. A run that enters one says so on stderr and keeps going,
// which is what makes "none of them was entered" a measurement instead of an
// assumption. (533 and 15 were RECOUNTED for this edit rather than stepped
// from the old pair: every ov007 function in symbols.txt joined to its delink
// block and to what is on disk, with func_ov007_020bfd70 counted through its
// src file because its block is the one exclusion slice_ov007.txt names.)
//
// NONE OF THE NINETEEN FIRED IN ANY RUN THE SEATING LANE MADE. THAT SENTENCE
// USED TO CONTINUE "and none of the sixteen has fired since" AND RUN LINK60
// LANE SC1 MADE IT FALSE, which is the good direction: giving 0x020c9688 a
// body is what lets the scene run far enough to reach the others at all. A
// 300-frame scene-1 run today enters func_ov007_020b46b0 twenty-four times
// (the 0x18 loop in src/func_ov007_020aed98.c), then func_ov007_020c20b8,
// func_02054c80 and one data_0208ea6c slot once each. THE CENSUS LOST A NAME
// THIS PASS: func_ov007_020b2998 used to be in that list and it was the only
// blocker in it. Lane CK1 matched it, lane PC2 brought the TU across, and the
// scene now dies TWO CALL LEVELS DEEPER on something that is not a trap at all
// (port/ov007_seat.txt section 5b). NONE OF THE FOUR NAMES LEFT IS A BLOCKER:
// they return 0 and the run carries on, which is the trap doing its job rather
// than the trap being harmless. port/ov007_seat.txt carries the counter
// readback for the nineteen and the gate manifest carries it for the fifteen.
//
// FOURTEEN OF THE FIFTEEN ARE TRAPPED. THE FIFTEENTH IS TRANSCRIBED, and the
// distinction is the whole of run link60 lane SC1. func_ov007_020c9688 is the
// one the trap list can no longer carry, because a trap there is not a
// coverage statement, it is scene 1's block: the trap returned 0 six times,
// func_ov007_020ade58 dereferenced one of them, and the title screen could not
// come up at all. port/unmatched/Ov007_OamCellBank_020c9688.cpp is a HOST
// TRANSCRIPTION of the ROM's 192 instructions at that address, derived at the
// config-aligned base out of extracted/overlays/overlay_0007.bin with all four
// of its relocations resolved and its one literal-pool word read.
//
// IT IS NOT A DECOMP AND IT DOES NOT PRETEND TO BE ONE. It is not scored by
// match.py, not counted by linkage.py, and it carries no
// "recovered from vtable slot identity" marker, so the inferred-stub guard
// neither counts it nor should. The number above stays at fifteen because the
// DECOMP is still fifteen short; the crack side owns closing it, and the day
// src/func_ov007_020c9688 exists the interim leaves the build in the same
// configure that notices. It went sixteen to fifteen this pass for the OTHER
// reason, the one that costs nothing: func_ov007_020b2998 got a real match.
static unsigned g_l2_trap_hits;
static void l2_trap(const char *name)
{
    ++g_l2_trap_hits;
    /* BOTH STREAMS, AND FLUSHED, and that is not belt-and-braces. stderr goes
       to the flight recorder's playlog FILE -- walk_window redirects it -- so a
       parent capturing the child's pipes never sees a word of it, which is
       exactly how port/tools/battery.py's SCENE_BLOCKED marker check reads the
       run. stdout is the stream the battery can see, and the flush is what
       survives the fault that usually follows a trap. */
    std::fprintf(stderr, "  [scene] UNMATCHED ov007 body entered: %s "
                 "(returns 0; port/ov007_seat.txt section 5)\n", name);
    std::fflush(stderr);
    std::printf("  [scene] UNMATCHED ov007 body entered: %s "
                "(returns 0; port/ov007_seat.txt section 5)\n", name);
    std::fflush(stdout);
}
extern "C" unsigned port_l2_trap_hits(void) { return g_l2_trap_hits; }
#define L2_UNMATCHED(sym)                                                      \
    extern "C" int sym(void);                                                  \
    extern "C" int sym(void) { l2_trap(#sym); return 0; }
L2_UNMATCHED(func_ov007_020ae834)
L2_UNMATCHED(func_ov007_020b1718)
/* func_ov007_020b2998 WAS HERE AND IT CAME OUT ON A REAL DECOMP. The decomp's
   main matched it as db0c4960635e on 2026-08-16 (PR #1536) and run link60's
   PC2 lane brought the matched TU across by address, so src/ has the body and
   a trap here would be an LNK2005 against it. This is the OTHER way an address
   leaves this list, and it is the one the port wants: 020c9688 below left on a
   host transcription, this one left on the ROM's own C. */
L2_UNMATCHED(func_ov007_020b46b0)
L2_UNMATCHED(func_ov007_020b8188)
L2_UNMATCHED(func_ov007_020ba05c)
L2_UNMATCHED(func_ov007_020c19cc)
L2_UNMATCHED(func_ov007_020c20b8)
L2_UNMATCHED(func_ov007_020c368c)
L2_UNMATCHED(func_ov007_020c4684)
L2_UNMATCHED(func_ov007_020c6e68)
/* func_ov007_020c9688 WAS HERE AND IT IS THE ONE THAT CAME OUT WITHOUT A
   DECOMP. It is still unmatched on main, so it is still one of section 3a's
   fifteen; what changed is that this address now has a body in the port and no
   longer needs a trap. port/unmatched/Ov007_OamCellBank_020c9688.cpp is a HOST
   TRANSCRIPTION read off extracted/overlays/overlay_0007.bin, and it retires
   itself the day src/ gains the match (the CMake block that adds it is guarded
   on the src TU not existing). Defining the trap as well would be an LNK2005,
   and leaving the trap INSTEAD would be choosing a known-wrong answer over a
   derived one now that somebody has read the ROM at that address. */
L2_UNMATCHED(func_ov007_020caeac)
L2_UNMATCHED(func_ov007_020cb4b0)
L2_UNMATCHED(func_ov007_020cb7c0)
/* 020b8fd4 surfaced only in the SECOND link, because its one caller spells it
   untagged as func_020b8fd4 (face (a) below) and the untagged name resolved
   before the tagged one was ever asked for. It was the nineteenth of the
   original nineteen and it is the fifteenth of the fifteen. */
L2_UNMATCHED(func_ov007_020b8fd4)
#undef L2_UNMATCHED

/* THE SEVENTEENTH ov007 FUNCTION WITHOUT A BODY HERE GETS NO TRAP, AND THE
   REASON CHANGED. func_ov007_020c49bc (0x020c49bc, 0x440 bytes) IS matched now
   -- it landed on the decomp's main as 02e53421e on 2026-08-06 and the
   port-catchup lane brought it across -- so src/func_ov007_020c49bc.c is in
   slice_ov007.txt and the tree has real C for it. It still gets no trap and
   still contributes nothing to the link, because NOTHING in the slice
   references it: the linker never asks for the symbol and /OPT:REF drops the
   object. The old reason (a trap would be a definition with no caller) has
   simply been replaced by a better one (a matched body is what a future
   caller should get). If a future slice does reach it, it resolves to the
   ROM's own code instead of failing, which is the direction that improves.

   That leaves func_ov007_020ba05c as the one ov007 address where main has a
   src file this branch cannot take, and the reason is that main does not have
   it either in the sense that matters. src/func_ov007_020ba05c.c IS on main,
   but its delink block carries no `complete`, so it is not enrolled: dsd
   supplies that range from ROM bytes, main's ROM build never compiles the
   file and no byte gate has ever run on it (tools/delaunder.py: "a
   non-enrolled file has no byte gate behind it"). Compiled anyway, in a
   detached worktree of origin/main with main's own tools/match.py and main's
   own compiler set, mwccarm 2004/b56 at -lang c99 rejects three member
   accesses the file's own structs do not declare -- 'array28' and 'array24'
   on StructObj20, 'f2C' on StructAInner -- and cl.exe rejects the same three.
   It is a configured draft, not a match, and the trap stays.
   port/port_catchup_inventory.txt row 7 carries the full readout, including
   the one thing in it that IS a decomp-side finding: chaos-db records the
   address as matched:true anyway, because its matched test is "a src file
   exists and carries no NONMATCHING banner" and never consults the enrolled
   set.

   FOUR MORE UNMATCHED BODIES, NOT ov007's, that the arm9 closure pulled in.
   Same treatment and the same counter, listed apart because they are a
   different debt: three are arm9/itcm functions with no C anywhere in the
   tree, and two are cross-overlay calls out of src/func_0201a458.c into
   overlays this build does not host. Naming them here is what makes them
   visible; guessing bodies for them would not be.
     func_01ffaa34  ITCM, config/arm9/itcm/symbols.txt names it and no src
                    defines it. Reached from func_ov007_020c5c14.
     (func_02054c80 WAS IN THIS LIST AND IS RETIRED. The note said "no arm9
      symbol at that address at all, so dsd's name is a guess about an
      address", and that was wrong: config/arm9/symbols.txt names
      `Vram__Map kind:function(arm,size=0xb8) addr:0x02054c80`, the matched
      body is src/Vram__Map.c, slice_gate10.txt carries it and it is in the
      link already. The trap was shadowing a body the binary contained under
      its other name, so every func_02054430 call -- which is how the VRAM
      banks are put back into LCDC mode -- returned 0 and mapped nothing.
      It is a name-spelling face now; see section 4(b).)
     func_0211d9c0  reached from func_0201a458 (the heap-for-the-next-scene
     func_02140d80  helper). Both are addresses in overlays this build does
                    not host, and ov007 is not co-resident with either. */
#define L2_UNMATCHED(sym)                                                          extern "C" int sym(void);                                                      extern "C" int sym(void) { l2_trap(#sym); return 0; }
L2_UNMATCHED(func_01ffaa34)
L2_UNMATCHED(func_0211d9c0)
L2_UNMATCHED(func_02140d80)
#undef L2_UNMATCHED

// ---- 4. NAME-SPELLING FACES, AND NOT ONE OF THEM IS A BODY -----------------
//
// (a) TWENTY-ONE ov007 FUNCTIONS THE SOURCE SPELLS WITHOUT ITS OVERLAY TAG.
//     `func_020b2160` and its twenty siblings are addresses inside ov007's own
//     .text (0x020ad660 .. 0x020ccb54) that some ov007 TUs name with the bare
//     `func_` prefix while the config -- and the TU that defines each body --
//     names them func_ov007_*. Checked one at a time: every one of the
//     twenty-one has an ov007 function symbol at that exact address, and
//     twenty of the twenty-one have NO symbol at that address in ov002 or
//     ov006 either, so there is no ambiguity to resolve. The exception is
//     0x020c897c, which ov002 also names; ov002 and ov007 are mutually
//     exclusive occupants of one slot and the caller here is ov007's, so
//     ov007's body is the right destination in this direction.
#pragma comment(linker, "/alternatename:_func_020b2160=_func_ov007_020b2160")
#pragma comment(linker, "/alternatename:_func_020b2370=_func_ov007_020b2370")
#pragma comment(linker, "/alternatename:_func_020b2728=_func_ov007_020b2728")
#pragma comment(linker, "/alternatename:_func_020b2cf0=_func_ov007_020b2cf0")
#pragma comment(linker, "/alternatename:_func_020b413c=_func_ov007_020b413c")
#pragma comment(linker, "/alternatename:_func_020b4464=_func_ov007_020b4464")
#pragma comment(linker, "/alternatename:_func_020b7658=_func_ov007_020b7658")
#pragma comment(linker, "/alternatename:_func_020b7a00=_func_ov007_020b7a00")
#pragma comment(linker, "/alternatename:_func_020b7a34=_func_ov007_020b7a34")
#pragma comment(linker, "/alternatename:_func_020b8fd4=_func_ov007_020b8fd4")
#pragma comment(linker, "/alternatename:_func_020b91b4=_func_ov007_020b91b4")
#pragma comment(linker, "/alternatename:_func_020bee14=_func_ov007_020bee14")
#pragma comment(linker, "/alternatename:_func_020bfaf0=_func_ov007_020bfaf0")
#pragma comment(linker, "/alternatename:_func_020c232c=_func_ov007_020c232c")
#pragma comment(linker, "/alternatename:_func_020c2390=_func_ov007_020c2390")
#pragma comment(linker, "/alternatename:_func_020c3df4=_func_ov007_020c3df4")
#pragma comment(linker, "/alternatename:_func_020c78b0=_func_ov007_020c78b0")
#pragma comment(linker, "/alternatename:_func_020c80a4=_func_ov007_020c80a4")
#pragma comment(linker, "/alternatename:_func_020c844c=_func_ov007_020c844c")
#pragma comment(linker, "/alternatename:_func_020c897c=_func_ov007_020c897c")
#pragma comment(linker, "/alternatename:_func_020c93b4=_func_ov007_020c93b4")
//
// (b) FIVE arm9 FUNCTIONS THE SOURCE NAMES BY ADDRESS. Each resolves to a real
//     arm9 symbol at OFFSET ZERO -- not an interior address, which is the
//     phantom-seat shape -- so each is a spelling and nothing more:
//       0x02018144 Deallocate            0x0201816c LoadFile
//       0x0203c280 Heap::_Deallocate     0x0203c28c Heap::Allocate
//       0x02055574 G3X::SetClearColor
#pragma comment(linker, "/alternatename:_func_02018144=_Deallocate")
#pragma comment(linker, "/alternatename:_func_0201816c=_LoadFile")
#pragma comment(linker, "/alternatename:_func_0203c280=__ZN4Heap11_DeallocateEPv")
#pragma comment(linker, "/alternatename:_func_0203c28c=__ZN4Heap8AllocateEj")
#pragma comment(linker, "/alternatename:_func_02055574=__ZN3G3X13SetClearColorEtiiib")
//     AND A SIXTH, ADDED BY THE DUALSCREEN LANE AND RETIRED OUT OF THE
//     UNMATCHED-TRAP CENSUS ABOVE: 0x02054c80 is `Vram__Map`, the function
//     that puts each bank named in a mask back into LCDC mode by writing 0x80
//     to its VRAMCNT byte. src/Vram__Map.c is matched and slice_gate10.txt
//     links it; the port had a trap on the address instead, so func_02054430 --
//     the ONE caller, `*p |= arg; return func_02054c80(arg, p)` -- returned 0
//     and no bank ever moved. The two spellings disagree about arity: the
//     caller's declaration is `(int, unsigned short *)` and the body is
//     `(int)`. That is a difference of one SPURIOUS cdecl argument and nothing
//     else -- the ROM passes the mask in r0 for both, the caller pushes the
//     extra word and pops it itself, and both call sites in the D3D family
//     (func_ov006_020e7508 and _020e759c) ignore the return.
#pragma comment(linker, "/alternatename:_func_02054c80=_Vram__Map")
//
// (c) THE _ZN6Player17St_EndingFly_MainEv NAMING TRAP, which the 2d map warns
//     about in its section 1 and which this slice is the first build to hit.
//     0x020c3d1c is an ov007 function and the community label on it is a
//     Player state name; ov007 is the title scene and has no Player. The
//     matched TU is src/_ZN6Player17St_EndingFly_MainEv.cpp, it is in this
//     slice, and it defines the flat Itanium name. Its callers spell the same
//     body FOUR ways between them -- once by address and three times as a
//     C++ method, with three different MSVC manglings because the three
//     declaring TUs disagree on the return type and on staticness.
//
//     THREE OF THOSE FOUR DIRECTIVES WERE RECEIVER DEFECTS AND ARE GONE. They
//     are defects 4, 5 and 6 of port/abi_checks.txt section 6; the faces that
//     replace them are further down this file and carry the evidence,
//     including what the ROM actually dispatches through
//     data_ov007_02103254. Only the by-address spelling stays an alias: both
//     sides are flat __cdecl names, so it is a NAME bridge and nothing else,
//     which is the only thing an /alternatename may ever be.
//
//     AND THE BY-ADDRESS SPELLING HAS A DEFECT OF ITS OWN THAT IS NOT FIXED
//     HERE, stated so the deletion of the other three cannot be read as the
//     class being closed. SIXTEEN matched ov007 TUs are `ldr ip,[pc]; bx ip;
//     .word 0x020c3d1c` long-branch veneers that declare
//     `extern int func_020c3d1c(void);` and call it with nothing:
//       func_ov007_020c0a9c _020c105c _020c1620 _020c2018 _020c22f8 _020c2410
//       _020c338c _020c3544 _020c8098 _020c8440 _020c8b04 _020c8f58
//       _020c9080 _020c92a0 _020c937c _020c9460
//     (RF1 filed this as five, which was the subset it had grepped; RFR1's
//     review took the census and it is sixteen. A seventeenth,
//     func_ov007_020c05f8, is the same shape spelling the flat name instead.)
//     On ARM r0 rides through the `bx`; on the host the ride survives only
//     because MSVC compiles each one as a jmp, and SIX of the sixteen are
//     entered with a real pushed argument, not a bare call --
//     src/func_ov007_020cbb04.cpp:18 into _020c8098, and
//     src/func_ov007_020b9770.cpp into _020c937c four times and _020c9080
//     twice. NOTHING IN THE SUITE CAN SEE ANY OF IT: both sides of this
//     directive are flat cdecl so aliascheck's rule R is right to stay quiet,
//     and nothing defines func_020c3d1c so aritycheck has no census row to
//     hang the declarations on. It is a seventh instance of the section 6
//     class, found by lane RF1 while fixing the other six, and it is written
//     up in section 6 item 7 rather than fixed, because sixteen frames want
//     their own tail-jump rows and their own drive. The ride itself is
//     already asserted: tailjump_guard DERIVES all 22 ov007 veneers from the
//     overlay image, so the safety of the status quo does not rest on this
//     comment being complete.
#pragma comment(linker, "/alternatename:_func_020c3d1c=__ZN6Player17St_EndingFly_MainEv")
//     The int-returning spelling now rides the void one. Both sides are
//     public __thiscall taking no arguments, so the receiver AGREES and the
//     pop agrees; only the return type differs, and EAX is exactly as
//     indeterminate here as r0 is in the ROM (0x020c3d1c returns whatever the
//     pointer it dispatched left behind, and all seven int-form call sites in
//     src/func_ov007_020cbb04.cpp discard it). This is the void/int return
//     bridge hal/lk4_solidheap_seat.cpp takes for Heap::Rescue, and it exists
//     because two decorations that differ only in return type cannot both be
//     declared on one class in one TU.
#pragma comment(linker, "/alternatename:?St_EndingFly_Main@Player@@QAEHXZ=?St_EndingFly_Main@Player@@QAEXXZ")
//     The free-function spelling: src/func_ov007_020b7764.cpp is its only
//     referrer and the receiver it must pass is a GLOBAL, so the face below
//     can supply it exactly. Alias rather than a second face declaration for
//     the reason the Sound::Func_02048ec4 row in hal/actor_classes_ov073.cpp
//     gives: a `namespace Player` in this TU would collide with the `struct
//     Player` the QAEX face needs. Both sides are __cdecl with no receiver,
//     so this is not a crossing.
#pragma comment(linker, "/alternatename:?St_EndingFly_Main@Player@@YAXXZ=_port_ov007_b7764_endingfly")
//
// (d) SIX C++-DECLARED CALLS ONTO FLAT DEFINITIONS. The cxxname_bridge defect
//     in its usual direction: an ov007 TU declares the callee inside a struct
//     or namespace, so MSVC mangles the reference, while the matched TU
//     defines the flat Itanium name.
#pragma comment(linker, "/alternatename:?DispOn@GX@@SAXXZ=__ZN2GX6DispOnEv")
#pragma comment(linker, "/alternatename:?DisableAllBanks@GX@@SAXXZ=__ZN2GX15DisableAllBanksEv")
#pragma comment(linker, "/alternatename:?LoadOBJ@GX@@SAXPBXII@Z=__ZN2GX7LoadOBJEPKvjj")
#pragma comment(linker, "/alternatename:?SetBankForTexPltt@GX@@YAXG@Z=__ZN2GX17SetBankForTexPlttEt")
#pragma comment(linker, "/alternatename:?div@cstd@@YAHHH@Z=__ZN4cstd3divEii")
//     Scene::SetFaders IS NOT IN THAT LIST AND MUST NOT BE, and the reason is
//     worth the paragraph because it cost this lane a fault that read like a
//     vtable bug. `?SetFaders@Scene@@QAEXPAUFaderBrightness@@@Z` is __THISCALL
//     -- `this` in ECX, the FaderBrightness* on the stack -- and the matched
//     TU defines a __CDECL function taking ONE stack argument. An
//     /alternatename between the two LINKS, and then the callee reads the
//     stack argument as `this`: dScDSMT_c::InitResources calls it with
//     this = self+0x54 and fb = self+0x50, so the body ran against self+0x50,
//     whose vptr is the FOUR-slot graphCallback_c table, and dispatched slot 9
//     of it -- eight words past the end, into the RTTI name string that starts
//     at 0x021032c0. The fault was eip 0x61226c6c reading 0x61626c6c, which is
//     the bytes "llba" out of "graphCallback_c". This is
//     hal/scene_actor_faces.cpp's veneer trap in a new dress: an alias cannot
//     change a calling convention, so where the conventions differ the answer
//     is a FACE that re-lands the arguments.
struct FaderBrightness;
struct Scene { void SetFaders(FaderBrightness *fb); };
void Scene::SetFaders(FaderBrightness *)
{
    /* the ROM body ignores its second argument; only `this` is used */
    _ZN5Scene9SetFadersEP15FaderBrightness(this);
}
//     ...and THREE IN THE OPPOSITE DIRECTION. Here the ov007 caller spells the
//     FLAT Itanium name and the matched TU defines a real C++ static member,
//     so the alias runs flat -> mangled. The decorations are read out of the
//     compiled .obj with dumpbin rather than hand-derived, because a wrong
//     decoration here is a silent no-op alias. THIS WAS FOUR UNTIL run link60
//     Stage 5 lane MR1, and the three that remain all target an SA (static,
//     __cdecl) member, so both sides are __cdecl and each is a name-only
//     bridge. The fourth was not, and it is the face below.
#pragma comment(linker, "/alternatename:__ZN9Animation17UpdateFileOffsetsER8BCA_File=?UpdateFileOffsets@Animation@@SAXAAUBCA_File@@@Z")
#pragma comment(linker, "/alternatename:__ZN15TextureSequence17UpdateFileOffsetsER8BTP_File=?UpdateFileOffsets@TextureSequence@@SAXAAUBTP_File@@@Z")
#pragma comment(linker, "/alternatename:__ZN5Model13LoadTexAndPalER8BMD_File=?LoadTexAndPal@Model@@SAXAAUBMD_File@@@Z")
//
//     ModelComponents::Render USED TO BE THE FOURTH LINE OF THIS BLOCK AND IT
//     WAS WRONG, in the way the Scene::SetFaders paragraph at the head of (d)
//     already says an alias is always wrong when the conventions differ. The
//     directive was
//
//         /alternatename:__ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3=?Render@ModelComponents@@QAEXPAUMatrix4x3@@PAUVector3@@@Z
//
//     and QAE is __thiscall. src/func_ov007_020bbff0.c declares the flat name
//     as three void * and calls it
//
//         _ZN15...RenderE...(*(void**)c, (char*)c+0x54, (char*)c+0x40)
//
//     which is three __cdecl pushes, so the receiver was never delivered: the
//     callee took `this` out of ECX, where the caller had left its OWN object
//     (one dereference short of *(void**)c), and then read mat out of the
//     stack slot holding the real this and vec out of the slot holding mat.
//     Every one of the three was wrong at once. Scene 1's Render slot is where
//     that fault landed; port/ov007_seat.txt section 5e records the fault, 5f
//     records this fix and the run that followed it, and 7 records the delta.
//
//     THE FACE BELOW IS THE FIX, in the shape Scene::SetFaders above uses and
//     hal/lk4_solidheap_seat.cpp's Heap::SetDefault uses running the other
//     way: a real __cdecl definition of the flat name that takes the receiver
//     as its first argument and makes the member call itself, so MSVC emits
//     the `mov ecx` the ROM's r0 always was.
//
//     THE DECLARATION IS LOCAL AND DELIBERATELY SO. It has to reproduce
//     ?Render@ModelComponents@@QAEXPAUMatrix4x3@@PAUVector3@@@Z exactly, which
//     needs Matrix4x3 and Vector3 to mangle as PAU (struct, not class) and
//     Render to be a public non-const non-static member. Neither type is
//     visible in this TU (it includes no game headers at all, only <cstdio>,
//     the ntr/ ones and dsstate_seg.h), so two forward declarations give the
//     right mangling with no risk of a header changing it later. The
//     decoration was checked against the compiled .obj with dumpbin after the
//     face landed, not derived by hand, which is the same rule the block above
//     states.
//
//     WHERE IT LIVES, AND WHEN TO MOVE IT. This file and the only compiled
//     flat-name caller, src/func_ov007_020bbff0.c, are in exactly the same
//     three targets (smoke_player, walk_window, walk_window_hires), so the
//     face resolves the name wherever the name is referenced and nowhere else.
//     There is a SECOND flat-name caller in the tree that no lane had named,
//     src/engine/fader/_ZN9FaderWipe11AdvanceFadeEv.cpp:12 and :34, and it is
//     in no slice today. The day a lane slices it into a target that does not
//     compile this file, the link fails with LNK2019 on this symbol and the
//     fix is to move the TEN lines below (the three declarations and the
//     seven-line definition) to
//     port/unmatched/ModelComponents_Render.cpp, which is in fourteen targets.
//     That is LK5's rule (a face belongs in a TU whose target set covers its
//     consumers) stated in advance instead of after the break.
struct Matrix4x3;
struct Vector3;
struct ModelComponents { void Render(Matrix4x3 *mat, Vector3 *vec); };
extern "C" void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(void *thiz,
                                                                 void *mat,
                                                                 void *vec)
{
    ((ModelComponents *)thiz)->ModelComponents::Render((Matrix4x3 *)mat,
                                                       (Vector3 *)vec);
}
//     ...and TWO MORE OF THE SAME SHAPE in the SaveData family, faced by run
//     link60 Stage 5 lane SV1 in the commit that made them fire. Their TUs are
//     in the slice and define real C++ members while their ov007 callers spell
//     the flat name, which is the same collision the Render row above had.
//
//     WHY THEY WERE STILL DIRECTIVES WHEN THIS BLOCK'S OWN ARGUMENT SAID THEY
//     SHOULD NOT BE. MR1 traced both, found the flat-name calls never happened
//     and declined to face a call nobody makes. They were unfired for exactly
//     one reason: the cart-read face further up this file returned the value
//     that skips the SetDefaultValues arm. Correcting that value starts the
//     calls, so the directives had to go in the same change.
//
//     THE RECEIVER IS THE BUFFER, which is what makes these two different from
//     the Render row and is the thing to get right. Both matched bodies write
//     through `this` and never read the parameter the mangled name declares:
//     src/_ZN8SaveData16SetDefaultValuesEP12FileSaveData.cpp clears 0x44 bytes
//     at `this` and stamps the file magic there, and the Mg sibling does the
//     same over 0x2e4. include/SaveData.h's three evidenced fields (0x008,
//     0x041, 0x042) all sit inside 0x44, so SaveData and FileSaveData are one
//     0x44 block under two names and the flat call's single pointer is both
//     the receiver and the argument. The faces pass it as both, and the second
//     hand-off is inert today because the parameter is dead in both bodies.
//
//     MEASURED OFF THE OBJECTS IN THIS TREE, NOT INHERITED. dumpbin /DISASM of
//     walk_window.dir/.../_ZN8SaveData16SetDefaultValuesEP12FileSaveData.cpp.obj
//     opens `push esi / push 44h / mov esi,ecx / push 0 / push esi / call
//     _func_0205a588` and closes `ret 4`; the Mg object is the same with 2E4h.
//     `mov esi,ecx` is the receiver coming out of ECX and `ret 4` is the
//     CALLEE popping the one stack argument.
//
//     AND THE CALLER SIDE MAKES IT WORSE THAN A DROPPED RECEIVER, which no
//     lane had noticed. src/_ZN8SaveData12ReadFileDataEjP12FileSaveData.cpp's
//     object reads `push esi / call __ZN8SaveData16SetDefaultValuesE... /
//     add esp,4`: a cdecl caller cleaning up an argument the __thiscall callee
//     has already popped. Both sides take the same four bytes, so ESP returns
//     four high, and that function's epilogue is `pop edi / pop esi / pop ebp
//     / ret` off an EBP frame. Each pop reads one slot along, `pop ebp` takes
//     the return address, and `ret` jumps to the first argument, a file index
//     of 0, 1 or 2. The directive was not a wrong `this`, it was a wrong
//     `this` AND a return into a small integer. Neither had ever run.
//
//     The declarations are local to this TU for the same reason the Render
//     row's are: they have to reproduce ?SetDefaultValues@SaveData@@QAEXPAU-
//     FileSaveData@@@Z and its Mg sibling exactly, both PAU and both public
//     non-const non-static members, and no game header is visible here.
//     COVERAGE IS THE RENDER FACE'S, by LK5's rule: this file and
//     ${SLICE_OV007_SOURCES}, which carries all three compiled flat-name
//     callers (ReadFileData, ReadMinigameData, EraseSaveFile), are listed in
//     the same three targets and in no others. Restoring either directive on
//     top of these faces does not silently win, it defeats the alias, and
//     port/tools/alternatename_guard.py refuses a defeated pair that is not in
//     its baseline. That is the signal, and it needs no marker of its own.
struct FileSaveData;
struct MinigameSaveData;
struct SaveData {
    void SetDefaultValues(FileSaveData *fsd);
    void SetDefaultValuesMg(MinigameSaveData *mg);
};
extern "C" void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(void *blk)
{
    ((SaveData *)blk)->SaveData::SetDefaultValues((FileSaveData *)blk);
}
extern "C" void _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData(void *blk)
{
    ((SaveData *)blk)->SaveData::SetDefaultValuesMg((MinigameSaveData *)blk);
}
//     THE THIRD OF THE FAMILY STAYS A DIRECTIVE AND THAT IS NOT AN OVERSIGHT.
//     SaveData::SaveFile decorates SA, not QAE: a STATIC member, which MSVC
//     emits __cdecl with no receiver at all, so the flat name's two pushes land
//     on the two parameters and nothing is dropped. The shape this block is
//     about needs a __thiscall right-hand side and this row has not got one.
#pragma comment(linker, "/alternatename:__ZN8SaveData8SaveFileEjP12FileSaveData=?SaveFile@SaveData@@SAHIPAUFileSaveData@@@Z")
//
// (e) ELEVEN MANGLED DATA SPELLINGS onto storage that already exists. Three of
//     them are the SAME symbol, data_ov007_0210342c, mangled three ways
//     because three TUs declare it with three different types. None of the
//     eleven is a stand-in: every right-hand side is either the ov007 mount's
//     own byte array or an arm9 global the build already defines.
#pragma comment(linker, "/alternatename:?data_02082214@@3QBFB=_data_02082214")
#pragma comment(linker, "/alternatename:?data_0209d4a8@@3HA=_data_0209d4a8")
#pragma comment(linker, "/alternatename:?data_0209f1e0@@3EA=_data_0209f1e0")
#pragma comment(linker, "/alternatename:?data_ov007_02102f28@@3PADA=_data_ov007_02102f28")
#pragma comment(linker, "/alternatename:?data_ov007_0210342c@@3PADA=_data_ov007_0210342c")
#pragma comment(linker, "/alternatename:?data_ov007_0210342c@@3PAHA=_data_ov007_0210342c")
#pragma comment(linker, "/alternatename:?data_ov007_0210342c@@3PAUS@@A=_data_ov007_0210342c")
#pragma comment(linker, "/alternatename:?data_ov007_02104b9c@@3PAHA=_data_ov007_02104b9c")
#pragma comment(linker, "/alternatename:?data_ov007_02104b9c@@3PAUB9C@@A=_data_ov007_02104b9c")
#pragma comment(linker, "/alternatename:?data_ov007_02104ba0@@3PAHA=_data_ov007_02104ba0")
#pragma comment(linker, "/alternatename:?data_ov007_02104ba0@@3PAUBA0@@A=_data_ov007_02104ba0")
#pragma comment(linker, "/alternatename:?data_ov007_02104bc0@@3PAHA=_data_ov007_02104bc0")
#pragma comment(linker, "/alternatename:?data_ov007_02104bd8@@3PAXA=_data_ov007_02104bd8")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3HA=_data_0209caa0")

/* The matched ActorBase methods for slots 13 and 14, and the reason this is a
   LOCAL declaration rather than include/ActorBase.h.
   MSVC encodes virtualness in the mangled name, and the TUs do not agree on
   it: src/_ZN9ActorBase9Virtual34Ejj.cpp and its Virtual38 sibling declare
   their method NON-virtual in a local struct (so the definitions are
   ?Virtual34@ActorBase@@QAEHII@Z, which is what hal/lk4_solidheap_seat.cpp
   already links against), while src/_ZN9ActorBase13OnHeapCreatedEv.cpp
   includes ActorBase.h and defines the VIRTUAL ?OnHeapCreated@ActorBase@@UAE_NXZ
   that hal/actor_vtables.cpp already links against. Only
   the two Virtual3x are the ones this file needs, so this file declares them
   non-virtual and hal/scene_actor_faces.cpp -- which includes the real header
   -- carries everything that has to be a virtual method. */
struct ActorBase {
    int Virtual34(unsigned a, unsigned b);           /* slot 13 body */
    int Virtual38(unsigned a, unsigned b);           /* slot 14 body */
};

/* ==== THE TWO RECEIVER-BRIDGING FACES FOR "Player::St_EndingFly_Main" =====

   port/abi_checks.txt section 6, defects 4/5 (the two __thiscall spellings)
   and 6 (the free-function spelling aritycheck had to learn a new declaration
   shape to see at all). The three deleted directives were

     ?St_EndingFly_Main@Player@@QAEHXZ = __ZN6Player17St_EndingFly_MainEv
     ?St_EndingFly_Main@Player@@QAEXXZ = __ZN6Player17St_EndingFly_MainEv
     ?St_EndingFly_Main@Player@@YAXXZ  = __ZN6Player17St_EndingFly_MainEv

   WHAT THE BODY ACTUALLY IS, derived from the ROM rather than from the name,
   because the name is wrong and the fix depends on the answer. 0x020c3d1c is

     stmdb sp!,{lr} / sub sp,#4
     ldr r1,[pc,#0x10] / ldr r1,[r1]    <- r1 = *data_ov007_02103254
     blx r1                             <- r0 UNTOUCHED, rides into the callee
     add sp,#4 / ldm sp!,{lr} / bx lr

   and config/arm9/overlays/ov007/relocs.txt:3264 says what that word holds:

     from:0x02103254 kind:load to:0x020c3e4c module:overlay(7)

   0x020c3e4c is six instructions and four of them are the argument setup, so
   all six are worth carrying:

     020c3e4c  ldr ip,[pc,#0xc]     ip = 0x020590fc
     020c3e50  mov r2,r0            arg 3 = the object
     020c3e54  mov r0,#0            arg 1 = 0, the table index
     020c3e58  mvn r1,#0            arg 2 = -1, "the current handle"
     020c3e5c  bx  ip
     020c3e60  .word 0x020590fc

   func_020590fc is free(). Under an interrupt lock it indexes a 12-byte
   record by the current handle and calls two list functions on the node at
   (obj - 0x20): func_02059364 UNLINKS it from the allocated list at rec+8,
   and func_0205929c INSERTS it into the address-sorted FREE list at rec+4,
   reading node[+8] as a size to coalesce with the neighbour above. So the
   body is a TEARDOWN TRAMPOLINE whose one argument is the object being freed,
   and the Player name on it is a mislabel -- the 2d map's section 1 says so
   and this is the disassembly behind that warning. The callers agree:
   src/func_ov007_020cbb04.cpp calls it on five sub-objects and then on the
   parent, and src/func_ov007_020b9770.cpp calls it on two globals and nulls
   each one immediately after.

   THE TRUE BODY IS SEATED AND MATCHED, so these are BRIDGES and not traps.
   src/_ZN6Player17St_EndingFly_MainEv.cpp is in this slice and defines the
   flat name as `void f(void *self)`. Nothing here is unseated, so a loud trap
   would be refusing to run a path the port already has the code for.

   WHAT THE DIRECTIVES DID. QAE is __thiscall: the nine call sites put the
   object in ECX and push NOTHING, and the flat cdecl body then read its first
   stack slot -- the RETURN ADDRESS -- and handed that to free(). YA is a
   free function: src/func_ov007_020b7764.cpp:9 spells
   `Player::St_EndingFly_Main()` with no arguments at all, so that site read
   the same return address. Either way the caller's own return address is
   INSERTED INTO THE FREE LIST as a block header at (return address - 0x20),
   with its size field read out of whatever instruction sits eight bytes into
   that header, and the coalescing test then compares that against the next
   free block. It has never fired because ov007's ending path is not drivable,
   which is why these two faces are proved by the checker and the disassembly
   and claim no drive.

   NEITHER FACE IS A SHADOW: all three directives are DELETED. */
struct Player {
    /* the void spelling, which is what src/func_ov007_020b9770.cpp declares
       and what the flat body's own return type says. The int spelling is
       bridged onto this one by the alias in block (c) above. */
    void St_EndingFly_Main();
};

extern "C" {
/* the matched flat body, and the ov007 global whose VALUE is the object
   src/func_ov007_020b7764.cpp's call site is torn down. The ov007 mount
   defines it as `u8 data_ov007_02103448[4]`; the matched TU reads it as an
   int, and so does this, because extern "C" data carries no type in the
   symbol and the int reading is the one the ROM's `ldr r0,[r0]` performs. */
void _ZN6Player17St_EndingFly_MainEv(void *self);
extern int data_ov007_02103448;

/* the RHS of the YA alias in block (c). A free __cdecl function taking
   nothing, exactly like the declaration it stands behind -- the receiver is
   not passed to it and never was, so the face reads it from the same global
   the ROM reads it from. */
void port_ov007_b7764_endingfly(void)
{ _ZN6Player17St_EndingFly_MainEv((void *)(size_t)data_ov007_02103448); }
}

void Player::St_EndingFly_Main()
{ _ZN6Player17St_EndingFly_MainEv(this); }

// ---- the shared eleven -----------------------------------------------------
static int  __fastcall sc_binit(void *s, void *)
{ return _ZN5Scene19BeforeInitResourcesEv(s); }
static void __fastcall sc_ainit(void *s, void *, unsigned a)
{ port_scene_after_init(s, a); }
static int  __fastcall sc_bclean(void *s, void *)
{ return _ZN5Scene22BeforeCleanupResourcesEv(s); }
static void __fastcall sc_aclean(void *s, void *, unsigned a)
{ _ZN5Scene21AfterCleanupResourcesEj(s, a); }
static int  __fastcall sc_bbeh(void *s, void *)
{ return _ZN5Scene14BeforeBehaviorEv(s); }
static void __fastcall sc_abeh(void *s, void *, unsigned a)
{ port_scene_after_behavior(s, a); }
static int  __fastcall sc_bren(void *s, void *)
{ return _ZN5Scene12BeforeRenderEv(s); }
static void __fastcall sc_aren(void *s, void *, unsigned a)
{ port_scene_after_render(s, a); }
static int  __fastcall sc_v34(void *s, void *, unsigned a, unsigned b)
{ return ((ActorBase *)s)->ActorBase::Virtual34(a, b); }
static int  __fastcall sc_v38(void *s, void *, unsigned a, unsigned b)
{ return ((ActorBase *)s)->ActorBase::Virtual38(a, b); }
static int  __fastcall sc_heap(void *s, void *)
{ return port_scene_on_heap_created(s); }
/* slots 0 and 3, which only a minigame table holds unoverridden. Not veneers. */
static int  __fastcall sc_base_init(void *s, void *)
{ return port_scene_base_init(s); }
static int  __fastcall sc_base_clean(void *s, void *)
{ return port_scene_base_cleanup(s); }

/* THE 18-SLOT SHAPE IS HARDCODED HERE, AND IT IS AN ov003 FINDING RATHER THAN
   A LAW ABOUT SCENES. All three ov003 classes were read out of the overlay
   image and all three are exactly _ZTV5Scene's eighteen slots with seven
   overridden, so this writes eleven fixed indices.
   ---- THE CAVEAT FIRED, AND A WIDTH PARAMETER IS NOT THE ANSWER -------------
   The paragraph that used to end here asked whoever seats the first ov006
   scene to read that class's table out of the image rather than assume this
   shape, and predicted that a deeper class would have "a LONGER table whose
   tail this function would leave unwritten while its indices 1..15 still land
   correctly". Lane MG1 read it. HALF OF THAT PREDICTION IS WRONG and the
   wrong half is the dangerous one.
   dScMgBase_c's own table (data_ov004_020bc0c0) is THIRTY-SIX slots and every
   minigame class's is too -- vtspan's three routes agree on 36 for both the
   base and MgShuffleShell's data_ov006_0213c304. The tail is indeed
   unwritten, which is the harmless half. The other half is that indices
   1..15 DO NOT still land correctly: dScMgBase_c overrides five of the
   eleven this function writes, so on a minigame table
       slot 1  is func_ov004_020b0930, not Scene::BeforeInitResources
       slot 2  is func_ov004_020b08f0, not Scene::AfterInitResources
       slot 5  is func_ov004_020b0840, not Scene::AfterCleanupResources
       slot 7  is func_ov004_020b0620, not Scene::BeforeBehavior
       slot 10 is func_ov004_020b04f4, not Scene::BeforeRender
   and calling this function on one would silently replace five of the
   framework's own overrides with the base bodies they exist to displace. A
   WIDTH PARAMETER WOULD NOT CATCH ANY OF THAT: the bug is the index list, not
   the length. So the generic form is scene_fill_rom() below, which keys on
   the ROM WORD the slot actually holds instead of on the slot number, and
   therefore cannot write a slot the ROM did not park a shared body in. It
   works at any width.
   THIS FUNCTION STAYS as it is, for ov003 and ov007 only, and not out of
   caution: ov003's table is a FRESH HOST ARRAY (port/ov003_syms.txt leaves
   data_ov003_020b1704 out of the mount) so it is zeroed at fill time and
   there is no ROM word in it to key on. An index fill is the only thing that
   can work there. ov007's table IS mounted and could move, and is left alone
   because its 18-slot shape is proven three ways and moving a working seat is
   not this lane's risk to take. */
static void scene_fill_shared(void **vt)
{
    vt[1]  = (void *)sc_binit;
    vt[2]  = (void *)sc_ainit;
    vt[4]  = (void *)sc_bclean;
    vt[5]  = (void *)sc_aclean;
    vt[7]  = (void *)sc_bbeh;
    vt[8]  = (void *)sc_abeh;
    vt[10] = (void *)sc_bren;
    vt[11] = (void *)sc_aren;
    vt[13] = (void *)sc_v34;
    vt[14] = (void *)sc_v38;
    vt[15] = (void *)sc_heap;
}

// ---- the generic fill: keyed on the ROM word, not on the slot -------------
//
// For a scene class whose vtable is inside a MOUNTED span, so the raw DS words
// are still there to read at fill time. Every word the port has a shared host
// body for is replaced; every word it does not is LEFT ALONE for the class's
// own list to write, and counted, so a class that leaves a live DS address in
// a dispatched slot says so instead of jumping to a DS address as a host one.
//
// The table is the ROM's own addresses. Two of them are the 0xc-byte tail-call
// VENEERS the header block of hal/scene_actor_faces.cpp derives, and they get
// the same treatment the index fill gives them: dispatch straight to the
// veneer's target with both arguments, which is what the three ROM
// instructions mean. Scene::AfterInitResources (0x0202e62c), the third veneer,
// is in the table too even though no minigame class holds it -- dScMgBase_c
// overrides slot 2 -- because the table is a statement about what the port can
// host, not about one class.
struct SceneRomFace { unsigned ds; void *host; };
static const SceneRomFace kSceneRomFaces[] = {
    /* the eleven the index fill writes, by the address each really holds */
    {0x0202e638u, (void *)sc_binit},   /* Scene::BeforeInitResources     */
    {0x0202e62cu, (void *)sc_ainit},   /* Scene::AfterInitResources  VENEER */
    {0x0202e5f0u, (void *)sc_bclean},  /* Scene::BeforeCleanupResources  */
    {0x0202e5d0u, (void *)sc_aclean},  /* Scene::AfterCleanupResources   */
    {0x0202e3d4u, (void *)sc_bbeh},    /* Scene::BeforeBehavior          */
    {0x0202e3c8u, (void *)sc_abeh},    /* Scene::AfterBehavior       VENEER */
    {0x0202e3a4u, (void *)sc_bren},    /* Scene::BeforeRender            */
    {0x0202e398u, (void *)sc_aren},    /* Scene::AfterRender         VENEER */
    {0x0204357cu, (void *)sc_v34},     /* ActorBase::Virtual34           */
    {0x0204349cu, (void *)sc_v38},     /* ActorBase::Virtual38           */
    {0x02043494u, (void *)sc_heap},    /* ActorBase::OnHeapCreated       */
    /* the two the minigame classes are the first to need: dScMgBase_c does
       not override slot 0 or slot 3, and ov003's and ov007's classes both
       override both, so no earlier fill ever saw these words. */
    {0x02043c80u, (void *)sc_base_init},
    {0x02043bf0u, (void *)sc_base_clean},
};

/* Returns the number of slots left holding a raw DS word, so the caller can
   assert that its own per-class list accounts for every one of them. A scene
   table with a raw word left in it is a wild call waiting to happen.
   NO CALLER TODAY. This is the generic fill the eighteen-slot caveat above
   asked for, landed with the measurement that answers the caveat and ahead of
   the seat that needs it: hal/scene_mg.cpp calls it, and hal/scene_mg.cpp is
   in the tree and not wired, for the reason port/mg_fanout_costs.txt section 8
   gives. /OPT:REF drops this function until that changes. */
extern "C" unsigned port_scene_fill_rom(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        const SceneRomFace *hit = 0;
        for (unsigned k = 0; k < sizeof kSceneRomFaces / sizeof kSceneRomFaces[0];
             ++k)
            if (kSceneRomFaces[k].ds == ds) { hit = &kSceneRomFaces[k]; break; }
        if (hit)
            vt[i] = hit->host;
        else
            ++left;
    }
    return left;
}

// ---- dScStarSel_c, id 4 ------------------------------------------------
//
// THE TICK WITNESS. One counter per dispatched slot, printed at the end of the
// run. A scene that "boots" is not a scene that RUNS, and the difference is
// invisible from the outside: the object exists either way. These say how many
// times the ROM's own processing lists actually entered ov003 code, which is
// the only evidence that the chain is live rather than merely linked.
static unsigned g_ss_hits[18];
static int  __fastcall ss_init(void *s, void *)
{ ++g_ss_hits[0];  func_ov003_020af8a0(s); return 1; }
static int  __fastcall ss_clean(void *, void *)
{ ++g_ss_hits[3];  return func_ov003_020af86c(); }
static int  __fastcall ss_beh(void *s, void *)
{ ++g_ss_hits[6];  return func_ov003_020af038(s); }
static int  __fastcall ss_render(void *s, void *)
{ ++g_ss_hits[9];  return func_ov003_020ae6f4(s); }
static int  __fastcall ss_pdes(void *, void *)
{ ++g_ss_hits[12]; func_ov003_020ae6f0(); return 0; }
/* the SM64DS_SCENE_SLOT9=0 stand-in. Counted separately so a run can never
   read its no-op as the real Render having run. */
static unsigned g_ss_render_skipped;
static int  __fastcall ss_render_noop(void *, void *)
{ ++g_ss_render_skipped; return 1; }
static void *__fastcall ss_d2(void *s, void *)
{ return (void *)(size_t)func_ov003_020addfc(s); }
static void *__fastcall ss_d0(void *s, void *)   { return func_ov003_020ade54(s); }

static void scene_fill_starsel(void)
{
    void **vt = data_ov003_020b1704;
    scene_fill_shared(vt);
    vt[0]  = (void *)ss_init;
    vt[3]  = (void *)ss_clean;
    vt[6]  = (void *)ss_beh;
    /* SM64DS_SCENE_SLOT9=0 leaves slot 9 on a no-op. The A/B that separates
       "the scene's own Render hangs or faults" from "the port's render bucket
       walks its list wrong", which nothing else in the port can tell apart:
       both show up as the frame never finishing. The default is the real body;
       this is a diagnostic, not a fallback, and the battery never sets it. */
    {
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        vt[9] = (s9 && s9[0] == '0') ? (void *)ss_render_noop
                                     : (void *)ss_render;
    }
    vt[12] = (void *)ss_pdes;
    vt[16] = (void *)ss_d2;
    vt[17] = (void *)ss_d0;
}

// ---- dScDSMT_c, id 1, the TITLE SCREEN and FILE SELECT (ov007) -------------
//
// THE WIDTH IS 18 AND THE ROM SAYS SO THREE WAYS, so scene_fill_shared's
// hardcoded shape above is left alone rather than parameterised: a width
// parameter with one caller and one value would be ceremony, and the next
// scene to disagree is the one that should add it.
//
//   1. the reloc run. config/arm9/overlays/ov007/relocs.txt has eighteen
//      CONSECUTIVE relocated words at 0x021032e8 .. 0x0210332c and then stops.
//      0x02103330, 0x02103334, 0x02103338 and 0x0210333c carry no relocation.
//   2. the ROM bytes. The mount emits data_ov007_021032e8 as 88 bytes -- the
//      delta to the next dsd symbol, which is the .bss base 0x02103340 -- and
//      words 18..21 of those 22 are 0x00000000 in the image. Trailing zeros,
//      not slots.
//   3. the shape. Every one of the eleven non-overridden slots holds the SAME
//      arm9 address ov003's three classes hold, byte for byte: 0x0202e638,
//      0x0202e62c, 0x0202e5f0, 0x0202e5d0, 0x0202e3d4, 0x0202e3c8, 0x0202e3a4,
//      0x0202e398, 0x0204357c, 0x0204349c, 0x02043494. dScDSMT_c is _ZTV5Scene
//      with seven slots overridden and adds no virtual of its own.
//
// THE FOUR TRAILING WORDS ARE LEFT AS THE ROM HAS THEM. The fill writes 0..17
// and does not touch 18..21, so a dispatch past the end reads the ROM's zeros
// and faults at 0 rather than running whatever the linker put next -- the
// failure vtspan.py exists to prevent, in its loud direction.
//
// THE SECOND TABLE. The factory writes a sub-object vptr at +0x50 as well
// (`p[0x50/4] = data_0208ee14; p[0x50/4] = data_ov007_021032b0`), and
// data_ov007_021032b0 is dScDSMT_c::graphCallback_c's own four-slot table --
// the RTTI name N9dScDSMT_c15graphCallback_cE at 0x021032c0, reached through
// the typeinfo record at 0x02103284 that sits one word below the table. It is
// in the mount too and it gets the same treatment. Slots 1 and 3 are matched
// arm9 Scene methods; the eleven-slot shared fill does not apply to it.
static unsigned g_ti_hits[18];
#if PORT_OV007_RIDETHROUGH_UNSEATED
/* SCENE 1'S BLOCKER, ANNOUNCED BY THE FRAME THAT DISPATCHES INTO IT.
 *
 * port/tools/battery.py's scene-1 row keys on the string below, so this is
 * the row's marker and not decoration. It is here, in ti_init, because this
 * is the LAST port-owned frame on the path: everything from
 * func_ov007_020cc4c0 down to the fault is a matched TU and the port owns no
 * code in between to hang a tighter probe on.
 *
 * The blocker is an ARM register ride-through, derived in
 * port/ov007_seat.txt section 5b and guarded at configure time by the PC2
 * block in port/CMakeLists.txt, which reads the middle TU's own
 * one-parameter declaration rather than a flag somebody has to remember to
 * clear. Fix the seam and this compiles out.
 *
 * WHAT THIS MARKER DOES NOT PROVE, and battery.py's row repeats it: it is
 * armed on entering InitResources, not on reaching the seam, so a DIFFERENT
 * fault inside InitResources would still print it. The trap print it
 * replaces was tighter because the trap WAS the blocker. */
static void ti_ridethrough_notice(void)
{
    static int said;
    if (said) return;
    said = 1;
    std::printf("  [scene] SCENE 1 BLOCKED: the ov007 ride-through at "
                "func_ov007_020be980 drops i and a2\n");
    std::fflush(stdout);
    std::fprintf(stderr, "  [scene] SCENE 1 BLOCKED: the ov007 ride-through at "
                 "func_ov007_020be980 drops i and a2\n");
    std::fflush(stderr);
}
#endif
#if PORT_OV007_AE558_UNSEATED
/* SCENE 1'S BLOCKER AFTER THE RIDE-THROUGH SEAM WAS SEATED, run link60 lane
 * RT1. Same placement and the same caveat as the notice above, for the same
 * reason: ti_init is still the last port-owned frame on the path.
 *
 * The blocker moved one frame deeper and stayed in its family. It is an
 * IMPLICIT r0 ARGUMENT: src/func_ov007_020add3c.c declares its callee
 * `extern int func_ov007_020ae558(void)` and calls it with nothing, while
 * src/func_ov007_020ae558.c defines it as `(char *self)`. The ROM's caller
 * never writes r0 between its prologue and the branch, so its own incoming
 * argument is still there to be read. Both TUs are matched. On the host the
 * callee reads the caller's saved esi instead.
 *
 * Armed by the RT1 block in port/CMakeLists.txt, which reads that
 * no-argument declaration AND whether the TU carrying it is still in the ov007
 * slice, so seating the seam disarms this in the configure that notices.
 * Derivation, measurements and the unruled seat: port/ov007_seat.txt 5c. */
static void ti_ae558_notice(void)
{
    static int said;
    if (said) return;
    said = 1;
    std::printf("  [scene] SCENE 1 BLOCKED: func_ov007_020add3c calls "
                "func_ov007_020ae558 without its implicit r0 argument\n");
    std::fflush(stdout);
    std::fprintf(stderr, "  [scene] SCENE 1 BLOCKED: func_ov007_020add3c calls "
                 "func_ov007_020ae558 without its implicit r0 argument\n");
    std::fflush(stderr);
}
#endif
static int  __fastcall ti_init(void *s, void *)
{
    ++g_ti_hits[0];
#if PORT_OV007_RIDETHROUGH_UNSEATED
    ti_ridethrough_notice();
#endif
#if PORT_OV007_AE558_UNSEATED
    ti_ae558_notice();
#endif
    return func_ov007_020cc4c0((char *)s);
}
static int  __fastcall ti_clean(void *, void *)
{ ++g_ti_hits[3];  return func_ov007_020cc45c(); }
static int  __fastcall ti_beh(void *s, void *)
{ ++g_ti_hits[6];  return func_ov007_020cc2cc((char *)s); }
#if PORT_OV007_MCRENDER_UNSEATED
/* SCENE 1'S BLOCKER AFTER THE IMPLICIT-r0 SEAM WAS SEATED, run link60 lane
 * AE1, AND THE FIRST ONE THIS FILE CAN ANNOUNCE FROM THE RIGHT FRAME.
 *
 * The two notices above sit in ti_init and both had to admit they were armed
 * on ENTERING InitResources rather than on reaching their seam, because every
 * frame in between was a matched TU. THIS ONE IS TIGHTER, measurably: with the
 * ae558 seat in, InitResources runs once and returns, Behavior runs 299 clean
 * ticks, and the fault is inside the Render slot. ti_render IS the port-owned
 * frame that dispatches into the faulting path, so this marker is armed one
 * slot away from the defect instead of a whole lifecycle away. It is still not
 * armed AT the seam -- a different fault inside the Render slot would print it
 * -- but the window it covers is a slot, not a scene.
 *
 * THE BLOCKER IS A CALLING CONVENTION AND NOT AN ARITY, which makes it a new
 * class for this scene. src/func_ov007_020bbff0.c declares the flat Itanium
 * name _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3 as three void* and
 * calls it with three cdecl pushes; the definition it reaches through this
 * file's own /alternatename is a real C++ member that MSVC emits __thiscall,
 * so `this` is expected in ECX and the three pushes land one slot off. The
 * banner over that alias block already says an alias cannot change a calling
 * convention, and Scene::SetFaders a few lines above it is the FACE that
 * exists for exactly this reason.
 *
 * Armed by the AE1 block in port/CMakeLists.txt, which reads the alias and the
 * caller's slice membership rather than the caller's declaration, because the
 * expected fix here is a face and a face does not touch the declaration.
 * Derivation, both host listings and the two other aliases in this file that
 * share the shape: port/ov007_seat.txt section 5e. */
static void ti_mcrender_notice(void)
{
    static int said;
    if (said) return;
    said = 1;
    std::printf("  [scene] SCENE 1 BLOCKED: func_ov007_020bbff0 calls "
                "ModelComponents::Render cdecl through a __thiscall alias\n");
    std::fflush(stdout);
    std::fprintf(stderr, "  [scene] SCENE 1 BLOCKED: func_ov007_020bbff0 calls "
                 "ModelComponents::Render cdecl through a __thiscall alias\n");
    std::fflush(stderr);
}
#endif
static int  __fastcall ti_render(void *s, void *)
{
    ++g_ti_hits[9];
#if PORT_OV007_MCRENDER_UNSEATED
    ti_mcrender_notice();
#endif
    return func_ov007_020cc2b0(s);
}
static int  __fastcall ti_pdes(void *, void *)
{ ++g_ti_hits[12]; func_ov007_020cc2ac(); return 0; }
/* the SM64DS_SCENE_SLOT9=0 stand-in, counted separately for the same reason
   ss_render_noop is: a no-op must never read as the real body having run. */
static unsigned g_ti_render_skipped;
static int  __fastcall ti_render_noop(void *, void *)
{ ++g_ti_render_skipped; return 1; }
/* SM64DS_SCENE_SLOT0=0, the same diagnostic one slot up, and it exists for a
   blocker that is not the port's: dScDSMT_c::InitResources dies inside an
   UNMATCHED ROM function. Counted separately for the same reason -- a run can
   never read the no-op as InitResources having run -- and the battery's
   retire probe re-runs the bare scene every pass, so the moment
   func_ov007_020c9688 gets a body this stops being needed and says so. */
static unsigned g_ti_init_skipped;
static int  __fastcall ti_init_noop(void *, void *)
{ ++g_ti_init_skipped; return 1; }
static void *__fastcall ti_d2(void *s, void *) { return func_ov007_020cc028((int *)s); }
static void *__fastcall ti_d0(void *s, void *) { return func_ov007_020cc070((int *)s); }
/* graphCallback_c */
static int __fastcall ti_gc0(void *, void *)  { return func_ov007_020cc110(); }
static int __fastcall ti_gc1(void *s, void *) { return _ZN5Scene14GraphCallback1Ev(s); }
static int __fastcall ti_gc2(void *s, void *) { return func_ov007_020cc0f4(s); }
static int __fastcall ti_gc3(void *s, void *) { return _ZN5Scene14GraphCallback3Ev(s); }

/* The registry named in the block comment above. One entry today; an array
   because the next seated scene class adds a row rather than a special case,
   and a linear walk over four pointers is not worth a smarter shape. */
static void *g_gc_seated[4];
static unsigned g_gc_seated_n;
static int g_gc_verdict = 1;

void port_graph_block_register(void *vt)
{
    for (unsigned i = 0; i < g_gc_seated_n; ++i)
        if (g_gc_seated[i] == vt) return;
    if (g_gc_seated_n < sizeof g_gc_seated / sizeof g_gc_seated[0])
        g_gc_seated[g_gc_seated_n++] = vt;
}

/* 1 = run func_02019144's tail, which is what the port's two publish sites
   and its OAM::Load already do. 0 = the block handled the display sync
   itself and the tail must NOT also run. */
extern "C" int port_graph_block_beat(void)
{
    static int off = -1;
    if (off < 0) off = std::getenv("SM64DS_GRAPH_BLOCK_OFF") ? 1 : 0;
    g_gc_verdict = 1;
    if (off) return 1;
    /* hal/w8a_stage_storage.cpp hosts the pointer; four bytes there, and the
       ROM's own null test is the first thing func_02019144 does. */
    extern unsigned char data_0209d4a8[4];
    void *p = *(void **)data_0209d4a8;
    if (!p) return 1;
    void **vt = *(void ***)p;
    for (unsigned i = 0; i < g_gc_seated_n; ++i) {
        if (g_gc_seated[i] != (void *)vt) continue;
        typedef int(__fastcall * Slot2)(void *, void *);
        g_gc_verdict = ((Slot2)vt[2])(p, 0);
        return g_gc_verdict;
    }
    return 1;
}

/* What the beat answered THIS frame, for the second half of the tail. The
   engine A display path runs the beat; the engine B one reads the answer,
   because the block's slot 2 is called once per frame on the DS and calling
   it twice would run the scene's whole display sync twice. */
extern "C" int port_graph_block_verdict(void) { return g_gc_verdict; }

static void scene_fill_title(void)
{
    /* THE MOUNT COMES UP FIRST, and the order matters in one direction only.
       port_ov007_syms_patch() rebases the mount's own in-span pointer words;
       every word this fill writes is a CODE address, which is outside the
       mount's coverage, so the patch cannot undo the fill -- but the fill
       would be undone if it ran first and the patch happened to cover a slot,
       so the safe order is the one the ov043/ov045 mounts already use. */
    static int done;
    if (!done) {
        done = 1;
        port_l2_pack_check();
        l2_fill_0208ea6c();
        port_ov007_pack_check();
        port_ov007_syms_patch();
    }

    void **vt = (void **)data_ov007_021032e8;
    scene_fill_shared(vt);
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        vt[0] = (s0 && s0[0] == '0') ? (void *)ti_init_noop
                                     : (void *)ti_init;
    }
    vt[3]  = (void *)ti_clean;
    vt[6]  = (void *)ti_beh;
    {
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        vt[9] = (s9 && s9[0] == '0') ? (void *)ti_render_noop
                                     : (void *)ti_render;
    }
    vt[12] = (void *)ti_pdes;
    vt[16] = (void *)ti_d2;
    vt[17] = (void *)ti_d0;

    void **gc = (void **)data_ov007_021032b0;
    gc[0] = (void *)ti_gc0;
    gc[1] = (void *)ti_gc1;
    gc[2] = (void *)ti_gc2;
    gc[3] = (void *)ti_gc3;

    /* The graph-callback table, words 0..5. Words 6 and 7 are NOT touched:
       word 7 is the typeinfo pointer at 0x021032ac, which points at
       0x02103284 and is INSIDE the mount, so the cross pass already rebased
       it, and word 6 carries no relocation at all. Writing either would undo
       correct work. */
    void **cb = (void **)data_ov007_02103290;
    cb[0] = (void *)func_ov007_020ccab4;
    cb[1] = (void *)func_ov007_020cca98;
    cb[2] = (void *)func_ov007_020cca80;
    cb[3] = (void *)func_ov007_020cca74;
    cb[4] = (void *)func_ov007_020cca68;
    cb[5] = (void *)func_ov007_020cc600;

    *(void **)data_ov007_02103254 = (void *)func_ov007_020c3e4c;
    *(void **)data_ov007_02103258 = (void *)func_ov007_020c3e64;

    port_graph_block_register(gc);
}

/* The registry's factory column is void *(*)(void) and the matched factory
   returns int *. One typed forwarder rather than a cast through an
   incompatible function pointer; /OPT:REF follows it to the real body. */
static void *title_spawn(void) { return (void *)func_ov007_020ccad0(); }

// ---- the registry seat -----------------------------------------------------
//
// Deliberately NOT a row in hal/actor_classes.inc. That table is the LEVEL
// cast, walked by the census and by the SM64DS_SKIP_CLASS knob, and a scene is
// not part of any level's cast. The seat is the same three statements
// port_actor_registry_install makes per row -- repoint the factory word, park
// the record in data_020a4bb8, run the fill -- and it makes exactly the same
// cross-check.
struct PortSceneClass {
    unsigned short id;
    const char *name;
    unsigned char *info;
    void *(*factory)(void);
    void (*fill)(void);
    /* DOES THIS SCENE READ data_02092110? See the audit in port_scene_run: the
       harness used to write the sublevel on every run, and for a scene that
       never reads it that write is not inert, it is a state the ROM would
       never have been in at that moment. Measured per class from the ROM's own
       relocations, not asserted. */
    unsigned char reads_sublevel;
};

/* TWO ROWS. dScTitle_c (id 2) and dScGameOver_c (id 8) are derived to the same
   depth -- SpawnInfo, factory and all eighteen vtable slots are recorded in
   port/ov003_syms.txt -- and are NOT seated, for two reasons that are named in
   full at the bottom of port/slice_scene1.txt: eleven of their fourteen slot
   bodies carry the "recovered from vtable slot identity" marker that
   port/tools/inferred_stub_guard refuses until each is ruled against the ROM,
   and three of their dtor TUs spell their vptr writes as per-TU placeholders
   (VT0/VT1/VT2, _ZTV10dScTitle_c, _ZTV8dScene_c, _ZTV7dBase_c, G0) that exist
   in no config. Adding them is a row here plus a block in the slice.
   ov007's dScDSMT_c hit BOTH of those blockers and cleared them rather than
   being excused from them: its six marker-carrying bodies are ruled against
   the ROM in port/tools/inferred_stub_adjudicated.txt, and its two placeholder
   spellings are resolved by the two faces at the top of this file.

   NO ROW GOES IN hal/actor_classes.inc FOR EITHER OF THEM, and the ov004/ov007
   map's step 1 ("the row lives in hal/actor_classes.inc") is superseded by the
   spine this file established rather than followed. That table is the LEVEL
   cast: hal/actor_registry.cpp walks it to build the census and it is what the
   SM64DS_SKIP_CLASS knob indexes. A scene is not part of any level's cast, a
   second seat of id 1 would have two writers for data_020a4bb8[1], and the
   three statements the row would make are exactly the three
   port_scene_registry_install makes below. */
/* THE MINIGAME ROW IS SEATED, run link60 lane MG2, and it is the third scene
   class and the first from ov006. dScMgCurling_c, actor id 0x176 (374), spawn
   symbol MgShuffleShell_Spawn; the ROM's own RTTI string at 0x0213c2d0 reads
   "14dScMgCurling_c" and Shuffle Shell is the same minigame's localised name.
   MG1 derived the whole seat -- SpawnInfo, factory, all thirty-six vtable
   slots, twenty-five bodies ruled REAL_DECOMP -- and left this row out for one
   reason, that the seat did not link. It links now: the wall was the mwcc
   pointer-to-member ABI and it is answered by two host copies against an
   address switch, port/unmatched/MgBase_StateDispatch.cpp and
   MgCurling_StateDispatch.cpp. port/mg_fanout_costs.txt section 4 carries the
   measured accounting.

   THE ROW IS THE ONLY THING THIS LANE CHANGES IN THIS FILE. Its fill, its
   factory forwarder and its overlay constructors are all in hal/scene_mg.cpp,
   and the thirty-three constructors are gated there on the requested id rather
   than run from this file's registry install, which is MG1's ruling and is
   load-bearing: __sinit_ov004_020b948c threads a node onto an arm9-global
   destructor list the LEVEL path walks, and this function runs on every boot.

   reads_sublevel is 0 and it is measured, not assumed: no relocation anywhere
   in ov006 lands on data_02092110 and no ov006 source TU names it,
   SublevelToLevel or SUBLEVEL_LEVEL_TABLE. A minigame is not about a course. */
/* THE SMARTBALL MINIGAME ROW, run mg5 lane SMB. dScMgSmartball_c, actor id 0x178
   (376), spawn symbol MgBingoBallSlotsShot_Spawn; the ROM's own RTTI string at
   0x0213ec7c reads "16dScMgSmartball_c" and Slots Shot and Bingo Ball are the
   two menu games this one class serves. Identity was re-derived from the image
   rather than taken from port/mg_fanout_costs.txt section 3: the doubled-id
   word 0x01780178 occurs exactly once in overlay_0006.bin, at 0x0213ebd4, so
   the SpawnInfo is 0x0213ebd0 and its first word 0x02119824 is the factory,
   and that factory's own literal pool names data_ov006_0213eefc as the vtable.
   All three of section 11's width checks put the table at 36 slots.

   IT IS APPENDED AFTER CURLING'S AND THAT IS THE POINT OF THE ORDER. Section
   11's finding is that this function walks the table in order and calls every
   row's fill on every boot, while the thirty-five overlay constructors run
   ONCE per process at the tail of the first minigame row's fill. Appending
   means curling's fill still runs the constructors and this row's fill finds
   them done. Nothing here depends on that -- both fills are idempotent and
   neither writes outside its own vtable -- but the rule the fan-out is cut
   with is "append", and this row obeys it rather than testing it.

   THE CLASS IS SEATED AND IT DOES NOT RUN, and the row is here anyway for the
   reason port/tools/battery.py exists: the scene list is read out of this
   table at run time, so a seated class gets a selftest on every battery and
   its blocker is recorded rather than remembered. Two of the nine override
   bodies -- slot 0 InitResources and slot 9 Render -- have no delink block and
   no src TU, so they are named traps, and slot 0 is where the class builds
   every sub-object its Behavior ticks. hal/scene_mg.cpp's pre-flight says so
   before the spawn and port/slice_smb.txt carries the derivation.

   reads_sublevel is 0 for curling's reason, unchanged: no relocation anywhere
   in ov006 lands on data_02092110. */

/* THE COINCENTRATION MINIGAME ROW, run mg5 lane CCN. dScMgCoin_c, actor id 0x17a
   (378), spawn symbol MgCoincentration_Spawn; the ROM's own RTTI string at
   0x0213bf24 reads "11dScMgCoin_c" and Coincentration is the minigame's
   localised name. Its SpawnInfo (0x0213bedc), factory (0x020de940) and vtable
   (data_ov006_0213bf50, width 36, span-checked three ways) were all re-derived
   from extracted/overlays/overlay_0006.bin by that lane rather than inherited
   from the fan-out table, and its five marker-carrying override bodies are
   ruled REAL_DECOMP in port/tools/inferred_stub_adjudicated.txt.

   THE ROW IS APPENDED AFTER CURLING'S AND THE ORDER IS LOAD-BEARING, which
   port/mg_fanout_costs.txt section 11 is the write-up of.
   port_scene_registry_install walks this table in TABLE ORDER and calls every
   row's fill on every boot, while port_scene_mg_overlay_load runs the
   thirty-five overlay constructors ONCE PER PROCESS at the tail of the first
   minigame fill that reaches it. So on any boot the curling fill runs the
   constructors and this fill runs after them. That is safe here for a reason
   this lane measured rather than assumed: this seat writes 36 words and no
   more, its span to the next config symbol is exactly 36 words, so it cannot
   reach past its own table into a .data pointer-to-member pair the way a
   37-slot fill of 0x169 or 0x16c would.

   reads_sublevel is 0, for the same measured reason curling's is: a minigame
   is not about a course. */

/* THE FLOWER MINIGAME ROW, run mg5 lane FLW.
   dScMgFlower_c, actor id 0x186 (390) -- the "Loves Me...?" petal minigame.
   The ROM's own RTTI string at 0x02140128 reads "13dScMgFlower_c", and the
   SpawnInfo it belongs to was found by scanning
   extracted/overlays/overlay_0006.bin for the doubled-id word 0x01860186,
   which occurs exactly once, at 0x02140118.

   THE ROW IS APPENDED AFTER CURLING'S AND THE POSITION IS LOAD-BEARING.
   port_scene_registry_install below walks this table in order and calls every
   row's fill on every boot, while hal/scene_mg.cpp runs the thirty-five
   overlay constructors ONCE PER PROCESS from the tail of the FIRST minigame
   row's fill. So appending means the constructors have already run against
   clean ROM words when this fill starts, which is the safe direction
   port/mg_fanout_costs.txt section 11 argues for. (Section 11's actual hazard
   -- a fill writing a word a constructor later copies -- is measured absent
   for this class: zero relocations leave ov006's .init code for either of the
   two tables this row fills. hal/scene_mg_flower.cpp section 4.)

   Its fill, its factory forwarder, its two face arrays and its one named trap
   are all in hal/scene_mg_flower.cpp; this row is the only thing this lane
   changes in this file, which is the shape lane MG2 established.

   reads_sublevel is 0 for the curling row's reason, re-derived rather than
   copied: no relocation anywhere in ov006 lands on data_02092110 and no ov006
   source TU names it. A minigame is not about a course. */
extern "C" {
extern unsigned char MgShuffleShell_SpawnInfo[];
void *port_mg_curling_spawn(void);
void port_scene_fill_curling(void);
/* run mg5 lane LUI: dScMgLuigi_c, the "Wanted!" minigame. */
extern unsigned char MgWanted_SpawnInfo[];
void *port_mg_luigi_spawn(void);
void port_scene_fill_luigi(void);
}

/* Run mg5, lane PCH. dScMgPachinko_c, actor id 0x170 = scene 368, the Bob-omb
   Squad slingshot minigame. The row is APPENDED AFTER curling's rather than
   inserted, and that ordering is load-bearing rather than tidy:
   port_scene_registry_install walks this table in order and calls every row's
   fill on every boot, while port_scene_mg_overlay_load runs the thirty-five
   overlay constructors ONCE PER PROCESS behind a `static int done` reached from
   the tail of the first minigame row's fill. So the first minigame row in this
   table is the one whose fill runs the constructors, and a row inserted BEFORE
   curling's would move that. port/mg_fanout_costs.txt section 11 is the
   analysis; its conclusion is that a wrong-width fill announces itself or stays
   latent depending on this order, and that the fix belongs in the WIDTH. This
   seat's width is 36 and is checked three ways in hal/scene_mg.cpp section 8,
   so the ordering here is defence in depth rather than the remedy.

   reads_sublevel is 0 for the reason the curling row's comment gives, re-checked
   for this class: no relocation in ov006 lands on data_02092110 and no TU in
   this class's closure names it. */
extern "C" {
extern unsigned char MgBobOmbSquad_SpawnInfo[];
void *port_mg_pachinko_spawn(void);
void port_scene_fill_pachinko(void);
/* run mg5 lane SMB: dScMgSmartball_c, the Slots Shot / Bingo Ball minigame.
   Same reads_sublevel reasoning and the same appended-row rule as above. */
extern unsigned char MgBingoBallSlotsShot_SpawnInfo[];
void *port_mg_smartball_spawn(void);
void port_scene_fill_smartball(void);
/* run mg5 lane CCN: dScMgCoin_c, the Coincentration minigame. */
extern unsigned char MgCoincentration_SpawnInfo[];
void *port_mg_coin_spawn(void);
void port_scene_fill_coin(void);
/* run mg5 lane FLW: dScMgFlower_c, the "Loves Me...?" minigame. Its SpawnInfo
   is spelled as the raw config symbol rather than a recovered name because the
   ROM gives this one no spawn symbol; hal/scene_mg_flower.cpp carries the
   derivation from the doubled-id word at 0x02140118. */
extern unsigned char data_ov006_02140114[];
void *port_mg_flower_spawn(void);
void port_scene_fill_flower(void);
/* run mg6 lane MEM: dScMgMemory2_c, the "Memory Master" minigame. Same
   reads_sublevel reasoning and the same appended-row rule as above, both
   re-checked for this class: no relocation anywhere in ov006 lands on
   data_02092110 and no TU in this class's closure names it. */
extern unsigned char MgMemoryMaster_SpawnInfo[];
void *port_mg_memory2_spawn(void);
void port_scene_fill_memory2(void);
/* run mg6 lane LKT: dScMgPachinko2_c, the "Lakitu Launch" minigame and the
   SIBLING of the 368 row above in both the class name and the ROM's own data
   adjacency. hal/scene_mg.cpp's "RUN mg6, LANE LKT" banner carries the
   derivation. */
extern unsigned char MgLakituLaunch_SpawnInfo[];
void *port_mg_pachinko2_spawn(void);
void port_scene_fill_pachinko2(void);
/* run mg6 lane SOS: dScMgBomroom_c, actor id 0x172 = scene 370, the
   "Sort or 'Splode" minigame. The spawn symbol is MgSortOrSplode and the ROM's
   own RTTI string at 0x0213bb38 reads "14dScMgBomroom_c", so the row below is
   named for the class the way SCENE_MG_CURLING and SCENE_MG_COIN are.
   port/slice_sos.txt carries the derivation, the three width checks and the
   two floors; hal/scene_mg_bomroom.cpp is the seat. */
extern unsigned char MgSortOrSplode_SpawnInfo[];
void *port_mg_bomroom_spawn(void);
void port_scene_fill_bomroom(void);
/* run mg6 lane S75: dScMgCurling2_c, the advanced half of the curling pair
   whose basic half is scene 374. Its SpawnInfo is spelled as the raw config
   symbol because the ROM gives this id no spawn symbol either;
   hal/scene_mg_curling2.cpp and port/slice_s75.txt carry the derivation from
   the doubled-id word 0x01770177 at 0x0213c438, which occurs exactly once in
   the overlay. */
extern unsigned char data_ov006_0213c434[];
void *port_mg_curling2_spawn(void);
void port_scene_fill_curling2(void);
/* run mg6 lane PPP: dScMgPanel_c, actor id 0x17c. The spawn symbol carries
   both localised titles the naming pipeline found for the one id, and the
   ROM's own RTTI at 0x0213dd84 -- reached through the type_info the word
   BEFORE the vtable points at -- reads "12dScMgPanel_c", so the row is named
   for the class the way SCENE_MG_CURLING and SCENE_MG_LUIGI are. Same
   reads_sublevel reasoning as the rows above, re-checked for this class: no
   relocation in ov006 lands on data_02092110 and no TU in this class's
   closure names it. */
extern unsigned char MgPuzzlePanelPuzzlePanic_SpawnInfo[];
void *port_mg_panel_spawn(void);
void port_scene_fill_panel(void);
/* run mg9 lane CUP: dScMgCup_c, actor id 0x169 = scene 361 -- the minigame the
   ROM calls "TOX BOX SHUFFLE" (message 559, reached from ov005 row 23's param
   0x000b1700 through data_ov004_020bc070[11]; hal/scene_mg_cup.cpp section 0
   has the chain and the control against every seated id's known title). The
   row keeps its class-derived NAME for the reason every row above does:
   port/tools/battery.py reads the hosted-scene set out of this table.
   The ROM gives this id no Mg*_Spawn symbol, so the SpawnInfo is spelled as
   the raw config symbol the way the 375 and 390 rows are; the doubled-id word
   0x01690169 sits at 0x0213c024 and occurs exactly once in the overlay. The
   class name is the ROM's own RTTI, "10dScMgCup_c" at 0x0213c054, reached
   through the type_info the word before the vtable points at -- the lane PPP
   chain, whose every edge is a relocation. port/slice_cup.txt carries the
   derivation and hal/scene_mg_cup.cpp is the seat. Same reads_sublevel
   reasoning as the rows above, re-checked for this class: no relocation in
   ov006 lands on data_02092110 and no TU in this class's closure names it. */
extern unsigned char data_ov006_0213c020[];
void *port_mg_cup_spawn(void);
void port_scene_fill_cup(void);
/* run mg9 lane MMT: dScMgMemory_c, actor id 0x16a = scene 362, the "Memory
   Match" minigame and the immediate SIBLING of the 363 row above. The spawn
   symbol is MgMemoryMatch and the ROM's own RTTI at 0x0213d0b4 -- reached
   through the type_info the relocated word before the vtable points at --
   reads "13dScMgMemory_c", so the row is named for the class the way
   SCENE_MG_CURLING and SCENE_MG_MEMORY2 are. Same reads_sublevel reasoning as
   the rows above, re-checked for this class: no relocation in ov006 lands on
   data_02092110 and no TU in this class's closure names it. */
extern unsigned char MgMemoryMatch_SpawnInfo[];
void *port_mg_memory1_spawn(void);
void port_scene_fill_memory1(void);
/* run mg9 lane S364: dScMgSlot1_c, actor id 0x16c = scene 364. The SpawnInfo
   is spelled as the raw config symbol because the ROM gives this id no spawn
   symbol -- the same reason the 375 and 390 rows above do -- and the row is
   named for the ROM's own RTTI at 0x0213e5bc, "12dScMgSlot1_c", reached
   through the type_info the word BEFORE the vtable points at. No naming
   import has ever named this address. The class is a three-reel slot machine
   and it serves TWO menu entries, read out of the ROM's BMG bank: ov005 row
   17 is message 565 "Mario Slot" and row 33 is message 587 "Super Mario
   Slot". The row keeps the CLASS name for the reason the 380 row does --
   0x17c is SCENE_MG_PANEL and not SCENE_MG_PUZZLE_PANIC. Same reads_sublevel
   reasoning
   as the rows above, re-checked for this class: no relocation in ov006 lands
   on data_02092110 and no TU in this class's closure names it. */
extern unsigned char data_ov006_0213e560[];
void *port_mg_slot1_spawn(void);
void port_scene_fill_slot1(void);
/* run mg9 lane BOX: dScMgSound_c, actor id 0x16f = scene 367, the "Boom Box"
   minigame. The spawn symbol is MgBoomBox and the class name is the ROM's own
   RTTI, reached through the type_info the word BEFORE the vtable points at:
   data_ov006_0213f844[-1] is 0x0213f6e4, whose name pointer is 0x0213f708,
   which reads "12dScMgSound_c". Same reads_sublevel reasoning as the rows
   above, re-checked for this class: no relocation in ov006 lands on
   data_02092110 and no TU in this class's closure names it. */
extern unsigned char MgBoomBox_SpawnInfo[];
void *port_mg_boombox_spawn(void);
void port_scene_fill_boombox(void);
/* run mg9 lane S371: dScMgAmida_c, actor id 0x173 = scene 371, and THE ONE
   TRUE WIDTH-37 CLASS of the twenty-nine (port/mg_fanout_costs.txt section 11).
   The ROM gives this id NO spawn symbol anywhere in config -- the whole-config
   ADDRESS sweep port/slice_s75.txt's Shell Smash lesson prescribes was run and
   returns nothing -- so the SpawnInfo is spelled as its raw config symbol, the
   way the 375 and 390 rows are. The ROM's own RTTI at 0x0213b828, reached
   through the type_info the word BEFORE the vtable points at, reads
   "12dScMgAmida_c", so the row is named for the class.
   TWO PLAYER TITLES ON ONE ID, the dScMgPanel_c shape: ov005 row 0 is
   "Mario's Slides" and row 24 is "Connect the Characters", both amidakuji.
   port_mg_scene_spawn_param takes the first matching row, so this row boots as
   row 0 with record 0 and text 0. port/slice_s371.txt carries the derivation
   and the 25-way control that proves the title chain. Same reads_sublevel
   reasoning as every row above, re-checked for this class: no relocation in
   ov006 lands on data_02092110 and no TU in this class's closure names it. */
extern unsigned char data_ov006_0213b814[];
void *port_mg_amida_spawn(void);
void port_scene_fill_amida(void);
/* run mg9 lane S381: dScMgMCarlo_c, actor id 0x17d = scene 381, the
   "Pair-a-Gone" card minigame. Its SpawnInfo is spelled as the raw config
   symbol because the ROM gives this id no spawn symbol -- no naming pass has
   ever resolved it, and a config-wide sweep for the address finds only
   symbols/actor_renames_report.txt's own "UNRESOLVED actor 381" line. The
   class name comes out of the ROM's type_info at 0x0213d588, reached through
   the word before the vtable, whose name pointer reads "13dScMgMCarlo_c"; the
   player title comes out of the ov005 launch table's only row for this scene
   (name-text 13 -> data_ov004_020bc070[13] = message 561 = "Pair-a-Gone").
   port/slice_s381.txt carries both derivations and the four width checks;
   hal/scene_mg_mcarlo.cpp is the seat. Same reads_sublevel reasoning as the
   rows above, re-derived rather than copied: no relocation anywhere in ov006
   lands on data_02092110 and no TU in this class's closure names it. */
extern unsigned char data_ov006_0213d580[];
void *port_mg_mcarlo_spawn(void);
void port_scene_fill_mcarlo(void);
/* run mg9 lane WIG: dScMgHanachan_c, actor id 0x182 = scene 386, the "Which
   Wiggler" minigame. The spawn symbol is MgWhichWiggler and the ROM's own RTTI
   at 0x0213ca88 -- reached through the type_info the word BEFORE the vtable
   points at -- reads "15dScMgHanachan_c", so the row below is named for the
   class the way SCENE_MG_CURLING, SCENE_MG_LUIGI and SCENE_MG_BOMROOM are.
   port/slice_wig.txt carries the derivation, the five width checks and the two
   floors; hal/scene_mg_wiggler.cpp is the seat. */
extern unsigned char MgWhichWiggler_SpawnInfo[];
void *port_mg_wiggler_spawn(void);
void port_scene_fill_wiggler(void);
/* run mg9 lane BOO: dScMgTeresa_c, actor id 0x183, "Hide and Boo Seek". The
   spawn symbol MgHideAndBooSeek carries the player title and the ROM's own
   RTTI at 0x0213f9c0 -- reached through the type_info the word BEFORE the
   vtable points at, relocs.txt from:0x0213fa08 to:0x0213f9b4 -- reads
   "13dScMgTeresa_c", so the row is named for the class the way SCENE_MG_PANEL
   and SCENE_MG_LUIGI are. Teresa is Boo's Japanese name, so the two witnesses
   agree about what the class is and only the relocation is unfalsifiable.
   Same reads_sublevel reasoning as every minigame row above, re-checked for
   this class rather than copied: no relocation in ov006 lands on
   data_02092110 and no TU in this class's closure names it. */
extern unsigned char MgHideAndBooSeek_SpawnInfo[];
void *port_mg_booseek_spawn(void);
void port_scene_fill_booseek(void);
/* run mg9 lane LKY: dScMgBSC_c, actor id 0x184. The spawn symbol is
   MgLuckyStars and the ROM's own RTTI -- reached through the type_info the word
   BEFORE the vtable points at, 0x0213fec4 -> 0x0213fd8c -> 0x0213fd98 -- reads
   "10dScMgBSC_c", so the row is named for the class the way SCENE_MG_CURLING,
   SCENE_MG_LUIGI and SCENE_MG_PANEL are. Same reads_sublevel reasoning as the
   rows above, re-checked for this class: no relocation in ov006 lands on
   data_02092110 and no TU in this class's closure names it. */
extern unsigned char MgLuckyStars_SpawnInfo[];
void *port_mg_luckystars_spawn(void);
void port_scene_fill_luckystars(void);
/* run mg9 lane PSY: dScMg3DEsp_c, actor id 0x185, the "Psyche Out!" minigame.
   The class name is the ROM's own type_info, reached the way the panel row
   above reaches its: the word BEFORE data_ov006_0213c8c4 points at 0x0213c7c8
   and that record's name pointer is 0x0213c7d4, which reads "12dScMg3DEsp_c".
   Same reads_sublevel reasoning as the rows above, re-checked for this class:
   no relocation in ov006 lands on data_02092110 and no TU in this class's
   closure names it. */
extern unsigned char MgPsycheOut_SpawnInfo[];
void *port_mg_esp3d_spawn(void);
void port_scene_fill_esp3d(void);
/* run mg11 lane MUG: dScMgSlot3_c, actor id 0x16d = scene 365, the "Mix-a-Mug"
   slot machine. Its SpawnInfo is spelled as the raw config symbol because the
   ROM gives this id no spawn symbol -- no naming pass has ever resolved it,
   and a config-wide sweep for the address finds only
   symbols/actor_renames_report.txt's own UNRESOLVED line. The class name is
   the ROM's own type_info, reached the way the rows above reach theirs: the
   word BEFORE data_ov006_0213eaa8 points at 0x0213e588 and that record's name
   pointer is 0x0213e5ac, which reads "12dScMgSlot3_c". port/slice_mug.txt has
   the title derivation (ov005 row 31 -> name text 27 -> BMG message 575 ->
   "Mix-a-Mug") and the 25-of-25 control that decides which byte of the param
   is the name-text index. Same reads_sublevel reasoning as every row above,
   re-checked for this class: no relocation in ov006 lands on data_02092110 and
   no TU in this class's closure names it. */
extern unsigned char data_ov006_0213e508[];
void *port_mg_slot3_spawn(void);
void port_scene_fill_slot3(void);
/* run mg11 lane BNP: dScMgJump_c, actor id 0x174 = scene 372, the "Bounce and
   Pounce" minigame. The spawn symbol MgBounceAndPounce carries the ROM's own
   English title through the ov005 launch table (row 4, param 0x00040400 ->
   name-text 4 -> data_ov004_020bc070[4] = 552 -> BMG message 552 = "Bounce and
   Pounce"), and the ROM's own RTTI -- reached through the type_info the word
   BEFORE the vtable points at, 0x0213cbe0 -> 0x0213cb98 -> 0x0213cba4 -- reads
   "11dScMgJump_c", so the row below is named for the class the way
   SCENE_MG_CURLING, SCENE_MG_LUIGI and SCENE_MG_PANEL are.

   THE SPAWNINFO IS SPELLED BY ITS SYMBOL AND THE VTABLE IS NOT, and the
   difference matters. config/arm9/overlays/ov006/symbols.txt gives the name
   _ZTV17MgBounceAndPounce to 0x0213c62c, which is the BASE class
   dScMgD3DBase_c's table, not this class's; this class's own table is only
   ever spelled data_ov006_0213cbe4. port/slice_bnp.txt section 4 is the
   derivation and hal/scene_mg_jump.cpp fills both.

   Same reads_sublevel reasoning as every minigame row above, re-derived rather
   than copied: no relocation anywhere in ov006 lands on data_02092110 and no
   TU in this class's closure names it. A minigame is not about a course. */
extern unsigned char MgBounceAndPounce_SpawnInfo[];
void *port_mg_jump_spawn(void);
void port_scene_fill_jump(void);
/* run mg11 lane BNT: dScMgJump2_c, actor id 0x175, scene 373, the "Bounce and
   Trounce" minigame. The spawn symbol is MgBounceAndTrounce and the ROM's own
   RTTI -- reached through the type_info the word BEFORE the vtable points at,
   0x0213ccf8 -> 0x0213cca4 -> +4 -> 0x0213ccbc -- reads "12dScMgJump2_c", so
   the row is named for the class the way SCENE_MG_CURLING, SCENE_MG_LUIGI and
   SCENE_MG_PANEL are. Its intermediate base is dScMgD3DBase_c, which 0x174,
   0x180 and 0x181 also derive from; port/slice_bnt.txt sections 3 and 7 are
   the attribution and the merge consequences. Same reads_sublevel reasoning as
   every minigame row above, re-checked for this class rather than copied: no
   relocation anywhere in ov006 lands on data_02092110 and no TU in this
   class's closure names it. */
extern unsigned char MgBounceAndTrounce_SpawnInfo[];
void *port_mg_jump2_spawn(void);
void port_scene_fill_jump2(void);
/* run mg11 lane SNW: dScMgSnowball_c, actor id 0x179 = scene 377, "Snowball
   Slalom".  The spawn symbol is MgSnowballSlalom and the ROM's own RTTI --
   reached through the type_info the word BEFORE the vtable points at,
   relocs.txt from:0x02140008 to:0x0213ffd0, whose name pointer 0x0213ffdc
   reads "15dScMgSnowball_c" -- names the class, so the row is named for the
   class the way SCENE_MG_CURLING, SCENE_MG_LUIGI and SCENE_MG_PANEL are.

   THIS IS THE ONE ROW port/mg_fanout_costs.txt SECTION 3 LEFT BLANK.  Its
   vtable resolves through no load relocation in its own factory, because
   src/MgSnowballSlalom_Spawn.cpp writes no vtable at all: it allocates 0xc59c
   and calls func_ov006_021295ac, which writes data_ov006_0213e448 and then
   data_ov006_0214000c.  port/slice_snw.txt is the hand derivation, including
   the five width checks, the ruling-out of the two alternative signature
   tables and the four-site pointer-to-member wall.

   Same reads_sublevel reasoning as every minigame row above, re-derived rather
   than copied: no relocation anywhere in ov006 lands on data_02092110 and no
   TU in this class's closure names it.  A minigame is not about a course. */
extern unsigned char MgSnowballSlalom_SpawnInfo[];
void *port_mg_snowball_spawn(void);
void port_scene_fill_snowball(void);
/* run mg11 lane PKR: dScMgCard_c, actor id 0x17b = scene 379, the "Picture
   Poker" minigame. The spawn symbol is MgPicturePoker and the ROM's own RTTI --
   reached through the type_info the word BEFORE the vtable points at,
   0x0213bdb0 -> 0x0213bc70 -> +4 -> 0x0213bc7c -- reads "11dScMgCard_c", so the
   row is named for the class the way SCENE_MG_CURLING, SCENE_MG_LUIGI and
   SCENE_MG_PANEL are. The player title comes out of the ov005 launch table's
   only row for this id (row 9, param 0x00050900 -> name text 5 ->
   data_ov004_020bc070[5] = message 553 = "Picture Poker"), decoded from
   NitroFS file 643 by this lane rather than cited. port/slice_pkr.txt carries
   both derivations, the five width checks, the dScMgCoin_c attribution ruling
   and the one floor; hal/scene_mg_card.cpp is the seat. Same reads_sublevel
   reasoning as every minigame row above, re-checked for this class rather than
   copied: no relocation in ov006 lands on data_02092110 and no TU in this
   class's closure names it. */
extern unsigned char MgPicturePoker_SpawnInfo[];
void *port_mg_card_spawn(void);
void port_scene_fill_card(void);
/* run mg11 lane PGO: dScMgMCarlo2_c, actor id 0x17e = scene 382, the
   "Pair-a-Gone And On" endless-mode card minigame -- the sequel to scene 381's
   Pair-a-Gone, which run mg9 lane S381 seated. This id DOES have a spawn
   symbol, unlike its sibling: MgPairAGoneAndOn_SpawnInfo at 0x0213d70c, whose
   second word is the doubled id 0x017e017e. The class name comes out of the
   ROM's own type_info at 0x0213d714, reached through the word before the
   vtable, whose name pointer 0x0213d730 reads "14dScMgMCarlo2_c"; the player
   title comes out of the ov005 launch table's only row for this scene
   (row 25, name-text 25 -> data_ov004_020bc070[25] = message 573 =
   "Pair-a-Gone And On"). port/slice_pgo.txt carries both derivations, the five
   width checks and the attribution finding that keeps the 25 member-pointer
   pairs after this vtable with their real owner, MgBobOmbSquad;
   hal/scene_mg_mcarlo2.cpp is the seat. Same reads_sublevel reasoning as every
   minigame row above, re-derived rather than copied: no relocation anywhere in
   ov006 lands on data_02092110 and no TU in this class's closure names it. */
extern unsigned char MgPairAGoneAndOn_SpawnInfo[];
void *port_mg_mcarlo2_spawn(void);
void port_scene_fill_mcarlo2(void);
/* run mg11 lane RLT: dScMgRoulette_c, actor id 0x17f = scene 383, "Mushroom
   Roulette". The spawn symbol is MgMushroomRoulette and the ROM's own type_info
   -- reached the way every row above reaches its, through the word BEFORE the
   vtable: relocs.txt from:0x0213e398 to:0x0213e300, whose name pointer
   0x0213e30c reads "15dScMgRoulette_c" -- so the row is named for the class the
   way SCENE_MG_CURLING, SCENE_MG_PANEL and SCENE_MG_ESP3D are. The player title
   is the ov005 launch table's only row for this id (row 13, param 0x00090d00 ->
   name-text 9 -> data_ov004_020bc070[9] = message 557 = "Mushroom Roulette").
   port/slice_rlt.txt carries the derivation, the four width checks, the six ROM
   adjudications and the two-address member-pointer census;
   hal/scene_mg_roulette.cpp is the seat. Same reads_sublevel reasoning as the
   rows above, re-checked for this class rather than copied: no relocation
   anywhere in ov006 lands on data_02092110 and no TU in this class's closure
   names it. */
extern unsigned char MgMushroomRoulette_SpawnInfo[];
void *port_mg_roulette_spawn(void);
void port_scene_fill_roulette(void);
/* run mg11 lane TTI: dScMgTrampoline_c, actor id 0x180 = scene 384,
   "Trampoline Time". The spawn symbol is MgTrampolineTime and the ROM's own
   RTTI -- reached through the type_info the word BEFORE the vtable points at,
   0x0213fb30 -> 0x0213fad0 -> 0x0213faf0 -- reads "17dScMgTrampoline_c", so
   the row is named for the class the way SCENE_MG_CURLING, SCENE_MG_LUIGI and
   SCENE_MG_PANEL are. The FIRST class the port seats under dScMgD3DBase_c,
   whose type_info 0x0213c5c8 the same record's third word points at; three
   sibling classes share that base. port/slice_tti.txt is the derivation and
   hal/scene_mg_trampoline.cpp is the seat. Same reads_sublevel reasoning as the
   rows above, re-checked for this class: no relocation in ov006 lands on
   data_02092110 and no TU in this class's closure names it. */
extern unsigned char MgTrampolineTime_SpawnInfo[];
void *port_mg_trampoline_spawn(void);
void port_scene_fill_trampoline(void);
/* run mg11 lane TTE: dScMgTrampoline2_c, actor id 0x181 = scene 385, the
   "Trampoline Terror" minigame. The spawn symbol is MgTrampolineTerror and the
   ROM's own RTTI -- reached through the type_info the word BEFORE the vtable
   points at, 0x0213fc78 -> 0x0213fc04 -> 0x0213fc5c -- reads
   "18dScMgTrampoline2_c", so the row is named for the class the way
   SCENE_MG_CURLING, SCENE_MG_LUIGI and SCENE_MG_PANEL are. The player title is
   the ov005 launch table's only row for this id (row 32, param 0x00202001 ->
   name-text 32 -> data_ov004_020bc070[32] = message 580 = "Trampoline Terror").
   Same reads_sublevel reasoning as every minigame row above, re-derived rather
   than copied: no relocation in ov006 lands on data_02092110 and no TU in this
   class's closure names it. */
extern unsigned char MgTrampolineTerror_SpawnInfo[];
void *port_mg_trampoline2_spawn(void);
void port_scene_fill_trampoline2(void);
}

static const PortSceneClass port_scene_classes[] = {
    {4, "SCENE_STAR_SELECT", StarSelect_SpawnInfo, StarSelect_Spawn,
     scene_fill_starsel, 1},
    {1, "SCENE_TITLE", data_ov007_02103264, title_spawn,
     scene_fill_title, 0},
    /* 374 is 0x176, and it is spelled in decimal because the other two rows
       are and because port/tools/battery.py reads its hosted-scene set out of
       this table. */
    {374, "SCENE_MG_CURLING", MgShuffleShell_SpawnInfo, port_mg_curling_spawn,
     port_scene_fill_curling, 0},
    /* run mg5 lane LUI. 366 is 0x16e; the spawn symbol is MgWanted and the
       ROM's own RTTI at 0x0213ce60 reads "12dScMgLuigi_c", so the row is named
       for the class the way SCENE_MG_CURLING is. APPENDED AFTER CURLING'S ROW
       on purpose: port_scene_registry_install walks this table in order and
       port_scene_mg_overlay_load runs the thirty-five constructors once per
       process at the tail of the FIRST minigame row's fill, so a row placed
       before curling's would have its fill run before those constructors read
       the mounted .data. port/mg_fanout_costs.txt section 11 is the
       derivation. Nothing in this class's fill writes outside its own 36-slot
       table -- the width is checked three ways in port/slice_lui.txt -- so the
       ordering is a rule this lane obeys rather than a hazard it relies on. */
    {366, "SCENE_MG_LUIGI", MgWanted_SpawnInfo, port_mg_luigi_spawn,
     port_scene_fill_luigi, 0},
    /* 368 is 0x170, spelled in decimal for the reason the row above is. */
    {368, "SCENE_MG_PACHINKO", MgBobOmbSquad_SpawnInfo, port_mg_pachinko_spawn,
     port_scene_fill_pachinko, 0},
    /* 376 is 0x178. Appended, per the rule section 11 of
       port/mg_fanout_costs.txt derives from the once-per-process constructor
       gate. */
    {376, "SCENE_MG_SMARTBALL", MgBingoBallSlotsShot_SpawnInfo,
     port_mg_smartball_spawn, port_scene_fill_smartball, 0},
    /* APPENDED AT THE END, run mg5 lane CCN. 378 is 0x17a, spelled in decimal
       for the reason the row above gives. See the block above this table for
       why appending rather than inserting is the correct place for a minigame
       row. */
    {378, "SCENE_MG_COIN", MgCoincentration_SpawnInfo, port_mg_coin_spawn,
     port_scene_fill_coin, 0},
    /* 390 is 0x186, spelled in decimal for the same two reasons the row above
       is: the other rows are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED LAST, on purpose; see the header. */
    {390, "SCENE_MG_FLOWER", data_ov006_02140114, port_mg_flower_spawn,
     port_scene_fill_flower, 0},
    /* APPENDED AFTER EVERY EXISTING ROW, run mg6 lane MEM. 363 is 0x16b,
       spelled in decimal for the two reasons the rows above are: the others
       are, and port/tools/battery.py reads its hosted-scene set out of this
       table. Appending matters twice for this class rather than once. It is
       the latent-safe direction port/mg_fanout_costs.txt section 11 derives
       from the once-per-process constructor gate; and this class shares the
       dScMgSingle3DBase_c table at 0x0213e448 with the flower row above, so
       running after that row means the flower's fill claims the middle table
       first and its witness keeps counting exactly what it counted before this
       seat existed. hal/scene_mg_memory2.cpp section 3 is the measurement. */
    {363, "SCENE_MG_MEMORY2", MgMemoryMaster_SpawnInfo, port_mg_memory2_spawn,
     port_scene_fill_memory2, 0},
    /* 369 is 0x171, spelled in decimal for the reason every row above gives.
       APPENDED AFTER ALL OF THEM, run mg6 lane LKT, and the position is
       load-bearing in the direction port/mg_fanout_costs.txt section 11 calls
       latent-safe: this function walks the table in order and calls every
       row's fill on every boot, while port_scene_mg_overlay_load runs the
       thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill. Appending means the constructors have already
       run against clean ROM words when this fill starts.

       THIS ROW'S CLASS IS THE ONE SECTION 11's HAZARD WOULD HAVE HIT, which
       is why the ordering argument is worth restating here rather than
       cross-referencing. hal/scene_mg.cpp section 8 records that a 37-slot
       fill of the 368 row would write a host thunk over data_ov006_0213da5c,
       the code word of THIS class's state 4. That fill is called with 36. And
       the reverse is measured in that file's LKT banner: this table is 36 words
       by four independent checks, so it cannot reach into id 0x17c's state
       table the way a 37th slot would.

       reads_sublevel is 0 for the curling row's reason, re-checked for this
       class: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {369, "SCENE_MG_PACHINKO2", MgLakituLaunch_SpawnInfo,
     port_mg_pachinko2_spawn, port_scene_fill_pachinko2, 0},
    /* APPENDED AT THE END, run mg6 lane SOS. 370 is 0x172, spelled in decimal
       for the two reasons the rows above are: the others are, and
       port/tools/battery.py reads its hosted-scene set out of this table.

       THE POSITION IS THE FILL-ORDER RULE AND NOT TIDINESS.
       port_scene_registry_install walks this table in TABLE ORDER and calls
       every row's fill on every boot, while port_scene_mg_overlay_load runs
       the thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill. Appending means the constructors have already
       run against clean ROM words when this fill starts, which is the
       latent-safe direction port/mg_fanout_costs.txt section 11 argues for.
       Section 11's actual hazard -- a fill writing a word a constructor later
       copies -- cannot arise here anyway: this seat writes 36 words, the span
       to the next config symbol is exactly 36 words, and slot 35 holds the
       family's terminal word, so the fill cannot reach past its own table.
       All three checks are in port/slice_sos.txt.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {370, "SCENE_MG_BOMROOM", MgSortOrSplode_SpawnInfo, port_mg_bomroom_spawn,
     port_scene_fill_bomroom, 0},
    /* 375 is 0x177, spelled in decimal for the two reasons every row above
       gives. APPENDED AFTER EVERY EXISTING ROW, run mg6 lane S75, and for this
       class the ordering rule is worth naming rather than inheriting: its
       vtable at 0x0213c510 is the closest any seated table sits to curling's
       live data. port/mg_fanout_costs.txt section 11's hazard is a fill that
       writes into a word an overlay constructor later copies into a .bss
       dispatch table, and appending is the latent-safe direction. It is also
       measured absent in both directions here -- hal/scene_mg_curling2.cpp
       section 3 has the four spans -- so this row obeys the rule rather than
       relying on it, and the lane's scene-374 canary is the check. */
    {375, "SCENE_MG_CURLING2", data_ov006_0213c434, port_mg_curling2_spawn,
     port_scene_fill_curling2, 0},
    /* 380 is 0x17c, spelled in decimal for the same two reasons the rows above
       are: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, which is the rule
       port/mg_fanout_costs.txt section 11 derives from the once-per-process
       constructor gate: port_scene_registry_install walks this table in order
       and port_scene_mg_overlay_load runs the thirty-five constructors at the
       tail of the FIRST minigame row's fill, so a row placed before an earlier
       class's would have its fill run before those constructors read the
       mounted .data. Nothing in this class's fill writes outside its own
       36-slot table -- the width is checked three ways in port/slice_ppp.txt --
       so appending is a rule this lane obeys rather than a hazard it needs. */
    {380, "SCENE_MG_PANEL", MgPuzzlePanelPuzzlePanic_SpawnInfo,
     port_mg_panel_spawn, port_scene_fill_panel, 0},
    /* 361 is 0x169, spelled in decimal for the two reasons every row above
       gives: the others are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane
       CUP, and for this class the ordering rule is load-bearing TWICE rather
       than once.

       ONE: it is the latent-safe direction port/mg_fanout_costs.txt section 11
       derives from the once-per-process constructor gate --
       port_scene_registry_install walks this table in order and calls every
       row's fill on every boot, while port_scene_mg_overlay_load runs the
       thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill, so a row placed earlier would have its fill
       run before those constructors read the mounted .data. Section 11's
       hazard is dScMgCup_c's OWN: the word past the end of its vtable is the
       mwcc pair src/__sinit_ov006_021304ac.c copies into curling's state table
       0x02141930 slot 2. This seat writes 36 words, the span to the next
       config symbol is exactly 36 words and slot 35 holds the family's
       terminal 0x020ad660, so the fill cannot reach past its own table -- the
       rule is obeyed rather than relied on, and the lane's scene-374 canary is
       the check.

       TWO: this class shares the dScMgSingle3DBase_c table at 0x0213e448 with
       the 390 row AND the 363 row above it. mg_apply keys on a DS address, so
       whichever fill runs first claims that table; running after both means
       the flower keeps it and BOTH existing witnesses keep counting exactly
       what they counted before this seat existed. hal/scene_mg_cup.cpp
       section 3 is the measurement, and this lane boots 390 and 363 after
       seating to prove the two censuses did not move.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {361, "SCENE_MG_CUP", data_ov006_0213c020, port_mg_cup_spawn,
     port_scene_fill_cup, 0},
    /* 362 is 0x16a, spelled in decimal for the two reasons every row above
       gives: the others are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane
       MMT, and the position is load-bearing TWICE for this class, the same way
       it is for the 363 row.

       It is the latent-safe direction port/mg_fanout_costs.txt section 11
       derives from the once-per-process constructor gate: this function walks
       the table in order and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors
       ONCE PER PROCESS at the tail of the FIRST minigame row's fill, so a row
       placed earlier would have its fill run before those constructors read
       the mounted .data. And this class shares the dScMgSingle3DBase_c table
       at 0x0213e448 with the flower row and the 363 row, so running last means
       the flower's fill keeps claiming the middle table and BOTH other seats'
       middle-table witnesses keep counting exactly what they counted before
       this seat existed. hal/scene_mg_memory1.cpp section 3 is the argument.

       SECTION 11's ACTUAL HAZARD IS LIVE FOR THIS CLASS AND IS MEASURED
       ABSENT. 0x16a is one of the twelve rows section 11 corrected from width
       37 to width 36, and the word at index 36 of data_ov006_0213d1b8 is the
       code half of an mwcc pair whose body, func_ov006_020f6538, is
       dScMgMemory2_c's round-end state -- so a 37-slot fill here would clobber
       a live state of the row above on the same tree. This fill is called with
       36, and port/slice_mmt.txt has all four checks that say 36. */
    {362, "SCENE_MG_MEMORY1", MgMemoryMatch_SpawnInfo, port_mg_memory1_spawn,
     port_scene_fill_memory1, 0},
    /* 364 is 0x16c, spelled in decimal for the two reasons every row above
       gives: the others are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane
       S364.

       THE POSITION IS THE FILL-ORDER RULE AND FOR THIS CLASS IT IS WORTH
       RESTATING RATHER THAN CROSS-REFERENCING, because this is the class
       port/mg_fanout_costs.txt section 11 uses as its second worked example.
       port_scene_registry_install walks this table in TABLE ORDER and calls
       every row's fill on every boot, while port_scene_mg_overlay_load runs
       the thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill. Appending means the constructors have already
       run against clean ROM words when this fill starts, which is the
       latent-safe direction.

       AND SECTION 11's HAZARD IS THE ONE THIS ROW WOULD HAVE CAUSED. Index 36
       of data_ov006_0213eb40 is MgBingoBallSlotsShot_SpawnInfo's factory word
       0x02119824, followed by the doubled id 0x01780178 -- so a 37-slot fill
       here writes a host thunk over the factory pointer of SCENE 376, which
       is the SCENE_MG_SMARTBALL row eleven lines above and ships today. The
       fill is called with 36 and four independent checks agree on that width
       (span to the next config symbol is exactly 36 words, slot 35 holds the
       family terminal 0x020ad660, the word past the end is a SpawnInfo record
       and not a slot, and the relocation count inside the span is 36 with
       indices 0..35 complete). port/slice_s364.txt section 3 is the
       derivation and this lane's sibling canary is a scene-376 boot.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {364, "SCENE_MG_SLOT1", data_ov006_0213e560, port_mg_slot1_spawn,
     port_scene_fill_slot1, 0},
    /* 367 is 0x16f, spelled in decimal for the same two reasons every row
       above is: the others are, and port/tools/battery.py reads its
       hosted-scene set out of this table. APPENDED AFTER EVERY EXISTING ROW,
       run mg9 lane BOX, and the position matters twice for this class rather
       than once.

       It is the latent-safe direction port/mg_fanout_costs.txt section 11
       derives from the once-per-process constructor gate; and this class is
       the THIRD seated under dScMgSingle3DBase_c (0x0213e448), after the
       flower row and the memory2 row, so running after both means the earlier
       fill keeps the middle table and its witness is unchanged. This seat's
       middle-table counters read zero by design and are printed anyway --
       hal/scene_mg_boombox.cpp section 3 is the measurement and the argument
       for promoting kSingle3DFaces to a seam when a FOURTH class is seated.

       Nothing in this class's fill writes outside its own 36-slot table. The
       width is checked FIVE ways in port/slice_box.txt, and for this class the
       fourth check has a named victim: index 36 of 0x0213f844 is an mwcc
       {code, 0} member pointer that __sinit_ov006_02132f68 copies into
       dScMgTeresa_c's state table, so a 37-slot fill would corrupt a
       DIFFERENT minigame's state. */
    {367, "SCENE_MG_BOOMBOX", MgBoomBox_SpawnInfo, port_mg_boombox_spawn,
     port_scene_fill_boombox, 0},
    /* 371 is 0x173, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane S371,
       and for THIS row the rule is load-bearing rather than inherited, because
       this is the only fill in the tree that writes a THIRTY-SEVENTH word.

       port_scene_registry_install walks this table in order and calls every
       row's fill on every boot; port_scene_mg_overlay_load runs the thirty-five
       overlay constructors ONCE PER PROCESS at the tail of the FIRST minigame
       row's fill. Appending means the constructors have already read clean ROM
       words before this fill starts, which is the latent-safe direction
       port/mg_fanout_costs.txt section 11 derives.

       AND THE 37th WORD IS THIS TABLE'S OWN, checked three ways before the
       count was written (port/slice_s371.txt section 3): the span from
       data_ov006_0213b918 to the next config symbol data_ov006_0213b9ac is
       exactly 37 words; slot 35 is this class's own func_ov006_020d1170 and
       NOT the family terminator ov004 0x020ad660, so the terminator check does
       not refuse it; and slot 36 carries a load relocation of its own,
       from:0x0213b9a8 to:0x020d1188, which is what a live pointer has and a
       phantom slot never does. Section 11's hazard is a fill reaching into
       another object's data, and the word after this table -- 0x0213b9ac,
       value 0x00000100, no relocation -- is untouched. */
    {371, "SCENE_MG_AMIDA", data_ov006_0213b814, port_mg_amida_spawn,
     port_scene_fill_amida, 0},
    /* 381 is 0x17d, spelled in decimal for the two reasons every row above
       gives: the others are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane
       S381, and appending matters twice for this class rather than once.

       It is the latent-safe direction port/mg_fanout_costs.txt section 11
       derives from the once-per-process constructor gate: this function walks
       the table in order and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors
       ONCE PER PROCESS at the tail of the FIRST minigame row's fill. And this
       class shares the dScMgSingle3DBase_c table at 0x0213e448 with the
       flower and memory2 rows above, so running after them means the flower's
       fill keeps claiming the middle table and both earlier witnesses keep
       counting exactly what they counted before this seat existed.
       hal/scene_mg_mcarlo.cpp section 3 measures it: this seat's middle copy
       reports 0 claimed slots and its derived copy reports 13 -- this class's
       own seven plus six of the middle base's eight, the other two being that
       base's D2 and D0, which slots 16 and 17 of the derived table override
       with bodies of this class's own. Measured on a 300-frame boot.

       The section-11 hazard is measured ABSENT here rather than assumed: zero
       relocations leave ov006's .init code (0x0212f4c4..0x02133600) for this
       class's code block (0x020f7634..0x020f8ef4) or its data span
       (0x0213d564..0x0213d6f4), so no constructor reads a word this fill
       writes. The width is 36 by four independent checks in
       port/slice_s381.txt, so the fill cannot reach past its own table
       either. */
    {381, "SCENE_MG_MCARLO", data_ov006_0213d580, port_mg_mcarlo_spawn,
     port_scene_fill_mcarlo, 0},
    /* 386 is 0x182, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane WIG,
       which is the rule port/mg_fanout_costs.txt section 11 derives from the
       once-per-process constructor gate: port_scene_registry_install walks this
       table in order and port_scene_mg_overlay_load runs the thirty-five
       constructors at the tail of the FIRST minigame row's fill, so a row placed
       before an earlier class's would have its fill run before those
       constructors read the mounted .data.

       FOR THIS CLASS THE ORDERING IS BELT AND BRACES RATHER THAN A REMEDY, and
       for a reason no earlier row could give: dScMgHanachan_c HAS NO OVERLAY
       CONSTRUCTOR. Not one relocation in ov006 whose source lies in a __sinit
       block lands anywhere in [0x0213c98c, 0x0213cab8], this class's whole
       .data, so there is no constructor of its own for a fill to race. Its
       fill also cannot reach past its own table: the width is 36 by five
       independent checks, the fourth being that exactly 36 relocation rows have
       a source inside the table and there is none at index 36, and the fifth
       that those sources step by 4 for exactly 36 words and then jump to 8.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {386, "SCENE_MG_WIGGLER", MgWhichWiggler_SpawnInfo, port_mg_wiggler_spawn,
     port_scene_fill_wiggler, 0},
    /* 387 is 0x183, spelled in decimal for the two reasons every row above
       gives: the others are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED AFTER EVERY EXISTING ROW, which is the
       rule port/mg_fanout_costs.txt section 11 derives from the once-per-
       process constructor gate: this function walks the table in order and
       calls every row's fill on every boot, while port_scene_mg_overlay_load
       runs the thirty-five constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill, so a row placed before an earlier class's
       would have its fill run before those constructors read the mounted
       .data.

       FOR THIS CLASS THE RULE IS OBEYED RATHER THAN RELIED ON, and that is
       measured rather than assumed. The width is 36 by all three of section
       11's checks (span to data_ov006_0213fa9c is exactly 36 words, slot 35
       holds the family's terminal 0x020ad660, and the word past the end is
       0x00000001 rather than a code address), and the fourth check says the
       hazard cannot arise at all: NO relocation anywhere in ov006 lands inside
       0x0213fa0c..0x0213fa9c except the table's own type_info word, so no
       overlay constructor reads any part of this table and there is no word
       here for a fill to clobber ahead of a copy. port/slice_boo.txt carries
       all four. */
    {387, "SCENE_MG_BOOSEEK", MgHideAndBooSeek_SpawnInfo,
     port_mg_booseek_spawn, port_scene_fill_booseek, 0},
    /* 388 is 0x184, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane LKY,
       and for this class the ordering rule earns its keep TWICE rather than
       once, which is the dScMgMemory2_c case one class further on.

       FIRST, the rule itself: port_scene_registry_install walks this table in
       order and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors ONCE
       PER PROCESS at the tail of the FIRST minigame row's fill. Appending means
       the constructors have already run against clean ROM words when this fill
       starts, which is the latent-safe direction port/mg_fanout_costs.txt
       section 11 derives. Nothing in this class's fill writes outside its own
       36-slot table -- the width is checked FOUR ways in port/slice_lky.txt --
       so this row obeys the rule rather than relying on it.

       SECOND, this class is the THIRD to sit under dScMgSingle3DBase_c
       (data_ov006_0213e448), after the flower row and the memory2 row above.
       All three files define their own face array over the same eight DS words
       and the fill keys on a DS address, so the row that runs FIRST claims the
       middle table and the later ones find nothing left to write. Appending
       means the flower keeps it and BOTH earlier witnesses keep counting
       exactly what they counted before this seat existed;
       hal/scene_mg_luckystars.cpp section 3 prints this seat's own middle-table
       claim count so the zero is measured rather than assumed. That file also
       records the seam promotion scene_mg_memory2.cpp says is due at the third
       class, and why this lane did not take it mid-fan-out. */
    {388, "SCENE_MG_LUCKYSTARS", MgLuckyStars_SpawnInfo,
     port_mg_luckystars_spawn, port_scene_fill_luckystars, 0},
    /* 389 is 0x185, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg9 lane PSY,
       and for this class the ordering rule carries the SECOND reason as well as
       the first.

       The first is the once-per-process constructor gate
       port/mg_fanout_costs.txt section 11 derives: this function walks the
       table in order and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors ONCE
       PER PROCESS at the tail of the FIRST minigame row's fill, so a row placed
       earlier would have its fill run before those constructors read the
       mounted .data. Nothing in this class's fill writes outside its own
       36-slot table -- the width is checked FOUR ways in port/slice_psy.txt --
       so appending is a rule this lane obeys rather than a hazard it needs.

       The second is dScMgSingle3DBase_c. This is the THIRD class the port seats
       under data_ov006_0213e448, after the flower row and the memory2 row, and
       all three define their own face array over the same eight DS words.
       psy_apply keys on a DS address, so the fill that runs first claims the
       middle table and the ones after it write nothing there. Appending after
       both means the flower keeps that table and its witness is unchanged, and
       this seat owns only its own derived table.
       hal/scene_mg_psycheout.cpp section 3 is the argument, and the seat prints
       the claimed counts so the split is measured rather than assumed.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {389, "SCENE_MG_ESP3D", MgPsycheOut_SpawnInfo, port_mg_esp3d_spawn,
     port_scene_fill_esp3d, 0},
    /* 365 is 0x16d, spelled in decimal for the two reasons every row above
       gives: the others are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg11 lane
       MUG, and for this class the position is load-bearing twice.

       ONE: it is the latent-safe direction port/mg_fanout_costs.txt section 11
       derives from the once-per-process constructor gate.
       port_scene_registry_install walks this table in TABLE ORDER and calls
       every row's fill on every boot, while port_scene_mg_overlay_load runs
       the thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill. Appending means the constructors have already
       run against clean ROM words when this fill starts -- and for THIS class
       one of those constructors is its own: __sinit_ov006_02132894 copies the
       eight member-pointer pairs of its state machine out of the .data span
       0x0213e4e0..0x0213e588 into data_ov006_02142bdc. Nothing this fill
       writes is inside that span; its whole reach is [0x0213eaa8, 0x0213eb38).

       TWO: this class is the NINTH seated under dScMgSingle3DBase_c
       (0x0213e448). EIGHT were seated at base -- the 361, 362, 363, 367, 381,
       388, 389 and 390 rows -- so running last means the earlier fill keeps
       the middle table and every earlier seat's middle-table witness keeps
       counting exactly what it counted before this row existed. (The count is
       measured in port/slice_mug.txt by sweeping every ov006 vtable's typeinfo
       base word, not inherited from a neighbour's header; this comment said
       FIFTH until review and was wrong by four.) hal/scene_mg_slot3.cpp
       section 3 is the
       argument and this seat prints its claimed counts so the split is
       measured rather than assumed.

       SECTION 11's HAZARD IS LIVE FOR THIS CLASS AND IS MEASURED ABSENT. The
       span from data_ov006_0213eaa8 to the next config symbol is THIRTY-EIGHT
       words, not 36, and the two extra words are the {0, &typeinfo} RTTI
       header of data_ov006_0213eb40 -- dScMgSlot1_c's vtable, the
       SCENE_MG_SLOT1 row twelve lines above, which ships today. A 38-slot fill
       would write a host thunk over scene 364's typeinfo pointer. The fill is
       called with 36 and FIVE independent checks agree on that width
       (port/slice_mug.txt); this lane's sibling canary is a scene-364 boot.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {365, "SCENE_MG_SLOT3", data_ov006_0213e508, port_mg_slot3_spawn,
     port_scene_fill_slot3, 0},
    /* 372 is 0x174, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg11 lane BNP,
       and for this class the ordering rule earns its keep twice.

       FIRST, the rule itself: port_scene_registry_install walks this table in
       order and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors ONCE
       PER PROCESS at the tail of the FIRST minigame row's fill. Appending means
       the constructors have already run against clean ROM words when this fill
       starts, which is the latent-safe direction port/mg_fanout_costs.txt
       section 11 derives. For this class the hazard is measured ABSENT rather
       than assumed: ZERO relocations leave ov006's .init range
       (0x0212f4c4..0x0213356c) for this class's data span
       (0x0213cb48..0x0213cc74), so this class has no overlay constructor at all
       and there is no word here for a fill to clobber ahead of a copy. The
       width is 36 by all five checks in port/slice_bnp.txt, so the fill cannot
       reach past its own table either -- which matters more than usual here,
       because the word past the end is id 0x175's FIRST STATE PAIR.

       SECOND, this is the FIRST of the four dScMgD3DBase_c classes the port has
       hosted. hal/scene_mg_jump.cpp fills the shared base table at 0x0213c62c
       as well as this class's own, mg_apply keys on a DS address, and the fill
       that runs first claims that table. So this row claims it and the seats
       for 0x175, 0x180 and 0x181 will find it already claimed -- which is
       correct, and their base-table claim counts reading zero is the evidence
       that nothing was filled twice. The seat prints both claim counts so the
       split is measured rather than assumed.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {372, "SCENE_MG_JUMP", MgBounceAndPounce_SpawnInfo, port_mg_jump_spawn,
     port_scene_fill_jump, 0},
    /* APPENDED AFTER EVERY EXISTING ROW, run mg11 lane BNT. 373 is 0x175,
       spelled in decimal for the two reasons every row above is: the others
       are, and port/tools/battery.py reads its hosted-scene set out of this
       table. The position is load-bearing twice for this class, the way it is
       for the memory2 and lucky-stars rows.

       The first is the once-per-process constructor gate section 11 of
       port/mg_fanout_costs.txt derives: this function walks the table in order
       and calls every row's fill on every boot, while port_scene_mg_overlay_load
       runs the thirty-five overlay constructors ONCE PER PROCESS at the tail of
       the FIRST minigame row's fill, so appending means the constructors have
       already run against clean ROM words when this fill starts. Nothing in
       this class's fill writes outside its own 36-slot tables -- both widths
       are checked in port/slice_bnt.txt section 4 -- so appending is a rule
       this lane obeys rather than a hazard it needs.

       The second is dScMgD3DBase_c, the intermediate base at
       data_ov006_0213c62c (which the config misnames _ZTV17MgBounceAndPounce).
       FOUR ids derive from it -- 0x174, 0x175, 0x180 and 0x181 -- and this
       wave seats the other three concurrently. Each seat defines its own
       seventeen-row array over the same seventeen DS words; jump2_apply keys
       on a DS address, so the fill that runs FIRST claims the middle table and
       the ones after it write nothing there. Appending after 0x174's row (which
       merges first, in id order) means that row keeps the middle table and this
       seat owns only its own derived table. hal/scene_mg_jump2.cpp section 3 is
       the argument, and the seat PRINTS the claimed count so the split is
       measured rather than assumed. */
    {373, "SCENE_MG_JUMP2", MgBounceAndTrounce_SpawnInfo, port_mg_jump2_spawn,
     port_scene_fill_jump2, 0},
    /* 377 is 0x179, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table.  APPENDED AFTER EVERY EXISTING ROW, run mg11 lane SNW,
       and the position is load-bearing twice, the dScMgBSC_c case one class
       further on.

       FIRST, the rule itself: port_scene_registry_install walks this table in
       order and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors ONCE
       PER PROCESS at the tail of the FIRST minigame row's fill.  Appending
       means the constructors have already run against clean ROM words when this
       fill starts, which is the latent-safe direction
       port/mg_fanout_costs.txt section 11 derives.  Nothing in this class's
       fill writes outside its own 36-slot table -- the width is checked FIVE
       ways in port/slice_snw.txt, and the word a thirty-seventh slot would take
       is the filename string this class's own InitResources loads -- so this
       row obeys the rule rather than relying on it.

       SECOND, this class is the FOURTH to sit under dScMgSingle3DBase_c
       (data_ov006_0213e448), after the flower row, the memory2 row and the
       luckystars row.  All four files define their own face array over the same
       eight DS words and the fill keys on a DS address, so the row that runs
       FIRST claims the middle table and the later ones find nothing left to
       write.  Appending means the flower keeps it and all three earlier
       witnesses keep counting exactly what they counted before this seat
       existed; hal/scene_mg_snowball.cpp section 6 prints this seat's own
       middle-table claim count so the zero is measured rather than assumed.
       That file also records why the kSingle3DFaces seam promotion -- due since
       the third class -- is still not the thing to take mid-fan-out. */
    {377, "SCENE_MG_SNOWBALL", MgSnowballSlalom_SpawnInfo,
     port_mg_snowball_spawn, port_scene_fill_snowball, 0},
    /* APPENDED AFTER EVERY EXISTING ROW, run mg11 lane PKR. 379 is 0x17b,
       spelled in decimal for the two reasons every row above gives: the others
       are, and port/tools/battery.py reads its hosted-scene set out of this
       table. Appending matters twice for this class, the way it does for the
       memory2, luckystars and psycheout rows.

       The first is port/mg_fanout_costs.txt section 11's latent-safe direction:
       port_scene_registry_install walks this table in order and calls every
       row's fill on every boot, while port_scene_mg_overlay_load runs the
       thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill, so a row placed earlier would have its fill
       run before those constructors read the mounted .data.

       The second is dScMgSingle3DBase_c. This is the FOURTH class the port
       seats under data_ov006_0213e448, after the flower, memory2 and
       luckystars rows, and all four define their own face array over the same
       eight DS words. card_apply keys on a DS address, so the fill that runs
       FIRST claims the middle table and the ones after it write nothing there.
       Appending after all three means the flower keeps that table and its
       witness is unchanged, and this seat owns only its own derived table --
       plus the two ELEMENT vtables at 0x0213bccc and 0x0213bcf4, which are this
       class's alone and which no other row touches.

       Nothing in this class's fill writes outside its own 36-slot table: the
       width is checked FIVE ways in port/slice_pkr.txt, and the word a
       thirty-seventh slot would take is dScMgCoin_c's live state pair -- the
       SEATED scene-378 row eight lines up this file.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {379, "SCENE_MG_CARD", MgPicturePoker_SpawnInfo, port_mg_card_spawn,
     port_scene_fill_card, 0},
    /* 382 is 0x17e, spelled in decimal for the two reasons every row above
       gives: the others are, and port/tools/battery.py reads its hosted-scene
       set out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg11 lane
       PGO, and appending matters twice for this class rather than once.

       It is the latent-safe direction port/mg_fanout_costs.txt section 11
       derives from the once-per-process constructor gate: this function walks
       the table in order and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors
       ONCE PER PROCESS at the tail of the FIRST minigame row's fill. And this
       class is the SIXTH to sit under dScMgSingle3DBase_c (0x0213e448), after
       the flower, memory2, luckystars, memory and mcarlo rows above, so running
       after them means the flower's fill keeps claiming the middle table and
       every earlier witness keeps counting exactly what it counted before this
       seat existed. hal/scene_mg_mcarlo2.cpp section 3 measures it: this seat's
       middle copy reports 0 claimed slots and its derived copy reports 14 --
       this class's own eight plus six of the middle base's eight, the other two
       being that base's D2 and D0, which slots 16 and 17 of the derived table
       override with bodies of this class's own.

       THE ROW ORDER IS BELT AND BRACES HERE AND THE WIDTH IS THE REAL GUARD.
       Zero relocations leave ov006's .init (0x0212f4c4..0x0213356c) for this
       class's code block (0x020f8ef4..0x020fa75c), so no constructor reads a
       word this fill writes -- but the twenty-five member-pointer pairs that
       .init DOES copy begin at 0x0213d878, the word immediately past this
       table, and they belong to MgBobOmbSquad. The width is 36 by five
       independent checks in port/slice_pgo.txt, of which the third (what the
       word past the end is) is FOOLED for this class: index 36 reads
       0x020fb4e0, a real code address. A 37-slot fill would have put a host
       thunk over the first state of a different minigame. */
    {382, "SCENE_MG_MCARLO2", MgPairAGoneAndOn_SpawnInfo, port_mg_mcarlo2_spawn,
     port_scene_fill_mcarlo2, 0},
    /* 383 is 0x17f, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg11 lane RLT,
       and the ordering rule earns its keep twice for this class as it does for
       the luckystars row.

       FIRST, the once-per-process constructor gate port/mg_fanout_costs.txt
       section 11 derives: this function walks the table in order and calls
       every row's fill on every boot, while port_scene_mg_overlay_load runs the
       thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill, so a row placed earlier would have its fill
       run before those constructors read the mounted .data. HERE THE HAZARD IS
       MEASURED ABSENT rather than avoided: not one relocation whose source lies
       in ov006's .init range (0x0212f4c4..0x0213356c) lands anywhere in this
       class's code block (0x0210788c..0x0210a4ac) or its data span
       (0x0213e2dc..0x0213e42c), so no constructor reads a word this fill
       writes. Nothing in the fill writes outside its own 36-slot table either
       -- the width is checked FOUR ways in port/slice_rlt.txt, the fourth being
       that exactly 36 relocation rows have a source inside the table and there
       is none at index 36.

       SECOND, dScMgSingle3DBase_c. This is the FIFTH class the port seats under
       data_ov006_0213e448, after the flower, memory2, luckystars and mcarlo
       rows, and all five define their own face array over the same eight DS
       words. rlt_apply keys on a DS address, so the fill that runs first claims
       the middle table and the ones after it write nothing there. Appending
       after all four means the flower keeps that table and every earlier
       witness is unchanged; hal/scene_mg_roulette.cpp section 3 prints this
       seat's own claimed counts so the split is measured rather than assumed.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {383, "SCENE_MG_ROULETTE", MgMushroomRoulette_SpawnInfo,
     port_mg_roulette_spawn, port_scene_fill_roulette, 0},
    /* APPENDED AFTER EVERY EXISTING ROW, run mg11 lane TTI. 384 is 0x180,
       spelled in decimal for the two reasons every row above is: the others
       are, and port/tools/battery.py reads its hosted-scene set out of this
       table.

       THE POSITION IS THE FILL-ORDER RULE AND IT MATTERS TWICE HERE.
       port_scene_registry_install walks this table in TABLE ORDER and calls
       every row's fill on every boot, while port_scene_mg_overlay_load runs the
       thirty-five overlay constructors ONCE PER PROCESS at the tail of the
       FIRST minigame row's fill. Appending means the constructors have already
       run against clean ROM words when this fill starts, which is the
       latent-safe direction port/mg_fanout_costs.txt section 11 argues for.

       And this is the FIRST class the port seats under dScMgD3DBase_c
       (data_ov006_0213c62c), a middle base that four ov006 classes share --
       0x174, 0x175, 0x180 and 0x181. tti_apply keys on a DS address, so the
       fill that runs first claims that table and any later one writes nothing
       there. On this tree this row is the only one under that base, so it
       claims it; hal/scene_mg_trampoline.cpp prints the claimed count so a
       tree that later carries a sibling row says out loud which lane's thunks
       the middle table is carrying, rather than leaving it inferred.

       THE WIDTH IS 36 AND A 37TH SLOT WOULD LAND ON THE SIBLING. Index 36 of
       data_ov006_0213fb34 is 0x0213fbc4, which is dScMgTrampoline2_c's own int
       flag -- its slot-23 body loads it from 0x021230bc, the mirror of this
       class's slot 23 loading data_ov006_0213fa9c. All five width checks are in
       port/slice_tti.txt and the fill is called with 36.

       The class name is the ROM's own type_info, reached the way the panel and
       Psyche Out rows reach theirs: the word BEFORE data_ov006_0213fb34 is
       0x0213fad0, and that record's name pointer 0x0213faf0 reads
       "17dScMgTrampoline_c", while its base pointer 0x0213c5c8 names
       "14dScMgD3DBase_c". The spawn symbol MgTrampolineTime carries the player
       title, so the row is named for the class the way SCENE_MG_CURLING,
       SCENE_MG_LUIGI and SCENE_MG_PANEL are.

       The ov005 launch table gives this scene exactly ONE row (row 12), so
       port_mg_scene_spawn_param's first-match rule is unambiguous here; its
       +0x04 word is 0x000c0c00 = save record 12, name text 12.

       reads_sublevel is 0 for the curling row's reason, re-checked for this
       class rather than copied: no relocation anywhere in ov006 lands on
       data_02092110 and no TU in this class's closure names it. */
    {384, "SCENE_MG_TRAMPOLINE", MgTrampolineTime_SpawnInfo,
     port_mg_trampoline_spawn, port_scene_fill_trampoline, 0},
    /* 385 is 0x181, spelled in decimal for the same two reasons every row above
       is: the others are, and port/tools/battery.py reads its hosted-scene set
       out of this table. APPENDED AFTER EVERY EXISTING ROW, run mg11 lane TTE,
       and for this class the ordering rule earns its keep twice.

       FIRST, the rule itself: port_scene_registry_install walks this table in
       TABLE ORDER and calls every row's fill on every boot, while
       port_scene_mg_overlay_load runs the thirty-five overlay constructors ONCE
       PER PROCESS at the tail of the FIRST minigame row's fill. Appending means
       the constructors have already run against clean ROM words when this fill
       starts, which is the latent-safe direction port/mg_fanout_costs.txt
       section 11 derives.

       AND FOR THIS CLASS THE HAZARD IS MEASURED ABSENT RATHER THAN INHERITED.
       dScMgTrampoline2_c HAS NO OVERLAY CONSTRUCTOR: zero relocations whose
       source lies in ov006's .init block (0x0212f4c4..0x02133600) land anywhere
       in its whole .data span 0x0213fbc8..0x0213fd0c. Its five state pairs are
       reached from literal pools inside its own bodies, not copied into a .bss
       table by a __sinit, so there is no constructor of its own for a fill to
       race. hal/scene_mg_trampoline2.cpp section 6 is the measurement.

       SECOND, this is the FIRST class the port seats under dScMgD3DBase_c, the
       middle base at 0x0213c62c that config/arm9/overlays/ov006/symbols.txt
       names _ZTV17MgBounceAndPounce after actor 0x174's spawn symbol. Four
       classes hold it -- 0x174, 0x175, 0x180 and this one -- and the fill keys
       on a DS address, so the row that runs FIRST claims that table. On this
       branch that is this row; when run mg11's other three land ahead of it in
       id order the 0x174 row claims it instead and this seat owns only its own
       derived table. The census prints its own middle-table claim count so the
       change is measured rather than assumed.

       THE WIDTH IS 36 BY FIVE CHECKS and the victim of a 37th is named and
       SHIPPING: index 36 of data_ov006_0213fc7c is 0x02124ae4, which is the
       code half of data_ov006_0213fd0c -- the FIRST of dScMgBSC_c's fourteen
       state pairs, the class the SCENE_MG_LUCKYSTARS row above seats. A 37-slot
       fill here would write a host thunk over a live state of scene 388.
       port/slice_tte.txt section 3 carries all five checks.

       reads_sublevel is 0 for the curling row's reason, re-derived rather than
       copied: no relocation anywhere in ov006 lands on data_02092110 and no TU
       in this class's closure names it. A minigame is not about a course. */
    {385, "SCENE_MG_TRAMPOLINE2", MgTrampolineTerror_SpawnInfo,
     port_mg_trampoline2_spawn, port_scene_fill_trampoline2, 0},
    {0, 0, 0, 0, 0, 0},
};

extern "C" const char *port_scene_class_name(unsigned id)
{
    for (const PortSceneClass *k = port_scene_classes; k->name; ++k)
        if (k->id == id)
            return k->name;
    return "?";
}

extern "C" void port_scene_registry_install(void)
{
    int n = 0;
    for (const PortSceneClass *k = port_scene_classes; k->name; ++k) {
        unsigned rec = *(unsigned short *)(k->info + 4);
        if (rec != k->id) {
            std::fprintf(stderr, "  [scene] %s: SpawnInfo at %p says id %u, "
                         "the spawn table says %u -- WRONG RECORD\n", k->name,
                         (void *)k->info, rec, k->id);
            continue;
        }
        *(void **)(k->info + 0) = (void *)k->factory;
        data_020a4bb8[k->id] = k->info;
        k->fill();
        ++n;
    }
    std::printf("[scene] %d scene classes registered (ov003, ov007, ov006)\n",
                n);
}

/* Does the scene the run is booting read data_02092110? Unknown ids answer 0,
   which is the conservative direction: an id with no row does not spawn. */
static int scene_reads_sublevel(int id)
{
    for (const PortSceneClass *k = port_scene_classes; k->name; ++k)
        if (k->id == (unsigned)id)
            return k->reads_sublevel != 0;
    return 0;
}

/* hal/model_host.cpp's window-register census, read by the irq2 witness at the
   end of port_scene_run. */
extern "C" void port_window_copy_count(unsigned long long *copies,
                                       unsigned long long *bytes,
                                       unsigned *last_word,
                                       unsigned long long *distinct);

// ---- the boot ---------------------------------------------------------------
//
// SM64DS_SCENE=<id> mirrors SM64DS_LEVEL: it names what to boot and nothing
// else. port_scene_env_want returns -1 when it is unset, which is the level
// harness's "not a scene run".
extern "C" int port_scene_env_want(void)
{
    static int want = -2;
    if (want != -2)
        return want;
    const char *e = std::getenv("SM64DS_SCENE");
    want = e ? std::atoi(e) : -1;
    return want;
}

/* THE SAME QUESTION port_scene_boot BELOW REFUSES ON, asked before the run is
   committed instead of after it has been declined.

   The debug menu's minigame picker (port/tests/walk_window.cpp) is the caller.
   It lists all thirty ids the ROM's own IsMinigameActorID accepts and reads
   this to decide which of them can be selected, so there is ONE hosted-scene
   list in the port and the picker cannot drift from it: seating a new scene
   lights its row up with no edit to the menu.

   data_020a4bb8 is the ROM's spawn table and port_scene_registry_install
   writes each hosted row into it on EVERY boot, level runs included
   (hal/level_boot.cpp's port_a2_seat_body), so this answers correctly from
   inside a level as well as from a scene run. */
extern "C" int port_scene_is_hosted(int id)
{
    return id >= 0 && id < 512 && data_020a4bb8[id] != 0;
}

/* Boot the scene. Two calls into matched arm9 and a report; the port does not
   spawn anything itself and does not touch data_02092664 by hand. Returns the
   scene object the ROM's spine made, or null. */
extern "C" void *port_scene_boot(int id)
{
    if (id < 0 || id >= 512 || !data_020a4bb8[id]) {
        std::fprintf(stderr, "FATAL: scene %d is not a hosted scene. Hosted:",
                     id);
        for (const PortSceneClass *k = port_scene_classes; k->name; ++k)
            std::fprintf(stderr, " %u (%s)", k->id, k->name);
        std::fprintf(stderr, "\n");
        return 0;
    }
    std::printf("[scene] %d = %s\n", id, port_scene_class_name((unsigned)id));

    /* THE SPAWN PARAMETER, and why it was 0 for six waves of minigame lanes.
       Run mg8, lane MMD.

       This call used to pass 0. On the DS nothing ever spawns a minigame scene
       with 0 unless the player picked the first row of the Rec Room: the launch
       path is func_ov005_020c0378's

           Scene::StartSceneFade(row->id, row->unk4, 0)
             -> Scene::SetSceneToSpawn(id, param)

       and dScMgBase_c's constructor unpacks that param into TWO indices --
       (param>>8)&0xff picks the row of data_0209caf4, the persistent
       per-minigame save record, and (param>>16)&0xff picks the row of
       data_ov004_020bc070, the name-text id it parks at +0x465e. With 0 all
       thirty minigames shared record 0 and asked for text 0.

       THE RECORD IS NOT COSMETIC ON THIS FAMILY. func_ov004_020ad878 reads
       field 1 of it, dScMgMemory2_c's InitResources copies that into +0xb4, and
       func_ov006_020f72c0 turns +0xb4 into the board size: 16 cards under 5,
       18 at 5, 20 at 10. Memory Master keeping its clear count in Bob-omb
       Squad's slot is the shape of that defect.

       The value is the ov005 row's own +0x04 word, read out of the verbatim
       table hal/scene_mg_sound.cpp already carries -- the same table, the same
       first-match-by-id rule, and 0 for anything that is not a minigame scene,
       which leaves scenes 1 and 4 exactly where they were. */
    const unsigned param = port_mg_scene_spawn_param(id);
    if (param)
        std::printf("[scene] spawn param %08x: save record %u, name text %u\n",
                    param, (param >> 8) & 0xffu, (param >> 16) & 0xffu);

    /* THE ROM'S OWN TWO CALLS. SetSceneToSpawn parks the id; SpawnIfNecessary
       runs the spine. data_02092660 is the "already spawned" latch and starts
       zeroed, so the second call takes its spawning branch. */
    _ZN5Scene15SetSceneToSpawnEjj((unsigned)id, param);
    const int r = _ZN5Scene16SpawnIfNecessaryEv();
    if (!r) {
        std::fprintf(stderr, "  [scene] Scene::SpawnIfNecessary declined "
                     "(pending id %u, latch %u)\n",
                     (unsigned)data_02092664, (unsigned)data_02092660);
        return 0;
    }
    std::printf("[scene] spawned %p, vptr %p\n", (void *)(size_t)r,
                *(void **)(size_t)r);
    return (void *)(size_t)r;
}

// ---- the run ---------------------------------------------------------------
//
//   SM64DS_SCENE=<id>            which scene. TWO are seated: 4 (dScStarSel_c,
//                                the star select, ov003) and 1 (dScDSMT_c, the
//                                title screen and file select, ov007). ov003's
//                                other two ids, 2 and 8, are refused by name,
//                                and so is anything else. Nothing here runs
//                                unless this is set.
//   SM64DS_SCENE_FRAMES=<n>      how many frames (default 300, the battery's).
//   SM64DS_SCENE_BMP=<path>      write the last frame.
//   SM64DS_SCENE_NO_RENDER=1     tick only, no render bucket. The A/B that
//                                separates a Behavior fault from a Render one.
//   SM64DS_SCENE_TRACE=1         name each render sub-step on stderr before
//                                it runs, so a HANG can be attributed. A
//                                fault already names itself through the fault
//                                probe; a hang leaves nothing behind without
//                                this.
//   SM64DS_SCENE_SUBLEVEL=<n>    which sublevel the scene is ABOUT. Default 6.
//   SM64DS_SCENE_SLOT9=0         leave the scene's Render slot on a no-op.
//   SM64DS_SCENE_SLOT0=0         scene 1 only: leave InitResources on a no-op.
//                                A DIAGNOSTIC AND NOT A SKIP -- it separates
//                                "InitResources faults" from "the object is
//                                unusable without it", and it proves the
//                                second, so port/tools/battery.py refuses it
//                                as a SCENE_SKIPS env. port/ov007_seat.txt
//                                section 5 has the measurement.
//
// ---- SM64DS_SCENE_SUBLEVEL, and why a default of 0 is not an option --------
//
// dScStarSel_c is a scene ABOUT a course: every branch in its InitResources,
// its Behavior and its Render is gated on SublevelToLevel(data_02092110), the
// arm9 SUBLEVEL_LEVEL_TABLE (0x02075298) lookup that turns the current
// sublevel into a course number. The ROM never reaches this scene without one
// -- you pick a course, the game latches the sublevel, and the star select
// comes up for it -- so the sublevel is an INPUT to the scene the way
// SM64DS_LEVEL is an input to a level boot, and the harness has to supply it.
//
// A run that does not is not neutral, it is invalid, and this cost the lane a
// hang before the reason was found. data_02092110 STARTS AT -1 (corrected by
// lane L2; the sentence here read "starts at 0" and the derivation for the
// real value is in port_scene_run below, next to the gate). Feeding -1 to
// SublevelToLevel indexes SUBLEVEL_LEVEL_TABLE one byte BELOW the table.
// SUBLEVEL_LEVEL_TABLE[0] is separately 0xff -- read back through its own
// declaration
// (include/decl_common.h: `extern signed char SUBLEVEL_LEVEL_TABLE[]`) that is
// -1, the "not a level" sentinel. Every `< 0xF` and `<= 0xE` gate in the scene
// then passes, so InitResources' star block (guarded by an UNSIGNED
// `(u32)level <= 0xE`, which 0xff fails) never runs and never writes the star
// count at +0x114, while Render's loops (guarded by the SIGNED compare, which
// -1 passes) walk that uninitialised count. Measured: the run hangs inside
// dScStarSel_c::Render on frame 0, with slot 9 no-op'd it completes, and the
// count at +0x114 is whatever the heap left there.
//
// The default is 6, the first main course (SUBLEVEL_LEVEL_TABLE[6] = 0,
// Bob-omb Battlefield), because it is the smallest real answer: course 0 with
// a zeroed save gives a one-entry star grid.
//
// Returns the process exit code. Called from walk_window's main once the host
// bring-up is done, and it does not return to the level path.
//
// ---- IT IS THREE FUNCTIONS, AND THE HEADLESS RUN IS THEIR COMPOSITION ------
//
// run link60 lane SW1. port_scene_run at the bottom of this block is
//
//     begin(nullptr, 1);  tick(frame, 1) per frame;  finish(frames);
//
// -- the same statements in the same order the one function had, so a headless
// scene run executes what it always executed. The split is a split and not a
// rewrite for exactly that reason: the battery's scene rows are the proof the
// rest of the lane is measured against, so they must not be measuring
// something new.
//
// What it buys is a WINDOW. tests/walk_window.cpp's scene_window_run drives
// the same three calls with a message pump, live input, the debug menu and a
// present between the ticks, and it needs no copy of the scene's bring-up, its
// capture or its census. The scene's frame belongs to this file and the host's
// frame belongs to that one, which is the division the level path already has.
//
// WHO GETS A WINDOW is walk_window's decision and not this file's: a scene run
// that names a frame budget or is a selftest is a MEASUREMENT and stays as
// headless as it ever was, and everything else is a SESSION. See
// port/scene_window.txt section 3 for why the frame budget is the signal.

/* What begin() resolved, for tick() and finish() to read. File scope rather
   than arguments because they are the RUN's settings and not the FRAME's. */
static int scn_frames = -1;
static int scn_no_render;
static int scn_trace;
static const char *scn_bmp;
static const char *scn_bmp_stacked;
/* The scene's framebuffer, moved out of port_scene_run's body unchanged: the
   same zero-initialised static storage, now reachable by a windowed loop that
   has to present it. */
static ntr::Framebuffer scn_fb;

/* THE LAYOUT, PROPOSED FROM THE ROM'S OWN PREDICATE. src/IsMinigameActorID.c
   is `id >= 0x169 && id <= 0x186` and it is already what gates the ov006
   overlay constructors (hal/scene_mg.cpp's port_scene_mg_prepare), so the
   question "is this scene a minigame" has a linked, ROM-side answer and this
   file does not invent a second one. Curling is 374 = 0x176 and is inside it;
   the star select (4) and the title screen (1) are not.

   IT IS A PROPOSAL. hal_sub_screen_set_stacked takes it as the default and
   SM64DS_DUAL_SCREEN overrides in either direction.

   ASKED ONCE, BY WHOEVER GETS THERE FIRST. The mode latches on its first
   reader and the setter refuses -- loudly, on purpose -- once the answer has
   been handed out. A windowed run has to know the layout BEFORE it can size a
   window, which is before begin() runs, so the proposal is made exactly once
   and the second caller is a no-op rather than a warning about a call order
   that is in fact correct. */
extern "C" void port_scene_layout_propose(void)
{
    static int proposed;
    if (proposed)
        return;
    proposed = 1;
    const int scene = port_scene_env_want();
    hal_sub_screen_set_stacked(scene >= 0 && IsMinigameActorID((unsigned)scene));
}

/* How many frames the run was asked for, readable before begin() so a windowed
   loop can honour a budget it was given. */
extern "C" int port_scene_frames_wanted(void)
{
    if (scn_frames < 0)
        scn_frames = std::getenv("SM64DS_SCENE_FRAMES")
                         ? std::atoi(std::getenv("SM64DS_SCENE_FRAMES")) : 300;
    return scn_frames;
}

/* The finished frame, for a present. Const because the windowed loop reads it
   and must not become a second writer of it. */
extern "C" const void *port_scene_framebuffer(void)
{
    return &scn_fb;
}

/* THE BRING-UP AND THE SPAWN. Returns 0, or the process exit code to die with.
   `hwnd` is the window this run will be presented into, or null for a headless
   one: hal_sub_screen_init_hw keys g_headless off exactly that, so passing a
   real window is the whole of what turns the stylus, the focus gate and the
   TAB latch on. See the block below it. */
extern "C" int port_scene_begin(void *hwnd, int zoom)
{
    const int scene = port_scene_env_want();
    const int frames = port_scene_frames_wanted();
    const int no_render = std::getenv("SM64DS_SCENE_NO_RENDER") != 0;
    const char *bmp = std::getenv("SM64DS_SCENE_BMP");
    /* SM64DS_SCENE_BMP_STACKED=<path>: the STACKED image of the last frame,
       both screens in one file at ntr::STACK_W x STACK_H. A SEPARATE
       ENVIRONMENT VARIABLE AND A SEPARATE FILE, deliberately: SM64DS_SCENE_BMP
       is 512x384 in both layouts and port/tools/battery.py's md5 rows are
       taken over that geometry, so a stacked capture is a new artifact asked
       for by name rather than the old artifact changing shape underneath the
       battery. Nothing in CI sets this. */
    const char *bmp_stacked = std::getenv("SM64DS_SCENE_BMP_STACKED");
    const int trace = std::getenv("SM64DS_SCENE_TRACE") != 0;
    const char *sub = std::getenv("SM64DS_SCENE_SUBLEVEL");
    const int sublevel = sub ? std::atoi(sub) : 6;

    scn_frames = frames;
    scn_no_render = no_render;
    scn_bmp = bmp;
    scn_bmp_stacked = bmp_stacked;
    scn_trace = trace;

    port_scene_layout_propose();

    /* THE DS'S POWER-ON INTERRUPT STATE, before anything can arm an interrupt.
       The ROM's own arming code brackets SetIRQHandler in
       `saved = IME; IME = 0; ... ; if (saved) IME = 1`, so a host that boots
       with IME at zero comes out of the bracket with IME STILL ZERO and the
       interrupt it just armed can never be delivered. Only ntr::rt_run used
       to seat it, and no walk_window path runs on that fiber. What it stands
       in for is src/func_0201a054.c, the game's own IRQ init, which is in no
       slice. See port/irq2_map.txt section 2. */
    ntr::rt_irq_boot_state();

    /* THE MINIGAME RNG SEED (run mg5, lane RNGSEED). Frozen randomness: the
       launcher's F5 boots straight into SM64DS_SCENE=<id> and so never runs
       dScMiniGm_c, the minigame MENU scene, whose per-frame tick
       func_ov005_020c14a0 is the ONLY thing that advances data_0209d4b8 on the
       way in. With no menu frames the state sits at the port's .bss zero and
       every launch replays one sequence -- which is why "Loves Me...?" always
       opened with 12 petals.

       HERE BECAUSE THIS IS WHERE BOTH FACTS ARE KNOWN AND NOTHING HAS DRAWN
       YET. `hwnd` is the windowed/headless answer port_scene_want_window
       already computed (a real window or nullptr), the frame budget is read
       just above, and the scene's own InitResources -- which is what calls the
       petal layout -- is not reached until the spawn, well below. The seeder
       gates itself on the ROM's IsMinigameActorID, so a non-minigame scene is
       a no-op and the level path never reaches this function at all.

       hal/scene_mg.cpp carries the derivation, the ROM addresses and the
       gating rule in full. */
    port_scene_mg_seed_rng(scene, hwnd != nullptr);

    /* The seat, minus the Stage. Everything in it -- the message archive, the
       registry and its gate, the five processing-list callbacks, the model and
       heap vtable seats -- is bring-up a scene needs exactly as much as a level
       does. The one thing a scene must not get is a Stage actor: on the DS the
       Stage IS the level scene (ACTOR_SPAWN_TABLE[3], ids 3/6/7 -> ov002), and
       two scene roots is not a state the game can be in. */
    port_scene_a2_seat();

    /* THE BOTTOM SCREEN, and the same argument the Stage seat above makes.
     *
     * The frame loop below already calls hal_sub_screen_frame_begin and
     * hal_sub_screen_present every frame, but NOTHING had ever armed them on
     * this path: walk_window's hal_sub_screen_init sits ~700 lines below the
     * `return port_scene_run()` that got us here, so a scene run reached the
     * per-frame half of the bottom screen and never the bring-up half. The
     * visible result was a bottom screen of flat backdrop.
     *
     * ONLY THE SHARED HALF, because a scene owns its own sub display and this
     * one has already configured it -- see the seam note in hal/sub_screen.cpp
     * for the ROM derivation. What was actually missing is OAM::EnableSubOAM:
     * the port pins data_0209e660 = 1 in hal/model_host.cpp, which routes every
     * sub sprite into the main shadow and stops engine B's OAM being uploaded
     * at all, and clearing it back to the DS's bss zero is what turns the panel
     * into a screen the game can draw on.
     *
     * hwnd IS THE ARGUMENT NOW, and it is the whole of the difference between
     * a headless scene run and a playable one. It used to be a hardcoded
     * nullptr, correct at the time -- walk_window created its window ~560
     * lines after the handover, so a scene run had none to pass -- and every
     * consumer of g_hwnd was written to fail open on it (hal_window_focused
     * returns 1, poll_touch's ScreenToClient guard fails and no touch is
     * delivered), which is the same state a headless selftest runs in.
     *
     * That fail-open is also what kept the STYLUS shut. hal_sub_screen_init_hw
     * sets `g_headless = SM64DS_WINDOW_SELFTEST || hwnd == nullptr`, and
     * poll_touch -- which hal_sub_screen_frame_begin has been calling on this
     * path every frame all along -- is gated on it. So a real window here
     * turns on the mouse-as-stylus, the focus gate and the TAB latch, with no
     * touch code written anywhere: the stacked branch of poll_touch is the
     * mapping the bottom half of the picture already had. A headless run still
     * passes nullptr and is unchanged. See port/scene_window.txt section 1c.
     *
     * SM64DS_SUB_NO_SCENE_INIT=1 puts the old behaviour back on this same
     * binary. It exists so the before/after is one build and one .dsstate base
     * -- notes/port-selftest-bmp-gate.md only lets BMPs be compared at equal
     * base -- and so the defect stays reproducible after the fix. */
    if (!std::getenv("SM64DS_SUB_NO_SCENE_INIT")) {
        hal_sub_screen_init_hw(hwnd, zoom > 0 ? zoom : 1);
    } else {
        std::printf("[sub] SM64DS_SUB_NO_SCENE_INIT=1: scene-path bring-up "
                    "SKIPPED (the pre-fix behaviour)\n");
    }
    std::fflush(stdout);

    /* THE SDAT ROOT, before anything can ask the sequencer a question.
       dScStarSel_c::InitResources opens with Sound::LoadInitialGroup(3) and
       Sound::LoadAndSetMusic_Layer1(0x16 or 0x24) -- the scene picks its own
       music the way a level picks its course theme -- and
       LoadAndSetMusic_Layer1 walks data_020a5bb8 + 0x84 UNCONDITIONALLY, with
       no self-initialising branch the way Sound::Play has. With the root
       unseated that is a read of 0x00000084 off a null base, which is exactly
       what the first scene boot faulted on (eax=0, accessing 00000084, inside
       Sound::InfoSequenceEntry::GetWithID +0x8).
       One tick with nothing queued seats it. This is the same call and the
       same reason hal/star_flow.cpp's seat_course_sound makes it before the
       level's own LoadGroupAndSetBank, moved to where a scene run needs it:
       on the DS the SDAT root is up long before any scene spawns, because the
       boot spine (func_0201a054 -> func_02053a8c / func_02053c40) brings the
       sound system up before Scene::PrepareToSpawnBoot ever runs. The level
       harness gets it inside port_stage_a_boot, which a scene run does not
       call, so it belongs here. */
    sdat_host_tick();

    /* THE SUBLEVEL THE SCENE IS ABOUT, before it spawns -- FOR THE SCENES THAT
       READ IT, and only those. See the header block: for dScStarSel_c this is
       an input, not a default, and 0 is not a legal value for it.
       ---- THE AUDIT lane L2 owed this line ----------------------------------
       The paragraph that used to stand here ended "whoever seats ov007 or an
       ov006 scene audits this line rather than inheriting it". That audit is
       done and the answer is that dScDSMT_c DOES NOT READ IT. The evidence is
       the ROM's own relocations, counted both ways so the negative is a
       measurement and not a failure to find something:
           relocations into 0x02092110 from anywhere in ov007   0
           relocations into 0x02092110 from anywhere in ov003   6
             (0x020ae0a0, 0x020ae194, 0x020ae344 in dScStarSel_c's Render
              closure, 0x020aefcc in its Behavior, 0x020af824 in its
              InitResources, 0x020b041c in dScGameOver_c's)
       and no ov007 source TU names data_02092110, SublevelToLevel or
       SUBLEVEL_LEVEL_TABLE anywhere in the 527.
       SO THE WRITE IS GATED, because leaving it would let the harness fabricate
       an input. It is worse than inert for id 1: hal/level_change.cpp reads the
       SAME BYTE as "the next level, -1 = nothing pending" and
       port_level_change_pending() is exactly `data_02092110 >= 0`. The ROM
       ships it as -1 (romdata.py: data_02092110[4] = {255,0,0,0}), the title
       screen is what comes up at boot before any level is pending, and writing
       6 there parks a level change the title screen would never have parked.
       Nothing in a scene run consumes it today -- port_scene_run does not
       return to walk_window's level loop, which is the only caller of
       port_level_change_pending -- so this is a latent cross-talk closed
       before it fires, not a bug that was firing.
       A CORRECTION THAT COMES WITH IT: the header block above says
       "data_02092110 starts at 0". It does not; it starts at -1, and
       build/port/host-src/romdata.c is where that is visible. The conclusion
       the star-select lane drew from it is untouched -- an unsupplied sublevel
       is still invalid and still walks an uninitialised star count -- but the
       sentinel it lands on is the table's index -1, not its index 0. */
    if (scene_reads_sublevel(scene)) {
        data_02092110 = (signed char)sublevel;
        std::printf("[scene] sublevel %d -> course %d\n", sublevel,
                    (int)SUBLEVEL_LEVEL_TABLE[sublevel & 0x3f]);
    } else {
        std::printf("[scene] sublevel NOT written (%s does not read "
                    "data_02092110; 0 relocations from ov007). It holds %d.\n",
                    port_scene_class_name((unsigned)scene),
                    (int)data_02092110);
    }

    /* ---- THE INSTALLED-FADER SLOT, RESET TO THE ROM'S BOOT VALUE ----------
       dScDSMT_c::InitResources calls Scene::SetFaders, and dScStarSel_c never
       does, so this lane is the first caller in the port's history. The
       matched TU opens:

           if (data_0209f5bc) { if (data_0209f5bc->vt->f14(...)) ... }

       -- a dispatch through slot 5 of whatever fader is currently installed.
       hal/fader_wipes.cpp PRE-SEATS that word (`void *data_0209f5bc =
       &hal_wipes[0];`) because two level-path functions deref it with no null
       check, and hal_wipes[0] is a host C++ HalFaderWipe whose MSVC vtable is
       not the ROM's FaderBrightness table. Slot 5 of it is not a method; the
       first boot of this scene faulted at eip 0x61226c6c reading 0x61626c6c,
       which is the bytes "llba" out of a string literal sitting behind the
       host vtable in .rdata.

       THE ROM'S OWN VALUE HERE IS ZERO, and that is why this is a reset and
       not a workaround. config/arm9/symbols.txt has data_0209f5bc as
       kind:bss, so the DS clears it at boot, and the title screen is what
       comes up before any fader has ever been installed -- the `if` is FALSE
       on the real machine at this exact moment. The port's pre-seat is a
       LEVEL-PATH invention, correct for the path it was written for and wrong
       for a scene that runs the ROM's own guard. Restoring the zero makes the
       guard take the branch the DS takes, and Scene::SetFaders' last two
       statements put a real object back in the word before anything else can
       read it.

       Applied to EVERY scene run, not just id 1, because it is the ROM's boot
       state and not an ov007 special case; the 46-level battery is untouched
       (port_scene_run is not on the level path) and scene 4 is re-measured
       green with it. THE REAL FIX IS NOT HERE: hal/fader_wipes.cpp should give
       its wipe objects ROM-SHAPED vtables so a ROM dispatch through slot 5
       lands on a method, and then this reset can go. ROUTED. */
    data_0209f5bc = 0;
    std::fprintf(stderr, "  [scene] data_0209f5bc reset to 0 (arm9 bss; the "
                 "ROM's own value before any scene installs a fader), was "
                 "%p\n", data_0209f5bc);
    std::fflush(stderr);

    /* SEED THE MINIGAME SOUND RECORD, before the scene spawns and its base
       ctor reads it. On the DS the Rec Room overlay ov005 writes
       data_0209b308's 0x34-byte sound row for the picked minigame and THEN
       launches the scene; a direct SM64DS_SCENE boot never runs ov005, so the
       record reads zero and dScMgBase_c::AfterInitResources takes its
       StopLoadedMusic branch -- the curling "no music" report. This copies the
       same verbatim ov005 row, keyed by scene id; a no-op for non-minigame
       scenes. The SDAT root the framework's own LoadAndSetMusic_Layer1 walks
       was seated by the sdat_host_tick above, so the queued START reaches a
       live sequencer. See hal/scene_mg_sound.cpp and port/minigame_music.txt. */
    port_scene_mg_seat_sound(scene);

    void *obj = port_scene_boot(scene);
    if (!obj) {
        std::fprintf(stderr, "scene %d did not spawn\n", scene);
        std::fflush(stdout);
        return 3;
    }
    /* DID THE SCENE BECOME THE TREE ROOT? This is the ROM's own answer to "is
       this really the scene", not the port's. func_0203b438's no-parent branch
       writes the first SceneNode it is handed into data_020a4b6c[0], and on a
       scene run the scene actor is the first thing spawned, so the tree head
       must come back as the scene's own node -- ActorBase::sceneNode, pinned
       at +0x14 by the ROM's own `str r4, [r5, #0x10]` owner back-pointer (see
       include/ActorBase.h). A level run prints the same line with the Stage in
       that slot, which is the point: on the DS the Stage IS scene 3, and here
       something else is. */
    {
        void *node = (char *)obj + 0x14;
        void *head = (void *)(size_t)data_020a4b6c[0];
        std::printf("[scene] tree root %p, scene node %p (%s), pending id "
                    "0x%x, latch %u\n", head, node,
                    head == node ? "the scene IS the root"
                                 : "NOT the root, something else is",
                    (unsigned)data_02092664, (unsigned)data_02092660);
    }
    std::fflush(stdout);
    return 0;
}

/* ONE FRAME OF THE SCENE.

   `tick_game` is the debug menu's pause, and it is the same switch the level
   loop's game_ticked is: with the menu open the world holds still and the
   picture keeps being drawn, so the panel is legible over a frozen scene. It
   is 1 on every headless frame, so the battery runs the loop it always ran.

   What the pause does NOT stop is the display scan-out below. The DS's beam
   does not care that a debug menu is open, and the level loop makes the same
   call outside its own pause. */
extern "C" void port_scene_tick(int frame, int tick_game)
{
    ntr::Framebuffer &fb = scn_fb;
    const int no_render = scn_no_render;
    const int trace = scn_trace;
    /* settings.json, watched while running: every scene path -- the player's
       windowed session and the harness's headless one -- ticks through here,
       so this one call is the whole of the live re-read for scenes. The walk
       loop in tests/walk_window.cpp has its own. The gap keys need no push:
       the layout latch reads host_settings_gen itself. */
    if (host_settings_poll()) {
        const int v = host_setting_volume();
        if (v >= 0) out_set_volume_pct(v);
    }
    /* The brace is the `for` that used to be here, kept so the body below is
       the old body at the old indentation and the diff says "the loop became a
       function" rather than reflowing three dozen lines nobody changed. */
    {
        hal_sub_screen_frame_begin();
        if (tick_game) {
            port_actor_tick();
            /* THE FRAME CLOCK, func_020197b8 phase 6 (hal/fader_wipes.cpp).
               After the actor phases and before the render. NOT the ROM's exact
               slot: the ROM steps it at phase 6, after phase 5 and so after its
               phase 2 fade advance, while this sits before port_fader_advance --
               one phase early, with nothing in between that reads the word.
               Every blink in the game hangs off this counter, and on this path
               that includes the only visual difference between a SELECTED
               Pair-a-Gone card and an idle one. */
            port_frame_clock_tick();
            port_fader_advance();
            /* SM64DS_MG_RESULTS_PROBE=<frame> (hal/scene_mg.cpp): raise the
               minigame framework's results panel through the ROM's own slot 27
               at a chosen frame, so the play-again prompt can be captured on a
               scene that cannot be played to its end headless. Unset, this is
               two integer compares. */
            port_mg_results_probe(frame);
        }
        /* THE DISPLAY SCAN-OUT, which is where IRQ 2 lives. The DS raises the
           HBlank edge once per scanline while the picture is being drawn, and
           the dWipe_c motion path is built on it: its handler programs the
           next line's window bounds out of a 192-line table. This loop is the
           host's timing authority and it does not run on the ntr fiber, so it
           raises the edge itself, HERE -- after the game's behaviour work and
           before the rasteriser below, which is the same place rt_run puts it
           relative to the frame hook. Runs with NO_RENDER too: the interrupt
           is display TIMING and the host's decision not to rasterise does not
           stop the DS's beam. See port/irq2_map.txt. */
        ntr::rt_scanout_frame();

        if (!no_render) {
            /* SM64DS_SCENE_TRACE=1 names the render sub-step a run is inside,
               unbuffered, so a HANG (not a fault, which the probe already
               catches) can be attributed without a debugger. Each line goes
               out before the step it names. */
            if (trace) std::fprintf(stderr, "[scene-trace] f%d gx_reset\n", frame);
            ntr::gx_reset();
            if (trace) std::fprintf(stderr, "[scene-trace] f%d actor_render\n", frame);
            port_actor_render();
            if (trace) std::fprintf(stderr, "[scene-trace] f%d clear\n", frame);
            for (int x = 0; x < ntr::SCREEN_W; ++x) fb.px[0][x] = 0xFF101820u;
            for (int y = 1; y < ntr::SCREEN_H; ++y)
                std::memcpy(fb.px[y], fb.px[0],
                            ntr::SCREEN_W * sizeof(fb.px[0][0]));
            if (trace) std::fprintf(stderr, "[scene-trace] f%d gx_render\n", frame);
            ntr::gx_render(fb);
            if (trace) std::fprintf(stderr, "[scene-trace] f%d composite\n", frame);
            port_message_composite_engine_a(&fb);
            if (trace) std::fprintf(stderr, "[scene-trace] f%d sub_present\n", frame);
            hal_sub_screen_present(&fb.px[0][0], ntr::SCREEN_W, ntr::SCREEN_H);
            if (trace) std::fprintf(stderr, "[scene-trace] f%d render done\n", frame);
        }
        if (tick_game)
            port_actor_scene_pass();

        /* THE HOSTED ARM7, ONE TICK PER FRAME -- drain the sound queue and
           feed the mixer, exactly as the level loop does in
           tests/walk_window.cpp. A SCENE RUN NEVER DID THIS, and that was the
           second half of the curling "no music" report: the SDAT root is
           seated once in port_scene_begin, but a command QUEUED after that
           point had nothing to consume it. dScMgBase_c::AfterInitResources
           (slot 2) runs during the spawn and queues the minigame's START, so
           on the DS the sound frame that follows plays it and here it sat in
           the ring for the whole run. Gated on tick_game for the debug menu's
           pause, like the game work above it: a paused scene should not keep
           advancing the sequencer. */
        if (tick_game)
            sdat_host_tick();

        if (frame == 0)
            std::printf("[scene] f0 ticked\n");
        std::fflush(stdout);
    }
}

/* THE CAPTURES AND THE CENSUS. `frames_run` is how many frames actually ran,
   which on a windowed run is however many the player sat through rather than a
   budget nobody named. Returns the process exit code. */
extern "C" int port_scene_finish(int frames_run)
{
    const int scene = port_scene_env_want();
    const int frames = frames_run;
    const int no_render = scn_no_render;
    const char *bmp = scn_bmp;
    const char *bmp_stacked = scn_bmp_stacked;
    ntr::Framebuffer &fb = scn_fb;

    if (bmp && !no_render)
        ntr::ppu_write_bmp(bmp, fb);

    /* THE STACKED CAPTURE, and read the geometry off the line it prints rather
       than assuming it. This is a NEW artifact: SM64DS_SCENE_BMP above still
       writes the same 512x384 framebuffer it always wrote, in either layout,
       because fb is what it writes and the stacked mode never touches fb. */
    if (bmp_stacked && !no_render) {
        const unsigned int *img = hal_sub_screen_stacked_image(&fb.px[0][0]);
        /* THE SIZE COMES OFF THE LAYOUT, not off STACK_H, because a scene with
           a simulated screen gap composes a TALLER image -- 512x864 at G = 48
           against 512x768 with no gap. Asking for the constant here would have
           written the first 768 rows of a 864-row image and called it the
           capture, which is the stale-artifact shape: a real file, from the
           real run, that is not the picture the run made. */
        int iw = 0, ih = 0;
        hal_sub_screen_stacked_size(&iw, &ih);
        /* THE TWO EXTRA BANDS ARE NAMED SEPARATELY, because there are two of
           them now and they sit at opposite ends of the image: the gapless
           headroom above the top screen and the simulated hinge between the
           halves. Subtracting two screen heights gives their SUM, and a line
           that reported that sum under the gap's name would call a 64-row
           headroom a 64-row gap on a run that has no gap at all. */
        const int head = hal_sub_screen_stacked_headroom();
        /* AND THE BAND IS NAMED FOR WHAT IT IS. With the object shift on the
           rows between the halves are not a hinge: they are world -G_rom..-1,
           the top engine draws into them, and calling them a gap in a capture
           line would put a gap-on word on the one picture that has none. */
        const int shift = hal_sub_screen_stacked_obj_shift();
        /* AND WHICH ENGINE IS IN WHICH HALF, because on a scene that drives
           POWCNT1 bit 15 the answer is a property of the LAST FRAME and not of
           the program. A capture from one of those scenes read without this
           line is a capture whose halves cannot be attributed. */
        std::fprintf(stderr, "  [scene] stacked capture: POWCNT1 %04x, so the "
                     "upper half is engine %c and the lower half engine %c\n",
                     *(volatile unsigned short *)0x04000304,
                     (*(volatile unsigned short *)0x04000304 & 0x8000) ? 'A' : 'B',
                     (*(volatile unsigned short *)0x04000304 & 0x8000) ? 'B' : 'A');
        /* THE SUB FRAMEBUFFER ON ITS OWN, beside the stacked image and under
           the same switch. Engine B's raster is 256x192 and is the ONE thing a
           stacked capture cannot be read back to when the halves can swap: the
           lower half is engine A on half the frames of a D3D minigame. Writing
           it separately is what makes "engine B produced this" a measurement.
           Named off the stacked path with _subB before the extension. */
        if (bmp_stacked) {
            char sub_path[512];
            std::snprintf(sub_path, sizeof sub_path, "%s", bmp_stacked);
            char *dot = std::strrchr(sub_path, '.');
            if (dot && (size_t)(dot - sub_path) + 10 < sizeof sub_path) {
                std::memmove(dot + 5, dot, std::strlen(dot) + 1);
                std::memcpy(dot, "_subB", 5);
                if (hal_sub_screen_write_bmp(sub_path))
                    std::printf("[scene] wrote %s, %dx%d: engine B's own "
                                "raster, whichever screen it is driving\n",
                                sub_path, ntr::SUB_W, ntr::SUB_H);
            }
        }
        if (img && ntr::ppu_write_bmp_px(bmp_stacked, img, iw, ih))
            std::printf("[scene] wrote %s, %dx%d: the top screen over the "
                        "bottom screen, each %dx%d, with %d row(s) of headroom "
                        "above and %d row(s) of %s between them\n",
                        bmp_stacked, iw, ih, ntr::SCREEN_W, ntr::SCREEN_H, head,
                        ih - ntr::SCREEN_H * 2 - head,
                        shift ? "the world's own rows, drawn by the top engine "
                                "at its shifted submission,"
                              : "gap");
        else
            std::fprintf(stderr, "  [scene] SM64DS_SCENE_BMP_STACKED asked for "
                         "%s but the stacked layout is %s and the bottom "
                         "screen %s scanned out; nothing written\n",
                         bmp_stacked, hal_sub_screen_stacked() ? "on" : "OFF",
                         hal_sub_screen_stacked() ? "was never" : "may not be");
    }

    /* THE SUB-SCREEN CENSUS, deliberately HERE and not beside the bring-up
       above. Probed before the spawn it reads all zeros in either mode and
       proves nothing: the scene has not run its InitResources yet, so no bank,
       no palette and no tile has been written by anybody. Probed after the
       frame loop it reads what the scene actually built, which is the only
       state a claim about the bottom screen can rest on. */
    hal_sub_screen_probe();
    std::fflush(stdout);
    /* THE WITNESS. Not "the scene booted" -- how many times each of the class's
       own slots was entered by the ROM's own processing lists. A scene that
       spawns and a scene that RUNS look identical from outside: the object
       exists either way, and the [scene] lines above would print the same for
       an object nothing ever ticked. */
    {
        const int t = (scene == 1);
        const unsigned *h = t ? g_ti_hits : g_ss_hits;
        const unsigned sk = t ? g_ti_render_skipped : g_ss_render_skipped;
        std::printf("[scene] %s slot hits: init %u, behavior %u, render %u, "
                    "cleanup %u, pending-destroy %u%s\n",
                    t ? "ov007" : "ov003",
                    h[0], h[6], h[9], h[3], h[12],
                    sk ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
        if (g_ti_init_skipped)
            std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                        "time(s)\n", g_ti_init_skipped);
        if (port_l2_trap_hits())
            std::printf("[scene] unmatched-body traps entered: %u\n",
                        port_l2_trap_hits());
        else
            std::printf("[scene] unmatched-body traps entered: 0 (none of "
                        "the 24 trapping sites was reached)\n");
    }
    /* THE IRQ-2 WITNESS, next to the slot census and for the same reason. A
       fade that is armed and a fade that MOVES look identical from outside:
       the table at data_0209f648 is built either way. What separates them is
       whether anything read it, so the count printed here is deliveries of
       the HBlank edge and, of those, the ones after which WIN0H/WIN1H held a
       different value -- a scanline actually programmed out of the table.
       Zero window writes with a nonzero delivery count means the handler ran
       and took an early return, which is a different bug from a shut gate. */
    {
        unsigned long long hb = 0, win = 0, cp = 0, cpb = 0, distinct = 0;
        unsigned last = 0;
        ntr::rt_hblank_counters(&hb, &win);
        port_window_copy_count(&cp, &cpb, &last, &distinct);
        const unsigned gates = ntr::rt_hblank_gates();
        std::printf("[scene] irq2: %llu HBlank deliver(s), %llu table read(s) "
                    "into WIN0H (%llu bytes), %llu of them CHANGING the "
                    "latched window\n", hb, cp, cpb, win);
        if (cp)
            std::printf("[scene] irq2 rows: %llu run(s) of equal rows across "
                        "%llu read(s), last row %08X (WIN1H:WIN0H)\n",
                        distinct, cp, last);
        std::printf("[scene] irq2 gates at exit: handler=%d ie=%d cpsr=%d "
                    "ime=%d dispstat=%d\n",
                    (gates & ntr::HBLANK_GATE_HANDLER) != 0,
                    (gates & ntr::HBLANK_GATE_IE) != 0,
                    (gates & ntr::HBLANK_GATE_CPSR) != 0,
                    (gates & ntr::HBLANK_GATE_IME) != 0,
                    (gates & ntr::HBLANK_GATE_DISPSTAT) != 0);
    }
    std::printf("[scene] %d frames of scene %d (%s), clean\n", frames, scene,
                port_scene_class_name((unsigned)scene));
    std::fflush(stdout);
    return 0;
}

/* THE HEADLESS RUN, which is the composition of the three above and nothing
   else. Every statement a scene run made before the split still runs, in the
   same order, with hwnd null, zoom 1 and the game ticking on every frame. */
extern "C" int port_scene_run(void)
{
    const int rc = port_scene_begin(nullptr, 1);
    if (rc)
        return rc;
    for (int frame = 0; frame < scn_frames; ++frame)
        port_scene_tick(frame, 1);
    return port_scene_finish(scn_frames);
}
