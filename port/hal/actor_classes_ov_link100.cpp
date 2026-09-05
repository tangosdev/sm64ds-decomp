// RUN link100, LANE OV: the two overlay actor classes the registry had never
// carried whose whole vtable is matched src with no guessed body in it.
//
// ---- HOW THE FRONTIER WAS MEASURED, so the next lane does not redo it ------
//
// port/tools/ov_seatscan.py walks ACTOR_SPAWN_TABLE (arm9 0x02090864, 391
// SpawnInfo*) and resolves every row to the overlay that owns it -- object
// overlays are OVERLAID, so an address alone names no module; a row is
// attributed to overlay N only when N's own image has a `kind:function`
// symbol at the record's +0 word AND the record's +4 halfword equals the
// table row's id, which is the same cross-check port_actor_registry_install
// makes at run time. 387 of the 391 rows resolve that way (0, 3, 332 and 360
// are arm9-resident), and at 7db4fc602 the registry carried 342 of them.
//
// Of the 45 it did not:
//   30  ov006, ids 361..390: minigame SCENES. Their slot 1 is
//       Scene::BeforeInitResources and they belong to scene_boot.cpp's
//       port_scene_classes, not to this table.
//    9  ov003/ov005/ov007/ov075, ids 1/2/4/5/6/7/8/346 and friends: also
//       Scene-derived, same reason.
//    6  actor classes, and three of those are mostly INFERRED STUBS:
//         99  ov029 CageLift          5 of 6 own bodies guess-marked
//        192  ov002 (0x021096b0)      7 of 9 guess-marked
//        137  ov043 (0x0211238c)      3 of 8 guess-marked
//        223  ov081 daSnowman_c       a documented register-skew refusal
//                                     (hal/actor_classes.inc's own note)
//       leaving the two seated here.
//
// port/tools/ov_orphans.py sweeps the other direction -- every Actor-derived
// table in every overlay that NO factory installs -- and finds sixteen. None
// is seated here and the report says why: twelve are intermediate bases whose
// table nothing in the ROM ever loads except its own D1/D0 (a closed
// two-cycle: the derived ctor writes only the final table), so filling one
// would be a keep-alive rather than a reference the game makes, and the four
// that ARE constructed (Enemy, CapEnemy, the bullies' base, Thwomp's) belong
// to classes another file already owns.
//
// ============================================================================
// CLASS A -- actor id 348, ov002, RTTI 10daChRoom_c (change room)
// ============================================================================
//
// BY ADDRESS, because every name in this family is shifted one class:
//
//   ACTOR_SPAWN_TABLE[348]        -> 0x021085d4   (VirtualDoor_SpawnInfo)
//   record +4 halfword             = 348          the registry's own check
//   record +0 word                -> 0x020b0980   (VirtualDoor_Spawn)
//   the factory's only data load
//   (reloc from:0x020b09ac)       -> 0x021085f8   (config _ZTV9CameraTag)
//   that table's -4 word          -> typeinfo 0x021085b8, name "10daChRoom_c"
//
// THE SHIFT, stated once. dsd's SpawnInfo/factory names are right and its
// vtable/method names are one class LATE, which the tree has already recorded
// twice (port/ov002_syms.txt's gate-20 block and hal/actor_classes_bob_world's
// SEAT-CAMERATAG note). Read straight off the four tables in this run of the
// image, typeinfo taken from the Itanium V-4 word and not from the record that
// happens to follow the table:
//
//   0x02108480  RTTI 7daBar_c       bodies named _ZN13InvisiblePole* ... no:
//                                   installed by InvisiblePole_Spawn, id 287
//   0x0210853c  RTTI 10daCamTag_c   installed by CameraTag_Spawn, id 333
//   0x021085f8  RTTI 10daChRoom_c   installed by VirtualDoor_Spawn, id 348 <-
//   0x021086b4  RTTI 11daChScene_c  installed by Exit_Spawn, id 349
//
// so 287, 333 and 349 were already seated and 348 was the one hole in the
// family. Its seven own bodies are the files dsd named _ZN9CameraTag*, and
// they are this class's because the table's own slots point at them.
//
// WHAT IT IS. A 212-byte plain Actor with no model and no collider (the
// factory is `operator new(212)`, Actor's ctor, store the table -- nothing
// else). Behavior reads the current player out of data_0209f394[data_0209f250],
// rotates the offset into the tag's own frame and, inside the tag's box, calls
// ChangeArea with the area number the spawn record's angle nibbles carry. It
// is the ROOM-CHANGE trigger volume of a castle door.
//
// WHERE IT SPAWNS. The Standard/Simple object tables never name 348; it comes
// through the DOOR sub-loader. src/_Z15LoadDoorObjectsRN11LVL_Overlay11ObjSub
// TableEij.cpp spawns data_ov002_0210cb88[record.fielda & 0x1f], and entry 0
// of that table IS 348. port/tools/ov_places.py decodes that second
// indirection (nothing else in the tree did, which is why 348 read as
// "placed on no level"): levels 17, 18 and 21 each place exactly one.
//
// ============================================================================
// CLASS B -- actor id 314, ov039, RTTI 11daObjKumo_c (kumo: cloud)
// ============================================================================
//
//   ACTOR_SPAWN_TABLE[314]        -> 0x02111834   (Cloud_SpawnInfo)
//   record +4 halfword             = 314, +6 = 5  (the render priority)
//   record +0 word                -> 0x0211137c   (Cloud_Spawn)
//   the factory's data load
//   (reloc from:0x021113b0)       -> 0x02111858   (_ZTV5Cloud)
//   that table's -4 word          -> typeinfo 0x02111818, name "11daObjKumo_c"
//
// No name shift here: both destructors and the factory load the same table,
// and dsd gave that one address TWO names (_ZTV5Cloud and _ZTV11daObjKumo_c,
// ov039/symbols.txt lines 52 and 53). Cloud_Spawn.c spells the first,
// _ZN5CloudD0Ev.c the second; the host array below is defined under
// _ZTV5Cloud and hal/cxx_aliases.cpp's link100 block aliases the other name
// onto it, the ov017 daObjKsWater_c treatment. Both names are held OUT of the
// ov039 mount, so the alias's left side is undefined everywhere and cannot be
// defeated by a definition.
//
// WHAT IT IS. A 292-byte plain Actor with one plain Model at +0xd4. Its
// InitResources loads the SharedFilePtr the overlay's own sinit built, hands
// the file to the model, builds the model matrix from the spawn yaw, and bumps
// a per-level counter. Its Behavior ranks the cloud against every other cloud
// by distance from the camera and hands the rank to Model::SetPolygonID, which
// is how a stack of clouds sorts against itself.
//
// WHERE IT SPAWNS. Level 31 (ov039, data/stage/habatake -- Tiny-Huge Island's
// sky) places SEVENTEEN of them, and no other level places any.
//
// ---- THE ov039 MOUNT -------------------------------------------------------
// ov039 was already mounted --whole in PORT_LEVEL_OVERLAYS (it is level 31's
// own overlay). That mount is one array and names nothing, so this adds the
// SECOND, per-symbol mount -- the ov009/ov010/ov015/ov016/ov018/ov029 shape --
// for the SpawnInfo record the registry rewrites in place, the two bss cells
// the overlay's sinit builds, and nothing else. The vtable stays out: it is
// host storage this file fills.
//
// ---- THE SINIT -------------------------------------------------------------
// __sinit_ov039_021113b4 constructs the model SharedFilePtr (file 1141) at
// 0x021118e4 and registers its teardown node at 0x021118ec. It runs from this
// file's bring-up, before the registry installs, so the pointer exists by the
// time any InitResources runs -- the ov017/ov045 order. Its two-argument reach
// into func_02017acc is the arity hal/cxx_aliases.cpp already owns and already
// serves for ov045's five sinits.
//
// ---- TWO SIBLING-OVERLAY SPELLINGS, ROUTED PER SOURCE ----------------------
// src/_ZN5Cloud13InitResourcesEv.cpp calls `func_ov044_02111214` and bumps
// `data_ov041_021118e0`: two sibling overlays' names for addresses that, in
// THIS class's module, are ov039's own (the ROM relocs at 0x0211136c and
// 0x02111378 both read module:overlay(39)). port/CMakeLists.txt routes both
// with a per-source -D, which a definition cannot defeat the way an
// /alternatename can. func_ov039_02111214 is a matched TU and joins the slice;
// data_ov039_021118e0 is a four-byte counter defined below.
//
// ---- THE TABLES ARE 31 SLOTS, BOTH ----------------------------------------
// Slot 30 is Actor::OnAimedAtWithEggReturnVec (arm9 0x020100dc) in both, and
// the word after it is the class's typeinfo record rather than a virtual, so
// neither is a Platform and neither has a Kill. Slots 13/14 are
// ActorBase::Virtual34/Virtual38 -- two u32 arguments each, bodies not in this
// link -- and are TRAPPED by name, the ccm/ov064/jrb/bbh/ov072 convention.
#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"
#include "Actor.h"
#include "ActorBase.h"

