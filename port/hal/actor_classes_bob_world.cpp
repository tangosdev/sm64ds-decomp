// Gate 33: the vtables of Bob-omb Battlefield's mechanisms, terrain objects
// and pickups. Everything in that level that is not a living creature.
//
// Same law as hal/actor_classes.cpp, which carries the long version: MSVC
// slot order, __fastcall thunks, every call qualified, and a named trap for
// anything the port cannot service. The ten shared slots are filled by the
// local copy of ac_fill_shared below rather than by reaching into the other
// file, so this one merges with the enemy stream without either touching the
// other's statics.
//
// ---- THE WIDTH IS 31 OR 32, NEVER 20 ---------------------------------------
//
// Twelve arrays here were written [20]. Every ROM table is 31 words (a plain
// Actor subclass, ending at Actor::OnAimedAtWithEggReturnVec) or 32 (a
// Platform subclass, slot 31 being Platform::Kill). BRICK_BLOCK is the
// confirmed level-3 crash and STATIC_ROCK the level-8 one -- STATIC_ROCK
// shares _ZTV13FortressTower, and that one is a 32-slot Platform table read
// as 20. Every width below comes from port/tools/vtspan.py, which walks the
// reloc run; the next-symbol bound truncates for four of them (_ZTV9WaterBomb
// reads as 7 words and is 31).
//
// Slots 20..29 are Actor's own interaction list, all matched arm9 bodies
// already in the build, and bw_fill_shared writes them for every table. Slot
// 30 returns a Vector3 by value: the sret contract is unproved and the body
// is in no slice, so it declines by name.
//
// ---- how the roster was established ---------------------------------------
//
// Bob-omb Battlefield is level 6 and ov014; see the header of
// slice_gate33.txt for the three reads that settle it. Every id below came
// out of ov014's own object tables, was resolved through the EU
// ACTOR_SPAWN_TABLE at arm9 0x02090864, and its vtable was checked against
// the ROM's Itanium RTTI -- the typeinfo pointer four bytes before the table.
// That last step matters because the config's class names are shifted by one
// through a run of ov002 (the gate-20 finding), and the RTTI is the only
// thing that says which way.
//
// ---- HEALING_HEART IS REGISTERED (gate 45); FOUR STILL ARE NOT -------------
//
// HEALING_HEART (297)'s fault was the model preload, and the preload is now
// hosted: hal/level_boot.cpp's port_stage_preload_shared_models runs
// Stage::InitResources' own loop
//
//     for (i = 0; i < 0xC; i++) Model::LoadFile(data_020756f0[i]);
//
// over all twelve SharedFilePtrs at level boot (12/12 seated). The heart's
// backtrace was followed to the bottom and named exactly that gap:
//
//     main -> port_actor_render -> func_02043fdc -> func_0204322c
//       -> port_actor_process -> Seaweed::Render +0x11
//       -> ModelAnim::Virtual18 +0xe -> ModelAnim::Virtual10 +0x25
//       -> Model::Virtual10 +0xc      reading through a NULL, eax = 0
//
// Model::Virtual10 is the bone-matrix copy reading a model whose file never
// arrived. With the twelve-model preload in place the SharedFilePtr::filePtr
// the heart reads is populated, its first frame is clean, and it now spawns in
// level 6 (census: HEALING_HEART x1) and runs 300 frames, exit 0.
//
// CAP (269), EXCLAMATION_SWITCH (11), WATER_BOMB (208) and ROLLING_IRON_BALL
// (220) still fault, but NOT on the preload -- each dies a few frames later
// jumping to a DS code address that is a state/behavior function the port has
// not built or seated:
//
//   EXCLAMATION_SWITCH -> func_ov002_020ba4d8 dispatches through the shared
//     mwcc PMF table data_ov002_0210e00c (StarSwitch indexes entries 1..4),
//     which MSVC widens over an incomplete class and reads at the wrong stride
//   WATER_BOMB         -> data_ov098_0213c724 holds func_ov098_0213b9d8, the
//     WaterBomb behavior, which is in no slice (unbuilt DS address called)
//   CAP / ROLLING_IRON_BALL -> the same shape, a table of DS behavior pointers
//     that needs the class's closure added to the slice and the table seated
//
// Each is a per-class PMF/state seat of the kind port/unmatched/*_States.cpp
// already carries for BobOmbBuddy, ChainChomp, KingBobOmb and SignPost. They
// are left BUILT -- slice, vtable fill, faces -- so the next attempt starts
// with the link closed and only the seat to write. Until then they have no
// registry row, so the spawn gate names them as skipped instead of dying.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
/* Actor's own interaction list, slots 20..29, plus the Platform tail. All
   matched arm9 bodies already in the build (slice_gate32, slice_gate50). */
int  _ZN5Actor9Virtual50Ev(void *self);                        /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);                /* slot 29 */
void _ZN8Platform4KillEv(void *self);                          /* slot 31 */
extern int data_02099f24[];               /* the frame phase */
extern unsigned char data_020a4b4c;       /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model);
}

static int __fastcall bw_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall bw_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall bw_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
/* Slots 5, 8 and 11 are ARM tail-call veneers on the ROM, so the thunk calls
   the target directly rather than forwarding through the veneer's own face
   and dropping the argument riding in r1. */
static void __fastcall bw_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall bw_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall bw_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall bw_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall bw_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
/* SLOTS 13 AND 14 KEEP THE GATE-16 TRAP, and the reason was measured rather
   than assumed this time. The vtable law wants both wired to ActorBase's own
   Virtual34/Virtual38 -- a heap-pressure path calls Virtual34 on every actor
   on the behaviour list, so a class that traps there dies on a frame that has
   nothing to do with it. Both are matched src, and so is everything under
   them, but everything under them is the SOLID HEAP ALLOCATOR: 17 SolidHeap
   files, 12 SolidHeapAllocator files and a synthetic sixteen-slot
   _ZTV9SolidHeap. That is a subsystem next to gate 3a's ExpandingHeap, not
   two slots on this gate's classes. The full chain is written out in the head
   of slice_gate33.txt so the next attempt starts with the bill in hand.
   Until then the trap names the slot, the actor and the phase, which is the
   loud version of "no level has reached this yet". */
static int __fastcall bw_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall bw_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static void __fastcall bw_pdes_base(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); }

static void bw_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n", slot, id,
                 port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall bw_trap13(void *s, void *) { bw_trap_report(s, 13); return 0; }
static int __fastcall bw_trap14(void *s, void *) { bw_trap_report(s, 14); return 0; }
static int __fastcall bw_trap16(void *s, void *) { bw_trap_report(s, 16); return 0; }
static int __fastcall bw_trap17(void *s, void *) { bw_trap_report(s, 17); return 0; }
/* slot 19, OnTurnIntoEgg(Player &player): the caller PUSHES the player, so the
   three-parameter veneer pops it. The ROM reloc at each of these tables + 0x4c
   lands on arm9 0x02010154, Actor::OnTurnIntoEgg, a tail-call veneer to
   KillAndTrackInDeathTable, so Yoshi swallowing one kills AND tracks it for
   respawn. Trapping the slot froze the actor forever instead. */
static int __fastcall bw_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
/* The interaction-tail traps. Slot 30 declines for every table here (its ROM
   body returns a Vector3 by value and the sret contract is unproved); the
   others are for classes that OVERRIDE one of Actor's tail bodies with one
   that is in no slice, where forwarding to Actor's would run the wrong code.
   Their own `ret` is unreachable -- port_actor_slot_decline aborts or raises
   for the quarantine handler -- so the two-parameter shape is dead code. */
#define BW_TRAP(n)     static int __fastcall bw_trap##n(void *s, void *)     { bw_trap_report(s, n); return 0; }
BW_TRAP(21) BW_TRAP(22) BW_TRAP(27) BW_TRAP(29) BW_TRAP(30) BW_TRAP(31)
#undef BW_TRAP

/* Actor's own interaction tail. Slots 21..28 take an argument the __thiscall
   caller PUSHED, so each thunk needs the dummy edx AND the named parameter. */
static int __fastcall bw_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall bw_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall bw_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall bw_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall bw_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall bw_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall bw_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall bw_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall bw_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall bw_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail; only the Platform tables below write it */
static int __fastcall bw_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

static void bw_fill_shared(void **vt)
{
    vt[1] = (void *)bw_binit;
    vt[2] = (void *)bw_ainit;
    vt[4] = (void *)bw_bclean;
    vt[5] = (void *)bw_aclean;
    vt[7] = (void *)bw_bbeh;
    vt[8] = (void *)bw_abeh;
    vt[10] = (void *)bw_bren;
    vt[11] = (void *)bw_aren;
    vt[13] = (void *)bw_trap13;
    vt[14] = (void *)bw_trap14;
    vt[15] = (void *)bw_heap;
    vt[18] = (void *)bw_yoshi;
    vt[19] = (void *)bw_turn_egg;
    /* Actor's interaction tail, in every table here; the two classes that
       override one of these rewrite their own slot below. */
    vt[20] = (void *)bw_v50;
    vt[21] = (void *)bw_pounded;
    vt[22] = (void *)bw_atk1;
    vt[23] = (void *)bw_atk2;
    vt[24] = (void *)bw_kicked;
    vt[25] = (void *)bw_pushed;
    vt[26] = (void *)bw_cannon;
    vt[27] = (void *)bw_mega;
    vt[28] = (void *)bw_under;
    vt[29] = (void *)bw_egg;
    vt[30] = (void *)bw_trap30;
}

// ---- CommonModel, the first time it is hosted ------------------------------
//
// Not an actor. The coin carries TWO of them (+0xd8 and +0x114) and every
// hosted class before this gate used Model or ModelAnim instead, so its
// vtable and its one cross-face have to land with the coin.
//
// _ZTV11CommonModel is arm9 0x0208e8a4 and it is THREE SLOTS: D1, D0, and
// DoSetFile. The class's Render is non-virtual, which is the whole difference
// from Model. Under MSVC the two destructor slots fold into one, so DoSetFile
// is slot 1 there and slot 2 in ROM/Itanium numbering -- and both get filled,
// the same dual-fill _ZTV5Model already carries in hal/cxxname_bridge.cpp,
// because a TU dispatching through a LOCAL shadow class counts the ROM's way.
//
// The live dispatcher is ModelBase::SetFile, which Coin::InitResources calls
// four times by its Itanium name.
#include "CommonModel.h"
extern "C" {
void *_ZTV11CommonModel[8];
}