/* hal/actor_slot30_seat.cpp: the shared SRET seat for slot 30. The ROM word
   in slot 30 of both tables IS the arm9 base body 0x020100dc. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);

extern "C" {
/* ---- the shared arm9 half both tables carry, slot for slot -------------- */
int _ZN5Actor19BeforeInitResourcesEv(void *self);
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);
int _ZN5Actor14BeforeBehaviorEv(void *self);
int _ZN5Actor12BeforeRenderEv(void *self);
int _ZN5Actor13OnYoshiTryEatEv(void *self);
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);
int _ZN5Actor9Virtual50Ev(void *self);
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);
void _ZN5Actor8OnKickedERS_(void *self, void *o);
void _ZN5Actor8OnPushedERS_(void *self, void *o);
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);
int _ZN5Actor16OnAimedAtWithEggEv(void *self);
/* the diagnostics the trap uses */
extern int data_02099f24[];
extern unsigned char data_020a4b4c;
const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);

/* ---- class A, ov002 id 348 ---------------------------------------------- */
int _ZN9CameraTag16CleanupResourcesEv(void);     /* slot  3, 0x020b0854 */
int _ZN9CameraTag6RenderEv(void);                /* slot  9, 0x020b0860 */
void _ZN9CameraTag16OnPendingDestroyEv(void);    /* slot 12, 0x020b085c */
int *_ZN9CameraTagD0Ev(int *self);               /* slot 17, 0x020b081c */
DSSTATE_BEGIN
void *_ZTV9CameraTag[31];                        /* ov002 0x021085f8 */
DSSTATE_END