static int __fastcall cm_dosetfile(void *self, void *, char *file, int a, int b)
{ return ((CommonModel *)self)->CommonModel::DoSetFile(file, a, b); }
/* MERGE DECISION, 2026-08-05, and the one in this consolidation that wants a
   real test rather than a reading.

   Gate 32 (King Bob-omb, the first actor to own a CommonModel) filled this
   table in ROM numbering with the real destructors: D1, D0, DoSetFile. This
   file filled it MSVC-first: a trap, then DoSetFile at both 1 and 2. Slot 1 is
   the collision, because ROM wants D0 there and MSVC wants DoSetFile.

   One table cannot serve both, so the verified path wins. The coins here are
   measured working and their dispatcher is ModelBase::SetFile, which is
   header-compiled and therefore counts MSVC slots: DoSetFile MUST be at 1.
   Gate 32's teardown was never exercised in its 600-frame runs, so its D0 at
   slot 1 is a reading rather than a measurement.

   Slot 0 takes gate 32's REAL D1 rather than this file's abort, which is
   strictly better: a teardown that does dispatch slot 0 destroys the object
   instead of killing the process. What now has no slot is D0. If a CommonModel
   is ever destroyed through ROM slot 1 it reaches DoSetFile with destructor
   arguments, so that is the thing to suspect if King Bob-omb misbehaves on
   despawn. */
extern "C" void *_ZN11CommonModelD1Ev(void *self);
static int __fastcall cm_d1(void *s, void *)
{ return (int)(size_t)_ZN11CommonModelD1Ev(s); }

extern "C" void port_fill_common_model_vtable(void)
{
    _ZTV11CommonModel[0] = (void *)cm_d1;
    _ZTV11CommonModel[1] = (void *)cm_dosetfile;   /* MSVC numbering */
    _ZTV11CommonModel[2] = (void *)cm_dosetfile;   /* ROM numbering */
}

static void hal_fill_common_model_vtable(void)
{ port_fill_common_model_vtable(); }

/* Coin::Render calls CommonModel::Render by its Itanium name from a TU that
   declared it extern "C", so the reference is cdecl and the definition is a
   real MSVC __thiscall method. A forwarding face, never an alias. The ROM's
   Render returns void and every caller ignores the result. */
extern "C" int _ZN11CommonModel6RenderEPK7Vector3(void *self, const void *scale)
{
    ((CommonModel *)self)->CommonModel::Render((const Vector3 *)scale);
    return 1;
}

/* The coin's collect path reaches the red-coin star by C name from
   func_ov002_020b10a0, and the definition is a real MSVC method. The shadow
   declaration is the CALLER'S: a decorated name is its own name, its class's
   name, its calling convention and its parameter types, nothing about
   layout, so one method is the whole declaration that is needed. */
struct StarMarkerFace { void SpawnRedCoinStarIfNecessary(); };
#pragma comment(linker, "/alternatename:?SpawnRedCoinStarIfNecessary@StarMarkerFace@@QAEXXZ=?SpawnRedCoinStarIfNecessary@StarMarker@@QAEXXZ")
extern "C" void _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv(void *self)
{ ((StarMarkerFace *)self)->SpawnRedCoinStarIfNecessary(); }

// ---- COIN (288), RED_COIN (289), BLUE_COIN (290) -- ov002 ------------------
//
// _ZTV4Coin, ov002 0x021087ec, RTTI 8daCoin_c. ONE CLASS, THREE IDS: the
// three factories are identical (948 bytes, Actor base, two CommonModels, a
// ShadowModel, a MovingCylinderClsn and a WithMeshClsn) and all three write
// this table, so one fill serves all three registry rows. The class reads
// mActorID back to tell which it is -- CleanupResources releases the red
// coin's own SharedFilePtr on 0x121, Behavior retires a blue coin's area on
// 0x122.
//
// Bob-omb Battlefield names 60 coins and 8 red coins in group 0 alone, which
// is the eight-red-coin star. Blue coins come from a BlueCoinSwitch, so the
// level's tables never name 290; it is registered anyway because it is the
// same table and the same factory.
//
// Behavior is the HOST COPY in port/unmatched/Coin_Behavior.cpp: the ROM's
// dispatches the behaviour state through an mwcc pointer-to-member table.
//
// SLOT 16 IS THE ROM'S D0 BODY MINUS ITS FINAL Deallocate, written out here
// rather than compiled from src/_ZN4CoinD1Ev.cpp, which is a real C++
// destructor over its own shadow hierarchy. This slot is LIVE and has to be:
// AfterCleanupResources dispatches it and then deallocates, and a coin is
// destroyed every single time one is collected. Member order is the reverse
// of the factory's construction order, which is what the ROM's own D0 does.
extern "C" {
int _ZN4Coin13InitResourcesEv(char *self);
int _ZN4Coin16CleanupResourcesEv(char *self);
int _ZN4Coin8BehaviorEv(void *self);            /* port/unmatched */
int func_ov002_020b2a90(void);                  /* OnYoshiTryEat */
void func_ov002_020b2a34(char *self, int arg);  /* OnTurnIntoEgg */
void *_ZTV4Coin[31];
void port_coin_states_seat(void);               /* port/unmatched */
/* the five member destructors the D1 body runs, in reverse order */
void _ZN12WithMeshClsnD1Ev(void *);
void *_ZN18MovingCylinderClsnD1Ev(void *);
void _ZN11ShadowModelD1Ev(void *);
void *_ZN11CommonModelD1Ev(void *);
void *_ZN5ActorD2Ev(void *);
}
/* Coin's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV8daCoin_c=__ZTV4Coin")
/* Gate 204: slot 17, the coin's own deleting destructor. src/_ZN4CoinD0Ev.c
   has been on slice_gate33.txt since that gate with nothing referencing it,
   so /OPT:REF dropped it before the map was written. */
extern "C" int *_ZN4CoinD0Ev(int *self);

#include "Coin.h"

static int __fastcall coin_init(void *s, void *)
{ return _ZN4Coin13InitResourcesEv((char *)s); }
static int __fastcall coin_clean(void *s, void *)
{ return _ZN4Coin16CleanupResourcesEv((char *)s); }
static int __fastcall coin_behavior(void *s, void *)
{ return _ZN4Coin8BehaviorEv(s); }
/* SM64DS_ACTOR_PROBE=1 for a CommonModel. The shared probe in
   hal/actor_classes.cpp reads a MODEL's layout (file at +0x0c, transforms at
   +0x14, matrix at +0x1c) and a CommonModel is a different, shorter object --
   vptr, modelFile at +0x04, the pooled components at +0x08, its 0x30-byte
   matrix at +0x0c and nothing after. Handing the shared probe one prints four
   numbers read past the end of it. */
static void coin_model_probe(const char *what, const char *m)
{
    static int on = -1;
    static int said;
    if (on < 0) on = std::getenv("SM64DS_ACTOR_PROBE") != 0;
    if (!on || said > 1) return;
    ++said;
    const int *t = (const int *)(m + 0x0c + 0x24);   /* mat4x3 translation */
    std::printf("[actor] %-17s model %p file %p pool %p mat.t (%d,%d,%d) "
                "scene\n", what, (const void *)m,
                *(void *const *)(m + 0x04), *(void *const *)(m + 0x08),
                t[0] >> 12, t[1] >> 12, t[2] >> 12);
}

static int __fastcall coin_render(void *s, void *)
{
    /* Two CommonModels: 0xd8 is the one drawn with flag 0x10 clear and 0x114
       the one drawn with it set. */
    coin_model_probe("COIN", (const char *)s + 0xd8);
    return ((Coin *)s)->Coin::Render();
}
static int __fastcall coin_d1(void *s, void *)
{
    *(void **)s = (void *)_ZTV4Coin;
    _ZN12WithMeshClsnD1Ev((char *)s + 0x1ac);
    _ZN18MovingCylinderClsnD1Ev((char *)s + 0x178);
    _ZN11ShadowModelD1Ev((char *)s + 0x150);
    _ZN11CommonModelD1Ev((char *)s + 0x114);
    _ZN11CommonModelD1Ev((char *)s + 0xd8);
    _ZN5ActorD2Ev(s);
    return (int)(size_t)s;
}
static int __fastcall coin_d0(void *s, void *)
{ return (int)(size_t)_ZN4CoinD0Ev((int *)s); }
static int __fastcall coin_yoshi(void *, void *)
{ return func_ov002_020b2a90(); }
static int __fastcall coin_egg(void *s, void *, int a)
{ func_ov002_020b2a34((char *)s, a); return 0; }

// ---- the coin's models are preloaded by Stage::InitResources ---------------
//
// Coin::InitResources does NOT load its own file for coin types 0 and 1. It
// reads data_ov002_020ff06c[type]->filePtr and hands it straight to
// ModelBase::SetFile, on the assumption that somebody already loaded it. That
// somebody is Stage::InitResources (arm9 0x0202cc0c), which walks a four-entry
// table of SharedFilePtr* at arm9 0x020756f0 and loads each one. The four
// entries -- and config/arm9/relocs.txt names all four -- are exactly the
// coin's two pairs:
//
//     0x020756f0 -> ov002 0x0210da48   file 0x8006   coin type 0, model B
//     0x020756f4 -> ov002 0x0210d9b8   file 0x8005   coin type 0, model A
//     0x020756f8 -> ov002 0x0210da50   file 0x8008   coin type 1, model B
//     0x020756fc -> ov002 0x0210d9f8   file 0x8007   coin type 1, model A
//
// Stage::InitResources DOES NOT RUN IN THE PORT: hal/stage_bridges.cpp fills
// every one of the Stage's twenty slots with a trap, and gate 26 landed the
// Stage as a scene root rather than as a running actor. Without the preload a
// spawned coin walks straight into Model::LoadTexAndPal with a null BMD_File
// and faults at +0xc -- which is exactly what the first run did, and the
// backtrace named every frame of it.
//
// So the load happens here, out of the same four SharedFilePtrs by name, with
// the same Model::LoadFile the ROM calls. Nothing is invented: the fileIDs are
// asserted against the ones __sinit_ov002_02100560 constructed them with, so a
// mount that has drifted says so instead of loading a stranger's archive.
// WHEN Stage::InitResources IS HOSTED THIS COMES OUT WHOLE -- it is one call
// and one table, in the wrong place only because its owner is not running.
extern "C" {
void *_ZN5Model8LoadFileER13SharedFilePtr(void *ptr);
extern unsigned char data_ov002_0210da48[], data_ov002_0210d9b8[],
    data_ov002_0210da50[], data_ov002_0210d9f8[];
}

static void port_coin_models_preload(void)
{
    static const struct { unsigned char *ptr; unsigned short file;
                          const char *what; }
    k[] = {
        {data_ov002_0210da48, 0x8006, "coin type 0 model B"},
        {data_ov002_0210d9b8, 0x8005, "coin type 0 model A"},
        {data_ov002_0210da50, 0x8008, "coin type 1 model B"},
        {data_ov002_0210d9f8, 0x8007, "coin type 1 model A"},
    };
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; ++i) {
        unsigned short id = *(unsigned short *)k[i].ptr;
        if (id != k[i].file) {
            std::fprintf(stderr, "FATAL: %s: the sinit constructed file %04x, "
                         "the ROM's own table says %04x -- WRONG BYTES\n",
                         k[i].what, id, k[i].file);
            std::abort();
        }
        _ZN5Model8LoadFileER13SharedFilePtr(k[i].ptr);
    }
}