/* ---- class B, ov039 id 314 ---------------------------------------------- */
int _ZN5Cloud16CleanupResourcesEv(void);         /* slot  3, 0x02111254 */
int _ZN5Cloud8BehaviorEv(char *self);            /* slot  6, 0x021112a0 */
int *_ZN5CloudD0Ev(int *self);                   /* slot 17, 0x021111d0 */
void __sinit_ov039_021113b4(void);               /* ov039 0x021113b4 */
/* the generated ov039 per-symbol mount (build/port/host-src/ov039_syms.c) */
void port_ov039_pack_check(void);
void port_ov039_syms_patch(void);
DSSTATE_BEGIN
void *_ZTV5Cloud[31];                            /* ov039 0x02111858 */
/* ov039 0x021118e0, the four-byte bss counter Cloud::InitResources bumps.
   Held out of the mount and defined here because decl_common.h declares the
   sibling spelling as a SCALAR int, not an array. */
int data_ov039_021118e0;
DSSTATE_END
}

/* ---- the real-C++ faces --------------------------------------------------
   Four of the nine own bodies are real C++ methods and two more are real C++
   destructors, so MSVC emits them under its own manglings
   (?InitResources@CameraTag@@QAEHXZ, ??1Cloud@@UAE@XZ and so on) rather than
   under the ROM's Itanium C names. The shadow classes below are declared with
   exactly the members whose manglings are needed -- a mangle depends on the
   name, the class, the calling convention and the signature, and on nothing
   else, so these declarations reach the matched bodies without the headers
   (which cannot be included here: CameraTag.h and Cloud.h declare the same
   two classes WITHOUT the virtual destructor, and both spellings cannot
   coexist in one TU). This is hal/dtor_faces_cpp.cpp's CPPD1 idiom and
   hal/actor_classes_ov017.cpp's method-face idiom in one place; the faces are
   private to this file so no other owner's file changes. */