extern "C" void hal_fill_coin_vtable(void)
{
    void **vt = _ZTV4Coin;
    hal_fill_common_model_vtable();
    port_coin_models_preload();
    port_coin_states_seat();
    bw_fill_shared(vt);
    vt[0] = (void *)coin_init;
    vt[3] = (void *)coin_clean;
    vt[6] = (void *)coin_behavior;
    vt[9] = (void *)coin_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)coin_d1;
    /* 17 IS THE ROM'S OWN WORD NOW (gate 204). Nothing dispatches the deleting
       form -- the destroy path is the D1 dispatch plus an explicit Deallocate
       by AfterCleanupResources -- so this is not a behaviour change; it is the
       port's copy of the table carrying what the ROM parks there, and it is
       the reference edge that pulls the matched D0 into the link. */
    vt[17] = (void *)coin_d0;
    vt[18] = (void *)coin_yoshi;
    vt[19] = (void *)coin_egg;
}

// ============================================================================
// The rest of the roster whose overlay is ALREADY MOUNTED: ov002 (the engine
// overlay, since gate 14), ov098 (gate 19), ov100 (gate 21) and ov102 (gate
// 23). Each of these is a registry row, a vtable fill and nothing else -- no
// new mount, no new sinit.
//
// Every vtable below was checked against the ROM's Itanium RTTI, because the
// ov002 names ARE shifted through this run and the shift is what decides
// which body a factory really installs:
//
//   id   SpawnInfo                    installs              RTTI
//   15   BrickBlock_SpawnInfo         _ZTV13BigBrickBlock   13daObjBlockL_c
//   322  MegaMushroomBlockTag_...     _ZTV10BrickBlock      19daObjBlockItemTag_c
//   14   CannonHatch_SpawnInfo        _ZTV11CannonHatch     20daObjCannonShutter_c
//   269  Cap_SpawnInfo                _ZTV13WaterfallMist   15daObjMarioCap_c
//   297  HealingHeart_SpawnInfo       _ZTV7Seaweed          12daObjHeart_c
//   11   ExclamationSwitch_SpawnInfo  _ZTV10StarSwitch      13daObjSwitch_c
//   329  InvisibleSecret_SpawnInfo    _ZTV15InvisibleSecret 13daObjNumber_c
//   299  ArrowSignLeft_SpawnInfo      _ZTV14ArrowSignRight  15daObjYajirusi_c
//   300  ArrowSignRight_SpawnInfo     _ZTV14ArrowSignRight  (same table)
//   208  WaterBomb_SpawnInfo          _ZTV9WaterBomb        7daWbm_c
//   220  RollingIronBall_SpawnInfo    _ZTV15RollingIronBall 7daIbl_c
//   298  WarpPipe_SpawnInfo           _ZTV13FortressTower   15daObjSimpleBg_c
//
// The ROM type names read: block-large, block-item-tag, cannon-shutter,
// mario-cap, heart, switch, number, yajirushi (arrow), water-bomb, iron-ball,
// simple-background. Every one is the thing its SpawnInfo says it is, and
// none is the thing its vtable config name says. The port uses the file the
// vtable points at and renames nothing, which is the gate-20 rule.
//
// BRICK_BLOCK (15) HAS NO FILL and that is not an omission: its factory
// installs _ZTV13BigBrickBlock, which gate 16 already fills for
// BLACK_BRICK_BLOCK (17). One class body serves six actor ids and switches on
// its own, so 15 is a registry row and nothing more.
//
// SLOT 17 TRAPS ON EVERY CLASS HERE, the gate-17 and Bird reading: the ROM's
// destroy path is AfterCleanupResources dispatching slot 16 and then
// deallocating itself, so the deleting form is never called. Slot 16 is live
// on all of them.

extern "C" {
void hal_fill_platform_vtable(void);      /* hal/actor_classes.cpp */
void _ZN15TextureSequenceD1Ev(void *);
void _ZN9ModelAnimD1Ev(void *);
void _ZN5ModelD1Ev(void *);
}

// ---- MEGA_MUSHROOM_BLOCK_TAG (322, ov002) x4 -------------------------------
//
// _ZTV10BrickBlock, ov002 0x02108c18, RTTI 19daObjBlockItemTag_c. A tag rather
// than a block: 220 bytes, no model of its own (slot 9 is ActorBase::Render in
// the ROM's own table), parked on top of another object. Slot 16 is the ROM's
// D0 minus its Deallocate, which for this class is the vtable store and
// Actor's D2 -- it has no members to destroy.
//
// THE COUNT AND THE LEVELS ARE COUNTED, not read off an object table. A boot
// of all fifty mounted levels reading each census puts 322 on level 0 x1,
// level 13 x2 and level 44 x1 -- FOUR, and NOT ONE ON LEVEL 6. The "x8 on
// Bob-omb Battlefield" this header carried came from ov014's object tables
// rather than from a boot; no level-6 census has ever produced one. The other
// three ids of this class sit at 321 (levels 0/7/42), 323 (0/8/10) and
// 324 (0/42x2), and level 0 is the only level carrying all four.
#include "BrickBlock.h"
extern "C" {
int _ZN10BrickBlock13InitResourcesEv(char *self);
void *_ZTV10BrickBlock[31];
}
static int __fastcall mmbt_init(void *s, void *)
{ return _ZN10BrickBlock13InitResourcesEv((char *)s); }
static int __fastcall mmbt_clean(void *s, void *)
{ return ((BrickBlock *)s)->BrickBlock::CleanupResources(); }
static void itemtag_probe_tick(char *tag);
static int __fastcall mmbt_behavior(void *s, void *)
{ itemtag_probe_tick((char *)s); return ((BrickBlock *)s)->BrickBlock::Behavior(); }
static int __fastcall mmbt_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall mmbt_d1(void *s, void *)
{
    *(void **)s = (void *)_ZTV10BrickBlock;
    _ZN5ActorD2Ev(s);
    return (int)(size_t)s;
}

// ---- THE FOUR-CELL ITEM-TAG DISPATCH SEAT ----------------------------------
//
// BrickBlock::Behavior's last act is to dispatch through
// data_ov002_0210dd30[unk_0d7] and destroy itself. __sinit_ov002_02100e50
// fills that bss table from four ROM {fn, adj} records, and the function word
// of each is a RAW DS TEXT ADDRESS: ovdata.py rebases only words inside its
// data window (0x02100000 up), and ov002's .text ends at 0x020ff014, so a
// pointer into .text is outside the window by construction and no pass
// rewrites it. Calling one on the host is a jump into mapped-but-NX memory.
//
// So the table is rewritten here, the Ukiki way (hal/actor_classes_ov030.cpp):
// {the ROM address the sinit's own source record carries, the host body}, with
// the ROM column CHECKED against the mounted bytes before anything is written.
// A mount that ever points at the wrong bytes aborts rather than calling into
// garbage. The rewrite is idempotent by VALUE rather than by a `done` flag, so
// a re-run over an already-seated table is a no-op and a re-run after a fresh
// sinit re-seats.
//
// THE THUNKS ARE __fastcall AND THAT IS MEASURED, NOT ASSUMED (R11). The
// dispatch site in the built object is
//     mov ecx,_data_ov002_0210dd30[eax+4] / mov eax,_data_ov002_0210dd30[eax]
//     add ecx,edi / call eax
// -- a real call with the receiver in ECX and nothing pushed, which is what
// R11 says needs a thunk. It cannot be a tail-jump forwarder: the TU calls
// MarkForDestruction after the dispatch and returns 1.
//
// The stride the site uses is the other half of the fix and lives in
// port/CMakeLists.txt (R12): without /vmg /vmm MSVC gives this TU's PMF the
// sixteen-byte general representation and `shl eax,4` walks off the end of a
// thirty-two-byte table.
//
// The four ids share this one class and pick their cell at InitResources:
//   idx 0  id 321 ONE_UP_MUSHROOM_BLOCK_TAG   ROM 0x020b429c  spawns 0x114
//   idx 1  id 322 MEGA_MUSHROOM_BLOCK_TAG     ROM 0x020b4250  spawns 0x115
//   idx 2  id 323 GREEN_SHELL_BLOCK_TAG       ROM 0x020b41f8  spawns 0x11d
//   idx 3  id 324 SILVER_STAR_BLOCK_TAG       ROM 0x020b42e4  star + marker
extern "C" {
void func_ov002_020b429c(char *self);
void func_ov002_020b4250(char *self);
void func_ov002_020b41f8(char *self);
void func_ov002_020b42e4(char *self);
}

// ---- SM64DS_TAG_PROBE: the headless exercise -------------------------------
//
// There is no headless way to make the game break a block. The tag's dispatch
// fires when daObjBlockL_c::Kill (src/func_ov002_020b38a0.c) notifies the tag
// through src/func_ov002_020b363c.c, and Kill runs off a punch, a ground pound
// or a mega collision -- input this port's scripted runs cannot deliver.
//
// So the probe drives the ROM'S OWN break path rather than poking the flag.
// With SM64DS_TAG_PROBE=1, an ATTACHED tag (unk_0d8 set, meaning its Behavior
// already found a block and stored itself in that block's +0x328) finds that
// block again by the same predicate the ROM uses and calls Kill on it. Kill
// calls the ROM's notifier, the notifier writes unk_0d6, and the tag's very
// next Behavior takes the dispatch branch on its own. Nothing here writes
// unk_0d6 and nothing here calls a handler; the only host code in the path is
// the walk that finds the block.
//
// Both ends print, so a run shows which cell was selected and which body it
// reached: the probe names the tag's id and index before the kill, and each
// thunk names itself when it is entered.
//
// THE STANDING EXERCISE IS SM64DS_LEVEL=0 SM64DS_TAG_PROBE=1, on SHIPPED
// placements and with no spawn override: level 0 is the only level carrying
// all four ids, one of each, so a single run drives cells 0, 1, 2 and 3 in
// order. Measured rc 0 under FAULTS_FATAL.
//
// SM64DS_SPAWN_ACTOR=15,<id> (a brick block and a tag together at the player)
// is the fallback for exercising one id on a level that does not place it.
extern "C" {
void func_ov002_020b38a0(char *self);          /* daObjBlockL_c::Kill */
void *_ZN5Actor4NextEPKS_(const void *prev);
}
static int itemtag_probe_on(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_TAG_PROBE");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v;
}
static void itemtag_probe_note(int idx, void *s)
{
    if (!itemtag_probe_on())
        return;
    std::printf("[tagprobe] DISPATCHED cell %d -> host body entered, self=%p\n",
                idx, s);
}
static void itemtag_probe_tick(char *tag)
{
    if (!itemtag_probe_on())
        return;
    if (*(unsigned char *)(tag + 0xd8) == 0)   /* not attached to a block yet */
        return;
    if (*(unsigned char *)(tag + 0xd6) != 0)   /* already notified */
        return;
    for (char *o = (char *)_ZN5Actor4NextEPKS_(0); o;
         o = (char *)_ZN5Actor4NextEPKS_(o)) {
        /* the ROM's own predicate: only 0xf / 0x10 / 0x11 carry a +0x328, and
           reading it on anything else is a read past the end of the actor */
        unsigned t = *(unsigned short *)(o + 0xc);
        if (t != 0xf && t != 0x10 && t != 0x11)
            continue;
        if (*(char **)(o + 0x328) != tag)
            continue;
        std::printf("[tagprobe] tag %p id %u cell %u attached to block %p "
                    "id %u -- calling daObjBlockL_c::Kill\n",
                    (void *)tag, *(unsigned short *)(tag + 0xc),
                    *(unsigned char *)(tag + 0xd7), (void *)o, t);
        func_ov002_020b38a0(o);
        return;
    }
}

static void __fastcall itemtag_state0(void *s)
{ itemtag_probe_note(0, s); func_ov002_020b429c((char *)s); }
static void __fastcall itemtag_state1(void *s)
{ itemtag_probe_note(1, s); func_ov002_020b4250((char *)s); }
static void __fastcall itemtag_state2(void *s)
{ itemtag_probe_note(2, s); func_ov002_020b41f8((char *)s); }
static void __fastcall itemtag_state3(void *s)
{ itemtag_probe_note(3, s); func_ov002_020b42e4((char *)s); }

struct PortItemTagCell { unsigned fn, delta; };
extern "C" PortItemTagCell data_ov002_0210dd30[4];

typedef void(__fastcall *PortItemTagFn)(void *);
static const struct { unsigned rom; PortItemTagFn host; } g_itemtag_cells[4] = {
    { 0x020b429cu, itemtag_state0 },
    { 0x020b4250u, itemtag_state1 },
    { 0x020b41f8u, itemtag_state2 },
    { 0x020b42e4u, itemtag_state3 },
};

extern "C" void port_itemtag_states_seat(void)
{
    for (int i = 0; i < 4; ++i) {
        PortItemTagCell &cell = data_ov002_0210dd30[i];
        unsigned host = (unsigned)(size_t)g_itemtag_cells[i].host;
        if (cell.fn == host && cell.delta == 0)
            continue;                       /* already seated */
        if (cell.fn != g_itemtag_cells[i].rom || cell.delta != 0) {
            std::fprintf(stderr, "FATAL: item-tag state cell %d: the sinit "
                         "left %08x/%u, the ROM's own record says %08x/0 -- "
                         "WRONG BYTES\n", i, cell.fn, cell.delta,
                         g_itemtag_cells[i].rom);
            std::abort();
        }
        cell.fn = host;
    }
}

extern "C" void hal_fill_mega_mushroom_block_tag_vtable(void)
{
    /* Seat and verify the four cells BEFORE anything can dispatch through
       them. The registry runs every fill on each level boot, which is after
       main() has run ov002's constructors, so the ROM words are in place by
       the time this reads them. */
    port_itemtag_states_seat();
    void **vt = _ZTV10BrickBlock;
    bw_fill_shared(vt);
    vt[0] = (void *)mmbt_init;
    vt[3] = (void *)mmbt_clean;
    vt[6] = (void *)mmbt_behavior;
    vt[9] = (void *)mmbt_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)mmbt_d1;
    vt[17] = (void *)bw_trap17;
}

// ---- CANNON_HATCH (14, ov002) x6 -------------------------------------------
//
// _ZTV11CannonHatch, ov002 0x02109d38, RTTI 20daObjCannonShutter_c. The lid on
// a cannon: a Platform subclass with its own MovingMeshCollider, so it is
// walkable while the cannon is shut. Bob-omb Battlefield names fourteen
// CANNONs (gate 19's class) and six of these.
//
// Its Behavior is C-named in its own TU even though include/CannonHatch.h
// declares it as a method, so the thunk calls the C name; taking the header at
// its word would ask for a symbol nothing defines.
#include "CannonHatch.h"
extern "C" {
int _ZN11CannonHatch8BehaviorEv(char *self);
int *_ZN11CannonHatchD1Ev(int *self);
void *_ZTV11CannonHatch[32];
}
#pragma comment(linker, "/alternatename:__ZTV20daObjCannonShutter_c=__ZTV11CannonHatch")
static int __fastcall ch_init(void *s, void *)
{ return ((CannonHatch *)s)->CannonHatch::InitResources(); }
static int __fastcall ch_clean(void *s, void *)
{ return ((CannonHatch *)s)->CannonHatch::CleanupResources(); }
static int __fastcall ch_behavior(void *s, void *)
{ return _ZN11CannonHatch8BehaviorEv((char *)s); }
static int __fastcall ch_render(void *s, void *)
{
    port_actor_render_probe("CANNON_HATCH", (char *)s + 0xd4);
    return ((CannonHatch *)s)->CannonHatch::Render();
}
static int __fastcall ch_d1(void *s, void *)
{ return (int)(size_t)_ZN11CannonHatchD1Ev((int *)s); }
extern "C" void hal_fill_cannon_hatch_vtable(void)
{
    void **vt = _ZTV11CannonHatch;
    hal_fill_platform_vtable();
    bw_fill_shared(vt);
    vt[0] = (void *)ch_init;
    vt[3] = (void *)ch_clean;
    vt[6] = (void *)ch_behavior;
    vt[9] = (void *)ch_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)ch_d1;
    vt[17] = (void *)bw_trap17;
    /* 32 slots; slot 31 is Platform::Kill, not overridden in the ROM table. */
    vt[31] = (void *)bw_kill;
}

// ---- SHUTTER_BOB (40, ov014) x2 --------------------------------------------
//
// _ZTV10ShutterBob (ov014 0x02114608, RTTI daObjBSwdoor_c -- "B swing door").
// Bob-omb Battlefield's own shutter, and a Platform subclass with a moving
// mesh collider, the exact CANNON_HATCH shape. Level 6's census printed id 40
// skipped x2; ACTOR_SPAWN_TABLE entry [40] relocates to ShutterBob_SpawnInfo
// at ov014 0x021145e4 (that record's +4 reads 40), so the resolution is the
// ROM's own. ov014 is already mounted (the chain chomp gate), and the
// SpawnInfo plus the class's one data reference (data_ov014_021145c4) come out
// of ov014_syms.txt, so this is a HAL-only class.
//
// The 32-slot vtable overrides 0/3/6/16/17. Slot 9 is func_ov002_020babf0,
// the shared Platform model-render (the class has no Render of its own), and
// slot 12 is ActorBase::OnPendingDestroy. Behavior/Init/Cleanup are real C++
// methods (call qualified against include/ShutterBob.h); D1/D0 are the C-form
// .c destructors, and slot 17 is a real deleting dtor rather than a trap. The
// vtable is a HOST array the registry fills (the ov085/ov080 rule) -- it is
// deliberately NOT in ov014_syms.txt, which mounting would hand a factory DS
// code addresses.
#include "ShutterBob.h"
extern "C" {
int func_ov002_020babf0(void *self);            /* the Platform model render */
int *_ZN10ShutterBobD1Ev(int *self);            /* .c, C linkage */
int *_ZN10ShutterBobD0Ev(int *self);            /* .c, C linkage */
/* 32, not 20: the ROM's table is the full Platform width, and construction can
   dispatch a slot past 19. A [20] host array leaves 20..31 reading adjacent
   memory -- a wild call. */
void *_ZTV10ShutterBob[32];
}
/* the destructors spell the class's own table by its RTTI name */
#pragma comment(linker, "/alternatename:__ZTV14daObjBSwdoor_c=__ZTV10ShutterBob")
/* InitResources reads data_ov014_021145c4 as a C++ `extern int`, which MSVC
   mangles to ?data_ov014_021145c4@@3HA; the mounted symbol is the C-linkage
   _data_ov014_021145c4. Alias it, byte-faithful (the STAR_MARKER precedent). */
#pragma comment(linker, "/alternatename:?data_ov014_021145c4@@3HA=_data_ov014_021145c4")
/* InitResources references MeshColliderBase::Enable with a CLASS-tagged Actor
   (?Enable@MeshColliderBase@@QAEXPAVActor@@@Z), which no Actor.h-including TU
   can emit -- hal/shutter_bob_face.cpp defines exactly that symbol, forwarding
   to the C-linkage host wrapper. See that file's header. */
static int __fastcall shb_init(void *s, void *)
{ return ((ShutterBob *)s)->ShutterBob::InitResources(); }
static int __fastcall shb_clean(void *s, void *)
{ return ((ShutterBob *)s)->ShutterBob::CleanupResources(); }
static int __fastcall shb_behavior(void *s, void *)
{ return ((ShutterBob *)s)->ShutterBob::Behavior(); }
static int __fastcall shb_render(void *s, void *)
{
    port_actor_render_probe("SHUTTER_BOB", (char *)s + 0xd4);
    return func_ov002_020babf0(s);
}
static int __fastcall shb_d1(void *s, void *)
{ return (int)(size_t)_ZN10ShutterBobD1Ev((int *)s); }
static int __fastcall shb_d0(void *s, void *)
{ return (int)(size_t)_ZN10ShutterBobD0Ev((int *)s); }
extern "C" void hal_fill_shutter_bob_vtable(void)
{
    void **vt = _ZTV10ShutterBob;
    hal_fill_platform_vtable();
    bw_fill_shared(vt);
    vt[0] = (void *)shb_init;
    vt[3] = (void *)shb_clean;
    vt[6] = (void *)shb_behavior;
    vt[9] = (void *)shb_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)shb_d1;
    vt[17] = (void *)shb_d0;
    /* The Platform tail. ShutterBob overrides NOTHING in 20..31: the ROM table
       carries Actor's own bodies at 20..30 and Platform::Kill at 31, so the
       shared fill's tail is already right and only Kill is left to write.
       Slots 29 and 31 used to trap on the reading that nothing reaches them;
       the ROM says what they are, so they run it. */
    vt[31] = (void *)bw_kill;
}