struct CameraTag {
    virtual ~CameraTag();
    int InitResources();
    int Behavior();
};
struct Cloud {
    virtual ~Cloud();
    int InitResources();
    int Render();
};
/* src/_ZN5CloudD1Ev.cpp's Cloud holds a `Model m0` at +0xd4, so its MSVC
   destructor calls ??1Model@@QAE@XZ. That face already exists --
   port/unmatched/Mg3DEsp_Faces.cpp defines Model::~Model() as one call into
   the ROM C name _ZN5ModelD1Ev -- and defining a second one here was an
   LNK2005 the first link caught. Nothing is needed on this side; the note
   stays so the next reader does not re-add it. */

/* ---- the trap ------------------------------------------------------------
   Slots 13 and 14 are ActorBase::Virtual34(u32,u32) and Virtual38(u32,u32),
   named by the ROM relocs at both tables' +0x34 and +0x38. Their bodies are
   solid-heap allocator work that is not in this link, and no __fastcall
   thunk here can carry two stack arguments the callee would have to pop, so
   they are trapped by name instead of being seated with the wrong contract.
   hal/actor_classes_bob_world.cpp records that a heap-pressure path
   dispatches Virtual34 on EVERY actor on the behaviour list, which is why an
   unfilled word here would be a live wild call rather than dead scaffolding. */
static void l100_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char m[128];
      std::snprintf(m, sizeof m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(m); }
}
static int __fastcall l100_trap13(void *s, void *) { l100_trap_report(s, 13); return 0; }
static int __fastcall l100_trap14(void *s, void *) { l100_trap_report(s, 14); return 0; }

/* ---- the shared 1..30 half, identical in both tables ----------------------
   Every slot below is byte-identical between ov002 0x021085f8 and ov039
   0x02111858 in the ROM images -- checked word for word against
   config/arm9/overlays/ov{002,039}/relocs.txt at table+4*N -- which is what
   makes one shared writer legitimate. The caller writes its own
   0/3/6/9/12/16/17 afterwards. */
static int __fastcall l100_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall l100_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall l100_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall l100_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall l100_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall l100_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall l100_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall l100_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall l100_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall l100_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall l100_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall l100_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall l100_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall l100_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall l100_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall l100_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall l100_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall l100_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall l100_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall l100_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall l100_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

static void l100_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)l100_binit;    /* Actor::BeforeInitResources */
    vt[2]  = (void *)l100_ainit;    /* Actor::AfterInitResources(u32) */
    vt[4]  = (void *)l100_bclean;   /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)l100_aclean;   /* Actor::AfterCleanupResources(u32) */
    vt[7]  = (void *)l100_bbeh;     /* Actor::BeforeBehavior */
    vt[8]  = (void *)l100_abeh;     /* Actor::AfterBehavior(u32) */
    vt[10] = (void *)l100_bren;     /* Actor::BeforeRender */
    vt[11] = (void *)l100_aren;     /* Actor::AfterRender(u32) */
    vt[13] = (void *)l100_trap13;   /* ActorBase::Virtual34(u32,u32) */
    vt[14] = (void *)l100_trap14;   /* ActorBase::Virtual38(u32,u32) */
    vt[15] = (void *)l100_heap;     /* ActorBase::OnHeapCreated */
    vt[18] = (void *)l100_yoshi;    /* Actor::OnYoshiTryEat */
    vt[19] = (void *)l100_egg;      /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)l100_v50;      /* Actor::Virtual50 */
    vt[21] = (void *)l100_pounded;  /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)l100_atk1;     /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)l100_atk2;     /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)l100_kicked;   /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)l100_pushed;   /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)l100_cannon;   /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)l100_mega;     /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)l100_under;    /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)l100_aimed;    /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)port_actor_s30_base;  /* OnAimedAtWithEggReturnVec, SRET */
}