// ---- SEESAW_BOB (39, ov095) x1 ---------------------------------------------
//
// _ZTV9SeesawBob (ov095 0x021374fc, RTTI daObjSeesaw_c). Bob-omb Battlefield's
// tilting seesaw bridge, and a Platform subclass like the shutter. Level 6's
// census printed id 39 skipped x1; ACTOR_SPAWN_TABLE entry [39] relocates to
// SeesawBob_SpawnInfo at ov095 0x02137484 (that record's +4 reads 39), so the
// resolution is the ROM's own. ov095 is a fresh per-symbol mount
// (ov095_syms.txt); the SpawnInfo, its three file-pointer tables and the
// SharedFilePtr statics the two ov095 sinits build all come from there.
//
// The 32-slot vtable overrides 0/3/6/9/16/17 AND slot 21 -- SeesawBob has a
// real Render of its own (unlike the shutter) and its own OnGroundPounded
// (func_ov095_021357d8, the rider's weight tilting the plank). Slot 12 is
// ActorBase::OnPendingDestroy. Init/Clean/Behavior/Render are real C++ methods
// (call qualified against include/SeesawBob.h); D1/D0 are the C-form .c
// destructors. SeesawBob_Spawn is the matched src -- it installs _ZTV9SeesawBob
// with a single store, so no host copy is needed. The vtable is a HOST array
// the registry fills (the ov085/ov080 rule), deliberately NOT in ov095_syms.txt.
#include "SeesawBob.h"
extern "C" {
int func_ov095_021357d8(char *self, char *other);   /* slot 21, OnGroundPounded */
int *_ZN9SeesawBobD1Ev(int *self);                   /* .c, C linkage */
int *_ZN9SeesawBobD0Ev(int *self);                   /* .c, C linkage */
void *_ZTV9SeesawBob[32];
}
/* the destructors spell the class's own table by its RTTI name */
#pragma comment(linker, "/alternatename:__ZTV13daObjSeesaw_c=__ZTV9SeesawBob")
/* InitResources takes the ADDRESS of MeshColliderBase::UpdatePosWithTransform,
   declared `extern int [...][]`, so MSVC mangles the reference as a DATA name
   (?...@@3PAHA). The real static's MSVC symbol is the SAX... method; alias the
   data spelling onto it -- the UpdatePosAndAngs @@3PAXA precedent in
   cxx_aliases.cpp. */
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3PAHA=?UpdatePosWithTransform@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
static int __fastcall ssb_init(void *s, void *)
{ return ((SeesawBob *)s)->SeesawBob::InitResources(); }
static int __fastcall ssb_clean(void *s, void *)
{ return ((SeesawBob *)s)->SeesawBob::CleanupResources(); }
static int __fastcall ssb_behavior(void *s, void *)
{ return ((SeesawBob *)s)->SeesawBob::Behavior(); }
/* the C name, not the C++ method: Render is a ModelAnim slot-5 host copy in
   port/unmatched/ModelAnim_Renders.cpp (the Whomp/UpDownLiftBbh case), so the
   matched src is dropped from slice_gate83.txt and this dispatches the host copy
   by its extern-"C" name, the way ssb_behavior calls the class method. */
extern "C" int _ZN9SeesawBob6RenderEv(void *self);   /* ModelAnim slot-5 host copy */
static int __fastcall ssb_render(void *s, void *)
{
    port_actor_render_probe("SEESAW_BOB", (char *)s + 0xd4);
    return _ZN9SeesawBob6RenderEv(s);
}
/* Slot 21 takes a second argument (the pounder) in r1 on the ROM side. On the
   host it does NOT ride the __fastcall thunk's second slot: that slot is edx,
   a register, and the caller of a thiscall vtable slot puts everything past
   `this` on the STACK. A two-parameter thunk therefore read edx as the
   pounder and, worse, popped nothing, so the caller's epilogue came up one
   slot short and returned to its own saved EBP. The third parameter is the
   pushed word, and declaring it both delivers the real pounder and makes the
   thunk `ret 4`, the same shape crate_egg and whomp_mega already use. */
static int __fastcall ssb_pounded(void *s, void *, void *pounder)
{ func_ov095_021357d8((char *)s, (char *)pounder); return 0; }
static int __fastcall ssb_d1(void *s, void *)
{ return (int)(size_t)_ZN9SeesawBobD1Ev((int *)s); }
static int __fastcall ssb_d0(void *s, void *)
{ return (int)(size_t)_ZN9SeesawBobD0Ev((int *)s); }
extern "C" void hal_fill_seesaw_bob_vtable(void)
{
    void **vt = _ZTV9SeesawBob;
    hal_fill_platform_vtable();
    bw_fill_shared(vt);
    vt[0] = (void *)ssb_init;
    vt[3] = (void *)ssb_clean;
    vt[6] = (void *)ssb_behavior;
    vt[9] = (void *)ssb_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)ssb_d1;
    vt[17] = (void *)ssb_d0;
    /* The Platform tail. SeesawBob overrides exactly one of it -- slot 21,
       OnGroundPounded (ov095 0x021357d8, the rider's weight tilting the
       plank), which IS in the build. The rest are Actor's own bodies plus
       Platform::Kill, which is what the ROM table holds; the shared fill wrote
       20..30 already. */
    vt[21] = (void *)ssb_pounded;
    vt[31] = (void *)bw_kill;
}

// ---- CAP (269, ov002) -------------------------------------------------------
//
// _ZTV13WaterfallMist, ov002 0x021095f0, RTTI 15daObjMarioCap_c. Mario's cap,
// knocked off and lying on the ground: an Enemy subclass (1040 bytes) with a
// ModelAnim, a ShadowModel and both collision shapes. One in Bob-omb
// Battlefield and six on the castle grounds.
//
// THE VTABLE IS NOT THE ONE GATE 20 FILLS. Gate 20's WATERFALL_MIST (197)
// installs _ZTV18PoppingLavaBubbles, whose RTTI is 16daObjWaterfall_c; the
// table spelled _ZTV13WaterfallMist is this one. Two tables, two classes, and
// the config names are one apart on both.
//
// Slots 18 and 19 are its own: the cap answers Yoshi and has a turn-into-egg
// body, which for a hat is the ROM's own arrangement.
#include "WaterfallMist.h"
extern "C" {
void _ZN13WaterfallMist16OnPendingDestroyEv(char *c);
int *_ZN13WaterfallMistD1Ev(int *self);
int *_ZN13WaterfallMistD0Ev(int *self);        /* slot 17, gate 204 */
int func_ov002_020b8270(void);
void func_ov002_020b81e0(char *self, int arg);
void *_ZTV13WaterfallMist[31];
}
/* Gate 204: the cap's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV15daObjMarioCap_c=__ZTV13WaterfallMist")
static int __fastcall cap_init(void *s, void *)
{ return ((WaterfallMist *)s)->WaterfallMist::InitResources(); }
static int __fastcall cap_clean(void *s, void *)
{ return ((WaterfallMist *)s)->WaterfallMist::CleanupResources(); }
static int __fastcall cap_behavior(void *s, void *)
{ return ((WaterfallMist *)s)->WaterfallMist::Behavior(); }
static int __fastcall cap_render(void *s, void *)
{
    port_actor_render_probe("CAP", (char *)s + 0x300);
    return ((WaterfallMist *)s)->WaterfallMist::Render();
}
/* OnPendingDestroy takes `this`; the earlier thunk called it with no argument,
   so at level teardown MarkForDestruction ran WaterfallMist::OnPendingDestroy
   over a garbage stack word and faulted. Pass the cap object. */
static int __fastcall cap_pdes(void *s, void *)
{ _ZN13WaterfallMist16OnPendingDestroyEv((char *)s); return 0; }
static int __fastcall cap_d1(void *s, void *)
{ return (int)(size_t)_ZN13WaterfallMistD1Ev((int *)s); }
static int __fastcall cap_d0(void *s, void *)
{ return (int)(size_t)_ZN13WaterfallMistD0Ev((int *)s); }
static int __fastcall cap_yoshi(void *, void *)
{ return func_ov002_020b8270(); }
static int __fastcall cap_egg(void *s, void *, int a)
{ func_ov002_020b81e0((char *)s, a); return 0; }
extern "C" void port_cap_states_seat(void);   /* port/unmatched */
extern "C" void hal_fill_cap_vtable(void)
{
    void **vt = _ZTV13WaterfallMist;
    /* gate 51: seat the nine-state PMF table __sinit_ov002_02101064 left as DS
       code addresses, the SignPost treatment -- WaterfallMist::Behavior
       dispatches it through +0x3bc. */
    port_cap_states_seat();
    bw_fill_shared(vt);
    vt[0] = (void *)cap_init;
    vt[3] = (void *)cap_clean;
    vt[6] = (void *)cap_behavior;
    vt[9] = (void *)cap_render;
    vt[12] = (void *)cap_pdes;
    vt[16] = (void *)cap_d1;
    /* 17: the ROM's own word, gate 204. Ends in func_ov002_020aed18 rather
       than Actor::D2 -- the cap is an Enemy subclass and that is Enemy's D2. */
    vt[17] = (void *)cap_d0;
    vt[18] = (void *)cap_yoshi;
    vt[19] = (void *)cap_egg;
}

// ---- HEALING_HEART (297, ov002) --------------------------------------------
//
// _ZTV7Seaweed, ov002 0x02109c74, RTTI 12daObjHeart_c. The spinning heart that
// refills the meter: 372 bytes, a ModelAnim at +0xd4 and a MovingCylinderClsn
// at +0x138. Slot 16 is the ROM's D0 minus its Deallocate.
#include "Seaweed.h"
extern "C" {
int _ZN7Seaweed16CleanupResourcesEv(char *self);
int _ZN7Seaweed8BehaviorEv(char *self);
void *_ZTV7Seaweed[31];
}
static int __fastcall hh_init(void *s, void *)
{ return ((Seaweed *)s)->Seaweed::InitResources(); }
static int __fastcall hh_clean(void *s, void *)
{ return _ZN7Seaweed16CleanupResourcesEv((char *)s); }
static int __fastcall hh_behavior(void *s, void *)
{ return _ZN7Seaweed8BehaviorEv((char *)s); }
/* the C name, not the C++ method: Seaweed::Render is a ModelAnim slot-5 host
   copy in port/unmatched/ModelAnim_Renders.cpp (the Whomp/UpDownLiftBbh case), so
   the matched src is dropped from slice_gate33.txt and this dispatches the host
   copy by its extern-"C" name. HEALING_HEART shares _ZTV7Seaweed, so this is the
   dispatch that actually faulted on the king-defeat path (frame 354). */
extern "C" int _ZN7Seaweed6RenderEv(void *self);   /* ModelAnim slot-5 host copy */
/* Gate 204: slot 17. The heart's own D0 spells its table by the RTTI name, so
   both spellings have to resolve to one object -- the CannonHatch/StarSwitch
   treatment three sections down, and the head of this file derives the shift
   that makes 12daObjHeart_c and _ZTV7Seaweed the same table. */