// ============================================================================
// CLASS A: ov002 0x021085f8, actor id 348, RTTI 10daChRoom_c
// ============================================================================
static int __fastcall chroom_init(void *s, void *)
{ return ((CameraTag *)s)->CameraTag::InitResources(); }
static int __fastcall chroom_clean(void *s, void *)
{ (void)s; return _ZN9CameraTag16CleanupResourcesEv(); }
static int __fastcall chroom_behavior(void *s, void *)
{ return ((CameraTag *)s)->CameraTag::Behavior(); }
static int __fastcall chroom_render(void *s, void *)
{ (void)s; return _ZN9CameraTag6RenderEv(); }
static int __fastcall chroom_pdes(void *s, void *)
{ (void)s; _ZN9CameraTag16OnPendingDestroyEv(); return 0; }
static int __fastcall chroom_d1(void *s, void *)
{ ((CameraTag *)s)->CameraTag::~CameraTag(); return (int)(size_t)s; }
static int __fastcall chroom_d0(void *s, void *)
{ return (int)(size_t)_ZN9CameraTagD0Ev((int *)s); }

extern "C" void hal_fill_change_room_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV9CameraTag;
    l100_fill_shared(vt);
    vt[0]  = (void *)chroom_init;      /* 0x020b0938 */
    vt[3]  = (void *)chroom_clean;     /* 0x020b0854 */
    vt[6]  = (void *)chroom_behavior;  /* 0x020b0868 */
    vt[9]  = (void *)chroom_render;    /* 0x020b0860 */
    vt[12] = (void *)chroom_pdes;      /* 0x020b085c */
    vt[16] = (void *)chroom_d1;        /* 0x020b07f8 */
    vt[17] = (void *)chroom_d0;        /* 0x020b081c */
}

// ============================================================================
// CLASS B: ov039 0x02111858, actor id 314, RTTI 11daObjKumo_c
// ============================================================================
static int __fastcall cloud_init(void *s, void *)
{ return ((Cloud *)s)->Cloud::InitResources(); }
static int __fastcall cloud_clean(void *s, void *)
{ (void)s; return _ZN5Cloud16CleanupResourcesEv(); }
static int __fastcall cloud_behavior(void *s, void *)
{ return _ZN5Cloud8BehaviorEv((char *)s); }
static int __fastcall cloud_render(void *s, void *)
{ return ((Cloud *)s)->Cloud::Render(); }
static int __fastcall cloud_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall cloud_d1(void *s, void *)
{ ((Cloud *)s)->Cloud::~Cloud(); return (int)(size_t)s; }
static int __fastcall cloud_d0(void *s, void *)
{ return (int)(size_t)_ZN5CloudD0Ev((int *)s); }

/* The ov039 bring-up: check the per-symbol mount landed where the ROM says,
   rebase its interior pointers, then run the overlay's own static
   initialiser. Once per process, ahead of the registry install, so the
   SharedFilePtr the first InitResources loads through is already built. */
DSSTATE_BEGIN
static int g_ov039_bringup_done;
DSSTATE_END

extern "C" void port_ov039_bringup(void)
{
    if (g_ov039_bringup_done)
        return;
    g_ov039_bringup_done = 1;
    port_ov039_pack_check();
    port_ov039_syms_patch();
    __sinit_ov039_021113b4();
}

extern "C" void hal_fill_cloud_vtable(void)
{
    port_ov039_bringup();
    void *volatile *vt = (void *volatile *)_ZTV5Cloud;
    l100_fill_shared(vt);
    vt[0]  = (void *)cloud_init;      /* 0x0211132c */
    vt[3]  = (void *)cloud_clean;     /* 0x02111254 */
    vt[6]  = (void *)cloud_behavior;  /* 0x021112a0 */
    vt[9]  = (void *)cloud_render;    /* 0x02111278 */
    vt[12] = (void *)cloud_pdes;      /* arm9 0x02043ac0, ActorBase's own */
    vt[16] = (void *)cloud_d1;        /* 0x021111a0 */
    vt[17] = (void *)cloud_d0;        /* 0x021111d0 */
}