#pragma comment(linker, "/alternatename:__ZTV12daObjHeart_c=__ZTV7Seaweed")
extern "C" int *_ZN7SeaweedD0Ev(int *self);
static int __fastcall hh_render(void *s, void *)
{
    port_actor_render_probe("HEALING_HEART", (char *)s + 0xd4);
    return _ZN7Seaweed6RenderEv(s);
}
static int __fastcall hh_d1(void *s, void *)
{
    *(void **)s = (void *)_ZTV7Seaweed;
    _ZN18MovingCylinderClsnD1Ev((char *)s + 0x138);
    _ZN9ModelAnimD1Ev((char *)s + 0xd4);
    _ZN5ActorD2Ev(s);
    return (int)(size_t)s;
}
static int __fastcall hh_d0(void *s, void *)
{ return (int)(size_t)_ZN7SeaweedD0Ev((int *)s); }
extern "C" void hal_fill_healing_heart_vtable(void)
{
    void **vt = _ZTV7Seaweed;
    bw_fill_shared(vt);
    vt[0] = (void *)hh_init;
    vt[3] = (void *)hh_clean;
    vt[6] = (void *)hh_behavior;
    vt[9] = (void *)hh_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)hh_d1;
    /* 17: the ROM's own word, gate 204. hh_d1 above is this same body minus
       the closing Deallocate, which is the D1/D0 split every class here has. */
    vt[17] = (void *)hh_d0;
}

// ---- EXCLAMATION_SWITCH (11, ov002) ----------------------------------------
//
// _ZTV10StarSwitch, ov002 0x02109940, RTTI 13daObjSwitch_c. The floor switch
// that fires an event: a Platform subclass (852 bytes) with a
// MovingMeshCollider at +0x124, which is what makes it stand on. One on
// Bob-omb Battlefield.
//
// This is the class the port's own notes record as a config naming bug --
// ExclamationSwitch carries the Platform vtable. It does, in the sense that
// its constructor is Platform's; the table its factory installs is
// _ZTV10StarSwitch and the RTTI settles that it really is a switch.
#include "StarSwitch.h"
extern "C" {
int _ZN10StarSwitch13InitResourcesEv(char *self);
int *_ZN10StarSwitchD1Ev(int *self);
void *_ZTV10StarSwitch[32];
}
#pragma comment(linker, "/alternatename:__ZTV13daObjSwitch_c=__ZTV10StarSwitch")
static int __fastcall xs_init(void *s, void *)
{ return _ZN10StarSwitch13InitResourcesEv((char *)s); }
static int __fastcall xs_clean(void *s, void *)
{ return ((StarSwitch *)s)->StarSwitch::CleanupResources(); }
static int __fastcall xs_behavior(void *s, void *)
{ return ((StarSwitch *)s)->StarSwitch::Behavior(); }
static int __fastcall xs_render(void *s, void *)
{
    port_actor_render_probe("EXCLAMATION_SWITCH", (char *)s + 0xd4);
    return ((StarSwitch *)s)->StarSwitch::Render();
}
static int __fastcall xs_d1(void *s, void *)
{ return (int)(size_t)_ZN10StarSwitchD1Ev((int *)s); }
/* Slot 21, OnGroundPounded. The switch has its OWN body (ov002 0x020b9fec,
   StarSwitch_OnGroundPounded), not Actor's do-nothing: a ground pound is how
   this switch fires its event. The ROM body takes only `this` and ignores the
   pounder, but the slot's one linked dispatcher is thiscall and PUSHES the
   pounder (walk_window.exe 0x45de24: `mov ecx,this / push [ebp+0xc] /
   call [edx+0x54]`, and it does NO `add esp` afterwards, so the callee pops the
   word). A three-parameter __fastcall veneer reads `this` from ecx, names the
   pushed pounder so MSVC emits `ret 4`, and drops it -- the same shape
   ssb_pounded and bbb_pounded already ship. A two-parameter thunk would read
   edx as the pounder, pop nothing, and return the caller one slot short. */
extern "C" void func_ov002_020b9fec(char *self);   /* slice_gate33 */
static int __fastcall xs_pounded(void *s, void *, void *)
{ func_ov002_020b9fec((char *)s); return 0; }
extern "C" void port_exclamation_switch_states_seat(void);  /* port/unmatched */
extern "C" void hal_fill_exclamation_switch_vtable(void)
{
    void **vt = _ZTV10StarSwitch;
    /* gate 51: seat the five-state PMF table data_ov002_0210e00c that
       __sinit_ov002_02101588 left as DS code addresses. StarSwitch::Behavior
       reaches it through the two host-copied dispatchers (OneUpMushroom case,
       ExclamationSwitch_StateDispatch.cpp). */
    port_exclamation_switch_states_seat();
    hal_fill_platform_vtable();
    bw_fill_shared(vt);
    vt[0] = (void *)xs_init;
    vt[3] = (void *)xs_clean;
    vt[6] = (void *)xs_behavior;
    vt[9] = (void *)xs_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)xs_d1;
    vt[17] = (void *)bw_trap17;
    /* 32 slots. Slot 21 (OnGroundPounded) is the switch's own body, ov002
       0x020b9fec -- matched src, now enrolled in slice_gate33 -- seated through
       the thiscall veneer xs_pounded (ret 4, matching the sole linked dispatch
       site at 0x45de24). A ground pound now runs the body instead of declining.
       Slot 31 is Platform::Kill. */
    vt[21] = (void *)xs_pounded;
    vt[31] = (void *)bw_kill;
}

// ---- INVISIBLE_SECRET (329, ov002) x5 --------------------------------------
//
// THE CONFIG'S NAME IS SHIFTED HERE TOO, and this one is shifted onto a
// vtable dsd never named at all -- the ov100 case (a real actor vtable left as
// data_ovNNN_*), not the ov002 one. The chain, by address:
//
//   InvisibleSecret_SpawnInfo   ov002 0x0210b00c   actor 329
//   InvisibleSecret_Spawn       ov002 0x020f085c, and its ONE literal
//                               (relocs.txt: from 0x020f0890) is 0x0210b030
//   data_ov002_0210b030         the vtable it installs, RTTI 9daSCoin_c
//   _ZTV15InvisibleSecret       ov002 0x0210b0ec, RTTI 13daObjNumber_c, and
//                               the record it follows is Number_SpawnInfo
//                               at 0x0210b0c8
//
// So the table the config spells _ZTV15InvisibleSecret is NUMBER'S, the six
// src files spelled _ZN15InvisibleSecret* are Number's methods, and this
// class's own five slots are the unnamed func_ov002_020f0* family that
// 0x0210b030 points at. Its object is 276 bytes with one MovingCylinderClsn
// at +0xd4 -- which is what the factory builds, and which the Number files
// would not fit.
//
// Slot 9 is ActorBase::Render in the ROM's own table: five of these on
// Bob-omb Battlefield and none of them is drawn until it is touched.
extern "C" {
int func_ov002_020f07dc(char *self);     /* InitResources */
int func_ov002_020f069c(char *self);     /* CleanupResources */
int func_ov002_020f06c0(char *self);     /* Behavior */
int *func_ov002_020f03c4(int *self);     /* D1 */
/* The vtable is HOST STORAGE the registry fills, not mounted ROM bytes, so
   the name dsd gave the address is declared here rather than emitted by
   ovdata. The factory spells it by its RTTI name. */
DSSTATE_BEGIN
void *data_ov002_0210b030[31];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV9daSCoin_c=_data_ov002_0210b030")
static int __fastcall is_init(void *s, void *)
{ return func_ov002_020f07dc((char *)s); }
static int __fastcall is_clean(void *s, void *)
{ return func_ov002_020f069c((char *)s); }
static int __fastcall is_behavior(void *s, void *)
{ return func_ov002_020f06c0((char *)s); }
static int __fastcall is_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall is_d1(void *s, void *)
{ return (int)(size_t)func_ov002_020f03c4((int *)s); }
extern "C" void hal_fill_invisible_secret_vtable(void)
{
    void **vt = data_ov002_0210b030;
    bw_fill_shared(vt);
    vt[0] = (void *)is_init;
    vt[3] = (void *)is_clean;
    vt[6] = (void *)is_behavior;
    vt[9] = (void *)is_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)is_d1;
    vt[17] = (void *)bw_trap17;
}

// ---- INVISIBLE_POLE (287, ov002) -- gate 46 --------------------------------
//
// RTTI 7daBar_c. Its factory (InvisiblePole_Spawn, 0x020b0710) installs
// data_ov002_02108480, NOT the _ZTV13InvisiblePole the config's own
// _ZN13InvisiblePole* methods belong to -- the gate-20 name shift, resolved by
// address: the vtable's slot funcs are the func_ov002_020b0* family (05d0,
// 0600, 0644, 064c, 0650, 0658, 067c), and the src for each carries its own
// `VT0 = _ZTV7daBar_c` note. It is a moving-bar actor: a 216-byte object with a
// MovingCylinderClsn at +0xd4, no PMF dispatch anywhere. Whomp's Fortress
// names one; the same class serves it and the level's other bar ids.
//
// The vtable is HOST STORAGE the registry fills (the InvisibleSecret case),
// aliased to _ZTV7daBar_c so the two destructor bodies that store VT0 back into
// the object write the same host array. Slot 16 is D1 (05d0: MovingCylinderClsn
// dtor + Actor::D2, no free) and slot 17 is D0 (0600: the same plus Deallocate)
// -- both live, unlike the pickups whose D0 is never reached.
extern "C" {
int func_ov002_020b067c(char *self);   /* slot 0  InitResources */
int func_ov002_020b0644(void);         /* slot 3  CleanupResources */
int func_ov002_020b0658(char *self);   /* slot 6  Behavior */
int func_ov002_020b0650(void);         /* slot 9  Render */
void func_ov002_020b064c(void);        /* slot 12 OnPendingDestroy */
int *func_ov002_020b05d0(int *self);   /* slot 16 D1 */
int *func_ov002_020b0600(int *self);   /* slot 17 D0 */
DSSTATE_BEGIN
void *data_ov002_02108480[31];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV7daBar_c=_data_ov002_02108480")
static int __fastcall ip_init(void *s, void *)
{ return func_ov002_020b067c((char *)s); }
static int __fastcall ip_clean(void *s, void *)
{ return func_ov002_020b0644(); }
static int __fastcall ip_behavior(void *s, void *)
{ return func_ov002_020b0658((char *)s); }
static int __fastcall ip_render(void *s, void *)
{ return func_ov002_020b0650(); }
static int __fastcall ip_pdes(void *s, void *)
{ func_ov002_020b064c(); return 0; }
static int __fastcall ip_d1(void *s, void *)
{ return (int)(size_t)func_ov002_020b05d0((int *)s); }
static int __fastcall ip_d0(void *s, void *)
{ return (int)(size_t)func_ov002_020b0600((int *)s); }
extern "C" void hal_fill_invisible_pole_vtable(void)
{
    void **vt = data_ov002_02108480;
    bw_fill_shared(vt);
    vt[0] = (void *)ip_init;
    vt[3] = (void *)ip_clean;
    vt[6] = (void *)ip_behavior;
    vt[9] = (void *)ip_render;
    vt[12] = (void *)ip_pdes;
    vt[16] = (void *)ip_d1;
    vt[17] = (void *)ip_d0;
}

// ---- ARROW_SIGN_LEFT (299) and ARROW_SIGN_RIGHT (300), ov098 ----------------
//
// _ZTV14ArrowSignRight, ov098 0x0213c3d8, RTTI 15daObjYajirusi_c (yajirushi:
// arrow). Two ids, one class, one table -- the coins' arrangement again, and
// the class reads its own id back to pick which way the arrow points. Bob-omb
// Battlefield names two lefts and three rights on the mountain path.
//
// THE TABLE IS ALREADY REAL STORAGE and already carries this class's methods:
// hal/actor_vtables.cpp defines _ZTV14ArrowSignRight as an initialised
// twenty-slot array, because ArrowSignRight is the class gate 9 proved the
// whole actor lifecycle with. What it does NOT carry is a registry row, and
// its slots 13/14/16/17 abort by name from that gate's own trap set. Filling
// it again at registration time replaces those with this gate's, so the two
// ids get the same treatment as everything else on the roster; the gate-9
// smoke keeps its own copy of the array and is untouched.
#include "ArrowSignRight.h"
extern "C" {
int _ZN14ArrowSignRight13InitResourcesEv(char *self);
int *_ZN14ArrowSignRightD1Ev(int *self);
extern void *_ZTV14ArrowSignRight[32];
}
static int __fastcall as_init(void *s, void *)
{ return _ZN14ArrowSignRight13InitResourcesEv((char *)s); }
static int __fastcall as_clean(void *s, void *)
{ return ((ArrowSignRight *)s)->ArrowSignRight::CleanupResources(); }
static int __fastcall as_behavior(void *s, void *)
{ return ((ArrowSignRight *)s)->ArrowSignRight::Behavior(); }
static int __fastcall as_render(void *s, void *)
{
    port_actor_render_probe("ARROW_SIGN", (char *)s + 0xd4);
    return ((ArrowSignRight *)s)->ArrowSignRight::Render();
}
static int __fastcall as_d1(void *s, void *)
{ return (int)(size_t)_ZN14ArrowSignRightD1Ev((int *)s); }
/* Slot 27, OnHitByMegaChar(Player &). The sign has its OWN body (ov098
   0x02137d80, ArrowSignRight_OnHitByMegaChar), not Actor's do-nothing. Both of
   the slot's linked dispatch sites are thiscall and PUSH the mega char: the two
   sites are func_ov002_020eed24 (walk_window.exe 0x45345e: `mov ecx,this /
   push [ebp+0xc] / call [edx+0x6c]`, then `mov eax,1 / pop / ret`, NO add esp)
   and func_ov102_02149ccc (0x485fd3: `mov ecx,esi / push edi / call [eax+0x6c]
   / pop edi / pop esi`, again no caller cleanup). The callee pops the word, so
   a three-parameter __fastcall veneer reads `this` from ecx, names the pushed
   char to force `ret 4`, and forwards -- the same shape xs_pounded/ssb_pounded
   ship. The body's closure (Player::IncMegaKillCount, func_02012694,
   Platform::KillByMegaChar) is all linked and dispatches no further vtable slot,
   so the chain terminates here. */
extern "C" void func_ov098_02137d80(void *self, void *player);  /* slice_gate33 */
static int __fastcall as_mega(void *s, void *, void *player)
{ func_ov098_02137d80(s, player); return 0; }
/* Slot 31, Kill(). The sign has its OWN Kill (ov098 0x02137ccc,
   ArrowSignRight_Kill -- a poof-dust + Sound::PlayBank3 + MarkForDestruction
   body), not the generic Platform::Kill. Every linked slot-31 dispatch site is
   thiscall with NO stack argument: the object's vptr is loaded, `this` is put in
   ecx, and `call [reg+0x7c]` runs with nothing pushed and no caller cleanup
   (e.g. walk_window.exe 0x4cc1db: `mov ecx,esi / mov eax,[esi] / call
   [eax+0x7c]`). Platform::Kill takes only `this` and returns void, so a
   two-parameter __fastcall veneer (this in ecx, the dummy edx, `ret 0`) matches.
   The body closes over Particle::System::NewSimple, Actor::DisappearPoofDustAt,
   Sound::PlayBank3 and ActorBase::MarkForDestruction -- all linked, none a
   vtable dispatch, so the chain terminates here. */
extern "C" int func_ov098_02137ccc(char *self);  /* slice_gate33 */
static int __fastcall as_kill(void *s, void *)
{ return func_ov098_02137ccc((char *)s); }
/* Slot 22, OnAttacked1(Actor &). The sign has its OWN body (ov098 0x02137d40,
   ArrowSignRight_OnAttacked1). Its one linked dispatcher (func_ov002_020ef228,
   walk_window.exe 0x481474) is thiscall and PUSHES the attacker, so a
   three-parameter __fastcall veneer reads `this` from ecx, names the pushed
   argument to force `ret 4`, and forwards -- the bw_atk1 shape. The body's only
   closure is a tail-dispatch of its OWN slot 31 (Kill); the raw src models that
   inner call as a cdecl function-pointer idiom, which mismatches slot 31's
   thiscall veneer. The HOST COPY unmatched/ArrowSign_OnAttacked1.cpp remodels
   the inner dispatch as a C++ thiscall virtual so it lands on as_kill correctly,
   the Player_HeadBonk treatment. With that the chain closes on the already
   linked slot-31 body. */
extern "C" void func_ov098_02137d40(void *self, char *o);  /* HOST COPY, slice_gate33 */
static int __fastcall as_atk1(void *s, void *, void *o)
{ func_ov098_02137d40(s, (char *)o); return 0; }
extern "C" void hal_fill_arrow_sign_vtable(void)
{
    void **vt = _ZTV14ArrowSignRight;
    hal_fill_platform_vtable();
    bw_fill_shared(vt);
    vt[0] = (void *)as_init;
    vt[3] = (void *)as_clean;
    vt[6] = (void *)as_behavior;
    vt[9] = (void *)as_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)as_d1;
    vt[17] = (void *)bw_trap17;
    /* 32 slots (ov098 0x0213c3d8; the next-symbol bound reads 23 and is
       wrong). Three of the tail are the sign's own bodies, matched in src.
       Slot 27 (OnHitByMegaChar, 0x02137d80) and slot 31 (Kill, 0x02137ccc) are
       now enrolled in slice_gate33 and seated through the veneers above: slot
       27's two linked dispatchers are thiscall (ret 4), slot 31's are thiscall
       with no arg (ret 0), each read from the linked binary, and neither body
       dispatches a further vtable slot so both chains are closed.

       SLOT 22 (OnAttacked1, 0x02137d40) is NOW SEATED. Its one linked
       dispatcher (func_ov002_020ef228, walk_window.exe 0x481474) is thiscall,
       so the ret-4 as_atk1 veneer balances the OUTER call. The recovered body
       tail-dispatches slot 31 through a plain function-pointer idiom
       (`c->vt->f[0x7c/4](c)`, a `int(*)(void*)`), which MSVC compiles as a
       CDECL indirect call: it PUSHES c, loads the vtable (not `this` in ecx),
       `call [reg+0x7c]`, then cleans the pushed word. That did not match slot
       31's thiscall veneer (as_kill, ret 0), a false-fix one call deep. The
       HOST COPY unmatched/ArrowSign_OnAttacked1.cpp models the inner dispatch
       as a real C++ thiscall virtual so MSVC emits `mov ecx,reg / call
       [reg+0x7c]` with nothing pushed -- byte for byte what slot 31's linked
       dispatchers emit -- and the chain closes on the already linked slot-31
       body. The Player_HeadBonk treatment. */
    vt[22] = (void *)as_atk1;
    vt[27] = (void *)as_mega;
    vt[31] = (void *)as_kill;
}

// ---- WATER_BOMB (208, ov098) x2 -- REGISTERED (gate 55) --------------------
//
// _ZTV9WaterBomb, ov098 0x0213c770, RTTI 7daWbm_c. The bomb a cannon fires.
// Gate 19 mounted the overlay and named this class in its header without
// registering it; Bob-omb Battlefield names two, and each fires more at the
// player from its state-0 body, so the census count oscillates as bombs land
// and MarkForDestruction retires them.
//
// THE GATE-51 BLOCKER NOTE WAS WRONG about the mechanism. It was NOT a
// cannon-to-cannon +0x348 chain -- func_ov098_0213a36c and func_ov098_0213a00c
// (the +0x348 scan/walk) are not even in the link, and the cannon's own driver
// (Cannon::Behavior -> func_ov098_0213a984, gate 19) does not touch that chain
// on this level. The real cause was two ordinary gaps on the BOMB side, found
// by registering it and reading the faults:
//
//   1. WaterBomb::Behavior dispatches its three states through
//      data_ov098_0213c930[unk_3c4], seeded by __sinit_ov098_0213c2b4 from the
//      SOURCE statics data_ov098_0213c724/72c/71c. Those .fn words are DS code
//      addresses (0213b9d8 / 0213bb1c / 0213b7e8), the ovdata contract, so the
//      first bomb to reach its Behavior faulted jumping to 0x0213b9d8. Seated
//      the same way as CAP and the iron ball, over the source statics before
//      the sinit copies them -- port/unmatched/WaterBomb_StateDispatch.cpp.
//   2. The three state bodies and the helper func_ov098_0213b520 were in no
//      slice at all. Added to slice_gate33.txt. State 0
//      (func_ov098_0213b9d8) is a host copy -- its matched src is int-returning
//      with mwccarm bare `return;`, which MSVC's C++ front end rejects (C2561),
//      and its (void) declaration of Actor::ClosestPlayer dropped the implicit
//      `this` that the host cdecl definition reads off the stack; the copy
//      passes self so ClosestPlayer scans from the real actor rather than a
//      garbage `this`. port/unmatched/WaterBomb_State0.cpp.
//
// Both are the routine seat/closure shape the other three of the four already
// took; there was no cannon-side infrastructure to build.
#include "WaterBomb.h"
extern "C" {
int *_ZN9WaterBombD1Ev(int *self);
void *_ZTV9WaterBomb[31];
}
static int __fastcall wb_init(void *s, void *)
{ return ((WaterBomb *)s)->WaterBomb::InitResources(); }
static int __fastcall wb_clean(void *s, void *)
{ return ((WaterBomb *)s)->WaterBomb::CleanupResources(); }
static int __fastcall wb_behavior(void *s, void *)
{ return ((WaterBomb *)s)->WaterBomb::Behavior(); }
static int __fastcall wb_render(void *s, void *)
{
    port_actor_render_probe("WATER_BOMB", (char *)s + 0x300);
    return ((WaterBomb *)s)->WaterBomb::Render();
}
static int __fastcall wb_d1(void *s, void *)
{ return (int)(size_t)_ZN9WaterBombD1Ev((int *)s); }
extern "C" void hal_fill_water_bomb_vtable(void)
{
    void **vt = _ZTV9WaterBomb;
    bw_fill_shared(vt);
    vt[0] = (void *)wb_init;
    vt[3] = (void *)wb_clean;
    vt[6] = (void *)wb_behavior;
    vt[9] = (void *)wb_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)wb_d1;
    vt[17] = (void *)bw_trap17;
}

// ---- ROLLING_IRON_BALL (220, ov100) x4 -------------------------------------
//
// _ZTV15RollingIronBall, ov100 0x02147f7c, RTTI 7daIbl_c (iron ball). The
// balls that roll down the mountain, and one of the two classes on this
// roster that MOVES on its own. Four on Bob-omb Battlefield: two on the upper
// slope at (423,2620,-5428) and (1477,3745,-5526), two on the lower at
// (-99,800,-3345) and (-1313,790,-3673). Gate 21 mounted ov100 and its header
// already lists the iron ball as a class other levels name.
#include "RollingIronBall.h"
extern "C" {
int *_ZN15RollingIronBallD1Ev(int *self);
void *_ZTV15RollingIronBall[31];
}
static int __fastcall rib_init(void *s, void *)
{ return ((RollingIronBall *)s)->RollingIronBall::InitResources(); }
static int __fastcall rib_clean(void *s, void *)
{ return ((RollingIronBall *)s)->RollingIronBall::CleanupResources(); }
static int __fastcall rib_behavior(void *s, void *)
{ return ((RollingIronBall *)s)->RollingIronBall::Behavior(); }
static int __fastcall rib_render(void *s, void *)
{
    port_actor_render_probe("ROLLING_IRON_BALL", (char *)s + 0x2cc);
    return ((RollingIronBall *)s)->RollingIronBall::Render();
}
static int __fastcall rib_d1(void *s, void *)
{ return (int)(size_t)_ZN15RollingIronBallD1Ev((int *)s); }
extern "C" void hal_fill_rolling_iron_ball_vtable(void)
{
    void **vt = _ZTV15RollingIronBall;
    bw_fill_shared(vt);
    vt[0] = (void *)rib_init;
    vt[3] = (void *)rib_clean;
    vt[6] = (void *)rib_behavior;
    vt[9] = (void *)rib_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)rib_d1;
    vt[17] = (void *)bw_trap17;
    /* 31 slots, a plain Actor table. Slot 29 (OnAimedAtWithEgg) is the ball's
       own body (ov100 0x02141fa8), matched in src and in no slice. */
    vt[29] = (void *)bw_trap29;
}

// ---- WARP_PIPE (298, ov102) ------------------------------------------------
//
// _ZTV13FortressTower, ov102 0x0214e1d8, RTTI 15daObjSimpleBg_c -- a simple
// background object with a collider, which is what a warp pipe is until
// somebody stands on it. slice_gate23.txt already records that this table's
// config name is the shifted one; the SpawnInfo at ov102 0x0214e134 is
// WarpPipe's and the actor id is 298.
//
// BOB-OMB BATTLEFIELD DOES NOT NAME IT. The level's object tables have no
// warp pipe -- SM64DS puts none there -- so this row is on the roster because
// the task asks for warp pipes and ov102 is already mounted, not because the
// level needs it. One row, and it is the class every level with a pipe uses.
//
// THE ENTRY AND EXIT TRANSITION IS NOT HERE. What the pipe does when Mario
// stands on it is the Player's own warp state plus a fader wipe and a
// level/entrance change, and the port has no level change: hal/level_boot.cpp
// mounts ov009 by name. That is port-beta-lvl's seam, not this gate's.
#include "FortressTower.h"
extern "C" {
int *_ZN13FortressTowerD1Ev(int *self);
void *_ZTV13FortressTower[32];
}
#pragma comment(linker, "/alternatename:__ZTV15daObjSimpleBg_c=__ZTV13FortressTower")
static int __fastcall wp_init(void *s, void *)
{ return ((FortressTower *)s)->FortressTower::InitResources(); }
static int __fastcall wp_clean(void *s, void *)
{ return ((FortressTower *)s)->FortressTower::CleanupResources(); }
static int __fastcall wp_behavior(void *s, void *)
{ return ((FortressTower *)s)->FortressTower::Behavior(); }
static int __fastcall wp_render(void *s, void *)
{
    port_actor_render_probe("WARP_PIPE", (char *)s + 0xd4);
    return ((FortressTower *)s)->FortressTower::Render();
}
static int __fastcall wp_d1(void *s, void *)
{ return (int)(size_t)_ZN13FortressTowerD1Ev((int *)s); }
extern "C" void hal_fill_warp_pipe_vtable(void)
{
    void **vt = _ZTV13FortressTower;
    hal_fill_platform_vtable();
    bw_fill_shared(vt);
    vt[0] = (void *)wp_init;
    vt[3] = (void *)wp_clean;
    vt[6] = (void *)wp_behavior;
    vt[9] = (void *)wp_render;
    vt[12] = (void *)bw_pdes_base;
    vt[16] = (void *)wp_d1;
    vt[17] = (void *)bw_trap17;
    /* 32 slots -- this is also STATIC_ROCK's table (id 61, Jolly Roger Bay
       x33), the confirmed level-8 crash. Slot 31 is Platform::Kill. */
    vt[31] = (void *)bw_kill;
}

// ---- WARP (347, ov002) x4 --------------------------------------------------
//
// The vtable at ov002 0x0210acbc, RTTI 11daWarpkun_c -- warp-kun, the warp
// POINT. This is what Bob-omb Battlefield actually uses where the task asked
// for warp pipes: SM64DS puts no pipe on this level, it puts four warps, and
// the level's own tables name id 347 four times.
//
// Its factory spells the table by the RTTI name and dsd never gave it one, the
// InvisibleSecret case again -- Warp_Spawn is ov002 0x020ec534 and its one
// literal (relocs.txt, from 0x020ec568) is 0x0210acbc. Every one of its seven
// slots is a plain C-named func_ov002_020ec* in its own TU, so this class needs
// no header, no method face and no shadow declaration: it is the cheapest
// class on the roster.
//
// 264-byte object with one MovingCylinderClsn at +0xd4, which is the trigger
// volume. What it does when the player enters it is a position change inside
// the same level, which is why it works here and the PIPE's level change does
// not.
extern "C" {
int func_ov002_020ec4c4(char *self);     /* InitResources */
int func_ov002_020ec3fc(char *self);     /* CleanupResources */
int func_ov002_020ec410(char *self);     /* Behavior */
int func_ov002_020ec408(char *self);     /* Render */
void func_ov002_020ec404(char *self);    /* OnPendingDestroy */
int *func_ov002_020ec388(int *self);     /* D1 */
DSSTATE_BEGIN
void *data_ov002_0210acbc[31];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV11daWarpkun_c=_data_ov002_0210acbc")
static int __fastcall warp_init(void *s, void *)
{ return func_ov002_020ec4c4((char *)s); }
static int __fastcall warp_clean(void *s, void *)
{ return func_ov002_020ec3fc((char *)s); }
static int __fastcall warp_behavior(void *s, void *)
{ return func_ov002_020ec410((char *)s); }
static int __fastcall warp_render(void *s, void *)
{ return func_ov002_020ec408((char *)s); }
static int __fastcall warp_pdes(void *s, void *)
{ func_ov002_020ec404((char *)s); return 0; }
static int __fastcall warp_d1(void *s, void *)
{ return (int)(size_t)func_ov002_020ec388((int *)s); }
extern "C" void hal_fill_warp_vtable(void)
{
    void **vt = data_ov002_0210acbc;
    bw_fill_shared(vt);
    vt[0] = (void *)warp_init;
    vt[3] = (void *)warp_clean;
    vt[6] = (void *)warp_behavior;
    vt[9] = (void *)warp_render;
    vt[12] = (void *)warp_pdes;
    vt[16] = (void *)warp_d1;
    vt[17] = (void *)bw_trap17;
}

// ---- THE WATCH (port mod) --------------------------------------------------
//
// THE SPAWN HOOK IS NOT HERE ANY MORE. This gate carried a minimal one while
// port-beta-lvl was still in flight; that branch is merged and its
// port_debug_spawn / port_debug_spawn_at / port_debug_spawn_env in
// hal/level_boot.cpp are the real thing, wired to the F5 menu and to the level
// parameter. SM64DS_SPAWN_ACTOR is theirs now and this file only reads it to
// know which ids to follow.
//
// What is left is the READ-BACK, and it is what proves BEHAVIOUR rather than
// survival: every sixty frames, how many of each watched id are still on the
// behaviour list, where the first of them is, and what the coin counter says.
// A pickup shows up as the count going down and data_0209f358 going up in the
// same report -- which is the whole of a coin working, and it also means the
// D1 in slot 16 ran and AfterCleanupResources deallocated behind it.
extern "C" {
extern int data_020a4b78[];       /* the behaviour processing list */
extern short data_0209f358[];     /* the coin counter GiveCoins increments */
}

static unsigned short g_watch[16];
static int g_watch_n;

/* Pick the ids up from the same variable the shared hook parses, once. */
static void port_bob_watch_arm(void)
{
    static int done;
    const char *p;
    if (done) return;
    done = 1;
    p = std::getenv("SM64DS_SPAWN_ACTOR");
    if (!p) return;
    while (*p && g_watch_n < (int)(sizeof g_watch / sizeof g_watch[0])) {
        char *end;
        long id = std::strtol(p, &end, 0);
        if (end == p) break;
        p = end;
        if (*p == ':') std::strtol(p + 1, &end, 0), p = end;
        /* the @<area> suffix (hal/level_boot.cpp's spelling), skipped so the
           watch keeps parsing a multi-entry list past it */
        if (*p == '@') std::strtol(p + 1, &end, 0), p = end;
        while (*p == ',' || *p == ' ') ++p;
        g_watch[g_watch_n++] = (unsigned short)id;
    }
}

extern "C" void port_bob_debug_watch(void)
{
    static int frame;
    int i;
    port_bob_watch_arm();
    if (!g_watch_n || (frame++ % 60))
        return;
    std::printf("[watch] frame %d coins %d:", frame - 1, data_0209f358[0]);
    for (i = 0; i < g_watch_n; ++i) {
        int n = 0;
        const int *first = 0;
        for (int *node = (int *)(size_t)data_020a4b78[0]; node && n < 4096;
             node = (int *)(size_t)node[1]) {
            char *o = (char *)(size_t)node[2];
            if (!o || *(unsigned short *)(o + 0xc) != g_watch[i])
                continue;
            if (!n++) first = (const int *)(o + 0x5c);
        }
        std::printf("  %u x%d", g_watch[i], n);
        if (first)
            std::printf(" @(%d,%d,%d)", first[0] >> 12, first[1] >> 12,
                        first[2] >> 12);
    }
    std::printf("\n");
}
